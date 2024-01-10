#include "MyDetectorConstruction.hh"
#include "MyDetectorConstMessenger.hh"

#include "G4RunManager.hh"

#include "G4Box.hh"
#include "G4Orb.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4Material.hh"
#include "G4Element.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"
#include "G4Region.hh"
#include "G4VisAttributes.hh"
#include <sstream>


MyDetectorConstruction::MyDetectorConstruction() 
:   G4VUserDetectorConstruction(),
    fMessenger(0),
    fDoBuildNanosphere(true),
    fDoBuildSiPanels(false),
    fMaxStepLimit(10.*nm),
    fSiO2sphereRad_nm(100*nm),
    fSiXYsize_cm(1.2*cm),
    fSiThickness_um(250.*um),
    fSourceOffset_cm(-0.2*cm)
{
    fMessenger = new MyDetectorConstMessenger(this);
    G4cout << "MyDetectorConstruction()" << G4endl;
}

MyDetectorConstruction::~MyDetectorConstruction() {
    delete fMessenger;
}

G4VPhysicalVolume* MyDetectorConstruction::Construct() {
    // Define the world volume
    G4double worldSizeXY = 4.0 * cm;
    G4double worldSizeZ = 4.0 * cm;
    G4Material* worldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");  // vacuum

    G4Box* worldBox = new G4Box("World", worldSizeXY / 2, worldSizeXY / 2, worldSizeZ / 2);
    worldLogVol = new G4LogicalVolume(worldBox, worldMaterial, "World");
    G4VPhysicalVolume* worldPhysVol = new G4PVPlacement(0, G4ThreeVector(), worldLogVol, "World", 0, false, 0);

    // Define Silica nanogel at (0,0,0) where radioactive source is placed
    // Define to be solid sphere with 100 nm default radius
    if (fDoBuildNanosphere){
        G4double sphereRad = fSiO2sphereRad_nm;
        G4Orb * nanosphereSolid = new G4Orb("nanosphereSolid", sphereRad);
        // Si02 density can range from 1.8-2.2 g/cm^3
        G4Element * elO = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
        G4Element * elSi = new G4Element("Silicon", "Si", 14., 28.09*g/mole);
        G4double SiO2_density = 2*g/cm3;
        // Define the material using silicon and oxygen 
        G4Material* SiO2 = new G4Material("mat_SiO2", SiO2_density, 2);
        SiO2->AddElement(elO, 2);
        SiO2->AddElement(elSi, 1);
        // Build the logical volume
        G4LogicalVolume* nanosphereLogVol = new G4LogicalVolume(nanosphereSolid, SiO2, "nanosphereLogVol");
        
        // Visibility attributed
        G4VisAttributes* Si02VisAtt = new G4VisAttributes(G4Colour::Red());
        Si02VisAtt->SetVisibility(true);
        nanosphereLogVol->SetVisAttributes(Si02VisAtt);
        
        // Add into the world with name nanospher
        new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), nanosphereLogVol, "nanosphere", worldLogVol, false, 0);
    }
    // Silicon faces to represent CCDs
    // Thickness inlcude deadlayer for now...
    if (fDoBuildSiPanels){
        G4double siThickness = fSiThickness_um;
        G4double siSizeXY = fSiXYsize_cm;  

        // Should check this gives the correct density...
        G4Material* siMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

        G4Box* siBox = new G4Box("Si", siSizeXY / 2, siSizeXY / 2, siThickness / 2);
        G4LogicalVolume* siLogVol = new G4LogicalVolume(siBox, siMaterial, "Si");

        // Set the user limits for maximum step size in the Si volume
        G4UserLimits* userLimits = new G4UserLimits();
        G4double limitStep = fMaxStepLimit;
        userLimits->SetMaxAllowedStep(limitStep);
        siLogVol->SetUserLimits(userLimits);
        G4VisAttributes* siVisAtt = new G4VisAttributes(G4Colour::Blue());
        siVisAtt->SetVisibility(true);
        siLogVol->SetVisAttributes(siVisAtt);

        // Position the Si detectors around the source
        // G4double sourceOffset = 0.499 * cm;  // Offset of the center of the detector box from the source (-0.2 for dice geometry)
        G4double sourceOffset = fSourceOffset_cm;  // Offset of the center of the detector box from the source (0.499 for source in front of single detector)
        std::vector<std::pair<G4ThreeVector, G4RotationMatrix>> placements = {
            {G4ThreeVector(0, 0, siSizeXY / 2 + siThickness / 2 - sourceOffset), G4RotationMatrix()},  // 1 front, no rotation
            {G4ThreeVector(-siSizeXY / 2 - siThickness / 2, 0, -sourceOffset), G4RotationMatrix().rotateY(90 * deg)},  // 2 left
            {G4ThreeVector(0, -siSizeXY / 2 - siThickness / 2, -sourceOffset), G4RotationMatrix().rotateX(-90 * deg)},  // 3 bottom
            {G4ThreeVector(0, siSizeXY / 2 + siThickness / 2, -sourceOffset), G4RotationMatrix().rotateX(90 * deg)},  // 4 top
            {G4ThreeVector(siSizeXY / 2 + siThickness / 2, 0, -sourceOffset), G4RotationMatrix().rotateY(-90 * deg)},  // 5 right
        };
            // {G4ThreeVector(0, 0, -siSizeXY / 2 - siThickness / 2 - sourceOffset), G4RotationMatrix().rotateY(180 * deg)}  // 6 back

        std::stringstream nameStream;

        for (size_t i = 0; i < placements.size(); ++i) {
            nameStream.str(""); // Clear the stringstream
            nameStream << "Si" << i+1; // Construct the name
            new G4PVPlacement(new G4RotationMatrix(placements[i].second), placements[i].first, siLogVol, nameStream.str(), worldLogVol, false, 0.);
        }
    }
    return worldPhysVol;
}

// void MyDetectorConstruction::SetMaxStepSize(G4double step) {
//     fMaxStepSize = step;
//     G4int daughters = worldLogVol->GetNoDaughters();

//     for (G4int i = 0; i < daughters; i++) {
//         G4VPhysicalVolume* daughter = worldLogVol->GetDaughter(i);
//         G4cout << daughter->GetName() << G4endl;
//         if (daughter->GetName() == "Si") {
//             daughter->GetLogicalVolume()->SetUserLimits(new G4UserLimits(fMaxStepSize));
//         }
//     }
// }

// Get Detector parameters from Messenger
void MyDetectorConstruction::SetBuildNanosphere(G4bool doBuildNanosphere){
    fDoBuildNanosphere = doBuildNanosphere;
}
void MyDetectorConstruction::SetBuildSiPanels(G4bool doBuildSiPanels){
    fDoBuildSiPanels = doBuildSiPanels;
}
void MyDetectorConstruction::SetSiO2sphereRad(G4double SiO2sphereRad_nm){
    fSiO2sphereRad_nm = SiO2sphereRad_nm;
}
void MyDetectorConstruction::SetSiXYsize(G4double SiXYsize_cm){
    fSiXYsize_cm = SiXYsize_cm;
}
void MyDetectorConstruction::SetSiThickness(G4double SiThickness_um){
    fSiThickness_um = SiThickness_um;
}
void MyDetectorConstruction::SetSourceOffset(G4double SourceOffset_cm){
    fSourceOffset_cm = SourceOffset_cm;
}
// Geant4 specific info related to recording events
void MyDetectorConstruction::SetMaxStepLimit(G4double MaxStepLimit){
    fMaxStepLimit = MaxStepLimit;
}