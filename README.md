Wetterstation (Arduino + Munin)
=======================

Ardunio Nano (Wetterstation + Sender)
- [Bosch BMP085 Luftdruck / Temperatursensor](http://www.amazon.de/exec/obidos/ASIN/B00MLVE63I/boxnxxtde-21)
- [DHT11 Luftfeuchte / Temperatursensor](http://www.amazon.de/exec/obidos/ASIN/B00K8PR16I/boxnxxtde-21)
- [OneWire DS18B20 Temperatursensor, gekapselt in eine Edelstahlhülse](http://www.amazon.de/exec/obidos/ASIN/B00M0GROK6/boxnxxtde-21)
- [433 MHz Sender](http://www.amazon.de/exec/obidos/ASIN/B00ATZV5EQ/boxnxxtde-21)

Arduino Uno (Empfänger)
- [Ethernet-Shield (feste private IP im Sketch)](http://www.amazon.de/exec/obidos/ASIN/B009N826DY/boxnxxtde-21)
- [433 MHz Empfänger](http://www.amazon.de/exec/obidos/ASIN/B00ATZV5EQ/boxnxxtde-21)

Die Kommunikation zwischen Sender und Empfänger per Funk wird mithilfe der VirtualWire-Bibliothek durchgeführt.

Eine Munin-Node fragt die Sensoren über die php-Scripten via Arduino Uno (Ethernet) ab. Munin zeichnet die Daten auf und visualisiert sie.

Die Wetterdaten können über die Android-App ["ChiemseeWetter"](https://play.google.com/store/apps/details?id=de.nxxt.wetter.chiemseewetter) abgerufen werden.
