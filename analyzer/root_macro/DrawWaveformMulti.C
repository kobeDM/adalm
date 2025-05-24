#include "inc/shinclude.h"

void DrawWaveformMulti( const String& filenameList, const String& outputDir )
{
    SetShStyle();
    ShUtil::ExistCreateDir( outputDir );
    
    // retrieve filenames
    std::list< String > fileList;
    std::ifstream ifsList( filenameList );
    if( ifsList.is_open( ) == false ) return;
    while( !ifsList.eof( ) ) {
        String filePath = "";
        std::getline( ifsList, filePath );
        if( ShUtil::ExistFile( filePath ) == false ) continue;
        fileList.push_back( filePath );
    }
    
    // setup histogram
    TH2F histWFCh1("histWFCh1","histWFCh1",1024,0,1024,1000,-500,500);
    TH2F histWFCh2("histWFCh2","histWFCh2",1024,0,1024,1000,-500,500);

    TH1F histSpCh1("histSPCh1","histSPCh1",1000,0,1000);
    TH1F histSpCh2("histSPCh2","histSPCh2",1000,0,1000);
    
    int ch1 = 0, ch2 = 0;
    int clock = 0;

    int ch1Min = 1023, ch2Min = 1023;
    int ch1Max = -1024, ch2Max = -1024;
    int evtID = 0;

    for( auto filePath : fileList ) {
        ShUtil::Cinfo( Form( "loading %s ...", filePath.c_str( ) ) );
        
        std::ifstream ifs( filePath );
        if( ifs.is_open() == false ) return;

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
            ss >> ch1 >> ch2;

            if     ( ch1 < ch1Min ) ch1Min = ch1;
            else if( ch1 > ch1Max ) ch1Max = ch1;

            if     ( ch2 < ch2Min ) ch2Min = ch2;
            else if( ch2 > ch2Max ) ch2Max = ch2;
        
            histWFCh1.Fill(clock, ch1);
            histWFCh2.Fill(clock, ch2);
            ++clock;
        }
    }

    TCanvas cvs("cvs","cvs",1600,1200);
    cvs.Divide( 2, 2 );

    cvs.cd( 1 );
    histSpCh1.GetXaxis()->SetTitle("ADC_{max} - ADC_{min}");
    histSpCh1.GetYaxis()->SetTitle("Events");
    histSpCh1.Draw("colz");

    ShTUtil::CreateDrawText( 0.7, 0.85, "Channel 1" );
    
    cvs.cd( 2 );
    histSpCh2.GetXaxis()->SetTitle("ADC_{max} - ADC_{min}");
    histSpCh2.GetYaxis()->SetTitle("Events");
    histSpCh2.Draw("colz");

    ShTUtil::CreateDrawText( 0.7, 0.85, "Channel 2" );
    
    cvs.cd( 3 );
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 1.00 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.80, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable( NRGBs, stops, red, green, blue, NCont );
    gStyle->SetNumberContours( NCont );
    gPad->SetRightMargin( 0.2 );

    histWFCh1.GetXaxis()->SetTitle("clock [10MHz sampling]");
    histWFCh1.GetYaxis()->SetTitle("ADC count");
    histWFCh1.GetZaxis()->SetTitle("Entries");
    histWFCh1.Draw("colz");

    ShTUtil::CreateDrawText( 0.6, 0.85, "Channel 1" );
    
    cvs.cd( 4 );
    TColor::CreateGradientColorTable( NRGBs, stops, red, green, blue, NCont );
    gStyle->SetNumberContours( NCont );
    gPad->SetRightMargin( 0.2 );

    histWFCh2.GetXaxis()->SetTitle("clock [10MHz sampling]");
    histWFCh2.GetYaxis()->SetTitle("ADC count");
    histWFCh2.GetZaxis()->SetTitle("Entries");
    histWFCh2.Draw("colz");
    
    ShTUtil::CreateDrawText( 0.6, 0.85, "Channel 2" );

    cvs.SaveAs( Form( "%s/wfsp.png", outputDir.c_str( ) ) );
    cvs.SaveAs( Form( "%s/wfsp.pdf", outputDir.c_str( ) ) );
    cvs.SaveAs( Form( "%s/wfsp.eps", outputDir.c_str( ) ) );
    
    return;
}



