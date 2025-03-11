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
	
  double V[2];
  int T[2];
  int C[2];
  int dT,dclocks;
  int dmode=1;//0 for opendrain, 1 for pushpull
  int ddir=1;//0for input, 1 for output
  const double freq=7.5e7; // clock for analog out
  const double dfreq=1e8; // clock for digital
  //timestamp
  //  struct timeval tv;
  //configure
  //   if(argc<1){
        //return -1;
  //}

  printf("ADALM2000 digial I/O sample (D0-D7 pos, D8-D15 neg output 500ns)  \n");
  int sopt = 0;
  int uopt = 0;
  int dopt = 0;
  //  char *cparam = NULL;
  string URI;
  char *dparam = NULL;
  struct option longopts[] = {
      { "SN",    no_argument,       NULL, 's' },
      { "URI",  required_argument, NULL, 'u' },
      { "delete", optional_argument, NULL, 'd' },
      { 0,        0,                 0,     0  },
  };
  int opt;
  int longindex;
  int numopt=0;
  while ((opt = getopt_long(argc, argv, "su:d::", longopts, &longindex)) != -1) {
    //    printf("%d %s\n", longindex, longopts[longindex].name);
    switch (opt) {
    case 's':
      sopt = 1;
      numopt++;
      //argc--;
      break;
    case 'u':
      uopt = 1;
      numopt+=2;
      //cout <<"argc"<<argc<<endl;  
      //      argc-=2;
      //	cout <<"argc"<<argc<<endl;  
      //        cparam = optarg;
      //      cout <<"optarg:"<<optarg<<endl;
      URI=optarg;
      //cout <<"URI to read:"<<URI<<endl;
      //
      break;
    case 'd':
      dopt = 1;
      numopt++;
      dparam = optarg;
      break;
    default:
      printf("error! \'%c\' \'%c\'\n", opt, optopt);
      return 1;
    }
  }
  //  cout <<"argc"<<argc<<endl;  
  argc-=numopt;
  //cout <<"argc"<<argc<<endl;  
  
  if(sopt){
    //printf("s = %d\n", sopt);
    cout <<"SN check mode is selected."<<endl;
  }
  //  printf("b = %d\n", bopt);
  if(uopt){
    //printf("u = %d, %s\n", uopt, URI.c_str());
  //  printf("c = %d, %s\n", uopt, cparam);
    cout<<"URI to read:"<<URI<<endl;
  }
  //  printf("d = %d, %s\n", dopt, dparam);
  //  for (int i = optind; i < argc; i++) {
  //  printf("arg = %s\n", argv[i]);
  // }

  if(argc>1){
    //    dch[i] = atof(argv[1+i*2]);
    dT = atoi(argv[1+numopt]);
  }
  else{
    printf("./ad_dout [-u || -URI device_URI] [-s || --SN] [nsec (min. 160ns)]  \n");
    printf("ex) ad_dout 300 \n");
    dT=500; //default 500ns
  }
  dclocks=((int)(dT*dfreq*1e-9))/4*4;
  if(dclocks<16)dclocks=16;
  printf("%dns (%d clocks)\n",dT,dclocks);
  
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
  //  uopt=0;
  M2k *ctx;//= m2kOpen();
  ctx= m2kOpen();
  if (!ctx) {
    std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
    return 1;
  }


  if(uopt){
    //    M2k *ctx = m2kOpen(URI.c_str());
    ctx = m2kOpen(URI.c_str());
    if (!ctx) {
      std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
      return 1;
      
    }
  }


  
  string SN=ctx->getSerialNumber();
  //string
  URI=ctx->getUri();
  //  cout<<"serial number:"<<ctx->getSerialNumber()<<endl;
  cout<<"URI:"<<URI<<endl;
  cout<<"serial number:"<<SN<<endl;

  if(sopt){
    return 1;
  }
  
  M2kAnalogOut *aout = ctx->getAnalogOut();
  M2kDigital *dio = ctx->getDigital(); //for dio
  dio->setSampleRateOut(dfreq);
  printf("Digital closk [Hz]: ");
  printf("%.1e\n", dio->getSampleRateOut());
	
  for (int i=0;i<16;i++){
    dio->setOutputMode(i,DIO_MODE(dmode));
    dio->setDirection(i,DIO_DIRECTION(ddir));
    //	  dio->enableChannel(i,1);	  
  }
    dio->setCyclic(false);
  //dio->setCyclic(true);
  //      dio->enableAllOut(true);
  for (int i=0;i<8;i++){
  dio->setValueRaw(i,DIO_LEVEL(0));
  dio->setValueRaw(i+8,DIO_LEVEL(1));
    //dio->setValueRaw(i+8,DIO_LEVEL(0));
  }
    dio->enableAllOut(true);
  //  for (int i=8;i<15;i++){
  // }
  //	dio->setCyclic(true);
  //vector<unsigned short> bufferout;

    unsigned short  ddata[1024];
  //	  unsigned short  ddata[128];
	  //	  for(int i=0;i<2;i++)	 ddata.push_back(40);
	  //for(int i=0;i<30;i++)	 ddata.push_back(1);
	  //  for(int i=0;i<32;i++){
	  // bufferout.push_back(i);
	  // }
	  for(int i=0;i<dclocks;i++){
	    //	  for(int i=0;i<4;i++){
	    //ddata[i]=255;//D0 for least bit(1), D1 second least bit(2)... D15 largest
	    ddata[i]=(1<<8)-1;//D0 for least bit(1), D1 second least bit(2)... D15 largest
	    //	    printf("%d",(1<<8));
	    //ddata[i]=0x0f;//D0 for least bit(1), D1 second least bit(2)... D15 largest
	  }
	  for(int i=dclocks;i<2*dclocks;i++){
	    // ddata[i]=256;
	    //	    ddata[i]=65280;//0xf0
	    ddata[i]=(1<<16)-(1<<8);//0xf0
	    //ddata[i]=(unsigned short)0xf0;
	  }
	  //	  for(int i=60;i<90;i++){
	  //  ddata[i]=2;
	  // }
	  //	for(int i=0;i<1000;i++)
	  printf("Num of output channel:%d\n",dio->getNbChannelsOut());
	  dio->push(ddata,2*dclocks);
	  //dio->push(bufferout);//with vector
	  //vector<double> freqs;
	  //	  freqs=aout->getAvailableSampleRates(0);
	  //	  sleep(3);
	  //printf("Available closks [Hz]: ");
	  //for (int i = 0; i < freqs.size(); i++)
	  //printf("%.1e, ", freqs[i]);
	  //printf("\n");

	//	double dfreq;
	//	dfreq=dio->getSampleRateIn();
  
	//#ifdef TRIGGERING
	//M2kHardwareTrigger *trig = ain->getTrigger();
	//#endif

	// Setup analog in
	//ain->enableChannel(0, true);
	//ain->enableChannel(1, true);
	//ain->setSampleRate(sampling_rate);
	//ain->setRange((ANALOG_IN_CHANNEL)0,(M2K_RANGE)dynamic_range);
	//ain->setRange((ANALOG_IN_CHANNEL)1,(M2K_RANGE)dynamic_range);
	//ain->setRange((ANALOG_IN_CHANNEL)0,-dynamic_range,dynamic_range);
	//ain->setRange((ANALOG_IN_CHANNEL)1,-dynamic_range,dynamic_range);

	//#ifdef TRIGGERING
	// setup analog trigger
	//trig->setAnalogSource((M2K_TRIGGER_SOURCE_ANALOG)trigger_source);
	//trig->setAnalogSource(CHANNEL_1_OR_CHANNEL_2);
	//trig->setAnalogDelay(-int(sampling_num/2));
	// set CH1 trigger
	//trig->setAnalogCondition(0,(M2K_TRIGGER_CONDITION_ANALOG)trig_edge);
	//trig->setAnalogLevel(0, Vth1);
	//trig->setAnalogMode(0,ANALOG);
	// set CH2 trigger
	//trig->setAnalogCondition(1,(M2K_TRIGGER_CONDITION_ANALOG)trig_edge);
	//trig->setAnalogLevel(1, Vth2);
	//	trig->setAnalogMode(1,ANALOG);
	//#endif

	// for test
	// setup analog output
	//	aout->setSampleRate(0,750000);
	//aout->setSampleRate(1,750000);
	//std::fout<<freqs<<endl;
	
	//	aout->setSampleRate(0,freq);
	//aout->setSampleRate(1,freq);
	//	aout->enableChannel(0, true);
	//aout->enableChannel(1, true);
	//	for(int i=0;i<dchnum;i++){  

	
	//	for(int i=0;i<2;i++){
	//  printf("ch %d freq=%.1e Hz (%.0f ns/clock)\n",i,aout->getSampleRate(i),1e9/aout->getSampleRate(i));
	//}
	// create output buffers
	//vector<double> sinv;
	//vector<double> saw;
	//	vector<double> pulse;
	//	vector<double> pulse_0;
	//vector<double> pulse_1;
	//for(int i=0;i<T[0]/(int)(1e9/freq)+1;i++)
	//	for(int i=0;i<1024;i++)
	//for(int i=0;i<C[0];i++)
	//	{
	  //	  double rad = 2*M_PI*(i/1024.0);
	  //double val = 1.*sin(rad);
	  //	  sinv.push_back(val);
	  //	  sinv.push_back(-1);
	  //sinv.push_back(V[0]);
	  //saw.push_back((0*i)/1024.0);
	  //	  pulse_0.push_back(V[0]);
	  //	  pulse_0.push_back(-1);
	  //	  pulse_0.push_back(V[0]);
	  //pulse_0.push_back(V[0]*2);//for 50ohm to 50ohm
	  //pulse[i]=V[0];
	  //}
	  //for(int i=0;i<C[1];i++)
	//	{
	  //	  pulse_1.push_back(V[1]);
	  //pulse_1.push_back(V[1]*2);//for 50ohm to 50ohm
	//}
	//	for(int i=0;i<2;i++){
	//printf("ch0 %d closk\n",(int)pulse_0.size());
	// printf("ch1 %d closk\n",(int)pulse_1.size());
	  //}
	//	aout->setCyclic(true);
	//aout->setCyclic(false);
	//aout->push({sinv,saw});
		//aout->push({saw,sinv});
	//aout->push(0,sinv);
	//	aout->push({pulse_0,pulse_1});
	//aout->push(0,pulse_0);
	//aout->push(1,pulse_0);
		//		aout->push(0,pulse_0);
	//sleep(3);


	//string strfheader = fheader;
	//string fname = strfheader + ".dat";
	/**string fname = strfheader;
	string fconfname = strfheader + ".cnf";
	printf("-------- SETTING CONFIGURE --------\n");
	printf("RUN CONFIG FILE NAME   : %s\n",        fconfname.c_str());
	printf("OUTPUT FILE NAME       : %s\n",        fname.c_str());
	printf("SAMPLING RATE          : %.1lf (Hz)\n",sampling_rate);
	printf("SAMPLING NUMBER        : %.0lf\n",     sampling_num);
	printf("DYNAMIC RANGE          : %d (0=+/-25V,1=+/-2.5V)\n", dynamic_range);
	printf("TRIGGER THRESHOLD CH1  : %.3lf (V)\n", Vth1);
	printf("TRIGGER THRESHOLD CH2  : %.3lf (V)\n", Vth2);
	printf("TRIGGER SOURCE         : %d    [ch1=0,ch2=1,or-trig=2,and-trig=3]\n",        trigger_source);
	printf("TRIGGER EDGE (R=0,F=1) : %d\n",        trig_edge);
	printf("-----------------------------------\n");
	FILE* fconf;
	fconf = fopen(fconfname.c_str(),"w");
	fprintf(fconf,"---------- RUN CONFIGURE ----------\n");
	fprintf(fconf,"RUN CONFIG FILE NAME   : %s\n",        fconfname.c_str());
	fprintf(fconf,"OUTPUT FILE NAME       : %s\n",        fname.c_str());
	fprintf(fconf,"SAMPLING RATE          : %.1lf (Hz)\n",sampling_rate);
	fprintf(fconf,"SAMPLING NUMBER        : %.0lf\n",     sampling_num);
	fprintf(fconf,"DYNAMIC RANGE          : %d (0=+/-25V,1=+/-2.5V)\n", dynamic_range);
	fprintf(fconf,"TRIGGER THRESHOLD CH1  : %.3lf (V)\n", Vth1);
	fprintf(fconf,"TRIGGER THRESHOLD CH2  : %.3lf (V)\n", Vth2);
	fprintf(fconf,"TRIGGER SOURCE         : %d    [ch1=0,ch2=1,or-trig=2,and-trig=3]\n",trigger_source);
	fprintf(fconf,"TRIGGER EDGE (R=0,F=1) : %d\n",        trig_edge);
	fprintf(fconf,"-----------------------------------\n");

	FILE* ofile;
	**/
	/**	int trigger_num = 0;
	int subrun_num = 0;
	gettimeofday(&tv,NULL);
	fprintf(fconf,"RUN START : timestamp %ld.%06lu\n",tv.tv_sec,tv.tv_usec);
	fflush(fconf);
	printf(" RUN START : timestamp %ld.%06lu\n",tv.tv_sec,tv.tv_usec);**/
	/**	while(true){
		//file close/open
		if(trigger_num%sub_entries==0){
			if(trigger_num != 0){
				fclose(ofile);
				gettimeofday(&tv,NULL);
				fprintf(fconf,"SUBRUN%d END   : timestamp %ld.%06lu\n",subrun_num,tv.tv_sec,tv.tv_usec);
				fflush(fconf);
				printf("\nSUBRUN%d END   : timestamp %ld.%06lu\n",subrun_num,tv.tv_sec,tv.tv_usec);
				subrun_num++;
			}
			std::stringstream ss;
			ss << fname << "_" << subrun_num << ".dat";
			ofile = fopen(ss.str().c_str(),"w");
			gettimeofday(&tv,NULL);
			fprintf(fconf,"SUBRUN%d START : timestamp %ld.%06lu\n",subrun_num,tv.tv_sec,tv.tv_usec);
			fflush(fconf);
			printf("\nSUBRUN%d START : timestamp %ld.%06lu\n",subrun_num,tv.tv_sec,tv.tv_usec);
			if(ofile == NULL){
				printf("cannot open file .\n");
				return -1;
			}
		}

		ain->startAcquisition(sampling_num);
		auto data = ain->getSamplesRaw(sampling_num);
		printf("\rTRIGGER NUM : %d",trigger_num);
		fflush(stdout);
		gettimeofday(&tv,NULL);
		fprintf(ofile,"#Ev. %d %ld.%06lu\n",trigger_num,tv.tv_sec,tv.tv_usec);
		fprintf(ofile,"#CH1 CH2\n");
		for(int samp=0;samp<sampling_num;samp++){
			fprintf(ofile ,"%.0lf %.0lf\n",data[0][samp],data[1][samp]);
		}
		/*
		for(double val : data[0])
		{
			fprintf(ofile,"%.0lf\n",val);
		}
		fprintf(ofile,"CH2\n");
		for(double val : data[1])
		{
			fprintf(ofile,"%.0lf\n",val);
		}
	
		gettimeofday(&tv,NULL);
		fprintf(ofile,"#Write %ld.%06lu\n",tv.tv_sec,tv.tv_usec);
		fflush(ofile);
		ain->stopAcquisition();
		trigger_num++;
	};**/
	/**	gettimeofday(&tv,NULL);
	printf("\nRUN END   : timestamp %ld.%06lu\n",tv.tv_sec,tv.tv_usec);
	fprintf(fconf,"\nRUN END   : timestamp %ld.%06lu\n",tv.tv_sec,tv.tv_usec);
	
	fclose(ofile);**/
	//	fclose(fconf);	
	contextClose(ctx);
}
