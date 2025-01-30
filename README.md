# FocusFlow - Blending technology with mindful productivity.

> [!TIP]
> FocusFlow is designed as a straightforward and effective focus assistant. It works perfectly as is, but it also provides the flexibility for customization. If you want to expand its capabilities, the hardware is robust, the design is tactile and satisfying, and the firmware is modular for easy development.
>
> I'm excited to see how you make it your own!

## About FocusFlow
FocusFlow is a minimalist, tactile tool designed to help you track and improve the time spent on meaningful tasks each day. With a simple yet intuitive interface powered by a rotary encoder and OLED display, it seamlessly integrates into your workflow—whether for focused work sessions, break tracking, or other time-based routines.

### Why FocusFlow?
- **Simple & Effective:** Designed for distraction-free focus tracking.
- **Affordable & Hackable:** Easy to build, modify, and expand.
- **Tactile & Satisfying:** Smooth rotary encoder control for an enjoyable experience.
- **Modular & Open-Source:** Contribute and customize freely.

## Features (v1)
- **Timer Modes:** Choose between *count up* or *count down* modes in minutes—no distracting seconds.
- **Rotary Encoder Control:** Navigate menus and adjust settings effortlessly. Rotate to scroll, click to select.
- **OLED Display:** Provides real-time session feedback, total accumulated *Flow* minutes, and menu navigation.
- **Buzzer Feedback:** Alerts for menu navigation, session start, and session end.
- **Auto IDLE Mode:** Prompts after 5 minutes of inactivity and turns off the display after 10 minutes to conserve energy.
- **Success Animation:** Short animation at the end of each session for positive reinforcement.
- **Easy Reset:** Reset your total focus time via the menu or by unplugging the device.

![FocusFlow](https://github.com/itsjustasemicolon/focusflow/blob/main/FocusFlow_v1.jpg?raw=true)

## Bill of Materials (BOM)
- **Adafruit SSD1306 OLED Display (128x64)**
- **KY-040 Rotary Encoder**
- **ESP32 Board** (Other Arduino-compatible boards may work with slight modifications.)
- **Breadboard**
- **Jumper Wires**

## Wiring & Pinout
| Component            | GPIO Pin |
|----------------------|-----|
| Rotary Encoder SW    | 5   |
| Rotary Encoder CLK   | 18  |
| Rotary Encoder DT    | 19  |
| Rotary Encoder VIN   | VIN |
| Rotary Encoder GND   | GND |
| OLED SDA             | 21  |
| OLED SCL             | 22  |
| OLED VIN             | VIN |
| OLED GND             | GND |
| Buzzer +ve           | 23  |
| Buzzer -ve           | GND  |

## Firmware & Assembly
The firmware consists of a single sketch file, easily compiled using the Arduino IDE (with ESP32 support and necessary libraries installed). Verify functionality before assembling the components on the breadboard.

### Assembly Steps
1. **Test Components:** Wire everything, upload firmware, and verify functionality before finalizing connections.
2. **Connect OLED Panel:** Ensure the display is correctly wired to the ESP32.
3. **Attach Rotary Encoder & ESP32:** Place components securely on the breadboard.
4. **Final Checks:** Ensure all connections are stable and working as expected.

## User Guide
### Getting Started
- Power on the device; the main menu will display the total focus time at the top.
- Use the rotary encoder to scroll through options (*UP, DOWN, Reset*), and press to select.

### Operating Modes
- **UP Mode:** Start a count-up timer for open-ended focus sessions.
- **DOWN Mode:** Set a countdown timer and track sessions to completion.
- **Reset:** Reset the accumulated *Flow* minutes to zero.
- **IDLE Mode:** A prompt appears after 5 minutes of inactivity; after 30 minutes, the display turns off to save power.

### Example Use Case
1. Select **UP** to start a focus session.
2. Stop the timer when done; your total focus time updates.
3. Select **DOWN** for a timed break session.
4. Reset total focus time at the end of the day, if needed.

## Code
 [FocusFlow_v1.ino](https://github.com/itsjustasemicolon/FocusFlow/blob/main/FocusFlow_v1.ino)

## Future Enhancements & Contributions
Potential improvements include:
- [ ] Sound notifications
- [ ] Wi-Fi connectivity for real-time tracking
- [ ] Additional animations & transitions
- [ ] New tracking modes
- [ ] API integration for external apps (time trackers, to-do lists, automation tools)

If you have ideas or improvements, feel free to contribute! I’m primarily a designer, not a programmer, so your expertise would be invaluable in evolving FocusFlow.

## Disclaimer
This is an open-source project provided *as is*, with no guarantees. Users should have basic electronics knowledge. If you encounter issues or have suggestions, please open an issue on GitHub.

Stay focused. Stay productive. Let's build something great together!

