#include "inc/shinclude.h"

void DrawWaveform(const String& filename)
{
    SetShStyle();

    std::ifstream ifs( filename );
    if( ifs.is_open() == false ) return;

    TH2F histWFCh1("histWFCh1","histWFCh1",1024,0,1024,100,-100,100);
    TH2F histWFCh2("histWFCh2","histWFCh2",1024,0,1024,100,-100,100);

    TH1F histSpCh1("histSPCh1","histSPCh1",100,0,100);
    TH1F histSpCh2("histSPCh2","histSPCh2",100,0,100);
    
    String name = "";
    int ch1 = 0, ch2 = 0;
    int clock = 0;

    int ch1Min = 1023, ch2Min = 1023;
    int ch1Max = -1024, ch2Max = -1024;
    int evtID = 0;
    while( !ifs.eof( ) ) {
        String line = "";
        std::getline( ifs, line );
        if( line.length( ) <= 0 || strncmp( line.c_str( ), "#", 1 ) == 0 ) {
            if( evtID != 0 ) {
                histSpCh1.Fill( ch1Max - ch1Min );
                histSpCh2.Fill( ch2Max - ch2Min );
            }
            
            ++evtID;
            clock = 0;
            ch1Min = 1023;
            ch2Min = 1023;
            ch1Max = -1024;
            ch2Max = -1024;
            continue;
        }
        std::stringstream ss( line );
        ss >> name >> ch1 >> ch2;

        if     ( ch1 < ch1Min ) ch1Min = ch1;
        else if( ch1 > ch1Max ) ch1Max = ch1;

        if     ( ch2 < ch2Min ) ch2Min = ch2;
        else if( ch2 > ch2Max ) ch2Max = ch2;
        
        histWFCh1.Fill(clock, ch1);
        histWFCh1.Fill(clock, ch2);
        ++clock;
    }

    String outname = ShUtil::GetFileName(ShUtil::ExtractPathWithoutExt( filename ));
    TCanvas cvs("cvs","cvs",800,600);
    
    histSpCh1.GetXaxis()->SetTitle("ADC count");
    histSpCh1.GetYaxis()->SetTitle("Entries");
    histSpCh1.Draw("colz");
    
    cvs.SaveAs( Form( "%s_spch1.png", outname.c_str() ) );
    cvs.SaveAs( Form( "%s_spch1.pdf", outname.c_str() ) );
    cvs.SaveAs( Form( "%s_spch1.eps", outname.c_str() ) );

    histSpCh2.GetXaxis()->SetTitle("ADC count");
    histSpCh2.GetYaxis()->SetTitle("Entries");
    histSpCh2.Draw("colz");
    
    cvs.SaveAs( Form( "%s_spch2.png", outname.c_str() ) );
    cvs.SaveAs( Form( "%s_spch2.pdf", outname.c_str() ) );
    cvs.SaveAs( Form( "%s_spch2.eps", outname.c_str() ) );
    
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 1.00 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.80, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable( NRGBs, stops, red, green, blue, NCont );
    gStyle->SetNumberContours( NCont );
    gPad->SetRightMargin( 0.2 );

    histWFCh1.GetXaxis()->SetTitle("clock [10 MHz sampling]");
    histWFCh1.GetYaxis()->SetTitle("ADC count");
    histWFCh1.GetZaxis()->SetTitle("Entries");
    histWFCh1.Draw("colz");
    
    cvs.SaveAs( Form( "%s_wfch1.png", outname.c_str() ) );
    cvs.SaveAs( Form( "%s_wfch1.pdf", outname.c_str() ) );
    cvs.SaveAs( Form( "%s_wfch1.eps", outname.c_str() ) );

    histWFCh2.GetXaxis()->SetTitle("clock [10 MHz sampling]");
    histWFCh2.GetYaxis()->SetTitle("ADC count");
    histWFCh2.GetZaxis()->SetTitle("Entries");
    histWFCh2.Draw("colz");
    
    cvs.SaveAs( Form( "%s_wfch2.png", outname.c_str() ) );
    cvs.SaveAs( Form( "%s_wfch2.pdf", outname.c_str() ) );
    cvs.SaveAs( Form( "%s_wfch2.eps", outname.c_str() ) );
    
    return;
}



