# dbh743
Porting db48x on a stm32h743 WeAct board

This is a quick and dirty implentation of Db48x https://github.com/c3d/db48x to a WeAct stm32h743 board, a Adafruit Sharp display and a home made keyboard with Omron B3F-4050.

Keyboard action seems (comparing with a DM42) stritly identical, but i'm unable to run the tests. The LCD display works as expected.

Only three files are modified in /src : 
  1. file.cc and file.h adapted to Segger emFile
  2. locals.cc line 39 #include <strings.h> need to be commented.
  3. tests.cc is not used at all

version update from 0.9.8 to 0.9.9 then to 0.9.10 without issues.

==> Thanks to c3d !

A sd card is used, with a msd usb driver. The usb is available when db48x is running.
The /help /config /library and /state are available. Files *.csv are loaded.

For the time beiing  State1.48s is used by default. 

The memory is not permanent, save state with second shifted exit key.

To reboot the calculator press F1 + F6 + Exit, State1 is loaded.
You can modify the files when working with the calculator.

A rndis ethernet over usb is still a work in progress.

The release version is less than 1mb, the debug one is 1.5mb (2mb on stm32h743 and stm32u585, 4mb on smt32u595)

Memory is not optimized at all, 256k of sram available for db48x only, but can be improoved !

The keyboard is a task sending through a mailbox key events to the db48x task dbu585_main_new.

The c3d program is modified as little as possible. No sleeping mode, no power off.

<img width="400" height="240" alt="About" src="https://github.com/user-attachments/assets/4bf281d2-fe7f-4946-b280-2134a76e7b8a" />

<img width="400" height="240" alt="scr1" src="https://github.com/user-attachments/assets/4eed6c49-3a62-4f2e-8f53-22f7e53e6d5b" />


Using Segger Tools under the Friendly license, https://www.segger.com/purchase/licensing/license-sfl/

Compiled with Segger Embedded Studio v8.24

Using embOs, emFile, emUsb and emNet
Screenshots with pylink, j-link interface for python

I have another version, using a WeAct stm32u585 board. But the FROM device is wired in spi and not in qspi, working reliably only at a few Mhz, the file system is then very, very slow.

 https://github.com/WeActStudio/MiniSTM32H7xx
 
To do : 
1. Find a way to do the tests, via usb (serial, ethernet) or j-link ?
2. Add a (some) system variable to choose the state at power on, 
3. Remove the DMCP, key mapping, menu system (related of point 2) 
4. Switch to embOs Ultra and work on the sleeping states. Wake by interrupt on EXTI
