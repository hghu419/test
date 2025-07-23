#include "RunActionMessenger.hh"
#include "RunAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"

namespace B4
{
RunActionMessenger::RunActionMessenger(RunAction* runAction)
 : fRunAction(runAction)
{
  // 创建 /run/output/ 目录命令
  fDirOutput = new G4UIdirectory("/run/output/");
  fDirOutput->SetGuidance("控制 ROOT 文件输出");

  // enableRoot
  fCmdEnable = new G4UIcmdWithABool("/run/output/enableRoot", this);
  fCmdEnable->SetGuidance("开启或关闭 ROOT 输出");
  fCmdEnable->SetParameterName("enable", true);
  fCmdEnable->SetDefaultValue(true);
  fCmdEnable->AvailableForStates(G4State_PreInit, G4State_Idle);

  // fileName
  fCmdFileName = new G4UIcmdWithAString("/run/output/fileName", this);
  fCmdFileName->SetGuidance("设置输出 ROOT 文件名（不含路径，含 .root 后缀），不设置则使用默认时间戳命名");
  fCmdFileName->SetParameterName("name", true);
  fCmdFileName->SetDefaultValue("");
  fCmdFileName->AvailableForStates(G4State_PreInit, G4State_Idle);

  // directory
  fCmdDirectory = new G4UIcmdWithAString("/run/output/directory", this);
  fCmdDirectory->SetGuidance("设置输出目录，不设置则当前目录");
  fCmdDirectory->SetParameterName("dir", true);
  fCmdDirectory->SetDefaultValue("");
  fCmdDirectory->AvailableForStates(G4State_PreInit, G4State_Idle);
}

RunActionMessenger::~RunActionMessenger()
{
  delete fCmdDirectory;
  delete fCmdFileName;
  delete fCmdEnable;
  delete fDirOutput;
}

void RunActionMessenger::SetNewValue(G4UIcommand* cmd, G4String val)
{
  if (cmd == fCmdEnable) {
    fRunAction->SetEnableOutput(fCmdEnable->GetNewBoolValue(val));
  }
  else if (cmd == fCmdFileName) {
    fRunAction->SetFileName(val);
  }
  else if (cmd == fCmdDirectory) {
    fRunAction->SetDirectory(val);
  }
}

} // namespace B4