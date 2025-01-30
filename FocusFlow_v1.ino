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
int flowMinutes = 0;   // Total flow minutes
int menuIndex = 0;     // 0 for UP, 1 for DOWN, 2 for Reset
String menuOptions[3] = {"UP", "DOWN", "RESET"};
unsigned long lastActivityTime = 0;
const unsigned long inactivityLimit = 5 * 60000;

enum State { MENU, COUNTING_UP, COUNTING_DOWN, SELECTING_DOWN_DURATION, IDLE };
State currentState = MENU;

int countdownValue = 50;
int initialCountdownValue = 50;
unsigned long previousMillis = 0;
int elapsedMinutes = 0;
bool isCounting = false;
unsigned long buttonDebounceTime = 0;
const unsigned long buttonDebounceDelay = 800;

// Rotary encoder debounce variables
unsigned long lastRotaryTime = 0;
const unsigned long rotaryDebounceDelay = 150;

// IDLE mode extended behavior
const unsigned long displayOffTimeLimit = 30 * 60000;
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
  
  handleRotaryInput();
  handleButtonPresses(currentMillis);
  handleCounting(currentMillis);
  handleInactivity(currentMillis);
}

//=========================================================
void initHardware() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);  // Buzzer initialization
  Serial.begin(9600);
}

//=========================================================
void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay(); 
  Serial.println("Display initialized.");
}

//=========================================================

void shortBeepBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(50); // Beep duration
  digitalWrite(BUZZER_PIN, LOW);
}

//=========================================================

void beepBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200); // Beep duration
  digitalWrite(BUZZER_PIN, LOW);
}

//=========================================================
void updateDisplay() {
  display.setTextColor(WHITE);
  display.clearDisplay();

  // Display top row
  String topRowText;
  
  if (currentState == COUNTING_UP) {
    topRowText = "Focus! \x18";
  } else if (currentState == COUNTING_DOWN) {
    topRowText = "Focus! \x19";
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

  // Display main row
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
  } else if (currentState == COUNTING_DOWN || currentState == SELECTING_DOWN_DURATION) {
    if (countdownValue >= 60) {
      int hours = countdownValue / 60;
      int minutes = countdownValue % 60;
      mainRowText = String(hours) + "h" + String(minutes) + "m";
    } else {
      mainRowText = String(countdownValue) + "m";
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
        startSelectingDownDuration();
      } else if (menuIndex == 2) {
        resetFlowMinutes();
      }
      break;
      
    case SELECTING_DOWN_DURATION:
      confirmCountdownSelection();
      break;

    case COUNTING_UP:
      stopCountingUp();
      break;

    case COUNTING_DOWN:
      stopCountingDown();
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
  beepBuzzer();  // Beep when starting UP timer
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
  currentState = COUNTING_DOWN;
  isCounting = true;
  lastActivityTime = millis();
  beepBuzzer();  // Beep when starting DOWN timer
  Serial.print("Counting DOWN started with "); Serial.println(countdownValue);
}

//=========================================================
void stopCountingUp() {
  flowMinutes += elapsedMinutes;
  beepBuzzer();  // Beep when exiting UP timer
  successAnimation();
  currentState = MENU;
  isCounting = false;
  Serial.println("Counting UP stopped. Returning to MENU.");
}

//=========================================================
void stopCountingDown() {
  flowMinutes += (initialCountdownValue - countdownValue);
  beepBuzzer();  // Beep when exiting DOWN timer
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
void handleCounting(unsigned long currentMillis) {
  if (!isCounting || (currentMillis - previousMillis < 60000)) return;

  previousMillis = currentMillis;
  
  if (currentState == COUNTING_UP) {
    elapsedMinutes++;
    updateDisplay();
    Serial.print("Counting UP: "); Serial.println(elapsedMinutes);
  } else if (currentState == COUNTING_DOWN) {
    countdownValue--;
    if (countdownValue <= 0) {
      flowMinutes += initialCountdownValue;
      beepBuzzer();  // Beep when countdown finishes
      successAnimation();
      currentState = MENU;
      isCounting = false;
      Serial.println("Countdown finished, returning to MENU.");
    }
    updateDisplay();
    Serial.print("Counting DOWN: "); Serial.println(countdownValue);
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
  
  if (currentCLK == LOW && previousCLK == HIGH && (millis() - lastRotaryTime > rotaryDebounceDelay)) {
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
  shortBeepBuzzer();
  lastActivityTime = millis();
  Serial.print(millis());
  Serial.print(" - Rotation detected, activity timer reset. Rotation: ");
  Serial.println(rotation);

  if (currentState == MENU) {
    menuIndex = (menuIndex + rotation + 3) % 3;
    updateDisplay();
    Serial.print(millis());
    Serial.print(" - Menu option: "); Serial.println(menuOptions[menuIndex]);
  } else if (currentState == SELECTING_DOWN_DURATION) {
    countdownValue = max(1, countdownValue + rotation);
    updateDisplay();
    Serial.print(millis());
    Serial.print(" - Countdown value: "); Serial.println(countdownValue);
  }
}

//=========================================================
void handleInactivity(unsigned long currentMillis) {
  if (currentMillis >= lastActivityTime) {
    unsigned long timeSinceLastActivity = currentMillis - lastActivityTime;

    if ((currentState == MENU || currentState == SELECTING_DOWN_DURATION) && 
        (timeSinceLastActivity > inactivityLimit)) {
      if (currentState != IDLE) {
        currentState = IDLE;
        idleStartTime = millis();
        updateDisplay();
        Serial.print(millis());
        Serial.println(" - IDLE state entered due to inactivity.");
      }
    }
  }

  if (currentState == IDLE && !displayOff && (currentMillis - idleStartTime > displayOffTimeLimit)) {
    displayOff = true;
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    Serial.print(millis());
    Serial.println(" - Display turned off after 30 minutes of IDLE.");
  }

  if (currentState == IDLE && (getRotation() != 0 || buttonPressed())) {
    currentState = MENU;
    lastActivityTime = millis();
    
    if (displayOff) {
      display.ssd1306_command(SSD1306_DISPLAYON);
      displayOff = false;
      Serial.print(millis());
      Serial.println(" - Display turned back on.");
    }

    updateDisplay();
    Serial.print(millis());
    Serial.println(" - Exiting IDLE mode. Back to MENU.");
  }
}