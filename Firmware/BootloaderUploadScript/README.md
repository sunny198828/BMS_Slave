# Upload Script
UpdateV2.bat "COMPORT" "FIRST SLAVE ID" "LAST SLAVE ID" "Firmware Datei"

Beispiel: 8 Zellen ID 101 bis 108, Comport COM6, Firmware Datei: FirmwareV2.hex

UpdateV2.bat COM6 101 108 FirmwareV2.hex

Es wird ein Ordner erstellt in dem avrdude die Logdateien ablegt und ein Ordner wo die EEPROM Daten abgelegt werden.

