"""
Yellow's RadioMaster Pocket as a DonkeyCar 5.4 controller (CONTROLLER_TYPE = "custom").

The Pocket reaches the Pi as a USB joystick through the Pro Micro bridge
(firmware/promicro/elrscontroller/). DonkeyCar's complete.py imports
MyJoystickController from this file in ~/mycar.

Controls, as the Pocket is set up on Yellow:
  right stick <->  CH1  steering (dead band STEERING_DEADBAND)
  left stick ^v    CH2  throttle amount: bottom = stopped, top = full. Holds its position
  SB rocker        CH6  direction: forward / neutral / reverse (DIRECTION_BY_SB)
  SA bumper        CH5  arm: light on = armed. Disarmed = throttle 0, user mode, not recording
  SC 3-position    CH7  drive mode (MODE_BY_SC)
  SD 2-position    CH8  record enable: records only while armed, moving forward, in user mode
Unused so far: right stick ^v (CH3), left stick <-> (CH4), SE pad (CH9).
Steering works while disarmed, so the servo can be checked on the stand.

Interlocks:
- Arming: SA must stay on for ARM_HOLD_S with the throttle stick at the bottom
  and SC on user, or arming is refused until SA is cycled off and on.
  DonkeyCar drops the joystick's start-up events, and the bridge reports SA
  before the axes in each USB report, so without this, arming at start-up or
  after a link drop could jump straight to a held throttle, or into autopilot.
- Direction: SB to neutral applies at once; to forward or reverse only with the
  throttle stick at the bottom. Until then the throttle is 0, so the motor is
  never flipped from forward to reverse under power.

Not a safety system on its own: the VESC kill relay on the Pro Micro is the
independent stop (firmware/promicro/elrscontroller/README.md).
"""
import logging
import struct
import time

from donkeycar.parts.controller import Joystick, JoystickController

logger = logging.getLogger(__name__)

# complete.py doesn't pass JOYSTICK_DEVICE_FILE to a "custom" controller.
DEVICE = "/dev/input/by-id/usb-Arduino_LLC_Arduino_Leonardo_HIDFF-if02-joystick"

ARM_HOLD_S = 0.3          # SA on this long, with the conditions above, before arming
STICK_BOTTOM = -0.90      # throttle stick at or below this counts as "at the bottom"
STEERING_DEADBAND = 0.05  # the right stick rests at about +0.03
# Switch position (axis -1 / 0 / +1) -> meaning. Which physical position gives -1
# is set by the Pocket; check on the stand and swap entries here if needed.
MODE_BY_SC = {1: 'user', 0: 'local_angle', -1: 'local'}   # user at the top, autopilot at the bottom
DIRECTION_BY_SB = {1: 1.0, 0: 0.0, -1: -1.0}   # forward, neutral, reverse
DIRECTION_NAMES = {1.0: 'forward', 0.0: 'neutral', -1.0: 'reverse'}


def position(axis_val):
    """A 3-position switch's axis value as -1, 0 or +1."""
    return -1 if axis_val < -0.5 else (1 if axis_val > 0.5 else 0)


class MyJoystick(Joystick):
    """The Pro Micro bridge, by Linux input code (jstest: X Y Rx Ry Throttle Rudder, Btn0-2)."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.axis_names = {
            0x00: 'left_stick_horz',   # CH4
            0x01: 'right_stick_vert',  # CH3
            0x03: 'right_stick_horz',  # CH1: steering
            0x04: 'left_stick_vert',   # CH2: throttle amount
            0x06: 'sc',                # CH7, sent as the HID Throttle axis
            0x07: 'sb',                # CH6, sent as the HID Rudder axis
        }
        self.button_names = {
            0x100: 'sa',  # CH5: arm
            0x101: 'sd',  # CH8: record
            0x102: 'se',  # CH9: pad
        }

    def poll(self):
        """
        As Joystick.poll(), except start-up events (type & 0x80) still update
        axis_states and button_states. The base class drops them, so a held
        stick or a switch left on would be unknown until it next moved.
        They still trigger nothing.
        """
        if self.jsdev is None:
            return None, None, None, None
        evbuf = self.jsdev.read(8)
        if not evbuf:
            return None, None, None, None
        _, value, typev, number = struct.unpack('IhBB', evbuf)
        startup = bool(typev & 0x80)
        if typev & 0x01:
            button = self.button_map[number]
            self.button_states[button] = value
            if startup:
                return None, None, None, None
            logger.info("button: %s state: %d" % (button, value))
            return button, value, None, None
        if typev & 0x02:
            axis = self.axis_map[number]
            fvalue = value / 32767.0
            self.axis_states[axis] = fvalue
            if startup:
                return None, None, None, None
            return None, None, axis, fvalue
        return None, None, None, None


class MyJoystickController(JoystickController):

    def __init__(self, *args, **kwargs):
        kwargs.setdefault('dev_fn', DEVICE)
        super().__init__(*args, **kwargs)
        self.armed = False
        self.arm_requested_at = None
        self.selected_mode = 'user'
        self.record_switch = False
        self.direction_switch = 0.0   # what SB asks for
        self.direction = 0.0          # what is engaged

    def init_js(self):
        try:
            self.js = MyJoystick(self.dev_fn)
            if not self.js.init():
                self.js = None
        except FileNotFoundError:
            logger.error(f"{self.dev_fn} not found.")
            self.js = None
        return self.js is not None

    def init_trigger_maps(self):
        # The throttle stick has no trigger: run_threaded() reads its position,
        # which MyJoystick.poll() keeps current from start-up on.
        self.axis_trigger_map = {
            'right_stick_horz': self.set_steering,
            'sb': self.set_direction_switch,
            'sc': self.set_mode_switch,
        }
        self.button_down_trigger_map = {
            'sa': self.request_arm,
            'sd': self.record_on,
        }
        self.button_up_trigger_map = {
            'sa': self.disarm_switch,
            'sd': self.record_off,
        }

    # --- sticks and switches -------------------------------------------------

    def set_steering(self, axis_val):
        if abs(axis_val) < STEERING_DEADBAND:
            axis_val = 0.0
        self.angle = self.steering_scale * axis_val

    def axis(self, name, default=0.0):
        return self.js.axis_states.get(name, default) if self.js else default

    def throttle_level(self):
        """Throttle stick as 0 (bottom) .. 1 (top), with the dead band at the bottom."""
        level = (self.throttle_dir * self.axis('left_stick_vert', -1.0) + 1.0) / 2.0
        return 0.0 if level <= self.dead_zone else level

    def set_direction_switch(self, axis_val):
        direction = DIRECTION_BY_SB[position(axis_val)]
        if direction != self.direction_switch:
            self.direction_switch = direction
            logger.info(f"SB: {DIRECTION_NAMES[direction]}")

    def set_mode_switch(self, axis_val):
        mode = MODE_BY_SC[position(axis_val)]
        if mode != self.selected_mode:
            self.selected_mode = mode
            logger.info(f"SC: mode {mode}" + ("" if self.armed else " (takes effect once armed)"))

    def record_on(self):
        self.record_switch = True
        logger.info("SD: recording enabled")

    def record_off(self):
        self.record_switch = False
        logger.info("SD: recording disabled")

    # --- arming ----------------------------------------------------------------

    def request_arm(self):
        if self.armed:
            return
        self.arm_requested_at = time.monotonic()
        logger.info(f"SA on: arming in {ARM_HOLD_S} s if the throttle stick is at the bottom and SC is on user")

    def disarm_switch(self):
        self.disarm("SA off")

    def disarm(self, reason):
        if self.armed or self.arm_requested_at is not None:
            logger.warning(f"Disarmed ({reason}): throttle 0, user mode")
        self.armed = False
        self.arm_requested_at = None

    def check_arm_request(self):
        if self.arm_requested_at is None:
            return
        stick = self.axis('left_stick_vert', 0.0)
        sc = self.axis('sc')
        if self.throttle_dir * stick > STICK_BOTTOM:
            logger.warning(f"Arm refused: throttle stick at {stick:+.2f}. Pull it to the bottom, then cycle SA off and on")
            self.arm_requested_at = None
        elif MODE_BY_SC[position(sc)] != 'user':
            logger.warning("Arm refused: SC is not on user. Set it, then cycle SA off and on")
            self.arm_requested_at = None
        elif time.monotonic() - self.arm_requested_at >= ARM_HOLD_S:
            self.arm_requested_at = None
            self.armed = True
            # Switch positions as they are now, including ones set before start-up.
            self.selected_mode = MODE_BY_SC[position(sc)]
            self.record_switch = bool(self.js.button_states.get('sd', 0)) if self.js else False
            self.direction_switch = DIRECTION_BY_SB[position(self.axis('sb'))]
            self.direction = self.direction_switch   # safe: the stick is at the bottom
            logger.info(f"Armed: {DIRECTION_NAMES[self.direction]}, "
                        f"recording {'enabled' if self.record_switch else 'disabled'}")

    # --- part interface --------------------------------------------------------

    def run_threaded(self, img_arr=None, mode=None, recording=None):
        """
        The radio is the authority: mode and recording coming in from the web
        UI are ignored. Returns angle, throttle, mode, recording.
        """
        self.img_arr = img_arr
        if self.js is None:  # device gone; the patched update() reconnects
            self.disarm("joystick lost")
        self.check_arm_request()
        if not self.armed:
            self.throttle = 0.0
            return self.angle, 0.0, 'user', False

        level = self.throttle_level()
        if self.direction_switch != self.direction:
            if self.direction_switch == 0.0 or level == 0.0:
                self.direction = self.direction_switch
                logger.info(f"Direction engaged: {DIRECTION_NAMES[self.direction]}")
        if self.direction_switch != self.direction:
            self.throttle = 0.0   # waiting for the stick to come to the bottom
        else:
            self.throttle = self.direction * level * self.throttle_scale

        mode = self.selected_mode
        recording = (self.record_switch and mode == 'user'
                     and self.direction > 0 and level > 0.0)
        return self.angle, self.throttle, mode, recording
