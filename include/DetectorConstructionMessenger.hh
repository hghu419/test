#ifndef B4DetectorConstructionMessenger_h
#define B4DetectorConstructionMessenger_h

#include "G4UImessenger.hh"

class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

namespace B4 {

class DetectorConstruction;

// define command to change the target material and length in macro

class DetectorConstructionMessenger : public G4UImessenger {
public:
  DetectorConstructionMessenger(DetectorConstruction* det);
  ~DetectorConstructionMessenger() override;

  void SetNewValue(G4UIcommand* cmd, G4String val) override;

private:
  DetectorConstruction*         fDet;
  G4UIcmdWithADoubleAndUnit*    fTargetLengthCmd;
  G4UIcmdWithADoubleAndUnit*    fTargetRadiusCmd;
  G4UIcmdWithAString*           fTargetMaterialCmd;
};

}  // namespace B4

#endif  // B4DetectorConstructionMessenger_h