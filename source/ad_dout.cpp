#define _USE_MATH_DEFINES
#include <getopt.h>
#include <iostream>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/digital/m2kdigital.hpp>  //for dio
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
using namespace libm2k::digital;  //for dio
using namespace libm2k::context;

int main( int argc, char *argv[] )
{

    double       V[2];
    int          T[2];
    int          C[2];
    int          dT, dclocks;
    int          dmode = 1;      // 0 for opendrain, 1 for pushpull
    int          ddir  = 1;      // 0 for input, 1 for output
    const double freq  = 7.5e7;  // clock for analog out
    const double dfreq = 1e8;    // clock for digital

    printf( "ADALM2000 digial I/O sample (D0-D7 pos, D8-D15 neg output 500ns)  \n" );
    int sopt = 0;
    int uopt = 0;
    int dopt = 0;
    //  char *cparam = NULL;
    string        URI;
    char         *dparam     = NULL;
    struct option longopts[] = {
        {"SN",     no_argument,       NULL, 's'},
        {"URI",    required_argument, NULL, 'u'},
        {"delete", optional_argument, NULL, 'd'},
        {0,        0,                 0,    0  },
    };
    int opt;
    int longindex;
    int numopt = 0;
    while ( ( opt = getopt_long( argc, argv, "su:d::", longopts, &longindex ) ) != -1 ) {
        switch ( opt ) {
        case 's':
            sopt = 1;
            numopt++;
            break;
        case 'u':
            uopt = 1;
            numopt += 2;
            URI = optarg;
            break;
        case 'd':
            dopt = 1;
            numopt++;
            dparam = optarg;
            break;
        default:
            printf( "error! \'%c\' \'%c\'\n", opt, optopt );
            return 1;
        }
    }
    argc -= numopt;
    if ( sopt ) {
        cout << "SN check mode is selected." << endl;
    }
    if ( uopt ) {
        cout << "URI to read:" << URI << endl;
    }

    if ( argc > 1 ) {
        dT = atoi( argv[1 + numopt] );
    } else {
        printf( "./ad_dout [-u || -URI device_URI] [-s || --SN] [nsec (min. 160ns)]  \n" );
        printf( "ex) ad_dout 300 \n" );
        dT = 500;  //default 500ns
    }
    dclocks = ( (int)( dT * dfreq * 1e-9 ) ) / 4 * 4;
    if ( dclocks < 16 )
        dclocks = 16;
    printf( "%dns (%d clocks)\n", dT, dclocks );

    //----------------------------
    //  Open ADALM2000
    //----------------------------

    M2k *ctx;
    ctx = m2kOpen( );
    if ( !ctx ) {
        std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
        return 1;
    }

    if ( uopt ) {
        ctx = m2kOpen( URI.c_str( ) );
        if ( !ctx ) {
            std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
            return 1;
        }
    }

    string SN = ctx->getSerialNumber( );
    //string
    URI = ctx->getUri( );
    cout << "URI:" << URI << endl;
    cout << "serial number:" << SN << endl;

    if ( sopt ) {
        return 1;
    }

    M2kAnalogOut *aout = ctx->getAnalogOut( );
    M2kDigital   *dio  = ctx->getDigital( );  //for dio
    dio->setSampleRateOut( dfreq );
    printf( "Digital closk [Hz]: " );
    printf( "%.1e\n", dio->getSampleRateOut( ) );

    for ( int i = 0; i < 16; i++ ) {
        dio->setOutputMode( i, DIO_MODE( dmode ) );
        dio->setDirection( i, DIO_DIRECTION( ddir ) );
    }
    dio->setCyclic( false );
    for ( int i = 0; i < 8; i++ ) {
        dio->setValueRaw( i, DIO_LEVEL( 0 ) );
        dio->setValueRaw( i + 8, DIO_LEVEL( 1 ) );
    }
    dio->enableAllOut( true );

    unsigned short ddata[1024];
    for ( int i = 0; i < dclocks; i++ ) {
        ddata[i] = ( 1 << 8 ) - 1;  //D0 for least bit(1), D1 second least bit(2)... D15 largest
    }
    for ( int i = dclocks; i < 2 * dclocks; i++ ) {
        ddata[i] = ( 1 << 16 ) - ( 1 << 8 );  //0xf0
    }

    printf( "Num of output channel:%d\n", dio->getNbChannelsOut( ) );

    dio->push( ddata, 2 * dclocks );

    contextClose( ctx );
}
