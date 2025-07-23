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
/// \file B4/B4a/include/EventAction.hh
/// \brief Definition of the B4a::EventAction class

#ifndef B4EventAction_h
#define B4EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>  // 添加vector支持
#include <fstream>
#include <mutex>
class G4Event;

namespace B4
{

class RunAction;
class PrimaryGeneratorAction;

class EventAction : public G4UserEventAction
{
public:
  EventAction();
  virtual ~EventAction() = default;

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  // 仅记录入射角
  void RecordEntry(G4int pdg,G4double E, const G4ThreeVector& pDir, const G4ThreeVector& pMom);
  // 文本输出配置
  static void EnableTextOutput(const G4String& filename);
private:
  //G4bool fRecorded;      // 是否已记录入射方向
  //G4double fTheta;       // 入射方向θ（单位：度）
  //G4double fPhi;         // 入射方向φ（单位：度）
    // 改用vector存储多个粒子数据
  std::vector<G4int> fPDGs;
  std::vector<G4double> fThetas;  // 所有粒子的θ角度(度)
  std::vector<G4double> fPhis;    // 所有粒子的φ角度(度)
  std::vector<G4double> fpx; 
  std::vector<G4double> fpy;
  std::vector<G4double> fpz;
  std::vector<G4double> fE;   

};

}  // namespace B4

#endif