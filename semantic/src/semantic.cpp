#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <move_base_msgs/MoveBaseAction.h>
//#include <graph.h>
//#include <linked_list.h>

std::ofstream outf("/home/ubuntu/Desktop/sem.txt");
//tf::TransformListener* listener_ptr;
float x;
float y;
int esplora;
ros::Publisher twist_pub;
//graph* g;

//tf::TransformListener listener;

/*struct graph_node_prop{
	char* label;
	int is_something;
	float x,y,z;
};*/
float dist(float xx, float yy, float xc, float yc){
	return sqrt(pow(abs(xx-xc),2)+pow(abs(yy-yc),2));
}

void poseCallback(const nav_msgs::Odometry::ConstPtr& msg) {
	std::cout << "ID " << msg->header.frame_id << std::endl;
	x = msg->pose.pose.position.x;
	y = msg->pose.pose.position.y;
	outf << "POSE:" << msg->header.stamp << "\t(" << x << "," << y << ")" << std::endl;
	/*graph_node* nearest = graph_nearest_node(g,x,y);
	graph_node_prop* n_prop = (graph_node_prop*) graph_get_node_value(nearest);
	if(dist(n_prop->x,n_prop->y,x,y) > 1){
		graph_node_prop* val = (graph_node_prop*)malloc(sizeof(graph_node_prop*));
		val->label = "X";
		val->is_something = 0;
		val->x = x;
		val->y = y;
		
		graph_node* n = graph_add_node(g,(void*)val);
		graph_add_edge(g,n,nearest);
	}*/
}

/*void aimlCallback(const ........ msg){
	if(msg->msg == "esplora") esplora = 1;
	if(msg->msg == "fine") esplora = 0;
	if(esplora && msg->msg == "siamo in"){
		nodo = nearest_node(g,x,y)
		nodo.label = msg->msg;
	}
	if(!esplora && msg->msg == "vai in"){
		dest = cerca_label(msg->msg);
		source = nearest_node(g,x,y);
		node_list = DFS(source,dest);
		for(int i = 0;i<node_list.size;i++){
			dest = node_list.next;
			vai_in(dest);
		}
	}
}*/
void vai_in(float dest_x, float dest_y){
	/*while(dist(x,y,dest_x,dest_y)>0,1){
		float XX = x-dest_x;
		float YY = x-dest_y;
		geometry_msgs::Twist twist;
		twist.linear.x = 0.1;
		twist.linear.y = 0.1;
		twist.linear.z = 0.1;
		twist_pub.publish(twist);
	}
	geometry_msgs::Twist stop;
	stop.linear.x = 0;
	stop.linear.y = 0;
	twist_pub.publish(stop);
		*/
	/*actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> move_b_client("/diago/move_base_simple", true);


  ROS_INFO("Attendo il server");
  //attendo la preparazione e settaggio del server
  move_b_client.waitForServer();

  ROS_INFO("Connesso");
  //setto il messaggio*/
  
  
  geometry_msgs::PoseStamped goal;
	goal.header.frame_id = "/map";
	//goal.header.stamp = ros::Time::now();
	goal.pose.position.x = dest_x;
	goal.pose.position.y = dest_y;
	goal.pose.orientation.w = 1.0;
  twist_pub.publish(goal);
  std::cout << "INVIATO" << std::endl;
  //move_b_client.sendGoal(goal);
	/*

  //imposto una durata,se il server non termina nel suo stato prima del timer termino il clien con la return
  int flag = move_b_client.waitForResult(ros::Duration(0.001));
  if (flag){
    ROS_INFO("Azione completata");
  }else{
    ROS_INFO("Tempo esaurito");
  }*/
}
int main(int argc, char** argv) {
	ros::init(argc, argv, "semantic");
	ros::NodeHandle nh;
	
	std::cout << "open" << std::endl;
	x = 0;
	y = 0;
	esplora = 0;
	/*g = graph_new();
	graph_node_prop* p =(graph_node_prop*)malloc(sizeof(graph_node_prop));
	p->label = "partenza";
	p->x = x;
	p->y = y;
	graph_add_node(g,(void*)p);*/
	
	ros::Subscriber odom_sub = nh.subscribe("/diago/odom", 10, &poseCallback);
	twist_pub = nh.advertise<geometry_msgs::PoseStamped>("/diago/move_base_simple/goal", 10);
	//twist_pub = nh.advertise<geometry_msgs::Twist>("/diago/cmd_vel", 10);
	
	vai_in(1.25,2.0);
	//ros::Subscriber aiml_sub = nh.subscribe(".......");
	
	//tf::TransformListener listener;
	//listener_ptr = &listener;
	
	ros::spin();
	
	return 0;
}
