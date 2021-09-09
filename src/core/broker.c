#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>

#include "lines.h"
#include "list.h"
#include "servidorAlmacenamiento.h"


//Mutex para servidor concurrente
pthread_mutex_t m;
pthread_attr_t attr;
pthread_cond_t copiado;
pthread_t thid;
int bool_copiado = 0;

list_t suscripciones;
char rpcHost[256] = "";

//Funcion de hilo para tratar las peticiones enviadas al servidor
void* tratar_peticion(int* sock) {
	pthread_mutex_lock(&m);

	int sc = *sock;

	bool_copiado = 1;
	pthread_cond_signal(&copiado);

	pthread_mutex_unlock(&m);

	char buffer[1025] = "";
	char tema[129] = "";
	char texto[1025] = "";
	int puertoLocal = -1;
	int retorno = -1;


	//Variables para RPC
	CLIENT* clnt;

	//Variables RPC para storeMessage
	enum clnt_stat retval_2;
	int result_2;
	char *storemessage_1_topic;
	char *storemessage_1_msg;

	//Variables RPC para getLastMessage
	enum clnt_stat retval_3;
	char *result_3;
	char *getlastmessage_1_topic;

	//Lectura del codigo de operacion
	readLine(sc, buffer, 256);
	printf("############### %s ###############\n", buffer);

	if(strcmp(buffer, "PUBLISH") == 0) {
		//############### LEER TEMA Y TEXTO ###############
		//Leer tema
		readLine(sc, tema, 129);
		printf("TEMA: %s\n", tema);

		//Teer texto
		readLine(sc, texto, 1025);
		printf("TEXTO: %s\n", texto);

		//############### ALMACENAR EN EL SERVIDOR DE MENSAJES ###############
		//Espacio para los parametros de RPC
		storemessage_1_topic = (char*) malloc(129);
		storemessage_1_msg = (char*) malloc(1025);

		clnt = clnt_create (rpcHost, SERVIDOR_ALMACENAMIENTO, PRIMERA_VERSION, "tcp");
		if (clnt == NULL) {
			clnt_pcreateerror(rpcHost);
			pthread_exit(0);
		}

		strcpy(storemessage_1_topic, tema);
		strcpy(storemessage_1_msg, texto);

		retval_2 = storemessage_1(storemessage_1_topic, storemessage_1_msg, &result_2, clnt);
		if (retval_2 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
			pthread_exit(0);
		}
		clnt_destroy (clnt);
		free(storemessage_1_topic);
		free(storemessage_1_msg);
		printf("Message saved in RPC server\n");

		//############### REENVIAR A SUSCRIPTORES ###############
		int sd;
		struct sockaddr_in server_addr;
		struct hostent *hp;
		node_t* aux = suscripciones.first;
		
		//Iterar la lista buscando el tema
		while(aux != NULL) {
			if(!strcmp(tema, aux->tema)) {
				if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
					printf("CLIENT: Error en el socket");
					return 0;
				}

				//Iniciar conexion con el hilo del suscriptor
				bzero((char*) &server_addr, sizeof(server_addr));
				hp = gethostbyname(aux->ip);

				memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
				server_addr.sin_family = AF_INET;

				server_addr.sin_port = htons(aux->puerto);
				if(connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
					removeSuscription(&suscripciones, tema, inet_ntoa(server_addr.sin_addr), puertoLocal);
					printf("Suscriptor not connecting. Unsubscribed from %s\n", tema);
				} else {
					//Enviar datos
					enviar(sd, tema, 129);
					enviar(sd, texto, 1025);

					printf("\tReenvio: %s : %s al puerto %d\n", tema, texto, aux->puerto);

					close(sd);

				}
			}
			aux = aux->next;
		}

	} else if(strcmp(buffer, "SUBSCRIBE") == 0) {
		readLine(sc, tema, 129);
		readLine(sc, buffer, 6); //Puerto
		puertoLocal = atoi(buffer);

		struct sockaddr_in sckt;
		socklen_t len = sizeof(sckt);
		getsockname(sc,(struct sockaddr*) &sckt, &len);

		printf("Client IP: %s\n", inet_ntoa(sckt.sin_addr));
		printf("Topic: %s\n", tema);

		retorno = addSuscription(&suscripciones, tema, inet_ntoa(sckt.sin_addr), puertoLocal);
		enviar(sc, (retorno == 0 ? "0" : "1"), 1);

		//############### REENVIAR ULTIMO MENSAJE ###############
		result_3 = (char*) malloc(1025);
		getlastmessage_1_topic = (char*) malloc(129);

		//Crear conexion RPC
		clnt = clnt_create (rpcHost, SERVIDOR_ALMACENAMIENTO, PRIMERA_VERSION, "tcp");
		if (clnt == NULL) {
			clnt_pcreateerror(rpcHost);
			pthread_exit(0);
		}

		strcpy(getlastmessage_1_topic, tema);

		//Leer ultimo mensaje de servidor RPC
		retval_3 = getlastmessage_1(getlastmessage_1_topic, &result_3, clnt);
		if (retval_3 != RPC_SUCCESS) {
			clnt_perror (clnt, "call failed");
			pthread_exit(0);
		}
		clnt_destroy (clnt);
		free(getlastmessage_1_topic);
		printf("Read last message from RPC server: %s\n", result_3);

		//Si hay un mensaje, se envia al suscriptor
		if(strcmp(result_3, "") != 0) {
			int sd;
			struct sockaddr_in server_addr;
			struct hostent *hp;
			
			if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				printf("CLIENT: Error en el socket");
				return 0;
			}

			bzero((char*) &server_addr, sizeof(server_addr));
			hp = gethostbyname(inet_ntoa(sckt.sin_addr));

			memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
			server_addr.sin_family = AF_INET;

			server_addr.sin_port = htons(puertoLocal);
			if(connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
				removeSuscription(&suscripciones, tema, inet_ntoa(sckt.sin_addr), puertoLocal);
				printf("Suscriptor not connecting. Unsubscribed from %s\n", tema);
			} else {
				enviar(sd, tema, 129);
				enviar(sd, result_3, 1025);

				printf("\tReenvio de RPC: %s : %s al puerto %d\n", tema, result_3, puertoLocal);

				close(sd);
			}

		}
		free(result_3);

	}else if(strcmp(buffer, "UNSUBSCRIBE") == 0) {
		readLine(sc, tema, 129);
		readLine(sc, buffer, 6);
		puertoLocal = atoi(buffer);

		struct sockaddr_in sckt;
		socklen_t len = sizeof(sckt);
		getsockname(sc,(struct sockaddr*) &sckt, &len);

		printf("UNSUBSCRIBE: ip es: %s\n", inet_ntoa(sckt.sin_addr));

		//Sacar suscripcion de la lista
		retorno = removeSuscription(&suscripciones, tema, inet_ntoa(sckt.sin_addr), puertoLocal);

		enviar(sc, (retorno == 0 ? "0" : "1"), 1);

	} else {
		printf("Operacion no reconocida: %s\n", buffer);
	}
		
	close(sc);
	printf("############### CONEXION CERRADA ###############\n\n");

	pthread_exit(0);
}

void print_usage() {
	printf("Usage: broker -p puerto -r servidor_almacenamiento \n");
}

//Manejador para SIGINT
//Cuando se finaliza el broker con Ctrl-C se libera la lista de suscripciones
void sigintHandler() {
	destroy(&suscripciones);
	exit(0);
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char puerto[256]= "";
	//char buffer[256]="";
	
	//Variables de la RPC
	CLIENT* clnt;
	enum clnt_stat retval_1;
	int result_1;

	//############### PARSEAR ARGUMENTOS ###############
	while ((option = getopt(argc, argv,"p:r:")) != -1) {
		switch (option) {
		    	case 'p' : 
				strcpy(puerto, optarg);
		    		break;
			case 'r' :
				strcpy(rpcHost, optarg);
				break;
		    	default: 
				print_usage(); 
		    		exit(-1);
		    }
	}

	//############### COMPROBACIONES E INICIALIZACIONES ###############

	//Los parametros no pueden ser nulos
	if (strcmp(puerto,"")==0){
		print_usage(); 
		exit(-1);
	}

	if (strcmp(rpcHost,"")==0){
		print_usage(); 
		exit(-1);
	}

	printf("Puerto: %s\n", puerto);
	printf("rpcHost: %sd\n", rpcHost);

	//Inicializar valores de la lista de suscripciones
	suscripciones.first = NULL;
	suscripciones.last = NULL;
	suscripciones.numeroTotalSuscripciones = 0;

	//Variables de la conexion
	struct sockaddr_in server_addr, client_addr;
	int sd, sc;
	int val;
	int size;

	//Crear socket
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("SERVER: Error en el socket");
		return 0;
	}

	//Asignar valores al socket
	val = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(atoi(puerto));

	//Abrir socket
	bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
	listen(sd, SOMAXCONN);

	//Mutex para servidor concurrente
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&copiado, NULL);
	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//Capturar SIGINT para liberar memoria de la lista
	struct sigaction act;
	act.sa_handler = sigintHandler;
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	//Inicializar servidor de almacenamiento de mensajes
	clnt = clnt_create (rpcHost, SERVIDOR_ALMACENAMIENTO, PRIMERA_VERSION, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror(rpcHost);
		return -1;
	}

	retval_1 = init_1(&result_1, clnt);
	if (retval_1 != RPC_SUCCESS) {
		clnt_perror(clnt, "call failed");
		return -1;
	}
	clnt_destroy (clnt);
	printf("RPC INIT DONE\n");

	//############### BUCLE PRINCIPAL ###############
	while(1) {
		sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);

		pthread_create(&thid, &attr, (void*) tratar_peticion, &sc);

		//Copiar parametros al hilo
		pthread_mutex_lock(&m);

		while(!bool_copiado)
			pthread_cond_wait(&copiado, &m);

		bool_copiado = 0;

		pthread_mutex_unlock(&m);
		
	}

	close(sd);
	
	return 0;
}
	
