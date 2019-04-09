#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/String.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <move_base_msgs/MoveBaseAction.h>

std::ofstream outf("/home/ubuntu/Desktop/sem.txt");

float _X;
float _Y;
int esplora;
ros::Publisher move_base_pub;


typedef enum {UNEXPLORED, EXPLORED, EXPLORING} STATUS;

typedef struct linked_list_node {
	void * value;
	struct linked_list_node * next;
} linked_list_node;

typedef struct linked_list {
	linked_list_node *head;
	linked_list_node *tail;
	int size;
} linked_list;

typedef struct linked_list_iterator linked_list_iterator;

linked_list * linked_list_new() {
    linked_list * ptr = (linked_list *) malloc(sizeof(linked_list));
    ptr->head = NULL;
    ptr->tail = NULL;
    ptr->size = 0;
    return (linked_list *) ptr;
}

void * linked_list_get(linked_list * ll, int index) {

    linked_list * ptr = (linked_list *) ll;
    if (index < 0 || index >= ptr->size)
        return NULL;
    
    int i;
    linked_list_node *current = ptr->head;
    for (i = 0; i < index && current != NULL; i++) {
        current = current->next;
    }
    if (current == NULL)
        return NULL;

    return current->value;
}

void linked_list_add(linked_list * ll, void * value) {

    linked_list * ptr = (linked_list *) ll;
    linked_list_node * added = (linked_list_node *) malloc(sizeof(linked_list_node));
    added->next = NULL;
    added->value = value;

	if (ptr->tail != NULL) {
		ptr->tail->next = added;
	}
    
	if (ptr->head == NULL) {
		ptr->head = added;
	}
    ptr->tail = added;
    ptr->size++;

    /*
    printf("added node: %p - size: %d - head: %p - tail: %p\n", value, ptr->size,
        ptr->head != NULL ? ptr->head->value : NULL, 
        ptr->tail != NULL ? ptr->tail->value : NULL);
    */
}

void linked_list_remove_last(linked_list *ll) {

    linked_list * ptr = (linked_list *) ll;

    if (ptr->tail == NULL)
        return;

    linked_list_node * current = ptr->head;
    linked_list_node * previous = NULL;

    // inefficient: use a double linked list!
    while(current->next != NULL) {
        previous = current;
        current = current->next;
    }

    ptr->tail = previous;
    
    if (previous == NULL)
        ptr->head = current->next;
    else
        previous->next = current->next;
    
    ptr->size--;
    /*
    printf("removed node: %p - size: %d - head: %p - tail: %p\n", current->value, ptr->size, 
        ptr->head != NULL ? ptr->head->value : NULL, 
        ptr->tail != NULL ? ptr->tail->value : NULL);
    */

    free(current);
    return;
}

void linked_list_delete(linked_list *l) {
    if (l == NULL) {
        return;
    }
	linked_list_node * it = l->head;

	while (it != NULL) {
		linked_list_node * rem = it;
		it = it->next;
		free(rem);
	}
	//free(it);
    free(l);
    return;
}

linked_list_iterator * linked_list_iterator_new(linked_list *ll) {
    linked_list *ptr = (linked_list *) ll;
    return (linked_list_iterator *) ptr->head;
}

linked_list_iterator * linked_list_iterator_next(linked_list_iterator * iter) {
    linked_list_node *ptr = (linked_list_node *) iter;
	if(ptr!= NULL)
		return (linked_list_iterator*)ptr->next;
	return NULL;
}

void * linked_list_iterator_getvalue(linked_list_iterator *iter) {
    linked_list_node *ptr = (linked_list_node *) iter;
    return ptr->value;
}

int linked_list_size(linked_list *ll) {
    linked_list *ptr = (linked_list *) ll;
    return ptr->size;
}

struct graph_node_prop{
	const char* label;
	int is_something;
	float x,y;
};

struct graph_node{
    void * value;
    linked_list * out_edges;
    STATUS state;
};
struct graph_prop {
	int n_vertices;
	int n_edges;
}graph_prop;

struct graph{
    linked_list * nodes;
	struct graph_prop* properties;
};

typedef struct graph graph;
typedef struct graph_node graph_node;
typedef struct graph_node_prop graph_node_prop;

graph* g; 

float dist(float xx, float yy, float xc, float yc){
	return sqrt(pow(abs(xx-xc),2)+pow(abs(yy-yc),2));
}

graph * graph_new() {
	graph * g = (graph *) malloc(sizeof(graph));
    g->nodes = linked_list_new();
	g->properties = (struct graph_prop*)malloc(sizeof(graph_prop));
	g->properties->n_edges = 0;
	g->properties->n_vertices = 0;
    return g;
}

linked_list * graph_get_nodes(graph * g) {
	return g->nodes;
}

linked_list * graph_get_neighbors(graph * g, graph_node * n) {
    return n->out_edges;
}

graph_node * graph_add_node(graph * g, void * value) {
	linked_list_iterator *it = linked_list_iterator_new(g->nodes);
	graph_node * n = (graph_node *) malloc(sizeof(graph_node));
	n->value = value;
    n->out_edges = linked_list_new();
	n->state = UNEXPLORED;
    linked_list_add(g->nodes, (void *) n);
	(g->properties->n_vertices)++;
    return n;
}

void graph_add_edge(graph * g, graph_node * v1, graph_node * v2) {
    linked_list_add(v1->out_edges, (void *) v2);
    linked_list_add(v2->out_edges, (void *) v1);
	(g->properties->n_edges)++;
}

void * graph_get_node_value(graph_node * n) {
	return n->value;
}

void* graph_new_node_prop(const char* label, int is_something, float x, float y){
	graph_node_prop* p = (graph_node_prop*)malloc(sizeof(graph_node_prop));
	p->label = label;
	p->is_something = is_something;
	p->x = x;
	p->y = y;
	return (void*)p;
}

float graph_node_get_X(graph_node* n){
	graph_node_prop* p = (graph_node_prop*)graph_get_node_value(n);
	return p->x;
}

float graph_node_get_Y(graph_node* n){
	graph_node_prop* p = (graph_node_prop*)graph_get_node_value(n);
	return p->y;
}

int graph_node_get_is_something(graph_node* n){
	graph_node_prop* p = (graph_node_prop*)graph_get_node_value(n);
	return p->is_something;
}

const char* graph_node_get_label(graph_node* n){
	graph_node_prop* p = (graph_node_prop*)graph_get_node_value(n);
	return p->label;
}

graph_node* graph_node_find_label(graph* g,const char* label){
	linked_list_iterator* it = linked_list_iterator_new(g->nodes);
	while(it!=NULL){
		graph_node* n = (graph_node*)linked_list_iterator_getvalue(it);
		graph_node_prop* n_prop =(graph_node_prop*) graph_get_node_value(n);
		if(n_prop->is_something==1 && strcmp(n_prop->label,label)==0){
			return n;
		}
		it = linked_list_iterator_next(it);
	}
	return NULL;
}

graph_node* graph_nearest_node(graph* g, float x, float y){
	int dist_prov,dist_min;
	graph_node* n_max = NULL;
	linked_list_iterator* it = linked_list_iterator_new(g->nodes);
	while(it!=NULL){
		graph_node* n = (graph_node*)linked_list_iterator_getvalue(it);
		graph_node_prop* n_prop = (graph_node_prop*)graph_get_node_value(n);
		dist_prov = dist(n_prop->x,n_prop->y,x,y);
		if(dist_prov < dist_min){
			dist_min = dist_prov;
			n_max = n;
		}
		it = linked_list_iterator_next(it);
	}
	return n_max;
}

void graph_node_set_label(graph_node* n, const char* label){
	graph_node_prop* p = (graph_node_prop*)graph_get_node_value(n);
	p->label = label;
}

void graph_DFS(graph_node* source, graph_node* target,linked_list* list){
	source->state = EXPLORING;
	linked_list_add(list,(void*)source);
	linked_list_iterator* it = linked_list_iterator_new(source->out_edges);
	for (; it;) {
		linked_list_iterator* itera = linked_list_iterator_next(it);
		graph_node* nxt = (graph_node*)linked_list_iterator_getvalue(it);
		const char* label_s = graph_node_get_label(source);
		const char* label_t = graph_node_get_label(target);
		if(strcmp(label_s,label_t)==0) return;
		if (nxt->state == UNEXPLORED){
			graph_DFS(nxt,target,list);
			linked_list_remove_last(list);
		}
		it = itera;
	}
	source->state = EXPLORED;
}

void poseCallback(const nav_msgs::Odometry::ConstPtr& msg) {
	_X = msg->pose.pose.position.x;
	_Y = msg->pose.pose.position.y;
	//std::cout << "Trovo il piu vicino" << std::endl;
	graph_node* nearest = graph_nearest_node(g,_X,_Y);
	//std::cout << "Trovato" << std::endl;
	float x_nearest = graph_node_get_X(nearest);
	float y_nearest = graph_node_get_Y(nearest);
	//std::cout << "Prendo xy:" << x_nearest << " " << y_nearest << std::endl;
	if(dist(x_nearest,y_nearest,_X,_Y) > 1){
		graph_node* n = graph_add_node(g,graph_new_node_prop("X",0,_X,_Y));						
		//std::cout << "aggiungo" << std::endl;
		graph_add_edge(g,n,nearest);
		//std::cout << "edge" << std::endl;
	}
}

std::string prendi_inizio(std::string msg){
	int x = 0,last_space;
	for (std::string::reverse_iterator rit=msg.rbegin(); rit!=msg.rend(); ++rit){
		std::cout<< "\t\t\t" << *rit << std::endl;
		if(*rit == ' ') break; 
    x++;
	}
	last_space = msg.size() - x;
	std::string inizio = msg.substr(0,last_space);
	return inizio;
}

std::string prendi_fine(std::string msg){
	int x = 0,last_space;
	for (std::string::reverse_iterator rit=msg.rbegin(); rit!=msg.rend(); ++rit){
		std::cout<< "\t\t\t" << *rit << std::endl;
		if(*rit== ' ') break; 
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
	std::cout<< "ARRIVATO: " << msg->data << std::endl;
	std::string inizio;
	std::string fine;
	fine = prendi_fine(msg->data);
	inizio = prendi_inizio(msg->data);
	std::cout<< "INIZIO/FINE: " << inizio << "/" << fine << std::endl;
	if(inizio == "siamo in"){
		std::cout<< "siamo in " <<  std::endl;
		graph_node* nodo = graph_nearest_node(g,_X,_Y);
		std::cout<< "Trovato più vicino " <<  std::endl;
		graph_node_set_label(nodo,fine.c_str());
		std::cout<< "Settata label: " << fine << std::endl;
	}
	if(inizio == "vai in"){
		std::cout<< "Vai in" << std::endl;
		graph_node* source = graph_node_find_label(g,fine.c_str());
		std::cout<< "Triovato source" << std::endl;
		graph_node* dest = graph_nearest_node(g,_X,_Y);
		std::cout<< "Trovato dest" << std::endl;
		linked_list* list = linked_list_new();
		std::cout<< "Creata List" << std::endl;
		graph_DFS(source,dest,list);
		std::cout<< "DFS <3" << std::endl;
		linked_list_iterator* it = linked_list_iterator_new(list);
		while(it!=NULL){
			std::cout<< "Ciclo nodi" << std::endl;
			graph_node* n = (graph_node*)linked_list_iterator_getvalue(it);
			int nx = graph_node_get_X(n);
			int ny = graph_node_get_Y(n);
			std::cout<< "Nodo: " << nx << "/" << ny << std::endl;
			go_to(nx,ny);
			std::cout<< "Andato" << std::endl;
			it = linked_list_iterator_next(it);
		}
	}
}

int main(int argc, char** argv) {
	ros::init(argc, argv, "semantic");
	ros::NodeHandle nh;
	
	std::cout << "open" << std::endl;
	
	_X = 0;
	_Y = 0;
	g = graph_new();
	graph_add_node(g,graph_new_node_prop("partenza",1,_X,_Y));
	//*/
	std::cout << "Nodo 1" << std::endl;
	//SUB
	ros::Subscriber odom_sub = nh.subscribe("/diago/odom", 10, &poseCallback);
	ros::Subscriber speech_sub = nh.subscribe("/best_speech_hypotesis", 10, &speechCallback);
	
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
