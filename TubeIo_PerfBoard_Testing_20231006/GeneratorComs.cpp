#include "GeneratorComs.h"


Generator::Generator()
{

}

ErrorCodes_t Generator::begin(ComType _ct)
{
  //Set the com type for the generator, here we will use the serial coms
  gen_ct = _ct;
  if(_ct == ComType::RS232)
  {
    //Set the serial port to the appropriate tx/rx lines | Start the serial coms
    Generator::gen_Serial = &Serial;
    Serial2.begin(HWS_BAUD);
    //gen_Serial->begin(HWS_BAUD);
  }
  return ErrorCodes_t::NoError;
}

ErrorCodes_t Generator::sendDataToGenerator(SpellmanCommand cmd, int arg1, int arg2, int arg3)
{
  char outBuffer[MAXBUFFERLEN];
  size_t pos = 0;
  outBuffer[pos++] = STX_G;
  addArgToArray(outBuffer, pos, (int)cmd);
  if(arg1 > -1) addArgToArray(outBuffer, pos, arg1);
  if(arg2 > -1) addArgToArray(outBuffer, pos, arg2);
  if(arg3 > -1) addArgToArray(outBuffer, pos, arg3);
  //now we need the checksum ONLY when using RS232
  if(gen_ct == ComType::RS232)
  {
    int cs = Generator::calculateCheckSum(outBuffer, pos);
    addArgToArray(outBuffer, pos, cs);
  }
  outBuffer[pos++] = ETX_G;
  Serial.write(outBuffer, pos);
  return ErrorCodes_t::NoError;
}

int Generator::calculateCheckSum(char* buf, size_t len)
{
  uint16_t _cs = 0;
  for(size_t i = 1; i < len; i++)
  {
    _cs += (uint16_t)buf[i];
  }
  //2-s complement
  _cs = ~_cs + 1; //~ is 1's compliment; then add 1
  //now set the 
  _cs &= 0x7F;
  _cs |= 0x40;
  return (int)_cs;
}

//Gets the character array from the generator, the TubeIO will parse it though.
char* Generator::recieveDataFromGenerator()
{
  char c = gen_Serial->read();
  if(c == STX_G)
  {
    char inData[MAXBUFFERLEN];
    gen_Serial->readBytesUntil(ETX_G, inData, MAXBUFFERLEN);
  }
  return NULL;
}

/*!
 *  @brief adds a character array to an existing buffer
 */
ErrorCodes_t Generator::addArgToArray(char* buf, size_t& pos, char* arg, size_t len)
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
ErrorCodes_t Generator::addArgToArray(char* buf, size_t& pos, int arg)
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
ErrorCodes_t Generator::addArgToArray(char* buf, size_t& pos, float arg)
{
  if(DECIMALSIZE + pos > MAXBUFFERLEN)return ErrorCodes_t::bufferOverFlow;
  char arg_c[DECIMALSIZE];
  dtostrf(arg, DECIMALSIZE, DECIMALPREC, arg_c);
  return addArgToArray(buf, pos, arg_c, DECIMALSIZE);
}


/*!
 *  @brief adds a double argument to an existing buffer
 */
 
ErrorCodes_t Generator::addArgToArray(char* buf, size_t& pos, double arg)
{
  if(DECIMALSIZE + pos > MAXBUFFERLEN)return ErrorCodes_t::bufferOverFlow;
  char arg_c[DECIMALSIZE];
  dtostrf(arg, DECIMALSIZE, DECIMALPREC, arg_c);
  return addArgToArray(buf, pos, arg_c, (size_t)DECIMALSIZE);
}

