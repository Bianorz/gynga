import RPi.GPIO as gpio
import smbus
import time
import sys
import struct
 
bus = smbus.SMBus(1)
address = 0x05

def ints_to_float(numbers):
    """
    Used to convert a 32bit in form of 4 integes < 256 to float.
    >>> ints_to_float([63, 128, 0, 0])
    1.0
    """
    return struct.unpack('<f', struct.pack('4B', *numbers))[0]  

def main():
    gpio.setmode(gpio.BCM)
    gpio.setup(17, gpio.OUT)
    status = False
    dataRec = [0,0,0,0]
    while 1:
        gpio.output(17, status)
	bus.write_i2c_block_data(address, 0b0 , [0b1, 0b10,0b11])
	time.sleep(1)
	for i in range(0,4):
	    dataRec[i] = bus.read_byte(address)
	n_float = ints_to_float(dataRec)
	print("%.2f" % n_float)
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print 'Interrupted'
        gpio.cleanup()
        sys.exit(0)
