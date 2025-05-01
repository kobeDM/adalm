#!/usr/bin/python3

import os
import subprocess
import json
import argparse

def parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('period', type=str, help='Analyzed period number')
    parser.add_argument('-a', '--all', action='store_true', help='Merge and analyze all datafiles')
    
    return parser.parse_args()
    

# Path
ADSW = os.environ['ADSW']
CWD = os.getcwd()
DATA_DIR = CWD.replace('ana', 'data')

DEFAULT_SUBRUN_NO  = '0'

# import config file
with open(ADSW + '/config/config.json', 'r') as file:
    json_file = json.load(file)


def run_merge():
    print('Merge datafiles.')
    macro_path = ADSW+'/bin/merge.sh'
    subprocess.run(macro_path, shell=True)

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
    root_cmd = macro_path + '("' + out_dir + '")'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_fitSpectrum(out_dir):
    if (json_file['ana']['fit']):
        print('>>> execute fitSpectrum')
        macro_path = ADSW+'/rootmacros//fitSpectrum.cxx'
        root_cmd = macro_path + '("' + out_dir + '")'
        cmd = ['root', '-q', '-l', '-n', root_cmd]
        subprocess.run(cmd)


def run():
    args = parser()
    
    per_name = 'per' + args.period.zfill(4)
    subrun_name = 'out_' + DEFAULT_SUBRUN_NO + '.dat'
    data_path   = DATA_DIR + '/' + per_name + '/' + subrun_name
    out_dir     = CWD + '/' + per_name

    make_dir(out_dir)
    
    
    if (args.all):
        os.chdir(DATA_DIR + '/' + per_name)
        run_merge()
        os.chdir(CWD)
        data_path = DATA_DIR + '/' + per_name + '/merge.dat'
    
    print('----------------------------------------------')
    run_makeTree(data_path, out_dir)
    print('----------------------------------------------')
    run_eventSelect(out_dir)
    print('----------------------------------------------')
    run_drawResult(out_dir)
    print('----------------------------------------------')
    run_fitSpectrum(out_dir)
        

if __name__ == "__main__":
    print('--- ANALYZER START ---')
    run()
    print('--- ANALYZER END ---')