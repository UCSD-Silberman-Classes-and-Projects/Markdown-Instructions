#!/usr/bin/env bash
# Flash a .hex onto an ATmega32U4 Pro Micro / Leonardo over USB, then verify it.
# MOVE TIER: the person runs this. Works on the Pi (plain avrdude) or on the Mac.
#
# Usage: flash_promicro.sh <file.hex> [serial port of the running board]
#   Pi:  bash flash_promicro.sh firmware.hex
#   Mac: tools/flash_promicro.sh firmware/promicro/elrscontroller/firmware.hex
#
# With no port given it finds the board itself. A board that is already running a
# sketch is reset into its bootloader with a 1200-baud touch; a blank board has no
# serial port, so the script asks for a hand reset (RST to GND twice) and waits.
set -u
shopt -s nullglob
HEX=${1:-}
APP_PORT=${2:-}
[[ -n "$HEX" && -f "$HEX" ]] || { echo "usage: $0 <file.hex> [port]"; exit 1; }

if [[ "$(uname -s)" == Darwin ]]; then
  AVRDUDE=(avrdude)
  command -v avrdude >/dev/null || AVRDUDE=(~/.platformio/packages/tool-avrdude/avrdude -C ~/.platformio/packages/tool-avrdude/avrdude.conf)
  STTY_DEV=-f
  PORTS='/dev/cu.usbmodem*'
else
  AVRDUDE=(avrdude)
  command -v avrdude >/dev/null || { echo "avrdude is not installed: sudo apt-get install avrdude"; exit 1; }
  STTY_DEV=-F
  # Arduino-family ports by stable name only: on the car the VESC is a ttyACM device too.
  PORTS='/dev/serial/by-id/*Arduino* /dev/serial/by-id/*SparkFun* /dev/serial/by-id/*Pro_Micro*'
fi

ports() { local p; for p in $PORTS; do echo "$p"; done; }

if [[ -z "$APP_PORT" ]]; then
  found=( $(ports) )
  if (( ${#found[@]} == 1 )); then
    APP_PORT=${found[0]}
  elif (( ${#found[@]} > 1 )); then
    echo "more than one board-like port; pass the one to flash as the second argument:"
    printf '  %s\n' "${found[@]}"
    exit 1
  fi
fi

# Wait up to $1 s for the bootloader's port: one that isn't the running board's, or
# the same name coming back after the running board's port has gone away.
wait_for_bootloader() {
  local deadline=$((SECONDS + $1)) gone=0 p
  while (( SECONDS < deadline )); do
    [[ -n "$APP_PORT" && ! -e "$APP_PORT" ]] && gone=1
    for p in $(ports); do
      if [[ "$p" != "$APP_PORT" || $gone == 1 ]]; then echo "$p"; return 0; fi
    done
    sleep 0.2
  done
  return 1
}

if [[ -n "$APP_PORT" ]]; then
  echo "resetting $APP_PORT into the bootloader"
  stty $STTY_DEV "$APP_PORT" 1200 || { echo "could not open $APP_PORT (is something else using it?)"; exit 1; }
  BL=$(wait_for_bootloader 12) || { echo "the bootloader port did not appear; try again, or reset the board by hand (RST to GND twice)"; exit 1; }
else
  echo "no running board found. Put it into the bootloader by hand: connect RST to GND twice within a second."
  BL=$(wait_for_bootloader 60) || { echo "no bootloader port appeared in 60 s"; exit 1; }
fi
echo "bootloader port: $BL"
sleep 0.5

# -U flash:w writes, then reads back and verifies; avrdude exits non-zero on a mismatch.
"${AVRDUDE[@]}" -p atmega32u4 -c avr109 -P "$BL" -b 57600 -U flash:w:"$HEX":i
