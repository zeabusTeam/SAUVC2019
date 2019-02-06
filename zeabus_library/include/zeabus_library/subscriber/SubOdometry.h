/*
	File name			:	SubOdometry.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 06
	Date last modified	:	2018 , ??? ??
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Using & Maintain
							
	Namespace			:	zeabus_library/subscriber
*/
//====================>

#include	<stdio.h>

#include	<iostream>

#include	<ros/ros.h>

#include	<nav_msgs/Odometry.h>

#ifndef _ZEABUS_LIBRARY_SUBSCRIBER_SEBIMU_H__
#define _ZEABUS_LIBRARY_SUBSCRIBER_SEBIMU_H__

namespace zeabus_library{

namespace subscriber{

	class SubOdometry{

		public:	
			SubOdometry( nav_msgs::Odometry* data );

			void register_data( nav_msgs::Odometry* data );

			void callback( const nav_msgs::Odometry& message );

			void callback_ttl( const nav_msgs::Odometry& message );
	
			void register_ttl( int* ttl , int number = 20 );

			void set_ttl( int number );

		private:
			int constant_ttl;
			
			int* ttl; // ttl = time to live 
			nav_msgs::Odometry* data;


	};

}

}

#endif
