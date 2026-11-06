# Pro Micro bridge files for the Radio Setup page

Everything `radiomaster_setup.html` tells students to download. Built and tested on the
lab spare car (Raspberry Pi 5, RadioMaster Pocket, RP4-TD receiver), running there since
2026-10-21.

| File | What it is | Page section |
|---|---|---|
| `firmware.hex` | The Pro Micro bridge firmware. sha256 `a61784dc9a421583293bbbd7d841b7b6e042c75b8b53679c286eef703758f58d` | 03 |
| `flash_promicro.sh` | Writes a `.hex` from the Pi (or a Mac) with avrdude and reads it back to verify | 03 |
| `my_joystick.py` | DonkeyCar 5.x custom controller for the Pocket (`CONTROLLER_TYPE = "custom"`) | 10 |
| `rc_probe.py` | Runs that controller alone, no motor and no camera, and prints what it would send | 10 |
| `source/` | The complete firmware source: `NikitaB04/ELRSController` at `a4c7877` with the patch applied, libraries included. A PlatformIO project, `board = leonardo` | |
| `0001-yellow-crsf-reader-failsafe-buttons.patch` | The same change as a readable diff against upstream | |
| `build.sh` | Runs the CRSF reader's host test, then rebuilds `firmware.hex` from `source/` with PlatformIO. The hex above was built with platform `atmelavr` 5.3.0; a newer toolchain may give a different hash | |

## What the firmware changes, relative to NikitaB04/ELRSController

- **CRSF frames are checksum-checked** (CRC8) and the reader resyncs on any error or gap.
  The stock parser accepts anything with `0x16` in the third byte, so line noise decodes as
  stick input and the axes jitter with the sticks still.
- **Link loss:** after 250 ms without a valid frame, all axes centre, buttons release and
  pin 8 goes LOW. The same state holds at power-up until the first good frame. Stock holds
  the last values forever.
- **Buttons:** 3 (CH5, CH8, CH9), on above 1400. Stock reports 2, inverted, with a
  threshold that reads an unassigned channel as pressed.
- **One USB report per frame** instead of one per axis, and the kill pin is set before the
  report so a blocked report cannot delay it.
- **A status line** once a second on the USB serial port (`frames= crc= link= ch=`), which
  the page uses for every check.
- **Axes unchanged:** X = CH4, Y = CH3, Rx = CH1, Ry = CH2, Rudder = CH6, Throttle = CH7,
  channels numbered as the radio shows them.

## Relay polarity: a deliberate deviation

The original design drives pin 8 HIGH to **kill** (relay NC = 5 V, NO = GND, VESC
Kill Switch Mode "ADC2 Low"), so a Pro Micro with no power, no USB or no firmware leaves
the motor **enabled**. This firmware drives pin 8 HIGH only while **armed (CH5 high) and
the link is up**, and Section 05 of the page wires the relay the other way round (COM to
ADC2, NO to the high side, NC to GND, active-high relay module).

**Do not run this firmware with a relay wired the original way.** The logic would be
inverted: motor enabled on link loss or when disarmed, killed when armed. The relay wiring
on the page comes from the design and bench tests; it has not yet been verified on a car.

## Before blaming the firmware

If the joystick appears on the Pi but nothing moves, the receiver is almost certainly
sending SBUS, or CRSF at its default 420000 baud. Every ELRSController build, this one
included, reads **CRSF at 115200**. Section 08 step 4 of the page sets it on the
receiver's web page (MODEL tab: Serial Protocol CRSF; OPTIONS tab: UART baud 115200; then
power-cycle the receiver).

## Flashing and rebuilding

`bash flash_promicro.sh firmware.hex` on the Pi, with `avrdude` installed (Section 03 shows
the expected output). It is a thin wrapper: a 1200-baud touch on the board's serial port to
reach the bootloader, then
`avrdude -p atmega32u4 -c avr109 -P <bootloader port> -b 57600 -U flash:w:firmware.hex:i`.
This copy of the script was tidied after the lab car's last flash and has not been re-run
since; if it misbehaves, those two steps by hand do the same job.

To change the firmware, edit `source/` and run `build.sh` (or open `source/` in PlatformIO
and click Upload, which builds and flashes in one go).
