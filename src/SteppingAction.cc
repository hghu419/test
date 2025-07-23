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
/// \file B4/B4a/src/SteppingAction.cc
/// \brief Implementation of the B4a::SteppingAction class

#include "SteppingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4LogicalVolume.hh"
#include "G4AnalysisManager.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* detConstruction, EventAction* eventAction, PrimaryGeneratorAction* genAction)
  : fDet(detConstruction), fEventAction(eventAction), fGenAction(genAction) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
   // 前后逻辑体积
  auto* prePV  = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  auto* postPV = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
  if (!prePV || !postPV) return;
  auto* preVol  = prePV->GetLogicalVolume();
  auto* postVol = postPV->GetLogicalVolume();

  auto* DetectorLV = fDet->GetDetectorLogical();


  // 入射：跨入 Shield
  if (preVol != DetectorLV && postVol == DetectorLV) {
    G4Track* track = step->GetTrack(); 
    fEventAction->RecordEntry(
      track->GetParticleDefinition()->GetPDGEncoding(),
      step->GetPreStepPoint()->GetKineticEnergy(),
      step->GetPreStepPoint()->GetMomentumDirection(),
      step->GetPreStepPoint()->GetMomentum());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B4
