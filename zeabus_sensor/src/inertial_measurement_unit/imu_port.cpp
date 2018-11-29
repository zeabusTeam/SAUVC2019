/*
	File name			:	imu_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is file to use read connect ros and IMU

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Production

	Namespace			:	None
*/

#include	<ros/ros.h>

#include	<zeabus_library/zeabus_sensor/lord_microstrain.h>

namespace Asio = boost::asio;
namespace DataIMU = zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET ;

int main( int argv , char** argc ){
	
	ros::init( argv , argc , "main_control");

	ros::NodeHandle ph(""); // Handle for manage param from launch

	std::string port_name = "/dev/microstrain/3dm_gx5_45_0000__6251.65901";

	zeabus_sensor::LordMicrostrain imu( port_name );

	ros::Rate rate( 20 );

	bool result ;

	imu.open_port( result );

	imu.set_option_port( Asio::serial_port_base::baud_rate( (unsigned int) 11522 ) );
	imu.set_option_port( Asio::serial_port_base::flow_control( 
							Asio::serial_port_base::flow_control::none ) );
	imu.set_option_port( Asio::serial_port_base::parity( 
							Asio::serial_port_base::parity::none ) );
	imu.set_option_port( Asio::serial_port_base::stop_bits( 
							Asio::serial_port_base::stop_bits::one ) );
	imu.set_option_port( Asio::serial_port_base::character_size( (size_t) 8 ) );

	result = false;
	int IMU_base_rate = 0;

	do{
		imu.command_idle( result );
		rate.sleep();
	}while( ( ! result ) && ph.ok() );

	do{
		imu.sensor_get_IMU_base_rate( result , IMU_base_rate );
		rate.sleep();
	}while( ( ! result ) && ph.ok() );

	printf("IMU_base_rate is %d\n" , IMU_base_rate );

	imu.set_IMU_rate( IMU_base_rate / 100 );

	do{
		imu.sensor_init_setup_IMU_format( 3 );
		printf("Line65 ");
		imu.sensor_add_message_type( DataIMU::SCALED_ACCELEROMETER_VECTOR );
		printf("Line67 ");
		imu.sensor_add_message_type( DataIMU::SCALED_GYRO_VECTOR );
		printf("Line69 ");
		imu.sensor_add_message_type( DataIMU::CF_QUATERNION );
		printf("Line71\n");
		imu.sensor_setup_IMU_format( result );
	}while( ( ! result ) && ph.ok() );
}
