#ifndef __GENERATORCOMS_HH__
#define __GENERATORCOMS_HH__

#include"ErrorCodes.h"
#include"genutil.h"

//all we care about monitoring are the tube kV, mA and filament current

typedef struct
{
  bool HVEnabled;       //1 = enabled
  bool Interlock1;      //1 = closed
  bool Interlock2;      //1 = closed
  bool ECRMode;         //1 = active
  bool PSFault;         //1 = falut
  bool RemoteMode;      //0 = remote; 1 = local
  bool FilamentEnabled; //1 = enabled
  bool FilamentType;    //1 = Large
  bool PreWarn;         //1 = xrays eminent
  bool largeFilConf;
  bool smallFilConf;
  bool R1;
  bool R2;
  bool R3;
  bool R4;
  bool PSReady;         //1 = ready
  bool InternalInterlock; //1 = closed
}GeneratorStatus;

typedef struct
{
  bool FilamentSelect;
  bool OverTemp;
  bool OverVolt;
  bool UnderVolt;
  bool OverCurrent;
  bool UnderCurrent;
  bool OverTempAnnode;
  bool OverTempCathode;
  bool InverterAnnode;
  bool FilamentFeedback;
  bool AnnodeArc;
  bool CathodeArc;
  bool CableConnectAnnode;
  bool CableConnectCahtode;
  bool ACLineMonitorAnnode;
  bool ACLineMonitorCathode;
  bool DCRailAnnode;
  bool DCRailCathode;
  bool LVPSN15;
  bool LVPSP15;
  bool WatchDog;
  bool BoardOverTemp;
  bool OverPower;
  bool KVDiff;
  bool mADiff;
  bool InverterNotReady;
}GeneratorFaults;

/*!
 *  @brief Commands enumeration structure to make control types of commands that can be sent to the generator
 */
typedef enum 
{
  ProgramkV = 10,
  ProgrammA,
  ProgramFilamentLimit,
  ProgramFilamentPreheatLimit,
  RequestkVSetPoint,
  RequestmASetPoint,
  RequestFilamentLimitSetPoint,
  RequestFilamentPreheatSetPoint,
  RequestAnalogReadBacks = 19,
  RequestHVOnHours = 21,
  RequestStatus,
  RequestSoftwareVersion,
  RequestModelNumber = 26,
  RequestUserConfig,
  RequestScaling,
  ResetHVOnHours = 30,
  ResetFaluts,
  SetFilamentSize,
  RequestPowerLimits = 38,
  RequestFPGARev = 43,
  RequestkVMonitor = 60,
  RequestNeg15LVPS = 65,
  RequestFaults = 68,
  RequestSystemVoltages,
  FilamentControl,
  XRVControllerPresent,
  ProgramPowerLimits = 97,
  ProgramHVOn,
  LocalRemoteModeSet
}SpellmanCommand;


class Generator
{
  public:
  Generator();
  ErrorCodes_t begin(ComType _ct = ComType::RS232);
  ErrorCodes_t sendDataToGenerator(SpellmanCommand cmd, int arg1 = -1, int arg2 = -1, int arg3 = -1);
  char* recieveDataFromGenerator();
  GeneratorStatus _status;
  GeneratorFaults _faults;
  ErrorCodes_t addArgToArray(char* buf, size_t& pos, char* arg, size_t len);
  ErrorCodes_t addArgToArray(char* buf, size_t& pos, int arg);
  ErrorCodes_t addArgToArray(char* buf, size_t& pos, float arg);
  ErrorCodes_t addArgToArray(char* bur, size_t& pos, double arg);
  private:
  int calculateCheckSum(char* buf, size_t len);
  ComType gen_ct;
  Stream* gen_Serial;
  
};

#endif