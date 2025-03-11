#!/usr/bin/python3

import os
import subprocess

# environment variable
ADSW = os.environ['ADSW']
print('ADALM tool directory path: ' + ADSW)

def run_fitSpectrum():
    print('>>> execute fitSpectrum')
    macro_path = ADSW+'/rootmacros/fitSpectrum.cxx'
    root_cmd = macro_path + '(\".\", 0)'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def main():
    print('--- fitSpectrum.py start ---')
    run_fitSpectrum()
    print('--- fitSpectrum.py end ---')

if __name__ == '__main__':
    main()
    
