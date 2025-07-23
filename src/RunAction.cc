// ********************************************************************
//
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
/// \file B4/B4a/src/RunAction.cc
/// \brief Implementation of the B4::RunAction class

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4AnalysisManager.hh"
#include "G4AccumulableManager.hh"
#include "G4RunManager.hh"
#include "G4Threading.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4Run.hh"
#include "G4ios.hh"
#include "DetectorConstruction.hh"
#include "Randomize.hh"
#include "G4Types.hh"
#include "RunActionMessenger.hh"
#include <ctime>
#include <iostream>
#include <filesystem>

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(bool isMaster, PrimaryGeneratorAction* genAction, DetectorConstruction* det)
  : G4UserRunAction(),
    fIsMaster(isMaster),
    fGenAction(genAction),
    fDet(det),
    fPassed("Passed", 0),
    fBlocked("Blocked", 0),
    fEnableOutput(true),
    fFileName(""),
    fDirectory(""),
    fAnalysisManager(nullptr),
    fRunMessenger(nullptr)
{
  // 注册accumulateManager
  auto* mgr = G4AccumulableManager::Instance();
  mgr->RegisterAccumulable(&fPassed);
  mgr->RegisterAccumulable(&fBlocked);
  // if you are using higher version of G4(like 11.3.2), you need to replace `RegisterAccumulable` with `Register`.

  fRunMessenger = new RunActionMessenger(this);

  // auto* analysis = G4AnalysisManager::Instance();
  if(fEnableOutput){
    // 创建分析管理器
    fAnalysisManager = G4AnalysisManager::Instance();
    fAnalysisManager->SetNtupleMerging(true);
    fAnalysisManager->SetVerboseLevel(1);

    // 在ntuple 中创建列：事件号、能量、Px、Py、Pz
    fAnalysisManager->CreateNtuple("tree", "TransmittedParticles");
    fAnalysisManager->CreateNtupleIColumn("PDG");
    fAnalysisManager->CreateNtupleDColumn("px");
    fAnalysisManager->CreateNtupleDColumn("py");
    fAnalysisManager->CreateNtupleDColumn("pz");
    fAnalysisManager->CreateNtupleDColumn("pE");
    fAnalysisManager->CreateNtupleDColumn("theta");
    fAnalysisManager->CreateNtupleDColumn("phi");
    fAnalysisManager->FinishNtuple();

    fAnalysisManager->CreateH2("theta_px", "Theta vs Px", 90, 0, 180,100, -20.0, 20.0);
    fAnalysisManager->CreateH2("theta_py", "Theta vs Py", 90, 0, 180,100, -20.0, 20.0);
    fAnalysisManager->CreateH2("theta_pz", "Theta vs Pz", 90, 0, 180,100, 0.0, 6000.0);
    fAnalysisManager->CreateH2("theta_p", "Theta vs P", 90, 0, 180,100, 0.0, 6000.0);
  }
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(100);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  auto* mgr = G4AccumulableManager::Instance();
  mgr->Reset(); //reset the accumulable numbers


  // 获取Master中生成器
  auto* gen = fGenAction;

  // 获取粒子信息
  fPtype = "unknown";
  fEnergy = 0.;
  if (gen && gen->GetParticleGun()){
    fPtype = gen->GetParticleGun()->GetParticleDefinition()->GetParticleName();
    fEnergy = gen->GetParticleGun()->GetParticleEnergy();
  }

  // 获取屏蔽层信息
  const auto* baseDet = G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  auto* det = dynamic_cast<const DetectorConstruction*>(baseDet);
  fTargetLength = (det ? det->GetTargetLength() : 0.);
  fTargetRadius = (det ? det->GetTargetRadius() : 0.);
  fTargetMaterial = (det ? det->GetTargetMaterialName() : "unknown");

  if (fEnableOutput) {
    // 1) 确定基础文件名
    std::string name;
    if (fFileName.empty()) {
      // 格式：类型_能量MeV_材料_厚度cm_时间戳.root
      std::ostringstream oss;
      oss << fPtype << "_"
          << std::fixed << std::setprecision(0) << fEnergy << "MeV_"
          << fTargetMaterial << "_"
          << std::fixed << std::setprecision(0)
          << (fTargetRadius/cm) << "cm_" 
          << (fTargetLength/cm) << "cm_";
  
      auto t  = std::time(nullptr);
      auto tm = *std::localtime(&t);
      oss << std::put_time(&tm, "%Y%m%d_%H%M%S")
          << ".root";
      name = oss.str();
    } else {
      name = fFileName;  // 用户在宏里指定了完整文件名（需含 .root）
    }
  
    // 2) 如用户指定目录，则去掉末尾斜杠并创建
    if (!fDirectory.empty()) {
      std::string dir = fDirectory;
      // 去掉所有末尾的 '/' 或 '\'
      while (!dir.empty() && (dir.back()=='/' || dir.back()=='\\')) {
        dir.pop_back();
      }
      // 递归创建目录（若已存在，此调用也不会报错）
      std::filesystem::create_directories(dir);
      // 最终路径 = 目录 + '/' + 文件名
      name = dir + "/" + name;
    }
  
    // 3) 打开 ROOT 文件
    G4AnalysisManager::Instance()->OpenFile(name);
    G4cout << "打开输出文件: " << name << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  // Master: 合并全局信息
  G4AccumulableManager::Instance()->Merge();

  // 打印全局事件数
  if (fIsMaster){
    G4int totalPassed = fPassed.GetValue();
    G4cout << "The totalPassed=" << totalPassed << G4endl;
    G4int totalBlocked = fBlocked.GetValue();
    G4cout << "The totalBlocked=" << totalBlocked << G4endl;
    G4int totalPrim = totalPassed  + totalBlocked;
    // 计算效率
    G4double efficiency = totalPrim > 0 ? (100 - 100. * totalPassed / totalPrim) : 0.;


    // 计算并打印全局所有信息
    G4cout
      << "========== Merged Run Summary ==========\n"
      << " Particle type         : " << fPtype << "\n"
      << " Particle energy       : " << G4BestUnit(fEnergy, "Energy") << "\n"
      << "=================================\n";
  }

  // fAnalysisManager = G4AnalysisManager::Instance();
  if(fEnableOutput){
    fAnalysisManager->Write();
    fAnalysisManager->CloseFile();
    G4cout << "ROOT 文件已写入并关闭" << G4endl;
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
