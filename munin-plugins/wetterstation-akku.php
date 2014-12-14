#!/usr/bin/php
<?php

error_reporting(E_ERROR);

if ($_SERVER['argv'][1]) {
  if ($_SERVER['argv'][1] == 'autoconf') {
    print "yes\n";
    exit(0);
  }
  if ($_SERVER['argv'][1] == 'config') {
    print "graph_title Akkuspannung\n";
    print "graph_args --alt-y-grid --rigid --lower-limit 10 --upper-limit 15 \n";
    print "graph_scale no\n";
    print "graph_vlabel Spannung in mV\n";
    print "graph_category wetterstation\n";
    print "graph_info Dieses Diagramm zeigt die Akkuspannung der Wetterstation in V.\n";
    print "BVcc.label V\n";
    exit(0);
  }
}

sleep(1);

$fd = fsockopen("tcp://192.168.0.97", 23, $errno, $errstr, 30);
if ($fd) {
  fwrite($fd, "\n");
  $zeile = fgets($fd, 1024);
  $zeile = preg_replace('/ +/',' ',$zeile);
  $zeile = explode(";", $zeile);
  echo $zeile[6]."\n";
  fclose($fd);
} else {
  echo "ERROR\n";
}

?>
