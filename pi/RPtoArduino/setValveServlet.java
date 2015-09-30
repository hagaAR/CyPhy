import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import java.io.IOException;
import org.w3c.dom.Document;

public class SetValveServlet extends HttpServlet
{
    private String greeting="Hello World, you are using setValve";
    public SetValveServlet(){}
    public SetValveServlet(String greeting)
    {
        this.greeting=greeting;
    }
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
		MainApp mainApp = new MainApp();	
		mainApp.setUpArduinoCommunication ();
		
		String useful_energy=request.getParameter("useful_energy");
		String water_flow_rate_string=request.getParameter("water_flow_rate");
		if(useful_energy==null||water_flow_rate_string==null){
			System.out.println("useful_energy: " +useful_energy);
			System.out.println("water_flow_rate_string: "+water_flow_rate_string);
			return;
		}
		float kWh = Float.parseFloat(useful_energy);
		System.out.println(kWh);
		float water_flow_rate = Float.parseFloat(water_flow_rate_string);
		mainApp.com.setValve(kWh,water_flow_rate);

        response.setContentType("text/html");
        response.setStatus(HttpServletResponse.SC_OK);
        response.getWriter().println();
    }
}
