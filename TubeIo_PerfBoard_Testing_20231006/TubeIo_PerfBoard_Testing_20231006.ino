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
  tubeIO.begin();
  tubeIO._debug = true;
}

void loop() 
{
  tubeIO.run();
}
