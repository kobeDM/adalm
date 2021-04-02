# RECOMMEND COMMAND
```
# Oscilloscope
$ python3 osci_2ch.py [sampling rate(Hz)] [sampling number] [+/-dynamic range 0(25V) or 1(2.5V)] [1ch Vth(V)] [2ch Vth(V)] [trigger source 0(ch1) 1(ch2) 2(or) 3(and)] [trigger edge 0(rise) 1(fall)]

# example command
$ python3 osci_2ch.py 100000000 1024 5 0.1 0.1 2 0
	
# Create GIF
# Commenting out in osci_2ch.py to create PNG
# This Line : #plt.savefig('./wavepng/Ev'+str(ev)+".png") # for create gif
$ python3 osci_2ch.py [sampling rate(Hz)] [sampling number] [+/-dynamic range 0(25V) or 1(2.5V)] [1ch Vth(V)] [2ch Vth(V)] [trigger source 0(ch1) 1(ch2) 2(or) 3(and)] [trigger edge 0(rise) 1(fall)]
$ python3 create_gif.py
```
