# adalm
Software for measurement and analysis using ADALM 2000

### Installation
1. Follow the instructions in the main branch

### Set up
    ```
    $ cd ~/adalm
    $ source setup.sh // Add environment variables
    ```

### DAQ
    ```
    $ cd /path/to/data/directry/data/YYYYMMDD
    $ daq.py
    ```

### Analysis
    ```
    $ cd /path/to/data/directry/ana/YYYYMMDD
    $ analyze.py [period No.]
    ```

フィッティングを別マクロで実行させる