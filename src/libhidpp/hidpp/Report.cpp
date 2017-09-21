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

#include <hidpp/Report.h>

#include <hidpp10/defs.h>
#include <hidpp20/defs.h>
#include <algorithm>

using namespace HIDPP;

static const std::array<uint8_t, 27> ShortReportDesc = {
	0x06, 0x00, 0xFF,	// Usage Page (FF00 - Vendor)
	0x09, 0x01,		// Usage (0001 - Vendor)
	0xA1, 0x01,		// Collection (Application)
	0x85, 0x10,		//   Report ID (16)
	0x75, 0x08,		//   Report Size (8)
	0x95, 0x06,		//   Report Count (6)
	0x15, 0x00,		//   Logical Minimum (0)
	0x26, 0xFF, 0x00,	//   Logical Maximum (255)
	0x09, 0x01,		//   Usage (0001 - Vendor)
	0x81, 0x00,		//   Input (Data, Array, Absolute)
	0x09, 0x01,		//   Usage (0001 - Vendor)
	0x91, 0x00,		//   Output (Data, Array, Absolute)
	0xC0			// End Collection
};

static const std::array<uint8_t, 27> LongReportDesc = {
	0x06, 0x00, 0xFF,	// Usage Page (FF00 - Vendor)
	0x09, 0x02,		// Usage (0002 - Vendor)
	0xA1, 0x01,		// Collection (Application)
	0x85, 0x11,		//   Report ID (17)
	0x75, 0x08,		//   Report Size (8)
	0x95, 0x13,		//   Report Count (19)
	0x15, 0x00,		//   Logical Minimum (0)
	0x26, 0xFF, 0x00,	//   Logical Maximum (255)
	0x09, 0x02,		//   Usage (0002 - Vendor)
	0x81, 0x00,		//   Input (Data, Array, Absolute)
	0x09, 0x02,		//   Usage (0002 - Vendor)
	0x91, 0x00,		//   Output (Data, Array, Absolute)
	0xC0			// End Collection
};

/* Alternative versions from the G602 */
static const std::array<uint8_t, 27> ShortReportDesc2 = {
	0x06, 0x00, 0xFF,	// Usage Page (FF00 - Vendor)
	0x09, 0x01,		// Usage (0001 - Vendor)
	0xA1, 0x01,		// Collection (Application)
	0x85, 0x10,		//   Report ID (16)
	0x95, 0x06,		//   Report Count (6)
	0x75, 0x08,		//   Report Size (8)
	0x15, 0x00,		//   Logical Minimum (0)
	0x26, 0xFF, 0x00,	//   Logical Maximum (255)
	0x09, 0x01,		//   Usage (0001 - Vendor)
	0x81, 0x00,		//   Input (Data, Array, Absolute)
	0x09, 0x01,		//   Usage (0001 - Vendor)
	0x91, 0x00,		//   Output (Data, Array, Absolute)
	0xC0			// End Collection
};

static const std::array<uint8_t, 27> LongReportDesc2 = {
	0x06, 0x00, 0xFF,	// Usage Page (FF00 - Vendor)
	0x09, 0x02,		// Usage (0002 - Vendor)
	0xA1, 0x01,		// Collection (Application)
	0x85, 0x11,		//   Report ID (17)
	0x95, 0x13,		//   Report Count (19)
	0x75, 0x08,		//   Report Size (8)
	0x15, 0x00,		//   Logical Minimum (0)
	0x26, 0xFF, 0x00,	//   Logical Maximum (255)
	0x09, 0x02,		//   Usage (0002 - Vendor)
	0x81, 0x00,		//   Input (Data, Array, Absolute)
	0x09, 0x02,		//   Usage (0002 - Vendor)
	0x91, 0x00,		//   Output (Data, Array, Absolute)
	0xC0			// End Collection
};

template<typename Container1, typename Container2>
static inline bool contains (const Container1 &cont, const Container2 &seq)
{
	return cont.end () != std::search (cont.begin (), cont.end (), seq.begin (), seq.end ());
}

bool HIDPP::checkReportDescriptor (const std::vector<uint8_t> &rdesc)
{
	return (contains (rdesc, ShortReportDesc) || contains (rdesc, ShortReportDesc2)) &&
		(contains (rdesc, LongReportDesc) || contains (rdesc, LongReportDesc2));
}

Report::InvalidReportID::InvalidReportID ()
{
}

const char *Report::InvalidReportID::what () const noexcept
{
	return "Invalid Report ID for a HID++ report";
}

Report::InvalidReportLength::InvalidReportLength ()
{
}

const char *Report::InvalidReportLength::what () const noexcept
{
	return "Invalid Report Length for a HID++ report";
}

namespace Offset
{
static constexpr unsigned int Type = 0;
static constexpr unsigned int DeviceIndex = 1;
static constexpr unsigned int SubID = 2;
static constexpr unsigned int Address = 3;
static constexpr unsigned int Parameters = 4;
}

Report::Report (uint8_t type, const uint8_t *data, std::size_t length)
{
	switch (static_cast<Type> (type)) {
	case Short:
		_data.resize (HeaderLength+ShortParamLength);
		break;
	case Long:
		_data.resize (HeaderLength+LongParamLength);
		break;
	default:
		throw InvalidReportID ();
	}
	if (length != _data.size ()-1)
		throw InvalidReportLength ();

	_data[Offset::Type] = type;
	std::copy_n (data, length, &_data[1]);
}

Report::Report (std::vector<uint8_t> &&data)
{
	std::size_t expected_len;
	switch (static_cast<Type> (data[0])) {
	case Short:
		expected_len = HeaderLength+ShortParamLength;
		break;
	case Long:
		expected_len = HeaderLength+LongParamLength;
		break;
	default:
		throw InvalidReportID ();
	}
	if (data.size () != expected_len)
		throw InvalidReportLength ();
	_data = std::move (data);
}

Report::Report (Type type,
		HIDPP::DeviceIndex device_index,
		uint8_t sub_id,
		uint8_t address)
{
	switch (type) {
	case Short:
		_data.resize (HeaderLength+ShortParamLength, 0);
		break;
	case Long:
		_data.resize (HeaderLength+LongParamLength, 0);
		break;
	}
	_data[Offset::Type] = type;
	_data[Offset::DeviceIndex] = device_index;
	_data[Offset::SubID] = sub_id;
	_data[Offset::Address] = address;
}

Report::Report (HIDPP::DeviceIndex device_index,
		uint8_t sub_id,
		uint8_t address,
		std::vector<uint8_t>::const_iterator param_begin,
		std::vector<uint8_t>::const_iterator param_end)
{
	switch (std::distance (param_begin, param_end)) {
	case ShortParamLength:
		_data.resize (HeaderLength+ShortParamLength);
		_data[Offset::Type] = Short;
		break;
	case LongParamLength:
		_data.resize (HeaderLength+LongParamLength);
		_data[Offset::Type] = Long;
		break;
	default:
		throw InvalidReportLength ();
	}
	_data[Offset::DeviceIndex] = device_index;
	_data[Offset::SubID] = sub_id;
	_data[Offset::Address] = address;
	std::copy (param_begin, param_end, &_data[Offset::Parameters]);
}

Report::Report (Type type,
		DeviceIndex device_index,
		uint8_t feature_index,
		unsigned int function,
		unsigned int sw_id)
{
	switch (type) {
	case Short:
		_data.resize (HeaderLength+ShortParamLength, 0);
		break;
	case Long:
		_data.resize (HeaderLength+LongParamLength, 0);
		break;
	}
	_data[Offset::Type] = type;
	_data[Offset::DeviceIndex] = device_index;
	_data[Offset::SubID] = feature_index;
	_data[Offset::Address] = (function & 0x0f) << 4 | (sw_id & 0x0f);
}

Report::Report (DeviceIndex device_index,
		uint8_t feature_index,
		unsigned int function,
		unsigned int sw_id,
		std::vector<uint8_t>::const_iterator param_begin,
		std::vector<uint8_t>::const_iterator param_end)
{
	switch (std::distance (param_begin, param_end)) {
	case ShortParamLength:
		_data.resize (HeaderLength+ShortParamLength);
		_data[Offset::Type] = Short;
		break;
	case LongParamLength:
		_data.resize (HeaderLength+LongParamLength);
		_data[Offset::Type] = Long;
		break;
	default:
		throw InvalidReportLength ();
	}
	_data[Offset::DeviceIndex] = device_index;
	_data[Offset::SubID] = feature_index;
	_data[Offset::Address] = (function & 0x0f) << 4 | (sw_id & 0x0f);
	std::copy (param_begin, param_end, &_data[Offset::Parameters]);
}

Report::Type Report::type () const
{
	return static_cast<Type> (_data[Offset::Type]);
}

DeviceIndex Report::deviceIndex () const
{
	return static_cast<HIDPP::DeviceIndex> (_data[Offset::DeviceIndex]);
}

uint8_t Report::subID () const
{
	return _data[Offset::SubID];
}

void Report::setSubID (uint8_t sub_id)
{
	_data[Offset::SubID] = sub_id;
}

uint8_t Report::address () const
{
	return _data[Offset::Address];
}

void Report::setAddress (uint8_t address)
{
	_data[Offset::Address] = address;
}

uint8_t Report::featureIndex () const
{
	return _data[Offset::SubID];
}

void Report::setFeatureIndex (uint8_t feature_index)
{
	_data[Offset::SubID] = feature_index;
}

unsigned int Report::function () const
{
	return (_data[Offset::Address] & 0xf0) >> 4;
}

void Report::setFunction (unsigned int function)
{
	_data[Offset::Address] = (function & 0x0f) << 4 | (_data[Offset::Address] & 0x0f);
}

unsigned int Report::softwareID () const
{
	return _data[Offset::Address] & 0x0f;
}

void Report::setSoftwareID (unsigned int sw_id)
{
	_data[Offset::Address] = (_data[Offset::Address] & 0xf0) | (sw_id & 0x0f);
}

std::size_t Report::parameterLength () const
{
	return parameterLength (static_cast<Type> (_data[Offset::Type]));
}

std::size_t Report::parameterLength (Type type)
{
	switch (type) {
	case Short:
		return ShortParamLength;
	case Long:
		return LongParamLength;
	default:
		throw std::logic_error ("Invalid type");
	}
}

std::vector<uint8_t>::iterator Report::parameterBegin ()
{
	return _data.begin () + Offset::Parameters;
}

std::vector<uint8_t>::const_iterator Report::parameterBegin () const
{
	return _data.begin () + Offset::Parameters;
}

std::vector<uint8_t>::iterator Report::parameterEnd ()
{
	return _data.end ();
}

std::vector<uint8_t>::const_iterator Report::parameterEnd () const
{
	return _data.end ();
}

const std::vector<uint8_t> &Report::rawReport () const
{
	return _data;
}

bool Report::checkErrorMessage10 (uint8_t *sub_id,
				  uint8_t *address,
				  uint8_t *error_code) const
{
	if (static_cast<Type> (_data[Offset::Type]) != Short ||
	    _data[Offset::SubID] != HIDPP10::ErrorMessage)
		return false;

	if (sub_id)
		*sub_id = _data[3];
	if (address)
		*address = _data[4];
	if (error_code)
		*error_code = _data[5];
	return true;
}

bool Report::checkErrorMessage20 (uint8_t *feature_index,
				  unsigned int *function,
				  unsigned int *sw_id,
				  uint8_t *error_code) const
{
	if (static_cast<Type> (_data[Offset::Type]) != Long ||
	    _data[Offset::SubID] != HIDPP20::ErrorMessage)
		return false;

	if (feature_index)
		*feature_index = _data[3];
	if (function)
		*function = (_data[4] & 0xF0) >> 4;
	if (sw_id)
		*sw_id = _data[4] & 0x0F;
	if (error_code)
		*error_code = _data[5];
	return true;
}

