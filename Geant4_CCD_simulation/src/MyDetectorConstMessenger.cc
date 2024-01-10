#include "MyDetectorConstMessenger.hh"
#include "MyDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4ios.hh"

MyDetectorConstMessenger::MyDetectorConstMessenger(MyDetectorConstruction* fDet)
:fDetector(fDet)
{
  fMydetDirectory = new G4UIdirectory("/mydet/");
  fMydetDirectory->SetGuidance("My detector setup control commands.");

  fBuildNanosphereCMD = new G4UIcmdWithABool("/mydet/BuildNanosphere", this);
  fBuildNanosphereCMD->SetGuidance("Control whether to build Si02 nanosphere");
  fBuildNanosphereCMD->SetDefaultValue(true);
    
  fBuildSiPanelsCMD = new G4UIcmdWithABool("/mydet/BuildSiPanels", this);
  fBuildSiPanelsCMD->SetGuidance("Control whether to build Si CCD panels");
  fBuildSiPanelsCMD->SetDefaultValue(false);

  fSetSiO2sphereRadCMD = new G4UIcmdWithADoubleAndUnit("/mydet/SiO2sphereRadius",this);
  fSetSiO2sphereRadCMD->SetGuidance("Radius SiO2 nanosphere");
  fSetSiO2sphereRadCMD->SetParameterName("SiO2sphereRadius", true);
  fSetSiO2sphereRadCMD->SetRange("SiO2sphereRadius>=0. && SiO2sphereRadius<10000.");
  fSetSiO2sphereRadCMD->SetDefaultValue(100.);
  fSetSiO2sphereRadCMD->SetDefaultUnit("nm");

  fSetSiXYsizeCMD = new G4UIcmdWithADoubleAndUnit("/mydet/SiXYsize",this);
  fSetSiXYsizeCMD->SetGuidance("Size of Si CCD Square face");
  fSetSiXYsizeCMD->SetParameterName("SiXYsize", true);
  fSetSiXYsizeCMD->SetRange("SiXYsize>=0. && SiXYsize<4.");
  fSetSiXYsizeCMD->SetDefaultValue(1.2);
  fSetSiXYsizeCMD->SetDefaultUnit("cm");

  fSetSiThicknessCMD = new G4UIcmdWithADoubleAndUnit("/mydet/SiThickness",this);
  fSetSiThicknessCMD->SetGuidance("Thickness of Si CCD");
  fSetSiThicknessCMD->SetParameterName("SiThickness", true);
  fSetSiThicknessCMD->SetRange("SiThickness>=0. && SiThickness<1000.");
  fSetSiThicknessCMD->SetDefaultValue(250.);
  fSetSiThicknessCMD->SetDefaultUnit("um");

  fSetSourceOffsetCMD = new G4UIcmdWithADoubleAndUnit("/mydet/SourceOffset",this);
  fSetSourceOffsetCMD->SetGuidance("Offset of Si CCD Box from source");
  fSetSourceOffsetCMD->SetParameterName("SourceOffset", true);
  fSetSourceOffsetCMD->SetRange("SourceOffset>=-0.5 && SourceOffset<=0.5");
  fSetSourceOffsetCMD->SetDefaultValue(-0.2);
  fSetSourceOffsetCMD->SetDefaultUnit("cm");

  fSetMaxStepLimitCMD = new G4UIcmdWithADoubleAndUnit("/mydet/MaxStepLimit",this);
  fSetMaxStepLimitCMD->SetGuidance("Max particle step size before recording step");
  fSetMaxStepLimitCMD->SetParameterName("MaxStepLimit", true);
  fSetMaxStepLimitCMD->SetRange("MaxStepLimit>=1.0 && MaxStepLimit<=1000000000.");
  fSetMaxStepLimitCMD->SetDefaultValue(10.0);
  fSetMaxStepLimitCMD->SetDefaultUnit("nm");

}

MyDetectorConstMessenger::~MyDetectorConstMessenger()
{
  delete fMydetDirectory;
  delete fBuildNanosphereCMD;
  delete fBuildSiPanelsCMD;
  delete fSetSiXYsizeCMD;
  delete fSetSiO2sphereRadCMD;
  delete fSetSiThicknessCMD;
  delete fSetSourceOffsetCMD;
  delete fSetMaxStepLimitCMD;
}

void MyDetectorConstMessenger::SetNewValue(G4UIcommand * command, G4String newValue)
{
  if (command == fBuildNanosphereCMD){
    fDetector->SetBuildNanosphere(fBuildNanosphereCMD->GetNewBoolValue(newValue));
  }
  else if (command == fBuildSiPanelsCMD){
    fDetector->SetBuildSiPanels(fBuildSiPanelsCMD->GetNewBoolValue(newValue));
  }
  else if (command == fSetSiO2sphereRadCMD){
    fDetector->SetSiO2sphereRad(fSetSiO2sphereRadCMD->GetNewDoubleValue(newValue));
  }
  else if ( command == fSetSiXYsizeCMD ){
    fDetector->SetSiXYsize(fSetSiXYsizeCMD->GetNewDoubleValue(newValue));
  }
  else if ( command == fSetSiThicknessCMD ){
    fDetector->SetSiThickness(fSetSiThicknessCMD->GetNewDoubleValue(newValue));
  }
  else if ( command == fSetSourceOffsetCMD ){
    fDetector->SetSourceOffset(fSetSourceOffsetCMD->GetNewDoubleValue(newValue));
  }
  else if ( command == fSetMaxStepLimitCMD ){
    fDetector->SetMaxStepLimit(fSetMaxStepLimitCMD->GetNewDoubleValue(newValue));
  }
  // Done reading macro commands
  else{
    std::cout << "Macro Command [" << command << "] Doesn't Exist" << std::endl;
  }

}

G4String MyDetectorConstMessenger::GetCurrentValue(G4UIcommand * command)
{
  G4String cv;
  if ( command==fSetSiXYsizeCMD ){
    cv = fSetSiXYsizeCMD->ConvertToString(fDetector->GetSiXYsize(),"cm"); 
  }

  return cv;
}