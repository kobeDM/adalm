#!/usr/bin/python3

import os, sys
import subprocess
import json

SOFT_DIR = os.environ['ADSW']

with open('../../cfg/config.json', 'r') as f:
    j = json.load(f)
SUB_DIR = j['sub_dir']
if(len(sys.argv)!=3):
    print('Usage : ./analyzer.py [initial run] [final run]')
    exit(0)

DIR_NAME = 'run' + sys.argv[1] + '-' + sys.argv[2]
OUT_DIR = SOFT_DIR+'/ana/result/'+SUB_DIR+'/'+DIR_NAME

def make_dir():
    subprocess.run(['mkdir', '-p', OUT_DIR])

def make_list():
    with open(OUT_DIR+'/chain.txt', "w") as f:
        for i in range(int(sys.argv[1]), int(sys.argv[2])):
            f.write(SOFT_DIR+'/ana/result/'+SUB_DIR+'/run'+str(i).zfill(4)+'/rawSub.root\n')

def run_chain():
    print('>>> execute chain.cpp')
    macro_path = SOFT_DIR+'/ana/rootMacro/chain.cpp'
    root_cmd = macro_path + '("' + OUT_DIR + '")'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_eventSelect():
    print('>>> execute eventSelect.cpp')
    macro_path = SOFT_DIR+'/ana/rootMacro/eventSelect.cpp'
    root_cmd = macro_path + '("' + OUT_DIR + '")'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)

def run_drawResult():
    print('>>> execute drawResult.cpp')
    macro_path = SOFT_DIR+'/ana/rootMacro/drawResult.cpp'
    root_cmd = macro_path + '("' + OUT_DIR + '")'
    cmd = ['root', '-q', '-l', '-n', root_cmd]
    subprocess.run(cmd)
 
def run():
    print(DIR_NAME+' analyze START ---->')
    make_dir()
    make_list()
    run_chain()
    run_eventSelect()
    run_drawResult()
    print(DIR_NAME+' ----> analyze COMPLETE')

if __name__ == '__main__':
    run()
