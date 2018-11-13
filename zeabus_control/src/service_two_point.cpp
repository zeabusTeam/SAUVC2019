////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_two_point.cpp 
//	Purpose		: for main_service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 18
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<zeabus_control/two_point.h>

#include	"service_control.cpp"
//#define _CHECK_ERROR_

#ifndef _service_two_point_cpp__
#define _service_two_point_cpp__

namespace zeabus_control{

	class two_point_service: public main_service{
		public:
			two_point_service ( double* current_state , double* target_state 
						, double* robot_error , double * ok_error ) :
							main_service( current_state , target_state 
											, robot_error , ok_error ){}
			bool call_relative_xy( two_point::Request &req ,
								 two_point::Response &res );
			bool call_absolute_xy( two_point::Request &req ,
								 two_point::Response &res );
	
	};

	bool two_point_service::call_relative_xy( two_point::Request &req ,
								 two_point::Response &res ){
			this->target_state[0] += req.point_1 * cos( target_state[5]);
			this->target_state[1] += req.point_1 * sin( target_state[5]);
			this->target_state[0] += req.point_2 * cos( target_state[5] + PI/2);
			this->target_state[1] += req.point_2 * sin( target_state[5] + PI/2);
			res.success = true;
			return true;
	}

	bool two_point_service::call_absolute_xy( two_point::Request &req ,
								two_point::Response &res ){
			this->target_state[0] = req.point_1;
			this->target_state[1] = req.point_2;
			res.success = true;
			return true;
	}

}
#endif