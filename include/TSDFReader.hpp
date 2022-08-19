#ifndef TSDFREADER
#define TSDFREADER 1

#include <TString.h>

#include <fstream>
#include <vector>

#include "TBlock.hpp"

const Int_t kSDFVersion = 1;
const Int_t kSDFRevision = 1;

const Int_t kBlockTypeOffset = 56;

class TSDFReader
{
 public:
  TSDFReader(TString fileName = "0300.sdf");
  virtual ~TSDFReader();

  Int_t GetNBlocks() { return fNBlocks; };

  TBlock *GetBlock(Int_t i) { return fBlock[i]; };
  std::vector<TBlock *> fBlock;

  TString GetFileName() { return fFileName; };

  Int_t GetStep() { return fStep; };
  Double_t GetTime() { return fTime; };

  Int_t GetBlockIndex(TString ID);
  Int_t GetBlockIndexByName(TString ID);

 private:
  void LoadBlocks();
  Int_t GetNextBlockType();

  TString fFileName;
  std::ifstream *fInputFile;

  void ReadFileHeader();
  Int_t fJobID1;
  Int_t fJobID2;
  Int_t fStringLength;
  Int_t fBlockHeaderLength;
  Long_t fNextBlockLocation;  // seek position
  Int_t fNBlocks;

  Int_t fStep;
  Double_t fTime;
};

// Constants for block, meta data and data

const Int_t c_blocktype_scrubbed = -1;
const Int_t c_blocktype_null = 0;
const Int_t c_blocktype_plain_mesh = 1;
const Int_t c_blocktype_point_mesh = 2;
const Int_t c_blocktype_plain_variable = 3;
const Int_t c_blocktype_point_variable = 4;
const Int_t c_blocktype_constant = 5;
const Int_t c_blocktype_array = 6;
const Int_t c_blocktype_run_info = 7;
const Int_t c_blocktype_source = 8;
const Int_t c_blocktype_stitched_tensor = 9;
const Int_t c_blocktype_stitched_material = 10;
const Int_t c_blocktype_stitched_matvar = 11;
const Int_t c_blocktype_stitched_species = 12;
const Int_t c_blocktype_species = 13;
const Int_t c_blocktype_plain_derived = 14;
const Int_t c_blocktype_point_derived = 15;
const Int_t c_blocktype_contiguous_tensor = 16;
const Int_t c_blocktype_contiguous_material = 17;
const Int_t c_blocktype_contiguous_matvar = 18;
const Int_t c_blocktype_contiguous_species = 19;
const Int_t c_blocktype_cpu_split = 20;
const Int_t c_blocktype_stitched_obstacle_group = 21;
const Int_t c_blocktype_unstructured_mesh = 22;
const Int_t c_blocktype_stitched = 23;
const Int_t c_blocktype_contiguous = 24;
const Int_t c_blocktype_lagrangian_mesh = 25;
const Int_t c_blocktype_station = 26;
const Int_t c_blocktype_station_derived = 27;
const Int_t c_blocktype_datablock = 28;
const Int_t c_blocktype_namevalue = 29;
const Int_t c_blocktype_max = 29;

const Int_t c_datatype_null = 0;
const Int_t c_datatype_integer4 = 1;
const Int_t c_datatype_integer8 = 2;
const Int_t c_datatype_real4 = 3;
const Int_t c_datatype_real8 = 4;
const Int_t c_datatype_real16 = 5;
const Int_t c_datatype_character = 6;
const Int_t c_datatype_logical = 7;
const Int_t c_datatype_other = 8;
const Int_t c_datatype_max = 8;

#endif
