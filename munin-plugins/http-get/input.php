<?php
// Push Example:
// "GET /wetterinput/input.php?data=t1_C.value=19.5,t2_C.value=19.0,t3_C.value=12.4,mbar.value=1018.3,hum%.value=47.0,mVcc.value=5028.0,BVcc.value=13.9,SVcc.value=21.4 HTTP/1.1"

$data = strip_tags($_GET['data']);

if ($data) {
  $data = str_replace('=', ' ', $data);
  $data = explode(',', $data);
  foreach ($data as $zeile) {
    if (strstr($zeile, 't1_C.value')) {
      file_put_contents("/tmp/wetterstation-temperatur1-value", $zeile."\n");
      $t1_C = str_replace('t1_C.value', '', $zeile);
    }
    else if (strstr($zeile, 't2_C.value')) {
      file_put_contents("/tmp/wetterstation-temperatur2-value", $zeile."\n");
      $t2_C = str_replace('t2_C.value', '', $zeile);
    }
    else if (strstr($zeile, 't3_C.value')) {
      file_put_contents("/tmp/wetterstation-temperatur3-value", $zeile."\n");
      $t3_C = str_replace('t3_C.value', '', $zeile);
    }
    else if (strstr($zeile, 'mbar.value')) {
      file_put_contents("/tmp/wetterstation-luftdruck-value", $zeile."\n");
      $mbar = str_replace('mbar.value', '', $zeile);
    }
    else if (strstr($zeile, 'hum%.value')) {
      file_put_contents("/tmp/wetterstation-luftfeuchte-value", $zeile."\n");
      $humi = str_replace('hum%.value', '', $zeile);
    }
    else if (strstr($zeile, 'mVcc.value')) {
      file_put_contents("/tmp/wetterstation-spannung-value", $zeile."\n");
      $mVcc = str_replace('mVcc.value', '', $zeile);
    }
    else if (strstr($zeile, 'BVcc.value')) {
      file_put_contents("/tmp/wetterstation-akku-value", $zeile."\n");
      $BVcc = str_replace('BVcc.value', '', $zeile);
    }
    else if (strstr($zeile, 'SVcc.value')) {
      file_put_contents("/tmp/wetterstation-solar-value", $zeile."\n");
      $SVcc = str_replace('SVcc.value', '', $zeile);
    }
  }
  $t0_C = min($t1_C, $t2_C, $t3_C);
  file_put_contents("/tmp/wetterstation-temperatur_min-value", "t0_C.value".$t0_C."\n");
}

?>
