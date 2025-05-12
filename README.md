# adalm
Software for measurement and analysis using ADALM 2000

### Installation
1. Follow the instructions in the main branch

### build
    ```
    $ cd ~/adalm
    $ mkdir build_daq build_out
    $ cd build_daq
    $ cmake ../source/daq && make install
    $ cd ../build_out
    $ cmake ../source/out && make install
    ```

### Set up
    ```
    $ cd ~/adalm
    $ source setup.sh // Add environment variables
    ```

### DAQ
    ```
    $ cd /path/to/data/directory/data/YYYYMMDD
    $ daq.py
    ```

### Analysis
    ```
    $ cd /path/to/data/directory/ana/YYYYMMDD
    $ analyze.py [period No.]
    ```
