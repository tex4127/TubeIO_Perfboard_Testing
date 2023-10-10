#ifndef __ERRORCODES_HH__
#define __ERRORCODES_HH__

#include"Arduino.h"

typedef enum
{
  NoError = 0,
  bufferOverFlow = 10,
  argumentOutOfRange
} ErrorCodes_t;

#endif