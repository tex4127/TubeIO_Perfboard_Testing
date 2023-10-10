/////////////////////////////////////////////
///
//  Copyright 2023 Rad Source Technologies, Inc
//
//  Author: Jacob Garner; jgarner@radsource.com
//  Date: 10/6/2023
///
/////////////////////////////////////////////

#include "TubeIO.h"

TubeIO tubeIO;

void setup() 
{
  //Initialize the tubeIO object | serial is initialized here based on argument for begin
  Serial.begin(HWS_BAUD);
  Serial.println("Starting Up");
  tubeIO.begin();
  tubeIO._debug = true;
}

void loop() 
{
  tubeIO.run();
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //delay(1000);
}
