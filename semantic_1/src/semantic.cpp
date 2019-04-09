#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/String.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <graph.h>
#include <linked_list.h>

std::ofstream outf("/home/ubuntu/Desktop/sem.txt");
//tf::TransformListener* listener_ptr;
float _X;
float _Y;
int esplora;
ros::Publisher move_base_pub;
graph* g;

//tf::TransformListener listener;

float dist(float xx, float yy, float xc, float yc){
	return sqrt(pow(abs(xx-xc),2)+pow(abs(yy-yc),2));
}

void poseCallback(const nav_msgs::Odometry::ConstPtr& msg) {
	//std::cout << "ID " << msg->header.frame_id << std::endl;
	_X = msg->pose.pose.position.x;
	_Y = msg->pose.pose.position.y;
	//outf << "POSE:" << msg->header.stamp << "\t(" << x << "," << y << ")" << std::endl;
	
	graph_node* nearest = graph_nearest_node(g,_X,_Y);
	float x_nearest = graph_node_get_X(nearest);
	float y_nearest = graph_node_get_Y(nearest);
	if(dist(x_nearest,y_nearest,_X,_Y) > 1){
		graph_node* n = graph_add_node(g,graph_new_node_prop("X",0,_X,_Y));						
		graph_add_edge(g,n,nearest);
	}
	//*/
}
/*
std::string prendi_inizio(std::string msg){
	int x = 0,last_space;
	for (std::string::reverse_iterator rit=msg.rbegin(); rit!=msg.rend(); ++rit){
		if(*rit == " ") break; 
    x++;
	}
	last_space = msg.size() - x;
	std::string inizio = msg.substr(0,last_space);
	return inizio;
}

std::string prendi_fine(std::string msg){
	int x = 0,last_space;
	for (std::string::reverse_iterator rit=msg.rbegin(); rit!=msg.rend(); ++rit){
		if(*rit==" ") break; 
    x++;
	}
	last_space = msg.size()-x;
	std::string fine = msg.substr(last_space,x);
	return fine;
}

void go_to(float dest_x, float dest_y){
  
  geometry_msgs::PoseStamped goal;
	goal.header.frame_id = "/map";
	//goal.header.stamp = ros::Time::now();
	goal.pose.position.x = dest_x;
	goal.pose.position.y = dest_y;
	goal.pose.orientation.w = 1.0;
  move_base_pub.publish(goal);
  std::cout << "INVIATO" << std::endl;

}

void speechCallback(const std_msgs::String::ConstPtr& msg){
	std::string inzio;
	std::string fine;
	fine = prendi_fine(msg);
	inizio = prendi_inizio(msg);
	if(inizio == "siamo in"){
		graph_node* nodo = graph_nearest_node(g,_X,_Y);
		graph_node_set_label(nodo,fine);
	}
	if(inizio == "vai in"){
		graph_node* source = graph_node_find_label(g,fine);
		graph_node* dest = graph_nearest_node(g,_X,_Y);
		linked_list* list = linked_list_new();
		graph_DFS(source,dest,list);
		linked_list_iterator* it = linked_list_iterator_new(list);
		while(it!=NULL){
			graph_node* n = (graph_node*)linked_list_iterator_getvalue(it);
			int nx = graph_node_get_X(n);
			int ny = graph_node_get_Y(n);
			go_to(nx,ny);
			it = linked_list_iterator_next(it);
		}
	}
}
*/
int main(int argc, char** argv) {
	ros::init(argc, argv, "semantic");
	ros::NodeHandle nh;
	
	std::cout << "open" << std::endl;
	
	_X = 0;
	_Y = 0;
	g = graph_new();
	graph_add_node(g,graph_new_node_prop("partenza",1,_X,_Y));
	//*/
	//SUB
	ros::Subscriber odom_sub = nh.subscribe("/diago/odom", 10, &poseCallback);
	//ros::Subscriber speech_sub = nh.subscribe("/best_speech_hypotesis", 10, &speechCallback);
	
	//PUB
	move_base_pub = nh.advertise<geometry_msgs::PoseStamped>("/diago/move_base_simple/goal", 1);
	//twist_pub = nh.advertise<geometry_msgs::Twist>("/diago/cmd_vel", 10);
	
	//go_to(1.25,2.0);
	//ros::Subscriber aiml_sub = nh.subscribe(".......");
	
	//tf::TransformListener listener;
	//listener_ptr = &listener;
	
	ros::spin();
	
	return 0;
}
