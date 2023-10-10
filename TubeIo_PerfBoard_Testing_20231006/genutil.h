#ifndef __GENUTIL_HH__
#define __GENUTIL_HH__

#include "Arduino.h"
#include "ErrorCodes.h"


#ifndef HWS_BAUD
#define HWS_BAUD 115200   //default serial baud for hardware
#endif

#ifndef STX_G
#define STX_G 0x02
#endif

#ifndef STX_T
#define STX_T 0x3C
#endif

#ifndef ETX_G
#define ETX_G 0x03
#endif

#ifndef ETX_T
#define ETX_T 0x3E
#endif

#ifndef ulong
#define ulong unsigned long
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef MAXBUFFERLEN
#define MAXBUFFERLEN 400
#endif

#ifndef DECIMALSIZE
#define DECIMALSIZE 14  //We want our max values to resemble 000000.000000
#endif

#ifndef DECIMALPREC
#define DECIMALPREC 6   //6 decimal places always!!
#endif



typedef enum
{
  RS232 = 0,
  Ethernet
} ComType;

/*!
 *  @brief used to store both raw and real values for certain variables 
 */
union Value
{
  uint16_t raw;
  float real;
};

union TimerValue
{
  ulong _timerSP;
  ulong _timerCV;
};

//ADD ARGUMENTS TO ARRAY METHODS | GET EVERYTHING TO A CHAR ARRAY AND ADD THAT OR JUST ADD THE RAW VALUE (+48 OF COURSE)

//ErrorCodes_t addArgToArray(char* buf, size_t& pos, char* arg, size_t len);
//ErrorCodes_t addArgToArray(char* buf, size_t& pos, int arg);
//ErrorCodes_t addArgToArray(char* buf, size_t& pos, float arg);
//ErrorCodes_t addArgToArray(char* bur, size_t& pos, double arg);
/*
class ArrayEdits
{
  public:
  ErrorCodes_t addArgToArray(char* buf, size_t& pos, char* arg, size_t len);
  ErrorCodes_t addArgToArray(char* buf, size_t& pos, int arg);
  ErrorCodes_t addArgToArray(char* buf, size_t& pos, float arg);
  ErrorCodes_t addArgToArray(char* bur, size_t& pos, double arg);
};
*/



#endif