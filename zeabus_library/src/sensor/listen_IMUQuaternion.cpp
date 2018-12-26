/*
	File name			:	listen_IMUQuaternion.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 24
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is source of file for listen zeabus_library/IMUQuaternion.h

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Product
							
	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/listen_IMUQuaternion.h>

#define _DEBUG_RECIEVE_DATA_
#define _DEBUG_CALCULATE_ACCELERATION_
#define _DEBUG_CODE_
#define _DEBUG_TIME_
#define _DEBUG_DECLARE_OBJECT_

namespace zeabus_sensor{

	ListenIMUQuaternion::ListenIMUQuaternion( double roll , double pitch , double yaw , double gravity ){
		RH.set_start_frame( roll , pitch , yaw );
		this->result_acceleration.resize( 3 , 1 );
		this->result_gyro.resize( 3 , 1 );
		this->result_quaternion.resize( 4 , 1 );
		this->receive_acceleration.resize( 3 , 1 );
		this->offset_gravity.resize( 3 , 1 );
		this->set_gravity( gravity );
	}

	void ListenIMUQuaternion::copy_value( zeabus_library::IMUQuaternion target ){
		this->receive_msg.quaternion = target.quaternion;
		this->receive_msg.angular_velocity = target.angular_velocity;
		this->receive_msg.linear_acceleration = target.linear_acceleration;
	}
			
	void ListenIMUQuaternion::callback( const zeabus_library::IMUQuaternion& message ){
		#ifdef _DEBUG_TIME_
			this->timer.start();
		#endif
		this->receive_msg = message;
		#ifdef _DEBUG_CODE_
			printf("Convert queaternion msg -> matrix\n");
		#endif
		zeabus_library::Point4_to_matrix( this->receive_msg.quaternion 
											, this->result_quaternion );
		#ifdef _DEBUG_CODE_
			printf("Convert angular velocity msg -> matrix\n");
		#endif
		zeabus_library::Point3_to_matrix( this->receive_msg.angular_velocity 
											, this->result_gyro );
		#ifdef _DEBUG_CODE_
			printf("Convert linear acceleration msg -> matrix\n");
		#endif
		zeabus_library::Point3_to_matrix( this->receive_msg.linear_acceleration 
											, this->receive_acceleration );

		RH.set_target_frame( message.quaternion );
		RH.target_rotation( this->receive_acceleration , this->result_acceleration );
	
		#ifdef _DEBUG_CALCULATE_ACCELERATION_
			zeabus_library::vector::print("receive acceleration" , this->receive_acceleration );
			zeabus_library::vector::print("Result acceleration " , this->result_acceleration );
			zeabus_library::vector::print("Offset gravity" , this->offset_gravity );
		#endif	

		#ifdef _DEBUG_TIME_
			printf("Last line of callback listen_IMUData : %15.8f\n" , this->timer.capture() );
		#endif
	}

	void ListenIMUQuaternion::set_orientation( double roll , double pitch , double yaw ){
		RH.set_start_frame( roll , pitch , yaw );
		RH.update_rotation( );
		
	}

	void ListenIMUQuaternion::set_gravity( double gravity ){
		this->offset_gravity( 0 , 0 ) = 0 ;
		this->offset_gravity( 1 , 0 ) = 0 ;
		this->offset_gravity( 2 , 0 ) = gravity ;
	}

	void ListenIMUQuaternion::get_result( zeabus_library::IMUQuaternion& message ){
		split_matrix_to_IMUQuaternion( this->result_quaternion , this->result_gyro
								,this->result_acceleration , message );	
	}

}