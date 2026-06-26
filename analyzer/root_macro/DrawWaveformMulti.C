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

  //rootファイルを作る
  //outputDirの中にrootファイルをつくりたい→Pathを指定しないといけない
  //  /必須
  String outputPath = outputDir + "/beam.root";
  //c_strはstringクラスを文字列のconst char*型に変換する関数
  //TFile::Open()の関数はconst char*型を受け取るから直さないといけない
  TFile *file = TFile::Open(outputPath.c_str(), "RECREATE");
  TTree *tree = new TTree("tree", "tree");
  TTree *tree_clock = new TTree("tree_clock", "tree_clock");
  

  double unixtime = 0.0;
  int evtNum = 0;
  std::vector<int> evt_array;
  int datfile_Num = 0;
  double waveform = 0.0;
  double pulse_height = 0.0;
  double charge_Integral = 0.0;
  double ADC_Integral = 0.0;
  double ADC_Integral_sum = 0.0;
  double ADC_Integral_ave = 0.0;
  double charge_1ADC =0.0;
  double charge = 0.0;
  double G_amp = 11500;
  double split = 2500 / 2048;
  int ADC[1024];
  std::vector<int> clock_array;
  double pedestal = 0.0;
  double pedestal_sum = 0.0;
  double pedestal_ave = 0.0;
  int clock = 0;

  int clock_idx = 0;
  double ADC1 = 0.0;
  int clock_evtNum = 0;



  tree->Branch("clock", &clock, "clock/I");
  tree->Branch("pedestal_ave", &pedestal_ave, "pedestal_ave/D");
  tree->Branch("waveform", ADC, "ADC[clock]/I");
  tree->Branch("datfile_Num", &datfile_Num, "datfile_Num/I");
  tree->Branch("evtNum", &evtNum, "evtNum/I");
  tree->Branch("unixtime", &unixtime, "unixtime/D");
  tree->Branch("pulse_height", &pulse_height, "pulse_height/D");
  tree->Branch("ADC_Integral", &ADC_Integral, "ADC_Integral/D");
  tree->Branch("charge_Integral", &charge_Integral, "charge_Integral/D");
  tree_clock->Branch("clock_idx", &clock_idx, "clock_idx/I");
  tree_clock->Branch("ADC1", &ADC1, "ADC1/D");
  tree_clock->Branch("evtNum", &evtNum, "evtNum/I");
  tree_clock->Branch("unixtime", &unixtime, "unixtime/D");
    
    
    
  // setup histogram
  TH2F histWFCh1("histWFCh1","histWFCh1",1024,0,1024,4096,-2047,2048);
  TH2F histWFCh2("histWFCh2","histWFCh2",1024,0,1024,4096,-2047,2048);

  TH1F histSpCh1("histSPCh1","histSPCh1",2048,0,2048);
  TH1F histSpCh2("histSPCh2","histSPCh2",2048,0,2048);
    
  int ch1 = 0, ch2 = 0;

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
	//if( evtID != 0 ) {

	//pedestal_sum += pedestal;
	pedestal_ave = pedestal / 200.0;
	pedestal_sum = pedestal_ave * 1024.0;
	ADC_Integral = ADC_Integral - pedestal_sum;

	pulse_height = ch1Max - ch1Min - pedestal_ave;
	
	
	int posnum = line.find("#Ev. ");
	if( posnum != std::string::npos ){
	  std::string dummy = "";
	  std::stringstream ss (line);
	  ss >> dummy >> evtNum >> unixtime;

	  evt_array.push_back(evtNum);
	  for(int i = 0; i < 1000; i++){
	    if(evt_array.size() == i * 1000){
	      datfile_Num += 1;
	    }
	  }
	}
	  
	  
	histSpCh1.Fill( ch1Max - ch1Min );
	histSpCh2.Fill( ch2Max - ch2Min );
	//	}
            
	++evtID;
      	//std::cout << " ADC_Integral : " << ADC_Integral << std::endl;
	int posnum_1 = line.find("#Write ");
	if( posnum_1 != std::string::npos ){
	  //std::cout << " pedestal_ave : " << pedestal_ave << std::endl;
	  tree->Fill();
	}

	//unixtime = 0.0;
	pedestal = 0.0;
	pedestal_ave = 0.0;
	pedestal_sum = 0.0;
	waveform = 0.0;
	ADC_Integral = 0.0;
	charge_Integral = 0.0;
	
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

      ADC[clock] = ch1;
      clock_array.push_back(clock);
      //std::cout << " ADC : " << ADC[clock] << std::endl;
      if (clock < 200){
	pedestal += ch1;
      }
      // if (clock > 500){
	
      
	ADC_Integral += ch1;
      
	charge_1ADC = split / G_amp;
	charge = charge_1ADC * ch1;
	if (charge > 0){
	  charge_Integral += charge;
	}
	//}	
      
      histWFCh1.Fill(clock, ch1);
      histWFCh2.Fill(clock, ch2);
      ADC1 = ch1;
      clock_idx = clock;
      //clock_evtNum = evtNum;
      tree_clock->Fill();
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

  tree->Write();
  tree_clock->Write();
  file->Close();
  return;
}



