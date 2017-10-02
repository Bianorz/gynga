# Gynga Energia - Internship Tasks

## Program an AVR or Arduino Using Raspberry Pi GPIO
- The Raspberry Pi can be used as an ISP programmer, the steps to do that can be found [here](https://github.com/Bianorz/gynga/blob/master/tarefas/prog_rasp_avr_isp.md). <br />
- Follow [these steps](https://github.com/Bianorz/gynga/blob/master/tarefas/comp_c_hex.md) to generate a hex file from a C file and then program the microcontroller. The tools used will be **avr-gcc** to compile the C code and **avrdude** to program the chip. 
## I2C Protocol
- The I2C is a very popular and powerful protocol used for communication between a master(or multiple masters) and a single or multiple slave devices.
- To communicate two arduinos, you can use [this code](https://github.com/Bianorz/gynga/blob/master/codes/arduinoMaster.ino) for the master and [this one](https://github.com/Bianorz/gynga/blob/master/codes/arduinoSlave.ino) for the slave device.
- An example of how to implement this communication using a master Raspberry and a slave Arduino can be found [here](https://github.com/Bianorz/gynga/blob/master/tarefas/comun_i2c_rasp_ard.md). <br />
- The code of a slave AtMega328P device using pure C can be found [here](https://github.com/Bianorz/gynga/blob/master/codes/slaveAtMega.c) while the code of master Raspberry using python can be found [here](https://github.com/Bianorz/gynga/blob/master/codes/masterPython.py). <br />
## Bootloader Programming
- A bootloader hex file can be found [here](https://github.com/Bianorz/gynga/blob/master/codes/optiboot.hex), the file is from the [Optiboot Project](https://github.com/Optiboot/optiboot). The steps used to program it in an AtMega328P microcontroller are described [here](https://github.com/Bianorz/gynga/blob/master/tarefas/install_bootloader.md). <br />
## ISP Programming
- In-System Programming allows programming and reprogramming of any AVR microcontroller positioned inside the end system.
- Six pins are used in this mode: **MISO,MOSI,SCK,RESET,VCC,GND**
  - MISO stands for Master In Slave Out. MISO is the input pin for Master AVR, and output pin for Slave AVR device. Data transfer from Slave to Master takes place through this channel.
  - MOSI stands for Master Out Slave In. This pin is the output pin for Master and input pin for Slave. Data transfer from Master to Slave takes place through this channel.
  - SCK This is the SPI clock line (since SPI is a synchronous communication).
- The chip is ready to enter **Programming Mode** only when reset goes active, after this you should wait at least 20 seconds to send the first command.
- All commands have **4 bytes**
   - 1º byte: Command code, selecting operation, target memory
   - 2º,3º byte: Address of the selected memory area
   - 4º byte: Data
   - ![alt text](https://raw.githubusercontent.com/Bianorz/gynga/master/tarefas/ex_enb_mem_acs.png "")
- Depending on the target device the Flash is programmed using “Byte” or “Page” mode. A device will only have one of these modes
available.
   - Byte: Each Flash location is dressed and programmed individually
   - Page: A temporary Page buffer is first filled, and then programmed in one single write cycle. This mode reduces the total Flash programming time and **it's the mode used in AtMega328P**
