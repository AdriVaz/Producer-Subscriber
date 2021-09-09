#include "fileManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int fm_init() {
	//Borra la carpeta que contiene los ficheros de texto
	char command[32];
	sprintf(command, "%s%s", "rm -rf ", TOPICS_DIR);

	system(command);

	//Crea la carpeta de temas
	if(mkdir(TOPICS_DIR, 0775) != 0)
		return 1;

	return 0;
}

//Almacena un texto
int fm_storeMessage(char* topic, char* msg) {
	int fd;
	char fileName[256];
	char msgBuffer[1024] = "";

	//Ruta del fichero que contiene el tema
	sprintf(fileName, "%s/%s", TOPICS_DIR, topic);

	//Asegurar que el mensaje sea de longitud 1024
	strcpy(msgBuffer, msg);

	//Abrir o crear el fichero correspondiente
	if((fd = open(fileName, O_CREAT | O_APPEND | O_WRONLY, 0775)) < 0)
		return 1;

	//Escribir todos los caracteres
	int pos = 0;
	int ret = 0;
	int msgSize = sizeof(msgBuffer);

	//Asegurar que se escriben todos los caracteres en el fichero
	do {
		ret = write(fd, msgBuffer + pos, msgSize - pos);
		pos += ret;
	} while(pos < msgSize && ret >= 0);
	
	close(fd);

	return ret < 0;
}

//Obtiene el ultimo mensaje
int fm_getLastMessage(char* topic, char* msgBuffer) {
	int fd;
	char fileName[256];
	strcpy(msgBuffer, "");

	//Ruta del fichero
	sprintf(fileName, "%s/%s", TOPICS_DIR, topic);

	//Si el fichero no existe, el tema no existe
	if((fd = open(fileName, O_RDONLY)) < 0)
		return 0;

	//Leer mensaje del fichero
	int pos = 0;
	int ret = 0;
	int msgSize = 1024;

	lseek(fd, -1024,SEEK_END);

	//Asegurar que se leen todos los caracteres
	do {
		ret = read(fd, msgBuffer + pos, msgSize - pos);
		pos += ret;
	} while(pos < msgSize && ret >= 0);
	
	close(fd);

	return ret < 0;
}
