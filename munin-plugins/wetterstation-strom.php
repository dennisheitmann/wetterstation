#!/usr/bin/php
<?php

error_reporting(E_ERROR);

if ($_SERVER['argv'][1]) {
  if ($_SERVER['argv'][1] == 'autoconf') {
    print "yes\n";
    exit(0);
  }
  if ($_SERVER['argv'][1] == 'config') {
    print "graph_title Stromfluss\n";
    print "graph_args --alt-y-grid \n";
    print "graph_scale no\n";
    print "graph_vlabel Strom in mA\n";
    print "graph_category wetterstation\n";
    print "graph_info Dieses Diagramm zeigt den Stromfluss vom/zum Akku der Wetterstation in mA.\n";
    print "I_mA.label mA\n";
    exit(0);
  }
}

$fd = fsockopen("tcp://192.168.0.97", 23, $errno, $errstr, 30);
if ($fd) {
  fwrite($fd, "\n");
  $zeile = fgets($fd, 1024);
  $zeile = explode(";", $zeile);
  echo $zeile[5];
  fclose($fd);
} else {
  echo "ERROR\n";
}

?>
