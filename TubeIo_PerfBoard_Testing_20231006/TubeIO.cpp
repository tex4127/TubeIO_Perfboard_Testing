#include "TubeIO.h"

void test()
{
  Serial.println("Test");
}

//Sets all values for the structure to false
void setDataStruct(GeneratorStatus* _data)
{
  bool* _g = &_data->HVEnabled;
  bool dv = false;
  for(uint i = 0; i < 17; i++)
  {
    memcpy(_g, &dv, sizeof(dv));
    _g++;
  }
}

//Sets all values for the structure to false
void setDataStruct(GeneratorFaults* _data)
{
  bool* _f = &_data->FilamentSelect;
  bool dv = false;
  for(uint i = 0; i < 27; i++)
  {
    memcpy(_f, &dv, sizeof(dv));
    _f++;
  }
}

TubeIO::TubeIO()
{

}

void TubeIO::_blink()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void TubeIO::begin(ControlSerial_t ser)
{
  //Setup the LED PIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  gen = new Generator();
  gen->begin();
  if(ser == ControlSerial_t::PhysicalUSB){ serial = &Serial; }
  else {serial = &Serial1;}
  //Start both serial busses
  Serial.begin(HWS_BAUD);
  Serial1.begin(HWS_BAUD);
  //Set up all values
  //lets set the raw and real values for our Generator conversions | Defaults to a 160; 4 kW system
  conv_kV_SP.real = 160.0/4095.0;
  conv_kV_MSR.real = conv_kV_SP.real * 1.2;
  conv_mA_SP.real = 30.0/4095.0;
  conv_mA_MSR.real = conv_mA_SP.real * 1.2;
  conv_Fil_SP.real = 6.0/4095.0;
  conv_Fil_MSR.real = conv_Fil_SP.real; //same value
  //Set Default Values for both data structs
  setDataStruct(&gen->_status);
  setDataStruct(&gen->_faults);
  //Send a request to get scaling from the generator
  gen->sendDataToGenerator(SpellmanCommand::RequestScaling);
  delay(50); //wait for the gen to send the data
  char genData[MAXBUFFERLEN]; genData[0] = '\x23';
  gen->recieveDataFromGenerator(genData);
  if(genData[0] != '\x23') parseGeneratorBuffer(genData);
  TubeIO::resetValue(&coolantTemp);
  TubeIO::resetValue(&coolantFlow);
  TubeIO::resetValue(&TubemA);
  TubeIO::resetValue(&TubekV);
  TubeIO::resetValue(&FilamentCur);
  TubeIO::resetValue(&TubeVac);
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
  //Blink the LED 3 times to signal the system is setup
  for(uint i = 0; i < 6; i++)
  {
    _blink();
    delay(100);
  }
  //ensure that the LED is on before procedding
  digitalWrite(LED_BUILTIN, HIGH);
  
}

void TubeIO::resetValue(Value* v)
{
  v->real = 0.0;
  v->raw = 0;
}

void TubeIO::run()
{
  //send the command to request analogReadBacks, Status, or Faults
  _startTime = millis();
  if(!genRequestSent)
  {
    switch (cycleNumber)
    {
      case 9:
      gen->sendDataToGenerator(SpellmanCommand::RequestStatus);
      break;
      case 19:
      gen->sendDataToGenerator(SpellmanCommand::RequestFaults);
      break;
      default:
      gen->sendDataToGenerator(SpellmanCommand::RequestAnalogReadBacks);
      break;
    }
  }
  //read all the other peripherals
  TubeIO::readIFM();
  TubeIO::readLineVoltage();
  TubeIO::readTubeVacuum();
  do
  {
    char gen_data[MAXBUFFERLEN]; gen_data[0] = '\x23';
    gen->recieveDataFromGenerator(gen_data);
    if(gen_data[0] != '\x23') TubeIO::parseGeneratorBuffer(gen_data);
    HandleSerialData();
  } while(millis() - _startTime < _cycleTime);
  cycleNumber++;
  if(cycleNumber == 20){cycleNumber = 0; digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));}
  if(!TubeIO::_debug){TubeIO::sendData();}
  else{TubeIO::printData_f(_startTime);}
}
void TubeIO::readLineVoltage()
{
  
}

ErrorCodes_t TubeIO::sendData()
{
  return ErrorCodes_t::NoError;
}

ErrorCodes_t TubeIO::printData_f(ulong _st)
{
  serial->println("********** NEW DATA  **********");
  serial->println("********** TUBE DATA **********");
  serial->println("Tube kV:            " + String(TubekV.real, 2));
  serial->println("Tube mA:            " + String(TubemA.real, 2));
  serial->println("Filament Current:   " + String(FilamentCur.real,2));
  serial->println("Coolant Flow (GPM): " + String(coolantFlow.real,2));
  serial->println("Coolant Temp (C):   " + String(coolantTemp.real, 2));
  serial->println("Tube Vacuum(V):     " + String(TubeVac.raw));
  serial->println("Cycle Time (ms):    " + String(millis() - _st));
  serial->println("********** GEN FAULTS **********");
  serial->println("PS FAULT:           " + String(gen->_status.PSFault));
  serial->println("********** GEN STATUS **********");
  serial->println("HV Enabled:         " + String(gen->_status.HVEnabled));
  serial->println("Interlock1:         " + String(gen->_status.Interlock1));
  serial->println("Interlock2:         " + String(gen->_status.Interlock2));
  serial->println("Remote Mode:        " + String(gen->_status.RemoteMode));
  serial->println("FilamentEnabled:    " + String(gen->_status.FilamentEnabled));
  serial->println("Pre Warn:           " + String(gen->_status.PreWarn));
  serial->println("PS Ready:           " + String(gen->_status.PSReady));
  serial->println("Internal Interlock: " + String(gen->_status.InternalInterlock));
  return ErrorCodes_t::NoError;
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
    _f = &gen->_faults.FilamentSelect;
    for(uint i = 0; i < 27; i ++)
    {
      tok = strtok(NULL, ",");
      bool _bt = (bool)atoi(tok);
      memcpy(_f, &_bt, sizeof(_bt));
      _f++;
    }
    break;
    //******************************************************************************************************************* GENERATOR STATUS
    case SpellmanCommand::RequestStatus:
    _g = &gen->_status.HVEnabled;
    for(uint i = 0; i < 17; i++)
    {
      tok = strtok(NULL, ",");
      bool _bt = (bool)atoi(tok);
      memcpy((bool*)_g, &_bt, sizeof(_bt));
      _g++;
    }
    break;
    default:
    //do nothing with the command
    break;
  }
}


void TubeIO::HandleSerialData()
{
  char c = serial->read();
  if(c == STX_T)
  {
    //valid start character; lets do some stuff!
    char inData[MAXBUFFERLEN];
    serial->readBytesUntil(ETX_T, inData, MAXBUFFERLEN);
    char* tok = strtok(inData, ",");
    SerialCommand cmd = SerialCommand(atoi(tok));
    switch (cmd)
    {
      //********************************** Program kV
      //RAW
      case SerialCommand::setGenKV_raw:
      //generate a command to set the generator kv to the appropriate setpoint
      tok = strtok(NULL, ",");
      _arg1 = atoi(tok);
      gen->sendDataToGenerator(SpellmanCommand::ProgramkV, _arg1);
      break;
      //REAL
      case SerialCommand::setGenKV_real:
      tok = strtok(NULL, ",");
      _arg1 = (int)(atof(tok) / conv_kV_SP.real);
      gen->sendDataToGenerator(SpellmanCommand::ProgramkV, _arg1);
      break;
      //********************************** Program mA
      //RAW
      case SerialCommand::setGenmA_raw:
      tok = strtok(NULL, ",");
      _arg1 = atoi(tok);
      gen->sendDataToGenerator(SpellmanCommand::ProgrammA, _arg1);
      break;
      //REAL
      case SerialCommand::setGenmA_real:
      tok = strtok(NULL, ",");
      _arg1 = (int)(atof(tok)/conv_mA_SP.real);
      break;
      //********************************** Program Filament Limit
      //RAW
      case SerialCommand::setFilLim_raw:
      tok = strtok(NULL, ",");
      _arg1 = atoi(tok);
      gen->sendDataToGenerator(SpellmanCommand::ProgramFilamentLimit, _arg1);
      break;
      //REAL
      case SerialCommand::setFilLim_real:
      tok = strtok(NULL, ",");
      _arg1 = (int)(atof(tok) / conv_Fil_SP.real);
      gen->sendDataToGenerator(SpellmanCommand::ProgramFilamentLimit, _arg1);
      //********************************** Program Preheat Limit
      //RAW
      case SerialCommand::setFilPreheatLim_raw:
      tok = strtok(NULL, ",");
      _arg1 = atoi(tok);
      gen->sendDataToGenerator(SpellmanCommand::ProgramFilamentPreheatLimit, _arg1);
      break;
      //REAL
      case SerialCommand::setFilPreheatLim_real:
      tok = strtok(NULL, ",");
      _arg1 = (int)(atof(tok) / conv_Fil_SP.real);
      gen->sendDataToGenerator(SpellmanCommand::ProgramFilamentPreheatLimit, _arg1);
      break;
      //********************************** Enable Filament
      case SerialCommand::enableFilament:
      tok = strtok(NULL, ",");
      _arg1 = atoi(tok);
      if(_arg1 != 1) _arg1 = 0;
      gen->sendDataToGenerator(SpellmanCommand::FilamentControl, _arg1);
      break;
      //********************************** Enable HV
      case SerialCommand::enableHV:
      tok = strtok(NULL, ",");
      _arg1 = atoi(tok);
      if(_arg1 != 1) _arg1 = 0; //force the value to be 1 or 0
      if(_arg1 == 1) {digitalWrite(VFDPOWERPIN, HIGH);} //force the pin to go high for the VFD
      gen->sendDataToGenerator(SpellmanCommand::ProgramHVOn, _arg1);
      break;
      //********************************** WATER PUMP
      case SerialCommand::enableWaterPump:
      digitalWrite(VFDPOWERPIN, !digitalRead(VFDPOWERPIN));
      break;
      //********************************** VFD INCREASE
      case SerialCommand::increaseVFD:
      if(_timer_VFD._timerCV == 0)
      {
        //set timer for VFD to be 100 ms then every loop we will check the value for timers
        _timer_VFD._timerCV = 100; //this will count down, so it will increase for ~100 ms
        digitalWrite(VFDFREQINCPIN, HIGH);
      }
      break;
      //********************************** VFD DECREASE
      case SerialCommand::decreaseVFD:
      if(_timer_VFD._timerCV == 0)
      {
        _timer_VFD._timerCV = 100;
        digitalWrite(VFDFREQDECPIN, HIGH);
      }
      break;
      //********************************** RESERVOIR HEATER
      case SerialCommand::enableHeater:
      digitalWrite(RESERVOIRHEATERPIN, !digitalRead(RESERVOIRHEATERPIN));
      break;
      //********************************** BYPASS VALVE
      case SerialCommand::enableBypass:
      digitalWrite(BYPASSVALVEPIN, !digitalRead(BYPASSVALVEPIN));
      break;
      //********************************** K8 Contactor
      case SerialCommand::enableK8Contactor:
      digitalWrite(K8CONTACTOR, !digitalRead(K8CONTACTOR));
      K8Contact_Engaged = (bool)digitalRead(K8CONTACTOR);
      break;
      //********************************** GEN AUX CONTACTOR
      case SerialCommand::enableGenAuxContactor:
      digitalWrite(GENAUXCONTACTOR, !digitalRead(GENAUXCONTACTOR));
      GenAuxContact_Engaged = digitalRead(GENAUXCONTACTOR);
      break;
      //********************************** PERIPHERAL CONTACTOR
      case SerialCommand::enablePeripheralContactor:
      digitalWrite(PERIPHERALCONTACTOR, !digitalRead(PERIPHERALCONTACTOR));
      PeripheralContact_Engaged = digitalRead(PERIPHERALCONTACTOR);
      break;
      //********************************** GET GEN SCALING
      case SerialCommand::getScaling:
      //only send the scaling request if HV is not enabled
      if(!gen->_status.HVEnabled)
      {
        gen->sendDataToGenerator(SpellmanCommand::RequestScaling);
        genRequestSent = true;
      }
      break;
      //********************************** BUZZER TEST
      case SerialCommand::testBuzzer:
      digitalWrite(BUZZERPIN, !digitalRead(BUZZERPIN));
      break;
      //********************************** RESET GEN FAULTS
      case SerialCommand::resetGenFaults:
      gen->sendDataToGenerator(SpellmanCommand::ResetFaluts);
      break;
      default:
      //Maybe do something to signal a bad command? add an LED?
      break;
    }
  }
}

void TubeIO::readTubeVacuum()
{
  TubeVac.raw = analogRead(SPCPIN);
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