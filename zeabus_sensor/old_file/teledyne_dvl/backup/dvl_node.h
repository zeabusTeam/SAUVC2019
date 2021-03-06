/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: dvl_node.h
//
//	Edit	: Sep 9 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Dec 22 , 2014
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ROS_ROS
	#include	<ros/ros.h>
	#define		ROS_ROS
#endif

#ifndef ZEABUS_EXTENSION_CONVERT_TO_STRING
	#include	<zeabus_extension/convert_to_string.h>
	#define		ZEABUS_EXTENSION_CONVERT_TO_STRING
#endif

#ifndef	ZEABUS_EXTENSION_LOG_DATA
	#include	<zeabus_extension/log_data.h>
	#define		ZEABUS_EXTENSION_LOG_DATA
#endif

#ifndef STD_MSGS_STRING
	#include	<std_msgs/String.h>
	#define		STD_MSGS_STRING
#endif

#ifndef GEOMETRY_MSGS_TWIST
	#include	<geometry_msgs/Twist.h>
	#define		GEOMETRY_MSGS_TWIST
#endif

#ifndef STDIO_H
	#include	<stdio.h>
	#define		STDIO_H
#endif

geometry_msgs::Twist data_dvl_node; // for send value velocity
ros::Publisher publisher_dvl_data; 
zeabus_extension::log_data::log dvl_log( "zeabus_sensor" , "log/teledyne_dvl" , "dvl_node");

void listen_port_dvl( const std_msgs::String message);
