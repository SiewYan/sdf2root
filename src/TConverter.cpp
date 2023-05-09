#include <iostream>

#include <TThread.h>
#include <TVector.h>

#include "TConverter.hpp"
#include "TMacroParticle.hpp"
#include "TProbe.hpp"

using std::cout;
using std::endl;

TConverter::TConverter(TSDFReader *reader, TString outName, RunMode runMode)
    : fReader(reader), fOutName(outName), fRunMode(runMode)
{
  FindPar();
  FindProbe();
}

TConverter::~TConverter() {}

void TConverter::GetData()
{
  if (fRunMode == RunMode::AllInfo || fRunMode == RunMode::AllMeshes) {
    std::cout<<"Making histograms"<< std::endl;
    TString name = fOutName + "_MeshInfo.root";
    TFile *file = new TFile(name, "RECREATE");

    // Save time info
    TVectorD time(1);
    time[0] = fReader->GetTime();
    time.Write("Time");

    // Mesh data
    auto mesh = new TMeshValue(fReader);
    mesh->SetAllOn();
    mesh->GetMeshData();

    file->Close();

    delete file;
    delete mesh;
  }
  if (fRunMode == RunMode::AllInfo || fRunMode == RunMode::AllParticles) {
    // for(auto name: fParName){
    std::cout<<"making Ntuple"<< std::endl;
    for (Int_t i = 0; i < fParName.size(); i++) {
      TString name = fParName[i];
      TString goodName = fParName[i];  // Stupid name!!
      goodName.ReplaceAll("/", "_");
      TString fileName = fOutName + "_" + goodName + ".root";
      
      std::cout<<"name : "<<name<< std::endl;
      std::cout<<"goodName : "<< goodName << std::endl;

      TFile *file = new TFile(fileName, "RECREATE");
      cout << name << endl;
      auto par = new TMacroParticle(fReader, name);
      par->MakeTree();
      file->Close();
      delete file;
      delete par;
    }
    for (Int_t i = 0; i < fProbeName.size(); i++) {
      TString name = fProbeName[i];
      TString goodName = fProbeName[i];  // Stupid name!!
      goodName.ReplaceAll("/", "_");
      TString fileName = fOutName + "_" + goodName + ".root";
      TFile *file = new TFile(fileName, "RECREATE");
      cout << name << endl;
      auto par = new TProbe(fReader, name);
      par->MakeTree();
      file->Close();
      delete file;
      delete par;
    }
  }
}

void TConverter::FindPar()
{
  // Using Px for finding particles.
  // In case of not recording Px, what is the best?
  // for (Int_t i = 0; i < fReader->GetNBlocks(); i++) {
  //   TString id = fReader->fBlock[i]->GetID();
  //   TString px = id(0, 3);
  //   if (px == "px/") {
  //     id.Remove(0, 3);  // 3 means size of "px/"
  //     fParName.push_back(id);
  //   }
  // }

  TString keyName = "Grid/Particles/";
  for (Int_t i = 0; i < fReader->GetNBlocks(); i++) {
    TString blockName = fReader->GetBlock(i)->GetName();
    //std::cout<<"FindPar, blockName : "<< blockName << std::endl;
    if (blockName.Index(keyName) >= 0) {
      TString parName = blockName.ReplaceAll(keyName, "");
      //std::cout<<"FindPar, parName : "<<	parName << std::endl;
      fParName.push_back(parName);
    }
  }
}

void TConverter::FindProbe()
{
  TString keyName = "Grid/Probe/";
  for (Int_t i = 0; i < fReader->GetNBlocks(); i++) {
    TString blockName = fReader->GetBlock(i)->GetName();
    //std::cout<<"FindProbe, blockName : "<< blockName << std::endl;
    if (blockName.Index(keyName) >= 0) {
      TString probeName = blockName.ReplaceAll(keyName, "");
      //std::cout<<"FindProbe, probeName : "<< probeName << std::endl;
      fProbeName.push_back(probeName);
    }
  }
}
