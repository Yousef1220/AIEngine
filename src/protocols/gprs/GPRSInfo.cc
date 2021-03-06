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
#include "GPRSInfo.h"

namespace aiengine {

void GPRSInfo::reset() { 
	imsi_ = 0;
	imei_ = 0;
	pdp_type_number_ = 0; // The upper protocol
}

void GPRSInfo::serialize(JsonFlow &j) {

#if !defined(RUBY_BINDING)
       	j.j["info"]["imsi"] = getIMSIString();
#else
        std::map<std::string, json_map_t> info;

       	info["imsi"] = getIMSIString();
        j.j["info"] = info;
#endif
}

std::string& GPRSInfo::getIMSIString() const { 
	std::ostringstream o;
	static std::string cad;
	uint8_t bcd;
	uint8_t *data = (uint8_t*)&imsi_;

	for(int i = 0; i < 8; ++i) {
		bcd = *data & 0xF;
		if (bcd != 0xF) o << (int)bcd;
		bcd = *data >> 4;
		if (bcd != 0xF) o << (int) bcd; 
		data++;
	}
			
	cad = o.str();
	return cad;
}

std::ostream& operator<< (std::ostream &out, const GPRSInfo &info) {

	out << " IMSI(" << info.getIMSIString() << ")";
	if (info.pdp_type_number_ == PDP_END_USER_TYPE_IPV4) out << "IPv4";
	if (info.pdp_type_number_ == PDP_END_USER_TYPE_IPV6) out << "IPv6";
	return out;
}

} // namespace aiengine
