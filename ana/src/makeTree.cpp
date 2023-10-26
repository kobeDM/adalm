#include "inc/inc.hpp"

void makeTree(const std::string &datafile, const std::string &outdirpath) {
    TFile *file =
        new TFile(Form("%s/raw.root", outdirpath.c_str()), "recreate");
    TTree *tree = new TTree("tree", "tree");

    const int clockN = 1024;
    float wfCh1[clockN];
    float wfCh2[clockN];
    float pedCh1;
    float pedCh2;
    float adcCh1;
    float adcCh2;
    float iadcCh1;
    float iadcCh2;

    tree->Branch("wfCh1", wfCh1, Form("wfCh1[%i]/F", clockN));
    tree->Branch("wfCh2", wfCh2, Form("wfCh2[%i]/F", clockN));
    tree->Branch("pedCh1", &pedCh1, "pedCh1/F");
    tree->Branch("pedCh2", &pedCh2, "pedCh2/F");
    tree->Branch("adcCh1", &adcCh1, "adcCh1/F");
    tree->Branch("adcCh2", &adcCh2, "adcCh2/F");
    tree->Branch("iadcCh1", &iadcCh1, "iadcCh1/F");
    tree->Branch("iadcCh2", &iadcCh2, "iadcCh2/F");

    std::ifstream ifs(datafile);
    if (ifs.is_open() == false) {
        std::cout << "datafile input error" << std::endl;
        return;
    }

    float ch1 = 0, ch2 = 0;
    int clock = 0;

    float ch1Min = 1023, ch2Min = 1023;
    float ch1Max = -1024, ch2Max = -1024;
    int evtID = 0;
    float ch1Sum = 0, ch2Sum = 0;

    while (!ifs.eof()) {
        std::string line = "";
        std::getline(ifs, line);
        if (line.length() <= 0 || strncmp(line.c_str(), "#", 1) == 0) {
            if (evtID != 0 && evtID % 3 == 2) {
                pedCh1 = ch1Sum / clock;
                pedCh2 = ch2Sum / clock;
                if (std::abs(ch1Max) > std::abs(ch1Min))
                    adcCh1 = ch1Max - pedCh1;
                else
                    adcCh1 = pedCh1 - ch1Min;
                if (std::abs(ch2Max) > std::abs(ch2Min))
                    adcCh2 = ch2Max - pedCh2;
                else
                    adcCh2 = pedCh2 - ch2Min;
                iadcCh1 = ch1Sum;
                iadcCh2 = ch2Sum;
                tree->Fill();
            }
            ++evtID;
            clock = 0;
            ch1Min = 1023;
            ch2Min = 1023;
            ch1Max = -1024;
            ch2Max = -1024;
            ch1Sum = 0;
            ch2Sum = 0;
            continue;
        }
        std::stringstream ss(line);
        ss >> ch1 >> ch2;

        if (ch1 < ch1Min)
            ch1Min = ch1;
        else if (ch1 > ch1Max)
            ch1Max = ch1;

        if (ch2 < ch2Min)
            ch2Min = ch2;
        else if (ch2 > ch2Max)
            ch2Max = ch2;

        ch1Sum += ch1;
        ch2Sum += ch2;
        wfCh1[clock] = ch1;
        wfCh2[clock] = ch2;
        ++clock;
    }

    tree->Write();
    file->Close();

    return;
}