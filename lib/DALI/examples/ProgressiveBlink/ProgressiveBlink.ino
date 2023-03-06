/*
   Copyright (c) 2019 Boot&Work Corp., S.L. All rights reserved
*/

#include <DALI.h>

DALI dali(2, 3);

uint8_t value = 0;
uint32_t lastStepTime = millis();

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600UL);

  dali.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  if (millis() - lastStepTime > 5) {
    value += 10;
    if (value > 254) {
      value = 0;
    }

    dali.setValue(6, value);

    lastStepTime = millis();
  }
}
