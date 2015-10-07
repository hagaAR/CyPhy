import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.server.ServerConnector;
import org.eclipse.jetty.servlet.DefaultServlet;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;

public class OneServletContext
{
    public static void main(String[] args) throws Exception
    {
        Server server = new Server(80);
        try{
			MainApp.setUpDataBase();
			MainApp.setUpArduinoCommunication();
			
            ServletContextHandler context = new ServletContextHandler(ServletContextHandler.SESSIONS);
            context.setContextPath("/");
            server.setHandler(context);
     
            context.addServlet(new ServletHolder(new HelloServlet()),"/*");
            context.addServlet(new ServletHolder(new HelloServlet("Buongiorno Mondo")),"/it/*");
            context.addServlet(new ServletHolder(new HelloServlet("Bonjour le Monde")),"/fr/*");

            GetDataServlet getDataServlet = new GetDataServlet("afficher requête");
            context.addServlet(new ServletHolder(getDataServlet),"/getData");
            GetLiveDataServlet getLiveDataServlet = new GetLiveDataServlet("afficher requête");
            context.addServlet(new ServletHolder(getLiveDataServlet),"/getLiveData");
            SetValveServlet setValveServlet1 = new SetValveServlet("afficher requête");
            context.addServlet(new ServletHolder(setValveServlet1),"/setValve");
            CollectDataServlet collectDataServlet = new CollectDataServlet("afficher requête");
            context.addServlet(new ServletHolder(collectDataServlet),"/collectData");
            // Initialize javax.websocket layer
            

            server.start();
            //server.dump(System.err);
            server.join();
        }
        catch (Throwable t){
            t.printStackTrace(System.err);
        }
        
    }
}

