#ifndef TMACROPARTICLE
#define TMACROPARTICLE 1
/*
    Macroparticle tree class.
*/

#include <TString.h>

#include "TBlockPointMesh.hpp"
#include "TBlockPointVar.hpp"
#include "TSDFReader.hpp"

class TMacroParticle
{
 public:
  TMacroParticle(TSDFReader *reader, TString parName);
  ~TMacroParticle();

  void MakeTree();

 private:
  TSDFReader *fReader;
  TString fParName;

  // position information
  void FindMesh();
  TBlockPointMesh *fGrid;

  // particle parameters
  void FindVar();
  TBlockPointVar *FindBlockPointVar(TString id);

  TBlockPointVar *fPx;
  TBlockPointVar *fPy;
  TBlockPointVar *fPz;

  TBlockPointVar *fVx;
  TBlockPointVar *fVy;
  TBlockPointVar *fVz;

  TBlockPointVar *fTIWx;  // Time_Integrated_Work_x
  TBlockPointVar *fTIWy;
  TBlockPointVar *fTIWz;

  TBlockPointVar *fEk;  // What is the E? this block shows the kinetic ene.

  TBlockPointVar *fWeight;

  TBlockPointVar *fOptDep;

  TBlockPointVar *fQEDEne;

  TBlockPointVar *fID;
};

#endif
