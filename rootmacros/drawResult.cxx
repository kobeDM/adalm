#include <fstream>
#include <iostream>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "TCanvas.h"
#include "TColor.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"

#include "../include/NAPStyle.h"

void drawResult( const std::string &resultdir )
{
    SetShStyle( 1 );

    // get json data
    const std::string           jsonfile = Form( "%s/config.json", resultdir.c_str( ) );
    boost::property_tree::ptree pt;
    read_json( jsonfile, pt );
    int    adc_ch1_bin = pt.get_optional<int>( "ana.ch1.adc_bin" ).get( );
    double adc_ch1_min = pt.get_optional<double>( "ana.ch1.adc_min" ).get( );
    double adc_ch1_max = pt.get_optional<double>( "ana.ch1.adc_max" ).get( );
    int    clk_ch1_bin = pt.get_optional<int>( "ana.ch1.clk_bin" ).get( );
    int    clk_ch1_min = pt.get_optional<int>( "ana.ch1.clk_min" ).get( );
    int    clk_ch1_max = pt.get_optional<int>( "ana.ch1.clk_max" ).get( );
    int    adc_ch2_bin = pt.get_optional<int>( "ana.ch2.adc_bin" ).get( );
    double adc_ch2_min = pt.get_optional<double>( "ana.ch2.adc_min" ).get( );
    double adc_ch2_max = pt.get_optional<double>( "ana.ch2.adc_max" ).get( );
    int    clk_ch2_bin = pt.get_optional<int>( "ana.ch2.clk_bin" ).get( );
    int    clk_ch2_min = pt.get_optional<int>( "ana.ch2.clk_min" ).get( );
    int    clk_ch2_max = pt.get_optional<int>( "ana.ch2.clk_max" ).get( );

    // open rootfile
    TFile *file = new TFile( Form( "%s/raw.root", resultdir.c_str( ) ) );
    if ( !file->IsOpen( ) ) {
        std::cout << "rootfile not found" << std::endl;
        return;
    }
    TTree *tree = (TTree *)file->Get( "tree" );

    // define ROOT objects
    TCanvas *cvs     = new TCanvas( "cvs", "cvs", 800, 800 );
    TH2F    *hWfCh1  = new TH2F( "hWfCh1", "hWfCh1", clk_ch1_bin, clk_ch1_min, clk_ch1_max, adc_ch1_bin, adc_ch1_min, adc_ch1_max );
    TH2F    *hWfCh2  = new TH2F( "hWfCh2", "hWfCh2", clk_ch2_bin, clk_ch2_min, clk_ch2_max, adc_ch2_bin, adc_ch2_min, adc_ch2_max );
    TH1F    *hAdcCh1 = new TH1F( "hAdcCh1", "hAdcCh1", adc_ch1_bin, adc_ch1_min, adc_ch1_max );
    TH1F    *hAdcCh2 = new TH1F( "hAdcCh2", "hAdcCh2", adc_ch2_bin, adc_ch2_min, adc_ch2_max );
    TF1     *fitCh1  = new TF1( "fitCh1", "gaus", adc_ch1_min, adc_ch1_max );
    TF1     *fitCh2  = new TF1( "fitCh2", "gaus", adc_ch2_min, adc_ch2_max );
    fitCh1->SetLineColor( kRed );
    fitCh2->SetLineColor( kRed );

    const int clockN        = 1024;
    int       wfCh1[clockN] = { 0 };
    int       wfCh2[clockN] = { 0 };
    int       pedCh1 = 0, pedCh2 = 0;
    int       adcCh1 = 0, adcCh2 = 0;
    int       iadcCh1 = 0, iadcCh2 = 0;

    tree->SetBranchAddress( "wfCh1", wfCh1 );
    tree->SetBranchAddress( "wfCh2", wfCh2 );
    tree->SetBranchAddress( "pedCh1", &pedCh1 );
    tree->SetBranchAddress( "pedCh2", &pedCh2 );
    tree->SetBranchAddress( "adcCh1", &adcCh1 );
    tree->SetBranchAddress( "adcCh2", &adcCh2 );
    tree->SetBranchAddress( "iadcCh1", &iadcCh1 );
    tree->SetBranchAddress( "iadcCh2", &iadcCh2 );

    const int nEntries = tree->GetEntries( );
    std::cout << "nEntries: " << nEntries << std::endl;

    // Fill waveform
    for ( int i = 0; i < nEntries; i++ ) {
        tree->GetEntry( i );
        // ch1
        for ( int clock = 0; clock < clockN; clock++ ) {
            hWfCh1->Fill( clock, wfCh1[clock] );
        }
        hAdcCh1->Fill( adcCh1 );
        // ch2
        for ( int clock = 0; clock < clockN; clock++ ) {
            hWfCh2->Fill( clock, wfCh2[clock] );
        }
        hAdcCh2->Fill( adcCh2 );
    }

    cvs->Divide( 2, 2 );

    cvs->cd( 1 );
    hAdcCh1->GetXaxis( )->SetTitle( "ADC count" );
    hAdcCh1->GetYaxis( )->SetTitle( "Entries" );
    hAdcCh1->Draw( "colz" );
    cvs->cd( 2 );
    hWfCh1->GetXaxis( )->SetTitle( "clock" );
    hWfCh1->GetYaxis( )->SetTitle( "ADC" );
    hWfCh1->Draw( "colz" );
    cvs->cd( 3 );
    hAdcCh2->GetXaxis( )->SetTitle( "ADC count" );
    hAdcCh2->GetYaxis( )->SetTitle( "Entries" );
    hAdcCh2->Draw( "colz" );
    cvs->cd( 4 );
    hWfCh2->GetXaxis( )->SetTitle( "clock" );
    hWfCh2->GetYaxis( )->SetTitle( "ADC" );
    hWfCh2->Draw( "colz" );

    cvs->SaveAs( Form( "%s/result.png", resultdir.c_str( ) ) );

    return;
}