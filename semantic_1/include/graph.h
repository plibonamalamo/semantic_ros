#ifndef __GRAPH_H__
#define	__GRAPH_H__

#include "linked_list.h"
#include <stdio.h>

typedef struct graph graph;
typedef struct graph_node graph_node;
typedef struct graph_node_prop graph_node_prop;

float dist(float xx, float yy, float xc, float yc);

graph * graph_new();
linked_list * graph_get_nodes(graph * g);
linked_list * graph_get_neighbors(graph * g, graph_node * n);
graph_node* graph_nearest_node(graph* g, float x, float y);
graph_node * graph_add_node(graph * g, void * value);
void* graph_new_node_prop(char* label, int is_something, float x, float y);
float graph_node_get_X(graph_node* n);
float graph_node_get_Y(graph_node* n);
char* graph_node_get_label(graph_node* n);
graph_node* graph_node_find_label(graph* g, char* label);
void graph_node_set_label(graph_node* n, char* label);
void* graph_DFS(graph_node* source, graph_node* target,linked_list* list);
int graph_node_get_is_something(graph_node* n);
void graph_add_edge(graph * g, graph_node * from, graph_node * to);
void * graph_get_node_value(graph_node * n);

#endif	/* __GRAPH_H__ */

