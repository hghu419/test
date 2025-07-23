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
/// \file B4/B4a/include/SteppingAction.hh
/// \brief Definition of the B4a::SteppingAction class

#ifndef B4SteppingAction_h
#define B4SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"


namespace B4{

  class EventAction;
  class DetectorConstruction;
  class PrimaryGeneratorAction;


  /// Stepping action class.
  ///
  /// SteppingAction counts particles passed the shield,
  /// and define algorithm for getting the energy of particles passed the shield.
  /// then transmit the energy to the event action.

class SteppingAction : public G4UserSteppingAction{

  public:
    SteppingAction(DetectorConstruction* detConstruction, EventAction* eventAction, PrimaryGeneratorAction* genAction);
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;

  private:
    PrimaryGeneratorAction* fGenAction;
    DetectorConstruction* fDet;
    EventAction* fEventAction;
};

}  // namespace B4

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
