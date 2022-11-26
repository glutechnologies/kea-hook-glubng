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

#ifndef KEA_HOOK_GLUBNG_SOCKET
#define KEA_HOOK_GLUBNG_SOCKET

#include <string>

#include "nlohmann/json.hpp"

using namespace isc::dhcp;
using namespace isc::hooks;

extern "C" {

  /* Sends lease4 information using sockets */
  int send_socket_data(const nlohmann::json &msg);

}

#endif