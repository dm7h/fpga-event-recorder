USB-TPLE README
===============

Andreas Mueller
andz@gmx.de
Datum: 2010-06-11

*************************************************************
Ein universelles, rekonfigurierbares und freies 
USB-Gerät
zur Timing-, Protokoll-, Logik- und Eventanalyse von
digitalen Signalen
*************************************************************

=============================================================
University of Applied Sciences - Hochschule Augsburg, Germany
=============================================================

Inhalt:
========

1. Was ist USB-TPLE
2. Verzeichnisstruktur
3. Entwicklungsumgebung


1. Was ist USB-TPLE
-------------------

Diese Arbeit handelt von der Entwicklung der Plattform für ein 
universelles, rekonfigurierbares und freies USB Gerät, zur Timing-, 
Protokoll-, Logik- und Eventanalyse von digitalen Signalen.
Hauptaufgabe des Gerätes ist es, exakte Timing-Analysen an 
Mikrocontrollern oder ähnlichem durchzuführen.
So kann zum Beispiel die Dauer eines Prozesses extern gemessen werden, 
ohne dass durch die Messung die Laufzeit beeinflusst wird.
Kerstück des Systems ist ein konfigurierbarer Logikbaustein (CPLD) 
der Firma Altera, sowie ein Mikrocontroller der Firma Atmel mit USB 
Anbindung.
Das gesamte Projekt, sowohl Hard- als auch Software, ist im Sinne 
von Open-Source frei verfügbar und kann unter der URL

http://sta.informatik.fh-augsburg.de

abgerufen werden. Auch ein SVN Repository mit TRAC ist unter dieser 
Adresse verfügbar.

2. Verzeichnisstruktur
----------------------

./                               - Wurzelverzeichnis
./Datasheets                     - Datenblätter
./Documentation                  - Dokumentation
./Hardware                       - Verzeichnis für Schaltpläne und Boardlayouts
   \- V1.0                       - Hardware Revision 1.0
   \- V1.1                       - Hardware Revision 1.1
   \- eagle_lib                  - Nicht-Stadard Eagle Bibliotheken
./Microcontroller_Firmware/      - Firmware Mikrocontroller
   \- LUFA_Based/                - Quellcode Verzeichnis für Lufa-Basierenden Quellcode
      \- estick_firmware         - Estick-JTAG Adapter
      \- LUFA_Based/DualSerial   - Duale virtuelle serielle Schnittstelle
   \- Atmel_Based                - Quellcode Verzeichnis für Lufa-Basierenden Quellcode
      \- USB_JTAG_UART           - Jam-Player basierender JTAG-Adapter
      \- Bootloader              - Atmel Bootloader
./PC_Software                    - PC-Software
   \- urjtag-0.10                - UrJTAG
   \- DFU_programmer             - Bootloader PC-Software
   \- USB_STAPL_Player           - Software zur CPLD-Konfiguration
./PLD_Firmware/                  - Firmware Logikbaustein
   \- src/                       - VHDL-Quellcode
   \- Quartus_project/           - Quartus-II Projektdateien
./readme.txt                     - Diese Datei

3. Entwicklungsumgebung
-----------------------

Entwicklungs-PC

- CPU: x86 (Intel)
- Host OS: MS Windows 7 (Professional)
- VM-OS: GNU/Linux (Debian Lenny)
- Kernel:  2.6.26-1-686
- GCC: 4.3.2
- Altera Quartus II: 9.1
- ARV Studio 8

Sonstige Hardware:

- Programmiergerät Mikrocontroller: Atmel JTAG-ICE-MKII
- Programmiergerät Logikbaustein: Altera USB-Blaster (Rev. 2)

Bei Fragen einfach auf http://www.hs-augsburg.de/~hhoegl vorbeischauen, oder eine Email an einen der Entwickler schicken.

Viel Spaß

