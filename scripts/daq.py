#!/usr/bin/python3

import os, sys
import subprocess
import json
import glob

# environment variable
ADSW = os.environ['ADSW']
print('ADALM tool directory path: ' + ADSW)

# argument check
if(len(sys.argv)!=1):
    print('Usage : daq.py')
    exit(0)

def fetch_config(dir):
    config_path = ADSW + '/config/config.json'
    cmd = ['cp', config_path, dir]
    # print(cmd)
    subprocess.run(cmd)

def find_newrun(dir_name):
    dirs = glob.glob(dir_name + "/per????")
    if len(dirs) == 0: # for first period
        subrun_dir = dir_name + '/per' + '0'.zfill(4)
        subprocess.run(['mkdir', '-p', subrun_dir])
        return 'per'+'0'.zfill(4)
    else:
        dirs.sort(reverse=True)
        subrun_dir = dir_name + '/per' + str(int(len(dirs))).zfill(4)
        subprocess.run(['mkdir','-p',dir_name + '/per' + str(int(len(dirs))).zfill(4)])
        return "per" + str(int(len(dirs))).zfill(4)

def run():
    per_name = find_newrun('.')

    fetch_config(per_name)

    with open(ADSW + '/config/config.json', 'r') as file:
        json_file = json.load(file)

    subentries = json_file['daq']['subentries']  # per 1 subrun entries
    sampling_rate = json_file['daq']['sampling_rate'] # max 100000000 (Hz) 1colck=10ns
    sampling_num = json_file['daq']['sampling_num'] #
    dynamic_range = json_file['daq']['dynamic_range'] # 0(+/-25V) or 1(+/-2.5V)
    trigger_Vth_1ch = json_file['daq']['trigger_Vth_1ch'] # V
    trigger_Vth_2ch = json_file['daq']['trigger_Vth_2ch'] # V
    trigger_src = json_file['daq']['trigger_src'] # 'ch1=0' or 'ch2=1' or 'or-trig=2'
    trigger_type = json_file['daq']['trigger_type'] # 'rise=0' or 'fall=1'
    uri = 'usb:'+json_file['daq']['uri'] # adalm uri name

    daq_cmd = ADSW+'/bin/daq' # daq cmd full path

    cmd = [
        daq_cmd,
        per_name + '/out',
        str(subentries),
        str(sampling_rate),
        str(sampling_num),
        str(dynamic_range),
        str(trigger_Vth_1ch),
        str(trigger_Vth_2ch),
        str(trigger_src),
        str(trigger_type),
        uri
    ]

    # print(cmd)
    subprocess.run(cmd)


if __name__ == '__main__':
    print('--- DAQ START ---')
    
    try:
        run()
    except KeyboardInterrupt:
        print('BREAK: Keyboard Interrupt')
    
    print('--- DAQ END ---')

