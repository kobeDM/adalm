#!/usr/bin/python3

import os
import sys
import subprocess
import json
import argparse
import shutil

def parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('period', nargs='?', type=int, help='Analyzed period number', default=-1)
    parser.add_argument('-a', '--all', action='store_true', help='Merge and analyze all datafiles', default=-1)
    parser.add_argument('--range', nargs=2, type=int, help='--range [first period] [last period]', default=[-1, -1])
    
    return parser.parse_args()
    

# Path
ADSW = os.environ['ADSW']
CWD = os.getcwd()
DATA_DIR = CWD.replace('ana', 'data')

DEFAULT_SUBRUN_NO  = '0'

# import config file
with open(ADSW + '/config/config.json', 'r') as file:
    json_file = json.load(file)


def run_merge(output_filename):
    print('Merge datafiles.')
    cmd = ADSW+'/bin/merge.sh ' + output_filename
    subprocess.run(cmd, shell=True)

def make_dir(out_dir):
    print('Create directory: ' + out_dir)
    subprocess.run(['mkdir', '-p', out_dir])

def move_file(_from, _to):
    shutil.move(_from, _to, check=True)

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

    # Check arguments
    if args.range != [-1, -1] and args.period != -1:
        print("Error: period を指定せず、--range のみを使用してください。")
        sys.exit(1)

    if args.range == [-1, -1] and args.period == -1 and not args.all:
        print("Error: period か --range のどちらかを指定してください。")
        sys.exit(1)

    per_name = None
    subrun_name = 'out_' + DEFAULT_SUBRUN_NO + '.dat'
    data_path = None
    out_dir = None

    # --- range mode ---
    if args.range != [-1, -1]:
        init_per = args.range[0]
        last_per = args.range[1]

        out_dir = f"{CWD}/per{init_per:04d}-{last_per:04d}"
        make_dir(out_dir)

        chain_dir_path = f"{DATA_DIR}/per{init_per:04d}-{last_per:04d}"
        make_dir(chain_dir_path)

        for per in range(init_per, last_per + 1):
            per_name = f"per{per:04d}"
            per_dir = f"{DATA_DIR}/{per_name}"

            os.chdir(per_dir)
            merge_file_name = f"out_per{per:04d}.dat"
            run_merge(merge_file_name)

            shutil.move(f"{per_dir}/{merge_file_name}", 
                        f"{chain_dir_path}/{merge_file_name}")

        os.chdir(chain_dir_path)
        run_merge("merge.dat")
        data_path = f"{chain_dir_path}/merge.dat"

    # --- single period mode ---
    elif args.period != -1:
        per_name = f"per{args.period:04d}"
        out_dir = f"{CWD}/{per_name}"
        make_dir(out_dir)

        if args.all:
            os.chdir(f"{DATA_DIR}/{per_name}")
            run_merge("merge.dat")
            data_path = f"{DATA_DIR}/{per_name}/merge.dat"
        else:
            data_path = f"{DATA_DIR}/{per_name}/{subrun_name}"

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