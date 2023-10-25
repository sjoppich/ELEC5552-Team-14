const int switchPin = 7;  // Output pin connected to the logic input of DG419LE
const int buttonPin = 2; // Input pin connected to a push button

enum SwitchingMode {
  MODE_OFF,
  MODE_10HZ,
  MODE_100HZ
};

SwitchingMode currentMode = MODE_OFF;

const int interval10Hz = 50000;  // Corresponds to 10Hz (half period in microseconds)
const int interval100Hz = 5000;  // Corresponds to 100Hz (half period in microseconds)

unsigned long lastSwitchTime = 0;
bool buttonPressedBefore = false;

void setup() {
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, LOW); // Start with NC (Normally Closed) connected
  
  pinMode(buttonPin, INPUT_PULLUP); // Button connected between buttonPin and GND
}

void loop() {
  unsigned long currentMillis = micros();

  // Check button press
  if (digitalRead(buttonPin) == LOW && !buttonPressedBefore) {
    // Button just pressed
    buttonPressedBefore = true;
    delay(20);  // debounce delay

    switch (currentMode) {
      case MODE_OFF:
        currentMode = MODE_10HZ;
        break;
      case MODE_10HZ:
        currentMode = MODE_100HZ;
        break;
      case MODE_100HZ:
        currentMode = MODE_OFF;
        break;
    }
  } else if (digitalRead(buttonPin) == HIGH) {
    buttonPressedBefore = false;
  }

  // Perform switching based on mode
  switch (currentMode) {
    case MODE_OFF:
      // Do nothing
      break;
    case MODE_10HZ:
      if (currentMillis - lastSwitchTime >= interval10Hz) {
        toggleSwitch();
        lastSwitchTime = currentMillis;
      }
      break;
    case MODE_100HZ:
      if (currentMillis - lastSwitchTime >= interval100Hz) {
        toggleSwitch();
        lastSwitchTime = currentMillis;
      }
      break;
  }
}

void toggleSwitch() {
  if (digitalRead(switchPin) == LOW) {
    digitalWrite(switchPin, HIGH);
  } else {
    digitalWrite(switchPin, LOW);
  }
}
