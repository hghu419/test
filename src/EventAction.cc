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
/// \file B4/B4a/src/EventAction.cc
/// \brief Implementation of the B4a::EventAction class

#include "EventAction.hh"
#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4AccumulableManager.hh"


namespace B4
{

EventAction::EventAction()
  : G4UserEventAction()
    //fRecorded(false),
    //fTheta(0.), fPhi(0.)
{}

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
  //fRecorded = false;
  //fTheta = 0.;
  //fPhi = 0.;
  fPDGs.clear();
  fThetas.clear();
  fPhis.clear();
  fpx.clear();
  fpy.clear();
  fpz.clear();
  fE.clear();  
}

void EventAction::RecordEntry(G4int pdg,G4double E,
                              const G4ThreeVector& pDir,
                              const G4ThreeVector& pMom)
{
  // if (!fRecorded) {
  //   fRecorded = true;
  //   fTheta = pDir.theta() / CLHEP::deg;  // 以角度为单位
  //   fPhi   = pDir.phi()   / CLHEP::deg;
  // }
  fPDGs.push_back(pdg);
  fThetas.push_back(pDir.theta() / CLHEP::deg);  // 转换为角度
  fPhis.push_back(pDir.phi() / CLHEP::deg);
  fpx.push_back(pMom.x());
  fpy.push_back(pMom.y());
  fpz.push_back(pMom.z());
  fE.push_back(E);
}

void EventAction::EndOfEventAction(const G4Event* event)
{
  auto* analysis = G4AnalysisManager::Instance();
  G4int eventID = event->GetEventID();
  // 为每个粒子填充一行数据
  for (size_t i = 0; i < fThetas.size(); ++i) {
    double p = sqrt(fpx[i]*fpx[i] + fpy[i]*fpy[i] + fpz[i]*fpz[i]);
    analysis->FillNtupleIColumn(0, fPDGs[i]);
    analysis->FillNtupleDColumn(1, fpx[i]); 
    analysis->FillNtupleDColumn(2, fpy[i]); 
    analysis->FillNtupleDColumn(3, fpz[i]); 
    analysis->FillNtupleDColumn(4, fE[i]);
    analysis->FillNtupleDColumn(5, fThetas[i]);  // θ
    analysis->FillNtupleDColumn(6, fPhis[i]);    // φ
    analysis->AddNtupleRow();  // 每粒子一行

    analysis->FillH2(0, fThetas[i],fpx[i]);
    analysis->FillH2(1, fThetas[i],fpy[i]);
    analysis->FillH2(2, fThetas[i],fpz[i]);
    analysis->FillH2(3, fThetas[i],p);    
  // if (fRecorded) {
  //   auto* analysis = G4AnalysisManager::Instance();
  //   analysis->FillNtupleDColumn(0, fTheta);
  //   analysis->FillNtupleDColumn(1, fPhi);
  //   analysis->AddNtupleRow();
  }


}

}  // namespace B4