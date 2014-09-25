wetterstation (Arduino)
=======================

Ardunio Nano mit 433MHz Sender
- Bosch BMP085 Luftdruck / Temperatursensor
- DHT11 Luftfeuchte / Temperatursensor
- OneWire DS18B20 Temperatursensor

Arduino Uno mit 433MHz Empfänger
- Ethernet-Shield (feste private IP im Sketch)

Die Kommunikation zwischen Sender und Empfänger per Funk wird mithilfe der VirtualWire-Bibliothek durchgeführt.

Eine Munin-Node fragt die Sensoren über die php-Scripten via Arduino Uno (Ethernet) ab. Munin zeichnet die Daten auf und visualisiert sie.

Die Wetterdaten können über die Android-App ["ChiemseeWetter"](https://play.google.com/store/apps/details?id=de.nxxt.wetter.chiemseewetter) abgerufen werden.
