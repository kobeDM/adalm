# gainAna
Software for LBGuPIC gain measurement

# DAQ
### build
```
$ cd ${gainAna}/daq
$ mkdir build
$ cd build
$ cmake ../src
$ make install
# binary is created in ${gainAna}/daq/bin
```
### execute
```
$ cd ${gainAna}/daq/bin
$ python3 daq.py [subrun name]
# data is recorded in ${gainAna}/daq/data
```

# analyzer
### execute
```
$ cd ${gainAna}/ana/script
$ python3 analyzer.py
# result is created in ${gainAna}/ana/result
```
# config file
- soft_path : directory where the gainAna is located
- sub_dir   : sub directry name

### daq
- subentries    : per 1 subrun entries
- sampling_rate : max 100000000 (Hz) 1colck=10ns
- sampling_num  : the number of clock / event
- dynamic_range : 0(+/-25V) or 1(+/-2.5V)
- trigger_Vth_1/2ch : V
- trigger_src       : 'ch1=0' or 'ch2=1' or 'or-trig=2'
- trigger_type      : 'rise=0' or 'fall=1'
- uri               : can be checked from getusbport.py

### ana
- 