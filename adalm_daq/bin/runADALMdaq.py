#!/usr/bin/python3

import os, sys
import subprocess
import numpy
import glob
import time
from datetime import datetime
import argparse
import json

ADALM_DIR = os.environ['ADALM_DIR']+'/'
daq_cmd = ADALM_DIR + '/bin/daq'

parser = argparse.ArgumentParser(description='ADALM DAQ TOOL')
parser.add_argument('-c', '--config', default='default_config',
                    help='Config json filename with path')

args = parser.parse_args()
if args.config == 'default_config':
        config_filename = ADALM_DIR + 'default_config.json'
else:
        config_filename = args.config

if os.path.exists(config_filename):
        pass
else:
        print('config file ' + config_filename + ' is not exist...')
        exit()

print('Use config file: ' + config_filename)

with open(config_filename) as f:
        config = json.load(f)
        if config['data_dir'][-1] != '/':
              config['data_dir'] = config['path'] + '/'

data_dir = config['data_dir']

# find the latest period
per_num = 1
per_dir = data_dir # temp
while True:
        per_dir = data_dir + '/per{:0=4}'.format( per_num )
        if os.path.exists( per_dir ):
                per_num += 1
                continue
        
        print( 'create data store directory: ' + per_dir )
        subprocess.run( ['mkdir', '-p', per_dir ] )

        print( 'copy config file to the data store directory (rename to per{:0=4}.xml'.format( per_num ) )
        subprocess.run( ['cp', config_filename, per_dir + '/per{:0=4}.xml'.format( per_num ) ] )
        break

              
##########################################

## User Edit ## DAQ CONFIGURATION ##

sub_entries     = config['sub_entries']     # the number of entries per file
sampling_rate   = config['sampling_rate']   # max 100000000 Hz ( 10 ns per clock )
sampling_depth  = config['sampling_depth']  # default: 1024
dynamic_range   = config['dynamic_range']   # 0 ( +/- 25V ) or 1 ( +/-2.5V )
trigger_Vth_ch1 = config['trigger_Vth_ch1'] # Unit: [V], both polarity is allowed
trigger_Vth_ch2 = config['trigger_Vth_ch2'] # Unit: [V], both polarity is allowed
trigger_src     = config['trigger_src']     # 0 ( ch1 ) or 1 ( ch2 ) or 2 ( ch1 || ch2 )
trigger_type    = config['trigger_type']    # 0 ( rise edge ) or 1 ( fall edge )

print( '' )
print( '=============================================================================' )
print( '                              Kobe DM ADALM DAQ                              ' )
print( '=============================================================================' )
print( '' )

print( 'Directory for data storage: ' + data_dir )
print( 'Run period: per{:0=4}'.format( per_num ) )
print( 'Run type: ' + config['file_header']  )

print( '' )
print( '' )

print( '***  DAQ START  (' + config['file_header'] + ' Run) ***' )

print( '' )
print( '' )

def run():
	cmd = [ daq_cmd
	,per_dir + '/' + config['file_header']
	,str(sub_entries)
	,str(sampling_rate)
	,str(sampling_depth)
	,str(dynamic_range)
	,str(trigger_Vth_ch1)
	,str(trigger_Vth_ch2)
	,str(trigger_src)
	,str(trigger_type)
	]

	# print_cmd(cmd)
	subprocess.run(cmd)


def terminate_run():
	now = datetime.now()
	print('\nRUN END   : '+str(datetime.timestamp(now)))
	with open( per_dir + '/' + config['file_header'] + '.cnf', 'a' ) as of:
		of.write('RUN END   : '+str(datetime.timestamp(now)))


def print_cmd(cmd):
	for c in cmd:
		print(c, end=' ')
		print()

def auto_run():
	while(True):
		try:
			run()
			time.sleep(1)
		except KeyboardInterrupt:
			terminate_run()
			exit(0)

if __name__ == '__main__':
	auto_run()



