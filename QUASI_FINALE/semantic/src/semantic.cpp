#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/String.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <move_base_msgs/MoveBaseAction.h>

std::ofstream outf("/home/ubuntu/Desktop/sem.txt");

float _X;
float _Y;
int esplora;
ros::Publisher move_base_pub;
int flag_DFS;
ros::Publisher speech_pub;
int cont;

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

void linked_list_remove_first(linked_list* ll){
	linked_list* ptr = (linked_list*)ll;
	if(ptr->tail == NULL) return;
	linked_list_node* current = ptr->head;
	linked_list_node* next = current->next;
	ptr->head = next;
	ptr->size--;
	free(current);
	return;
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
graph_node* part;
linked_list* list;

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
	//if(n->value == NULL) std::cout << "VALLUE UL" << std::endl;
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
	//if(p == NULL || n == NULL) std::cout << "NUll in get label" << std::endl;
	return p->label;
}

graph_node* graph_node_find_label(graph* g,const char* label){
	//<< "\tCerco label " << label << std::endl;
	linked_list_iterator* it = linked_list_iterator_new(g->nodes);
	while(it!=NULL){
		//<< "\tWHILE" << std::endl;
		graph_node* n = (graph_node*)linked_list_iterator_getvalue(it);
		graph_node_prop* n_prop =(graph_node_prop*) graph_get_node_value(n);
		const char* n_label = graph_node_get_label(n);
		//<< "CICLO: " << n_label << std::endl;
		if(strcmp(n_label,label)==0){
			//<< "Trovato" << std::endl;
			return n;
		}
		//<< "\tPasso " << n_prop->label << std::endl;
		it = linked_list_iterator_next(it);
	}
	return NULL;
}

graph_node* graph_nearest_node(graph* g, float x, float y){
	//std::cout << "NEAREST" << std::endl;
	int dist_prov,dist_min;
	graph_node* n_max = part;
	linked_list_iterator* it = linked_list_iterator_new(g->nodes);
	//if(it == NULL) std::cout << " IT NULL" << std::endl;
	while(it!=NULL){
		graph_node* n = (graph_node*)linked_list_iterator_getvalue(it);
		//graph_node_prop* n_prop = (graph_node_prop*)graph_get_node_value(n);
		//std::cout << "NEA "<<graph_node_get_label(n) << std::endl;
		float xn = graph_node_get_X(n);
		float yn = graph_node_get_Y(n);
			//std::cout << "asdadsd" << xn << yn << std::endl;
		dist_prov = dist(xn,yn,x,y);
		if(dist_prov <= dist_min){
			dist_min = dist_prov;
			//std::cout << " DIST" << std::endl;
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
	//<< "Aggiunto nodo source" << std::endl;
	//<< "Aggiunto nodo source" << std::endl;
	const char* label_s = graph_node_get_label(source);
	const char* label_t = graph_node_get_label(target);
	// <<  "\t\tSOURCE/DEST " << label_s << "/" << label_t << std::endl;
	// << "\t\tFLAG: " << flag_DFS << std::endl;
	if(strcmp(label_s,label_t)==0) {
		flag_DFS = 1;
		// <<  "Cambio flag  " << flag_DFS << std::endl;
		return;
	}
	linked_list_iterator* it = linked_list_iterator_new(source->out_edges);
	//<< "Creato Iteratore" << std::endl;
	for (; it && !flag_DFS;) {
		linked_list_iterator* itera = linked_list_iterator_next(it);
		//<< "Preso Itera" << std::endl;
		graph_node* nxt = (graph_node*)linked_list_iterator_getvalue(it);
		//<< "Preso next: " << graph_node_get_label(nxt) << std::endl;
		if (nxt->state == UNEXPLORED){
			//<< "Parto nuova DFS" << std::endl;
			//linked_list_add(list,(void*)source);
			// << "Aggiunto next" << std::endl;
			graph_DFS(nxt,target,list);
			//<< "Esco DFS  " << flag_DFS << std::endl;
			if(!flag_DFS==1) linked_list_remove_last(list);
			//if(!flag_DFS==1) //<< "Rimuovo da lista" << std::endl;
		}
		it = itera;
	}
	//linked_list_remove_first(list);
	source->state = EXPLORED;
}

char* str_to_char(std::string str){
	char* buffer = (char*)malloc(sizeof(char)*str.size());
	std::size_t length = str.copy(buffer,str.size(),0);
	buffer[length]='\0';
	return buffer;
}

void poseCallback(const nav_msgs::Odometry::ConstPtr& msg) {
	_X = msg->pose.pose.position.x;
	_Y = msg->pose.pose.position.y;
	//std::cout << "Trovo il piu vicino cont:" << cont << std::endl;
	graph_node* nearest = graph_nearest_node(g,_X,_Y);
	//std::cout << "nodo vicno" << std::endl;
	if(nearest == NULL){
		 nearest = part;
		 std::cout<< "NULLO"<<std::endl;
	 }
	const char* nearest_label = graph_node_get_label(nearest);
	//std::cout << "preso label" << std::endl;
	float nearest_x = graph_node_get_X(nearest);
	std::string nea = std::string(nearest_label);
	//std::cout << "Trovato più vicino: " << nea << "  " << nearest_x << std::endl;
	float x_nearest = graph_node_get_X(nearest);
	float y_nearest = graph_node_get_Y(nearest);
	// << "Prendo xy:" << x_nearest << " " << y_nearest << std::endl;
	if(dist(x_nearest,y_nearest,_X,_Y) > 1){
		std::string num = std::to_string(cont);
		const char* num_c = str_to_char(num);
		// << "\t\tTNU M: " << num << "     " << num_c << std::endl;
		graph_node* n = graph_add_node(g,(void*)graph_new_node_prop(num_c,0,_X,_Y));						
		graph_add_edge(g,n,nearest);
		cont++;
		float xn = graph_node_get_X(n);
		float yn = graph_node_get_Y(n);
		std::cout << "aggiungo " << num_c << " " << xn << " " << yn << std::endl;
		//std::cout << "edge " << xn << yn << std::endl;
	}
	std_msgs::String str;
	if(cont == 3){
		str.data = "siamo in cucina";
		speech_pub.publish(str);
		cont++;
	}
	if(cont == 7){
		str.data = "vai in cucina";
		speech_pub.publish(str);
		cont++;
	}
}

std::string prendi_inizio(std::string msg){
	int x = 0,last_space;
	for (std::string::reverse_iterator rit=msg.rbegin(); rit!=msg.rend(); ++rit){
		//<< "\t\t\t" << *rit << std::endl;
		if(*rit == ' ') break; 
    x++;
	}
	last_space = msg.size() - x;
	std::string inizio = msg.substr(0,last_space-1);
	return inizio;
}

std::string prendi_fine(std::string msg){
	int x = 0,last_space;
	for (std::string::reverse_iterator rit=msg.rbegin(); rit!=msg.rend(); ++rit){
		//<< "\t\t\t" << *rit << std::endl;
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
  // << "INVIATO" << std::endl;

}

void speechCallback(const std_msgs::String::ConstPtr& msg){
	//<< "ARRIVATO: " << msg->data << std::endl;
	std::string inizio;
	std::string fine;
	fine = prendi_fine(msg->data);
	inizio = prendi_inizio(msg->data);
	//<< "INIZIO/FINE: " << inizio << "/" << fine << std::endl;
	if(inizio == "siamo in"){
		//<< "siamo in " <<  std::endl;
		graph_node* nodo = graph_nearest_node(g,_X,_Y);
		//<< "Trovato più vicino " <<  std::endl;
		
		char* label_c = str_to_char(fine);
		// << "buffer contains: " << label_c << '\n';
		
		graph_node_set_label(nodo,label_c);
		//<< "Settata label: " << fine << std::endl;
	}
	if(inizio == "vai in"){
		//<< "Vai in" << std::endl;
		char* fine_c = str_to_char(fine);
		//<< "Cerco " << fine_c << std::endl;
		graph_node* source = graph_nearest_node(g,_X,_Y);
		//<< "Triovato source " << graph_node_get_label(source) << " " << graph_node_get_X(source) << std::endl;
		graph_node* dest = graph_node_find_label(g,fine_c);
		//<< "Trovato dest " << graph_node_get_label(dest) << " " << graph_node_get_X(dest) << std::endl;
		//<< "Creata List" << std::endl;
		graph_DFS(source,dest,list);
		flag_DFS = 0;
		//<< "DFS <3" << std::endl;
		linked_list_iterator* it = linked_list_iterator_new(list);
		graph_node* n = (graph_node*)linked_list_iterator_getvalue(it);
		float nx = graph_node_get_X(n);
		float ny = graph_node_get_Y(n);
		const char* n_lab = graph_node_get_label(n);
		std::cout<< "Vado al Nodo: " << n_lab << "X: " << nx << "Y: " << ny << std::endl;
		go_to(nx,ny);
		//<< "Andato" << std::endl;
		linked_list_remove_first(list);
	}
}

void resultCallback(const move_base_msgs::MoveBaseActionResult::ConstPtr& msg){
	linked_list_iterator* it = linked_list_iterator_new(list);
	if(!it) return;
	graph_node* n = (graph_node*)linked_list_iterator_getvalue(it);
	float nx = graph_node_get_X(n);
	float ny = graph_node_get_Y(n);
	std::cout<< "Vado al Nodo: " << graph_node_get_label(n) << "X: " << nx << "Y: " << ny << std::endl;
	go_to(nx,ny);
	linked_list_remove_first(list);
}
int main(int argc, char** argv) {
	ros::init(argc, argv, "semantic");
	ros::NodeHandle nh;
	
	// << "open" << std::endl;
	list = linked_list_new();
	flag_DFS = 0;
	cont = 0;
	_X = 0;
	_Y = 0;
	g = graph_new();
	part = graph_add_node(g,(void*)graph_new_node_prop("partenza",1,_X,_Y));
	//*/
	// << "Nodo 1" << std::endl;
	//SUB
	ros::Subscriber odom_sub = nh.subscribe("/diago/base_pose_ground_truth", 10, &poseCallback);
	ros::Subscriber speech_sub = nh.subscribe("/best_speech_hypotesis", 10, &speechCallback);
	ros::Subscriber result_sub = nh.subscribe("/diago/move_base/result",10,&resultCallback);
	
	//PUB
	move_base_pub = nh.advertise<geometry_msgs::PoseStamped>("/diago/move_base_simple/goal", 1);
	speech_pub = nh.advertise<std_msgs::String>("/best_speech_hypotesis", 1);
	//twist_pub = nh.advertise<geometry_msgs::Twist>("/diago/cmd_vel", 10);
	
	//go_to(1.25,2.0);
	//ros::Subscriber aiml_sub = nh.subscribe(".......");
	
	//tf::TransformListener listener;
	//listener_ptr = &listener;
	
	ros::spin();
	
	return 0;
}
