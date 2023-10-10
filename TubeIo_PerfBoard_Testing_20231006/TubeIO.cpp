#include "TubeIO.h"

TubeIO::TubeIO()
{

}

void TubeIO::begin(ControlSerial_t ser)
{
  _debug = false;
  if(ser == ControlSerial_t::PhysicalUSB){ serial = &Serial; }
  else {serial = &Serial1;}
  //Set up all values
  //Send a request to get scaling from the generator
  gen->sendDataToGenerator(SpellmanCommand::RequestScaling);
  //lets set the raw and real values for our Generator conversions
  conv_tube
  TubeIO::resetValue(&coolantTemp);
  TubeIO::resetValue(&coolantFlow);
  TubeIO::resetValue(&TubemA);
  TubeIO::resetValue(&TubekV);
  TubeIO::resetValue(&FilamentCur);
  //Setup Pin Modes for Digital Controls
  pinMode(VFDPOWERPIN, OUTPUT);
  digitalWrite(VFDPOWERPIN, LOW);
  pinMode(VFDFREQINCPIN, OUTPUT);
  digitalWrite(VFDFREQINCPIN, LOW);
  pinMode(VFDFREQDECPIN, OUTPUT);
  digitalWrite(VFDFREQDECPIN, LOW);
  pinMode(RESERVOIRHEATERPIN, OUTPUT);
  digitalWrite(RESERVOIRHEATERPIN, LOW);
  pinMode(BYPASSVALVEPIN, OUTPUT);
  digitalWrite(BYPASSVALVEPIN, LOW);
}

void TubeIO::resetValue(Value* v)
{
  v->real = 0.0;
  v->raw = 0;
}

void TubeIO::run()
{
  if(!TubeIO::_debug){TubeIO::sendData();}
  else{TubeIO::printData_f();}
}

void TubeIO::readIFM()
{
  //read the value at the pins 
  TubeIO::coolantFlow.raw = analogRead(IFMFLOWPIN);
  TubeIO::coolantTemp.raw = analogRead(IFMTEMPPIN);
  //Now convert the raw counts to real values
  TubeIO::coolantTemp.real = 0.242 * TubeIO::coolantTemp.raw - 45;
  TubeIO::coolantFlow.real = 0.03373 * TubeIO::coolantFlow.raw - 6.275;
}

//Changes state of the buzzer (on/off)
void TubeIO::soundBuzzer()
{
  digitalWrite(BUZZERPIN, !digitalRead(BUZZERPIN));
}

//Changes state of the main contactor (K8)
void TubeIO::engageK8Contactor()
{
  digitalWrite(K8CONTACTOR, !digitalRead(K8CONTACTOR));
}

void TubeIO::parseGeneratorBuffer(char* data)
{
  //all data will start from <STX> but not include the STX and extend to <ETX>
  //Tokenize the char array
  char* tok = strtok(data, ",");
  //get the command type
  SpellmanCommand _cmd = SpellmanCommand(atoi(tok));
  switch (_cmd)
  {
    //******************************************************************************************************************* ANALOG READBACKS
    case SpellmanCommand::RequestAnalogReadBacks:
    //kV is first
    tok = strtok(NULL, ",");
    TubekV.raw = atoi(tok);
    TubekV.real = TubekV.raw * conv_kV_MSR.real; //this has already been calculated under Request Scaling | Default is 160 kV
    //Tube mA next
    tok = strtok(NULL, ",");
    TubemA.raw = atoi(tok);
    TubemA.real = TubemA.raw * conv_mA_MSR.real; //this has already been calculated under Request Scaling | Default is 30 mA
    //Filament Feedback next
    tok = strtok(NULL, ",");
    FilamentCur.raw = atoi(tok);
    break;
    //******************************************************************************************************************* REQUEST SCALING
    case SpellmanCommand::RequestScaling:
    //kV first, returns int value
    tok = strtok(NULL, ",");
    conv_kV_SP.real = (float)(atoi(tok))/4095;  //the 4095 is coming from the 12 bit value/ this will propogate through out the rest of the values. 160/4095 ~0.039 | 192/4095 ~0.0468
    conv_kV_MSR.real = 1.2 * conv_kV_SP.real;
    //mA is next
    tok = strtok(NULL, ",");
    conv_mA_SP.real = (float)(atoi(tok))/4095;  //same as above regarding the 4095; 30
    conv_mA_MSR.real = 1.2 * conv_mA_SP.real;
    //next value is supply plarity. We are always limited by 6 A on filament current
    break;
    //******************************************************************************************************************* GENERATOR FAULTS
    case SpellmanCommand::RequestFaults:
    //Lets parse some faults!
    _f = &gen->_faults;
    for(uint i = 0; i < 27; i ++)
    {
      tok = strtok(NULL, ",");
      bool _bt = (bool)atoi(tok);
      memcpy((bool*)_f, &_bt, sizeof(_bt));
      _f += sizeof(bool*);
    }
    break;
    //******************************************************************************************************************* GENERATOR STATUS
    case SpellmanCommand::RequestStatus:
    _g = &gen->_status;
    for(uint i = 0; i < 17; i++)
    {
      tok = strtok(NULL, ",");
      bool _bt = (bool)atoi(tok);
      memcpy((bool*)_g, &_bt, sizeof(_bt));
      _g += sizeof(bool*);
    }
    break;
    default:
    //do nothing with the command
    break;
  }
}

/*
 EXTRA STUFF FOR LATER
tok = strtok(NULL, ",");
    gen->_faults.FilamentSelect = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.OverTemp = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.OverVolt = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.UnderVolt = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.OverCurrent = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.UnderCurrent = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.OverTempAnnode = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.OverTempCathode = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.InverterAnnode = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.FilamentFeedback = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.AnnodeArc = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.CathodeArc = (bool)atoi(tok);
    tok = strtok(NULL, ",");
    gen->_faults.
*/