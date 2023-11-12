#include "inc/inc.hpp"

void fitSpectrum(const std::string resultdir, const int n) {
    // get ADSW path
    const char *SOFT_PATH = std::getenv("ADSW");
    std::string soft_path(std::getenv("ADSW"));
    if (soft_path.empty()) {
        std::cout << "ADSW environment variable is not set" << std::endl;
        return;
    }

    // read json
    const std::string jsonfile = Form("%s/cfg/config.json", soft_path.c_str());
    boost::property_tree::ptree pt;
    read_json(jsonfile, pt);
    boost::optional<int> ran_min = pt.get_optional<int>("ana.ran_min");
    boost::optional<int> ran_max = pt.get_optional<int>("ana.ran_max");
    boost::optional<int> fitCh1_min = pt.get_optional<int>("ana.fit1_min");
    boost::optional<int> fitCh1_max = pt.get_optional<int>("ana.fit1_max");
    boost::optional<int> fitCh2_min = pt.get_optional<int>("ana.fit2_min");
    boost::optional<int> fitCh2_max = pt.get_optional<int>("ana.fit2_max");
    boost::optional<int> ch = pt.get_optional<int>("ana.fit_ch");

    gROOT->SetBatch();
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = {0.00, 0.34, 0.61, 0.84, 1.00};
    Double_t red[NRGBs] = {0.00, 0.00, 0.87, 1.00, 1.00};
    Double_t green[NRGBs] = {0.00, 0.81, 1.00, 0.20, 0.00};
    Double_t blue[NRGBs] = {0.80, 1.00, 0.12, 0.00, 0.00};
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

    // open cutindex
    std::string filename = Form("%s/cutindex.dat", resultdir.c_str());
    std::ifstream ifs(filename);
    if (ifs.is_open() == false) {
        std::cout << "failed to open cutindex" << std::endl;
        return;
    }

    // open rootfile
    TFile *file = new TFile(Form("%s/raw.root", resultdir.c_str()));
    if (!file->IsOpen()) {
        std::cout << "rootfile not found" << std::endl;
        return;
    }
    // get tree
    TTree *tree = (TTree *)file->Get("tree");

    // define ROOT object
    TCanvas *cvs = new TCanvas("cvs", "cvs", 800, 400);
    cvs->Divide(2, 1);
    TH1F *histSp[2];
    TF1 *fit[2];
    for (int i = 0; i < 2; i++) {
        histSp[i] = new TH1F(Form("histSp%i", i + 1), Form("histSp%i", i + 1),
                             150, ran_min.get(), ran_max.get());
        if (ch.get() == 1)
            fit[i] =
                new TF1(Form("fit%i", i + 1), "gaus", fitCh1_min.get(), fitCh1_max.get());
        else
            fit[i] =
                new TF1(Form("fit%i", i + 1), "gaus", fitCh2_min.get(), fitCh2_max.get());
    }

    // set data
    const int clockN = 1024;
    float adcCh1 = 0, adcCh2 = 0;
    tree->SetBranchAddress("adcCh1", &adcCh1);
    tree->SetBranchAddress("adcCh2", &adcCh2);

    // fill
    int bool1 = 0, bool2 = 0;
    int count = 0;
    while (ifs >> bool1 >> bool2) {
        tree->GetEntry(count);
        if (ch.get() == 1) {
            // ch1
            histSp[0]->Fill(adcCh1);
            if (bool1 == true) {
                histSp[1]->Fill(adcCh1);
            }
        } else {
            // ch2
            histSp[0]->Fill(adcCh2);
            if (bool1 == true) {
                histSp[1]->Fill(adcCh2);
            }
        }
        count++;
    }

    // labeling and fit
    cvs->cd(1);
    histSp[0]->SetStats(0);
    histSp[0]->SetTitle("Spectrum (raw)");
    histSp[0]->GetXaxis()->SetTitle("ADC count");
    histSp[0]->GetYaxis()->SetTitle("Entries");
    histSp[0]->Draw("colz");
    if (ch.get() == 1)
        histSp[0]->Fit(fit[0], "q", "", fitCh1_min.get(), fitCh1_max.get());
    else
        histSp[0]->Fit(fit[0], "q", "", fitCh2_min.get(), fitCh2_max.get());
    const float mean_1 = fit[0]->GetParameter(1);
    const float meane_1 = fit[0]->GetParError(1);
    const float sigma_1 = fit[0]->GetParameter(2);
    const float simgae_1 = fit[0]->GetParError(2);
    TLatex tex;
    tex.SetTextSize(0.04);
    tex.DrawLatex(150 * 0.55, 0.95 * histSp[0]->GetMaximum(),
                  Form("mean : %4.2f +- %3.2f", mean_1, meane_1));
    tex.DrawLatex(150 * 0.55, 0.9 * histSp[0]->GetMaximum(),
                  Form("sigma : %3.2f +- %3.2f", sigma_1, simgae_1));

    cvs->cd(2);
    histSp[1]->SetStats(0);
    histSp[1]->SetTitle("Spectrum (after cut)");
    histSp[1]->GetXaxis()->SetTitle("ADC count");
    histSp[1]->GetYaxis()->SetTitle("Entries");
    histSp[1]->Draw("colz");
    if (ch.get() == 1)
        histSp[1]->Fit(fit[1], "q", "", fitCh1_min.get(), fitCh1_max.get());
    else
        histSp[1]->Fit(fit[1], "q", "", fitCh2_min.get(), fitCh2_max.get());
    const float mean_2 = fit[1]->GetParameter(1);
    const float meane_2 = fit[1]->GetParError(1);
    const float sigma_2 = fit[1]->GetParameter(2);
    const float simgae_2 = fit[1]->GetParError(2);
    tex.SetTextSize(0.04);
    tex.DrawLatex(150 * 0.55, 0.95 * histSp[1]->GetMaximum(),
                  Form("mean : %4.2f +- %3.2f", mean_2, meane_2));
    tex.DrawLatex(150 * 0.55, 0.9 * histSp[1]->GetMaximum(),
                  Form("sigma : %3.2f +- %3.2f", sigma_2, simgae_2));

    // save
    if (ch.get() == 1)
        cvs->SaveAs(Form("%s/fitSpectrumCh1_%i.png", resultdir.c_str(), n));
    else
        cvs->SaveAs(Form("%s/fitSpectrumCh2_%i.png", resultdir.c_str(), n));

    return;
}