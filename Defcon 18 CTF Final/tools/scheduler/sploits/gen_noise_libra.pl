#!/usr/bin/perl

$min_noise_size = 1;
$max_noise_size = 32;
$noise_charset="abcdefghijklmnopqrstuvwxyz";

$noise_size = int($min_noise_size + rand(($max_noise_size - $min_noise_size)));

print "HELO ";
print "".substr($noise_charset, int(rand(length($noise_charset))), 1) foreach(1..$noise_size);
print "\n";

