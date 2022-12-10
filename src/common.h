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


#ifndef KEA_HOOK_GLUBNG_COMMON
#define KEA_HOOK_GLUBNG_COMMON

#define CALLOUT_LEASE4_SELECT 1
#define CALLOUT_LEASE4_RENEW 2
#define CALLOUT_LEASE4_RELEASE 3
#define CALLOUT_LEASE4_DECLINE 4
#define CALLOUT_LEASE4_EXPIRE 5
#define CALLOUT_LEASE4_RECOVER 6
#define CALLOUT_PKT4_CIRCUIT_ID 7

#include <string>

extern "C" {

/* From load.cc */

/* Socket address (filesystem). */
extern std::string socket_name;

}

#endif