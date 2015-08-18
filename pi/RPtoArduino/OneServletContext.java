import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.servlet.DefaultServlet;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;


public class OneServletContext
{
    public static void main(String[] args) throws Exception
    {
        Server server = new Server(80);
 
        ServletContextHandler context = new ServletContextHandler(ServletContextHandler.SESSIONS);
        context.setContextPath("/");
        server.setHandler(context);
 
        context.addServlet(new ServletHolder(new HelloServlet()),"/*");
        context.addServlet(new ServletHolder(new HelloServlet("Buongiorno Mondo")),"/it/*");
        context.addServlet(new ServletHolder(new HelloServlet("Bonjour le Monde")),"/fr/*");
        
        MainAppServlet mainAppServlet = new MainAppServlet("Bonjour, activer MainApp");
        context.addServlet(new ServletHolder(mainAppServlet),"/MainApp");
 
        server.start();
        server.join();
    }
}

