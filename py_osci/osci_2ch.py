import libm2k

import time

import sys

import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime


print('python3 osci_2ch.py [sampling rate(Hz)] [sampling number] [+/-dynamic range 0(25V) or 1(2.5V)] [1ch Vth(V)] [2ch Vth(V)] [trigger source 0(ch1) 1(ch2) 2(or) 3(and)] [trigger edge 0(rise) 1(fall)]')

if len(sys.argv) != 8:
	print('python3 osci_2ch.py [sampling rate(Hz)] [sampling number] [+/-dynamic range 0(25V) or 1(2.5V)] [1ch Vth(V)] [2ch Vth(V)] [trigger source 0(ch1) 1(ch2) 2(or) 3(and)] [trigger edge 0(rise) 1(fall)]')
	print('ex) python3 osci_2ch.py 10000000 1024 5 0.1 0.1 2 0')
	exit(0)

sampling_rate = float(sys.argv[1])
sampling_num  = int(sys.argv[2])
dynamic_range = int(sys.argv[3])
Vth1          = float(sys.argv[4])
Vth2          = float(sys.argv[5])
trig_src      = int(sys.argv[6])
trig_edge     = int(sys.argv[7])


ctx=libm2k.m2kOpen()
if ctx is None:
	print("Connection Error: No ADALM2000 device available/connected to your PC.")
	exit(1)

ctx.calibrateADC()
ctx.calibrateDAC()

#Definetion
ain=ctx.getAnalogIn()
aout=ctx.getAnalogOut()
trig=ain.getTrigger()

#Initialization
ain.enableChannel(0,True)
ain.enableChannel(1,True)
ain.setVerticalOffset(0,0)
ain.setVerticalOffset(1,0)
ain.setSampleRate(sampling_rate)
ain.setRange(0,dynamic_range)
ain.setRange(1,dynamic_range)
#ain.setRange(0,-dynamic_range,dynamic_range)
#ain.setRange(1,-dynamic_range,dynamic_range)


### uncomment the following block to enable triggering
trig.setAnalogSource(trig_src)
trig.setAnalogDelay(-int(sampling_num/2))
# CHANNEL 1
trig.setAnalogCondition(0,trig_edge)
trig.setAnalogLevel(0,Vth1)
trig.setAnalogMode(0, libm2k.ANALOG)
# CHANNEL 1
trig.setAnalogCondition(1,trig_edge)
trig.setAnalogLevel(1,Vth2)
trig.setAnalogMode(1, libm2k.ANALOG)

## For test W1 W2 output
#aout.setSampleRate(0, 750000)
#aout.setSampleRate(1, 750000)
#aout.enableChannel(0, True)
#aout.enableChannel(1, True)
#x=np.linspace(-np.pi,np.pi,1024)
#buffer1=np.linspace(-1.,1.,1024)
#buffer2=np.sin(x)
#buffer = [buffer1, buffer2]
#aout.setCyclic(True)
#aout.push(buffer)

ev = 0

fig = plt.figure()

print('Start Oscilloscope ...')
while True:
	Time = time.time()
	try:
		fig.clear()
		ain.startAcquisition(sampling_num)
		data = ain.getSamples(sampling_num)
		now = datetime.now()
		print('\rEv. ' + str(ev)  + '  |  start time : ' + str(datetime.timestamp(now)),end='')
		plt.plot(data[0],label='CH1')
		plt.plot(data[1],label='CH2')
		plt.xlabel('1clock='+str(1./sampling_rate*1e6)+'us' )
		plt.ylabel('Volt(V)')
		plt.xlim(0,sampling_num)
		plt.grid()
		plt.legend()
		plt.draw()
		plt.pause(0.2)
		#plt.savefig('./wavepng/Ev'+str(ev)+".png") # for create gif
		ain.stopAcquisition()
		ev = ev + 1
		time.sleep(0.1)
	except KeyboardInterrupt:
		print('\n')
		libm2k.contextClose(ctx)
		exit(0)



