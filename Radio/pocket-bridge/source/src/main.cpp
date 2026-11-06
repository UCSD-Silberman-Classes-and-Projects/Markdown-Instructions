#include <Arduino.h>
#include <Joystick.h>
#include "utils/CrsfReader.h"
#include "utils/SBusTracker.h"
// #include <Streaming.h>

#define MIN_SIGNAL 190
#define MAX_SIGNAL 1790

#define ACTIVE_SIGNAL 1792
#define REALESED_SIGNAL 192
#define MAJORITY_THREASH ( (HISTORY_SIZE / 2 + 1) * ACTIVE_SIGNAL) / HISTORY_SIZE
// A switch counts as on above this. Well clear of centre: an unassigned channel sits at
// ~997, just over MAJORITY_THREASH (995), and used to read as pressed.
#define SWITCH_ON_SIGNAL 1400
#define CENTER_SIGNAL ((MIN_SIGNAL + MAX_SIGNAL) / 2)

// The receiver speaks CRSF on Serial1: 115200 8N1 (set RCVR_UART_BAUD to match).
#define CRSF_BAUD 115200
// No valid RC frame for this long = link lost. ELRS stops sending RC frames on
// failsafe, so without this the last stick values would stay in force.
#define LINK_TIMEOUT_MS 250
// Fail-safe relay: HIGH (energised) only while armed with the link up, so loss of
// power, USB, firmware or link all leave it at rest = kill. Wire NO to the VESC ADC2
// high side and NC to GND, with Kill Switch Mode ADC2 Low. This is the reverse of the
// course wiring, where a dead Pro Micro left the motor enabled.
#define RUN_PIN 8

#define XAXIS_CHANNEL 3
#define YAXIS_CHANNEL 2
#define RX_CHANNEL 0
#define RY_CHANNEL 1
#define RUBBER_CHANNEL 5
#define THROTTEL_CHANNEL 6
#define LBUTTON_CHANNEL 4   // CH5, Pocket SA: arm. ExpressLRS's arm channel too
#define RBUTTON_CHANNEL 7   // CH8, Pocket SD
#define BUTTON3_CHANNEL 8   // CH9, Pocket SE pad (momentary)
#define ARM_CHANNEL LBUTTON_CHANNEL

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_MULTI_AXIS,
  3, 0,                  // Button Count, Hat Switch Count
  true, true, false,     // X and Y, but no Z Axis
  true, true, false,   // No Rx, Ry, or Rz
  true, true,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

CrsfReader crsf;
unsigned long last_frame_ms = 0;
bool link_up = false;

// Once a second, a status line on the USB serial port (ttyACM on the Pi). Writes are
// dropped while no one has the port open, so this costs nothing in normal use.
#define STATUS_INTERVAL_MS 1000
unsigned long last_status_ms = 0;
uint32_t status_loops = 0;
uint16_t status_bytes = 0, status_frames = 0;

SBusTracker xAxisTracker;
SBusTracker yAxisTracker;
SBusTracker rxTracker;
SBusTracker ryTracker;
SBusTracker rubberTracker;
SBusTracker throttleTracker;
SBusTracker lButTracker;
SBusTracker rButTracker;
SBusTracker button3Tracker;

// A HID report waits up to 250 ms (the USB core's send timeout) when the Pi isn't
// reading the joystick: nothing has it open, or the Pi has crashed. Measured on Yellow:
// the loop fell to 4 passes a second and the 64-byte serial buffer overflowed. So after
// a send that blocked, skip sends for a second; the next one carries the latest state.
#define HID_BLOCKED_MS 100
#define HID_BACKOFF_MS 1000
unsigned long hid_backoff_until = 0;

void send_joystick() {
  if ((long)(millis() - hid_backoff_until) < 0) {
    return;
  }
  unsigned long start = millis();
  Joystick.sendState();
  if (millis() - start > HID_BLOCKED_MS) {
    hid_backoff_until = millis() + HID_BACKOFF_MS;
  }
}

// Link lost (or not up yet): centre every axis, release both buttons, engage the kill
// relay, and restart the averages from neutral so recovery doesn't replay stale input.
void failsafe() {
  xAxisTracker = SBusTracker(CENTER_SIGNAL);
  yAxisTracker = SBusTracker(CENTER_SIGNAL);
  rxTracker = SBusTracker(CENTER_SIGNAL);
  ryTracker = SBusTracker(CENTER_SIGNAL);
  rubberTracker = SBusTracker(CENTER_SIGNAL);
  throttleTracker = SBusTracker(CENTER_SIGNAL);
  lButTracker = SBusTracker(REALESED_SIGNAL);
  rButTracker = SBusTracker(REALESED_SIGNAL);
  button3Tracker = SBusTracker(REALESED_SIGNAL);
  Joystick.setXAxis(CENTER_SIGNAL);
  Joystick.setYAxis(CENTER_SIGNAL);
  Joystick.setRxAxis(CENTER_SIGNAL);
  Joystick.setRyAxis(CENTER_SIGNAL);
  Joystick.setRudder(CENTER_SIGNAL);
  Joystick.setThrottle(CENTER_SIGNAL);
  Joystick.setButton(0, 0);
  Joystick.setButton(1, 0);
  Joystick.setButton(2, 0);
  digitalWrite(RUN_PIN, LOW);  // kill, before the report, which may block
  send_joystick();
}

void setup() {

  //Set pinout
  digitalWrite(RUN_PIN, LOW);
  pinMode(RUN_PIN, OUTPUT);
  // Configure JoyStick
  Joystick.setXAxisRange(MIN_SIGNAL, MAX_SIGNAL);
  Joystick.setYAxisRange(MIN_SIGNAL, MAX_SIGNAL);
  Joystick.setRxAxisRange(MIN_SIGNAL, MAX_SIGNAL);
  Joystick.setRyAxisRange(MIN_SIGNAL, MAX_SIGNAL);
  Joystick.setRudderRange(MIN_SIGNAL, MAX_SIGNAL);
  Joystick.setThrottleRange(MIN_SIGNAL, MAX_SIGNAL);
  
  // Begin!!! One HID report per frame (sendState) instead of one per axis.
  Joystick.begin(false);
  Serial1.begin(CRSF_BAUD);
  Serial.begin(115200);  // USB CDC; the baud is ignored
  failsafe();  // no link yet: neutral outputs, kill engaged
}

void update_trackers(const uint16_t *channels) {
  xAxisTracker.add(channels[XAXIS_CHANNEL]);
  yAxisTracker.add(channels[YAXIS_CHANNEL]);
  rxTracker.add(channels[RX_CHANNEL]);
  ryTracker.add(channels[RY_CHANNEL]);
  rubberTracker.add(channels[RUBBER_CHANNEL]);
  throttleTracker.add(channels[THROTTEL_CHANNEL]);
  lButTracker.add(channels[LBUTTON_CHANNEL]);
  rButTracker.add(channels[RBUTTON_CHANNEL]);
  button3Tracker.add(channels[BUTTON3_CHANNEL]);
}

void loop() {
  bool frame = false;
  while (Serial1.available() > 0) {
    status_bytes++;
    if (crsf.feed(Serial1.read(), micros())) {
      frame = true;  // keep draining; only the newest frame matters
      status_frames++;
    }
  }
  status_loops++;

  if (frame) {
    last_frame_ms = millis();
    link_up = true;
    update_trackers(crsf.channels);

    Joystick.setXAxis(xAxisTracker.get_estimated());
    Joystick.setYAxis(yAxisTracker.get_estimated());
    Joystick.setRxAxis(rxTracker.get_estimated());
    Joystick.setRyAxis(ryTracker.get_estimated());
    Joystick.setRudder(rubberTracker.get_estimated());
    Joystick.setThrottle(throttleTracker.get_estimated());
    // setButton() takes a uint8_t and treats non-zero as pressed. Passing
    // get_button_state() (1792 / 192) truncated to 0x00 / 0xC0 and inverted both buttons.
    bool armed = lButTracker.get_estimated() > SWITCH_ON_SIGNAL;
    Joystick.setButton(0, armed ? 1 : 0);
    Joystick.setButton(1, rButTracker.get_estimated() > SWITCH_ON_SIGNAL ? 1 : 0);
    Joystick.setButton(2, button3Tracker.get_estimated() > SWITCH_ON_SIGNAL ? 1 : 0);

    // The relay first: it must not wait on the USB report.
    digitalWrite(RUN_PIN, armed ? HIGH : LOW);
    send_joystick();
  }
  else if (link_up && millis() - last_frame_ms > LINK_TIMEOUT_MS) {
    link_up = false;
    failsafe();
  }

  if (millis() - last_status_ms >= STATUS_INTERVAL_MS) {
    last_status_ms = millis();
    // e.g. "t=12000 loops=8123 bytes=5400 frames=150 crc=0 link=1 ch=992,992,172,992,191,997,997,191,191,997,997,997" (CH1-12)
    Serial.print(F("t="));      Serial.print(last_status_ms);
    Serial.print(F(" loops=")); Serial.print(status_loops);
    Serial.print(F(" bytes=")); Serial.print(status_bytes);
    Serial.print(F(" frames="));Serial.print(status_frames);
    Serial.print(F(" crc="));   Serial.print(crsf.crc_errors);
    Serial.print(F(" link="));  Serial.print(link_up ? 1 : 0);
    Serial.print(F(" ch="));
    for (uint8_t ch = 0; ch < 12; ch++) {
      if (ch) Serial.print(',');
      Serial.print(crsf.channels[ch]);
    }
    Serial.println();
    status_loops = 0;
    status_bytes = status_frames = 0;
  }
}
