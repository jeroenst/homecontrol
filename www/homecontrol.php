<?php
 $qstr = str_replace('%20', ' ', $_SERVER['QUERY_STRING']);
 $qstr = ereg_replace("[^A-Za-z0-9]", " ", $qstr);
// $qstr = "help";
 $cmd = "/usr/bin/homecontrol $qstr";
 exec ($cmd, $output, $execreturnvalue);
 if ($execreturnvalue == 0)
 {
  echo "<HTML><BODY>Ok</BODY></HTML>";
 }
 else
 {
  echo "<HTML><BODY>Fail</BODY></HTML>";
 }
?>