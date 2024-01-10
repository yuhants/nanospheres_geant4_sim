#ifndef MYPRIMARYGENERATORACTION_HH
#define MYPRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include <vector>
#include "TH1.h"
#include "TGraph.h"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"


class G4Event;
class MyPrimaryGeneratorMessenger;

class MyPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    static MyPrimaryGeneratorAction* Instance();

    virtual void GeneratePrimaries(G4Event* anEvent);
    void SetIonZA(G4int Z, G4int A);
    void SetParticleAngles(const G4ThreeVector& angles);

    G4String GetParticleInfo();
    G4int GetEventID() { return fEventID; }
    G4int GetPrimaryKind() { return fPrimaryKind; }
    G4double GetPrimaryEnergy() { return fPrimaryEnergy; }

    enum generatorType
    {
        Particle,
        Ion,
        Spectrum,
        Tritium
    };
    void setGeneratorType(generatorType whichGen);
    void setParticleType(G4String whichParticle);
    void SetParticleEnergyAndUnit(double whatEnergy, std::string whichUnit);
    void setEnergySpectrum(std::string spectrumFilePath);
    enum positionType
    {
        RandomSphere,
        Point
    };
    void setPositionType(positionType whichPositionType, double radius, double x, double y, double z, std::string unit);
    G4ThreeVector setParticlePosition(positionType whichPos);

private:
    static MyPrimaryGeneratorAction* instance;
    
    MyPrimaryGeneratorAction();
    virtual ~MyPrimaryGeneratorAction();

    MyPrimaryGeneratorAction(const MyPrimaryGeneratorAction&) = delete;
    MyPrimaryGeneratorAction& operator=(const MyPrimaryGeneratorAction&) = delete;

    G4double GetTritiumElectronEnergy();

    G4ParticleTable* pTable;

    G4ParticleGun* fParticleGun;
    G4GeneralParticleSource* fGeneralParticleSource;
    G4int fEventID;
    MyPrimaryGeneratorMessenger* messenger;
    G4bool useSpecifiedAngles;
    G4ThreeVector fParticleAngles;
    G4ThreeVector fDirection;
    
    std::string filePrefix;
    G4String fParticleName;
    G4String fParticleEnergyStr;
    G4double fPrimaryEnergy;
    G4int fPrimaryKind;
    G4ParticleDefinition* fIon;

    generatorType fGeneratorType;
    G4ParticleDefinition* fParticleDef;
    G4String fParticleType;
    G4double fParticleEnergy;
    G4String fGeneratorName;

    positionType fPositionType;
    G4double fPrimaryParticleRadius;
    G4double fPrimaryParticleX, fPrimaryParticleY, fPrimaryParticleZ;

    TGraph * gSpectrum;
    TH1D * hSpectrum;

};

#endif
