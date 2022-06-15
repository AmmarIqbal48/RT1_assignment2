//*************************************************Libraries used for Progran*******************************************************

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h" 
#include "geometry_msgs/Twist.h" 
#include "std_srvs/Empty.h"
#include "second_assignment/Changespedd.h"

//**************************************************Control_node************************************************************

ros::Publisher pub;

//****************************************************Global Variable*******************************************************

//variable "plus" allows us to increment/decrement the velocity of the robot
float plus = 0.0;

// setting a general distance value for comparison
float distance_value = 40.0;

//********************************************Funtions used in program ******************************************************

// This function is used to obtain the minimum distance from the wall in spectrum range of [0] to [720]
// I will pass the 2 values that describes my range and the array of distances about that range.

float Compute_min(int min_range, int max_range, float distance_wall[]){
	
	
	
	//we will obtain the minimum distance from wall
		for(int i = min_range; i <= max_range; i++){
		
		if(distance_wall[i] <= distance_value){
		
			distance_value = distance_wall[i];
		}
	}
	
	return distance_value;
}


// We introduce a boolean function to handle the request input coming from the user_node
// and this is allowed by the custom service we had created "Changespeed.srv" that acts as a bridge
// between the nodes. 

bool ServerCallback(second_assignment::Changespeed::Request &req, second_assignment::Changespeed::Response &res){
	

	/*This is the function that uses the Service 'service'. It reads the requests from client and
	change the global variable 'my_input.response.range' that multiplies the velocities so that 
	it makes the robot increase/decrease the velocity. 
	1.	The keyborad key 'a' is used for increasing the multiplier and therefore also the robot velocity.
	2.	The keyborad key 's' is used for decreasing the multiplier and therefore also the robot velocity.
	3.	The keyborad key 'r' is used for restarting the robot from its initial position but also for resetting his initial velocity.	 
	
	This is a bool function, it returns true every time that one of the inputs above is received. */
		
	if(req.input=='a'){
		my_input.response.range+=0.5;
	}
	else if(req.input=='s'){
		my_input.response.range-=0.5;
	}
	else if(req.input=='r'){
		my_input.response.range = 1.0;
	}
	return true;
}
	


// Function RobotCallback() to handle the datas about /base_scan topic 

void RobotCallback(const sensor_msgs::LaserScan::ConstPtr& msg){ 

	// We have to publish the velocity on topic cmd_vel.
	
	geometry_msgs::Twist my_vel; 
	
	// Initilize an empty arrey in order to fullfil it with the ranges[i]'s values
	
	float laser_scanner[721];
	
	for(int i = 0; i < 722; i++){
	
		laser_scanner[i] = msg->ranges[i];
	}
	
	// We take the minimum distance of the robot from the wall in each subsections
	
	float min_right_dist = Compute_min(0, 105, laser_scanner);
	float min_front_dist = Comput_min(305, 405, laser_scanner);
	float min_left_dist = Compute_min(615, 715, laser_scanner);
	
	// As we have done in the previous assignement, the robot will avoid going against the 
	// obstacles thanks to these simply statements.
	
	if(min_front_dist < 1.6){
			
    	if(min_right_dist < min_left_dist){
    	
    	// The robot will turn right if the minimum distance from the wall on the left is 
        // greater than the distance of the wall on the right
    	
    		my_vel.angular.z = 1.0;
    		my_vel.linear.x = 0.1;
		}
		
		else if(min_right_dist > min_left_dist){
		
    	// The robot will turn left if the minimum distance from the wall on the right is 
	// greater than the distance of the wall on the left
		
			my_vel.angular.z = -1.0;
			my_vel.linear.x = 0.1;
		}
	}
		
	else{
	
	// If the robot doesn't have to make a curve to avoid hitting walls, will go straight.
	// The plus variable will increment/decrement if we press "A"/"D" on the user_node
	
	my_vel.linear.x = 1.0 + addition;
	my_vel.angular.z = 0.0;
	
		// Just a simple control to avoid make the robot going backwards.
	
		if(my_vel.linear.x < 0.0){
			
			my_vel.linear.x = 0.0;
			my_vel.angular.z = 0.0;
		}
	}
	
	// Publish my_vel on cmd_vel (both angular and linear of my_vel since i used geometry_msgs::Twist my_vel and
	// i associate my_vel to linear and angular velocity)
		
	pub.publish(my_vel);					
}

//*************************************************************MAIN**********************************************************

int main (int argc, char **argv){

	// Initialize the node, setup the NodeHandle for handling the communication with the ROS
	//system

	ros::init(argc, argv, "control_node");
	ros::NodeHandle nh;
	
	// We have to introduce this line since the control_node acts like a server
	
	ros::ServiceServer service =  nh.advertiseService("/accelaration", ServerCallback);
	
	// Since we have to publish my_vel in the cmd_vel topic we are goint to instert this important line:
	
	pub = nh.advertise<geometry_msgs::Twist> ("/cmd_vel", 1);

	// Define the subscriber to /base_scan topic
	
	ros::Subscriber sub = nh.subscribe("/base_scan", 1, RobotCallback);
	
	ros::spin();
	
	return 0;
}
