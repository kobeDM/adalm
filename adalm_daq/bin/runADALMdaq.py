#!/usr/bin/python3

import os, sys
import subprocess
import numpy
import glob
import time
from datetime import datetime


if(len(sys.argv)!=2):
	print('Usage : ./runADALMdaq.py [run name]')
	exit(0)

data_header = sys.argv[1]
print(data_header+' run START ---->')


## User Edit ## DATA DIRECTORY LOCATION ##

data_dir = '/home/pi/adalm2000/data/test_data/data/' # with the last slash
sub_dir = '20201005'

##########################################



def run():

	## User Edit ## DAQ CONFIGURATION ##

	subentries = 1000  # per 1 subrun entries
	sampling_rate = 100000000.0 # max 100000000 (Hz) 1colck=10ns
	sampling_num = 1024 #
	dynamic_range = 1 # 0(+/-25V) or 1(+/-2.5V)
	trigger_Vth_1ch = -0.01 # V
	trigger_Vth_2ch = -0.01 # V
	trigger_src = 2 # 'ch1=0' or 'ch2=1' or 'or-trig=2'
	trigger_type = 1 # 'rise=0' or 'fall=1'

	daq_cmd = '/home/pi/adalm2000/adalm_daq/bin/daq' # daq cmd full path
	
	####################################



	subprocess.run(['mkdir', '-p', data_dir+sub_dir ])

	subrun_name = find_newrun(data_dir+sub_dir+'/')

	cmd = [daq_cmd
	,data_dir+sub_dir + '/' + subrun_name + '/' + data_header
	,str(subentries)
	,str(sampling_rate)
	,str(sampling_num)
	,str(dynamic_range)
	,str(trigger_Vth_1ch)
	,str(trigger_Vth_2ch)
	,str(trigger_src)
	,str(trigger_type)
	]

	#print_cmd(cmd)
	subprocess.run(cmd)


def terminate_run(dir_name):
	now = datetime.now()
	print('\nRUN END   : '+str(datetime.timestamp(now)))
	dirs = glob.glob(dir_name+"per????")
	dirs.sort(reverse=True)
	subrun_dir = dir_name+'/per'+str(int(len(dirs)-1)).zfill(4)
	with open(subrun_dir+'/'+data_header+'.cnf','a') as of:
		of.write('RUN END   : '+str(datetime.timestamp(now)))


def print_cmd(cmd):
	for c in cmd:
		print(c, end=' ')
		print()


def find_newrun(dir_name):
	#dirs = os.listdir(dir_name)
	dirs = glob.glob(dir_name+"per????")
	if len(dirs) == 0:
		subrun_dir = dir_name+'/per'+'0'.zfill(4)
		subprocess.run(['mkdir','-p',subrun_dir])
		print('per'+'0'.zfill(4) + '/' + data_header)
		return 'per'+'0'.zfill(4)
	else:
		dirs.sort(reverse=True)
		subrun_dir = dir_name+'/per'+str(int(len(dirs))).zfill(4)
		subprocess.run(['mkdir','-p',dir_name+'/per'+str(int(len(dirs))).zfill(4)])
		#return data_header+str(int(files[0][num_pos+3:num_pos+3+4])+1)
		return "per" + str(int(len(dirs))).zfill(4)


def auto_run():
	while(True):
		try:
			run()
			time.sleep(1)
		except KeyboardInterrupt:
			terminate_run(data_dir+sub_dir+'/')
			exit(0)

if __name__ == '__main__':
	auto_run()



