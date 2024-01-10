#include "G4ClassificationOfNewTrack.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "MyStackingAction.hh"

MyStackingAction::MyStackingAction()
    : G4UserStackingAction() { }

MyStackingAction::~MyStackingAction() { }

G4ClassificationOfNewTrack MyStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{

    G4ClassificationOfNewTrack result(fUrgent);

    // Reset time for immediate daughter track(s)
    // This has the effect of having all daughters of radioactive decay start at t=0
    if (aTrack->GetCreatorProcess()){
        if (aTrack->GetCreatorProcess()->GetProcessName() == "RadioactiveDecay" && aTrack->GetParentID() == 1){
                (const_cast<G4Track*>(aTrack))->SetGlobalTime(0.0);
        }
    }

    // Change neutrino from nu_e with mass = 0 (in geant4 its zero but really its <1 eV) 
    // into sterile neutrino with some user defined keV - MeV mass
    // Need to according scale momentum to maintain energy conservation
    // Also need to adjust momentum of recoiling V51 to maintain 
    /*
    G4double nu_sterile_mass = 0.3 * MeV;

    if (aTrack->GetCreatorProcess()){
        if (aTrack->GetCreatorProcess()->GetProcessName() == "RadioactiveDecay" && aTrack->GetParentID() == 1 &&
        aTrack->GetDefinition()->GetParticleName().contains("nu")){
            G4ThreeVector momentum_i = aTrack->GetDynamicParticle()->GetMomentum();
            G4double total_momentum_i =  aTrack->GetDynamicParticle()->GetTotalMomentum();

            G4double total_momentum_f = pow(total_momentum_i*total_momentum_i - nu_sterile_mass*nu_sterile_mass, 0.5);
            G4ThreeVector momentum_f = momentum_i * (total_momentum_f/total_momentum_i);

            (const_cast<G4DynamicParticle*>(aTrack->GetDynamicParticle()))->SetMass(nu_sterile_mass);
            (const_cast<G4DynamicParticle*>(aTrack->GetDynamicParticle()))->SetMomentum(momentum_f);
        }
    }
    */

    return result;

}