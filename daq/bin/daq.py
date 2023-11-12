#!/usr/bin/python3

import os, sys
import subprocess
import json
import glob
import time
from datetime import datetime

with open('../../cfg/config.json', 'r') as f:
    j = json.load(f)

if(len(sys.argv)!=1):
    print('Usage : ./daq.py')
    exit(0)

print(' run START ---->')

SOFT_PATH = os.environ['ADSW']
print('SOFT_PATH : '+SOFT_PATH)
data_dir = SOFT_PATH+'/daq/data'
# sub_dir = j['sub_dir']
sub_dir = datetime.now().strftime("%Y%m%d")


def run():

    subentries = j['daq']['subentries']  # per 1 subrun entries
    sampling_rate = j['daq']['sampling_rate'] # max 100000000 (Hz) 1colck=10ns
    sampling_num = j['daq']['sampling_num'] #
    dynamic_range = j['daq']['dynamic_range'] # 0(+/-25V) or 1(+/-2.5V)
    trigger_Vth_1ch = j['daq']['trigger_Vth_1ch'] # V
    trigger_Vth_2ch = j['daq']['trigger_Vth_2ch'] # V
    trigger_src = j['daq']['trigger_src'] # 'ch1=0' or 'ch2=1' or 'or-trig=2'
    trigger_type = j['daq']['trigger_type'] # 'rise=0' or 'fall=1'
    uri = 'usb:'+j['daq']['uri'] # adalm uri name

    daq_cmd = SOFT_PATH+'/daq/bin/daq' # daq cmd full path

    subprocess.run(['mkdir', '-p', data_dir+'/'+sub_dir ])

    subrun_name = find_newrun(data_dir+"/"+sub_dir+'/')

    cmd = [daq_cmd
    ,data_dir+'/'+sub_dir + '/' + subrun_name + '/out'
    ,str(subentries)
    ,str(sampling_rate)
    ,str(sampling_num)
    ,str(dynamic_range)
    ,str(trigger_Vth_1ch)
    ,str(trigger_Vth_2ch)
    ,str(trigger_src)
    ,str(trigger_type)
    ,uri
    ]

    try:
        subprocess.run(cmd)
    except subprocess.CalledProcessError as e:
        print(e)
        delete_newrun(data_dir+"/"+sub_dir+'/')


def terminate_run(dir_name):
    now = datetime.now()
    print('\nRUN END   : '+str(datetime.timestamp(now)))
    dirs = glob.glob(dir_name+"run????")
    dirs.sort(reverse=True)
    subrun_dir = dir_name+'/run'+str(int(len(dirs)-1)).zfill(4)
    with open(subrun_dir+'/out.cnf','a') as of:
        of.write('RUN END   : '+str(datetime.timestamp(now)))

def find_newrun(dir_name):
    dirs = glob.glob(dir_name+"/run????")
    if len(dirs) == 0:
        subrun_dir = dir_name+'/run'+'0'.zfill(4)
        subprocess.run(['mkdir','-p',subrun_dir])
        print('run'+'0'.zfill(4) + '/out')
        return 'run'+'0'.zfill(4)
    else:
        dirs.sort(reverse=True)
        subrun_dir = dir_name+'/run'+str(int(len(dirs))).zfill(4)
        subprocess.run(['mkdir','-p',dir_name+'/run'+str(int(len(dirs))).zfill(4)])
        print('run'+str(int(len(dirs))).zfill(4) + '/out')
        return "run" + str(int(len(dirs))).zfill(4)

def delete_newrun(dir_name):
    dirs = glob.glob(dir_name+"run????")
    if len(dirs) == 0:
        return
    latest_run = dirs[-1]
    subprocess.run(['rm', '-r', dir_name+'/'+latest_run])
    print('deleted', latest_run)

def auto_run():
    while(True):
        try:
            time.sleep(1)
            run()
        except KeyboardInterrupt:
            terminate_run(data_dir+'/'+sub_dir+'/')
            exit(0)

if __name__ == '__main__':
    auto_run()

