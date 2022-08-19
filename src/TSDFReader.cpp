/*
 For Reading SDF file, output of EPOCH.
 Almost variable names are same as file format description.
*/

#include "TSDFReader.hpp"

#include <TString.h>

#include <iostream>

#include "TBlock.hpp"
#include "TBlockPlainMesh.hpp"
#include "TBlockPlainVar.hpp"
#include "TBlockPointMesh.hpp"
#include "TBlockPointVar.hpp"

using std::cout;
using std::endl;

TSDFReader::TSDFReader(TString fileName)
{
  fFileName = fileName;
  fInputFile = new std::ifstream(fFileName, std::ios::in | std::ios::binary);
  if (fInputFile->fail()) {
    std::cerr << "file " << fFileName << " is not found" << endl;
    exit(0);
  }

  ReadFileHeader();

  LoadBlocks();
}

TSDFReader::~TSDFReader() { fInputFile->close(); }

void TSDFReader::ReadFileHeader()
{
  Char_t sdf[4];
  fInputFile->read(sdf, sizeof(Char_t) * 4);
  cout << "sdf = " << sdf << endl;
  if (!TString(sdf).Contains("SDF1")) {  // Using contains for safety.
    std::cerr << "file " << fFileName << " is not SDF file" << endl;
    exit(0);
  }

  Int_t endianness;
  fInputFile->read((Char_t *)(&endianness), sizeof(endianness));
  cout << "endianness = " << endianness << endl;
  if (endianness != 16911887) {
    std::cerr
        << "This file have not same endian.  Converter is not implemented now."
        << endl;
    exit(0);
  }

  Int_t sdf_version;
  fInputFile->read((Char_t *)(&sdf_version), sizeof(sdf_version));
  cout << "sdf_version = " << sdf_version << endl;
  if (sdf_version > kSDFVersion) {
    std::cerr << "The version of file has to be newer than this reader. The "
                 "reader version = "
              << kSDFVersion << endl;
    exit(0);
  }

  Int_t sdf_revision;
  fInputFile->read((Char_t *)(&sdf_revision), sizeof(sdf_revision));
  cout << "sdf_revision = " << sdf_revision << endl;
  if (sdf_version == kSDFVersion && sdf_revision > kSDFRevision) {
    std::cerr << "The revision of file is newer than this reader. The reader "
                 "revision = "
              << kSDFRevision << endl;
  }

  Char_t code_name[32];
  fInputFile->read(code_name, sizeof(Char_t) * 32);
  cout << "code_name = " << code_name << endl;

  fInputFile->read((Char_t *)(&fNextBlockLocation), sizeof(fNextBlockLocation));
  cout << "first_block_location = " << fNextBlockLocation << endl;

  Long_t summary_location;
  fInputFile->read((Char_t *)(&summary_location), sizeof(summary_location));
  cout << "summary_location = " << summary_location << endl;

  Int_t summary_size;
  fInputFile->read((Char_t *)(&summary_size), sizeof(summary_size));
  cout << "summary_size = " << summary_size << endl;

  Int_t nblocks;
  fInputFile->read((Char_t *)(&nblocks), sizeof(nblocks));
  cout << "nblocks = " << nblocks << endl;
  if (nblocks == 0) {
    std::cerr << "This file is not finished." << endl;
    exit(0);
  }
  fNBlocks = nblocks;

  fInputFile->read((Char_t *)(&fBlockHeaderLength), sizeof(fBlockHeaderLength));
  cout << "block_header_length = " << fBlockHeaderLength << endl;

  fInputFile->read((Char_t *)(&fStep), sizeof(fStep));
  cout << "step = " << fStep << endl;

  fInputFile->read((Char_t *)(&fTime), sizeof(fTime));
  cout << "time = " << fTime << endl;

  fInputFile->read((Char_t *)(&fJobID1), sizeof(fJobID1));
  cout << "jobid1 = " << fJobID1 << endl;

  fInputFile->read((Char_t *)(&fJobID2), sizeof(fJobID2));
  cout << "jobid2 = " << fJobID2 << endl;

  fInputFile->read((Char_t *)(&fStringLength), sizeof(fStringLength));
  cout << "string_length = " << fStringLength << endl;

  Int_t code_io_version;
  fInputFile->read((Char_t *)(&code_io_version), sizeof(code_io_version));
  cout << "code_io_version = " << code_io_version << endl;

  Char_t restart_flag;
  fInputFile->read(&restart_flag, sizeof(restart_flag));
  cout << "restart_flag = " << Int_t(restart_flag) << endl;

  Char_t subdomain_file;
  fInputFile->read(&subdomain_file, sizeof(subdomain_file));
  cout << "subdomain_file = " << Int_t(subdomain_file) << endl;
}

void TSDFReader::LoadBlocks()
{
  // Load run_info block
  fBlock.push_back(new TBlock(fInputFile, fNextBlockLocation, fStringLength,
                              fBlockHeaderLength));

  for (Int_t i = 0; i < fNBlocks - 1;
       i++) {  // push_back means adding to (i + 1)
    fNextBlockLocation = fBlock[i]->GetNextLocation();
    Int_t blockType = GetNextBlockType();
    switch (blockType) {
      case c_blocktype_plain_mesh:
        fBlock.push_back(new TBlockPlainMesh(
            fInputFile, fNextBlockLocation, fStringLength, fBlockHeaderLength));
        break;
      case c_blocktype_plain_variable:
        fBlock.push_back(new TBlockPlainVar(fInputFile, fNextBlockLocation,
                                            fStringLength, fBlockHeaderLength));
        break;
      case c_blocktype_point_mesh:
        fBlock.push_back(new TBlockPointMesh(
            fInputFile, fNextBlockLocation, fStringLength, fBlockHeaderLength));
        break;
      case c_blocktype_point_variable:
        fBlock.push_back(new TBlockPointVar(fInputFile, fNextBlockLocation,
                                            fStringLength, fBlockHeaderLength));
        break;
      case c_blocktype_run_info:
        fBlock.push_back(new TBlock(fInputFile, fNextBlockLocation,
                                    fStringLength, fBlockHeaderLength));
        break;
      default:
        fBlock.push_back(new TBlock(fInputFile, fNextBlockLocation,
                                    fStringLength, fBlockHeaderLength));
        break;
    }
    fBlock[i + 1]->ReadMetadata();
    fBlock[i + 1]->PrintHeader();
    fBlock[i + 1]->PrintMetadata();
  }
}

Int_t TSDFReader::GetNextBlockType()
{
  Int_t blockType;
  fInputFile->seekg(fNextBlockLocation + kBlockTypeOffset, std::ios::beg);
  fInputFile->read((Char_t *)&blockType, sizeof(blockType));

  return blockType;
}

Int_t TSDFReader::GetBlockIndex(TString ID)
{
  ID.ToLower();
  ID.ReplaceAll(" ", "");  // Sanitize.  I should do partial match searching?
  for (Int_t i = 0; i < GetNBlocks(); i++) {
    TString blockID = fBlock[i]->GetID();
    blockID.ToLower();
    blockID.ReplaceAll(" ", "");  // Sanitize

    if (ID == blockID) return i;
  }

  return -1;
}

Int_t TSDFReader::GetBlockIndexByName(TString ID)
{
  ID.ToLower();
  ID.ReplaceAll(" ", "");  // Sanitize.  I should do partial match searching?
  for (Int_t i = 0; i < GetNBlocks(); i++) {
    TString blockID = fBlock[i]->GetName();
    blockID.ToLower();
    blockID.ReplaceAll(" ", "");  // Sanitize

    if (ID == blockID) return i;
  }

  return -1;
}
