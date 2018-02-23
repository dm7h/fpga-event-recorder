#!/usr/bin/perl

 $my_time = time(); # Create a unique name via time
 $my_time = sprintf("%08X",$my_time);
 print $my_time ."\n";

