/*
 * Copyright 2015 Clément Vuchener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LIBHIDPP_HIDPP20_DEVICE_H
#define LIBHIDPP_HIDPP20_DEVICE_H

#include <hidpp/Device.h>

namespace HIDPP { class Dispatcher; }

namespace HIDPP20 {

class Device: public HIDPP::Device
{
public:
	static unsigned int softwareID;
	Device (HIDPP::Dispatcher *dispatcher, HIDPP::DeviceIndex device_index = HIDPP::DefaultDevice);
	Device (HIDPP::Device &&other);

	std::vector<uint8_t> callFunction (uint8_t feature_index,
					   unsigned int function,
					   std::vector<uint8_t>::const_iterator param_begin,
					   std::vector<uint8_t>::const_iterator param_end);

	inline std::vector<uint8_t> callFunction (uint8_t feature_index,
						  unsigned int function,
						  const std::vector<uint8_t> params = {})
	{
		return callFunction (feature_index, function, params.begin (), params.end ());
	}
};

}

#endif
