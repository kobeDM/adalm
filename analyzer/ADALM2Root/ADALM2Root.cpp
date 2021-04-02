//------------------------------------------------------------------
// Read tree adalm_daq output data
// Update: 08. October 2020
// Author: T.Shimada
//------------------------------------------------------------------

//// STL
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
// ROOT
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TSystem.h"
#include "TClassTable.h"
#include "TApplication.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TString.h"
//USER
#include "ADALMConfig.hh"
#define ADALM_ADC_BIT 4096


int main(int argc, char** argv){

	std::cout <<std::endl;
	std::cout << "---------------------------------------" <<std::endl;
	std::cout << "ADALM analyzer ADALM2Root" <<std::endl;
	std::cout << "version 0.0" <<std::endl;
	std::cout << "Author : T.shimada" <<std::endl;
	std::cout << "---------------------------------------" <<std::endl;
	std::cout <<std::endl;

	if(argc != 3){
		std::cout << "Usage :" << std::endl;
		std::cout << "./ad2_dat2root file.dat config.json" << std::endl;
		return -1;
	}

	std::string file = argv[1];
	std::ifstream ifs(file);
	if(!ifs.is_open()) return -1;

	//++++++++++++++++++++++++++++++++++++++++++
	//  read config file (json)
	//++++++++++++++++++++++++++++++++++++++++++
	std::string conffilename=argv[2];
	ADALMConfig* adalm_conf = new ADALMConfig();
	adalm_conf->ReadConfigJSON(conffilename);
	adalm_conf->PrintConfigJSON();
	double sampling_rate      = adalm_conf->sampling_rate;
	int    sampling_num       = adalm_conf->sampling_num;
	double dynamic_range      = adalm_conf->dynamic_range;
	int    offset_sampling    = adalm_conf->offset_sampling;

  TString filename;
  std::string::size_type pos = file.find("/");
  while(pos != std::string::npos){
    file = file.substr(pos+1);
    pos = file.find("/");
  }
  pos = file.find(".dat");
  filename = file.substr(0,pos);


	//++++++++++++++++++++++++++++++++++++++++++
	//  set buffer 
	//++++++++++++++++++++++++++++++++++++++++++
	std::string buf_info[3];
	int buf_adc[2];
	int EvID;
	double out_sampling_rate;
	double timestamp_start;
	double timestamp_end;
	double sample_time[sampling_num];//us
	double wf_ch1[sampling_num];
	double wf_ch2[sampling_num];
	double ph_max_ch1;
	double ph_max_ch2;
	double ph_min_ch1;
	double ph_min_ch2;
	double area_ch1;
	double area_ch2;
	double offset_ch1;
	double offset_ch2;
	
	//++++++++++++++++++++++++++++++++++++++++++
	//  read config file 
	//++++++++++++++++++++++++++++++++++++++++++
	TFile* out_file = TFile::Open(filename+".root", "RECREATE");
  TTree* tree = new TTree("tree", "tree");
  tree->Branch("EvID",            &EvID,              "EvID/I");
  tree->Branch("sampling_rate",   &out_sampling_rate, "sampling_rate/D");
  tree->Branch("timestamp_start", &timestamp_start,   "timestamp_start/D");
  tree->Branch("timestamp_end",   &timestamp_end,     "timestamp_end/D");
  tree->Branch("sample_time",     &sample_time,       Form("sample_time[%d]/D",sampling_num));
  tree->Branch("wf_ch1",          wf_ch1,             Form("wf_ch1[%d]/D",sampling_num));
  tree->Branch("wf_ch2",          wf_ch2,             Form("wf_ch2[%d]/D",sampling_num));
  tree->Branch("offset_ch1",      &offset_ch1,        "offset_ch1/D");
  tree->Branch("offset_ch2",      &offset_ch2,        "offset_ch2/D");
  tree->Branch("ph_max_ch1",      &ph_max_ch1,        "ph_max_ch1/D");
  tree->Branch("ph_max_ch2",      &ph_max_ch2,        "ph_max_ch2/D");
  tree->Branch("ph_min_ch1",      &ph_min_ch1,        "ph_min_ch1/D");
  tree->Branch("ph_min_ch2",      &ph_min_ch2,        "ph_min_ch2/D");
  tree->Branch("area_ch1",        &area_ch1,          "area_ch1/D");
  tree->Branch("area_ch2",        &area_ch2,          "area_ch2/D");
	out_sampling_rate = sampling_rate;

	int ev =0;
	std::cout << "Main Loop Start..." <<std::endl;
  while(ifs >> buf_info[0] >> EvID >> timestamp_start){
		std::cout << "\rFill Data to ROOT File... Ev." << ev << "/max" << 1000 <<std::flush;
		ifs >> buf_info[0] >> buf_info[1];
		area_ch1 = 0;
		area_ch2 = 0;
    offset_ch1 = 0;
    offset_ch2 = 0;
		ph_max_ch1 = -999999;
		ph_min_ch2 = 999999;
		ph_max_ch1 = -999999;
		ph_min_ch2 = 999999;
		for(int ibin=0;ibin<offset_sampling;++ibin){
      offset_ch1 += double(buf_adc[0])*dynamic_range/double(ADALM_ADC_BIT)/double(offset_sampling);
      offset_ch2 += double(buf_adc[1])*dynamic_range/double(ADALM_ADC_BIT)/double(offset_sampling);
		}
		for(int ibin=0;ibin<sampling_num;++ibin){
      ifs >> buf_adc[0] >> buf_adc[1];
			sample_time[ibin] = ibin/sampling_rate*1e6; //us
      wf_ch1[ibin] = double(buf_adc[0])*dynamic_range/double(ADALM_ADC_BIT);
      wf_ch2[ibin] = double(buf_adc[1])*dynamic_range/double(ADALM_ADC_BIT);
      // max algorithm
			if(ph_max_ch1<double(buf_adc[0])*dynamic_range/double(ADALM_ADC_BIT))ph_max_ch1 = double(buf_adc[0])*dynamic_range/double(ADALM_ADC_BIT);
      if(ph_max_ch2<double(buf_adc[1])*dynamic_range/double(ADALM_ADC_BIT))ph_max_ch2 = double(buf_adc[1])*dynamic_range/double(ADALM_ADC_BIT);
      if(ph_min_ch1>double(buf_adc[0])*dynamic_range/double(ADALM_ADC_BIT))ph_min_ch1 = double(buf_adc[0])*dynamic_range/double(ADALM_ADC_BIT);
      if(ph_min_ch2>double(buf_adc[1])*dynamic_range/double(ADALM_ADC_BIT))ph_min_ch2 = double(buf_adc[1])*dynamic_range/double(ADALM_ADC_BIT);
			// area calc
			area_ch1 += double(buf_adc[0])*dynamic_range/double(ADALM_ADC_BIT) - offset_ch1;
      area_ch2 += double(buf_adc[1])*dynamic_range/double(ADALM_ADC_BIT) - offset_ch2;
		}
    ifs >> buf_info[2] >> timestamp_end;
    ev++;
		tree->Fill();
  }
	std::cout << std::endl;
	std::cout << "... Analyzer End"<< std::endl;

  tree->Write();
  out_file->Close();

  return 0;
}

