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

#include <cstddef>
#include <log/message_types.h>
#include <log/message_initializer.h>

extern const isc::log::MessageID GLUBNG_MISSING_PARAM = "GLUBNG_MISSING_PARAM";
extern const isc::log::MessageID GLUBNG_MISTYPED_PARAM = "GLUBNG_MISTYPED_PARAM";
extern const isc::log::MessageID GLUBNG_SOCKET_EXCEPTION = "GLUBNG_SOCKET_EXCEPTION";

namespace {

const char* values[] = {
    "GLUBNG_MISSING_PARAM", "required parameter \"%1\" missing in configuration",
    "GLUBNG_MISTYPED_PARAM", "parameter \"%1\" in configuration has wrong type",
    "GLUBNG_SOCKET_EXCEPTION", "exception occurred in unidirectional socket to glubngd: \"%1\"",
    NULL
};

const isc::log::MessageInitializer initializer(values);

} // Anonymous namespace

