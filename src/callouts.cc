/* 
 * This file is part of the GluBNG distribution (https://github.com/glutechnologies/kea-hook-glubng).
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
#include <dhcpsrv/lease_mgr.h>
#include <dhcpsrv/lease_mgr_factory.h>
#include <dhcpsrv/lease.h>
#include <dhcpsrv/host.h>
#include <cc/data.h>

#include "nlohmann/json.hpp"
#include "common.h"

#include <string>
#include <vector>

#include "socket.h"
#include "logger.h"

using namespace isc::dhcp;
using namespace isc::hooks;
using namespace isc::data;

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

  /// @brief Get DHCPv4 extended info.
  ///
  /// @param lease The lease to get extended info from.
  /// @return The extended info or null.
  inline static ConstElementPtr get_extended_info4(const Lease4Ptr& lease) {
    ConstElementPtr user_context = lease->getContext();
    if (!user_context || (user_context->getType() != Element::map)) {
      return (ConstElementPtr());
    }
    ConstElementPtr isc = user_context->get("ISC");
    if (!isc || (isc->getType() != Element::map)) {
      return (ConstElementPtr());
    }
    return (isc->get("relay-agent-info"));
  }

  // https://www.rfc-editor.org/rfc/rfc3046
  uint32_t parse_cid_from_string(const std::string& rai) {
    std::string::const_iterator it = rai.begin();
    OptionBuffer buf;
    // Parse only cid
    std::string cid_bytes_str(it+4, it+6);
    int cid_bytes = std::stoi(cid_bytes_str, nullptr, 16);
    LOG_INFO(glubng_logger, "ret cid_bytes %1").arg(cid_bytes);
    std::string cid_str(it+6, it+6+(cid_bytes*2));
    uint32_t ret = std::stoi(cid_str, nullptr, 16);
    LOG_INFO(glubng_logger, "ret cid %1").arg(cid_str);
    return ret;
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

  int host4_identifier(CalloutHandle& handle) {
    std::string flex_id_str;
    Pkt4Ptr query;
    handle.getArgument("query4", query);

    nlohmann::json msg, res;
    msg["callout"] = CALLOUT_PKT4_CIRCUIT_ID;

    // Process query
    extract_pkt4(msg, query);

    try {
      if (msg["query"]["option82-circuit-id"].empty()) {
        // Get all leases for given HW (MAC)
        Lease4Collection leases4 = LeaseMgrFactory::instance().getLease4(HWAddr::fromText(msg["query"]["hw-addr"]));
        // Usually only one lease, take first
        if (!leases4.empty()) {
          Lease4Ptr lease = leases4.front();
          ConstElementPtr relay = get_extended_info4(lease);
          if (relay) {
            uint32_t cid = parse_cid_from_string(relay->stringValue());
            std::stringstream stream;
            // Send circuit-id in hex as string (JSON)
            stream << "0x" << std::setfill('0') << std::setw(sizeof(uint32_t)*2) << std::hex << cid;
            msg["query"]["option82-circuit-id"] = stream.str();
          }
        }
      }

      // Send data to socket
      int ret;
      ret = send_socket_data_receive(msg, true, res);

      if (!res["flex-id"].empty()) {
        // Store flex-id from GluBNGd
        flex_id_str = res["flex-id"].get<std::string>();
        LOG_INFO(glubng_logger, "flex-id host4 %1").arg(flex_id_str);
        OptionBuffer flex_id(flex_id_str.begin(), flex_id_str.end());
        handle.setArgument("id_value", flex_id);
      }
    } catch (std::exception &e) {
      LOG_WARN(glubng_logger, "processing rai from user-context %1").arg(e.what());
    }
    return 0;
  }
}