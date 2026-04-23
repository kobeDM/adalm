// #include "inc/shinclude.h"
#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TTree.h"

#include "../include/NAPStyle.h"

void CalcDriftV( )
{
    SetShStyle( );

    std::cout << "### start CalcDriftV ###" << std::endl;

    const double driftLength  = 53.5;  // cm
    const double samplingRate = 10.0;  // MHz

    const double adcThr = -500.0;  // clock when exceeding threhold is stored as fallClk.

    const double fitTopMin    = -20.0;
    const double fitTopMax    = 20.0;
    const double fitBottomMin = 30.0;
    const double fitBottomMax = 70.0;

    const std::string inputFilePath = "/home/msgc/cn1/ana/20260106/per0000/raw.root";
    const std::string header        = "200V/cm";

    TFile *inputFile = new TFile( inputFilePath.c_str( ), "READ" );
    if ( !inputFile ) {
        std::cerr << "Error: root file was not found." << std::endl;
        return;
    }

    TTree    *inputTree = (TTree *)inputFile->Get( "tree" );
    const int totEvt    = inputTree->GetEntries( );
    std::cout << "Total events: " << totEvt << " events" << std::endl;

    int wfCh1[1024] = { }, wfCh2[1024] = { };
    int pedCh1 = 0, pedCh2 = 0;
    int adcMinCh1 = 0, adcMinCh2 = 0;
    inputTree->SetBranchAddress( "wfCh1", wfCh1 );
    inputTree->SetBranchAddress( "wfCh2", wfCh2 );
    inputTree->SetBranchAddress( "pedCh1", &pedCh1 );
    inputTree->SetBranchAddress( "pedCh2", &pedCh2 );
    inputTree->SetBranchAddress( "adcMinCh1", &adcMinCh1 );
    inputTree->SetBranchAddress( "adcMinCh2", &adcMinCh2 );

    TCanvas *c1    = new TCanvas( "c1", "c1", 800, 800 );
    TH1D    *h_clk = new TH1D( "h_clk", Form( ";Clock difference (%.1lf MHz);Entries", samplingRate ), 220, -20, 200 );

    for ( int evt = 0; evt < totEvt; ++evt ) {
        inputTree->GetEntry( evt );

        // std::cout << "Min ch1: " << adcMinCh1 << "\tMin ch2: " << adcMinCh2 << std::endl;

        if ( adcMinCh1 > adcThr )
            continue;
        if ( adcMinCh2 > adcThr )
            continue;

        // calculate fall timing
        int  fallClkCh1 = 0, fallClkCh2 = 0;
        bool fallFlagCh1 = false, fallFlagCh2 = false;

        for ( int clk = 0; clk < 1024; ++clk ) {
            if ( wfCh1[clk] < adcThr && !fallFlagCh1 ) {
                fallClkCh1  = clk;
                fallFlagCh1 = true;
            }
            if ( wfCh2[clk] < adcThr && !fallFlagCh2 ) {
                fallClkCh2  = clk;
                fallFlagCh2 = true;
            }
        }

        int clkDiff = fallClkCh1 - fallClkCh2;
        h_clk->Fill( clkDiff );
    }

    h_clk->Draw( );

    // fitting
    TF1   *f_erfTop        = new TF1( "fitTop", "[0]*TMath::Erf((x-[1])/[2])+[3]", fitTopMin, fitTopMax );
    TF1   *f_erfBottom     = new TF1( "fitBottom", "[0]*TMath::Erf((x-[1])/[2])+[3]", fitTopMin, fitTopMax );
    double paramsTop[4]    = { 10, 0, 1, 10 };
    double paramsBottom[4] = { -10, 50, 1, 10 };
    f_erfTop->SetLineColor( kRed );
    f_erfBottom->SetLineColor( kRed );
    f_erfTop->SetParameters( paramsTop );
    f_erfBottom->SetParameters( paramsBottom );
    h_clk->Fit( "fitTop", "ML", "", fitTopMin, fitTopMax );
    h_clk->Fit( "fitBottom", "ML", "", fitBottomMin, fitBottomMax );
    f_erfTop->Draw( "SAME" );

    double riseClk   = f_erfTop->GetParameter( 1 );
    double fallClk   = f_erfBottom->GetParameter( 1 );
    double driftTime = ( fallClk - riseClk ) / samplingRate;
    double driftV    = driftLength / driftTime;

    TLatex *latex = new TLatex( );
    latex->SetNDC( );
    latex->SetTextFont( 42 );
    latex->SetTextSize( 0.03 );
    latex->DrawLatex( 0.3, 0.90, inputFilePath.c_str( ) );
    latex->DrawLatex( 0.3, 0.86, header.c_str( ) );
    latex->DrawLatex( 0.3, 0.82, Form( "Drift velocity: %.2lf cm/us", driftV ) );

    c1->SaveAs( "DriftV.png" );
    c1->SaveAs( "DriftV.pdf" );
    // c1->SaveAs("DriftV.eps");

    return;
}
