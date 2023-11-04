# adalm
Software for measurement and analysis using ADALM 2000
### Installation
1. Follow the instructions in the main branch
2. Enter the directory location in soft_path in config
### DAQ
    ```
    $ cd daq/bin
    $ ./daq.py
    ```
1. Measurement results are stored in the `daq/data/subrun_name/run_name` directory

### Analyze
    ```
    $ cd ana/bin
    $ ./analyze.py <run_name> <datafile_No.>
    ```
1. The analysis results are saved in the `ana/result/subrun_name/run_name` directory