package suscriptor;

import java.io.*;
import java.net.*;

import formatterClient.FormatterServiceService;
import formatterClient.FormatterService;

class ThreadMensajes extends Thread {
	private ServerSocket sd;
	private String _wsHost;
	private short _wsPort;

	public ThreadMensajes(ServerSocket sd, String webServiceHost, short webServicePort) {
		this.sd = sd;
		this._wsHost = webServiceHost;
		this._wsPort = webServicePort;
	}

	public void freeSocket() {
		try {
			this.sd.close();
		} catch (IOException ioe) {
			ioe.printStackTrace();
		}
	}

	public ServerSocket getServerSocket() {
		return this.sd;
	}

	public void run() {
		Socket sc;
		String tema = "";
		String mensaje = "";

		while(true) {
			tema = "";
			mensaje = "";
			try {
				sc = sd.accept();

				DataInputStream in = new DataInputStream(sc.getInputStream());

				String buff = "";
				char charBuff = '0';

				System.out.print("MESSAGE FROM ");

				URL url = null;
				try {
					url = new URL("http://" + this._wsHost + ":" + this._wsPort + "/formatter?wsdl");
				} catch(MalformedURLException e) {
					e.printStackTrace();
				}

				FormatterServiceService formatter = new FormatterServiceService(url);
				FormatterService port = formatter.getFormatterServicePort();

				while((charBuff = (char) in.readByte()) != '\0') {
					buff += charBuff;
				}

				System.out.print(buff + " : ");
				buff = "";
				
				while((charBuff = (char) in.readByte()) == '\0');

				do {
					buff += charBuff;
				} while((charBuff = (char) in.readByte()) != '\0');

				String aux = port.formatear(buff);

				System.out.println(aux);

				System.out.print("c> ");
				sc.close();

			} catch(IOException ioe) {
				break;
			}

		}
	}
}

