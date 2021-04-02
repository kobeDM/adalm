# INSTALATION
```
$sudo apt-get install git cmake swig g++ python3-dev python3-setuptools
```
You have to install libiio
URL : https://wiki.analog.com/resources/tools-software/linux-software/libiio

```
$ git clone https://github.com/analogdevicesinc/libm2k.git
$ mkdir libm2k-build
$ cd libm2k-build
$ cmake -DENABLE_EXCEPTIONS=TRUE ../libm2k
$ make
$ sudo make install
```

LOCAL INSTALATON

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

# DAQ
```
$ cd ${adalm_daq}
$ mkdir build
$ cd build
$ cmake ../source
$ make install
$ ls ../bin
daq daq_test

# FOR TEST
$ cd ${adalm_daq}
$ mkdir bench
$ ../bin/daq_test out 100 1000000 1024 2 0.1 0.1 2 0
$ ls
out.dat out.cnf
```
