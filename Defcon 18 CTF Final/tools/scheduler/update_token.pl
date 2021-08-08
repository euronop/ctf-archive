#!/usr/bin/perl
#
# CREATE TABLE tokens (id integer primary key, token TEXT, harvest_timestamp INTEGER, submit_timestamp INTEGER, return_message TEXT);

$argv_0_hex = unpack("H*", $ARGV[0]);

@stdin = <STDIN>;
$_ = $stdin[($#stdin)];
chomp;
$stdin_hex = unpack("H*", $_);

exit if((!$ARGV[0]) || (!$_));

print $_.": ".$stdin[$_]."\n" for(0..$#stdin);
print $_."\n";
#print $stdin_hex."\n";
#print $ARGV[0]."\n";
#print $argv_0_hex."\n";

do {
	print "sqlite3 ctf.db \"update tokens set submit_timestamp=strftime('%s','now'),return_message=x'$stdin_hex' WHERE token=x'$argv_0_hex';\"\n";
	$_ = `sqlite3 ctf.db "update tokens set submit_timestamp=strftime('%s','now'),return_message=x'$stdin_hex' WHERE token=x'$argv_0_hex';"`;
	
	chomp;

	print STDERR $_ if($_);
} while($_);

