package formatter;

import javax.xml.ws.Endpoint;

public class formatterPublisher {
	public static void main(String[] args) {
		short publishPort = Short.parseShort(args[0]);

		if(args.length != 1) {
			System.out.println("Usage: java -cp . formatter.formatterPublisher <port>");
		}

		if(publishPort < 1024 || publishPort > 65535) {
			System.out.println("Port number must be betweeb 1024 and 65535");
			return;
		}

		//URL donde se publicara el XML
		final String url = "http://localhost:" + args[0] + "/formatter";

		System.out.println("Publishing formatterService at endpoint: " + url);
		Endpoint.publish(url, new formatterService());
	}
}
