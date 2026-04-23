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

    printf( "ad_out: ADALM2000 output sample program\n" );
    printf( "ad_out [-h || -help] [-a || -d (analog or digital output andlog is default)] [-u || -URI device_URI] [-s || --SN] [-v||--V1 V] [-t||--T1 nsec] [--V2 V] [--T2 nsec] [-f rate(Hz) (for aout)] [-n num of pulses (for aout with -f)] [-l (digital-latchup)] [-m (digital-latchdown)] \n" );

    int    hopt = 0;
    int    sopt = 0;
    int    uopt = 0;
    int    topt = 0;
    int    vopt = 0;
    int    ropt = 0;
    int    wopt = 0;
    int    fopt = 0;
    int    nopt = 0;
    int    dopt = 0;  // digital out
    int    lopt = 0;  // digital latch up
    int    mopt = 0;  // digital latch up
    int    aopt = 1;  // analog out, default
    int    pulse_done;
    int    numofpulses = 1;
    double rate        = 0;
    //  char *cparam = NULL;
    string        URI;
    char         *dparam     = NULL;
    struct option longopts[] = {
        {"ANALOG",            no_argument,       NULL, 'a'},
        {"SN",                no_argument,       NULL, 's'},
        {"T1",                required_argument, NULL, 't'},
        {"T2",                required_argument, NULL, 'r'},
        {"V1",                required_argument, NULL, 'v'},
        {"V2",                required_argument, NULL, 'w'},
        {"URI",               required_argument, NULL, 'u'},
        {"DIGITAL",           no_argument,       NULL, 'd'},
        {"DIGITAL_LATCHUP",   no_argument,       NULL, 'l'},
        {"DIGITAL_LATCHDOWN", no_argument,       NULL, 'm'},
        {"FREQ",              required_argument, NULL, 'f'},
        {"NUM_OF_PULSES",     required_argument, NULL, 'n'},
        {"help",              no_argument,       NULL, 'h'},
        {0,                   0,                 0,    0  },
    };
    int opt;
    int longindex;
    int numopt = 0;
    while ( ( opt = getopt_long( argc, argv, "mlhadsu:t:v:r:w:f:n:", longopts, &longindex ) ) != -1 ) {
        switch ( opt ) {
        case 'h':
            hopt = 1;
            numopt++;
            break;
        case 'l':
            lopt = 1;
            dopt = 1;
            aopt = 0;
            numopt++;
            break;
        case 'm':
            mopt = 1;
            dopt = 1;
            aopt = 0;
            numopt++;
            break;
        case 'a':
            aopt = 1;
            numopt++;
            break;
        case 'd':
            dopt = 1;
            aopt = 0;
            numopt++;
            break;
        case 'u':
            uopt = 1;
            numopt += 2;
            URI = optarg;
            cout << "URI to access:" << URI << endl;
            break;
        case 't':
            topt = 1;
            numopt += 2;
            T[0] = dT = (atoi)( optarg );
            break;
        case 'r':
            ropt = 1;
            numopt += 2;
            T[1] = (atoi)( optarg );
            break;
        case 'v':
            vopt = 1;
            numopt += 2;
            V[0] = (atof)( optarg );
            break;
        case 'w':
            wopt = 1;
            numopt += 2;
            V[1] = (atof)( optarg );
            break;
        case 's':
            sopt = 1;
            numopt++;
            break;
        case 'f':
            fopt = 1;
            rate = (atof)( optarg );
            numopt++;
            break;
        case 'n':
            nopt        = 1;
            numofpulses = (atoi)( optarg );
            numopt++;
            break;
        default:
            printf( "error! \'%c\' \'%c\'\n", opt, optopt );
            return 1;
        }
    }
    if ( hopt ) {
        cout << "ex: ad_out -u usb:1.9.5 --T1 300 --V1 -0.7 --T2 1000 --V2 -1.5 " << endl;
        cout << "\t to output analog signals to CH1 (-0.7V 300ns) and CH2 (-1.5V 1000ns) connected to usb:1.9.5." << endl;
        cout << "ex: ad_out -d usb:1.9.5 --T1 300" << endl;
        cout << "\t to output digital signals to D0:D15 (300ns) connected to usb:1.9.5." << endl;
        cout << "ex: ad_out -s" << endl;
        cout << "\t to get the serial number of available adalm2000." << endl;
        cout << "ex: ad_out -s  -u usb:1.9.5" << endl;
        cout << "\t to get the serial number adalm2000 connected to usb:1.9.5." << endl;
        return 0;
    }
    if ( aopt ) {
        cout << "**** Analog output mode is selected. ****" << endl;
        dopt = 0;
    }
    if ( dopt ) {
        if ( lopt ) {
            cout << "**** Digital latch up is selected. ****" << endl;
        }
        if ( mopt ) {
            cout << "**** Digital latch down is selected. ****" << endl;
        } else
            cout << "**** Digital output mode is selected. ****" << endl;
    }
    if ( uopt ) {
        cout << "URI to access: " << URI << endl;
    }
    if ( !topt ) {
        T[0] = dT = 500;
    }
    if ( !vopt ) {
        V[0] = V[1] = -1;
    }
    if ( !ropt ) {
        T[1] = 500;
    }
    if ( !wopt ) {
        V[1] = -1;
    }

    if ( aopt ) {
        for ( int i = 0; i < 2; i++ ) {
            if ( T[i] > 0 ) {
                C[i] = T[i] / (int)( 1e9 / freq );  //clocks
                if ( C[i] == 0 )
                    C[i] = 1;
            } else
                C[i] = 0;
            C[i] = ( C[i] / 4 + 1 ) * 4;
        }
    }
    if ( dopt && !lopt && !mopt ) {
        dclocks = ( (int)( dT * dfreq * 1e-9 ) ) / 4 * 4;
        if ( dclocks < 16 )
            dclocks = 16;
        printf( "%dns (%d clocks)\n", dT, dclocks );
    }

    //  Open ADALM2000
    M2k *ctx;
    if ( uopt ) {
        ctx = m2kOpen( URI.c_str( ) );
    } else {
        ctx = m2kOpen( );
    }

    if ( !ctx ) {
        std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
        return 1;
    }

    string SN = ctx->getSerialNumber( );
    URI       = ctx->getUri( );

    cout << "URI:" << URI << endl;
    cout << "serial number:" << SN << endl;

    if ( sopt ) {
        return 1;
    }

    if ( dopt ) {
        M2kDigital *dio = ctx->getDigital( );  //for dio
        dio->setSampleRateOut( dfreq );
        printf( "Digital closk [Hz]: " );
        printf( "%.1e\n", dio->getSampleRateOut( ) );

        for ( int i = 0; i < 16; i++ ) {
            dio->setOutputMode( i, DIO_MODE( dmode ) );
            dio->setDirection( i, DIO_DIRECTION( ddir ) );
        }
        dio->setCyclic( false );

        if ( lopt ) {
            int ch = 6;
            for ( int i = 0; i < ch; i++ ) {
                dio->setValueRaw( i, DIO_LEVEL( 1 ) );
            }
            for ( int i = 0; i < ch; i++ ) {
                dio->setValueRaw( i + 8, DIO_LEVEL( 0 ) );
            }
            cout << "digital 0-5ch latched up." << endl << flush;
            while ( 1 ) {
                sleep( 1 );
            }
        }
        if ( mopt ) {
            int ch = 6;
            for ( int i = 0; i < ch; i++ ) {
                dio->setValueRaw( i, DIO_LEVEL( 0 ) );
            }
            for ( int i = 0; i < ch; i++ ) {
                dio->setValueRaw( i + 8, DIO_LEVEL( 0 ) );
            }
        } else {
            for ( int i = 0; i < 8; i++ ) {
                dio->setValueRaw( i, DIO_LEVEL( 0 ) );
                dio->setValueRaw( i + 8, DIO_LEVEL( 1 ) );
            }
            unsigned short ddata[1024];
            for ( int i = 0; i < dclocks; i++ ) {
                ddata[i] = ( 1 << 8 ) - 1;  //D0-D7 HIGH / D8-D15 LOW (D0 for least bit(1), D1 second least bit(2)... D15 largest)
            }
            for ( int i = dclocks; i < 2 * dclocks; i++ ) {
                ddata[i] = ( 1 << 16 ) - ( 1 << 8 );  //D0-D7 LOW / D8-D15 HIGH (0xf0)
            }
            printf( "Num of output channel:%d\n", dio->getNbChannelsOut( ) );
            dio->enableAllOut( true );
            dio->push( ddata, 2 * dclocks );
        }
    }

    if ( aopt ) {
        M2kAnalogOut *aout = ctx->getAnalogOut( );  //for analog

        for ( int i = 0; i < 2; i++ ) {
            aout->setSampleRate( i, freq );
            aout->enableChannel( i, true );
            printf( "ch %d freq=%.1e Hz (%.0f ns/clock)", i, aout->getSampleRate( i ), 1e9 / aout->getSampleRate( i ) );
            printf( "\n" );
        }

        vector<double> pulse_0;
        vector<double> pulse_1;
        for ( int i = 0; i < C[0]; i++ )
            pulse_0.push_back( 0 );
        for ( int i = C[0]; i < 2 * C[0]; i++ )
            pulse_0.push_back( V[0] );
        for ( int i = 2 * C[0]; i < 3 * C[0]; i++ )
            pulse_0.push_back( 0 );
        for ( int i = 0; i < C[1]; i++ )
            pulse_1.push_back( 0 );
        for ( int i = C[1]; i < 2 * C[1]; i++ )
            pulse_1.push_back( V[1] );
        for ( int i = 2 * C[1]; i < 3 * C[1]; i++ )
            pulse_1.push_back( 0 );

        printf( "ch0 %.3lfV %dns (%d clock)\t", V[0], T[0], (int)pulse_0.size( ) );
        if ( nopt )
            printf( "%d pulses ", (int)numofpulses );
        if ( fopt )
            printf( "repeting at %d Hz.", (int)rate );
        printf( "\n" );
        printf( "ch1 %.3lfV %dns (%d clock)\t", V[1], T[1], (int)pulse_1.size( ) );
        if ( nopt )
            printf( "%d pulses ", (int)numofpulses );
        if ( fopt )
            printf( "repeting at %d Hz.", (int)rate );
        printf( "\n" );

        aout->setCyclic( false );

        aout->push( 0, pulse_0 );
        aout->push( 1, pulse_1 );

        pulse_done = 1;
        while ( fopt ) {
            usleep( int( 1e6 / rate ) );
            aout->push( 0, pulse_0 );
            aout->push( 1, pulse_1 );
            pulse_done++;
            if ( nopt && pulse_done >= numofpulses )
                break;
        }
    }

    contextClose( ctx );
}
