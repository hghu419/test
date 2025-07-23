#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"

namespace B4 {

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* det)
 : fDet(det)
{
  fTargetLengthCmd = new G4UIcmdWithADoubleAndUnit("/det/targetLength", this);
  fTargetLengthCmd->SetGuidance("Set target length");
  fTargetLengthCmd->SetParameterName("length", false);
  fTargetLengthCmd->SetDefaultUnit("cm");
  fTargetLengthCmd->AvailableForStates(G4State_PreInit);

  fTargetRadiusCmd = new G4UIcmdWithADoubleAndUnit("/det/targetRadius", this);
  fTargetRadiusCmd->SetGuidance("Set target Radius");
  fTargetRadiusCmd->SetParameterName("radius", false);
  fTargetRadiusCmd->SetDefaultUnit("cm");
  fTargetRadiusCmd->AvailableForStates(G4State_PreInit);

  fTargetMaterialCmd = new G4UIcmdWithAString("/det/targetMaterial", this);
  fTargetMaterialCmd->SetGuidance("Set target material (NIST name)");
  fTargetMaterialCmd->SetParameterName("material", false);
  fTargetMaterialCmd->AvailableForStates(G4State_PreInit);
}

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
  delete fTargetLengthCmd;
  delete fTargetRadiusCmd;
  delete fTargetMaterialCmd;
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* cmd, G4String val)
{
  if (cmd == fTargetLengthCmd) {
    G4double length = fTargetLengthCmd->GetNewDoubleValue(val);
    fDet->SetTargetLength(length);
    G4RunManager::GetRunManager()->ReinitializeGeometry();
  }
  else if (cmd == fTargetRadiusCmd) {
  G4double radius = fTargetRadiusCmd->GetNewDoubleValue(val);
  fDet->SetTargetRadius(radius);
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
  else if (cmd == fTargetMaterialCmd) {
    fDet->SetTargetMaterial(val);
    G4RunManager::GetRunManager()->ReinitializeGeometry();
  }

}

}  // namespace B4
