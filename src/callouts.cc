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

#include <hooks/hooks.h>
#include <dhcp/pkt4.h>
#include <dhcp/dhcp6.h>
#include <dhcp/pkt6.h>
#include <dhcp/option6_ia.h>
#include <dhcpsrv/subnet.h>
#include <dhcpsrv/lease.h>

#include "nlohmann/json.hpp"
#include "common.h"

#include <string>
#include <vector>

#include "socket.h"

using namespace isc::dhcp;
using namespace isc::hooks;

extern "C" {
  void extract_pkt4(nlohmann::json &res, const Pkt4Ptr pkt4) {
    res["query"]["type"] = std::string(pkt4->getName());
    res["query"]["interface"] = pkt4->getIface();
    res["query"]["if-index"] = pkt4->getIndex();

    HWAddrPtr hwaddr = pkt4->getHWAddr();
    if (hwaddr) {
      res["query"]["hw-addr"] = hwaddr->toText(false);
      res["query"]["hw-addr-type"] = std::to_string(hwaddr->htype_);
      res["query"]["hw-addr-source"] = std::to_string(hwaddr->source_);
    }

    /* Specific Options */
    OptionPtr option60 = pkt4->getOption(60);
    if (option60) {
      res["query"]["option60"] = option60->toString();
    }

    OptionPtr rai = pkt4->getOption(82);
    if (rai) {
      res["query"]["option82"] = rai->toHexString();

      OptionPtr circuit_id = rai->getOption(RAI_OPTION_AGENT_CIRCUIT_ID);
      if (circuit_id) {
        res["query"]["option82-circuit-id"] = circuit_id->toHexString();
      }

      OptionPtr remote_id = rai->getOption(RAI_OPTION_REMOTE_ID);
      if (remote_id) {
        res["query"]["option82-remote-id"] = remote_id->toHexString();
      }
    }
  }

  void extract_subnet4(nlohmann::json &res, const Subnet4Ptr subnet) {
    /* The subnet given by Kea might be NULL, this seems to happen when
     * Kea fails to find a matching subnet for a client request. */
    if (subnet != NULL) {
      res["subnet"]["name"] = subnet->toText();
      std::pair<isc::asiolink::IOAddress, uint8_t> prefix = subnet->get();
      res["subnet"]["prefix"] = prefix.first.toText();
      res["subnet"]["len"] = prefix.second;
    }
  }

  void extract_lease4(nlohmann::json &res, const Lease4Ptr lease) {
    res["lease"]["state"] = lease->basicStatesToText(lease->state_);
    res["lease"]["is-expired"] = lease->expired();
    res["lease"]["address"] = lease->addr_.toText();
    if (lease->hwaddr_) {
      res["lease"]["hw-addr"] = lease->hwaddr_->toText(false);
    }
    res["lease"]["hostname"] = lease->hwaddr_->toText(false);
    res["lease"]["cltt"] = lease->cltt_;
    res["lease"]["valid-lft"] = lease->valid_lft_;
  }

  int lease4_select(CalloutHandle& handle) {
    Pkt4Ptr query;
    Subnet4Ptr subnet;
    bool fake_allocation;
    Lease4Ptr lease;

    nlohmann::json msg;
    msg["callout"] = CALLOUT_LEASE4_SELECT;

    handle.getArgument("query4", query);
    // Process query
    extract_pkt4(msg, query);

    handle.getArgument("subnet4", subnet);
    extract_subnet4(msg, subnet);

    handle.getArgument("lease4", lease);
    extract_lease4(msg, lease);
    
    /* Send data to socket */
    int ret;
    ret = send_socket_data(msg);
    return 0;
  }

  int lease4_renew(CalloutHandle& handle) {
    Pkt4Ptr query;
    Subnet4Ptr subnet;
    Lease4Ptr lease;

    nlohmann::json msg;
    msg["callout"] = CALLOUT_LEASE4_RENEW;

    handle.getArgument("query4", query);
    // Process query
    extract_pkt4(msg, query);

    handle.getArgument("subnet4", subnet);
    extract_subnet4(msg, subnet);

    handle.getArgument("lease4", lease);
    extract_lease4(msg, lease);
    
    /* Send data to socket */
    int ret;
    ret = send_socket_data(msg);
    return 0;
  }

  int lease4_release(CalloutHandle& handle) {
    Pkt4Ptr query;
    Subnet4Ptr subnet;
    bool fake_allocation;
    Lease4Ptr lease;

    nlohmann::json msg;
    msg["callout"] = CALLOUT_LEASE4_RELEASE;

    handle.getArgument("query4", query);
    // Process query
    extract_pkt4(msg, query);

    handle.getArgument("lease4", lease);
    extract_lease4(msg, lease);
    
    /* Send data to socket */
    int ret;
    ret = send_socket_data(msg);
    return 0;
  }

  int lease4_decline(CalloutHandle& handle) {
    Pkt4Ptr query;
    Lease4Ptr lease;

    nlohmann::json msg;
    msg["callout"] = CALLOUT_LEASE4_DECLINE;

    handle.getArgument("query4", query);
    // Process query
    extract_pkt4(msg, query);

    handle.getArgument("lease4", lease);
    extract_lease4(msg, lease);

    /* Send data to socket */
    int ret;
    ret = send_socket_data(msg);
    return 0;
  }

  int lease4_expire(CalloutHandle& handle) {
    Lease4Ptr lease;
    bool remove_lease;

    nlohmann::json msg;
    msg["callout"] = CALLOUT_LEASE4_EXPIRE;

    handle.getArgument("lease4", lease);
    extract_lease4(msg, lease);

    handle.getArgument("remove_lease", remove_lease);
    msg["remove-lease"] = remove_lease;
    
    /* Send data to socket */
    int ret;
    ret = send_socket_data(msg);
    return 0;
  }

  int lease4_recover(CalloutHandle& handle) {
    Lease4Ptr lease;

    nlohmann::json msg;
    msg["callout"] = CALLOUT_LEASE4_RECOVER;

    handle.getArgument("lease4", lease);
    extract_lease4(msg, lease);

    /* Send data to socket */
    int ret;
    ret = send_socket_data(msg);
    return 0;
  }
}