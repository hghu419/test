#ifndef B4RunActionMessenger_h
#define B4RunActionMessenger_h

#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "RunAction.hh"

namespace B4
{
class RunAction;

class RunActionMessenger : public G4UImessenger {
public:
  explicit RunActionMessenger(RunAction* runAction);
  ~RunActionMessenger() override;

  void SetNewValue(G4UIcommand* cmd, G4String val) override;

private:
  RunAction*              fRunAction;

  G4UIdirectory*          fDirOutput;      // /run/output/
  G4UIcmdWithABool*       fCmdEnable;      // enable/disable ROOT 输出
  G4UIcmdWithAString*     fCmdFileName;    // 自定义文件名
  G4UIcmdWithAString*     fCmdDirectory;   // 自定义输出目录
};

} // namespace B4
#endif  // B4RunActionMessenger_h
