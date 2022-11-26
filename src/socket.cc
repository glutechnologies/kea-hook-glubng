/* 
 * This file is part of the XXX distribution (https://github.com/glutechnologies/kea-hook-glubng).
 * Copyright (c) 2022 Glutec
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/asio.hpp>
#include <hooks/hooks.h>
#include <dhcp/pkt4.h>
#include <dhcp/dhcp6.h>
#include <dhcp/pkt6.h>
#include <dhcp/option6_ia.h>
#include <dhcpsrv/subnet.h>
#include <dhcpsrv/lease.h>

#include "nlohmann/json.hpp"
#include "logger.h"
#include "common.h"

using namespace isc::dhcp;
using namespace isc::hooks;

extern "C" {
  int send_socket_data(const nlohmann::json &msg) {
    boost::asio::io_service io_service;
    boost::asio::local::stream_protocol::socket socket(io_service);

    try {
      socket.connect(boost::asio::local::stream_protocol::endpoint(socket_name));
      
      std::string res = msg.dump();
      boost::asio::write(socket, boost::asio::buffer(res.data(), res.size()));
    } catch (std::exception &e) {
      LOG_ERROR(glubng_logger, GLUBNG_SOCKET_EXCEPTION).arg(e.what());
    }

    return 0;
  }
}