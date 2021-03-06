/*
	File name			:	rotation_handle.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 23
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of code for quaternion and matrix rotation

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	test

	Namespace			:	zeabus_library/rotation
*/

#include	<stdio.h>

#include	<zeabus_library/rotation/quaternion_handle.h>

#include	<zeabus_library/matrix.h>

#include	<zeabus_library/vector.h>

#include	<zeabus_library/error_code.h>

#ifndef _ZEABUS_LIBRARY_ROTATION_ROTATION_HANDLE_H__
#define _ZEABUS_LIBRARY_ROTATION_ROTATION_HANDLE_H__

namespace zeabus_library{

namespace rotation{

	class RotationHandle : public QuaternionHandle{
	
		public:
			RotationHandle();

////////////////////////////////////////////////////////////////////////////////////////////////
//	reference is http://www.chrobotics.com/library/understanding-quaternions
//		but we have confuse and testing follow referene it isn't work
//		But it dosen't mean all data is wrong I think it wrong about Vb = Rbi(Qbi)Vi
//		we must to invese to make it work
	
			size_t start_rotation( boost::numeric::ublas::matrix< double >& value 
							, boost::numeric::ublas::matrix< double >& result );

			size_t target_rotation( boost::numeric::ublas::matrix< double >& value 
							, boost::numeric::ublas::matrix< double >& result );

		private:
			boost::numeric::ublas::matrix< double > temporary_matrix;
			double diff_euler[3];	 

	}; 

}

}

#endif
