#!/usr/bin/python3

import os, sys
import subprocess
import json
import glob

SOFT_DIR = os.environ['ADSW']

# import config file
with open('../../cfg/config.json', 'r') as f:
    j = json.load(f)

# arguments
if len(sys.argv) == 3:
    latest_dir = sorted(glob.glob(SOFT_DIR+'/daq/data/*'))[-1]
    SUB_DIR = latest_dir.replace(SOFT_DIR+'/daq/data/', '')
    RUN_NAME = 'run' + sys.argv[1]
    SUBRUN_NO  = sys.argv[2]
elif len(sys.argv) == 4:
    SUB_DIR = sys.argv[1]
    RUN_NAME = 'run' + sys.argv[2]
    SUBRUN_NO  = sys.argv[3]
else:
    print('Usage1 : ./analyzer.py [run No.] [subrun No.]')
    print('Usage2 : ./analyzer.py [subdir name] [run No.] [subrun No.]')
    exit(0)

SUBRUN_NAME = 'out_'+SUBRUN_NO+'.dat'
OUT_DIR = SOFT_DIR+'/ana/result/'+SUB_DIR+'/'+RUN_NAME

def make_dir():
    subprocess.run(['mkdir', '-p', OUT_DIR])

def run_makeTree():
    print('>>> execute makeTree.cpp')
    data_path = SOFT_DIR+'/daq/data/'+SUB_DIR+'/'+RUN_NAME+'/'+SUBRUN_NAME

    macro_path = SOFT_DIR+'/ana/src/makeTree.cpp'
    root_cmd = macro_path + '("' + data_path + '", "' + OUT_DIR +'")'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_eventSelect():
    print('>>> execute eventSelect.cpp')
    macro_path = SOFT_DIR+'/ana/src/eventSelect.cpp'
    root_cmd = macro_path + '("' + OUT_DIR + '")'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_drawResult():
    print('>>> execute drawResult.cpp')
    macro_path = SOFT_DIR+'/ana/src/drawResult.cpp'
    root_cmd = macro_path + '("' + OUT_DIR + '", ' + str(SUBRUN_NO) + ')'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_fitSpectrum():
    if (j['ana']['fit'] is True):
        print('>>> execute fitSpectrum.cpp')
        macro_path = SOFT_DIR+'/ana/src/fitSpectrum.cpp'
        root_cmd = macro_path + '("' + OUT_DIR + '", '  + str(SUBRUN_NO) + ')'
        cmd = ['root', '-q', '-l', '-n', root_cmd]
        subprocess.run(cmd)


def run():
    print(RUN_NAME+'/'+SUBRUN_NAME+' analyze START ---->')
    make_dir()
    run_makeTree()
    run_eventSelect()
    run_drawResult()
    run_fitSpectrum()
    print(RUN_NAME+'/'+SUBRUN_NAME+' ----> analyze COMPLETE')

if __name__ == "__main__":
    run()