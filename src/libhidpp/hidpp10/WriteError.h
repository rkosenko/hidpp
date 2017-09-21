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

#ifndef LIBHIDPP_HIDPP10_WRITE_ERROR_H
#define LIBHIDPP_HIDPP10_WRITE_ERROR_H

#include <stdexcept>
#include <cstdint>

namespace HIDPP10
{

class WriteError: public std::exception
{
public:
	WriteError (uint8_t error_code);

	virtual const char *what () const noexcept;

private:
	uint8_t _error_code;
	std::string _error_message;
};

}

#endif
