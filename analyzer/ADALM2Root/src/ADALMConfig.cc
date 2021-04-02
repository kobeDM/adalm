#include "ADALMConfig.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>


ADALMConfig::ADALMConfig()
{
}

ADALMConfig::~ADALMConfig()
{
}


void ADALMConfig::PrintConfigJSON()
{
	
	printf("---------- configuration parameters ----------\n");
	printf("Sampling Rate                    : %.2lf Hz\n",sampling_rate);
	printf("Sampling Number                  : %d \n",     sampling_num);
	printf("Dynamic Range                    : %.2lf mV\n",dynamic_range);
	printf("Waveform Offset Calc Sampling    : %d clock\n",offset_sampling);
	printf("----------------------------------------------\n");

}

void ADALMConfig::ReadConfigJSON(std::string conffilename)
{
	std::string::size_type index_conf = conffilename.find(".json");
	if( index_conf == std::string::npos ) { 
		std::cout << "Failure!!!" << std::endl;
	}
	boost::property_tree::ptree pt;
	read_json(conffilename,pt);

	if(boost::optional<double> buf = pt.get_optional<double>("config.sampling_rate")){
		this->sampling_rate = pt.get<double>("config.sampling_rate");
	}else{
	}
	
	if(boost::optional<int> buf = pt.get_optional<int>("config.sampling_num")){
		this->sampling_num = pt.get<int>("config.sampling_num");
	}else{
	}
	
	if(boost::optional<double> buf = pt.get_optional<double>("config.dynamic_range")){
		this->dynamic_range = pt.get<double>("config.dynamic_range");
	}else{
	}
	
	if(boost::optional<int> buf = pt.get_optional<int>("config.offset_sampling")){
		this->offset_sampling = pt.get<int>("config.offset_sampling");
	}else{
	}



}


