#include "TMacroParticle.hpp"

#include <TTree.h>

#include "TBlockPointMesh.hpp"
#include "TBlockPointVar.hpp"

using std::cout;
using std::endl;

TMacroParticle::TMacroParticle(TSDFReader *reader, TString parName)
    : fReader(reader),
      fParName(parName),
      fGrid(nullptr),
      fPx(nullptr),
      fPy(nullptr),
      fPz(nullptr),
      fVx(nullptr),
      fVy(nullptr),
      fVz(nullptr),
      fTIWx(nullptr),
      fTIWy(nullptr),
      fTIWz(nullptr),
      fEk(nullptr),
      fWeight(nullptr),
      fOptDep(nullptr),
      fQEDEne(nullptr),
      fID(nullptr)
{
  FindMesh();
  FindVar();
}

TMacroParticle::~TMacroParticle() {}

void TMacroParticle::FindMesh()
{
  TString meshID = "grid/" + fParName;
  Int_t index = fReader->GetBlockIndex(meshID);
  if (index < 0) return;
  fGrid = (TBlockPointMesh *)fReader->fBlock[index];
  fGrid->ReadMetadata();
  fGrid->ReadData();  // This shoud be changed.
}

void TMacroParticle::FindVar()
{
  fPx = FindBlockPointVar("px/" + fParName);
  fPy = FindBlockPointVar("py/" + fParName);
  fPz = FindBlockPointVar("pz/" + fParName);

  fVx = FindBlockPointVar("vx/" + fParName);
  fVy = FindBlockPointVar("vy/" + fParName);
  fVz = FindBlockPointVar("vz/" + fParName);

  fTIWx = FindBlockPointVar("time_integrated_work_x/" + fParName);
  fTIWy = FindBlockPointVar("time_integrated_work_y/" + fParName);
  fTIWz = FindBlockPointVar("time_integrated_work_z/" + fParName);

  fEk = FindBlockPointVar("ek/" + fParName);

  fWeight = FindBlockPointVar("weight/" + fParName);

  fOptDep = FindBlockPointVar("optical depth/" + fParName);

  fQEDEne = FindBlockPointVar("qed energy/" + fParName);

  fID = FindBlockPointVar("id/" + fParName);
}

TBlockPointVar *TMacroParticle::FindBlockPointVar(TString id)
{
  id = "particles/" + id;
  Int_t index = fReader->GetBlockIndexByName(id);
  if (index < 0) return nullptr;
  TBlockPointVar *block = (TBlockPointVar *)fReader->fBlock[index];
  block->ReadMetadata();
  block->ReadData();  // This shoud be changed.

  return block;
}

void TMacroParticle::MakeTree()
{
  TString treeName = fParName;
  treeName.ReplaceAll("/", "_");
  TTree *tree = new TTree(treeName, "particle information");

  // Using similar name is not so good.  Block: fPx, Variable: Px.
  // Position
  Double_t x;
  Double_t y;
  Double_t z;
  if (fGrid) {
    tree->Branch("x", &x, "x/D");
    tree->Branch("y", &y, "y/D");
    tree->Branch("z", &z, "z/D");
  }

  // Momentum
  Double_t Px;
  if (fPx) tree->Branch("Px", &Px, "Px/D");
  Double_t Py;
  if (fPy) tree->Branch("Py", &Py, "Py/D");
  Double_t Pz;
  if (fPz) tree->Branch("Pz", &Pz, "Pz/D");

  // Velocity
  Double_t Vx;
  if (fVx) tree->Branch("Vx", &Vx, "Vx/D");
  Double_t Vy;
  if (fVy) tree->Branch("Vy", &Vy, "Vy/D");
  Double_t Vz;
  if (fVz) tree->Branch("Vz", &Vz, "Vz/D");

  // Time Integrated Work
  Double_t TIWx;
  if (fTIWx) tree->Branch("TIWx", &TIWx, "TIWx/D");
  Double_t TIWy;
  if (fTIWy) tree->Branch("TIWy", &TIWy, "TIWy/D");
  Double_t TIWz;
  if (fTIWz) tree->Branch("TIWz", &TIWz, "TIWz/D");

  // Kinetic energy
  Double_t Ek;
  if (fEk) tree->Branch("Ek", &Ek, "Ek/D");

  // Weight
  Double_t Weight;
  if (fWeight) tree->Branch("Weight", &Weight, "Weight/D");

  // Optical depth
  Double_t optDep;
  if (fOptDep) tree->Branch("OptDep", &optDep, "OptDep/D");

  // QED energy
  Double_t QEDEne;
  if (fQEDEne) tree->Branch("QEDEne", &QEDEne, "QEDEne/D");

  // Particle ID
  // Don't use PARTICLE_ID4.
  Long64_t id;
  if (fID) tree->Branch("ID", &id, "ID/L");

  Long64_t kNoPar = 0;
  if (fPx)
    kNoPar = fPx->GetDataSize();
  else if (fTIWx)
    kNoPar = fTIWx->GetDataSize();

  for (Long64_t i = 0; i < kNoPar; i++) {
    if (i % 1000000 == 0)
      cout << i << " / " << kNoPar << " (" << i * 100. / kNoPar << " %)"
           << endl;

    if (fPx) Px = fPx->GetData(i);
    if (fPy) Py = fPy->GetData(i);
    if (fPz) Pz = fPz->GetData(i);

    if (fVx) Vx = fVx->GetData(i);
    if (fVy) Vy = fVy->GetData(i);
    if (fVz) Vz = fVz->GetData(i);

    if (fTIWx) TIWx = fTIWx->GetData(i);
    if (fTIWy) TIWy = fTIWy->GetData(i);
    if (fTIWz) TIWz = fTIWz->GetData(i);

    if (fEk) Ek = fEk->GetData(i);

    if (fWeight) Weight = fWeight->GetData(i);

    if (fOptDep) optDep = fOptDep->GetData(i);

    if (fQEDEne) QEDEne = fQEDEne->GetData(i);

    if (fID) id = fID->GetID(i);

    if (fGrid) {
      x = fGrid->GetData(i);
      if (fGrid->GetNDims() > 1) y = fGrid->GetData(i + kNoPar);
      if (fGrid->GetNDims() > 2) z = fGrid->GetData(i + kNoPar + kNoPar);
    }
    tree->Fill();
  }
  cout << kNoPar << " events done." << endl;

  tree->Write();
  delete tree;
}
