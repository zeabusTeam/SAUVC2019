/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: manage_port.h
//
//	Create on		: Oct 05 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: for using manage port
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	BOOST_ASIO_HPP
	#include <boost/asio.hpp> // this is top of serial port I include all
	#define BOOST_ASIO_HPP
#endif

#ifndef IOSTREAM
	#include	<iostream>
	#define BOOST_ASIO_HPP
#endif

namespace zeabus_extension{
namespace manage_port{
	
	class specific_port : private boost::noncopyable{

		public:
			specific_port(  std::string name_port = "" ); //function init
			void set_name_port( std::string name_port ); //set name of port
			void open_port( std::string name_device = ""); // open port
			void close_port(); // we will close port now
			bool is_open(); // check now port are open or not
			

		protected:
			// order is importance service must build first
			boost::asio::io_service io_service;
			boost::asio::serial_port io_port;
			boost::asio::serial_port_base::baud_rate io_baudrate;
			boost::asio::serial_port_base::flow_control io_flow_control;
			boost::asio::serial_port_base::parity io_parity;
			boost::asio::serial_port_base::stop_bits io_stop_bits;
			boost::asio::serial_port_base::character_size io_character_size;

		private:
			std::string name_port; // collect name of port to open example is /dev/ttys0
	};

}
}
