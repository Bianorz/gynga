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

    while True:
        bus.write_i2c_block_data(address, 0b0, [0b1, 0b10, 0b11])
        time.sleep(1)

        # TASK 1: test with list compreension
        dataRec1 = [bus.read_byte(address) for _ in range(4)]
        # TASK 2: test with read block data
        dataRec2 = read_i2c_block_data(address, 0, 4)

        n_float1 = ints_to_float(dataRec1)
        n_float2 = ints_to_float(dataRec2)

        print("%.2f" % n_float1)
        print("%.2f" % n_float2)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print 'Interrupted'
        gpio.cleanup()
        sys.exit(0)
