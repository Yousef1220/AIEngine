AUTOMAKE_OPTIONS = gnu

TEST = test_flows 

check_PROGRAMS = test_flows 

test_flows_CPPFLAGS = $(EXTRA_TEST_CPPFLAGS) -I../ $(BOOST_CPPFLAGS) -DSTAND_ALONE_TEST
test_flows_SOURCES =     ../Flow.cc ../IPAddress.cc \
			../protocols/tcp/TCPInfo.cc \
			../protocols/http/HTTPInfo.cc \
			../protocols/ssl/SSLInfo.cc \
			../protocols/smtp/SMTPInfo.cc \
			../protocols/bitcoin/BitcoinInfo.cc \
			../protocols/dns/DNSInfo.cc \
			../protocols/gprs/GPRSInfo.cc \
			../protocols/sip/SIPInfo.cc \
			../protocols/ssdp/SSDPInfo.cc \
			../protocols/coap/CoAPInfo.cc \
			../protocols/pop/POPInfo.cc \
			../protocols/imap/IMAPInfo.cc \
			../protocols/mqtt/MQTTInfo.cc \
			../protocols/netbios/NetbiosInfo.cc \
			../protocols/dhcp/DHCPInfo.cc \
			../protocols/smb/SMBInfo.cc \
			../protocols/dhcp6/DHCPv6Info.cc \
			../protocols/ssh/SSHInfo.cc \
			../protocols/dcerpc/DCERPCInfo.cc \
			../protocols/frequency/Frequencies.cc \
			../protocols/frequency/PacketFrequencies.cc \
			../AnomalyManager.cc FlowManager.cc test_flows.cc

test_flows_LDFLAGS   = $(EXTRA_TEST_LDFLAGS)
test_flows_LDADD   = $(BOOST_LDFLAGS) -lboost_unit_test_framework

CLEANFILES = *.o *.lo *.so

MAINTAINERCLEANFILES = \
        $(srcdir)/Makefile \
        $(srcdir)/Makefile.in \
        `rm -rf $(srcdir)/build `

