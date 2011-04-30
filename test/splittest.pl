#!/usr/bin/env perl
use strict;
use warnings;
use IO::Socket;
$| = 1;

print "Usage: splittest.pl <packet file> [<port> [<chunk length> [-v]]]\n" and exit unless @ARGV;
my $port = $ARGV[1] || 4440;
my $length = int($ARGV[2] || 1); $length = 1 if $length < 1;
my $verbose = $ARGV[3];

open FILE, '<', $ARGV[0] or die "$ARGV[0]: $!\n";
my $file = ''; $file .= $_ while <FILE>;
close FILE;

my $s = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "localhost", PeerPort => $port) or die "localhost:$port: $!\n";
$s->autoflush(1);

my $c = 0;
while ($c < length $file) {
	my $chunk = substr $file, $c, $length;
	print "[$chunk]" if $verbose;
	print $s $chunk;
	$c += $length;
}

print while <$s>;
close $s;
