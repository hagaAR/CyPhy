import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import java.io.IOException;
import org.w3c.dom.Document;
import java.text.SimpleDateFormat;
import java.util.*;

public class GetDataServlet extends HttpServlet
{
    private String greeting="Hello World, you are using getData";
    public GetDataServlet(){}
    public GetDataServlet(String greeting)
    {
        this.greeting=greeting;
    }
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
		MainApp mainApp = new MainApp();
				
		mainApp.setUpDataBase ();

		String date1=request.getParameter("date_from");
		String date2=request.getParameter("date_to");
		if(date1==null || date2==null){
			Date today= new Date();
			SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd");
			date1=df.format(today);
			date2=date1;
		}
		Document resultXMLDocument;
		String resultXMLDocumentString;
		resultXMLDocument=mainApp.exportSensorDataTableToXMLDocument (date1,date2);
		resultXMLDocumentString = mainApp.XMLDocumentToString(resultXMLDocument);
		System.out.println(resultXMLDocumentString);
        response.setContentType("text/html");
        response.setStatus(HttpServletResponse.SC_OK);
        response.getWriter().println(resultXMLDocumentString);
        //response.getWriter().println("session=" + request.getSession(true).getId());
    }
}
