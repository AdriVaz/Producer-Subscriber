BIN_FILES = editor broker servidorAlmacenamiento formatterService.class suscriptor.class

SRC_DIR = src
INCLUDE_DIR = $(SRC_DIR)/include
OBJ_DIR = object
CORE_DIR = $(SRC_DIR)/core
RPC_DIR = $(SRC_DIR)/rpc
WEBSERVICE_DIR = $(SRC_DIR)/webservice

WS_SERVER_DIR = $(WEBSERVICE_DIR)/formatter

CC = gcc
CCFLAGS = -Wall -Werror -g -I$(INCLUDE_DIR)
RM = rm -rf
JAVA = javac
JAVAFLAGS = -d .

all: $(BIN_FILES)
.PHONY: all

editor: $(OBJ_DIR)/editor.o $(OBJ_DIR)/lines.o
	$(CC) $(CCFLAGS) $^ -o $@

broker: $(OBJ_DIR)/broker.o $(OBJ_DIR)/lines.o $(OBJ_DIR)/list.o $(OBJ_DIR)/servidorAlmacenamiento_clnt.o $(OBJ_DIR)/servidorAlmacenamiento_xdr.o
	$(CC) $(CCFLAGS) $^ -o $@ -lpthread -lnsl

servidorAlmacenamiento: $(OBJ_DIR)/servidorAlmacenamiento_server.o $(OBJ_DIR)/servidorAlmacenamiento_svc.o $(OBJ_DIR)/servidorAlmacenamiento_xdr.o $(OBJ_DIR)/fileManager.o
	$(CC) $(CCFLAGS) $^ -o $@ -lpthread -lnsl

formatterService.class: $(WS_SERVER_DIR)/formatterService.java $(WS_SERVER_DIR)/formatterPublisher.java
	wsimport -keep http://www.dataaccess.com/webservicesserver/numberconversion.wso?WSDL
	$(JAVA) $(JAVAFLAGS) $^
	wsgen -cp . -wsdl formatter.formatterService -d formatter

suscriptor.class: $(CORE_DIR)/suscriptor/suscriptor.java $(CORE_DIR)/suscriptor/ThreadMensajes.java
	wsimport -p formatterClient -keep formatter/FormatterServiceService.wsdl
	$(JAVA) $(JAVAFLAGS) $^

$(OBJ_DIR):
	mkdir $@

$(OBJ_DIR)/%.o: $(CORE_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(RPC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(WEBSERVICE_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR) $(BIN_FILES) suscriptor formatter formatterClient com topics

.SUFFIXES:
.PHONY: clean
