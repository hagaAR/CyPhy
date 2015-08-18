import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import java.io.IOException;

public class MainAppServlet extends HttpServlet
{
    private String greeting="Hello World, you are using MainApp";
    public MainAppServlet(){}
    public MainAppServlet(String greeting)
    {
        this.greeting=greeting;
    }
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
		MainApp mainApp = new MainApp();
		
		mainApp.setUpArduinoCommunication();		
		mainApp.setUpDataBase ();
		mainApp.dropAndCreateSensorDataTable ();
		mainApp.getDataSample_insertInSensorDataTable (40);
        response.setContentType("text/html");
        response.setStatus(HttpServletResponse.SC_OK);
        response.getWriter().println("<h1>"+greeting+"</h1>");
        response.getWriter().println("session=" + request.getSession(true).getId());
    }
}
