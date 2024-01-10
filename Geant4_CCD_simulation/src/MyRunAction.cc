#include "MyRunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "MyPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"

MyRunAction::MyRunAction(MyPrimaryGeneratorAction* generatorAction) 
    : G4UserRunAction(), fGeneratorAction(generatorAction) {

    fTree = new TTree("Events", "Event");

    // Primaries branch
    fTree->Branch("primaries.particleName", &primaryName);
    fTree->Branch("primaries.volumeName", &primaryVolumeName);
    fTree->Branch("primaries.pid", &primaryPid);
    fTree->Branch("primaries.time_ns", &primaryTime_ns);
    fTree->Branch("primaries.energy_keV", &primaryEnergy_keV);
    fTree->Branch("primaries.totalEnergy_keV", &primaryTotalEnergy_keV);
    fTree->Branch("primaries.totalMomentum_keV", &primaryTotalMomentum_keV);
    fTree->Branch("primaries.position_X_mm", &primaryPosition_X_mm);
    fTree->Branch("primaries.position_Y_mm", &primaryPosition_Y_mm);
    fTree->Branch("primaries.position_Z_mm", &primaryPosition_Z_mm);
    fTree->Branch("primaries.direction_X", &primaryDirection_X);
    fTree->Branch("primaries.direction_Y", &primaryDirection_Y);
    fTree->Branch("primaries.direction_Z", &primaryDirection_Z);

    // Exit info
    fTree->Branch("exits.trackID", &fExitTrackID);
    fTree->Branch("exits.parentID", &fExitParentID);
    fTree->Branch("exits.particleName", &fExitParticleName);
    fTree->Branch("exits.creationProcess", &fExitParticleCreatorProcess);
    fTree->Branch("exits.particleMass_keV", &fExitParticleMassKeV);
    fTree->Branch("exits.particleCharge", &fExitParticleCharge);
    fTree->Branch("exits.postStepEnergy_keV", &fExitEnergy);
    fTree->Branch("exits.postStepMomentumX_keV", &fExitMomentumX);
    fTree->Branch("exits.postStepMomentumY_keV", &fExitMomentumY);
    fTree->Branch("exits.postStepMomentumZ_keV", &fExitMomentumZ);
    fTree->Branch("exits.postStepPositionX_um", &fExitPositionX);
    fTree->Branch("exits.postStepPositionY_um", &fExitPositionY);
    fTree->Branch("exits.postStepPositionZ_um", &fExitPositionZ);

    // Tracks+Steps branch
    fTree->Branch("tracks.trackID", &fTrackID);
    fTree->Branch("tracks.stepNumber", &fTrackStepNum);
    fTree->Branch("tracks.parentID", &fTrackParentID);
    fTree->Branch("tracks.detectorName", &fDetectorName);
    fTree->Branch("tracks.detectorID", &fDetectorID);
    fTree->Branch("tracks.particleName", &fParticleName);
    fTree->Branch("tracks.particleID", &fParticleID);
    fTree->Branch("tracks.creationProcess", &fParticleCreatorProcesses);
    fTree->Branch("tracks.particleMass_keV", &fParticleMass);
    fTree->Branch("tracks.stepTime_ns", &fStepTime_ns);
    fTree->Branch("tracks.preStepEnergy_keV", &fPreStepEnergy);
    fTree->Branch("tracks.energyDeposition_keV", &fEnergyDep);
    fTree->Branch("tracks.stepLength_um", &fStepLength);
    fTree->Branch("tracks.preStepMomentumX_keV", &fPreStepMomentumX);
    fTree->Branch("tracks.preStepMomentumY_keV", &fPreStepMomentumY);
    fTree->Branch("tracks.preStepMomentumZ_keV", &fPreStepMomentumZ);
    fTree->Branch("tracks.postStepMomentumX_keV", &fPostStepMomentumX);
    fTree->Branch("tracks.postStepMomentumY_keV", &fPostStepMomentumY);
    fTree->Branch("tracks.postStepMomentumZ_keV", &fPostStepMomentumZ);
    fTree->Branch("tracks.x_global_um", &fXglobal);
    fTree->Branch("tracks.y_global_um", &fYglobal);
    fTree->Branch("tracks.z_global_um", &fZglobal);
    fTree->Branch("tracks.x_local_um", &fXlocal);
    fTree->Branch("tracks.y_local_um", &fYlocal);
    fTree->Branch("tracks.z_local_um", &fZlocal);
    fTree->Branch("tracks.implantDepth_um", &implantDepth);

}

MyRunAction::~MyRunAction() {
    if (fFile != nullptr) {
        delete fFile;
        fFile = nullptr;
    }
}

void MyRunAction::BeginOfRunAction(const G4Run* /*aRun*/) {
    //fEnergyDep = 0.0;
    //fDetectorID = 0;
}

void MyRunAction::EndOfRunAction(const G4Run* /*aRun*/) {
    G4String particleInfo = fGeneratorAction->GetParticleInfo();
    std::stringstream ss;
    ss << "../output/" << particleInfo << ".root";
    G4String filename = ss.str();
    G4cout << "Writing to file: " << filename << G4endl;
    fFile = new TFile(filename.c_str(), "RECREATE");
    fTree->Write();
    fFile->Close();
    
    if (fFile != nullptr) {
        delete fFile;
        fFile = nullptr;
    }
}
