#!/usr/bin/python3

import os
import shutil
import subprocess
import json
import argparse
import shutil

# Path
ADSW = os.environ['ADSW']

# import config file
with open(ADSW + '/config/config.json', 'r') as file:
    json_file = json.load(file)


def arg_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('period', nargs='?', type=int, help='Analyzed period number', default=-1)
    args = parser.parse_args()
    return args

def make_dir(out_dir):
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
        print(f"Directory '{out_dir}' created.")
    else:
        print(f"Directory '{out_dir}' already exists.")

def merge_data_files(data_dir_path, output_file_path):
    if not os.path.exists(data_dir_path):
        print(f"Data directory '{data_dir_path}' does not exist.")
        return
    if os.path.exists(output_file_path):
        print(f"Output file '{output_file_path}' already exists. Skipping merge.")
        return
    cmd = f"cat {data_dir_path}/*.dat > {output_file_path}"
    subprocess.run(cmd, shell=True)

def copy_config_file(file_from, file_to):
    if not os.path.exists(file_from):
        print(f"Config file '{file_from}' does not exist.")
        return
    if os.path.exists(file_to):
        print(f"Config file '{file_to}' already exists. Skipping copy.")
        return
    shutil.copy(file_from, file_to)

def run_makeTree(data_path, out_dir_path):
    print('>>> execute makeTree')
    print(data_path)
    macro_path = f"{ADSW}/rootmacros/makeTree.cxx"
    root_cmd   = f'{macro_path}("{data_path}", "{out_dir_path}")'
    cmd        = f"root -q -l -b '{root_cmd}'"
    subprocess.run(cmd, shell=True)

def run_drawResult(out_dir):
    print('>>> execute drawResult')
    macro_path = f"{ADSW}/rootmacros/drawResult.cxx"
    root_cmd   = f'{macro_path}("{out_dir}")'
    cmd        = f"root -q -l -b '{root_cmd}'"
    subprocess.run(cmd, shell=True)

def run_analyzer():
    args = arg_parser()
    period = args.period

    current_dir = os.getcwd()
    data_dir_path = current_dir.replace("analysis", "data")
    print(f"Current directory: {current_dir}")
    print(f"Data directory   : {data_dir_path}")

    # make analysis directory
    analysis_dir_name = f"per{str(period).zfill(4)}"
    make_dir(analysis_dir_name)

    # copy config file
    config_file_from = f"{ADSW}/config/config.json"
    config_file_to   = f"{analysis_dir_name}/config.json"
    copy_config_file(config_file_from, config_file_to)

    # merge data files
    data_dir_path         = f"{data_dir_path}/{analysis_dir_name}"
    merged_data_file_path = f"{analysis_dir_name}/out_merge.dat"
    merge_data_files(data_dir_path, merged_data_file_path)

    # run makeTree
    out_dir_path = analysis_dir_name
    run_makeTree(merged_data_file_path, out_dir_path)

    # run drawResult
    run_drawResult(out_dir_path)
        

def main():
    run_analyzer()

if __name__ == "__main__":
    print('--- ANALYZER START ---')
    main()
    print('--- ANALYZER END ---')
