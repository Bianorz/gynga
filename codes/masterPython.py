
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
    while 1:
       	bus.write_i2c_block_data(address, 0b1 , [0b11, 0b111,0b1111])
	time.sleep(1)
	dataRec = bus.read_i2c_block_data(address, 0, 4)
	n_float = ints_to_float(dataRec)
	print("%.2f" % n_float)
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print 'Interrupted'
        sys.exit(0)
