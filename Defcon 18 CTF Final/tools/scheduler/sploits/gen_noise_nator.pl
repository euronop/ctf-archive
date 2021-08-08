#!/usr/bin/perl

$min_noise_size = 2;
$max_noise_size = 50;
$noise_charset=" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

$noise_size = int($min_noise_size + rand(($max_noise_size - $min_noise_size)));

print "".substr($noise_charset, int(rand(length($noise_charset))), 1) foreach(1..$noise_size);
print "\n";

