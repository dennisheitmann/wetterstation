#!/usr/bin/php
<?php

error_reporting(E_ERROR);

if ($_SERVER['argv'][1]) {
  if ($_SERVER['argv'][1] == 'autoconf') {
    print "yes\n";
    exit(0);
  }
  if ($_SERVER['argv'][1] == 'config') {
    print "graph_title Luftdruck\n";
    print "graph_args --alt-y-grid --rigid --lower-limit 970 --upper-limit 1030 --base 1000 --units-exponent 1\n";
    print "graph_scale no\n";
    print "graph_vlabel Luftdruck in mbar\n";
    print "graph_category wetterstation\n";
    print "graph_info Dieses Diagramm zeigt den Luftdruck bezogen auf Nullniveau (0m) an.\n";
    print "mbar.label p[mbar]\n";
    exit(0);
  }
}

$fd = fsockopen("tcp://192.168.0.97", 23, $errno, $errstr, 30);
if ($fd) {
  fwrite($fd, "\n");
  $zeile = fgets($fd, 1024);
  $zeile = preg_replace('/ +/',' ',$zeile);
  $zeile = explode(";", $zeile);
  echo $zeile[3]."\n";
  fclose($fd);
} else {
  echo "ERROR\n";
}

?>
