# RT1_assignment2

ROS MONZA RACING C++
----------------------
This the secnod project of the course Research_Track_1, the project aim is to run the robot within the below given circuit with the following abalities

* Constantly driving around the environment in the clockwise direction when the user start.
* Avoiding to hit the walls.
* Being able to increase or decrease the velocity if the user wants 
* And Resetting its initial position when the user wants.

The package contains the C++ sources needed for the interaction with the robot & the user.


![image](https://user-images.githubusercontent.com/104999107/175375406-1560982b-888c-4e56-b70f-84e05d329abc.png)

## Installing and Running 

The simulator requires ROS ( Robot Operating System ),Once you have installed ROS and created your workspace you have to download the package second_assignment in the src folder of your workspace.

run the Assignment write the following command in your terminal
```
roslaunch second_assignment file.launch
```

## Nodes Used In The Program

To complete the requirment of the above mentioned behavior of the robot two nodes(**control.cpp** & **user.cpp**)that interact with robot and user and custom service(**Chnagespeed.srv**) to manage the change in speed are created to fulfil the requirments

### Conrol Node 

In the control node there is the main code of the project. This node handles multiple information and perform communications between the nodes and the user ,moreover it contains the main structure of the code which allows the robot to avoid hitting wall and drive through the circuit without any problem.


In the base_scan topic, which provides data about the laser that scans the surrounding environment, there is the type message sensor_msgs/LaserScan. The topic provides an array, which returns the distances between the robot and the obstacles; every distance is given by the array ranges[i] (i = 0,...,720) and it is computed considering each of 721 section in which the vision of the robot is divided, since the vision of the robot is included in a spectrum range of 180 degrees in front of it and expressed in radiant. I have separated 3 big subsections (right, left and in front of the robot), inside the 0-720 spectrum, for the vision of the robot and i have computed the minimum distance between the robot and the obstacle for each subsection, in order to implement the similar logic seen in the previous assignment. This is the function:


```

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
```
Furthermore the node permits to increment/decrement the velocity of the robot and reset its position (through the inputs given from keyboard and "passed" by the custom service Changespeed.srv (in the srv folder) that handles two elements: char Kinput, that is the request from the client and float32 range, that is the response from the server; in fact the control node is the server node that receives the request from the user node (client node)). 

```
/*This is the function that uses the Service 'service'. It reads the requests from client and
	change the global variable 'my_input.response.range' that multiplies the velocities so that 
	it makes the robot increase/decrease the velocity. 
	1.	The keyborad key 'a' is used for increasing the multiplier and therefore also the robot velocity.
	2.	The keyborad key 'd' is used for decreasing the multiplier and therefore also the robot velocity.
	3.	The keyborad key 'r' is used for restarting the robot from its initial position but also for resetting his initial velocity.	 
	
	This is a bool function, it returns true every time that one of the inputs above is received. */
		
	if(req.input=='a'){
		my_input.response.range+=0.5;
	}
	else if(req.input=='d'){
		my_input.response.range-=0.5;
	}
	else if(req.input=='r'){
		my_input.response.range = 1.0;
	}
	return true;
}
```
## User Node

The user node represents the interface of the project to  the user that assist the user to control the behavior the robot, as  user can increase/decrease the velocity of the robot and reset its position by simple commands:

 * A --> accelerate the robot by 0.5
 * D --> decelerate the robot by 0.5
 * R --> reset robot position.

```

char GETCHAR(void){

	char ch;
	
	std::cout << "ACCELERATE the robot: press A\n";
	std::cout << "DECELERATE the robot: press D\n";
	std::cout << "RESET the robot's position: press R\n";
	
	std::cin >> ch;
	
	return ch;
}
```
## Service
The service Changespeed.srv


```
char Kinput

---

float32 range

```
The request of he the user is sent to the control node and the server node will manage the request using a switch. No response will be sent to the user node  since the service operate directly on the control node.

## Flow Chart

![flowchart 2](https://user-images.githubusercontent.com/104999107/175698855-503c5184-a396-4120-9b74-4a011dd6f4e9.png)

## rqt_graph

![image](https://user-images.githubusercontent.com/104999107/175792139-f4f5d7a5-0c18-4370-bc72-c29c1179587b.png)

## Video

https://user-images.githubusercontent.com/104999107/177035138-ef5ef846-4aef-495d-9f38-5d6e94453e5d.mp4

## Conclusion And Future Improvement

1) When we are increasing the speed of the robot the robot can collide with the walls because we have a circuit which has sharp turns so the robot having hgher    spedd can crush itself with the walls. 

2) The robot gets very slow on turns.
