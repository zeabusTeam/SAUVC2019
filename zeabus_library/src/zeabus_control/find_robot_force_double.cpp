////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_force_double.cpp 
//	Purpose		: for can make parity of force to do that
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/find_robot_force_double.h>

namespace zeabus_control{

	// version 1 will target on x and y
	void pid_to_robot_foce_v_1( double* pid_force ,  double* robot_force){

		for( int run = 2 ; run < 6 ; run++) robot_force[run] = pid_force[run];

		if( fabs( pid_force[0] - pid_force[1] ) > 1.5 ){
			if( fabs( pid_force[0] ) > fabs( pid_force[1] ) ){
				robot_force[1] = 0;
				robot_force[0] = pid_force[0];
			}
			else{
				robot_force[0] = 0;
				robot_force[1] = pid_force[1];
			}
		}
		else{
			robot_force[0] = pid_force[0];
			robot_force[1] = pid_force[1];
		}
		
	}

	// version 2 will add force for maintain depth of robot
	void pid_to_robot_foce_v_2( double* pid_force ,  double* robot_force){

		for( int run = 0 ; run < 6 ; run++) robot_force[run] = pid_force[run];

		robot_force[2] += -0.98;
	}	
}

