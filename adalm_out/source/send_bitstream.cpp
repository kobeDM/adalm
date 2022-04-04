#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <math.h>
#include <bitset>
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

bool readBitStream( const string& inputFile, vector< unsigned short >* pData );

bool checkData( vector< unsigned short >* pData );


int main(int argc, char* argv[])
{
    printf("send_bitstream: ADALM2000 digital output sample program\n");
    printf("send_bitstream [-h || -help] [-u || -URI device_URI] [-f || -file inputfile]\n");
    int hopt = 0, uopt = 0, fopt = 0;
    double rate=0.0;
    string URI = "";
    string inputfile = "bitstream.txt";
    char* dparam = nullptr;
    struct option longopts[] = {
        { "URI",  required_argument, nullptr, 'u' },
        { "help", no_argument,       nullptr, 'h' },
        { "file", required_argument, nullptr, 'f' },
        { 0,      0,                 0,        0  },
    };
    int opt       = 0;
    int longindex = 0;
    int numopt    = 0;
    while ((opt = getopt_long(argc, argv, "mlhadsuf:", longopts, &longindex)) != -1) {
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
        case 'f':
            fopt=1;
            numopt+=2;
            inputfile=optarg;
            cout <<"Input bitstream file:"<<inputfile<<endl;
            break;
        default:
            printf("error! \'%c\' \'%c\'\n", opt, optopt);
            return 1;
        }
    }
    //  cout <<"argc"<<argc<<endl;  
    //cout <<"argc"<<argc<<endl;  
    if(hopt){
        cout <<"ex: send_bitstream -u usb:1.9.5 -f bitstream_user.txt"<<endl;
        cout <<"\t to output bitstream written in bitstream_user.txt from usb:1.9.5 ADALM2000."<<endl;
        cout <<"ex: send_bitstream"<<endl;
        cout <<"\t to output bitstream written in bitstream.txt (default) from ADALM2000 which is found first."<<endl;
        return 0;
    } 
    if(uopt){
        cout<<"URI to access: "<<URI<<endl;
    }
  
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

    sleep( 1 );

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << "##         SEND BITSTREAM          ##" << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    const double dfreq=1e6; // clock for digital

    M2kDigital *dio = ctx->getDigital(); //for dio
    if( dio == nullptr ) return 1;
    dio->setSampleRateOut(dfreq);
    printf("Digital clock [Hz]: ");
    printf("%.1e\n", dio->getSampleRateOut());
	
    dio->enableAllOut(true);
    for (int i=0;i<16;i++){
        dio->setOutputMode(i,DIO_MODE(1));
        dio->setDirection(i,DIO_DIRECTION(1));
        dio->enableChannel(i,1);	  
        dio->setValueRaw(i,DIO_LEVEL(0));
    }
    dio->setCyclic(false);

    printf("Num of output channel:%d\n",dio->getNbChannelsOut());

    vector< unsigned short > dataArr;
    readBitStream( inputfile, &dataArr );
    cout << dataArr.size( ) << endl;
    // checkData( &dataArr ); // for debug
    dio->enableAllOut(true);

    dataArr.resize(10000); // NOTICE!! : BLACK MAGIC...
    dio->push( dataArr );

    dio->enableAllOut(false);
    contextClose(ctx);
    
    return 0;
}


bool readBitStream( const string& inputFile, vector< unsigned short >* pData )
{
    if( pData == nullptr ) return false;

    // initialize data array
    pData->clear( );
    
    // open inputfile
    ifstream ifs( inputFile );
    if( ifs.is_open( ) == false ) {
        cout << "Failed to open input bitstream file..." << endl;
        return false;
    }
    
    // store temporary string array
    int maxLength = 0;
    vector< string > strArr;
    strArr.reserve( 16 );
    while( !ifs.eof( ) ) {
        string line = "";
        std::getline( ifs, line );
        if( line.length( ) <= 0 || strncmp( line.c_str( ), "#", 1 ) == 0 ) continue;

        string ch = "", bsStr = "";
        stringstream ss( line );
        ss >> ch >> bsStr;

        strArr.push_back( bsStr );
        if( maxLength < bsStr.length( ) ) maxLength = bsStr.length( );
    }
        
    // format check
    if( strArr.size( ) < 16 ) {
        for( int i = strArr.size( ); i < 16; ++i ) strArr.push_back( "" );
    }

    for( int i = 0; i < 16; ++i ) {
        int tail = maxLength - strArr.at( i ).length( );
        if( tail == 0 ) continue;

        for( int j = 0; j < tail; ++j ) {
            strArr.at( i ).push_back( ' ' );
        }
    }
          
    // parse and add to data array
    for( int i = 0; i < maxLength; ++i ) {
        unsigned short data = 0;
        
        for( int ch = 0; ch < 16; ++ch ) {
            auto bit = (strArr.at( ch )).at( i );
            // cout << bit << endl;
            if( bit != '_' ) data += ( 0xffff & 1 << ch );
        }

        pData->push_back( data );
        // cout << bitset<16>( data ) << endl;
    }

    return true;
}

bool checkData( vector< unsigned short >* pData )
{
    if( pData == nullptr ) return false;
    cout << "test" << endl;
    
    for( auto data : *pData ) {
        // cout << data << endl;
        cout << bitset<16>( data ) << endl;
    }

    return false;
}
