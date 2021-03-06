/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: convert_to_string.cpp
//
//	Last Update		: Aug 24 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	:	for convert type to type
//
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ZEABUS_EXTENSION_CONVERT_TO_STRING
	#include	<zeabus_extension/convert_to_string.h> // include standard function of c
	#define ZEABUS_EXTENSION_CONVERT_TO_STRING
#endif
/*
template< typename type_variable > std::string to_string( type_variable data){
	std::ostringstream temporary;
	temporary << data;
	return temporary.str();
}
*/
std::string zeabus_extension::convert::to_string( boost::posix_time::ptime data ){
	std::ostringstream temporary;
	temporary << data;
	return temporary.str();
}

std::string zeabus_extension::convert::to_string( boost::gregorian::date data ){
	std::ostringstream temporary;
	temporary << data;
	return temporary.str();
}

std::string zeabus_extension::convert::to_string( int data , bool assign_position , int position , bool sign ){
	std::ostringstream temporary;
	temporary << data;
	if( ! assign_position ) return temporary.str();
	else{
		int count = 0;
		std::string problem = temporary.str();
		std::string answer = "";
		bool sign_plus = true;
		for( int run = 0 ; ; run++){
			try{
				if( problem[run] == '+' ) sign_plus = true;
				else if( problem[run] == '-' ) sign_plus = false;
				else if( problem[run] != '\0' && problem[run] != '\n'){
					answer += to_string( problem[run]);
					count++; 
				}
				else break;
			}
			catch( std::exception& error_msg){
				std::cout << "Standard Exception : " << error_msg.what() << "\n";
			}
		}
		std::cout << "In put have all char is " << count << "\n";
		for( ; count < position ; count++){
			answer = "0" + answer;
		}
		if( sign ){
			if( sign_plus ) answer = "+" + answer;
			else answer = "-" + answer;
		}
		std::cout << "Out put is answer :" << answer << "\n";
		return answer;
	}
}

std::string zeabus_extension::convert::to_string( float data ){
	std::ostringstream temporary;
	temporary << data;
	return temporary.str();
}

std::string zeabus_extension::convert::to_string( char data ){
	std::ostringstream temporary;
	temporary << data;
	return temporary.str();
}

// this function have more detail to make you can convert double to string 
// and you can design about number of position in before and after point
std::string zeabus_extension::convert::to_string( double data , bool assign_position  
								, bool have_point , int front , int back){
	std::ostringstream temporary;
	temporary << data;
	// if you don't want to assign that. It will return by default of ostringstream
	if( ! assign_position ) return temporary.str();
	else{
		if( ! have_point ){
			std::cout << "Should put seconde parameter is false or don't put\n";
		}
		bool mode = true;
		bool sign = true;
		int count_front = 0;
		int count_back = 0;
		std::string answer = "";
		std::string problem = temporary.str();
		if( problem[0] == '-') sign = false;
		else sign = true;
		for( int run = 0 ; ; run++){
			try{
				#ifdef convert_test
					std::cout << "Consider on " << to_string( problem[run]) << "\n";
				#endif
				if( problem[run] == '-' || problem[run] == '+') continue;
				else if( problem[run] == '.' ) mode = false;
				else if( (problem[run] != '\n' || problem[run] != '\0' ) 
								&& is_num( problem[run] ) ) {
					if( mode ){
						#ifdef convert_test
							std::cout << "In mode consider on \'" <<( problem[run]) << "\'\n";
						#endif

						if( count_front == front){
							std::cout << "Over limit of front\n";
							count_front = front;
							std::exit( -1 );
						}
						else{
							#ifdef convert_test
								std::cout << "add front " << to_string( problem[run]) << "\n";
							#endif
							answer = answer + to_string( problem[run]);
							count_front++;
						}
					}
					else{
						count_back++;
						if( count_back > back){
							std::cout << "Over limit of back\n";
							count_back = back;
							break;
						}
						else{
							answer = answer + to_string( problem[run]);
						}
					}
				}
				else break;
			}
			catch( std::exception& error_msg){
				std::cout << "Standard Exception : " << error_msg.what() << "\n";
			}
		}
		for( ; count_front != front ; count_front++) answer = "0"+answer;
		for( ; count_back != back ; count_back++) answer = answer+"0";
		if( sign ) answer = "+" + answer ;
		else answer = "-" + answer;
		return answer;
	}
}

// this function will edit space to key as you want example change '-' , '*' to '_'
std::string zeabus_extension::convert::edit_space( std::string data , std::string key){
	int order = 0;
	std::string answer = "";
	while( true ){
// if that is char to tell new line or end of string that will break	 
// that mean this function can edit one line only
		if( data[order] == '\n' || data[order] == '\0' ) break;
// if that cahr have in syntax will change to key
		else if( in_set( data[order] ) ) answer += key;
// if not will + same char
		else answer += to_string( data[order] );
		order++;
	}
	return answer;
}
// this function will check data input have in set of syntax or not
bool zeabus_extension::convert::in_set( char data){
	int run = 0;
	for( run = 0 ; run < 7 ; run++){
		if( data == set_syntax[run]) return true;
	}
	return false;
}

// this function will check data input it is number or not
bool zeabus_extension::convert::is_num( char data){
	int run = 0;
	for( run = 0 ; run < 10 ; run++){
		if( data == set_number[run]){
// for test in function but now finish don't use 2 line under
			//std::cout << "cach data is " << (int)data << " same with " << run << "\n";
			//std::cout << "return true for data \'" << data << "\'\n";
			return true;
		}
	}
	return false;
}
