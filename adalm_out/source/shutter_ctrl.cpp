#define _USE_MATH_DEFINES
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogout.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::context;

// ------------------------------------------------------------------
// Only ADALM2000 ch.0 is enable in this program.
// You have to change this parameter to 1 if you want to use ch.1.
const int TARGET_CHANNEL = 0;

// ------------------------------------------------------------------
// Shutter open / close parameters are fixed.
// You can change the rotation angle with below params.
const double DUTY_CYCLE_OPEN_LARGE  = 0.05;
const double DUTY_CYCLE_OPEN_MEDIUM = 0.075;
const double DUTY_CYCLE_OPEN_SMALL  = 0.105;
const double DUTY_CYCLE_CLOSE       = 0.14;

// ------------------------------------------------------------------
// Shutter control pulse frequency [Hz] is fixed.
// You DON'T HAVE TO CHANGE this value.
const double CTRL_FREQ = 50.0; // [Hz]

// ------------------------------------------------------------------
// ADALM sampling rate is set to 7.5e+5
// You DON'T HAVE TO CHANGE this value.
const double ADALM_SAMPLE_FREQ = 7.5e+05; // [Hz]

// ------------------------------------------------------------------
// ADALM output pulse hight is set to 5 V
// You DON'T HAVE TO CHANGE this value.
const double ADALM_OUTPUT_PULSE_HEIGHT = 5.0; // [V]

// ------------------------------------------------------------------
// ADALM output power supply is set to 5 V
// You DON'T HAVE TO CHANGE this value.
const double ADALM_OUTPUT_POWER_SUPPLY = 5.0; // [V]


enum ADALM_SHUTTER_STATUS {
			   SHUTTER_CLOSE       = 0,
			   SHUTTER_OPEN_SMALL  = 1,
			   SHUTTER_OPEN_MEDIUM = 2,
			   SHUTTER_OPEN_LARGE  = 3
};

void usage( )
{
    std::cout << "Usage:" << std::endl;
    std::cout << "./shutter_ctrl [CLOSE, SMALL, MEDIUM or LARGE]" << std::endl;
    std::cout << std::endl;
    std::cout << "bye..." << std::endl;
    
    return;
}

int main(int argc, char* argv[])
{
    if(argc<2){
        usage( );
        return -1;
    }
    
    // bool isOpen = false;
    ADALM_SHUTTER_STATUS status = ADALM_SHUTTER_STATUS::SHUTTER_CLOSE;
    std::string shutterStatusStr = argv[1];
    double dutyCicle = 0.0;
    if( shutterStatusStr == "CLOSE" ) {
        std::cout << std::endl;
        std::cout << "--- SHUTTER CLOSE ---" << std::endl;
        std::cout << std::endl;
	status = ADALM_SHUTTER_STATUS::SHUTTER_CLOSE;
	dutyCicle = DUTY_CYCLE_CLOSE;
    }
    else if( shutterStatusStr == "SMALL"  ) {
        std::cout << std::endl;
        std::cout << "--- SHUTTER OPEN (SMALL) ---" << std::endl;
        std::cout << std::endl;
	status = ADALM_SHUTTER_STATUS::SHUTTER_OPEN_SMALL;
	dutyCicle = DUTY_CYCLE_OPEN_SMALL;
    }
    else if( shutterStatusStr == "MEDIUM"  ) {
        std::cout << std::endl;
        std::cout << "--- SHUTTER OPEN (MEDIUM) ---" << std::endl;
        std::cout << std::endl;
	status = ADALM_SHUTTER_STATUS::SHUTTER_OPEN_MEDIUM;
	dutyCicle = DUTY_CYCLE_OPEN_MEDIUM;
    }
    else if( shutterStatusStr == "LARGE"  ) {
        std::cout << std::endl;
        std::cout << "--- SHUTTER OPEN (LARGE) ---" << std::endl;
        std::cout << std::endl;
	status = ADALM_SHUTTER_STATUS::SHUTTER_OPEN_LARGE;
	dutyCicle = DUTY_CYCLE_OPEN_LARGE;
    }
    else {
        usage( );
    }

    // Create control pulse
    int nSample = static_cast< int >( ADALM_SAMPLE_FREQ / CTRL_FREQ );
    int nHigh   = nSample * dutyCicle;

    std::cout << "Total sampling per pulse: " << nSample << ", " << 1000.0 / CTRL_FREQ << " ms" << std::endl;
    std::cout << "duty cycle: " << nHigh << ", " << 100.0 * (double)nHigh / (double)nSample << " %" << std::endl;
    
    std::vector< double > pulseArray;
    pulseArray.reserve( nSample );

    for( int idx = 0; idx < nSample; ++idx ) {
        double outputVoltage = ( idx < nHigh ) ? ADALM_OUTPUT_PULSE_HEIGHT : 0.0;
        pulseArray.push_back( -1.0*outputVoltage );
    }


    // Open ADALM
    std::cout << "Open ADALM" << std::endl;
    M2k *pContext = m2kOpen();
    if( pContext == nullptr ) {
        std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
        return -1;
	}

    // Apply voltage (5V)
    M2kPowerSupply* pPS = pContext->getPowerSupply( );
    if( pPS == nullptr ) return -1;
    std::cout << "Measuring initial voltage..." << std::endl;
    std::cout << "channel 0: " << pPS->readChannel( 0 ) << " V" << std::endl;
    
    pPS->enableChannel( 0, true );
    pPS->pushChannel( 0, ADALM_OUTPUT_POWER_SUPPLY );

    usleep(100000);
    
    std::cout <<  std::endl;
    std::cout << "Voltage set to " << ADALM_OUTPUT_POWER_SUPPLY << " V" << std::endl;
    std::cout << " Measuring current voltage..." << std::endl;
    std::cout << "channel 0: " << pPS->readChannel( 0 ) << " V" << std::endl;
    std::cout <<  std::endl;
    
    // Send control pulse
	M2kAnalogOut* pAnalogOut = pContext->getAnalogOut();
    if( pAnalogOut == nullptr ) return -1;

    pAnalogOut->setSampleRate( TARGET_CHANNEL, ADALM_SAMPLE_FREQ );
    pAnalogOut->enableChannel( TARGET_CHANNEL, true );
    pAnalogOut->setCyclic( true );


    std::cout << "Start to send control pulse..." << std::endl;
    pAnalogOut->push( TARGET_CHANNEL, pulseArray );
    sleep( 2 );
    std::cout << "Done!" << std::endl;

    std::cout << std::endl;
    
    std::cout << "End ADALM control..." << std::endl;
    pPS->pushChannel( 0, 0 );
    pPS->enableChannel( 0, false );

    std::cout << "Close ADALM" << std::endl;
    contextClose( pContext );
    
    std::cout << std::endl;
    std::cout << "Shutter control successfully worked." << std::endl;

    return 0;
}
