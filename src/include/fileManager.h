#ifndef _FILEMANAGER_H
#define _FILEMANAGER_H

#define TOPICS_DIR "topics"

int fm_init();
int fm_storeMessage(char* topic, char* msg);
int fm_getLastMessage(char* topic, char* msgBuffer);

#endif
