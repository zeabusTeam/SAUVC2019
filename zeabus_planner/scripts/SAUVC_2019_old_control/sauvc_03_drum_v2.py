#!/usr/bin/python2

import rospy
import math

try:
	from zeabus_extension.manage_log import log
	from zeabus_extension.control_auv import control_auv 
except:
	print("Pleas install setup.bash in zeabus_extension package")
	exit()

from zeabus_vision.srv import vision_srv_drum
from zeabus_vision.msg import vision_drum
from std_msgs.msg import String

class play_drum:
	
	def __init__( self , rate , first_forward , first_survey , forward , survey ):
		self.rate = rospy.Rate( rate )
		self.first_forward = first_forward;
		self.first_survey = first_survey;
		self.forward = forward
		self.survey = survey
		self.past_mode = None

		# mode -2 first_forward first_survey forward survey forward reverse_survey -2 -1 0 1 2
		self.mode = -2;
		self.log_command = log( "zeabus_planner" , "log" , "02_drum_command" )
		self.log_vision = log( "zeabus_planner" , "log" , "02_drum_vision" )
		self.client_drum = rospy.ServiceProxy('vision_drum' , vision_srv_drum )
		self.auv = control_auv( "play_drum")

		self.result_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }

		self.collect_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }
		self.target_depth = -3.4

	def play( self ):
		self.auv.absolute_depth( self.target_depth )
		while( not rospy.is_shutdown() and not self.auv.ok_position("z" , 0.1 ) ):
			self.rate.sleep()
		self.survey_mode()

	def survey_mode( self ):
		print("Now Survey Mode" )
		self.distance = 0
		if( self.mode == -2 ):	
			self.distance = self.first_forward 
		elif( self.mode == -1 ):
			self.distance = self.first_survey
		elif( self.mode == 0 ):
			self.distance = self.forward
		elif( self.mode in [ 1 , 2 ]):
			self.distance = self.survey

		if( self.mode in [-2 , 0 ] ):
			self.log_command.write("Move forward distance is " + str( self.distance ) , True , 0)
			print("\t Move Forward distance is " + str( self.distance))
		elif( self.mode in [-1 , 1 ] ):
			self.past_mode = 1
			self.log_command.write("Move left distance is " + str( self.distance ) , True , 0)
			print("\t Move Left distance is " + str( self.distance))
		elif( self.mode in [-1 , 1 ] ):
			self.past_mode = 2
			self.log_command.write("Move right distance is " + str( self.distance ) , True , 0)
			print("\t Move Right distance is " + str( self.distance))
		else:
			print("\t\t Missing line 72")
		target_found = 2
		count_found = 0		
# move survey and try to find object
		while( not rospy.is_shutdown() and self.auv.calculate_distance() < self.distance ):
			if( self.mode in [-2 , 0 ] ):
				self.auv.velocity( x = 0.10 )
				print( "move_forward : " + str( self.auv.calculate_distance() ) )
			elif( self.mode in [-1 , 1 ]):
				self.auv.velocity( y = 0.07 )
				print( "move_left : " + str( self.auv.calculate_distance() ) )
			else:
				self.auv.velocity( y = -0.07 )
				print( "move_right : " + str( self.auv.calculate_distance() ) )
			self.rate.sleep()
			self.analysis_all( 5 )
			if( self.result_vision["n_obj"] == 1):
				print("Found Count is " + str(count_found) )
				count_found += 1
				if( count_found == target_found ):
					print("Found Change Mode to use")
					break
			else:
				if( count_found > 0 ):
					print("Now Don't Found")
					count_found = 0
# finish survey decision to change mode
		if( count_found == target_found ):
			print("Found equal target_found")
			self.move_to_center()
		else:
			if( self.mode == -2 ):
				self.mode = -1
			elif( self.mode == -1 ):
				self.mode = 0
			elif( self.mode == 0 ):
				if( self.past_mode == 1 ):
					self.mode = 2
				elif( self.past_mode == 2):
					self.mode = 1
			elif( self.mode in [1 , 2]):
				self.mode = 0
			self.survey_mode()

	def move_to_center( self ):
		print( "Mode move to center")
		while( not rospy.is_shutdown() and self.target_depth > -4.2 ):
			print("move to center")
			while( not rospy.is_shutdown() ):
				self.analysis_all( 5 )
				self.rate.sleep():
				if( abs( self.result_vision['cx']) < 0.1 and
						abs( self.result_vision['cy']) < 0.1 ):
					print("ok")
				elif( abs( self.result_vision['cx']) > abs( self.result_vision['cy']) ):
					print("Play X and center is " + str( self.result_vision['cx']))
					if( self.result_vision['cx'] < 0 ):
						self.auv.velocity( y = -0.15 )
					else:
						self.auv.velocity( y = 0.15) 
				else:
					print("Play Y and center is " + str( self.result_vision['cy']))
					if( self.result_vision['cy'] < 0):
						self.auv.velocity( x = -0.1 )
					else:
						self.auv.velocity( x = 0.1 )
			
			
	def analysis_all( self , amont ):
		self.reset_vision_data()
		found = 0
		unfound = 0
		self.log_vision.write("Want to find Drum" , True , 0 )
		while( found < amont and unfound < amont ):
			self.individual_analysis()
			if( self.collect_vision[ "n_obj"] < 1 ):
				self.log_vision.write("Not Found" , False ,1 )
				unfound += 1
			else:
				self.log_vision.write("Found Result is " + self.print_result() , False , 1)
				found += 1
		if( found == amont):
			print( "The Result of Vision is Found! result is " + self.print_result()  )
			self.result_vision = self.collect_vision
			self.result_vision["cx"] /= amont
			self.result_vision["cy"] /= amont
			self.result_vision["area"] /= amont
		else:
			print( "The Result of Vision is UnFound!!!")
			self.result_vision["n_obj"] = 0
			
	def print_result( self ):
		return( "[ " + str( self.collect_vision["left"] ) + " , " + 
				str( self.collect_vision["right"] ) + " , " + 
				str( self.collect_vision["forward"] ) + " , " + 
				str( self.collect_vision["backward"]) + " ]")

	def individual_analysis( self ):
		temporary = self.client_drum( String("drum") , String("drop")).data
		if( temporary.n_obj == 1 ):
			self.collect_vision[ "n_obj" ] = temporary.n_obj
			self.collect_vision[ "cx" ] += temporary.cx
			self.collect_vision[ "cy" ] += temporary.cy
			self.collect_vision[ "left" ] = temporary.left
			self.collect_vision[ "right" ] = temporary.right
			self.collect_vision[ "forward" ] = temporary.forward
			self.collect_vision[ "backward" ] = temporary.backward
			self.collect_vision[ "area" ] += temporary.area
		else:
			self.collect_vision[ "n_obj" ] = temporary.n_obj

	def reset_vision_data( self ):
		self.result_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }

		self.collect_vision = { "n_obj" : -1 , "cx" : 0 , "cy" : 0 , "left" : False 
								,	"right" : False , "forward" : False , "backward" : False 
								,	"area" : 0 }

if __name__=="__main__":
	rospy.init_node("Mission Gate")	
	mission_drum = play_drum( 30 , 1 , 3 , 1 , 2)
	mission_drum.play()
