// Define pins for buttons, MOSFETs, sensors, and LEDs
const int buttonAPin = 8;
const int buttonBPin = 9;
const int buttonCPin = 10;
const int mosfetAPin = 5;
const int mosfetBPin = 6;
const int mosfetCPin = 7;
const int sensorAPin = A9;
const int sensorBPin = A10;
const int buttonActivatePin = 3;
const int buttonAbortPin = 4;
const int ledAPin = 11;
const int ledBPin = 12;
const int ledCPin = 13;

// Define variables for tracking time and MOSFET states
unsigned long lastTriggerTime = 0;
int lastTriggeredMosfet = -1;
bool systemActive = false;
bool abortSequence = false;

void setup() {
  // Set up button pins as inputs with pull-up resistors
  pinMode(buttonAPin, INPUT_PULLUP);
  pinMode(buttonBPin, INPUT_PULLUP);
  pinMode(buttonCPin, INPUT_PULLUP);
  pinMode(buttonActivatePin, INPUT_PULLUP);
  pinMode(buttonAbortPin, INPUT_PULLUP);

  // Set up MOSFET pins as outputs
  pinMode(mosfetAPin, OUTPUT);
  pinMode(mosfetBPin, OUTPUT);
  pinMode(mosfetCPin, OUTPUT);

  // Set up sensor pins as inputs
  pinMode(sensorAPin, INPUT);
  pinMode(sensorBPin, INPUT);

  // Set up LED pins as outputs
  pinMode(ledAPin, OUTPUT);
  pinMode(ledBPin, OUTPUT);
  pinMode(ledCPin, OUTPUT);

  // Start serial communication at 9600 baud
  Serial.begin(9600);
}

void loop() {
  // Check if the system should be activated or paused
  if (digitalRead(buttonActivatePin) == LOW) {
    systemActive = !systemActive; // toggle system active state
    delay(1000); // wait for button debounce
  }

  // Check if the sequence should be aborted
  if (digitalRead(buttonAbortPin) == LOW) {
    abortSequence = true;
    delay(1000); // wait for button debounce
  }

  // Check if the system is currently active
  if (systemActive) {
    // Read sensor values
    int sensorAValue = analogRead(sensorAPin);
    int sensorBValue = analogRead(sensorBPin);

    // Check if the thresholds are met for triggering a MOSFET
    if (sensorAValue >= 300 && sensorBValue < 100) {
      // Select a random MOSFET to trigger
      int mosfetToTrigger = random(3);

      // Check if this MOSFET was already triggered last time
      while (mosfetToTrigger == lastTriggeredMosfet) {
        mosfetToTrigger = random(3);
      }

      // Set the selected MOSFET to the on state and record the trigger time
      if (mosfetToTrigger == 0) {
        digitalWrite(mosfetAPin, HIGH);
        Serial.print("300,");
        Serial.print(sensorBValue);
        Serial.print(",A,");
        lastTriggeredMosfet = 0;
      } else if (mosfetToTrigger == 1) {
        digitalWrite(mosfetBPin, HIGH);
        Serial.print("300,");
        Serial.print(sensorBValue);
        Serial.print(",B,");
        lastTriggeredMosfet = 1;
      } else
      digitalWrite(mosfetCPin, HIGH);
      Serial.print("300,");
      Serial.print(sensorBValue);
      Serial.print(",C,");
      lastTriggeredMosfet = 2;
      lastTriggerTime = millis();
    }

    // Check if enough time has passed since the last MOSFET trigger
    if (lastTriggerTime > 0 && millis() - lastTriggerTime >= 500) {
      // Turn off all MOSFETs and reset the last trigger time
      digitalWrite(mosfetAPin, LOW);
      digitalWrite(mosfetBPin, LOW);
      digitalWrite(mosfetCPin, LOW);
      lastTriggerTime = 0;
      lastTriggeredMosfet = -1;
    }
  }

  // Check if buttons are pressed and light up the corresponding LED
  if (digitalRead(buttonAPin) == LOW) {
    digitalWrite(ledAPin, HIGH);
  } else {
    digitalWrite(ledAPin, LOW);
  }

  if (digitalRead(buttonBPin) == LOW) {
    digitalWrite(ledBPin, HIGH);
  } else {
    digitalWrite(ledBPin, LOW);
  }

  if (digitalRead(buttonCPin) == LOW) {
    digitalWrite(ledCPin, HIGH);
  } else {
    digitalWrite(ledCPin, LOW);
  }

  // Check if the abort button was pressed and light up all LEDs for 2 seconds
  if (abortSequence) {
    digitalWrite(ledAPin, HIGH);
    digitalWrite(ledBPin, HIGH);
    digitalWrite(ledCPin, HIGH);
    delay(2000);
    digitalWrite(ledAPin, LOW);
    digitalWrite(ledBPin, LOW);
    digitalWrite(ledCPin, LOW);
    abortSequence = false;
  }

  // Check if the flash button was pressed and flash all LEDs 5 times for 250ms each
  if (digitalRead(buttonActivatePin) == LOW) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(ledAPin, HIGH);
      digitalWrite(ledBPin, HIGH);
      digitalWrite(ledCPin, HIGH);
      delay(250);
      digitalWrite(ledAPin, LOW);
      digitalWrite(ledBPin, LOW);
      digitalWrite(ledCPin, LOW);
      delay(250);
    }
  }

  // Check if a MOSFET was triggered and a button was pressed
  if (lastTriggerTime > 0 && (digitalRead(buttonAPin) == LOW || digitalRead(buttonBPin) == LOW || digitalRead(buttonCPin) == LOW)) {
    // Calculate the duration between the MOSFET trigger and button press
    unsigned long duration = millis() - lastTriggerTime;
    Serial.print(duration);
    Serial.println("ms");
  }

  delay(50); // small delay for button debouncing
}
