#!/usr/bin/python2
#################################################################################################
####
####	FILE		: mission_all_01.py
####	Maintain	: Supasan Komonlit
####	Create on	: 2019 , FEB 21
####	Purpose		: For competition in SAUVC2019
####
#################################################################################################
from __future__			import print_function
#===============>

import rospy
import math
import time
import os

from vision_collector		import VisionCollector
from standard_mission		import StandardMission 

from zeabus_library.srv		import TwoBool

new_pid = 1

class MissionAll( StandardMission ):
	
	def __init__( self , name ):
		self.name = name

		StandardMission.__init__( self , self.name , "/mission/all" , self.callback )

		self.gate = VisionCollector("gate")
		self.run_gate = False
		self.drum = VisionCollector("drum")
		self.run_drum = False
		self.flare = VisionCollector("flare")
		self.run_flare = False
		self.mission_gate = rospy.ServiceProxy( "/mission/gate" , TwoBool )
		self.mission_flare = rospy.ServiceProxy( "/mission/flare" , TwoBool )
		self.mission_drum = rospy.ServiceProxy( "/mission/drop" , TwoBool )
		self.mission_golf = rospy.ServiceProxy( "/mission/pick" , TwoBool )

		self.start_time = time.time()

		self.start_yaw = 0

		self.over = self.over_distance
		self.type_over = "distance"
#		self.over = self.over_time
#		self.type = "time"

		self.echo(self.name , "FINISHED SETUP ALL MISSION")

	def callback( self , message ):
		if( self.state and message.data ):
			self.echo( self.name , "Now mission will run please close befor try again")
			return False
		elif( message.data ):
			self.state = True
			self.main_play()
		else:
			self.echo( self.name , "Switch call to stop run mission")
			self.state = False
			if( self.run_gate ):
				self.run_gate = self.gate( False )
				self.echo( self.name , "Call to close gate")
			if( self.run_drum ):
				self.run_drum = self.drun( False)
				self.echo( self.name , "Call to close drum")
			if( self.run_flare ):
				self.run_flare = self.flare( False )
				self.echo( self.name ,  "Call to close flare")
			return False
		
		self.echo( self.name , "End Callback")
		return True

	def main_play( self ):

		# This function will call by switch we must reset data target
		self.reset_velocity( "xy" )
		self.reset_target( "xy")
		self.reset_target( "yaw" )
		self.fix_z( -0.5 )

		self.collect_state()
		self.start_yaw = self.save_state[5]
	
		self.echo( self.name , "START ALL MISSION at yaw is " + str( self.start_yaw ) )
	
		self.wait_state( "z" , 0.1 , 5 )	

		result = self.survey_mode( 2 , 6 , 1.5 , -4 , "gate" , self.gate )

		if( self.state ):	
			self.fix_z( -1.3 )
			self.fix_yaw( self.start_yaw - math.pi / 2 )
			self.wait_state( "z" , 0.1 , 5 )
			self.wait_state( "yaw" , 0.1 , 5 )
			result = self.survey_mode( 6 , 6 , 1 , -4 , "flare" , self.flare )

		if( self.state ):
			self.fix_z( -0.5 )
			self.fix_yaw( self.start_yaw )
			self.wait_state( "z" , 0.1 , 5 )
			self.wait_state( "yaw" , 0.1 , 5 )
			result = self.survey_mode( 6 , 10 , 1 , 5 , "drum" , sefl.drum )
	
		if( self.state ):
			self.fix_z( -0.5 )
			self.wait_state( "z" , 0.1 , 5 )
			self.velocity_xy( -0.1 , 0 )
			self.over_time( 15 , True )
			while( self.ok_state() ):
				self.sleep( 0.1 )
				if( self.over_time( 15 ) ):
					break
			self.reset_target( "xy" )
			self.velocity_xy( 0.1 , 0 )
			while( self.ok_state() ):
				self.sleep( 0.1 )
				self.drum.analysis_all( "blue" , "side" , 5 )
				if( self.drum.have_object() ):
					self.mission_golf( True )
					break

		self.fix_z( 0 )	
			
		
		# FINISH Part set up data information for startup

		return True
		
		
	# WARNING ! survey mode will use after rotation 
	# step is move forward move slide move forward and move inverse slide
	def survey_mode( self , first_forward , first_slide , forward , slide , mission , service ):
		self.echo( self.name , "We start survey for mission " + str(mission) ) 
		type_movement = 1 # 1 is forward , 2 is first_slide , 3 is forward , 4 is slide
		current_fix_velocity = False
		count_have_object = 0
		limit_value = first_forward
		while( self.ok_state() ):
			# This part of connect to vision and call service for make make mission
			if( mission == "gate" ):
				self.gate.analysis_all( "gate" , "sevinar" , 5 )
				if( self.gate.have_object() ): count_have_object += 1
				else: count_have_object = 0
			elif( mission == "flare" ):
				self.flare.analysis_all( "flare" , "far" , 5 )
				if( self.flare.have_object() ): count_have_object += 1
				else:
					self.flare.analysis_all( "flare" , "near" , 5 )
					if( self.flare.have_object() ): count_have_object += 1
					else: count_have_object = 0
			elif( mission == "drum" ):
				self.drum.analysis_all( "drum" , "drop" , 5 )
				if( self.drum.have_object() ): count_have_object += 1
				else: count_have_object = 0
			else:
				self.echo( self.name , "Don't have this mode vision aborted" )
				self.state = False
			if( count_have_object > 0 ):
				if( current_fix_velocity ):
					self.reset_velocity( "xy" )
					self.reset_target("xy")
					current_fix_velocity = False
					if( self.type_over == "distance" ):
						limit_value -= self.distance()
					elif( self.type_over == "time" ):
						limit_value -= time.time() - self.start_time
				self.echo( self.name , "Now survey for mission " + str(mission) + 
						" Count " + str(count_have_object) )
				if( count_have_object == 5 ):
					# This is part about send service to individual mission to do task
					self.echo( self.name , "We send process to mission " + str(mission) )
					result = service( True )
					count_have_object = 0
					if( result ):
						return True
				continue
			# This part of connect to control for survey
			if( type_movement == 1 ):
				if( not current_fix_velocity ):
					self.over( limit_value , True )
					self.velocity_xy( 0.10 , 0 )
					current_fix_velocity = True
				if( self.over( limit_value ) ):
					type_movement = 2 
					self.reset_velocity( "xy" )
					current_fix_velocity = False
	
			elif( type_movement == 2 ):
				if( not current_fix_velocity ):
					self.over( limit_value , True )
					self.velocity_xy( 0 , math.copysign( 0.1 , first_slide ) )	
					current_fix_velocity = True 
				if( self.over( limit_value ) ):
					type_movement = 3
					self.reset_velocity( "xy" )
					current_fix_velocity = False

			elif( type_movement == 3 ):
				if( not current_fix_velocity ):
					self.over( limit_value , True )
					self.velocity_xy( 0.1 , 0 )
					current_fix_velocity = True
				if( self.over( limit_value ) ) :
					type_movement = 4
					self.reset_velocity( "xy" )
					current_fix_velocity = False
					first_forward = forward

			elif( type_movement == 4 ):
				if( not current_fix_velocity ):
					self.over( limit_value , True )
					self.velocity_xy( 0 , math.copysign( 0.1 , slide) )
					current_fix_velocity = True
				if( self.over( limit_value ) ):
					type_movement = 1
					self.reset_velocity( "xy" )	
					current_fix_velocity = False
					first_slide = math.copysign( slide , first_slide )
			else:
				self.echo( self.name , "Dont'have this mode type_movement aborted")
				self.state = False
		return False

	def over_time( self , limit_value , setup = False ):
		if( setup ):
			self.start_time = time.time()	
			return False
		elif( abs(limit_value) < time.time() - self.start_time ):
			print( "For check difftime " + str( time.time() - self.start_time ) +
					" and limit time is " + str( limit_value ) )
			return True
		else:
			print( "For check difftime " + str( time.time() - self.start_time ) +
					" and limit time is " + str( limit_value ) )
			return False

	def over_distance( self , limit_value , setup = False ):
		if( setup ):
			self.collect_state()
			return False
		elif( self.distance() > limit_value ):
			print( "For check distance " +str( self.distance() ) 
					+" and limit is " + str( limit_value ) )
			return True
		else:
			print( "For check distance " +str( self.distance() ) 
					+" and limit is " + str( limit_value ) )
			return False
			
		
if __name__ == "__main__":
	rospy.init_node( "mission_all" )
	MA = MissionAll( "mission_all" )
	rate = rospy.Rate( 10 )
	while( not rospy.is_shutdown() ):
#		print( "RUNNING on new_pid before : " + str(new_pid) )
		if( new_pid == 0):
			os._exit(0)
#		print( "RUNNING on new_pid after : " + str(new_pid) )
		rate.sleep()
	print("End of file new_pid is " + str(new_pid))
