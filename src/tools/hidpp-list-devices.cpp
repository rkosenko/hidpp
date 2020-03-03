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

#include <cstdio>
#include <cstring>

#include <misc/Log.h>
#include <hid/DeviceMonitor.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp/Device.h>
#include <hidpp10/Error.h>
#include <hidpp20/Error.h>

#include "common/common.h"
#include "common/Option.h"
#include "common/CommonOptions.h"

class DevicePrinter: public HID::DeviceMonitor
{
protected:
	void addDevice (const char *path)
	{
		try {
			HIDPP::SimpleDispatcher dispatcher (path);
			bool has_receiver_index = false;
			for (HIDPP::DeviceIndex index: {
					HIDPP::DefaultDevice,
					HIDPP::CordedDevice,
					HIDPP::WirelessDevice1,
					HIDPP::WirelessDevice2,
					HIDPP::WirelessDevice3,
					HIDPP::WirelessDevice4,
					HIDPP::WirelessDevice5,
					HIDPP::WirelessDevice6 }) {
				// Skip wireless devices, if the default index (used by the receiver) already failed.
				if (!has_receiver_index && index == HIDPP::WirelessDevice1)
					break;
				try {
					HIDPP::Device dev (&dispatcher, index);
					auto version = dev.protocolVersion ();
					printf ("%s", path);
					if (index != HIDPP::DefaultDevice)
						printf (" (device %d)", index);
					printf (": %s (%04hx:%04hx) HID++ %d.%d\n",
							dev.name ().c_str (),
							dispatcher.hidraw ().vendorID (), dev.productID (),
							std::get<0> (version), std::get<1> (version));
					if (index == HIDPP::DefaultDevice && version == std::make_tuple (1, 0))
						has_receiver_index = true;
				}
				catch (HIDPP10::Error &e) {
					if (e.errorCode () != HIDPP10::Error::UnknownDevice && e.errorCode () != HIDPP10::Error::InvalidSubID) {
						Log::error ().printf ("Error while querying %s wireless device %d: %s\n",
								      path, index, e.what ());
					}
				}
				catch (HIDPP20::Error &e) {
					if (e.errorCode () != HIDPP20::Error::UnknownDevice) {
						Log::error ().printf ("Error while querying %s device %d: %s\n",
								      path, index, e.what ());
					}
				}
				catch (HIDPP::Dispatcher::TimeoutError &e) {
					Log::warning ().printf ("Device %s (index %d) timed out\n",
								path, index);
				}
			}

		}
		catch (HIDPP::Dispatcher::NoHIDPPReportException &e) {
		}
		catch (std::system_error &e) {
			Log::warning ().printf ("Failed to open %s: %s\n", path, e.what ());
		}
	}

	void removeDevice (const char *path) { }
};

int main (int argc, char *argv[])
{
	std::vector<Option> options = {
		VerboseOption (),
	};
	Option help = HelpOption (argv[0], "", &options);
	options.push_back (help);

	int first_arg;
	if (!Option::processOptions (argc, argv, options, first_arg))
		return EXIT_FAILURE;

	if (argc-first_arg != 0) {
		fprintf (stderr, "%s", getUsage (argv[0], "", &options).c_str ());
		return EXIT_FAILURE;
	}

	(DevicePrinter ()).enumerate ();

	return 0;
}

