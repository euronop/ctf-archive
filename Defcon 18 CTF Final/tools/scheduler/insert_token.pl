#!/usr/bin/perl
#
# CREATE TABLE tokens (id integer primary key, token TEXT, harvest_timestamp INTEGER, submit_timestamp INTEGER, return_message TEXT);

$_ = <STDIN>;
chomp;

exit if((!$_) || (!/^[\da-f]+$/));

$stdin_hex = unpack("H*");

do {
#	print "sqlite3 ctf.db \"INSERT INTO tokens (token,harvest_timestamp,submit_timestamp,return_message) VALUES(x'$stdin_hex',strftime('%s','now'),NULL,NULL);\"";

	$_ = `sqlite3 ctf.db "INSERT INTO tokens (token,harvest_timestamp,submit_timestamp,return_message) VALUES(x'$stdin_hex',strftime('%s','now'),NULL,NULL);" 2>&1`;
        chomp;

        print STDERR $_ if($_);
} while($_);

