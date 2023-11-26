#define USE_ARDUINO_INTERRUPTS true    
#include <PulseSensorPlayground.h>

const int PULSESENSOR_PIN = 0;
const int SWITCH_PIN = 9;
const int LED_SWITCH_PIN = 3;
const int LED_PULSE_PIN = 4;

const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

int incomingByte;
byte samplesUntilReport;
PulseSensorPlayground pulseSensor;

void setup () {
  Serial.begin(115200);

  pinMode(LED_PULSE_PIN, OUTPUT);
  pinMode(LED_SWITCH_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);

  digitalWrite(LED_PULSE_PIN, LOW);
  digitalWrite(LED_SWITCH_PIN, LOW);

  pulseSensor.analogInput(PULSESENSOR_PIN);
  pulseSensor.setSerial(Serial);
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

  // NOTE: feedback if something goes wrong
  if (!pulseSensor.begin()) failureFeedback();
  else okFeedback();
}

void loop () {
  // Get switch state and display its state
  const int pressed = digitalRead(SWITCH_PIN);
  digitalWrite(LED_SWITCH_PIN, pressed);

  // Handle pulse led
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == 'H') digitalWrite(LED_PULSE_PIN, HIGH);
    if (incomingByte == 'L') digitalWrite(LED_PULSE_PIN, LOW);
  }

  // Read pulse sensor
  if (pulseSensor.sawNewSample()) {
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      
      // Send sensor states via Serial
      // Sending 100|0 to simulate capacitive cycles used in previous version of this sensor for retrocompatibility
      if (pressed == HIGH) Serial.print(100);
      else Serial.print(0);
      Serial.write(',');
      pulseSensor.outputSample();
    }
  }
}

void okFeedback () {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PULSE_PIN, LOW);
    digitalWrite(LED_SWITCH_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PULSE_PIN, HIGH);
    digitalWrite(LED_SWITCH_PIN, LOW);
    delay(200);
  }

  digitalWrite(LED_PULSE_PIN, LOW);
  digitalWrite(LED_SWITCH_PIN, LOW);
}

void failureFeedback () {
  for(;;) {
    digitalWrite(LED_PULSE_PIN, LOW);
    digitalWrite(LED_SWITCH_PIN, LOW);
    delay(200);
    digitalWrite(LED_PULSE_PIN, HIGH);
    digitalWrite(LED_SWITCH_PIN, HIGH);
    delay(200);
  }
}
