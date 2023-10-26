#include "inc/inc.hpp"

void eventSelect(const std::string &resultdir) {
    gROOT->SetBatch();

    TFile *file = new TFile(Form("%s/raw.root", resultdir.c_str()));
    if (!file->IsOpen()) {
        std::cout << "rootfile not found" << std::endl;
        return;
    }
    TTree *tree = (TTree *)file->Get("tree");

    TH2F *dist[2];
    for (int i = 0; i < 2; i++) {
        dist[i] = new TH2F(Form("dist%i", i), Form("dist%i", i), 150, 0, 150,
                           150, -20000, 20000);
    }

    const int evtN = tree->GetEntries();
    const int clockN = 1024;
    float wfCh1[clockN] = {0};
    float wfCh2[clockN] = {0};
    float pedCh1 = 0, pedCh2 = 0;
    float adcCh1 = 0, adcCh2 = 0;
    float iadcCh1 = 0, iadcCh2 = 0;
    tree->SetBranchAddress("wfCh1", wfCh1);
    tree->SetBranchAddress("wfCh2", wfCh2);
    tree->SetBranchAddress("pedCh1", &pedCh1);
    tree->SetBranchAddress("pedCh2", &pedCh2);
    tree->SetBranchAddress("adcCh1", &adcCh1);
    tree->SetBranchAddress("adcCh2", &adcCh2);
    tree->SetBranchAddress("iadcCh1", &iadcCh1);
    tree->SetBranchAddress("iadcCh2", &iadcCh2);

    for (int event = 0; event < evtN; event++) {
        tree->GetEntry(event);
        dist[0]->Fill(adcCh1, iadcCh1);
        dist[1]->Fill(adcCh2, iadcCh2);
    }

    TF1 *fit1 = new TF1("fit1", "pol1");
    dist[0]->Fit("fit1", "q");
    const float itc1 = fit1->GetParameter(0);
    const float slp1 = fit1->GetParameter(1);

    TF1 *fit2 = new TF1("fit2", "pol1");
    dist[1]->Fit("fit2", "q");
    const float itc2 = fit2->GetParameter(0);
    const float slp2 = fit2->GetParameter(1);

    std::ofstream cutindex;
    cutindex.open(Form("%s/cutindex.dat", resultdir.c_str()), std::ios::out);

    const float cutwid = 1000;
    float thrmin1 = 0, thrmax1 = 0;
    float thrmin2 = 0, thrmax2 = 0;
    for (int event = 0; event < evtN; event++) {
        tree->GetEntry(event);
        thrmin1 = adcCh1 * slp1 + itc1 - cutwid;
        thrmax1 = adcCh1 * slp1 + itc1 + cutwid;
        thrmin2 = adcCh2 * slp2 + itc2 - cutwid;
        thrmax2 = adcCh2 * slp2 + itc2 + cutwid;
        if (thrmin1 < iadcCh1 && iadcCh1 < thrmax1) {
            cutindex << 1 << "\t";
        } else {
            cutindex << 0 << "\t";
        }
        if (thrmin2 < iadcCh2 && iadcCh2 < thrmax2) {
            cutindex << 1 << std::endl;
        } else {
            cutindex << 0 << std::endl;
        }
    }

    cutindex.close();
    file->Close();

    return;
}