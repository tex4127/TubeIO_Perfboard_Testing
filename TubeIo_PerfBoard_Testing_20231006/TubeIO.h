#ifndef __TUBEIO_HH__
#define __TUBEIO_HH__

#include "util.h"
#include"GeneratorComs.h"


#ifndef IFMFLOWPIN
#define IFMFLOWPIN 18
#endif

#ifndef IFMTEMPPIN
#define IFMTEMPPIN 19
#endif

#ifndef SPCPIN
#define SPCPIN 41
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
#define K8CONTACTOR 9
#endif

#ifndef BUZZERPIN
#define BUZZERPIN 30
#endif

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
  bool _debug;
  private:
  void resetValue(Value* v);
  Stream* serial;
  Value coolantTemp;
  Value coolantFlow;
  Value TubemA;
  Value TubekV;
  Value FilamentCur;
  Value conv_mA_SP;
  Value conv_kV_SP;
  Value conv_Fil_SP;
  Value conv_mA_MSR;
  Value conv_kV_MSR;
  Value conv_Fil_MSR;
  void soundBuzzer();
  void readIFM();
  void readLineVoltage();
  void engageK8Contactor();
  ErrorCodes_t setTubemA(float arg);
  ErrorCodes_t setTubekV(float arg);
  ErrorCodes_t setPreheatLim(float arg);
  void EnableFilament();
  void EnableHV();
  ErrorCodes_t sendData();
  ErrorCodes_t printData_f();
  TimerValue _timer_Buzzer;
  TimerValue _timer_VFD;
  Generator* gen;
  GeneratorFaults* _f;
  GeneratorStatus* _g;
  void requestTubeAnalogRB();
  void parseGeneratorBuffer(char* data);
  bool _b; //throw away value for handling boolean memcpy
};

#endif