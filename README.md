Wetterstation (Arduino + Munin)
=======================

Arduino Nano (Wetterstation + Sender)
- [Bosch BMP085 Luftdruck / Temperatursensor](http://www.amazon.de/exec/obidos/ASIN/B00MLVE63I/boxnxxtde-21)
- [DHT11 Luftfeuchte / Temperatursensor](http://www.amazon.de/exec/obidos/ASIN/B00K8PR16I/boxnxxtde-21)
- [OneWire DS18B20 Temperatursensor, gekapselt in eine Edelstahlhülse](http://www.amazon.de/exec/obidos/ASIN/B00M0GROK6/boxnxxtde-21)
- [433 MHz Sender](http://www.amazon.de/exec/obidos/ASIN/B00ATZV5EQ/boxnxxtde-21)
- Spannungsversorgung
  - [10 W Solarpanel](http://www.amazon.de/exec/obidos/ASIN/B007HAZY8Y/boxnxxtde-21) 
  - [6 V 4,2Ah Akku](http://www.amazon.de/exec/obidos/ASIN/B002E4WJJU/boxnxxtde-21)
  - [LM2596 Schaltreglermodul](http://www.amazon.de/exec/obidos/ASIN/B009P04YTO/boxnxxtde-21)

Die Wetterstation befindet sich in einem selbstgebauten Wetterhütte (entfernt angelehnt an die Bauanleitung für eine Wetterhütte des  [DWD](http://www.dwd.de/bvbw/appmanager/bvbw/dwdwwwDesktop?_nfpb=true&_pageLabel=_dwdwww_spezielle_nutzer_hobbymeteorologen_meldungen&_state=maximized&_windowLabel=T19604631211153463022325)).

Arduino Uno (Empfänger)
- [Ethernet-Shield](http://www.amazon.de/exec/obidos/ASIN/B009N826DY/boxnxxtde-21) (feste private IP im Sketch)
- [433 MHz Empfänger](http://www.amazon.de/exec/obidos/ASIN/B00ATZV5EQ/boxnxxtde-21)

Die Kommunikation zwischen Sender und Empfänger per Funk wird mithilfe der [VirtualWire-Bibliothek](http://www.airspayce.com/mikem/arduino/VirtualWire/) durchgeführt.

Eine [Munin](http://munin-monitoring.org/)-Node fragt die Sensoren mit PHP-Scripten via Ethernet (Arduino Uno) ab. Ein [Munin](http://munin-monitoring.org/)-Server zeichnet die Daten auf und visualisiert sie.

Die Wetterdaten können über die Android-App ["ChiemseeWetter"](https://play.google.com/store/apps/details?id=de.nxxt.wetter.chiemseewetter) abgerufen werden.
