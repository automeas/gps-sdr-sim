#include "gpssim.h"
//
////
//// Copyright 2011-2012,2014 Ettus Research LLC
////
//// This program is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////
//
//#include <uhd/types/tune_request.hpp>
//#include <uhd/utils/thread_priority.hpp>
//#include <uhd/utils/safe_main.hpp>
//#include <uhd/usrp/multi_usrp.hpp>
//#include <boost/program_options.hpp>
//#include <boost/format.hpp>
//#include <boost/thread.hpp>
//#include <iostream>
//#include <fstream>
//#include <complex>
//#include <csignal>
//
//int main(int argc, char *argv[])
//{
//	char *argstr[] = { "", "-v", "-e", "brdc3540.14n", "-b", "16", "-s", "2500000", "-l", "34.77444,133.72611,100", "-d", "10" };
//	//argv[0] = &argstr[0];
//	boost::thread t(v_main, 12, argstr);
//	boost::thread t_consumer(v_comsumer);
//	t.join();
//	//t_consumer.join();
//	//printf("done\n");
//	//fifo_benchmark(1000000);
//	return 0;
//}

//
// Copyright 2011-2012,2014 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <uhd/types/tune_request.hpp>
#include <uhd/utils/thread_priority.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/gregorian/gregorian.hpp> 
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <iostream>
#include <fstream>
#include <complex>
#include <csignal>
#include <cmath>
#define BOOST_DATE_TIME_SOURCE  

namespace po = boost::program_options;

static bool stop_signal_called = false;
void sig_int_handler(int){ stop_signal_called = true; }

void send_from_queue(
	uhd::usrp::multi_usrp::sptr usrp,
	const std::string &cpu_format,
	const std::string &wire_format,
	size_t samps_per_buff
	){

	//create a transmit streamer
	uhd::stream_args_t stream_args(cpu_format, wire_format);
	uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(stream_args);

	uhd::tx_metadata_t md;
	md.start_of_burst = false;
	md.end_of_burst = false;

	//std::ifstream infile(file.c_str(), std::ifstream::binary);

	//loop until the entire file has been read
	vector<frame> frame_buffer(250000);
	while (not stop_signal_called){

		if (frame_queue.size()>2){
			tx_stream->send(&(frame_queue.front().front()), 250000, md);
			frame_queue.pop();
		}
		boost::thread::yield();
	}

}

int UHD_SAFE_MAIN(int argc, char *argv[]){
	uhd::set_thread_priority_safe();

	//variables to be set by po
	std::string args, ant, subdev, ref, wirefmt;
	size_t spb;
	double rate, freq, gain, bw, delay, lo_off;

	//setup the program options
	rate = 2500000; //default
	freq = 1575420000; //default
	gain = 80; //default
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "help message")
		("args", po::value<std::string>(&args)->default_value("master_clock_rate=50e6"), "multi uhd device address args")
		("spb", po::value<size_t>(&spb)->default_value(10000), "samples per buffer")
		("rate", po::value<double>(&rate), "rate of outgoing samples")
		("freq", po::value<double>(&freq), "RF center frequency in Hz")
		("lo_off", po::value<double>(&lo_off), "Offset for frontend LO in Hz (optional)")
		("gain", po::value<double>(&gain), "gain for the RF chain")
		("ant", po::value<std::string>(&ant), "antenna selection")
		("subdev", po::value<std::string>(&subdev), "subdevice specification")
		("bw", po::value<double>(&bw), "analog frontend filter bandwidth in Hz")
		("ref", po::value<std::string>(&ref)->default_value("internal"), "reference source (internal, external, mimo)")
		("wirefmt", po::value<std::string>(&wirefmt)->default_value("sc16"), "wire format (sc8 or sc16)")
		("delay", po::value<double>(&delay)->default_value(0.0), "specify a delay between repeated transmission of file")
		("repeat", "repeatedly transmit file")
		("int-n", "tune USRP with integer-n tuning")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	//print the help message
	if (vm.count("help")){
		std::cout << boost::format("UHD TX samples from file %s") % desc << std::endl;
		return ~0;
	}

	bool repeat = vm.count("repeat") > 0;

	//create a usrp device
	std::cout << std::endl;
	std::cout << boost::format("Creating the usrp device with: %s...") % args << std::endl;
	uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(args);

	//Lock mboard clocks
	usrp->set_clock_source(ref);

	//always select the subdevice first, the channel mapping affects the other settings
	if (vm.count("subdev")) usrp->set_tx_subdev_spec(subdev);

	std::cout << boost::format("Using Device: %s") % usrp->get_pp_string() << std::endl;

	std::cout << boost::format("Setting TX Rate: %f Msps...") % (rate / 1e6) << std::endl;
	usrp->set_tx_rate(rate);
	std::cout << boost::format("Actual TX Rate: %f Msps...") % (usrp->get_tx_rate() / 1e6) << std::endl << std::endl;

	std::cout << boost::format("Setting TX Freq: %f MHz...") % (freq / 1e6) << std::endl;
	uhd::tune_request_t tune_request;
	if (vm.count("lo_off")) tune_request = uhd::tune_request_t(freq, lo_off);
	else tune_request = uhd::tune_request_t(freq);
	if (vm.count("int-n")) tune_request.args = uhd::device_addr_t("mode_n=integer");
	usrp->set_tx_freq(tune_request);
	std::cout << boost::format("Actual TX Freq: %f MHz...") % (usrp->get_tx_freq() / 1e6) << std::endl << std::endl;

	//set the rf gain
	std::cout << boost::format("Setting TX Gain: %f dB...") % gain << std::endl;
	usrp->set_tx_gain(gain);
	std::cout << boost::format("Actual TX Gain: %f dB...") % usrp->get_tx_gain() << std::endl << std::endl;

	//set the analog frontend filter bandwidth
	if (vm.count("bw")){
		std::cout << boost::format("Setting TX Bandwidth: %f MHz...") % bw << std::endl;
		usrp->set_tx_bandwidth(bw);
		std::cout << boost::format("Actual TX Bandwidth: %f MHz...") % usrp->get_tx_bandwidth() << std::endl << std::endl;
	}

	//set the antenna
	if (vm.count("ant")) usrp->set_tx_antenna(ant);

	boost::this_thread::sleep(boost::posix_time::seconds(1)); //allow for some setup time

	//Check Ref and LO Lock detect
	std::vector<std::string> sensor_names;
	sensor_names = usrp->get_tx_sensor_names(0);
	if (std::find(sensor_names.begin(), sensor_names.end(), "lo_locked") != sensor_names.end()) {
		uhd::sensor_value_t lo_locked = usrp->get_tx_sensor("lo_locked", 0);
		std::cout << boost::format("Checking TX: %s ...") % lo_locked.to_pp_string() << std::endl;
		UHD_ASSERT_THROW(lo_locked.to_bool());
	}
	sensor_names = usrp->get_mboard_sensor_names(0);
	if ((ref == "mimo") and(std::find(sensor_names.begin(), sensor_names.end(), "mimo_locked") != sensor_names.end())) {
		uhd::sensor_value_t mimo_locked = usrp->get_mboard_sensor("mimo_locked", 0);
		std::cout << boost::format("Checking TX: %s ...") % mimo_locked.to_pp_string() << std::endl;
		UHD_ASSERT_THROW(mimo_locked.to_bool());
	}
	if ((ref == "external") and(std::find(sensor_names.begin(), sensor_names.end(), "ref_locked") != sensor_names.end())) {
		uhd::sensor_value_t ref_locked = usrp->get_mboard_sensor("ref_locked", 0);
		std::cout << boost::format("Checking TX: %s ...") % ref_locked.to_pp_string() << std::endl;
		UHD_ASSERT_THROW(ref_locked.to_bool());
	}

	//set sigint if user wants to receive
	if (repeat){
		std::signal(SIGINT, &sig_int_handler);
		std::cout << "Press Ctrl + C to stop streaming..." << std::endl;
	}


	//file the buffer
	char *argstr[] = { "", "-e", "brdc0720.17n", "-b", "16", "-s", "2500000", "-l", "34.8097531308,113.5292048967,50", "-d", "1", "-T", "now" };
	boost::thread t(v_main, 13, argstr);
	boost::thread t_input(keyboard_input);

	//send from queue
	send_from_queue(usrp, "sc16", wirefmt, spb);


	//finished
	std::cout << std::endl << "Done!" << std::endl << std::endl;

	return EXIT_SUCCESS;
}
