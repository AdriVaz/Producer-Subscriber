program SERVIDOR_ALMACENAMIENTO {
	version PRIMERA_VERSION {
		int init() = 1;
		int storeMessage(string topic, string msg) = 2;
		string getLastMessage(string topic) = 3;
	} = 1;
} = 99;
