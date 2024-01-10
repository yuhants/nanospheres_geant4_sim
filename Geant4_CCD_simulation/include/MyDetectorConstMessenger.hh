#ifndef MyDetectorConstMessenger_h
#define MyDetectorConstMessenger_h 1

class MyDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;

#include "G4UImessenger.hh"
#include "globals.hh"

class MyDetectorConstMessenger: public G4UImessenger
{
  public:
    MyDetectorConstMessenger(MyDetectorConstruction* fDet);
    ~MyDetectorConstMessenger();

    virtual void SetNewValue(G4UIcommand * command, G4String newValues);
    virtual G4String GetCurrentValue(G4UIcommand * command);

  private:
    MyDetectorConstruction * fDetector;

    G4UIdirectory *             fMydetDirectory;
    G4UIcmdWithABool*           fBuildNanosphereCMD;
    G4UIcmdWithABool*           fBuildSiPanelsCMD;
    G4UIcmdWithADoubleAndUnit*  fSetSiO2sphereRadCMD;
    G4UIcmdWithADoubleAndUnit*  fSetSiXYsizeCMD;
    G4UIcmdWithADoubleAndUnit*  fSetSiThicknessCMD;
    G4UIcmdWithADoubleAndUnit*  fSetSourceOffsetCMD;
    G4UIcmdWithADoubleAndUnit*  fSetMaxStepLimitCMD;
    
};

#endif


