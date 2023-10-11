#ifndef __TUBEIO_HH__
#define __TUBEIO_HH__

#include "GeneratorComs.h"


#ifndef IFMFLOWPIN
#define IFMFLOWPIN 18
#endif

#ifndef IFMTEMPPIN
#define IFMTEMPPIN 19
#endif

#ifndef SPCPIN
#define SPCPIN 41
#endif

#ifndef SPCHIPOTPIN
#define SPCHIPOTPIN 33 // was 40
#endif

#ifndef HVENABLEDPIN
#define HVENABLEDPIN 39
#endif

#ifndef VFDPOWERPIN
#define VFDPOWERPIN 2
#endif

#ifndef VFDFREQINCPIN
#define VFDFREQINCPIN 3
#endif

#ifndef VFDFREQDECPIN
#define VFDFREQDECPIN 4
#endif

#ifndef RESERVOIRHEATERPIN
#define RESERVOIRHEATERPIN 5
#endif

#ifndef BYPASSVALVEPIN
#define BYPASSVALVEPIN 6
#endif

#ifndef K8CONTACTOR
#define K8CONTACTOR 30
#endif

#ifndef GENAUXCONTACTOR
#define GENAUXCONTACTOR 31
#endif

#ifndef PERIPHERALCONTACTOR
#define PERIPHERALCONTACTOR 32
#endif

#ifndef BUZZERPIN
#define BUZZERPIN 9
#endif

#ifndef LINEVOLTAGEPIN
#define LINEVOLTAGEPIN 23
#endif

typedef enum
{
  //Generator change settings
  setGenKV_raw = 10,
  setGenKV_real,
  setGenmA_raw,
  setGenmA_real,
  setFilLim_raw,
  setFilLim_real,
  setFilPreheatLim_raw,
  setFilPreheatLim_real,
  enableFilament,
  enableHV,
  getScaling,
  //Coolant Controls
  enableWaterPump = 21,
  increaseVFD,
  decreaseVFD,
  enableBypass,
  enableHeater,
  enableHiPot,
  //Peripheral Controls
  enableK8Contactor = 30,
  enableGenAuxContactor,
  enablePeripheralContactor,
  resetGenFaults,
  testBuzzer
} SerialCommand;


typedef enum
{
  PhysicalUSB = 0,
  serial1
} ControlSerial_t;
/*
typedef enum
{
  idle = 0,
  run
} mode_t;
*/


class TubeIO
{
  public:
  TubeIO();
  void begin(ControlSerial_t ser = ControlSerial_t::PhysicalUSB);
  uint16_t get_raw(Value* val); //generic getter for a raw value
  float get_real(Value* val);
  void run();
  bool _debug = false;
  void HandleSerialData();
  private:
  void _blink();
  void resetValue(Value* v);
  Stream* serial;
  Value coolantTemp;
  Value coolantFlow;
  Value TubemA;
  Value TubekV;
  Value FilamentCur;
  Value TubeVac;
  bool SPC_HVE;
  Value conv_mA_SP;
  Value conv_kV_SP;
  Value conv_Fil_SP;
  Value conv_mA_MSR;
  Value conv_kV_MSR;
  Value conv_Fil_MSR;
  Value LineVoltage;
  void soundBuzzer();
  void readIFM();
  void readLineVoltage();
  void readTubeVacuum();
  void engageK8Contactor();
  ErrorCodes_t setTubemA(float arg);
  ErrorCodes_t setTubekV(float arg);
  ErrorCodes_t setPreheatLim(float arg);
  void EnableFilament();
  void EnableHV();
  ErrorCodes_t sendData();
  ErrorCodes_t printData_f(ulong _st = 0);
  TimerValue _timer_Buzzer;
  TimerValue _timer_VFD;
  ulong _startTime;
  ulong _cycleTime = 50;
  bool K8Contact_Engaged = false;
  bool GenAuxContact_Engaged = false;
  bool PeripheralContact_Engaged = false;
  Generator* gen;
  bool* _f;
  bool* _g;
  bool genRequestSent = false;
  uint8_t cycleNumber = 0;
  void requestTubeAnalogRB();
  void parseGeneratorBuffer(char* data);
  //Temp vars stored lower on stack for storing generator command arguments
  int _arg1;
  int _arg2;
  int _arg3;
};

#endif