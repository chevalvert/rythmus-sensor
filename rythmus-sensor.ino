#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>

const int PULSESENSOR_PIN = 0;
const int CAPACITIVE_PIN = 5;
const int LED_PIN = 2;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

int incomingByte;
int capacitive_cycles = 0;
byte samplesUntilReport;
PulseSensorPlayground pulseSensor;

void setup () {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pulseSensor.analogInput(PULSESENSOR_PIN);
  pulseSensor.setSerial(Serial);
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

  // NOTE: feedback if something goes wrong
  if (!pulseSensor.begin()) failureFeedback();
}

void loop () {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == 'H') digitalWrite(LED_PIN, HIGH);
    if (incomingByte == 'L') digitalWrite(LED_PIN, LOW);
  }

  if (pulseSensor.sawNewSample()) {
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

      Serial.print(capacitive_cycles);
      Serial.write(',');
      pulseSensor.outputSample();
    }

    capacitive_cycles = readCapacitivePin(CAPACITIVE_PIN);
  }
}

void failureFeedback () {
  for(;;) {
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
  }
}
