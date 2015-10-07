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

public class GetLiveDataServlet extends HttpServlet
{
	//String answer = "";
    private String greeting="Hello World, you are using GetLiveDataServlet";
    public GetLiveDataServlet(){}
    public GetLiveDataServlet(String greeting)
    {
        this.greeting=greeting;
    }
    
     
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
		
		//MainApp mainApp = new MainApp();		
		//MainApp.setUpDataBase ();
			
		//GetLiveDataServlet observer = new GetLiveDataServlet();
		
		/*mainApp.addObserver((Observable o, Object arg) -> {
			System.out.println("addObserver");
			
			String xmlString;
			xmlString  = (String) arg;
			try{
				
			 answer = xmlString;//mainApp.XMLDocumentToString(resultXMLDocument);
			//answer = resultXMLDocumentString;
			//System.out.println(resultXMLDocumentString);
			response.setContentType("text/html");
			response.setStatus(HttpServletResponse.SC_OK);
			//response.getWriter().println(resultXMLDocumentString);
			
			
			
			} catch(Exception e){
				e.printStackTrace();
			}
			

		});*/
		
		//MainApp.setUpArduinoCommunication ();
		//mainApp.getDataSampleFromAllSensors_insertInSensorDataTable(0);
		String answer = MainApp.getLiveData();
		/*while(answer.equals("")){
			answer = mainApp.getAnswer();
		}*/
		System.out.println("Message Sent");
		response.getWriter().println(answer);
		answer = "";

    }
}
