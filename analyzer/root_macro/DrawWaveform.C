#include "inc/shinclude.h"

void DrawWaveform(const String& filename)
{
    SetShStyle();

    std::ifstream ifs( filename );
    if( ifs.is_open() == false ) return;

    TH2F hist("hist","hist",1024,0,1024,100,-100,100);

    String name = "";
    int ch1 = 0, ch2 = 0;
    int clock = 0;
    while( !ifs.eof( ) ) {
        String line = "";
        std::getline( ifs, line );
        if( line.length( ) <= 0 || strncmp( line.c_str( ), "#", 1 ) == 0 ) {
            clock = 0;
            continue;
        }
        std::stringstream ss( line );
        ss >> name >> ch1 >> ch2;

        hist.Fill(clock, ch1);
        ++clock;
    }

    TCanvas cvs("cvs","cvs",800,600);

    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 1.00 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.80, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable( NRGBs, stops, red, green, blue, NCont );
    gStyle->SetNumberContours( NCont );
    gPad->SetRightMargin( 0.2 );

    hist.GetXaxis()->SetTitle("clock [10 MHz sampling]");
    hist.GetYaxis()->SetTitle("ADC count");
    hist.GetZaxis()->SetTitle("Entries");
    hist.Draw("colz");

    String outname = ShUtil::ExtractPathWithoutExt( filename );
    cvs.SaveAs( Form( "%s.png", outname.c_str() ) );
    cvs.SaveAs( Form( "%s.pdf", outname.c_str() ) );
    cvs.SaveAs( Form( "%s.eps", outname.c_str() ) );

    return;
}



