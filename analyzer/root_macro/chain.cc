#include "TChain.h"

void chain(string header){
	TChain *chain = new TChain("tree","tree");
	chain->Add(Form("root_data/%s*.root",header.c_str()));
	chain->Merge(Form("%s.root",header.c_str()),"recreate");
}


