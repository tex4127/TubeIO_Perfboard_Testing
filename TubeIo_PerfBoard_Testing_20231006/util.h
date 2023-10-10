#ifndef __UTIL_HH__
#define __UTIL_HH__

#include "ErrorCodes.h"
#include "Arduino.h"

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

/*!
 *  @brief adds a character array to an existing buffer
 */
ErrorCodes_t addArgToArray(char* buf, size_t& pos, char* arg, size_t len)
{
  if(len + pos > MAXBUFFERLEN) return ErrorCodes_t::bufferOverFlow;
  for(size_t i = 0; i < len; i++)
  {
    buf[pos++] = arg[i];
  }
  buf[pos++] = 0x2C; //always add the comma here, NOT during other overloads that are recursive to this
  return ErrorCodes_t::NoError;
}

/*!
 *  @brief adds an integer value to an existing buffer
 */
ErrorCodes_t addArgToArray(char* buf, size_t& pos, int arg)
{
  if(arg > 9999) return ErrorCodes_t::argumentOutOfRange;
  if(4 + pos > MAXBUFFERLEN) return ErrorCodes_t::bufferOverFlow;
  //No errors, proceed with concatnation
  int arg_c = arg;
  if(arg > 1000) { buf[pos++] = (arg_c/1000) + 48; arg_c %= 1000;}
  if(arg > 100)  { buf[pos++] = (arg_c/100) + 48;  arg_c %= 100;}
  if(arg > 10)   { buf[pos++] = (arg_c/10) + 48;   arg_c %= 10;}
  buf[pos++] = 48 + arg_c;
  buf[pos++] = 0x2C;  //add the comma here because we are not refering to another overload which does
  return ErrorCodes_t::NoError;
}

/*!
 *  @brief adds a float argument to an existing buffer
 */
ErrorCodes_t addArgToArray(char* buf, size_t& pos, float arg)
{
  if(DECIMALSIZE + pos > MAXBUFFERLEN)return ErrorCodes_t::bufferOverFlow;
  char arg_c[DECIMALSIZE];
  dtostrf(arg, DECIMALSIZE, DECIMALPREC, arg_c);
  return addArgToArray(buf, pos, arg_c, DECIMALSIZE);
}

/*!
 *  @brief adds a double argument to an existing buffer
 */
ErrorCodes_t addArgToArray(char* buf, size_t& pos, double arg)
{
  if(DECIMALSIZE + pos > MAXBUFFERLEN)return ErrorCodes_t::bufferOverFlow;
  char arg_c[DECIMALSIZE];
  dtostrf(arg, DECIMALSIZE, DECIMALPREC, arg_c);
  return addArgToArray(buf, pos, arg_c, (size_t)DECIMALSIZE);
}




#endif