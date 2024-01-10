#ifndef MYDETECTORCONSTRUCTION_HH
#define MYDETECTORCONSTRUCTION_HH

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"

class G4VPhysicalVolume;
class MyDetectorConstMessenger;

class MyDetectorConstruction : public G4VUserDetectorConstruction 
{
public:
    MyDetectorConstruction();
    virtual ~MyDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    void SetBuildNanosphere(G4bool doBuildNanosphere);
    inline G4bool GetBuildNanosphere(){ return fDoBuildNanosphere; }

    void SetBuildSiPanels(G4bool doBuildSiPanels);
    inline G4bool GetBuildSiPanels(){ return fDoBuildSiPanels; }

    void SetMaxStepLimit(G4double MaxStepLimit);
    inline G4double GetMaxStepLimit(){ return fMaxStepLimit; }

    void SetSiO2sphereRad(G4double SiO2sphereRad_nm);
    inline G4double GetSiO2sphereRad(){ return fSiO2sphereRad_nm; }

    void SetSiXYsize(G4double SiXYsize_cm);
    inline G4double GetSiXYsize(){ return fSiXYsize_cm; }

    void SetSiThickness(G4double SiThickness_um);
    inline G4double GetSiThickness(){ return fSiThickness_um; }

    void SetSourceOffset(G4double SourceOffset_cm);
    inline G4double GetSourceOffset(){ return fSourceOffset_cm; }

private:
    MyDetectorConstMessenger* fMessenger;

    // G4double fMaxStepSize;
    G4LogicalVolume* worldLogVol;

    // control whether to build stuff
    G4bool fDoBuildNanosphere;
    G4bool fDoBuildSiPanels;

    // Geant4 max step size before recording event
    G4double fMaxStepLimit;

    // Detector params
    G4double fSiO2sphereRad_nm;
    G4double fSiXYsize_cm;
    G4double fSiThickness_um;
    G4double fSourceOffset_cm;
};

#endif
