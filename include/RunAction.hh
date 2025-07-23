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
/// \file B4/B4a/include/RunAction.hh
/// \brief Definition of the B4::RunAction class

#ifndef B4RunAction_h
#define B4RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4Accumulable.hh"
#include "G4AccumulableManager.hh"
#include "G4AnalysisManager.hh"


namespace B4
{

class PrimaryGeneratorAction;
class DetectorConstruction;
class RunActionMessenger;

/// Run action class
///
/// This class create the tuple at the RunAction function.
/// It accumulates statistic and computes dispersion of the energy deposit
/// and direction of particles passed the shielding layer.
///
/// In EndOfRunAction(),the shield information, particle information is printed.
/// And analysis is saved in the ROOT file.

class RunAction : public G4UserRunAction
{
  public:
    RunAction(bool isMaster, PrimaryGeneratorAction* genAction, DetectorConstruction* det);
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run* ) override;
    void EndOfRunAction(const G4Run* ) override;

    // set messenger interface
    void SetEnableOutput(bool flag) { fEnableOutput = flag; }
    void SetFileName(G4String& name) { fFileName = name; }
    void SetDirectory(G4String& dir) { fDirectory = dir; }

    bool IsOutputEnabled() const { return fEnableOutput; }

    // define counters
    void AddPassedParticles(G4int n) {fPassed += n;}
    void AddBlockedParticles(G4int n) { fBlocked += n; }

  private:
    const  bool fIsMaster;
    PrimaryGeneratorAction* fGenAction;
    DetectorConstruction* fDet;
    G4Accumulable<G4int> fPassed;
    G4Accumulable<G4int> fBlocked;
    G4AnalysisManager* fAnalysisManager;
    RunActionMessenger* fRunMessenger;

    bool fEnableOutput;
    G4String fFileName;
    G4String fDirectory;

    G4String fMaterial;
    G4String fPtype;
    G4double fEnergy;
    
    G4double fTargetLength;
    G4double fTargetRadius;
    G4String fTargetMaterial;

    // 防止拷贝？
    RunAction(const RunAction&) = delete;
    RunAction& operator =(const RunAction&) = delete;
};

}  // namespace B4

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
