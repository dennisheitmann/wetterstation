#!/usr/bin/php
<?php

error_reporting(E_ERROR);

if ($_SERVER['argv'][1]) {
  if ($_SERVER['argv'][1] == 'autoconf') {
    print "yes\n";
    exit(0);
  }
  if ($_SERVER['argv'][1] == 'config') {
    print "graph_title voltage\n";
    print "graph_args --alt-y-grid --rigid --lower-limit 0 --upper-limit 100 \n";
    print "graph_scale no\n";
    print "graph_vlabel voltage in mV\n";
    print "graph_category sensors\n";
    print "graph_info This graphs shows the voltage in mV.\n";
    print "mVcc.label mV\n";
    exit(0);
  }
}

$fd = fsockopen("tcp://192.168.0.97", 23, $errno, $errstr, 30);
if ($fd) {
  fwrite($fd, "\n");
  $zeile = fgets($fd, 1024);
  $zeile = explode(";", $zeile);
  echo $zeile[4];
  fclose($fd);
} else {
  echo "ERROR\n";
}

?>
