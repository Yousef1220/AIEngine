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
#ifndef _TEST_UDPGENERIC_H_
#define _TEST_UDPGENERIC_H_

#include <string>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBLOG4CXX
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#endif

#include "Protocol.h"
#include "StackTest.h"
#include "../ip/IPProtocol.h"
#include "../udp/UDPProtocol.h"
#include "UDPGenericProtocol.h"

using namespace aiengine;

#ifdef HAVE_LIBLOG4CXX
using namespace log4cxx;
using namespace log4cxx::helpers;
#endif

struct StackUDPGenericTest : public StackTest
{
        IPProtocolPtr ip;
        UDPProtocolPtr udp;
	UDPGenericProtocolPtr gudp;
        MultiplexerPtr mux_ip;
        MultiplexerPtr mux_udp;

       // FlowManager and FlowCache
        FlowManagerPtr flow_mng;
        FlowCachePtr flow_cache;

        // FlowForwarders
        SharedPointer<FlowForwarder> ff_udp;
        SharedPointer<FlowForwarder> ff_gudp;

        StackUDPGenericTest()
        {
#ifdef HAVE_LIBLOG4CXX
                BasicConfigurator::configure();
#endif
                ip = IPProtocolPtr(new IPProtocol());
                udp = UDPProtocolPtr(new UDPProtocol());
                gudp = UDPGenericProtocolPtr(new UDPGenericProtocol());
                mux_ip = MultiplexerPtr(new Multiplexer());
                mux_udp = MultiplexerPtr(new Multiplexer());
                ff_udp = SharedPointer<FlowForwarder>(new FlowForwarder());
                ff_gudp = SharedPointer<FlowForwarder>(new FlowForwarder());

                // Allocate the flow caches and tables
                flow_mng = FlowManagerPtr(new FlowManager());
                flow_cache = FlowCachePtr(new FlowCache());

                // configure the ip
                ip->setMultiplexer(mux_ip);
                mux_ip->setProtocol(static_cast<ProtocolPtr>(ip));
                mux_ip->setProtocolIdentifier(ETHERTYPE_IP);
                mux_ip->setHeaderSize(ip->getHeaderSize());
                mux_ip->addChecker(std::bind(&IPProtocol::ipChecker, ip, std::placeholders::_1));
                mux_ip->addPacketFunction(std::bind(&IPProtocol::processPacket, ip, std::placeholders::_1));

                //configure the udp
                udp->setMultiplexer(mux_udp);
                mux_udp->setProtocol(static_cast<ProtocolPtr>(udp));
                mux_udp->setProtocolIdentifier(IPPROTO_UDP);
                mux_udp->setHeaderSize(udp->getHeaderSize());
                mux_udp->addChecker(std::bind(&UDPProtocol::udpChecker, udp, std::placeholders::_1));
                mux_udp->addPacketFunction(std::bind(&UDPProtocol::processPacket, udp, std::placeholders::_1));

                // configure the generic udp 
                gudp->setFlowForwarder(ff_gudp);
                ff_gudp->setProtocol(static_cast<ProtocolPtr>(gudp));
                ff_gudp->addChecker(std::bind(&UDPGenericProtocol::udpGenericChecker, gudp, std::placeholders::_1));
                ff_gudp->addFlowFunction(std::bind(&UDPGenericProtocol::processFlow, gudp, std::placeholders::_1));

		mux_eth->addUpMultiplexer(mux_ip, ETHERTYPE_IP);
		mux_ip->addDownMultiplexer(mux_eth);
		mux_ip->addUpMultiplexer(mux_udp, IPPROTO_UDP);
		mux_udp->addDownMultiplexer(mux_ip);

                // Connect the FlowManager and FlowCache
                flow_cache->createFlows(1);

                udp->setFlowCache(flow_cache);
                udp->setFlowManager(flow_mng);

                // Configure the FlowForwarders
                udp->setFlowForwarder(ff_udp);

                ff_udp->addUpFlowForwarder(ff_gudp);

        }

        ~StackUDPGenericTest() {}
};

#endif
