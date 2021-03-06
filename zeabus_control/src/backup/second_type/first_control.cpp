/*
	File name			:	first_control.cpp
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 20
	Date last modified	:	2019 , JAN ??
	Purpose				:	For control system will give target velocity
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.2.0
	status				:	USE

	Namespace			:	-
*/
//===============>

#include	<stdio.h>

#include	<ros/ros.h>

#include	<math.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/convert/Point3.h>

#include	<zeabus_library/control/service_control.h>

#include	<zeabus_library/subscriber/SubOdometry.h>

#include	<zeabus_library/control/listen_twist.h>

#include	<zeabus_library/linear_equation.h>

#include	<zeabus_library/general.h>

#include	<zeabus_library/Twist.h>

#include	<zeabus_library/euler.h>

#include	"first_control_function.cpp"

#define _PRINT_DATA_
//#define _PRINT_QUATERNION_

#define _DEBUG_ORDER_

namespace euler_ = zeabus_library::euler;

int main( int argv , char** argc ){

	ros::init( argv , argc , "first_control" );

	ros::NodeHandle ph("~"); // Handle for manage param from launch file == parameter handle
	ros::NodeHandle nh(""); // Handle for manage about this file in ros system == node handle

///////////////////////////////////-- PARAMETER PART --//////////////////////////////////////////
	std::string topic_state;
	std::string topic_output;
	std::string topic_twist;
	int frequency;
	int constant_value = 12;

	ph.param< std::string >("topic_state" , topic_state , "/localize/state");
	ph.param< std::string >("topic_twist" , topic_twist , "/control/twist");
	ph.param< std::string >("topic_output" ,topic_output , "/control/target" );
	ph.param< int >("frequency" , frequency , 50 );
///////////////////////////////////////////////////////////////////////////////////////////////

	int mode_control = 0;	//  mode control is consider by roll and pitch
							//	mode 0 is roll & pitch normal is 0
	
	bool temp_bool = false;
	double temp_distance = 0;

	nav_msgs::Odometry current_state;
	nav_msgs::Odometry target_state;
	nav_msgs::Odometry temp_state;

	zeabus_library::subscriber::SubOdometry listen_odometry( &current_state );
	int received = 0 ;
	bool first_time_received = false;
	listen_odometry.register_ttl( &received , 1 ); 

	int received_velocity[6] = { 0  , 0 , 0 , 0 , 0 , 0};
	zeabus_library::Twist target_velocity;
	zeabus_library::control::ListenTwist listen_velocity( received_velocity );
	listen_velocity.set_constant( 20 );
		

	zeabus_library::control::ServiceControl service( &mode_control );
	service.register_position( &current_position , &target_position , &diff_position );
	service.register_euler( current_euler , target_euler , diff_euler );
	service.register_quaternion( &current_quaternion , &target_quaternion );
	double ok_error[6] = { 0.05 , 0.05 , 0.1 , 0.05 , 0.05 , 0.05};
	service.register_ok_error( ok_error );

//////////////////////////////////////-- ROS SYSTEM --///////////////////////////////////////////
	ros::Subscriber sub_state = nh.subscribe( topic_state , 1 
								, &zeabus_library::subscriber::SubOdometry::callback_ttl
								, &listen_odometry );

	ros::Subscriber sub_target = nh.subscribe( topic_twist , 1
								, &zeabus_library::control::ListenTwist::callback
								, &listen_twist );

	ros::Publisher tell_target = nh.advertise< zeabus_library::Twist >( topic_output , 1 );

	ros::ServiceServer service_distance = 
		nh.advertiseService( "/control/relative_xy"
						, &zeabus_library::control::ServiceControl::callback_relative_position
						, &service );

	ros::ServiceServer service_mode = 
		nh.advertiseService( "/contol/mode"
						, &zeabus_library::control::ServiceControl::callback_mode_control
						, &service );

	ros::ServiceServer service_fix_point = 
		nh.advertiseService( "/control/fix_point"
						, &zeabus_library::control::ServiceControl::callback_fix_point 
						, &service );

	ros::ServiceServer service_check_position = 
		nh.advertiseService( "/control/check_position"
						, &zeabus_library::control::ServiceControl::callback_check_position
						, &service );

	ros::ServiceServer service_get_target = 
		nh.advertiseService( "/control/get_target"
						, &zeabus_library::control::ServiceControl::callback_get_target 
						, &service );

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		// for find diff euler
		rh.set_start_frame( current_quaternion ); 
		rh.set_target_frame( target_quaternion );
		rh.start_frame.get_RPY( current_euler[0] , current_euler[1] , current_euler[2] );
		rh.target_frame.get_RPY( target_euler[0] , target_euler[1] , target_euler[2] );
		rh.get_RPY( diff_euler[0] , diff_euler[1] , diff_euler[2] ); // start go to target
		rh.update_rotation();
		rh.update_rotation();

		if( mode_control == 0 ){
			#ifdef _DEBUG_ORDER_
				printf("BEFORE PLAN Z\n");
			#endif
			if( count_velocity[2] != 0 ){
				message.linear.z = target_velocity.z;
				target_position.z = current_position.z;
				count_velocity[2]--;
			}
			else{
				diff_position.z = target_position.z - current_position.z;
				message.linear.z = assign_veloity_z( diff_position.z );
			}
			#ifdef _DEBUG_ORDER_
				printf("BEFORE PLAN YAW\n");
			#endif
			if( count_velocity[5] != 0 ){
				message.angular.z = target_gyroscope.z;
				target_quaternion = current_quaternion;
				count_velocity[5]--;
			}
			else{
				message.angular.z = assign_gyroscope_z( diff_euler[2] );
			}
		}
		tell_target.publish( message );
		#ifdef _PRINT_DATA_
			zeabus_library::clear_screen();
			printf("current_position   : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, current_position.x , current_position.y , current_position.z 
						, current_euler[0] , current_euler[1] , current_euler[2] );
			printf("target_position    : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, target_position.x , target_position.y , target_position.z 
						, target_euler[0] , target_euler[1] , target_euler[2] );
			printf("diff_position      : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, diff_position.x , diff_position.y , diff_position.z 
						, diff_euler[0] , diff_euler[1] , diff_euler[2] );
			printf("output_velocity    : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, message.linear.x , message.linear.y ,message.linear.z 
						, message.angular.x , message.angular.y , message.angular.z );
			printf("current_velocity   : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, current_velocity.x , current_velocity.y ,current_velocity.z 
						, current_gyroscope.x , current_gyroscope.y , current_gyroscope.z );
			printf("target_velocity    : %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf  %8.3lf\n\n"
						, target_velocity.x , target_velocity.y ,target_velocity.z 
						, target_gyroscope.x , target_gyroscope.y , target_gyroscope.z );
			printf("count_velocity     : %8.3d  %8.3d  %8.3d  %8.3d  %8.3d  %8.3d\n\n"
						, count_velocity[0] , count_velocity[1] ,count_velocity[2] 
						, count_velocity[3] , count_velocity[4] , count_velocity[5] );
		#ifdef _PRINT_QUATERNION_
			printf("current_quaternion : %8.3lf %8.3lf %8.3lf %8.3lf\n\n"
							, current_quaternion.w , current_quaternion.x 
							, current_quaternion.y ,current_quaternion.z );
			printf("target_quaternion : %8.3lf %8.3lf %8.3lf %8.3lf\n\n"
							, target_quaternion.w , target_quaternion.x 
							, target_quaternion.y , target_quaternion.z );
		#endif
		#endif
	}

}

// This first control will interest in world frame but 
