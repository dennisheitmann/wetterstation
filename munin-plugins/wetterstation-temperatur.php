#!/usr/bin/php
<?php

error_reporting(E_ERROR);

if ($_SERVER['argv'][1]) {
  if ($_SERVER['argv'][1] == 'autoconf') {
    print "yes\n";
    exit(0);
  }
  if ($_SERVER['argv'][1] == 'config') {
    print "graph_title Temperatur\n";
    print "graph_args --alt-y-grid -l 0\n";
    print "graph_scale no\n";
    print "graph_vlabel Temperatur in Grad Celsius\n";
    print "graph_category wetterstation\n";
    print "graph_info Dieses Diagramm zeigt die Temperatur in Grad Celsius an (Bosch).\n";
    print "t1_C.label t[C]\n";
    exit(0);
  }
}

$fd = fsockopen("tcp://192.168.0.97", 23, $errno, $errstr, 30);
if ($fd) {
  fwrite($fd, "\n");
  $zeile = fgets($fd, 1024);
  $zeile = explode(";", $zeile);
  echo $zeile[0]."\n";
  fclose($fd);
} else {
  echo "ERROR\n";
}

?>
