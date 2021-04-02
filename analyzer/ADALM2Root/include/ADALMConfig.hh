#ifndef ADALMConfig_hh
#define ADALMConfig_hh

#include <stdio.h>
#include <string>
#include <iostream>

class ADALMConfig
{

	private:

	public:
		ADALMConfig();
		~ADALMConfig();
		
		double sampling_rate = 100000000; // Hz
		int    sampling_num  = 1024; // Hz
		double dynamic_range = 5000; // mV
		int offset_sampling = 100; //clock
		
		void ReadConfigJSON(std::string filename);
		void PrintConfigJSON();


};

#endif
