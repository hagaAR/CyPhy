import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import java.io.IOException;
import org.w3c.dom.Document;
import java.text.SimpleDateFormat;
import java.util.*;

import java.util.Observable;
import java.util.Observer;

import javax.websocket.server.ServerContainer;
import org.eclipse.jetty.websocket.jsr356.server.deploy.WebSocketServerContainerInitializer;

public class GetLiveDataServlet extends HttpServlet
{
    private String greeting="Hello World, you are using GetLiveDataServlet";
    public GetLiveDataServlet(){}
    public GetLiveDataServlet(String greeting)
    {
        this.greeting=greeting;
    }
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
		MainApp mainApp = new MainApp();		
		mainApp.setUpDataBase ();
		mainApp.setUpArduinoCommunication ();
		
		GetLiveDataServlet observer = new GetLiveDataServlet();
		
		ServerContainer wscontainer = WebSocketServerContainerInitializer.configureContext(context);
	   	// Add WebSocket endpoint to javax.websocket layer
	    EventSocket eventSocket = new EventSocket();
	    wscontainer.addEndpoint(eventSocket);
	    eventSocket.onOpen((Session session,EndpointConfig config) -> {
			mainApp.addObserver((Observable o, Object arg) -> {
				String [] dataArray;
				dataArray = arg;
				Document resultXMLDocument;
				DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
				DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
				// root elements
				resultXMLDocument = docBuilder.newDocument();
				Element rootElement = resultXMLDocument.createElement("SensorData");
				resultXMLDocument.appendChild(rootElement);
				String sensor_name = dataArray[0];
				String sensor_value = dataArray[1];
				String sensor_data_timestamp = dataArray[2];
				// sensorData elements
				Element sensorData = resultXMLDocument.createElement("sensorData");
				rootElement.appendChild(sensorData);
				// sensorName elements
				Element sensorName = resultXMLDocument.createElement("sensorName");
				sensorName.appendChild(doc.createTextNode(sensor_name));
				sensorData.appendChild(sensorName);
				// sensorDataValue, sensorDataTimestamp elements
				Element sensorDataValue = resultXMLDocument.createElement("dataValue");
				sensorDataValue.appendChild(resultXMLDocument.createTextNode(sensor_value));
				sensorData.appendChild(sensorDataValue);
				Element sensorDataTimestamp = resultXMLDocument.createElement("timestamp");
				sensorDataTimestamp.appendChild(resultXMLDocument.createTextNode(sensor_data_timestamp));
				sensorData.appendChild(sensorDataTimestamp);
				String resultXMLDocumentString;
				resultXMLDocumentString = mainApp.XMLDocumentToString(resultXMLDocument);
				System.out.println(resultXMLDocumentString);
		        response.setContentType("text/html");
        		response.setStatus(HttpServletResponse.SC_OK);
        		//response.getWriter().println(resultXMLDocumentString);

        		session.getBasicRemote().sendText(resultXMLDocumentString);
        		///references
        		//////reste à implementer un client pour la socket côté anylogic
        		///http://docs.oracle.com/javaee/7/api/javax/websocket/Endpoint.html
        		///http://docs.oracle.com/javaee/7/api/javax/websocket/Session.html
        		////https://github.com/jetty-project/embedded-jetty-websocket-examples/blob/master/javax.websocket-example/src/main/java/org/eclipse/jetty/demo/EventClient.java
			});
		});

    }
}
