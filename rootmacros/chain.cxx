#include "inc/inc.hpp"

void chain(const std::string &dirpath)
{
    TFile *file = new TFile(Form("%s/rawChain.root", dirpath.c_str()), "RECREATE");
    TChain *tree = new TChain("tree", "tree");

    std::ifstream ifs(Form("%s/chain.txt", dirpath.c_str()));
    if (ifs.is_open() == false)
    {
        std::cout << "rootfile list not found" << std::endl;
        return;
    }

    while (!ifs.eof())
    {
        std::string line = "";
        std::getline(ifs, line);
        // std::cout << line << std::endl;
        if (line != "")
            tree->Add(Form("%s", line.c_str()));
    }

    tree->Write();
    file->Close();

    return;
}