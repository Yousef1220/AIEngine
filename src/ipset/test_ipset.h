/*
 * AIEngine a new generation network intrusion detection system.
 *
 * Copyright (C) 2013-2018  Luis Campo Giralte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Ryadnology Team; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Ryadnology Team, 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Written by Luis Campo Giralte <me@ryadpasha.com> 
 *
 */
#ifndef _TEST_IPSET_H_
#define _TEST_IPSET_H_

#include <string>
#include "StackTest.h"
#include "IPSet.h"
#include "IPRadixTree.h"
#include "IPSetManager.h"
#include "../Protocol.h"
#include "../Multiplexer.h"
#include "../protocols/ethernet/EthernetProtocol.h"
#include "../protocols/vlan/VLanProtocol.h"
#include "../protocols/ip/IPProtocol.h"
#include "../protocols/ip6/IPv6Protocol.h"
#include "../protocols/tcp/TCPProtocol.h"
#include "../protocols/udp/UDPProtocol.h"
#include "../protocols/tcpgeneric/TCPGenericProtocol.h"
#include "../protocols/udpgeneric/UDPGenericProtocol.h"
#include "../regex/RegexManager.h"

#ifdef HAVE_BLOOMFILTER
#include <boost/bloom_filter/basic_bloom_filter.hpp>
#include "IPBloomSet.h"
#endif

using namespace aiengine;

struct StackTCPIPSetTest: public StackTest
{
        IPProtocolPtr ip;
        TCPProtocolPtr tcp;
        UDPProtocolPtr udp;
        TCPGenericProtocolPtr gtcp;
        UDPGenericProtocolPtr gudp;
        MultiplexerPtr mux_ip;
        MultiplexerPtr mux_tcp;
        MultiplexerPtr mux_udp;

	SharedPointer<FlowForwarder> ff_tcp;
	SharedPointer<FlowForwarder> ff_gtcp;
	SharedPointer<FlowForwarder> ff_udp;
	SharedPointer<FlowForwarder> ff_gudp;
       // FlowManager and FlowCache
        FlowManagerPtr flow_mng;
        FlowCachePtr flow_cache;

        StackTCPIPSetTest()
        {
                tcp = TCPProtocolPtr(new TCPProtocol());
                udp = UDPProtocolPtr(new UDPProtocol());
                gtcp = TCPGenericProtocolPtr(new TCPGenericProtocol());
                gudp = UDPGenericProtocolPtr(new UDPGenericProtocol());
                ip = IPProtocolPtr(new IPProtocol());
                mux_ip = MultiplexerPtr(new Multiplexer());
                mux_tcp = MultiplexerPtr(new Multiplexer());
                mux_udp = MultiplexerPtr(new Multiplexer());
		ff_tcp = SharedPointer<FlowForwarder>(new FlowForwarder());
		ff_udp = SharedPointer<FlowForwarder>(new FlowForwarder());
		ff_gtcp = SharedPointer<FlowForwarder>(new FlowForwarder());
		ff_gudp = SharedPointer<FlowForwarder>(new FlowForwarder());

                // configure the ip
                ip->setMultiplexer(mux_ip);
                mux_ip->setProtocol(static_cast<ProtocolPtr>(ip));
                mux_ip->setProtocolIdentifier(ETHERTYPE_IP);
                mux_ip->setHeaderSize(ip->getHeaderSize());
                mux_ip->addChecker(std::bind(&IPProtocol::ipChecker, ip, std::placeholders::_1));
                mux_ip->addPacketFunction(std::bind(&IPProtocol::processPacket, ip, std::placeholders::_1));

                //configure the tcp
                tcp->setMultiplexer(mux_tcp);
                mux_tcp->setProtocol(static_cast<ProtocolPtr>(tcp));
                mux_tcp->setProtocolIdentifier(IPPROTO_TCP);
                mux_tcp->setHeaderSize(tcp->getHeaderSize());
                mux_tcp->addChecker(std::bind(&TCPProtocol::tcpChecker, tcp, std::placeholders::_1));
                mux_tcp->addPacketFunction(std::bind(&TCPProtocol::processPacket, tcp, std::placeholders::_1));
		ff_tcp->setProtocol(static_cast<ProtocolPtr>(tcp));

                udp->setMultiplexer(mux_udp);
                mux_udp->setProtocol(static_cast<ProtocolPtr>(udp));
                mux_udp->setProtocolIdentifier(IPPROTO_UDP);
                mux_udp->setHeaderSize(udp->getHeaderSize());
                mux_udp->addChecker(std::bind(&UDPProtocol::udpChecker, udp, std::placeholders::_1));
                mux_udp->addPacketFunction(std::bind(&UDPProtocol::processPacket, udp, std::placeholders::_1));
                ff_udp->setProtocol(static_cast<ProtocolPtr>(udp));

                // configure the generic tcp
                gtcp->setFlowForwarder(ff_gtcp);
                ff_gtcp->setProtocol(static_cast<ProtocolPtr>(gtcp));
                ff_gtcp->addChecker(std::bind(&TCPGenericProtocol::tcpGenericChecker, gtcp, std::placeholders::_1));
                ff_gtcp->addFlowFunction(std::bind(&TCPGenericProtocol::processFlow, gtcp, std::placeholders::_1));

                gudp->setFlowForwarder(ff_gudp);
                ff_gudp->setProtocol(static_cast<ProtocolPtr>(udp));
                ff_gudp->addChecker(std::bind(&UDPGenericProtocol::udpGenericChecker, gudp, std::placeholders::_1));
                ff_gudp->addFlowFunction(std::bind(&UDPGenericProtocol::processFlow, gudp, std::placeholders::_1));

                // configure the multiplexers
                mux_eth->addUpMultiplexer(mux_ip, ETHERTYPE_IP);
                mux_ip->addDownMultiplexer(mux_eth);
                mux_ip->addUpMultiplexer(mux_tcp, IPPROTO_TCP);
                mux_tcp->addDownMultiplexer(mux_ip);
                mux_ip->addUpMultiplexer(mux_udp, IPPROTO_UDP);
                mux_udp->addDownMultiplexer(mux_ip);

		tcp->setFlowForwarder(ff_tcp);	
                mux_tcp->setProtocol(static_cast<ProtocolPtr>(tcp));
        	ff_tcp->setProtocol(static_cast<ProtocolPtr>(tcp));

		udp->setFlowForwarder(ff_udp);	
                mux_udp->setProtocol(static_cast<ProtocolPtr>(udp));
        	ff_udp->setProtocol(static_cast<ProtocolPtr>(udp));

                // Allocate the flow caches and tables
                flow_mng = FlowManagerPtr(new FlowManager());
                flow_cache = FlowCachePtr(new FlowCache());

                // Connect the FlowManager and FlowCache
                flow_cache->createFlows(2);
                tcp->createTCPInfos(2);
                // udp->createUDPInfos(2);

                tcp->setFlowCache(flow_cache);
                tcp->setFlowManager(flow_mng);
                udp->setFlowCache(flow_cache);
                udp->setFlowManager(flow_mng);

                // Configure the FlowForwarders
                tcp->setFlowForwarder(ff_tcp);
                ff_tcp->addUpFlowForwarder(ff_gtcp);

                udp->setFlowForwarder(ff_udp);
                ff_udp->addUpFlowForwarder(ff_gudp);
        }

	void inject(Packet &pkt) {

        	mux_eth->setPacket(&pkt);
        	eth->setHeader(pkt.getPayload());
        	mux_eth->setNextProtocolIdentifier(eth->getEthernetType());
        	mux_eth->forwardPacket(pkt);
	}

        void show() {
		ip->statistics(std::cout, 5);
                udp->statistics(std::cout, 5);
                gudp->statistics(std::cout, 5);
        }

        ~StackTCPIPSetTest() {}
};

#endif
