#include <TString.h>

#include <iostream>

#include "RunMode.hpp"
#include "TConverter.hpp"
#include "TSDFReader.hpp"

using std::cout;
using std::endl;

void PrintUsage()
{
  cout << "Usage\n"
       << "sdf2root [option] input file name\n"
       << "-a: all information (mesh and macro particle)\n"
       << "-m: all mesh information\n"
       << "-p: all macro particle\n"
       << endl;
}

int main(int argc, char **argv)
{
  TString inputName{""};
  TString outputName{""};
  RunMode runMode = RunMode::Interactive;

  // check arguments
  for (int i = 1; i < argc; i++) {
    cout << argv[i] << endl;
    TString arg{argv[i]};
    if (arg == "-h") {
      PrintUsage();
      return 0;
    } else if (arg.Contains(".sdf")) {
      inputName = arg;
      outputName = arg(arg.Last(TString("/")[0]) + 1,
                       arg.Index(".sdf") - (arg.Last(TString("/")[0]) +
                                            1));  // can you beliebe this shit?
      cout << inputName << endl;
      cout << outputName << endl;
    } else if (arg == "-a")
      runMode = RunMode::AllInfo;
    else if (arg == "-p")
      runMode = RunMode::AllParticles;
    else if (arg == "-m")
      runMode = RunMode::AllMeshes;
    else {
      PrintUsage();
      return 0;
    }
  }

  if (inputName == "") {
    PrintUsage();
    return 0;
  }

  // Reading file information
  TSDFReader *reader = new TSDFReader(inputName);

  Int_t counter = 0;
  for (auto block : reader->fBlock) {
    cout << counter++ << "\t" << block->GetBlockType() << "\t"
         << block->GetDataType() << "\t" << block->GetName() << "\t"
         << block->GetID() << "\t" << endl;
  }

  TConverter *converter = new TConverter(reader, outputName, runMode);
  converter->GetData();
  delete converter;

  delete reader;

  return 0;
}
