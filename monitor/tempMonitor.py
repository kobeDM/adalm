import libm2k as l
import time
import sys
import matplotlib.pyplot as plt
from datetime import datetime
import numpy as np

if len(sys.argv) != 2:
        print('ex) python3 ADALM_voltmeter1.py filename')
        exit(0)
        
sleep = 1

## create output file
fheader = sys.argv[1]
ofheader = fheader + '.dat'
with open(ofheader,'w') as f:
        print('#'+str(datetime.now())+'\n#sleeptime= '+str(sleep),file=f)

ctx=l.m2kOpen()
if ctx is None:
	print("Connection Error: No ADALM2000 device available/connected to your PC.")
	exit(1)

ctx.calibrateADC()

ain=ctx.getAnalogIn()
aout=ctx.getAnalogOut()
ps=ctx.getPowerSupply()

ain.setRange(0,0) #1=+-25V 0=+-2.5V
ain.setRange(1,0) #1=+-25V 0=+-2.5V
ain.enableChannel(0,True)
ain.enableChannel(1,True)

## enable voltage output
ps.enableChannel(0,True)
#ps.pushChannel(0,1.3)


##temperature
tempMAX = 50
tempMIN = 30
tempV0_MAX = 0.0107*tempMAX+2.6868   #V0
tempV0_MIN = 0.0107*tempMIN+2.6868   #V0
tempV1_MAX = 0.00925*tempMAX+2.7805  #V1
tempV1_MIN = 0.00925*tempMIN+2.7805  #V1

def VtoTemp0(V):
    return (V-2.6868)/0.0107 # Temperature0

def VtoTemp1(V):
    return (V-2.7805)/0.00925 # Temperature1


fig = plt.figure()
count = 0
signal = 0
print('start time : ' +str(datetime.now()),end='')
print('\nsleeptime= '+str(sleep))
print('\nStart Oscilloscope ...')

data0_list =[]
data1_list =[]

while True:
        data = ain.getVoltage()
        odata0 = data[0]
        odata1 = data[1]
        dt_now = datetime.now()

        temp0 = VtoTemp0(odata0)
        temp1 = VtoTemp1(odata1)
        
        data0_list.append(temp0)
        data1_list.append(temp1)

        

        if temp0<0:
                temp0=0
        if temp1<0:
                temp1=0

        if count%10 == 0 :
            val0 = np.mean(data0_list[-10:])
            val1 = np.mean(data1_list[-10:])

            print(val0)
            print(val1)
                        
        ##outV ON/OFF
        # signal = 0 -> OFF
        # signal = 1 -> ON

        if temp0 >= tempMAX or temp1 >= tempMAX:
                signal = 1
        #if temp1 >= tempMAX:
                #signal = 1
        #print('power ' + str(signal))
                

        if signal == 1:        
                ps.pushChannel(0,2.4)
        if signal == 0:
                ps.pushChannel(0,0)
                
        if temp0 <= tempMIN:
                 signal = 0
        #if temp1 <= tempMIN:
                 #signal = 0
        
        #####oscilloscope######
        if count % 10 == 0 and count != 0:
                temp0 = VtoTemp0(odata0)
                temp1 = VtoTemp1(odata1)
                val0 = np.mean(data0_list[-10:])
                val1 = np.mean(data1_list[-10:])
                
                #fig.clear()
                p0 = plt.scatter(count,val0,s=10,c='red',label=None)
                p1 = plt.scatter(count,val1,s=10,c='blue',label=None)
                #plt.plot(odata0,label='CH1',c='red')
                #plt.plot(odata1,label='CH2',c='blue')
                #plt.xlabel('1clock='+str(1./sampling_rate*1e6)+'us')
                plt.ylabel('Temperature (C)')
                #plt.xlim(0,20)
                plt.ylim(10, 60)
                plt.grid()
                plt.legend([p0,p1],['CH1','Ch2'])
                plt.draw()
                plt.pause(0.2)
                plt.savefig(fheader + '.png')
                data0_list = []
                data1_list = []
        count = count + 1
        #######################
        
        ## save data
        temp0 = VtoTemp0(odata0)
        temp1 = VtoTemp1(odata1)
        
        with open(ofheader,'a') as f:
                #if signal == 1:
                        #print('#'+str(datetime.now()),file=f)
                print('#'+str(datetime.now()),temp0,temp1,signal,file=f)
        print('CH1=',temp0,'  CH2=',temp1,odata0,odata1,count,signal)
        
        time.sleep(sleep)

l.contextClose(ctx)
exit(0)
