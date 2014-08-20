#!/usr/bin/php
<?php

error_reporting(E_ERROR);

if ($_SERVER['argv'][1]) {
  if ($_SERVER['argv'][1] == 'autoconf') {
    print "yes\n";
    exit(0);
  }
  if ($_SERVER['argv'][1] == 'config') {
    print "graph_title barometric pressure outside\n";
    print "graph_args --alt-y-grid --rigid --lower-limit 970 --upper-limit 1030 --base 1000 --units-exponent 1\n";
    print "graph_scale no\n";
    print "graph_vlabel pressure in mbar\n";
    print "graph_category sensors\n";
    print "graph_info This graphs shows the barometric pressure (sea level).\n";
    print "mbar.label p[mbar]\n";
    exit(0);
  }
}

$fd = fsockopen("tcp://192.168.0.97", 23, $errno, $errstr, 30);
if ($fd) {
  fwrite($fd, "\n");
  $zeile = fgets($fd, 1024);
  $zeile = explode(";", $zeile);
  echo $zeile[2]."\n";
  fclose($fd);
} else {
  echo "ERROR\n";
}

?>
