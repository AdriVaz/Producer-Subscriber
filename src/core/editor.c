#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lines.h"

void print_usage() {
	    printf("Usage: editor -h host -p puerto -t \"tema\" -m \"texto\"\n");
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char host[256]= "";
	char puerto[256]= "";
	char tema[256]= "";
	char texto[1025]= "";

	//############### PARSEAR ARGUMENTOS ###############
	while ((option = getopt(argc, argv,"h:p:t:m:")) != -1) {
		switch (option) {
		        case 'h' : 
				if(strlen(optarg) > 256) {
					printf("ERROR: host max length: 256 bytes\n");
					exit(-1);
				}
				strcpy(host, optarg);
		    		break;
		    	case 'p' : 
				if(strlen(optarg) > 256) {
					printf("ERROR: puerto max length: 256 bytes\n");
					exit(-1);
				}
				strcpy(puerto, optarg);
		    		break;
		    	case 't' : 
				if(strlen(optarg) > 128) {
					printf("ERROR: tema max length: 128 bytes\n");
					exit(-1);
				}
				strcpy(tema, optarg);
		    		break;
		    	case 'm' : 
				if(strlen(optarg) > 1024) {
					printf("ERROR: texto max length: 1024 bytes\n");
					exit(-1);
				}
				strcpy(texto, optarg);
		    		break;
		    	default: 
				print_usage(); 
		    		exit(-1);
		    }

	}

	if (strcmp(host,"")==0 || strcmp(puerto,"")==0 ||
		strcmp(tema,"")==0 || strcmp(texto,"")==0){
		print_usage(); 
		exit(-1);
	}

	printf("Host: %s\n", host);
	printf("Puerto: %s\n", puerto);
	printf("Tema: %s\n", tema);
	printf("texto: %s\n", texto);


	//Variables de la conexion
	int sd;
	struct sockaddr_in server_addr;
	struct hostent *hp;

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("ERROR en la creacion del socket\n");
		//perror("EDITOR: Error en el socket");
		return 0;
	}

	bzero((char*) &server_addr, sizeof(server_addr));
	hp = gethostbyname(host);

	memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(puerto));

	//Establecer conexion
	if(connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
		printf("Error in the connection to the server %s:%s\n", host, puerto);
		//perror("EDITOR: ERROR de conexion"); 
		return -1;
	}

	//############### ENVIAR PUBLICACION ###############
	char* operacion = "PUBLISH";
	
	if(enviar(sd,(void*) operacion,strlen(operacion)+1)==-1){
		perror("EDITOR: ERROR al enviar");
		close(sd);
		return -1;
	}
		

	if(enviar(sd,(void*) tema,strlen(tema)+1)==-1){
		perror("EDITOR: ERROR al enviar");
		close(sd);
		return -1;
	}

	if(enviar(sd,(void*) texto,strlen(texto)+1)==-1){
		perror("EDITOR: ERROR al enviar");
		close(sd);
		return -1;
	}

	
	close(sd);

	return 0;
}
	
