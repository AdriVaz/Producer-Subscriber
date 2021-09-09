package suscriptor;

import java.io.*;
import java.net.*;
import java.lang.*;
import java.util.*;
import gnu.getopt.Getopt;

class suscriptor {
	
	/********************* TYPES **********************/
	
	
	/******************* ATTRIBUTES *******************/
	
	private static String _server   = null;
	private static short _port = -1;

	private static String _webServiceHost = null;
	private static short _webServicePort = -1;
		
	private static ArrayList <String> suscripciones = new ArrayList <String>();
	private static ThreadMensajes thmsg = null;
	
	/********************* METHODS ********************/
	
	static int subscribe(String topic) 
	{
		// Write your code here

		try {
			Socket clientSocket = new Socket(_server, _port);
			
			//Se usa mas adelante
			ServerSocket serverSocket = null;

			DataOutputStream ostream = new DataOutputStream(clientSocket.getOutputStream());

			if(thmsg == null) {
				serverSocket = new ServerSocket(0);
				thmsg = new ThreadMensajes(serverSocket, _webServiceHost, _webServicePort);
				thmsg.start();
			}

			//Escribir codigo de operacion
			ostream.writeBytes("SUBSCRIBE\0");

			//Escribir parametros
			ostream.writeBytes(topic + '\0');
			ostream.writeBytes(Integer.toString(thmsg.getServerSocket().getLocalPort()) + '\0');

			//Leer valor de retorno
			DataInputStream istream = new DataInputStream(clientSocket.getInputStream());

			char ret = (char) istream.readByte();


			if(ret == '0') {
				System.out.println("SUBSCRIBE OK");
				if(!suscripciones.contains(topic))
					suscripciones.add(topic);
			} else if(ret == '1') {
				System.out.println("SUBSCRIBE FAIL");
			}

		} catch(SocketException se) {
			System.out.println("Error in the connection to the broker " + _server + ":" + _port);
			//se.printStackTrace();
		} catch(IOException ioe) {
			ioe.printStackTrace();
		}
        
		return 0;
	}

	static int unsubscribe(String topic) 
	{
		// Write your code here
		
		try {
			Socket clientSocket = new Socket(_server, _port);

			DataOutputStream ostream = new DataOutputStream(clientSocket.getOutputStream());

			//Escribir codigo de operacion
			ostream.writeBytes("UNSUBSCRIBE\0");

			//Escribir parametros
			ostream.writeBytes(topic + '\0');
			ostream.writeBytes(Integer.toString(thmsg.getServerSocket().getLocalPort()) + '\0');

			//Leer valor de retorno
			DataInputStream istream = new DataInputStream(clientSocket.getInputStream());

			char ret = (char) istream.readByte();

			if(ret == '0') {
				suscripciones.remove(topic);
				//Matar hilo escuchador si no hay suscripciones
				if(suscripciones.size() == 0) {
					if(thmsg != null) {
						thmsg.freeSocket();
						thmsg = null;
					}
				}
				System.out.println("UNSUBSCRIBE OK");
			} else if(ret == '1') {
				System.out.println("TOPIC NOT SUBSCRIBED");
			} else if(ret == '2') {
				System.out.println("UNSUBSCRIBE FAIL");
			}

		} catch(SocketException se) {
			System.out.println("Error in the connection to the broker " + _server + ":" + _port);
			//se.printStackTrace();
		} catch(IOException ioe) {
			ioe.printStackTrace();
		}

		return 0;
	}
	
	/**
	 * @brief Command interpreter for the suscriptor. It calls the protocol functions.
	 */
	static void shell() 
	{
		boolean exit = false;
		String input;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** SUBSCRIBE *************/
					if (line[0].equals("SUBSCRIBE")) {
						if  (line.length >= 2) {
							String tema = "";

							for(int i = 1; i < line.length; ++i)
								tema += line[i] + " ";

							tema = tema.substring(0, tema.length() - 1); 

							subscribe(tema);
						} else {
							System.out.println("Syntax error. Usage: SUBSCRIBE <topic>");
						}
					} 
					
					/********** UNSUBSCRIBE ************/
					else if (line[0].equals("UNSUBSCRIBE")) {
						if  (line.length >= 2) {
							String tema = "";

							for(int i = 1; i < line.length; ++i)
								tema += line[i] + " ";

							tema = tema.substring(0, tema.length() - 1);

							unsubscribe(tema);
						} else {
							System.out.println("Syntax error. Usage: UNSUBSCRIBE <topic>");
						}
					    } 
					    
					    /************** QUIT **************/
					    else if (line[0].equals("QUIT")){
							if (line.length == 1) {
								exit = true;
							} else {
								System.out.println("Syntax error. Use: QUIT");
							}
						} 
						
					/************* UNKNOWN ************/
					else {						
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}				
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * @brief Prints program usage
	 */
	static void usage() 
	{
		System.out.println("Usage: java -cp . suscriptor -s <broker-host> -p <proker-host-port> -w <web-service-host> -q <web-service-host-port>");
	}
	
	/**
	 * @brief Parses program execution arguments 
	 */ 
	static boolean parseArguments(String [] argv) 
	{
		Getopt g = new Getopt("suscriptor", argv, "ds:p:w:q:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				//case 'd':
				//	_debug = true;
				//	break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Short.parseShort(arg);
					break;
				case 'w':
					_webServiceHost = g.getOptarg();
					break;
				case 'q':
					arg = g.getOptarg();
					_webServicePort = Short.parseShort(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}
		
		if (_server == null)
			return false;
		
		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		if(_webServiceHost == null)
			return false;

		if((_webServicePort < 1024) || (_webServicePort > 65535) || (_webServicePort == _port))
			return false;

		return true;
	}
	
	
	
	/********************* MAIN **********************/
	
	public static void main(String[] argv) 
	{
		if(!parseArguments(argv)) {
			usage();
			return;
		}

		// Write code here
		
		shell();

		for(int i = 0; i < suscripciones.size(); ++i)
			unsubscribe(suscripciones.get(i));

		//Para evitar que se quede el hilo activo e impida que termine
		//Esto solo ocurre cuando no se puede desuscribir de todos
		//los temas porque el broker este caido por ejemlo
		if(thmsg != null) {
			thmsg.freeSocket();
			thmsg = null;
		}

		System.out.println("+++ FINISHED +++");
	}
}
