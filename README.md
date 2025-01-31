# FocusFlow: Blending Technology with Mindful Productivity  
> [!TIP]  
> FocusFlow is a simple yet powerful device designed to help you maintain focus. It functions seamlessly as is and offers the versatility to be customized according to your needs. With robust hardware, a tactile and minimalistic design, and modular firmware, it provides a solid foundation for innovation.  
>  
> Whether you're enhancing its features or integrating it into a larger system, the possibilities are endless. I'm eager to see how you make it uniquely yours!  

---

## About FocusFlow  
FocusFlow is a minimalist, tactile tool designed to help you track and improve the time spent on meaningful tasks each day. With a simple yet intuitive interface powered by a rotary encoder and OLED display, it seamlessly integrates into your workflow—whether for focused work sessions, break tracking, or other time-based routines.  

### Why FocusFlow?  
- **Simple & Effective:** Designed for distraction-free focus tracking.  
- **Affordable & Hackable:** Easy to build, modify, and expand.  
- **Tactile & Satisfying:** Smooth rotary encoder control for an enjoyable experience.  
- **Modular & Open-Source:** Contribute and customize freely.  

---

## Features (v1)  
- **Timer Modes:** Choose between *count up* or *count down* modes in minutes—no distracting seconds.  
- **Rotary Encoder Control:** Navigate menus and adjust settings effortlessly. Rotate to scroll, click to select.  
- **OLED Display:** Provides real-time session feedback, total accumulated *Flow* minutes, and menu navigation.  
- **Buzzer Feedback:** Alerts for menu navigation, session start, and session end.  
- **Auto IDLE Mode:** Prompts after 5 minutes of inactivity and turns off the display after 30 minutes to conserve energy.  
- **Success Animation:** Short animation at the end of each session for positive reinforcement.  
- **Easy Reset:** Reset your total focus time via the menu or by unplugging the device.  

![FocusFlow](https://github.com/itsjustasemicolon/focusflow/blob/main/FocusFlow_v1.jpg?raw=true)  

---

## Bill of Materials (BOM)  
| Component                     | Quantity |  
|-------------------------------|----------|  
| Adafruit SSD1306 OLED Display | 1        |  
| KY-040 Rotary Encoder         | 1        |  
| ESP32 Board                   | 1        |  
| Breadboard                    | 1        |  
| Jumper Wires                  | As needed|  

---

## Pinout & Wiring  
| Component            | ESP32 GPIO Pin |  
|----------------------|----------------|  
| Rotary Encoder SW    | 5              |  
| Rotary Encoder CLK   | 18             |  
| Rotary Encoder DT    | 19             |  
| OLED SDA             | 21             |  
| OLED SCL             | 22             |  
| Buzzer +ve           | 23             |  

![Circuit Diagram](https://github.com/itsjustasemicolon/FocusFlow/blob/main/circuitDiagram.png)  

---

## Firmware & Assembly  
### Steps  
1. **Test Components:** Wire everything, upload firmware, and verify functionality before finalizing connections.  
2. **Connect OLED Panel:** Ensure the display is correctly wired to the ESP32.  
3. **Attach Rotary Encoder & ESP32:** Place components securely on the breadboard.  
4. **Final Checks:** Ensure all connections are stable and working as expected.  

---

## User Guide  
### Getting Started  
1. Power on the device; the main menu displays total focus time at the top.  
2. Use the rotary encoder to scroll through options (*UP*, *DOWN*, *Reset*), and press to select.  

### Operating Modes  
- **UP Mode:** Start a count-up timer for open-ended sessions.  
- **DOWN Mode:** Set a countdown timer and track sessions to completion.  
- **Reset:** Reset accumulated *Flow* minutes to zero.  
- **IDLE Mode:** Prompt after 5 minutes of inactivity; display turns off after 30 minutes.  

---

## Code  
[FocusFlow_v1.ino](https://github.com/itsjustasemicolon/FocusFlow/blob/main/FocusFlow_v1.ino)  

---

## Future Enhancements & Contributions  
Potential improvements include:  
- [ ] Sound notifications  
- [ ] Wi-Fi connectivity for real-time tracking  
- [ ] Additional animations & transitions  
- [ ] New tracking modes  
- [ ] API integration for external apps  

---

## Disclaimer  
This is an open-source project provided *as is*, with no guarantees. Users should have basic electronics knowledge. If you encounter issues or have suggestions, please open an issue on GitHub.  

**Stay focused. Stay productive. Let's build something great together!**  
