#define _USE_MATH_DEFINES
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
// #include <sys/time.h>
#include <time.h>
#include <math.h>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::context;

// uncomment the following definition to test triggering
#define TRIGGERING

int main(int argc, char* argv[])
{
	if(argc!=10){
		printf("./main [outfileheader] [sub_entries] [sampling_rate(Hz)] [sampling_number] [dynamic range 0(+/-25V) or 1(+/-2.5V)] [ch1 Vth(V)] [ch2 Vth(V)] [trigger source 0(ch1) or 1(ch2) or 2(or)] [Trig Edge RISE=0 or FALL=1]\n");
		printf("ex) daq out 1000 1000000 1024 2 0.1 0.1 2 0\n");
		return -1;
	}
	
	//----------------------------
	//  configure
	const char* fheader        = argv[1];
	int         sub_entries    = atoi(argv[2]);	
	double      sampling_rate  = double(atof(argv[3]));	
	double      sampling_num   = atoi(argv[4]);	
	int         dynamic_range  = atoi(argv[5]);
	double      Vth1           = double(atof(argv[6]));
	double      Vth2           = double(atof(argv[7]));
	int         trigger_source = atoi(argv[8]);
	int         trig_edge      = atoi(argv[9]);

	//----------------------------
	//  Open ADALM2000
	M2k *ctx = m2kOpen();
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	ctx->calibrateADC();
	ctx->calibrateDAC();

	M2kAnalogIn *ain = ctx->getAnalogIn();
	ain->setVerticalOffset(ANALOG_IN_CHANNEL_1,0);
	ain->setVerticalOffset(ANALOG_IN_CHANNEL_2,0);
	M2kAnalogOut *aout = ctx->getAnalogOut();

#ifdef TRIGGERING
	M2kHardwareTrigger *trig = ain->getTrigger();
#endif

	// Setup analog in
	ain->enableChannel(0, true);
	ain->enableChannel(1, true);
	ain->setSampleRate(sampling_rate);
	ain->setRange((ANALOG_IN_CHANNEL)0,(M2K_RANGE)dynamic_range);
	ain->setRange((ANALOG_IN_CHANNEL)1,(M2K_RANGE)dynamic_range);

#ifdef TRIGGERING
	// setup analog trigger
	trig->setAnalogSource((M2K_TRIGGER_SOURCE_ANALOG)trigger_source);
	trig->setAnalogDelay(-int(sampling_num/2));
	// set CH1 trigger
	trig->setAnalogCondition(0,(M2K_TRIGGER_CONDITION_ANALOG)trig_edge);
	trig->setAnalogLevel(0, Vth1);
	trig->setAnalogMode(0,ANALOG);
	// set CH2 trigger
	trig->setAnalogCondition(1,(M2K_TRIGGER_CONDITION_ANALOG)trig_edge);
	trig->setAnalogLevel(1, Vth2);
	trig->setAnalogMode(1,ANALOG);
#endif

	string strfheader = fheader;
	string fname = strfheader;
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

	int trigger_num = 0;
	int subrun_num = 0;

	//timestamp
	struct timespec tspec;
    double event_rate = 0.0;
    double rate_start = 0.0;
    clock_gettime( CLOCK_REALTIME, &tspec );
	fprintf(fconf,"RUN START : timestamp %ld.%06lu\n",tspec.tv_sec,tspec.tv_nsec);
	fflush(fconf);
	printf(" RUN START : timestamp %ld.%06lu\n",tspec.tv_sec,tspec.tv_nsec);
	while(true){
		//file close/open
		if(trigger_num%sub_entries==0){
			if(trigger_num != 0){
				fclose(ofile);
                clock_gettime( CLOCK_REALTIME, &tspec );
				fprintf(fconf,"SUBRUN%d END   : timestamp %ld.%06lu\n",subrun_num,tspec.tv_sec,tspec.tv_nsec);
				fflush(fconf);
				printf("\nSUBRUN%d END   : timestamp %ld.%06lu\n",subrun_num,tspec.tv_sec,tspec.tv_nsec);
				subrun_num++;
			}
			std::stringstream ss;
			ss << fname << "_" << subrun_num << ".dat";
			ofile = fopen(ss.str().c_str(),"w");
            clock_gettime( CLOCK_REALTIME, &tspec );
			fprintf(fconf,"SUBRUN%d START : timestamp %ld.%06lu\n",subrun_num,tspec.tv_sec,tspec.tv_nsec);
			fflush(fconf);
			printf("\nSUBRUN%d START : timestamp %ld.%06lu\n",subrun_num,tspec.tv_sec,tspec.tv_nsec);
			if(ofile == nullptr){
				printf("cannot open file .\n");
				return -1;
			}
		}

		ain->startAcquisition(sampling_num);
		auto data = ain->getSamplesRaw(sampling_num);
        clock_gettime( CLOCK_REALTIME, &tspec );

        if( trigger_num % 10 == 0 ) {
            double rate_now = static_cast< double >( tspec.tv_sec ) + (static_cast< double >( tspec.tv_nsec ) * 0.000000001);
            if( trigger_num != 0 )
                event_rate = 10.0 / ( rate_now - rate_start );
            rate_start = rate_now;
        }
        
        printf("\rTRIGGER NUM : %d,    REALTIME RATE : %.2lf Hz",trigger_num, event_rate);
		fflush(stdout);
		fprintf(ofile,"#Ev. %d %ld.%06lu\n",trigger_num,tspec.tv_sec,tspec.tv_nsec);
		fprintf(ofile,"#CH1 CH2\n");
		for(int samp=0;samp<sampling_num;samp++){
			fprintf(ofile ,"%.0lf %.0lf\n",data[0][samp],data[1][samp]);
		}

        clock_gettime( CLOCK_REALTIME, &tspec );
		fprintf(ofile,"#Write %ld.%06lu\n",tspec.tv_sec,tspec.tv_nsec);
		fflush(ofile);
		ain->stopAcquisition();
		trigger_num++;
	};
    clock_gettime( CLOCK_REALTIME, &tspec );
	printf("\nRUN END   : timestamp %ld.%06lu\n",tspec.tv_sec,tspec.tv_nsec);
	fprintf(fconf,"\nRUN END   : timestamp %ld.%06lu\n",tspec.tv_sec,tspec.tv_nsec);
	
	fclose(ofile);
	fclose(fconf);
	
	contextClose(ctx);
}
