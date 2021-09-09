package formatter;

import javax.jws.WebService;
import javax.jws.WebMethod;
import java.util.regex.*;
import java.math.BigInteger;
import java.net.*;

import com.dataaccess.webservicesserver.*;

@WebService
public class formatterService {
	@WebMethod
	public String formatear(String texto) {

		//Cliente del servicio de conversion de numeros a texto
		NumberConversion service = new NumberConversion();
		NumberConversionSoapType port = service.getNumberConversionSoap();

		//Buscar con espresiones regulares cada numero del texto
		Pattern p = Pattern.compile("([0-9]+)");
		Matcher m = p.matcher(texto);
		StringBuffer out = new StringBuffer();

		while(m.find()) {
			//Guardar numero localizado
			String number = m.group();

			//Reemplazar el numero con el numero y la salida del servicio web
			m.appendReplacement(out, number + " (" + port.numberToWords(new BigInteger(number)).trim() + ") ");
		}

		m.appendTail(out);

		//Sustituye dos o mas espacios por uno solo
		String salida = out.toString().replaceAll("  +", " ");

		return salida;
	}
}
