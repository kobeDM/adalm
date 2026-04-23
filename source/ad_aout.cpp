#define _USE_MATH_DEFINES
#include <iostream>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2k.hpp>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::context;

// uncomment the following definition to test triggering
#define TRIGGERING

int main( int argc, char *argv[] )
{

    double V[2];
    int    T[2];
    int    C[2];
    //  const int freq=750000;
    const double freq = 7.5e7;
    //timestamp
    struct timeval tv;
    //  configure
    if ( argc < 3 ) {
        printf( "./main [ch0 V(V)] [ch0 T(ns)]  [ch1 V(V)] [ch1 T(ns)] \n" );
        printf( "ex) ad_out -1 100 \n" );
        return -1;
    }
    V[0] = atof( argv[1] );
    T[0] = atoi( argv[2] );
    if ( argc > 3 ) {
        V[1] = atof( argv[3] );
        T[1] = atoi( argv[4] );
    } else {
        V[1] = 0;
        T[1] = 0;
    }

    for ( int i = 0; i < 2; i++ ) {
        if ( T[i] > 0 ) {
            C[i] = T[i] / (int)( 1e9 / freq );  //clocks
            if ( C[i] == 0 )
                C[i] = 1;
        } else
            C[i] = 0;
        C[i] = ( C[i] / 4 + 1 ) * 4;
        printf( "adalm2000 output ch%d: %.3fV %dns (%d clock)\n", i, V[i], T[i], C[i] );
    }

    //  Open ADALM2000
    M2k *ctx = m2kOpen( );
    if ( !ctx ) {
        std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
        return 1;
    }

    M2kAnalogOut  *aout = ctx->getAnalogOut( );
    vector<double> freqs;
    freqs = aout->getAvailableSampleRates( 0 );
    printf( "Available closks [Hz]: " );
    for ( int i = 0; i < freqs.size( ); i++ )
        printf( "%.1e, ", freqs[i] );
    printf( "\n" );

    aout->setSampleRate( 0, freq );
    aout->setSampleRate( 1, freq );
    aout->enableChannel( 0, true );
    aout->enableChannel( 1, true );

    for ( int i = 0; i < 2; i++ ) {
        printf( "ch %d freq=%.1e Hz (%.0f ns/clock)\n", i, aout->getSampleRate( i ), 1e9 / aout->getSampleRate( i ) );
    }

    vector<double> pulse_0;
    vector<double> pulse_1;

    for ( int i = 0; i < C[0]; i++ ) {
        pulse_0.push_back( V[0] * 2 );  //for 50ohm to 50ohm
    }
    for ( int i = 0; i < C[1]; i++ ) {
        pulse_1.push_back( V[1] * 2 );  //for 50ohm to 50ohm
    }

    printf( "ch0 %d closk\n", (int)( pulse_0.size( ) ) );
    printf( "ch1 %d closk\n", (int)( pulse_1.size( ) ) );

    aout->setCyclic( false );
    aout->push( { pulse_0, pulse_1 } );

    contextClose( ctx );
}
