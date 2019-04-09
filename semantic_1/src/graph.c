#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef enum {UNEXPLORED, EXPLORED, EXPLORING} STATUS;

struct graph_node_prop{
	char* label;
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

float dist(float xx, float yy, float xc, float yc){
	return sqrt(pow(abs(xx-xc),2)+pow(abs(yy-yc),2));
}

void* graph_new_node_prop(char* label, int is_something, float x, float y){
	graph_node_prop* p = (graph_node_prop*)malloc(sizeof(graph_node_prop));
	p->label = label;
	p->is_something = is_something;
	p->x = x;
	p->y = y;
	return (void*)p;
}

float graph_node_get_X(graph_node* n){
	graph_node_prop* p = graph_get_node_value(n);
	return p->x;
}

float graph_node_get_Y(graph_node* n){
	graph_node_prop* p = graph_get_node_value(n);
	return p->y;
}

int graph_node_get_is_something(graph_node* n){
	graph_node_prop* p = graph_get_node_value(n);
	return p->is_something;
}

char* graph_node_get_label(graph_node* n){
	graph_node_prop* p = graph_get_node_value(n);
	return p->label;
}

graph_node* graph_node_find_label(graph* g, char* label){
	linked_list_iterator* it = linked_list_iterator_new(g->nodes);
	while(it!=NULL){
		graph_node* n = linked_list_iterator_getvalue(it);
		graph_node_prop* n_prop = graph_get_node_value(n);
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
		graph_node* n = linked_list_iterator_getvalue(it);
		graph_node_prop* n_prop = graph_get_node_value(n);
		dist_prov = dist(n_prop->x,n_prop->y,x,y);
		if(dist_prov < dist_min){
			dist_min = dist_prov;
			n_max = n;
		}
		it = linked_list_iterator_next(it);
	}
	return n_max;
}

void graph_node_set_label(graph_node* n, char* label){
	graph_node_prop* p = graph_get_node_value(n);
	p->label = label;
}

void* graph_DFS(graph_node* source, graph_node* target,linked_list* list){
	source->state = EXPLORING;
	linked_list_add(list,(void*)source);
	linked_list_iterator* it = linked_list_iterator_new(source->out_edges);
	for (; it;) {
		linked_list_iterator* itera = linked_list_iterator_next(it);
		graph_node* nxt = (graph_node*)linked_list_iterator_getvalue(it);
		char* label_s = graph_node_get_label(source);
		char* label_t = graph_node_get_label(target);
		if(strcmp(label_s,label_t)==0) return;
		if (nxt->state == UNEXPLORED){
			DFS(nxt,target,list);
			linked_list_remove_last(list);
		}
		it = itera;
	}
	source->state = EXPLORED;
}
