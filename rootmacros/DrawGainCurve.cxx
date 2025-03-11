// #include "inc/shinclude.h"

void DrawGainCurve()
{
    // SetShStyle();

    std::string dataFile = "./ampCalib.dat";
    std::ifstream ifs(dataFile);
    if (!ifs)
    {
        std::cerr << "Error: data file doesn't found !!" << std::endl;
        return;
    }

    std::string line = "";

    TCanvas *cvs = new TCanvas("cvs", "cvs", 600, 600);
    TGraphErrors *tgeGainCurve = new TGraphErrors();

    while (std::getline(ifs, line))
    {
        if (line[0] == '#')
            continue;
        std::istringstream iss(line);
        double charge, pulseHeight, pulseHeightError;
        iss >> charge >> pulseHeight >> pulseHeightError;

        tgeGainCurve->SetPoint(tgeGainCurve->GetN(), charge, pulseHeight);
        tgeGainCurve->SetPointError(tgeGainCurve->GetN() - 1, 0, pulseHeightError);
    }

    // tgeGainCurve->SetMarkerStyle(1);
    tgeGainCurve->GetXaxis()->SetTitle("Charge [pC]");
    tgeGainCurve->GetYaxis()->SetTitle("Pulse height [ADC]");
    tgeGainCurve->GetXaxis()->SetLimits(0, 100);
    tgeGainCurve->GetYaxis()->SetRangeUser(0, 200);
    tgeGainCurve->Draw("AP");

    // Fitting
    TF1 *fit = new TF1("fit", "pol1", 0, 200);
    fit->SetLineColor(kRed);
    tgeGainCurve->Fit("fit");

    // Draw parameters
    double offset = fit->GetParameter(0);
    double offsetErr = fit->GetParError(0);
    double slope = fit->GetParameter(1);
    double slopeErr = fit->GetParError(1);
    TLatex *latex = new TLatex();
    latex->DrawLatex(0.02, 140, Form("Slope  = %.2lf #pm %.2lf", slope, slopeErr));
    latex->DrawLatex(0.02, 130, Form("Offset = %.2lf #pm %.2lf", offset, offsetErr));

    cvs->SaveAs("GainCurve.png");
}