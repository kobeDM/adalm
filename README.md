# ADALM DAQ INSTALATION
read this README.md or adalm_daq/README.md (old README.md)

Installation flow : libiio and libini -> libm2k -> adalm daq 
## basic
```
$sudo apt-get install git cmake swig g++ python3-dev python3-setuptools
```

## libiio and libini 
You have to install libiio and libini before libmk2 installation
see! URL : https://wiki.analog.com/resources/tools-software/linux-software/libiio

```
# libini
$ cd ~/
$ mkdir tmp; cd tmp
$ sudo apt-get install libaio-dev libusb-1.0-0-dev libserialport-dev libxml2-dev libavahi-client-dev doxygen graphviz
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

## LOCAL INSTALATON (install path specific, not usually, optional)

```
$ cd ~
$ mkdir local 
$ cd ~/local
$ git clone https://github.com/analogdevicesinc/libm2k.git
$ mkdir libm2k-build
$ cd libm2k-build
$ cmake --prefix=../libm2k-install -DENABLE_EXCEPTIONS=TRUE ../
$ make
$ make install
```

## ADALM DAQ TOOL
```
$ git clone https://github.com/kobeDM/adalm.git
$ cd adalm
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
```
