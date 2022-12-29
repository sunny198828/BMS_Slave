@echo off
setlocal enableDelayedExpansion
DEL log
DEL eepromData
MD log
MD eepromData
echo COM_PORT	= %1
echo FIRST_SLAVE	= %2
echo LAST_SLAVE	= %3
echo File		= %4

FOR /L %%x IN (%2,1,%3) do (
	
	echo {"ID":%%x,"COMMAND":"bootloader"} > %1
 	ping 127.255.255.255 -n 1 -w 500> nul
	start /wait avrdude.exe -c arduino -p t3227 -P %1 -b 9600 -U flash:w:%4:a -U eeprom:r:"eepromData/SLAVE%%x.eep":i -l log/log_SLAVE%%x.txt 2>&1
	ping 127.255.255.255 -n 1 -w 1000> nul
	)
@echo exit