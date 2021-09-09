#include "list.h"

//Añade una suscripcion al final de la lista
int addSuscription(list_t* lista, char* tema, char* ip, int puerto) {
	node_t* aux = lista->first;

	while(aux != NULL) {
		if(puerto == aux->puerto && !strcmp(tema, aux->tema) && !strcmp(aux->ip, ip))
			return 0;
		aux = aux->next;
	}

	node_t* newNode = (node_t*) malloc(sizeof(node_t));

	if(newNode == NULL)
		return -1;

	memset((void*) newNode, 0, sizeof(node_t));

	strcpy(newNode->tema, tema);
	newNode->puerto = puerto;
	strcpy(newNode->ip, ip);

	if(lista->first == NULL && lista->last == NULL) {
		lista->first = newNode;
		lista->last = newNode;

		lista->numeroTotalSuscripciones++;
		return 0;	
	}

	lista->last->next = newNode;
	lista->last = newNode;

	lista->numeroTotalSuscripciones++;

	return 0;
}

//Borra una suscripcion de la lista
int removeSuscription(list_t* lista, char* tema, char* ip, int puerto) {
	if(lista->first == NULL)
		return 1;

	node_t* aux = lista->first;

	//Es el primer nodo
	if(lista->first->puerto == puerto && !strcmp(lista->first->tema, tema) && !strcmp(aux->ip, ip)) {
		lista->first = lista->first->next;
		if(lista->first == NULL)
			lista->last = NULL;
			
		free(aux);
		lista->numeroTotalSuscripciones--;
		return 0;
	}

	//Es el ultimo nodo
	if(lista->last->puerto == puerto && !strcmp(lista->last->tema, tema) && !strcmp(aux->ip, ip)) {
		while(aux->next->next != NULL)
			aux = aux->next;	

		free(lista->last);
		lista->last = aux;
		aux->next = NULL;

		lista->numeroTotalSuscripciones--;
		return 0;
	}

	//Es un nodo en el medio
	while(aux->next != NULL) {
		if(aux->next->puerto == puerto && !strcmp(aux->next->tema, tema) && !strcmp(aux->ip, ip)) {
			node_t* toDelete = aux->next;
			aux->next = aux->next->next;

			free(toDelete);
			lista->numeroTotalSuscripciones--;
			return 0;
		}

		aux = aux->next;
	}

	return 1;
}

//Vacia la lista
int destroy(list_t* lista) {
	node_t* aux;
	node_t* p = lista->first;

	if(p == NULL)
		return 0;
	else {
		while(p != NULL) {
			aux = p->next;
			free(p);
			p = aux;
		}

	}
	return 0;
}
