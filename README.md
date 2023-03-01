# ADALM DAQ INSTALATION
read this README.md or adalm_daq/README.md (old README.md)

Installation flow : libiio and libini -> libm2k -> adalm daq 
## basic
```
$sudo apt install git cmake swig g++ python3-dev python3-setuptools
```

## libiio and libini 
You have to install libiio and libini before libmk2 installation
see! URL : https://wiki.analog.com/resources/tools-software/linux-software/libiio

```
# libini
$ cd ~/
$ mkdir tmp; cd tmp
$ sudo apt install bison flex libcdk5-dev swig
$ sudo apt install libaio-dev libusb-1.0-0-dev libserialport-dev libxml2-dev libavahi-client-dev doxygen graphviz
$ git clone https://github.com/pcercuei/libini.git
$ cd libini
$ mkdir build && cd build && cmake ../ && make && sudo make install

# libiio
$ cd ~/tmp
$ git clone https://github.com/analogdevicesinc/libiio.git
$ cd libiio
$ cmake ./
$ make all
$ sudo make install
```

## libm2k
```
$ cd tmp
$ git clone https://github.com/analogdevicesinc/libm2k.git
$ mkdir libm2k-build
$ cd libm2k-build
$ cmake ../libm2k
$ make
$ sudo make install
```

### LOCAL INSTALATON (install path specific, not usually, optional)

```
$ cd ~
$ mkdir local 
$ cd ~/local
$ git clone https://github.com/analogdevicesinc/libm2k.git
$ cd libm2k
$ mkdir libm2k-build
$ cd libm2k-build
$ cmake --prefix=../libm2k-install -DENABLE_EXCEPTIONS=TRUE ../
$ make
$ make install
```

## ADALM DAQ TOOL
```
$ git clone https://github.com/kobeDM/adalm.git
$ cd adalm/adalm_daq
$ mkdir build
$ cd build
$ cmake ../source
$ make install
$ ls ../bin
daq daq_test # if you don't want to test, remove daq_test line from CMakeLists.txt

# FOR TEST
$ cd ${adalm_daq}
$ mkdir bench
$ ../bin/daq_test out 100 1000000 1024 2 0.1 0.1 2 0
$ ls
out.dat out.cnf

If ”error while loading shared libraries: libm2k.so.0.7.0: cannot open shared object file: No such file or directory” happens,
$sudo /sbin/ldconfig 
```

## ADALM OUTPUT SAMPLES
```
# COMPLIE
$ cd ${adalm_out}
$ mkdir build
$ cmake ../source
$make
$./ad_out

# FOR TEST
ad_out: ADALM2000 output sample program
ad_out [-h || -help] [-a || -d (analog or digital output andlog is default)] [-u || -URI device_URI] [-s || --SN] [-v||--V1 V] [-t||--T1 nsec] [--V2 V] [--T2 nsec] 
ex: ad_out -u usb:1.9.5 --T1 300 --V1 -0.7 --T2 1000 --V2 -1.5 
         to output analog signals to CH1 (-0.7V 300ns) and CH2 (-1.5V 1000ns) connected to usb:1.9.5.
ex: ad_out -d usb:1.9.5 --T1 300
         to output digital signals to D0:D15 (300ns) connected to usb:1.9.5.
ex: ad_out -s
         to get the serial number of available adalm2000.
ex: ad_out -s  -u usb:1.9.5
         to get the serial number adalm2000 connected to usb:1.9.5.


```
## ADALM SCAN
```
getusbport.py : USB port scan for ADALM2000 devices   
getusbport.py [S/N]

ex ./getusbport.py 
    to list the ADALM2000 devices  
ex ./getusbport.py 10447384b904001612002500df1edb619310447384b904001612002500df1edb6193
    to know the URI for 10447384b904001612002500df1edb619310447384b904001612002500df1edb6193
```

# OPTION: ROOT INSTALLATION for Raspberry Pi

ROOT version: 6.18.04 (recommended by S.Higashino: not the latest version but confirmed to work in the Raspberry Pi OS)

Download from the CERN ROOT homepage: https://root.cern/install/all_releases/

```
$ cd Download
$ tar -zxvf root_v6.18.04.source.tar.gz
$ sudo move /opt/root/6_18_04 root-6.18.04
```

Before starting to compile, you have to install some related packages as shown in the ROOT installation guide: https://root.cern/install/dependencies/

```
$ sudo apt install dpkg-dev cmake g++ gcc binutils libx11-dev libxpm-dev \
libxft-dev libxext-dev python libssl-dev
```

and one more package: 

```
$ sudo apt install davix-dev
```

Then, it's time to compile ROOT!

```
$ sudo mkdir build_root
$ cd build_root
$ sudo cmake –DCMAKE_SHARED_LINKER_FLAGS=‘-latomic’ -DPYTHON_EXECUTABLE='/usr/bin/python3' ../root-6.18.04
$ sudo make -j2
$ sudo make install
```

The compile should be done successfully. After the compilation, you have to source thisroot.sh (or thisroot.{fish, csh}):

```
$ source /opt/root/6_18_04/build_root/bin/thisroot.sh
```

or input it in your .bashrc (or something like that).
