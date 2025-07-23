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
/// \file B4/B4a/src/DetectorConstruction.cc
/// \brief Implementation of the B4::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Colour.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "DetectorConstructionMessenger.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4SubtractionSolid.hh"

// Scoring includes
#include "G4SDManager.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSFlatSurfaceFlux.hh"
#include "G4Exception.hh"


namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(),
    fTargetLength(50.0*cm),
    fTargetRadius(3.5*cm),
    fTargetMaterial(nullptr),
    fDetectorRadius(40.0*cm),
    fDetectorLength(150.0*cm),
    fDetectorMaterial(nullptr),
    fTargetLogical(nullptr),
    fDetectorLogical(nullptr),
    fCheckOverlaps(true),
    fMessenger(nullptr)
  {
    fMessenger = new DetectorConstructionMessenger(this);
  }

DetectorConstruction::~DetectorConstruction() {
  delete fMessenger;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // 使用NIST材料数据库
  G4NistManager* nistManager = G4NistManager::Instance();
  
  // 定义世界材料 - 真空
  nistManager->FindOrBuildMaterial("G4_Galactic");
  
  // 定义液态氢材料
  G4double density = 0.0708*g/cm3;  // 液态氢密度
  G4Material* liquidH2 = new G4Material("liquidH2", density, 1);
  liquidH2->AddElement(nistManager->FindOrBuildElement("H"), 2);
  //G4Material* liquidH2 = nistManager->FindOrBuildMaterial("G4_Pb");
  // 定义探测器材料 - 可以是塑料闪烁体
  G4Material* gas = nistManager->FindOrBuildMaterial("G4_AIR");
  
  // 设置靶材料和探测器材料
  fTargetMaterial = liquidH2;
  fDetectorMaterial = gas;
  
  // 打印材料表
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetDetectorMaterial(const  G4String& name)
{
  auto* mat = G4NistManager::Instance()->FindOrBuildMaterial(name);
  if ( mat )
    fDetectorMaterial = mat;
}
void DetectorConstruction::SetTargetMaterial(const  G4String& name)
{
  auto* mat = G4NistManager::Instance()->FindOrBuildMaterial(name);
  if ( mat )
    fTargetMaterial = mat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // 世界体积尺寸
  G4double worldSize = 300.0*cm;
  
  // 获取材料
  G4Material* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  
  // 检查材料是否定义成功
  if (!worldMaterial || !fTargetMaterial || !fDetectorMaterial) {
    G4ExceptionDescription msg;
    msg << "无法找到定义的材料。";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  //
  // World
  //
  G4Box* worldSolid = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldSolid, worldMaterial, "World");
  G4VPhysicalVolume* worldPV = new G4PVPlacement(
    nullptr,               // 无旋转
    G4ThreeVector(),       // 位于原点
    worldLV,          // 逻辑体积
    "World",               // 物理体积名称
    nullptr,               // 母体积
    false,                 // 无布尔操作
    0,                     // 拷贝编号
    fCheckOverlaps         // 检查重叠
  );

  // 
  // 液态氢靶 (圆柱体)
  //
  G4Tubs* targetSolid = new G4Tubs(
    "Target",              // 名称
    0.,                    // 内半径
    fTargetRadius,         // 外半径
    fTargetLength/2,       // 半长度
    0.*deg,              // 起始角度
    360.*deg                  // 终止角度
  );
  
  fTargetLogical = new G4LogicalVolume(
    targetSolid,           // 固体
    fTargetMaterial,       // 材料
    "Target"               // 名称
  );
  
  new G4PVPlacement(
    nullptr,               // 无旋转
    G4ThreeVector(0, 0, -fDetectorLength/2), // 位于原点
    fTargetLogical,        // 逻辑体积
    "Target",              // 物理体积名称
    worldLV,          // 母体积
    false,                 // 无布尔操作
    0,                     // 拷贝编号
    fCheckOverlaps         // 检查重叠
  );
  
  // 
  // 敏感探测器 (使用布尔操作创建一个有入射面的盒子)
  //
  
  // 外部大盒子
  G4Tubs* outerTubs = new G4Tubs(
    "outerTubs",              // 名称
    0.,                    // 内半径
    fDetectorRadius,         // 外半径
    fDetectorLength/2,       // 半长度
    0.*deg ,                    // 起始角度
    360.*deg                   // 终止角度
  );
  
  // 内部小盒子
  G4Tubs* innerTubs = new G4Tubs(
    "innerTubs",              // 名称
    0.,                    // 内半径
    fDetectorRadius-1*cm,         // 外半径
    fDetectorLength/2,       // 半长度
    0.*deg ,                    // 起始角度
    360.*deg                   // 终止角度
  );
  
  // 布尔操作: 从外部大盒子中减去内部小盒子
  G4SubtractionSolid* detectorSolid = new G4SubtractionSolid(
    "Detector",            // 名称
    outerTubs,              // 被减体
    innerTubs,              // 减体
    nullptr,               // 无旋转
    G4ThreeVector(0, 0, -1*cm) 
  );
  
  fDetectorLogical = new G4LogicalVolume(
    detectorSolid,         // 固体
    fDetectorMaterial,     // 材料
    "Detector"             // 名称
  );

  // 将探测器放置在靶的下游 (沿Z轴正方向)
  new G4PVPlacement(
    nullptr,               // 无旋转
    G4ThreeVector(0, 0, 0), // 位置
    fDetectorLogical,      // 逻辑体积
    "Detector",            // 物理体积名称
    worldLV,          // 母体积
    false,                 // 无布尔操作
    0,                     // 拷贝编号
    fCheckOverlaps         // 检查重叠
  );

  // 
  // 设置可视化属性
  //
  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  
  G4VisAttributes* targetVis = new G4VisAttributes(G4Colour(0.2, 0.2, 1.0, 0.5));
  targetVis->SetForceSolid(true);
  fTargetLogical->SetVisAttributes(targetVis);
  
  G4VisAttributes* detectorVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.3));
  detectorVis->SetForceSolid(true);
  fDetectorLogical->SetVisAttributes(detectorVis);

  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
