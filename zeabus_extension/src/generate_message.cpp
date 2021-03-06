/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: generate_message.cpp
//
//	Last Update		: Sep 01 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: Detail Function of generate_message.cpp
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_GENERATE_MESSAGE
	#include	<zeabus_extension/generate_message.h>
	#define ZEABUS_EXTENSION_GENERATE_MESSAGE
#endif

std::string zeabus_extension::generate_message::specific_message(	int number_line 
											,	std::string* message 
											,	std::string* detail){

	std::string answer = "";

	for( int run = 0 ; run < number_line ; run++){
		answer += message[run];
		answer += detail[run];
		if( run == number_line -1 ) break;
		answer += "\n";
	}

	return answer;
}

zeabus_extension::generate_message::log_message::log_message( int number_line 
										,	std::string* message ){
	setup_begin( message , number_line);
}

void zeabus_extension::generate_message::log_message::setup_begin( std::string* message 
												,int number_line ){
	if( number_line != 0 ) this->number_line = number_line;
	if( message != nullptr ){
		this->begin_line = new std::string[number_line];
		for( int run = 0 ; run < number_line ; run++){
			this->begin_line[run] = message[run];
		}
	}
}

std::string zeabus_extension::generate_message::log_message::message( std::string* data){
	return generate_message::specific_message( this->number_line
											,  this->begin_line
											,  data);
	
}
