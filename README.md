# Bricktroll OLED edition
It can now operate with cheap OLED displays, later I plan to add a 'blind precompiled remote' to run it without a display at all.
You can now compile it in the __Arduino IDE__.

Follow to the __bricktroll_oled__ folder

__________________________
original README below this line
__________________________
# Bricktroll

Firmware for a "real hardware remote" for Lego Control+ using an esp32.

# Installation

- Install [platformio](https://platformio.org/)
- Edit platformio.ini and set correct upload port
- Connect the device via usb
- To flash the firmware run the following command:
```bash
  pio run --target=upload
```

# Suitable hardware
https://bit.ly/39XGrUp
