#include <Arduino.h>
#include <RCSwitch.h>

RCSwitch switcher = RCSwitch();

#define TRANSMITTER_PIN 1

String readInput()
{
  String line = "";
  char byte;

  while (true) {
    if (Serial.available()) {
      byte = Serial.read();

      if (byte == '\n') {
        break;
      }

      line = line + String(byte);
    }
  }

  return line;
}

unsigned long binarySearch(unsigned long leftBound, unsigned long rightBound, unsigned int bitLength)
{
  if (leftBound == rightBound) {
    return leftBound;
  }

  unsigned long middle = (leftBound + rightBound) / 2;

  Serial.println("");
  Serial.print("Checking range of codes [");
  Serial.print(leftBound);
  Serial.print(",");
  Serial.print(middle);
  Serial.print("] with bit length ");
  Serial.println(bitLength);

  unsigned long code;
  for (code  = middle; code < rightBound; code++) {
    switcher.send(code, bitLength);

    if (code % 100 == 0) {
      Serial.println(code);
    }
  }

  // check if code successfully received
  Serial.println("");
  Serial.println("Is device switched [y/n]?");

  if (readInput() == "y") {
    return binarySearch(middle, rightBound, bitLength);
  } else {
    return binarySearch(leftBound, middle, bitLength);
  }
}

void setup() {
  Serial.begin(115200);
  switcher.enableTransmit(TRANSMITTER_PIN);
  switcher.setRepeatTransmit(1);
  switcher.setPulseLength(305);
  switcher.setProtocol(1); //todo: allow define protocols
}

void loop() {
  // read bit length
  Serial.print("Input bit length: ");
  int bitLength = readInput().toInt();

  // define range
  unsigned long one = 1;
  unsigned long leftBound = one;
  unsigned long rightBound = (one << bitLength) - 1;

  String message = "\nInitial range: [";
  message += leftBound;
  message += ", ";
  message += rightBound;
  message += "] with bit length ";
  message += bitLength;

  Serial.println(message);

  unsigned long code = binarySearch(leftBound, rightBound, bitLength);

  Serial.println("Code is: " + code);
}