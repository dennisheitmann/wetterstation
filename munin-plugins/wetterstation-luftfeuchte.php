#!/usr/bin/php
<?php

error_reporting(E_ERROR);

if ($_SERVER['argv'][1]) {
  if ($_SERVER['argv'][1] == 'autoconf') {
    print "yes\n";
    exit(0);
  }
  if ($_SERVER['argv'][1] == 'config') {
    print "graph_title humidity outside\n";
    print "graph_args --alt-y-grid --rigid --lower-limit 0 --upper-limit 100 \n";
    print "graph_scale no\n";
    print "graph_vlabel humidity in percent\n";
    print "graph_category sensors\n";
    print "graph_info This graphs shows the humidity (DHT11 Sensor).\n";
    print "hum%.label h[%]\n";
    exit(0);
  }
}

$fd = fsockopen("tcp://192.168.0.97", 23, $errno, $errstr, 30);
if ($fd) {
  fwrite($fd, "\n");
  $zeile = fgets($fd, 1024);
  $zeile = explode(";", $zeile);
  echo $zeile[3];
  fclose($fd);
} else {
  echo "ERROR\n";
}

?>
