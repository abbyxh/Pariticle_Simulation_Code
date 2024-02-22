//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include <array>
#include <iostream>
#include "B1DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0),
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  // Envelope parameters
  //
  G4double env_sizeX = 28, env_sizeY = 60, env_sizeZ = 18;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double world_sizeX = 20*env_sizeX;
  G4double world_sizeY = 20*env_sizeY;
  G4double world_sizeZ  = 20*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
              
///////////////////////////////////////////////////////////////////////////////////

////Creating an array of points for components inside the envelope////
///Loop for envelope///

G4double pixelXLength {60};
G4double pixelYLength {28};
G4double pixelZLength {18};

int len {4};
std::array<std::array<double, 3>, 64> pixelLocations{};

for(int i{0}; i < len; i += 2)
{
  for(int j{0}; j < len; ++j)
  {
    for(int k{0}; k < len; ++k)
    {
      pixelLocations[k + len * j + len * len * i][2] = i * pixelZLength;
      pixelLocations[k + len * j + len * len * i][1] = j * pixelYLength;
      pixelLocations[k + len * j + len * len * i][0] = k * pixelXLength;

      //every other z layer is offset
      pixelLocations[k + len * j + len * len * (i + 1)][2] = i * pixelZLength;
      pixelLocations[k + len * j + len * len * (i + 1)][1] = (j + 0.5) * pixelYLength;
      pixelLocations[k + len * j + len * len * (i + 1)][0] = (k + 0.5) * pixelXLength;
    }
  }
}

//using the envelope defined in B1DetectorConstruction

for (G4int i{0}; i < 64; ++i)
{
  G4Box* solidEnv =    
    new G4Box("Envelope",                    
        0.5*pixelXLength, 0.5*pixelYLength, 0.5*pixelZLength);
      
  G4LogicalVolume* logicEnv =
    new G4LogicalVolume(solidEnv,            //its solid
                        env_mat,             //its material
                        "Envelope");       

  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(pixelLocations[i][0], pixelLocations[i][1], pixelLocations[i][2]),         
                    logicEnv,                //its logical volume    //idk if this will mess up the mothervolumes for each component
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    i,                       //copy number
                    checkOverlaps);

}
  
  //     
  // Resistor
  //
  std::array<std::array<G4double, 4>, 7> resistor = {{{4.5, -28, -2, 0}, {4.5, -14, -2, 0}, {4.5, 28, -2, 0}, {-8.5, 28, -2, 0}, {5, 10, 2, 0}, {11, 0, -2, 90}, {-2, 13.5, -2, 90}}};

  G4Material* res_mat_outer = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
 
  G4double outer_res_radius = 3;
  G4double outer_res_height = 1;
 
  //Inner metal of resistor

  G4Material* res_mat_inner = nist->FindOrBuildMaterial("G4_Si");

  G4double inner_res_radius = 2.9;
  G4double inner_res_height = 0.9;

  ////Loop to change the points///

  for (G4int copyNo=0; copyNo<7; copyNo++) {

      G4ThreeVector pos_res_outer = G4ThreeVector(resistor[copyNo][0], resistor[copyNo][1], resistor[copyNo][2]);

      G4Tubs* solidShape3 =    
          new G4Tubs("Resistor_out", 
            0., outer_res_radius, outer_res_height/2, resistor[copyNo][3] * deg, 360. * deg);
                      
      G4LogicalVolume* logicResOut =                         
        new G4LogicalVolume(solidShape3, res_mat_outer, "Resistor_out", 0, 0, 0);           //its name
                    
      new G4PVPlacement(0,                       //no rotation
                        pos_res_outer,                    //at position
                        logicResOut,             //its logical volume
                        "Resistor_out",                //its name
                        logicEnv,                //its mother  volume
                        false,                   //no boolean operation
                        copyNo,                       //copy number
                        checkOverlaps);          //overlaps checking


        //Inner metal of resistor

  G4Tubs* solidShape4 =    
    new G4Tubs("Resistor_in", 
      0., inner_res_radius, inner_res_height/2, resistor[copyNo][3] * deg, 360. * deg);
                      
  G4LogicalVolume* logicResIn = 
    new G4LogicalVolume(solidShape4, res_mat_inner, "Resistor_in", 0, 0, 0);           
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(0, 0, 0),                    //at position
                    logicResIn,             //its logical volume
                    "Resistor_in",                //its name
                    logicResOut,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps); 
    }

  ///Capacitor///////

  std::array<std::array<G4double, 3>, 3> capacitor = {{{0.5, -14, -2}, {0.5, 0, -2}, {6, 20, -2}}};

  //Capacior
  //Outer plastic of capacitor
  G4Material* cap_mat_outer = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  G4Material* cap_mat_inner = nist->FindOrBuildMaterial("G4_Si");
 
  G4double outer_cap_radius = 3;
  G4double outer_cap_height = 1;

  G4double inner_cap_radius = 2.9;
  G4double inner_cap_height = 0.9;

  G4Tubs* solidShape1 =    
    new G4Tubs("Capacitor_out", 
    0., outer_cap_radius, outer_cap_height/2, 90. * deg, 360. * deg);
                    

  for (G4int copyNo=0; copyNo<3; copyNo++) {

      G4ThreeVector pos_cap_outer = G4ThreeVector(capacitor[copyNo][0], capacitor[copyNo][1], capacitor[copyNo][2]);
              
      G4LogicVolume* logicCapOut = 
        new G4LogicalVolume(solidShape1, cap_mat_outer, "Capacitor_out", 0, 0, 0);           
                    
      new G4PVPlacement(0,                       //no rotation
                        pos_cap_outer,                    //at position
                        logicCapOut,             //its logical volume
                        "Capacitor_out",                //its name
                        logicEnv,                //its mother  volume
                        false,                   //no boolean operation
                        copyNo,                       //copy number
                        checkOverlaps);          //overlaps checking
    }

  ///Inner Capacitor///

  G4Tubs* solidShape2 =    
    new G4Tubs("Capacitor_in", 
    0., inner_cap_radius, inner_cap_height/2, 0. * deg, 360. * deg);
                      
  G4LogicVolume* logicCapIn = 
    G4LogicalVolume(solidShape2, cap_mat_inner, "Capacitor_in", 0, 0, 0); 
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(0, 0, 0),                    //at position
                    logicCapIn,             //its logical volume
                    "Capacitor_in",                //its name
                    logicCapOut,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);  

  /////For the chips/////
  std::array<std::array<G4double, 3>, 4> chip = {{{6, -21, -4}, {6, -7, -4}, {6, 3, -4}, {6, 13, -4}}};

  fNbOfChip = 4;
  fLogicChip = new G4LogicalVolume*[fNbOfChip];

  G4Material* chipOuterMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");  //not sure if this is the name of a real plastic
  G4double chipOuterX {9}; //defining x, y, z lengths of the external plastic on the chip
  G4double chipOuterY {7};
  G4double chipOuterZ {4};

  G4Box* chipOuter = 
  new G4Box("chipInner",                       //its name
      0.5 * chipOuterX, 0.5 * chipOuterY, 0.5 * chipOuterZ);  

  for (G4int copyNo=0; copyNo<4; copyNo++) {

      G4ThreeVector pos_chip_outer = G4ThreeVector(chip[copyNo][0], chip[copyNo][1], chip[copyNo][2]);
              
        G4LogicalVolume* logicChipOuter =                         
          new G4LogicalVolume(chipOuter,         //its solid
                              chipOuterMat,          //its material
                              "chipOuter");           //its name
                    
        new G4PVPlacement(0,                       //no rotation
                          pos_chip_outer,                    //at position
                          logicChipOuter,             //its logical volume
                          "chipOuter",                //its name
                          logicEnv,                //its mother  volume
                          false,                   //no boolean operation
                          copyNo,                       //copy number
                          checkOverlaps);          //overlaps checking
    }

  G4Material* chipInnerMat = nist->FindOrBuildMaterial("G4_Si");  //not sure if this is the name of a real plastic
  G4double chipInnerX {8};   //defining x, y, z lengths of the components inside the chip
  G4double chipInnerY {6};  //note these are just an assumptions as we dont know the true size of the silicon inside the diodes
  G4double chipInnerZ {3};

  G4Box* chipInner = 
    new G4Box("chipInner",                       //its name
        0.5 * chipInnerX, 0.5 * chipInnerY, 0.5 * chipInnerZ);   //its size
  
  G4LogicalVolume* logicChipInner = 
    new G4LogicalVolume(chipInner,         //?? not sure think thats its in diodeOuter above
                        chipInnerMat,      //its material
                        "chipInner");      //its name
          
  new G4PVPlacement(0,                    //no rotation
                    G4ThreeVector(0, 0, 0),      //coordinate
                    logicChipInner,      //it's logical volume
                    "chipOuter",         //its name
                    logicChipOuter,      //its mother volume
                    false,                //no boolean operation
                    0,                    //copy number? presumably the number of times it appears which will be more than 0     
                    checkOverlaps);


  /*
  AG creating the diode
  */
  G4Material* diodeOuterMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");  //not sure if this is the name of a real plastic
  G4double diodeOuterX {1.8}; //defining x, y, z lengths of the diode
  G4double diodeOuterY {1.35};
  G4double diodeOuterZ {1.05};

  G4Box* diodeOuter = 
    new G4Box("diodeOuter",                       //its name
        0.5 * diodeOuterX, 0.5 * diodeOuterY, 0.5 * diodeOuterZ);   //its size
  
  G4LogicalVolume* logicDiodeOuter = 
    new G4LogicalVolume(diodeOuter,         //its solid ?? presumably means filled by diodeOuter
                        diodeOuterMat,      //its material
                        "diodeOuter");      //its name
         
  new G4PVPlacement(0,                    //no rotation
                    G4ThreeVector(0, 0, 0),      //coordinate
                    logicDiodeOuter,      //it's logical volume
                    "diodeOuter",         //its name
                    logicEnv,             //its mother volume
                    false,                //no boolean operation
                    0,                    //copy number? presumably the number of times it appears which will be more than 0     
                    checkOverlaps);       //overlaps checking
  

  G4Material* diodeInnerMat = nist->FindOrBuildMaterial("G4_Si");  //not sure if this is the name of a real plastic
  G4double diodeInnerX {0.7};   //defining x, y, z lengths of the sensor inside the diode
  G4double diodeInnerY {0.7};  //note these are just an assumptions as we dont know the true size of the silicon inside the diodes
  G4double diodeInnerZ {0.4};

  G4Box* diodeInner = 
    new G4Box("diodeInner",                       //its name
        0.5 * diodeInnerX, 0.5 * diodeInnerY, 0.5 * diodeInnerZ);   //its size
  
  G4LogicalVolume* logicDiodeInner = 
    new G4LogicalVolume(diodeInner,         //?? not sure think thats its in diodeOuter above
                        diodeInnerMat,      //its material
                        "diodeInner");      //its name
          
  new G4PVPlacement(0,                    //no rotation
                    G4ThreeVector(0, 0, 0),      //coordinate
                    logicDiodeInner,      //it's logical volume
                    "diodeOuter",         //its name
                    logicDiodeOuter,      //its mother volume
                    false,                //no boolean operation
                    0,                    //copy number? presumably the number of times it appears which will be more than 0     
                    checkOverlaps);


  /*
  Transistor
  */
  G4Material* transOuterMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");  //not sure if this is the name of a real plastic
  G4double transOuterX {1.5}; //defining x, y, z lengths of the diode
  G4double transOuterY {1.0};
  G4double transOuterZ {1.0};

  G4Box* transOuter = 
    new G4Box("transOuter",                       //its name
        0.5 * transOuterX, 0.5 * transOuterY, 0.5 * transOuterZ);   //its size
  
  G4LogicalVolume* logicTransOuter = 
    new G4LogicalVolume(transOuter,         //its solid ?? presumably means filled by diodeOuter
                        transOuterMat,      //its material
                        "transOuter");      //its name
         
  new G4PVPlacement(0,                    //no rotation
                    G4ThreeVector(0, 0, 0),      //coordinate
                    logicTransOuter,      //it's logical volume
                    "transOuter",         //its name
                    logicEnv,             //its mother volume
                    false,                //no boolean operation
                    0,                    //copy number? presumably the number of times it appears which will be more than 0     
                    checkOverlaps);       //overlaps checking
  

  G4Material* transInnerMat = nist->FindOrBuildMaterial("G4_Si");  //not sure if this is the name of a real plastic
  G4double transInnerX {1.4};   //defining x, y, z lengths of the sensor inside the diode
  G4double transInnerY {0.9};  //note these are just an assumptions as we dont know the true size of the silicon inside the diodes
  G4double transInnerZ {0.9};

  G4Box* transInner = 
    new G4Box("transInner",                       //its name
        0.5 * transInnerX, 0.5 * transInnerY, 0.5 * transInnerZ);   //its size
  
  G4LogicalVolume* logicTransInner = 
    new G4LogicalVolume(transInner,         //?? not sure think thats its in diodeOuter above
                        transInnerMat,      //its material
                        "transInner");      //its name
          
  new G4PVPlacement(0,                    //no rotation
                    G4ThreeVector(0, 0, 0),      //coordinate
                    logicTransInner,      //it's logical volume
                    "transOuter",         //its name
                    logicTransOuter,      //its mother volume
                    false,                //no boolean operation
                    0,                    //copy number? presumably the number of times it appears which will be more than 0     
                    checkOverlaps);

  /*
  counter
  */

  G4Material* counterOuterMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");  //not sure if this is the name of a real plastic
  G4double counterOuterX {19}; //defining x, y, z lengths of the external plastic on the counter
  G4double counterOuterY {8};
  G4double counterOuterZ {4};

  G4Box* counterOuter = 
    new G4Box("counterOuter",                       //its name
        0.5 * counterOuterX, 0.5 * counterOuterY, 0.5 * counterOuterZ);   //its size
  
  G4LogicalVolume* logicCounterOuter = 
    new G4LogicalVolume(counterOuter,         //its solid ?? presumably means filled by diodeOuter
                        counterOuterMat,      //its material
                        "counterOuter");      //its name

  
  new G4PVPlacement(0,                    //no rotation
                    G4ThreeVector(),      //coordinate
                    logicCounterOuter,      //it's logical volume
                    "counterOuter",         //its name
                    logicEnv,             //its mother volume
                    false,                //no boolean operation
                    0,                    //copy number? presumably the number of times it appears which will be more than 0     
                    checkOverlaps);       //overlaps checking


  G4Material* counterInnerMat = nist->FindOrBuildMaterial("G4_Si");  //not sure if this is the name of a real plastic
  G4double counterInnerX {18};   //defining x, y, z lengths of the inner components of the counter
  G4double counterInnerY {7};  //note these are just an assumptions as we dont know the true size of the silicon inside the diodes
  G4double counterInnerZ {3};

  G4Box* counterInner = 
    new G4Box("counterInner",                       //its name
        0.5 * counterInnerX, 0.5 * counterInnerY, 0.5 * counterInnerZ);   //its size
  
  G4LogicalVolume* logicCounterInner = 
    new G4LogicalVolume(counterInner,         //?? not sure think thats its in diodeOuter above
                        counterInnerMat,      //its material
                        "counterInner");      //its name
          
  new G4PVPlacement(0,                    //no rotation
                    G4ThreeVector(0, 0, 0),      //coordinate
                    logicCounterInner,      //it's logical volume
                    "counterOuter",         //its name
                    logicCounterOuter,      //its mother volume
                    false,                //no boolean operation
                    0,                    //copy number? presumably the number of times it appears which will be more than 0     
                    checkOverlaps);


/*
plastic seperator
*/
  G4Material* seperatorMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");  //not sure if this is the name of a real plastic
  G4double seperatorX {27.5};   //defining x, y, z lengths of the seperator sheet which all the components arr attatched to
  G4double seperatorY {59.5};  
  G4double seperatorZ {1};

  G4Box* seperator = 
    new G4Box("seperator",                       //its name
        0.5 * seperatorX, 0.5 * seperatorY, 0.5 * seperatorZ);   //its size
  
  G4LogicalVolume* logicSeperator = 
    new G4LogicalVolume(seperator,         //?? not sure think thats its in diodeOuter above
                        seperatorMat,      //its material
                        "seperator");      //its name
          
  new G4PVPlacement(0,                    //no rotation
                    G4ThreeVector(0, 0, 0),      //coordinate
                    logicSeperator,      //it's logical volume
                    "seperator",         //its name
                    logicEnv,      //its mother volume
                    false,                //no boolean operation
                    0,                    //copy number? presumably the number of times it appears which will be more than 0     
                    checkOverlaps);

  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // Set Metal of Diode (inner diode) as scoring volume
  //
  fScoringVolume = logicDiodeInner;

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
