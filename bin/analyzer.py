#!/usr/bin/python3

import os, sys
import subprocess
import json

# Path
ADSW = os.environ['ADSW']
CWD = os.getcwd()
DATA_DIR = CWD.replace('ana', 'data')

# import config file
with open(ADSW + '/config/config.json', 'r') as file:
    json_file = json.load(file)

# argument check
if len(sys.argv) == 2:
    PER_NAME = 'per' + sys.argv[1].zfill(4)
else:
    print('Usage1 : analyzer.py [period No.]')
    exit(0)

SUBRUN_NO  = '0'
SUBRUN_NAME = 'out_' + SUBRUN_NO + '.dat'
DATA_PATH = DATA_DIR + '/' + PER_NAME + '/' + SUBRUN_NAME
OUT_DIR = CWD + '/' + PER_NAME

def make_dir(out_dir):
    print('Create directory: ' + out_dir)
    subprocess.run(['mkdir', '-p', out_dir])

def run_makeTree(data_path, out_dir_path):
    print('>>> execute makeTree')
    print(data_path)
    macro_path = ADSW+'/rootmacros/makeTree.cxx'
    root_cmd = macro_path + '("' + data_path + '", "' + out_dir_path +'")'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_eventSelect(out_dir):
    print('>>> execute eventSelect')
    macro_path = ADSW+'/rootmacros/eventSelect.cxx'
    root_cmd = macro_path + '("' + out_dir + '")'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_drawResult(out_dir):
    print('>>> execute drawResult')
    macro_path = ADSW+'/rootmacros/drawResult.cxx'
    root_cmd = macro_path + '("' + out_dir + '", ' + str(SUBRUN_NO) + ')'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_fitSpectrum(out_dir):
    if (json_file['ana']['fit']):
        print('>>> execute fitSpectrum')
        macro_path = ADSW+'/rootmacros//fitSpectrum.cxx'
        root_cmd = macro_path + '("' + out_dir + '", '  + str(SUBRUN_NO) + ')'
        cmd = ['root', '-q', '-l', '-n', root_cmd]
        subprocess.run(cmd)


def run():
    make_dir(OUT_DIR)
    # link_util()
    print('----------------------------------------------')
    run_makeTree(DATA_PATH, OUT_DIR)
    print('----------------------------------------------')
    run_eventSelect(OUT_DIR)
    print('----------------------------------------------')
    run_drawResult(OUT_DIR)
    print('----------------------------------------------')
    run_fitSpectrum(OUT_DIR)
        

if __name__ == "__main__":
    print('--- ANALTZER START ---')

    run()

    print('--- ANALTZER END ---')