import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import java.text.SimpleDateFormat;
import java.io.IOException;
import org.w3c.dom.Document;
import java.util.*;

public class CollectDataServlet extends HttpServlet
{
    private String greeting="Hello World, you are using CollectDataServlet";
    public CollectDataServlet(){}
    public CollectDataServlet(String greeting)
    {
        this.greeting=greeting;
    }
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
		MainApp mainApp = new MainApp();		
		mainApp.setUpDataBase ();
		mainApp.setUpArduinoCommunication ();
		try{
			String startDateString=request.getParameter("start_date");
			int dataTimePeriod=Integer.parseInt(request.getParameter("period"));
			
			SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
			Date startDate=new Date();
			if(startDateString !=null){
				startDate=sdf.parse(startDateString);
			}
			mainApp.startDataCollection(startDate,dataTimePeriod);
			response.setContentType("text/html");
			response.setStatus(HttpServletResponse.SC_OK);
		}catch(Exception e){
			e.printStackTrace();
		}
        //response.getWriter().println("session=" + request.getSession(true).getId());
    }
}
