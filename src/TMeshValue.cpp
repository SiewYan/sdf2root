#include <iostream>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>

#include "TMeshValue.hpp"
#include "TBlockPlainMesh.hpp"
#include "TBlockPlainVar.hpp"


using std::cout;
using std::endl;

TMeshValue::TMeshValue(TSDFReader *reader)
   : fReader(reader)
{
   //ReadMeshGrid();
   GetListOfMesh();
}

TMeshValue::~TMeshValue()
{}

void TMeshValue::ReadMeshGrid(TString id)
{
   Int_t index = fReader->GetBlockIndex(id);
   if(index < 0) index = fReader->GetBlockIndex("grid"); // default
   
   TBlockPlainMesh *block = (TBlockPlainMesh*)fReader->fBlock[index];

   block->ReadMetadata();
   Int_t nGrids[3];
   for(Int_t i = 0; i < 3; i++){
      fHisMin[i] = block->GetMinVal(i);
      fHisMax[i] = block->GetMaxVal(i);
      nGrids[i] = block->GetNGrids(i);
      fHisUnit[i] = block->GetUnits(i);
      fHisLabel[i] = block->GetAxisLabel(i);
   }

   block->ReadData();
   fHisBinWidth[0] = fabs(block->GetData(1) - block->GetData(0));
   if(nGrids[1] > 1) fHisBinWidth[1] = fabs(block->GetData(1 + nGrids[0]) - block->GetData(nGrids[0]));
   if(nGrids[2] > 1) fHisBinWidth[2] = fabs(block->GetData(1 + nGrids[0] + nGrids[1])
                                            - block->GetData(nGrids[0] + nGrids[1]));
}

void TMeshValue::GetListOfMesh()
{
   for(auto block: fReader->fBlock){
      if(block->GetBlockType() == 3){
         MeshPar meshPar;
         TString id = block->GetID();
         if(id == "ex") meshPar = MeshPar(id, "Ex", "Electric field x", kFALSE);
         else if(id == "ey") meshPar = MeshPar(id, "Ey", "Electric field y", kFALSE);
         else if(id == "ez") meshPar = MeshPar(id, "Ez", "Electric field z", kFALSE);
         else if(id == "bx") meshPar = MeshPar(id, "Bx", "Magnetic field x", kFALSE);
         else if(id == "by") meshPar = MeshPar(id, "By", "Magnetic field y", kFALSE);
         else if(id == "bz") meshPar = MeshPar(id, "Bz", "Magnetic field z", kFALSE);
         else if(id == "jx") meshPar = MeshPar(id, "Jx", "Current x", kFALSE);
         else if(id == "jy") meshPar = MeshPar(id, "Jy", "Current y", kFALSE);
         else if(id == "jz") meshPar = MeshPar(id, "Jz", "Current z", kFALSE);
         else{
            TString name = id;
            name.ReplaceAll("/", "_");
            TString title = id;
            title.ReplaceAll("/", " ");
            meshPar = MeshPar(id, name, title, kFALSE);
         }

         fMeshList.push_back(meshPar);
      }
   }
}

void TMeshValue::SetAllOn()
{
   for(auto &mesh: fMeshList) mesh.Use = kTRUE;
}

void TMeshValue::GetMeshData()
{
   for(auto &mesh: fMeshList){
      if(mesh.Use == kTRUE){
	cout << "Gettting " << mesh.ID <<"\t"<< mesh.Name <<"\t"<< mesh.Title << endl;
	GetMeshHis(mesh.ID, mesh.Name, mesh.Title);
      }
   }

}

// this is where you make histo
void TMeshValue::GetMeshHis(TString id, TString hisName, TString hisTitle)
{
   TH1 *his{nullptr};

   Int_t index = fReader->GetBlockIndex(id);
   if(index < 0) return;

   ReadMeshGrid("grid/" + id);
   
   TBlockPlainVar *block = (TBlockPlainVar*)fReader->fBlock[index];
   block->ReadData();

   TString name = hisName;
   TString title = hisTitle;
   Double_t norm = block->GetNormFactor();

   Int_t nBins[3] = {
      block->GetNGrids(0),
      block->GetNGrids(1),
      block->GetNGrids(2),
   };
   
   Int_t stagger = block->GetStagger();
   Int_t dim = block->GetNDims();
   
   Double_t delta[3] = {
      fHisBinWidth[0] / 2.,
      fHisBinWidth[1] / 2.,
      fHisBinWidth[2] / 2.,      
   };

   // stagger.  Don't use bit mask.  Use number
   // Check, is this right or not.
   Double_t shift[3] = {0., 0., 0.};
   if(stagger == 1)
      shift[0] += delta[0];
   else if(stagger == 2)
      shift[1] += delta[1];
   else if(stagger == 4)
      shift[2] += delta[2];
   else if(stagger == 3){
      shift[0] += delta[0];
      shift[1] += delta[1];
   }   
   else if(stagger == 5){
      shift[0] += delta[0];
      shift[2] += delta[2];
   }   
   else if(stagger == 6){
      shift[1] += delta[1];
      shift[2] += delta[2];
   }   
   else if(stagger == 7){
      shift[0] += delta[0];
      shift[1] += delta[1];
      shift[2] += delta[2];
   }
   else if(stagger != 0){
      cout << "stagger error: " << stagger << endl;
   }
   
   if(dim == 1){
      his = new TH1D(name, title,
                     nBins[0], fHisMin[0] - delta[0] + shift[0], fHisMax[0] + delta[0] + shift[0]);
      his->SetXTitle(fHisLabel[0] + " [" + fHisUnit[0] + "]");
      for(Int_t x = 1; x <= nBins[0]; x++){
         his->SetBinContent(x, norm * block->GetData(x - 1));
	 std::cout<<"x : "<< x << " ; norm : " << norm << " ; block->GetData(x - 1) : " << block->GetData(x - 1) << " ; tot : " << norm * block->GetData(x - 1) << std::endl;
      }
   }
   else if(dim == 2){
      his = new TH2D(name, title,
                     nBins[0], fHisMin[0] - delta[0] + shift[0], fHisMax[0] + delta[0] + shift[0],
                     nBins[1], fHisMin[1] - delta[1] + shift[1], fHisMax[1] + delta[1] + shift[1]);
      his->SetXTitle(fHisLabel[0] + " [" + fHisUnit[0] + "]");
      his->SetYTitle(fHisLabel[1] + " [" + fHisUnit[1] + "]");
      for(Int_t x = 1; x <= nBins[0]; x++){
         for(Int_t y = 1; y <= nBins[1]; y++){
            Int_t i = (x - 1) + ((y - 1) * nBins[0]);
            his->SetBinContent(x, y, norm * block->GetData(i));
         }
      }
   }
   else if(dim == 3){ // TH3D makes size error of something... I should check whats happen
      his = new TH3F(name, title,
                     nBins[0], fHisMin[0] - delta[0] + shift[0], fHisMax[0] + delta[0] + shift[0],
                     nBins[1], fHisMin[1] - delta[1] + shift[1], fHisMax[1] + delta[1] + shift[1],
                     nBins[2], fHisMin[2] - delta[2] + shift[2], fHisMax[2] + delta[2] + shift[2]);
      his->SetXTitle(fHisLabel[0] + " [" + fHisUnit[0] + "]");
      his->SetYTitle(fHisLabel[1] + " [" + fHisUnit[1] + "]");
      his->SetZTitle(fHisLabel[2] + " [" + fHisUnit[2] + "]");
      for(Int_t x = 1; x <= nBins[0]; x++){
         for(Int_t y = 1; y <= nBins[1]; y++){
            for(Int_t z = 1; z <= nBins[2]; z++){
               Int_t i = (x - 1) + ((y - 1) * nBins[0]) + ((z - 1) * nBins[0] * nBins[1]);
               his->SetBinContent(x, y, z, Float_t(norm * block->GetData(i)));
            }
         }
      }
   }

   //block->PrintHeader();
   //block->PrintMetadata();

   his->Write();
   delete his;
}

