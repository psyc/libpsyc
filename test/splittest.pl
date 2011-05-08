#!/usr/bin/env perl
use strict;
use warnings;
use IO::Socket;
$| = 1;

print "Usage: splittest.pl <packet file> [[<host>:]<port> [<chunk length> [-v]]]\n" and exit unless @ARGV;
my ($host, $port) = $ARGV[1] =~ /^(?:([a-z0-9.-]+):)?(\d+)$/i;
$host ||= 'localhost'; $port ||= 4440;
my $length = int($ARGV[2] || 1); $length = 1 if $length < 1;
my $verbose = $ARGV[3];

open FILE, '<', $ARGV[0] or die "$ARGV[0]: $!\n";
my $file = ''; $file .= $_ while <FILE>;
close FILE;

my $s = IO::Socket::INET->new(Proto => "tcp", PeerAddr => $host, PeerPort => $port) or die "$host:$port: $!\n";

my $c = 0;
while ($c < length $file) {
	my $chunk = substr $file, $c, $length;
	print "[$chunk]" if $verbose;
	print $s $chunk;
	$c += $length;
}

print while <$s>;
close $s;
