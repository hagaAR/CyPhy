import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import java.io.IOException;
import org.w3c.dom.Document;

public class MainAppServlet extends HttpServlet
{
    private String greeting="Hello World, you are using MainApp";
    public MainAppServlet(){}
    public MainAppServlet(String greeting)
    {
        this.greeting=greeting;
    }
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
		MainApp mainApp = new MainApp();
		
		mainApp.setUpArduinoCommunication();		
		mainApp.setUpDataBase ();
		mainApp.dropAndCreateSensorDataTable ();
		mainApp.getDataSample_insertInSensorDataTable (40);
		String date1="2015-09-01";
		String date2="2015-09-05";
		String datePlusTardString="2015-09-02 14:19:00";
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
		Document resultXMLDocument;
		Date nowDate= new Date();
        response.setContentType("text/html");
        response.setStatus(HttpServletResponse.SC_OK);
        response.getWriter().println("<h1>"+greeting+"</h1>");
        response.getWriter().println("session=" + request.getSession(true).getId());
    }
}
