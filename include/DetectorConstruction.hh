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
/// \file B4/B4a/include/DetectorConstruction.hh
/// \brief Definition of the B4::DetectorConstruction class


#ifndef B4DetectorConstruction_h
#define B4DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4Threading.hh"
#include "G4String.hh"
#include "globals.hh"

class G4GlobalMagFieldMessenger;
class G4LogicalVolume;
class EventAction;

namespace B4
{
  class DetectorConstructionMessenger;

  class DetectorConstruction : public G4VUserDetectorConstruction
  {
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;


    // 构造几何
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;


    G4double GetTargetLength() const { return fTargetLength; }
    G4double GetTargetRadius() const { return fTargetRadius; }
    G4String GetTargetMaterialName() const {
      return fTargetMaterial ? fTargetMaterial->GetName() : "unknown";
    }
    // 定义敏感探测器：保存虚拟探测层逻辑体积(线程私有)
    G4LogicalVolume* GetTargetLogical() const { return fTargetLogical; }
    G4LogicalVolume* GetDetectorLogical() const { return fDetectorLogical; }

    void SetTargetMaterial(const G4String& name);
    void SetTargetLength(G4double val) { fTargetLength = val; }
    void SetTargetRadius(G4double val) { fTargetRadius = val; }

    void SetDetectorMaterial(const G4String& name);
    void SetDetectorLength(G4double val) { fDetectorLength = val; }

  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    // data members
    //
    G4double fTargetLength;   
    G4double fTargetRadius;            
    G4Material* fTargetMaterial;           
    G4LogicalVolume* fTargetLogical;

    G4double fDetectorLength;              
    G4Material* fDetectorMaterial;             
    G4LogicalVolume* fDetectorLogical;
    G4double fDetectorRadius;   

    G4bool fCheckOverlaps;

    // 线程私有的磁场管理器
    static G4ThreadLocal G4GlobalMagFieldMessenger* fMagFieldMessenger;

    DetectorConstructionMessenger* fMessenger;
};

}  // namespace B4

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif // B4DetectorConstruction_h
