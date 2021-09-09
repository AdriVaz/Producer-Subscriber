#ifndef _LIST_H
#define _LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
	char ip[16];
	int puerto;
	char tema[128];

	struct node* next;
} node_t;

typedef struct list {
	node_t* first;
	node_t* last;
	int numeroTotalSuscripciones;
} list_t;


int addSuscription(list_t* lista, char* tema, char* ip, int puerto);
int removeSuscription(list_t* lista, char* tema, char* ip, int puerto);
int destroy(list_t* lista);

#endif
