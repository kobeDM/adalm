#include "../include/inc.hpp"

void drawResult(const std::string &resultdir, const int n)
{
    // get ADSW path
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
    boost::optional<int> ran_bin = pt.get_optional<int>("ana.ran_bin");
    boost::optional<int> ran_min = pt.get_optional<int>("ana.ran_min");
    boost::optional<int> ran_max = pt.get_optional<int>("ana.ran_max");

    gROOT->SetBatch();
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = {0.00, 0.34, 0.61, 0.84, 1.00};
    Double_t red[NRGBs] = {0.00, 0.00, 0.87, 1.00, 1.00};
    Double_t green[NRGBs] = {0.00, 0.81, 1.00, 0.20, 0.00};
    Double_t blue[NRGBs] = {0.80, 1.00, 0.12, 0.00, 0.00};
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

    std::string filename = Form("%s/cutindex.dat", resultdir.c_str());
    std::ifstream ifs(filename);
    if (ifs.is_open() == false)
    {
        std::cout << "failed to open cutindex" << std::endl;
        return;
    }

    // open rootfile
    TFile *file = new TFile(Form("%s/raw.root", resultdir.c_str()));
    if (!file->IsOpen())
    {
        std::cout << "rootfile not found" << std::endl;
        return;
    }
    TTree *tree = (TTree *)file->Get("tree");

    // define ROOT objects
    TCanvas *cvs = new TCanvas("cvs", "cvs", 800, 800);
    TCanvas *cvs_c = new TCanvas("cvs_c", "cvs_c", 1200, 800);
    TH2F *wfdist[2];
    TH1F *adcdist1d[2];
    TH2F *wfdist_c[2];
    TH1F *adcdist1d_c[2];
    TH2F *adcdist2d_c[2];
    TF1 *fit[2];
    TLegend *tl[2];
    for (int i = 0; i < 2; i++)
    {
        // precut graphs
        wfdist[i] = new TH2F(Form("wfdist%i", i), Form("wfdist%i", i), 1024, 0,
                             1024, ran_bin.get() * 2, ran_max.get() * (-1), ran_max.get());
        adcdist1d[i] = new TH1F(Form("adcdist1d%i", i), Form("adcdist1d%i", i),
                                ran_bin.get(), ran_min.get(), ran_max.get());
        // cut graphs
        wfdist_c[i] = new TH2F(Form("wfdist_c%i", i), Form("wfdist_C%i", i),
                               1024, 0, 1024, ran_bin.get() * 2, ran_max.get() * (-1), ran_max.get());
        adcdist1d_c[i] = new TH1F(Form("adcdist1d_c%i", i),
                                  Form("adcdist1d_c%i", i), ran_bin.get(), ran_min.get(), ran_max.get());
        adcdist2d_c[i] =
            new TH2F(Form("adcdist_c%i", i), Form("adcdist_c%i", i), ran_bin.get(), ran_min.get(),
                     ran_max.get(), 150, -20000, 20000);
        fit[i] = new TF1(Form("fit%i", i), "pol1");
        tl[i] = new TLegend(0.7, 0.8, 0.9, 0.9);
    }

    const int clockN = 1024;
    int wfCh1[clockN] = {0};
    int wfCh2[clockN] = {0};
    int pedCh1 = 0, pedCh2 = 0;
    int adcCh1 = 0, adcCh2 = 0;
    int iadcCh1 = 0, iadcCh2 = 0;

    tree->SetBranchAddress("wfCh1", wfCh1);
    tree->SetBranchAddress("wfCh2", wfCh2);
    tree->SetBranchAddress("pedCh1", &pedCh1);
    tree->SetBranchAddress("pedCh2", &pedCh2);
    tree->SetBranchAddress("adcCh1", &adcCh1);
    tree->SetBranchAddress("adcCh2", &adcCh2);
    tree->SetBranchAddress("iadcCh1", &iadcCh1);
    tree->SetBranchAddress("iadcCh2", &iadcCh2);

    int bool1 = 0, bool2 = 0;
    int count = 0;
    while (ifs >> bool1 >> bool2)
    {
        tree->GetEntry(count);
        // ch1
        for (int clock = 0; clock < clockN; clock++)
        {
            wfdist[0]->Fill(clock, wfCh1[clock]);
            if (bool1 == true)
            {
                wfdist_c[0]->Fill(clock, wfCh1[clock]);
            }
        }
        adcdist1d[0]->Fill(adcCh1);
        if (bool1 == true)
        {
            adcdist1d_c[0]->Fill(adcCh1);
        }
        adcdist2d_c[0]->Fill(adcCh1, iadcCh1);
        // ch2
        for (int clock = 0; clock < clockN; clock++)
        {
            wfdist[1]->Fill(clock, wfCh2[clock]);
            if (bool2 == true)
            {
                wfdist_c[1]->Fill(clock, wfCh2[clock]);
            }
        }
        adcdist1d[1]->Fill(adcCh2);
        if (bool2 == true)
        {
            adcdist1d_c[1]->Fill(adcCh2);
        }
        adcdist2d_c[1]->Fill(adcCh2, iadcCh2);
        count++;
    }

    cvs->Divide(2, 2);
    cvs_c->Divide(3, 2);

    // for precut
    // cvs->cd();
    for (int i = 0; i < 2; i++)
    {
        cvs->cd(i * 2 + 1);
        adcdist1d[i]->SetStats(0);
        adcdist1d[i]->SetTitle(Form("ADCdist Ch%i (raw)", i + 1));
        adcdist1d[i]->GetXaxis()->SetTitle("ADC count");
        adcdist1d[i]->GetYaxis()->SetTitle("Entries");
        adcdist1d[i]->Draw("colz");
        cvs->cd(i * 2 + 2);
        wfdist[i]->SetStats(0);
        wfdist[i]->SetTitle(Form("Waveform Ch%i (raw)", i + 1));
        wfdist[i]->GetXaxis()->SetTitle("clock");
        wfdist[i]->GetYaxis()->SetTitle("ADC");
        wfdist[i]->Draw("colz");
    }

    // for cut
    // cvs_c->cd();
    const float xx2[4] = {(float)ran_min.get(), (float)ran_max.get(), (float)ran_max.get(), (float)ran_min.get()};
    const int cutwid = 1000;
    for (int i = 0; i < 2; i++)
    {
        cvs_c->cd(i * 3 + 1);
        adcdist1d_c[i]->SetStats(0);
        adcdist1d_c[i]->SetTitle(Form("ADCdist Ch%i (cut)", i + 1));
        adcdist1d_c[i]->GetXaxis()->SetTitle("ADC count");
        adcdist1d_c[i]->GetYaxis()->SetTitle("Entries");
        adcdist1d_c[i]->Draw("colz");
        cvs_c->cd(i * 3 + 2);
        wfdist_c[i]->SetStats(0);
        wfdist_c[i]->SetTitle(Form("Waveform Ch%i (cut)", i + 1));
        wfdist_c[i]->GetXaxis()->SetTitle("clock");
        wfdist_c[i]->GetYaxis()->SetTitle("ADC");
        wfdist_c[i]->Draw("colz");
        cvs_c->cd(i * 3 + 3);
        adcdist2d_c[i]->SetStats(0);
        adcdist2d_c[i]->SetTitle(Form("ADCdist2D Ch%i (cut)", i + 1));
        adcdist2d_c[i]->GetXaxis()->SetTitle("ADC count");
        adcdist2d_c[i]->GetYaxis()->SetTitle("total ADC");
        adcdist2d_c[i]->Draw("colz");
        adcdist2d_c[i]->Fit(Form("fit%i", i), "q");
        const float itc2 = fit[i]->GetParameter(0);
        const float slp2 = fit[i]->GetParameter(1);

        const float yy2[4] = {
            slp2 * xx2[0] + itc2 - cutwid, slp2 * xx2[1] + itc2 - cutwid,
            slp2 * xx2[2] + itc2 + cutwid, slp2 * xx2[3] + itc2 + cutwid};
        TGraph *eband2 = new TGraph(4, xx2, yy2);
        eband2->SetFillStyle(3001);
        eband2->SetFillColor(kRed - 9);
        eband2->Draw("SAME F");

        tl[i]->SetFillStyle(0);
        tl[i]->AddEntry(fit[i], "fit func.", "l");
        tl[i]->AddEntry(eband2, "used area", "f");
        tl[i]->Draw();
    }

    cvs->SaveAs(Form("%s/result_%i.png", resultdir.c_str(), n));
    cvs_c->SaveAs(Form("%s/result_c_%i.png", resultdir.c_str(), n));

    return;
}