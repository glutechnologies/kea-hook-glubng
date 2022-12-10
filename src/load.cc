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

#include <signal.h>
#include <hooks/hooks.h>

#include "logger.h"
#include "common.h"

using namespace isc::hooks;
using namespace isc::data;

/* Socket address (filesystem) */
std::string socket_name;

extern "C" {

int load(LibraryHandle& handle) {
    ConstElementPtr socket = handle.getParameter("socket");
    if (!socket) {
        LOG_ERROR(glubng_logger, GLUBNG_MISSING_PARAM).arg("socket");
        return 1;
    }
    if (socket->getType() != Element::string) {
        LOG_ERROR(glubng_logger, GLUBNG_MISTYPED_PARAM).arg("socket");
        return 1;
    }
    socket_name = socket->stringValue();

    return 0;
}

} // end extern "C"
