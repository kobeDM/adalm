#define _USE_MATH_DEFINES
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <math.h>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/digital/m2kdigital.hpp> //for dio

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::digital; //for dio
using namespace libm2k::context;

// uncomment the following definition to test triggering
#define TRIGGERING

int main(int argc, char* argv[])
{
  double voltArray[2];

  printf("send_bitstream: ADALM2000 digital output sample program\n");
  printf("send_bitstream [-h || -help] [-u || -URI device_URI] [-s || --SN] [-v||--V1 V] [--V2 V]\n");
  int hopt = 0, uopt = 0, vopt=0, wopt=0;
  double rate=0.0;
  string URI = "";
  char* dparam = nullptr;
  struct option longopts[] = {
    { "V1",   required_argument, nullptr, 'v' },
    { "V2",   required_argument, nullptr, 'w' },
    { "URI",  required_argument, nullptr, 'u' },
    { "help", no_argument,       nullptr, 'h' },
    { 0,      0,                 0,        0  },
  };
  int opt       = 0;
  int longindex = 0;
  int numopt    = 0;
  while ((opt = getopt_long(argc, argv, "mlhadsu:t:v:r:w:f:", longopts, &longindex)) != -1) {
    switch (opt) {
    case 'h':
      hopt = 1;
      numopt++;
      break;
    case 'u':
      uopt=1;
      numopt+=2;
      URI=optarg;
      cout <<"URI to access:"<<URI<<endl;
      break;
    case 'v':
      vopt=1;
      numopt+=2;
      voltArray[0]=(atof)(optarg);
      break;
    case 'w':
      wopt=1;
      numopt+=2;
      voltArray[1]=(atof)(optarg);
      break;
    default:
      printf("error! \'%c\' \'%c\'\n", opt, optopt);
      return 1;
    }
  }
  //  cout <<"argc"<<argc<<endl;  
    //cout <<"argc"<<argc<<endl;  
  if(hopt){
    cout <<"ex: ad_out -u usb:1.9.5 --T1 300 --V1 -0.7 --T2 1000 --V2 -1.5 "<<endl;
    cout <<"\t to output analog signals to CH1 (-0.7V 300ns) and CH2 (-1.5V 1000ns) connected to usb:1.9.5."<<endl;
    cout <<"ex: ad_out -d usb:1.9.5 --T1 300"<<endl;
    cout <<"\t to output digital signals to D0:D15 (300ns) connected to usb:1.9.5."<<endl;
    cout <<"ex: ad_out -s"<<endl;
    cout <<"\t to get the serial number of available adalm2000."<<endl;
    cout <<"ex: ad_out -s  -u usb:1.9.5"<<endl;
    cout <<"\t to get the serial number adalm2000 connected to usb:1.9.5."<<endl;
    return 0;
  } 
  if(uopt){
    cout<<"URI to access: "<<URI<<endl;
  }
  if(!vopt){
    voltArray[0]=voltArray[1]=0;
  }
  if(!wopt){
    voltArray[1]=0;
  }
  
  //----------------------------
  //  configure
  /**	const char* fheader = argv[1];
	int sub_entries = atoi(argv[2]);	
	double sampling_rate = double(atof(argv[3]));	
	double sampling_num = atoi(argv[4]);	
	int dynamic_range = atoi(argv[5]); // V	
	double Vth1 = double(atof(argv[6])); // V (TRIGGER THRESHOLD)	
	double Vth2 = double(atof(argv[7])); // V	(TRIGGER THRESHOLD)
	int trigger_source = atoi(argv[8]);
	int trig_edge = atoi(argv[9]);
  **/

  //----------------------------
  //  Open ADALM2000
  M2k *ctx;//= m2kOpen();
  if(uopt){
    ctx = m2kOpen(URI.c_str());
  }
  else{
    ctx = m2kOpen();
  }
    
  if (!ctx) {
    std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
    return 1;
  }
  
  string SN=ctx->getSerialNumber();
  //string
  URI=ctx->getUri();
  cout<<"URI:"<<URI<<endl;
  cout<<"serial number:"<<SN<<endl;

  M2kPowerSupply* pPS = ctx->getPowerSupply( );
  if( pPS == nullptr ) {
    std::cout << "ABORT!!!" << std::endl;
    abort( );
  }
  else {
    std::cout << "OK!" << std::endl;
  }

  
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "#####################################" << std::endl;
  std::cout << "##           SET VOLTAGE           ##" << std::endl;
  std::cout << "#####################################" << std::endl;
  std::cout << std::endl;
  
  std::cout << "Measuring current voltage..." << std::endl;
  std::cout << "channel 0: " << pPS->readChannel( 0 ) << " V" << std::endl;
  std::cout << "channel 1: " << pPS->readChannel( 1 ) << " V" << std::endl;

  std::cout << std::endl;
  std::cout << std::endl;
  
  std::cout << "Apply voltage..." << std::endl;
  pPS->enableChannel( 0, true );
  pPS->enableChannel( 1, true );
  pPS->pushChannel( 0, voltArray[0] );
  pPS->pushChannel( 1, voltArray[1] );
  std::cout << "Done!, re-measuring current voltage..." << std::endl;
  std::cout << "channel 0: " << pPS->readChannel( 0 ) << " V" << std::endl;
  std::cout << "channel 1: " << pPS->readChannel( 1 ) << " V" << std::endl;



  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "#####################################" << std::endl;
  std::cout << "##         SEND BITSTREAM          ##" << std::endl;
  std::cout << "#####################################" << std::endl;
  std::cout << std::endl;

  
  
  contextClose(ctx);

  return 0;
}
