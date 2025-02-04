#include <Wire.h>
#include <Adafruit_SSD1306.h>

//-----------------------------------------------
Adafruit_SSD1306 display(128, 64, &Wire, -1);
//-----------------------------------------------
#define CLK    18
#define DT     19
#define SW     5
#define BUZZER_PIN 23  // Buzzer pin added

//-----------------------------------------------
int flowMinutes = 0;   // Total flow minutes to track productivity
int menuIndex = 0;     // Menu index for navigation
String menuOptions[3] = {"UP", "DOWN", "RESET"}; // Menu options
unsigned long lastActivityTime = 0; // Tracks the last user activity timestamp
const unsigned long inactivityLimit = 5 * 60000; // Inactivity timeout for idle state (5 minutes)

enum State { MENU, COUNTING_UP, COUNTING_DOWN, SELECTING_DOWN_DURATION, 
             IDLE, COUNTING_BREAK, SELECTING_BREAK_DURATION };
State currentState = MENU;

int countdownValue = 50; // Default countdown timer value in minutes
int initialCountdownValue = 50;
int breakDuration = 10;  // User-adjustable break duration in minutes
unsigned long previousMillis = 0;
int elapsedMinutes = 0;
bool isCounting = false;
unsigned long buttonDebounceTime = 0;
const unsigned long buttonDebounceDelay = 800; // Debounce time for button presses (800 ms)

// Rotary encoder debounce variables
unsigned long lastRotaryTime = 0;
const unsigned long rotaryDebounceDelay = 50; // Debounce delay for rotary encoder (150 ms)

// IDLE mode extended behavior
const unsigned long displayOffTimeLimit = 25 * 60000; // Display off after 30 minutes of inactivity
unsigned long idleStartTime = 0;
bool displayOff = false;

//=========================================================
void setup() {  
  initHardware();
  initDisplay();
  updateDisplay();
  Serial.println("Setup complete, starting loop...");
}

//=========================================================
void loop() {
  unsigned long currentMillis = millis();
  
  handleRotaryInput(); // Handle rotary encoder navigation
  handleButtonPresses(currentMillis); // Handle button press events
  handleCounting(currentMillis); // Handle counting logic
  handleInactivity(currentMillis); // Handle device inactivity logic
}

//=========================================================
void initHardware() {
  pinMode(CLK, INPUT); // Rotary encoder CLK pin
  pinMode(DT, INPUT);  // Rotary encoder DT pin
  pinMode(SW, INPUT);  // Rotary encoder switch pin
  pinMode(BUZZER_PIN, OUTPUT);  // Buzzer initialization
  Serial.begin(19200); // Initialize serial communication
}

//=========================================================
void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Initialize display
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay(); 
  Serial.println("Display initialized.");
}

//=========================================================
void shortBeepBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(50); // Short beep duration
  digitalWrite(BUZZER_PIN, LOW);
}

//=========================================================
void beepBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200); // Longer beep duration
  digitalWrite(BUZZER_PIN, LOW);
}

//=========================================================
void updateDisplay() {
  display.setTextColor(WHITE);
  display.clearDisplay();

  // Top row text
  String topRowText;
  if (currentState == COUNTING_UP) {
    topRowText = "Focus \x18";
  } else if (currentState == COUNTING_DOWN) {
    topRowText = "Focus \x19";
  } else if (currentState == COUNTING_BREAK) {
    topRowText = "Break \x19"; // Updated header
  } else if (currentState == SELECTING_DOWN_DURATION) {
    topRowText = "Set Focus";
  } else if (currentState == SELECTING_BREAK_DURATION) {
    topRowText = "Set Break";
  } else {
    if (flowMinutes >= 60) {
      int hours = flowMinutes / 60;
      int minutes = flowMinutes % 60;
      topRowText = "Flow:" + String(hours) + "h" + String(minutes) + "m";
    } else {
      topRowText = "Flow:" + String(flowMinutes) + "m";
    }
  }

  int topRowTextWidth = topRowText.length() * 12;
  int topRowX = (128 - topRowTextWidth) / 2;

  display.setTextSize(2);
  display.setCursor(topRowX, 0);
  display.print(topRowText);

  // Display main row text for menu or counting status
  String mainRowText;
  
  if (currentState == MENU) {
    mainRowText = menuOptions[menuIndex];
  } else if (currentState == COUNTING_UP) {
    if (elapsedMinutes >= 60) {
      int hours = elapsedMinutes / 60;
      int minutes = elapsedMinutes % 60;
      mainRowText = String(hours) + "h" + String(minutes) + "m";
    } else {
      mainRowText = String(elapsedMinutes) + "m";
    }
  } else if (currentState == SELECTING_DOWN_DURATION || 
             currentState == COUNTING_DOWN || currentState == COUNTING_BREAK) {
    if (countdownValue >= 60) {
      int hours = countdownValue / 60;
      int minutes = countdownValue % 60;
      mainRowText = String(hours) + "h" + String(minutes) + "m";
    } else {
      mainRowText = String(countdownValue) + "m";
    }
  } else if (currentState == SELECTING_BREAK_DURATION) {
    if (breakDuration >= 60) {
      int hours = breakDuration / 60;
      int minutes = breakDuration % 60;
      mainRowText = String(hours) + "h" + String(minutes) + "m";
    } else {
      mainRowText = String(breakDuration) + "m";
    }
  } else if (currentState == IDLE) {
    mainRowText = "IDLE";
  }
  
  int mainRowTextWidth = mainRowText.length() * 24;
  int mainRowX = (128 - mainRowTextWidth) / 2;
  display.setTextSize(4);
  display.setCursor(mainRowX, 30);
  display.print(mainRowText);
  
  
  display.display();
}

//=========================================================
bool buttonPressed() {
  // Check for button press with debounce logic
  if (digitalRead(SW) == LOW && (millis() - buttonDebounceTime > buttonDebounceDelay)) {
    buttonDebounceTime = millis();
    lastActivityTime = millis();
    return true;
  }
  return false;
}

//=========================================================
void handleButtonPresses(unsigned long currentMillis) {
  if (!buttonPressed()) return;

  switch (currentState) {
    case MENU:
      if (menuIndex == 0) {
        startCountingUp();
      } else if (menuIndex == 1) {
        shortBeepBuzzer(); 
        startSelectingDownDuration();
      } else if (menuIndex == 2) {
        resetFlowMinutes();
      }
      break;
      
    case SELECTING_DOWN_DURATION:
      confirmCountdownSelection();
      break;

    case SELECTING_BREAK_DURATION:
      confirmBreakDuration();
      break;

    case COUNTING_UP:
      stopCountingUp();
      break;

    case COUNTING_DOWN:
      stopCountingDown();
      break;

    case COUNTING_BREAK:
      currentState = MENU;
      isCounting = false;
      beepBuzzer();
      successAnimation();
      Serial.println("Break stopped. Returning to MENU.");
      break;
  }
  updateDisplay();
}

//=========================================================
void startCountingUp() {
  currentState = COUNTING_UP;
  elapsedMinutes = 0;
  isCounting = true;
  lastActivityTime = millis();
  beepBuzzer();
  Serial.println("Counting UP started.");
}

//=========================================================
void startSelectingDownDuration() {
  currentState = SELECTING_DOWN_DURATION;
  countdownValue = 50;
  lastActivityTime = millis();
  Serial.println("Selecting DOWN duration.");
}

//=========================================================
void confirmCountdownSelection() {
  initialCountdownValue = countdownValue;
  currentState = SELECTING_BREAK_DURATION;
  lastActivityTime = millis();
  updateDisplay();
  Serial.println("Now selecting BREAK duration.");
}

//=========================================================
void confirmBreakDuration() {
  currentState = COUNTING_DOWN;
  isCounting = true;
  lastActivityTime = millis();
  beepBuzzer();
  Serial.print("Starting COUNTING_DOWN with ");
  Serial.print(initialCountdownValue);
  Serial.print("m focus and ");
  Serial.print(breakDuration);
  Serial.println("m break");
  updateDisplay();
}

//=========================================================
void stopCountingUp() {
  flowMinutes += elapsedMinutes;
  beepBuzzer();
  successAnimation();
  currentState = MENU;
  isCounting = false;
  Serial.println("Counting UP stopped. Returning to MENU.");
}

//=========================================================
void stopCountingDown() {
  flowMinutes += (initialCountdownValue - countdownValue);
  beepBuzzer();
  successAnimation();
  currentState = MENU;
  isCounting = false;
  Serial.println("Counting DOWN stopped. Returning to MENU.");
}

//=========================================================
void resetFlowMinutes() {
  flowMinutes = 0;
  beepBuzzer();
  Serial.println("Flow minutes reset to 0.");
  updateDisplay();
}

//=========================================================
//=========================================================
void handleCounting(unsigned long currentMillis) {
  if (!isCounting || (currentMillis - previousMillis < 60000)) return;

  previousMillis = currentMillis;
  
  if (currentState == COUNTING_UP) {
    elapsedMinutes++;
    updateDisplay();
    Serial.print("Counting UP: "); Serial.println(elapsedMinutes);
  } 
  else if (currentState == COUNTING_DOWN) {
    countdownValue--;
    if (countdownValue <= 0) {
      flowMinutes += initialCountdownValue;
      beepBuzzer();
      successAnimation();
      currentState = COUNTING_BREAK;
      countdownValue = breakDuration; // Set to break duration
      isCounting = true;
      previousMillis = currentMillis;
      updateDisplay(); // Force immediate display update
      Serial.println("Countdown finished, starting break timer.");
    }
    updateDisplay();
    Serial.print("Counting DOWN: "); Serial.println(countdownValue);
  } 
  else if (currentState == COUNTING_BREAK) {
    countdownValue--;
    if (countdownValue <= 0) {
      for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(150);
        digitalWrite(BUZZER_PIN, LOW);
        if (i < 2) delay(100);
      }
      successAnimation();
      currentState = COUNTING_DOWN;
      countdownValue = initialCountdownValue;
      isCounting = true;
      previousMillis = currentMillis;
      updateDisplay(); // Force immediate display update
      Serial.println("Break finished, restarting COUNTING_DOWN.");
    }
    updateDisplay();
    Serial.print("Counting BREAK: "); Serial.println(countdownValue);
  }
}

//=========================================================
void successAnimation() {
  display.clearDisplay();
  int centerX = 64, centerY = 32;

  for (int radius = 2; radius <= 30; radius += 2) {
    display.drawCircle(centerX, centerY, radius, WHITE);
    display.display();
    delay(100);

    if (radius % 4 == 0) {
      display.clearDisplay();
      display.display();
      delay(2);
    }
  }
  
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 20);
  display.print("SUCCESS");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
}

//=========================================================
int getRotation() {
  static int previousCLK = digitalRead(CLK);
  int currentCLK = digitalRead(CLK);
  
  if (currentCLK == LOW && previousCLK == HIGH && 
     (millis() - lastRotaryTime > rotaryDebounceDelay)) {
    lastRotaryTime = millis();
    int DTValue = digitalRead(DT);

    previousCLK = currentCLK;
    return (DTValue != currentCLK) ? 1 : -1;
  }
  
  previousCLK = currentCLK;
  return 0;
}

//=========================================================
void handleRotaryInput() {
  int rotation = getRotation();
  if (rotation == 0) return;
  lastActivityTime = millis();

  if (currentState == MENU) {
    shortBeepBuzzer();
    menuIndex = (menuIndex + rotation + 3) % 3;
    Serial.print("Menu option: "); Serial.println(menuOptions[menuIndex]);
  } 
  else if (currentState == SELECTING_DOWN_DURATION) {
    countdownValue = max(1, countdownValue + rotation);
    Serial.print("Focus duration: "); Serial.println(countdownValue);
  } 
  else if (currentState == SELECTING_BREAK_DURATION) {
    breakDuration = max(1, breakDuration + rotation);
    Serial.print("Break duration: "); Serial.println(breakDuration);
  }
  
  updateDisplay();
}

//=========================================================
void handleInactivity(unsigned long currentMillis) {
  if (currentMillis >= lastActivityTime) {
    unsigned long timeSinceLastActivity = currentMillis - lastActivityTime;

    // Handle break duration selection timeout
    if (currentState == SELECTING_BREAK_DURATION && 
        timeSinceLastActivity > 5000) {
      confirmBreakDuration();
      Serial.println("Break duration auto-confirmed after timeout");
    }
    // Handle general inactivity timeout
    else if ((currentState == MENU || currentState == SELECTING_DOWN_DURATION) && 
            (timeSinceLastActivity > inactivityLimit)) {
      if (currentState != IDLE) {
        currentState = IDLE;
        idleStartTime = millis();
        updateDisplay();
        shortBeepBuzzer();
        Serial.println("Entering IDLE state due to inactivity");
      }
    }
  }

  // Handle display power management
  if (currentState == IDLE && !displayOff && 
     (currentMillis - idleStartTime > displayOffTimeLimit)) {
    displayOff = true;
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    Serial.println("Display turned off");
  }

  // Wake from IDLE state
  if (currentState == IDLE && (getRotation() != 0 || buttonPressed())) {
    currentState = MENU;
    lastActivityTime = millis();
    
    if (displayOff) {
      display.ssd1306_command(SSD1306_DISPLAYON);
      displayOff = false;
      Serial.println("Display turned on");
    }
    shortBeepBuzzer();
    updateDisplay();
    Serial.println("Exiting IDLE state");
  }
}
