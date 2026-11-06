"""Run Yellow's RadioMaster controller (my_joystick.py) alone, with no motor and
no camera, and print what it would send to the car whenever that changes.

Use it to check the mapping, the arming interlock and link loss with nothing
able to move. Run on the Pi, in the DonkeyCar venv, from ~/mycar (where
my_joystick.py is deployed):

    cd ~/mycar && ~/env/bin/python rc_probe.py [seconds]     # default 60
"""
import logging
import sys
import threading
import time

from my_joystick import MyJoystickController

seconds = float(sys.argv[1]) if len(sys.argv) > 1 else 60

logging.basicConfig(level=logging.INFO, stream=sys.stdout,
                    format="%(relativeCreated)7.0fms %(levelname)s %(message)s")

# The values myconfig.py passes in (JOYSTICK_*).
c = MyJoystickController(throttle_dir=1.0, throttle_scale=0.5, steering_scale=1.0,
                         auto_record_on_throttle=False)
c.set_deadzone(0.05)
threading.Thread(target=c.update, daemon=True).start()

t0 = time.time()
last = None
while time.time() - t0 < seconds:
    angle, throttle, mode, recording = c.run_threaded()   # 20 Hz, like the drive loop
    state = (round(angle, 2), round(throttle, 2), mode, recording, c.armed, c.direction)
    if state != last:
        print(f"{(time.time() - t0) * 1000:7.0f}ms angle={state[0]:+.2f} throttle={state[1]:+.2f} "
              f"mode={state[2]} recording={state[3]} armed={state[4]} direction={state[5]:+.0f}", flush=True)
        last = state
    time.sleep(0.05)
c.running = False
print("done")
