#include "../include/inc.hpp"

void makeTree(const std::string &datafile, const std::string &outdirpath)
{
    // get soft path
    const char *SOFT_PATH = std::getenv("ADSW");
    std::string soft_path(std::getenv("ADSW"));
    if (soft_path.empty())
    {
        std::cout << "ADSW environment variable is not set" << std::endl;
        return;
    }

    // get json data
    const std::string jsonfile = Form("%s/config/config.json", soft_path.c_str());
    boost::property_tree::ptree pt;
    read_json(jsonfile, pt);
    boost::optional<float> thr_ch1_cfg = pt.get_optional<float>("ana.thr_ch1");
    boost::optional<float> thr_ch2_cfg = pt.get_optional<float>("ana.thr_ch2");
    const float thr_ch1 = thr_ch1_cfg.get();
    const float thr_ch2 = thr_ch2_cfg.get();

    // make rootfile
    TFile *file =
        new TFile(Form("%s/raw.root", outdirpath.c_str()), "recreate");
    TTree *tree = new TTree("tree", "tree");

    const int clockN = 1024;
    int wfCh1[clockN];
    int wfCh2[clockN];
    int pedCh1;
    int pedCh2;
    int adcCh1;
    int adcCh2;
    int iadcCh1;
    int iadcCh2;
    int adcMaxCh1;
    int adcMaxCh2;
    int adcMinCh1;
    int adcMinCh2;

    tree->Branch("wfCh1", wfCh1, Form("wfCh1[%i]/I", clockN));
    tree->Branch("wfCh2", wfCh2, Form("wfCh2[%i]/I", clockN));
    tree->Branch("pedCh1", &pedCh1, "pedCh1/I");
    tree->Branch("pedCh2", &pedCh2, "pedCh2/I");
    tree->Branch("adcCh1", &adcCh1, "adcCh1/I");
    tree->Branch("adcCh2", &adcCh2, "adcCh2/I");
    tree->Branch("adcCh2", &adcCh2, "adcCh2/I");
    tree->Branch("adcCh2", &adcCh2, "adcCh2/I");
    tree->Branch("iadcCh1", &iadcCh1, "iadcCh1/I");
    tree->Branch("iadcCh2", &iadcCh2, "iadcCh2/I");
    tree->Branch("adcMaxCh1", &adcMaxCh1, "adcMaxCh1/I");
    tree->Branch("adcMaxCh2", &adcMaxCh2, "adcMaxCh2/I");
    tree->Branch("adcMinCh1", &adcMinCh1, "adcMinCh1/I");
    tree->Branch("adcMinCh2", &adcMinCh2, "adcMinCh2/I");

    std::ifstream ifs(datafile);
    if (ifs.is_open() == false)
    {
        std::cout << "datafile input error" << std::endl;
        return;
    }

    int ch1 = 0, ch2 = 0;
    int clock = 0;

    int ch1Min = 1023, ch2Min = 1023;
    int ch1Max = -1024, ch2Max = -1024;
    int evtID = 0;
    int ch1Sum = 0, ch2Sum = 0;
    int ch1PedSum = 0, ch2PedSum = 0;
    int pedClockMin = 100, pedClockMax = 200;

    while (!ifs.eof())
    {
        std::string line = "";
        std::getline(ifs, line);
        if (line.length() <= 0 || strncmp(line.c_str(), "#", 1) == 0)
        {
            if (evtID != 0 && evtID % 3 == 2)
            {
                adcMinCh1 = ch1Min;
                adcMinCh2 = ch2Min;
                adcMaxCh1 = ch1Max;
                adcMaxCh2 = ch2Max;
                pedCh1 = ch1PedSum / (pedClockMax - pedClockMin);
                pedCh2 = ch2PedSum / (pedClockMax - pedClockMin);
                // if (std::abs(ch1Max) > std::abs(ch1Min))
                adcCh1 = ch1Max - pedCh1;
                // else
                //     adcCh1 = pedCh1 - ch1Min;
                // if (std::abs(ch2Max) > std::abs(ch2Min))
                adcCh2 = ch2Max - pedCh2;
                // else
                //     adcCh2 = pedCh2 - ch2Min;
                iadcCh1 = ch1Sum + pedCh1 * clock;
                iadcCh2 = ch2Sum + pedCh2 * clock;
                if (adcCh1 > thr_ch1 && adcCh2 > thr_ch2)
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
            ch1PedSum = 0;
            ch2PedSum = 0;
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

        if (clock >= pedClockMin && clock <= pedClockMax)
        {
            ch1PedSum += ch1;
            ch2PedSum += ch2;
        }

        ++clock;
    }

    tree->Write();
    file->Close();

    return;
}