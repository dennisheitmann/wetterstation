#!/usr/bin/php
<?php

error_reporting(E_ERROR);

if ($_SERVER['argv'][1]) {
  if ($_SERVER['argv'][1] == 'autoconf') {
    print "yes\n";
    exit(0);
  }
  if ($_SERVER['argv'][1] == 'config') {
    print "graph_title Temperature outside\n";
    print "graph_args --alt-y-grid -l 0\n";
    print "graph_scale no\n";
    print "graph_vlabel temperature in degC\n";
    print "graph_category sensors\n";
    print "graph_info This graphs shows the temperature outside.\n";
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
