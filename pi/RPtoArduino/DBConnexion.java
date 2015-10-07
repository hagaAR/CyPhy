import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;
import java.sql.Timestamp;

import java.util.*;
import java.text.SimpleDateFormat;
import java.io.File;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class DBConnexion {
	
	private static Connection con;
	private static String db_url;
	private static String user;
	private static String password;
	
	public DBConnexion(){
		
	}
	
	public DBConnexion(String db, String usr,String pwd){
		db_url="jdbc:postgresql:"+db;
		user = usr;
		password =pwd;
	}
	
	public void connect(){
		//db_url="jdbc:postgresql:hagadb";
		//user = "haga";
		//password ="a";
		try{
			Class.forName("org.postgresql.Driver");
			con=DriverManager.getConnection(db_url, user, password);
			
			System.out.println("connexion db");

		} catch (Exception ex){
			ex.printStackTrace();
			System.err.println(ex.getClass().getName()+": "+ex.getMessage());
			System.exit(0);
		}
	}
	
	public void disconnect (){
		try{
			con.close();
		}catch(Exception e){
		}
	}
	
	
	public void create_sensors_Table() {
		String sql_query1;
		String sql_query2;
		Statement statement=null;
		
		//not decided what variables types
		sql_query2="CREATE TABLE sensors" +
					" (id INTEGER PRIMARY KEY not NULL, "  +
					"name VARCHAR(100)," +
					"unit VARCHAR(100)" +
					 ")" 
					;
		//sql_query1="DROP TABLE sensors";
		try{
			statement = con.createStatement();
			//statement.executeUpdate(sql_query1);
			statement.executeUpdate(sql_query2);
			System.out.println("creation table: 'Sensors'");
			statement.close();
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		}
		
	}
	public void drop_sensors_table() {
		String sql_query;
		Statement statement=null;
		
		//not decided what variables types
		sql_query="DROP TABLE sensors" ;
		try{
			statement = con.createStatement();
			statement.executeUpdate(sql_query);
			System.out.println("drop table: 'sensors'");
			statement.close();
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		}
		
	}
	
	
	public void show_sensors_table() {
		String sql_query;
		Statement statement=null;
		ResultSet rs;
		System.out.println("show table: sensors");
		System.out.println("id | name | unit");
		//not decided what variables types
		sql_query="Select * from sensors";
		try{
			statement = con.createStatement();
			rs=statement.executeQuery(sql_query);
			while(rs.next()){
				int id = rs.getInt("id");
				String name = rs.getString("name");
				String unit = rs.getString("unit");
				System.out.println(id +" "+ name +" "+ unit);
			}
			
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		} 
		
	}
	public void show_sensor_data_table() {
		String sql_query;
		Statement statement=null;
		ResultSet rs;
		System.out.println("show table: sensor_data");
		System.out.println("sensor_id | value | timestamp");
		//not decided what variables types
		sql_query="Select * from sensor_data";
		try{
			statement = con.createStatement();
			rs=statement.executeQuery(sql_query);
			while(rs.next()){
				int sensor_id = rs.getInt("sensor_id");
				double value = rs.getDouble("value");
				Timestamp timestamp = rs.getTimestamp("timestamp");
				System.out.println(sensor_id +" | "+ value +" | "+ timestamp.toString());
			}
			
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		} 
		
	}
	
	public void show_sensor_data_table(String dateDebut,String dateFin){
		String sql_query;
		Statement statement=null;
		ResultSet rs;
		System.out.print("show table: sensor_data ");
		System.out.println("between "+dateDebut+" and "+dateFin);
		System.out.println("sensor_name | value | timestamp");

		//timestamp format: yyyy-mm-dd hh:mm:ss
		//not decided what variables types
		sql_query="SELECT sd.*,s.* FROM sensor_data sd"
			+" INNER JOIN sensors s on sd.sensor_id=s.id"
			+" WHERE sd.timestamp >=TO_TIMESTAMP('"+dateDebut+"', 'yyyy-MM-dd hh:mm:ss')" 
				+" AND sd.timestamp<TO_TIMESTAMP('"+dateFin+"', 'yyyy-MM-dd hh:mm:ss')";

		try{
			statement = con.createStatement();
			rs=statement.executeQuery(sql_query);
			
			while(rs.next()){
				String sensor_name = rs.getString("name");
				double value = rs.getDouble("value");
				Timestamp timestamp = rs.getTimestamp("timestamp");
				System.out.println(sensor_name +" | "+ value +" | "+ timestamp.toString());
				
			}
			
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		} 
		
	}
	
	public Document exportXML_sensor_data_table(String dateDebut,String dateFin){
		String sql_query;
		Statement statement=null;
		ResultSet rs;
		Timestamp timestampFrom =null;
		Timestamp timestampTo =null;
		try{
			SimpleDateFormat format = new SimpleDateFormat("YY-MM-dd hh:mm:ss");
			Date dateFrom = format.parse(dateDebut);
			timestampFrom = new Timestamp(dateFrom.getTime());
			Date dateTo = format.parse(dateFin);
			timestampTo = new Timestamp(dateTo.getTime());
		}catch(Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		}
		System.out.print("show table: sensor_data ");
		System.out.println("between "+dateDebut+" and "+dateFin);
		System.out.println("sensor_id | value | timestamp");

		//timestamp format: yyyy-mm-dd hh:mm:ss
		//not decideTimestamp timestamp = new Timestamp(timeStamp+ (1000*Integer.parseInt(dataArray[2])));d what variables types
		sql_query="SELECT sd.*,s.* FROM sensor_data sd"
			+" INNER JOIN sensors s on sd.sensor_id=s.id"
			+" WHERE sd.timestamp >='" + timestampFrom
				+"' AND sd.timestamp<='"+timestampTo+"'";
		Document doc;
		
		
		try{
			statement = con.createStatement();
			rs=statement.executeQuery(sql_query);
				
			DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
			// root elements
			doc = docBuilder.newDocument();
			Element rootElement = doc.createElement("SensorData");
			doc.appendChild(rootElement);

			while(rs.next()){
				String sensor_name = rs.getString("name");
				double sensor_value = rs.getDouble("value");
				Timestamp sensor_data_timestamp = rs.getTimestamp("timestamp");
				String sensor_unit= rs.getString("unit");
				// sensorData elements
				Element sensorData = doc.createElement("sensorData");
				rootElement.appendChild(sensorData);
				// sensorName elements
				Element sensorName = doc.createElement("sensorName");
				sensorName.appendChild(doc.createTextNode(sensor_name));
				sensorData.appendChild(sensorName);
				// sensorDataValue, sensorDataValueUnit, sensorDataTimestamp elements
				Element sensorDataValue = doc.createElement("dataValue");
				String sensor_value_String=String.valueOf(sensor_value);
				sensorDataValue.appendChild(doc.createTextNode(sensor_value_String));
				sensorData.appendChild(sensorDataValue);
				Element sensorDataValueUnit = doc.createElement("valueUnit");
				sensorDataValueUnit.appendChild(doc.createTextNode(sensor_unit));
				sensorData.appendChild(sensorDataValueUnit);
				Element sensorDataTimestamp = doc.createElement("timestamp");
				sensorDataTimestamp.appendChild(doc.createTextNode(sensor_data_timestamp.toString()));
				sensorData.appendChild(sensorDataTimestamp);
		 
				System.out.println(sensor_name +" | "+ sensor_value +" | "+ sensor_data_timestamp.toString());
				
			}
			return doc;
			//writeXMLDocumentFromSensorDataTable(doc,dateDebut,dateFin);
			
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		} 
		return null;
	}
	
	public void writeXMLDocumentFromSensorDataTable(Document doc,String dateDebut,String dateFin){
		try{
			// write the content into xml file
			TransformerFactory transformerFactory = TransformerFactory.newInstance();
			Transformer transformer = transformerFactory.newTransformer();
			DOMSource source = new DOMSource(doc);
			String fileName;
			String fileRepository;
			fileName ="SensorData_from_" + dateDebut + "_to_"+dateFin;
			fileRepository ="./Request_results/" + fileName + ".xml";
			StreamResult result = new StreamResult(new File(fileRepository));
	 
			// Output to console for testing
			// StreamResult result = new StreamResult(System.out);
	 
			transformer.transform(source, result);
	 
			System.out.println("File saved!");
		}catch(Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		}
	}
	
	public void exportCSV_sensor_data_table(String dateDebut,String dateFin){
		String sql_query;
		Statement statement=null;
		ResultSet rs;
		System.out.print("show table: sensor_data ");
		System.out.println("between "+dateDebut+" and "+dateFin);
		System.out.println("sensor_id | value | timestamp");

		//timestamp format: yyyy-mm-dd hh:mm:ss
		//not decided what variables types
		sql_query="SELECT sd.*,s.* FROM sensor_data sd"
			+" INNER JOIN sensors s on sd.sensor_id=s.id"
			+" WHERE sd.timestamp >=TO_TIMESTAMP('"+dateDebut+"', 'yyyy-MM-dd hh:mm:ss')" 
				+" AND sd.timestamp<=TO_TIMESTAMP('"+dateFin+"', 'yyyy-MM-dd hh:mm:ss')";
		
		try{
			statement = con.createStatement();
			rs=statement.executeQuery(sql_query);
				
			DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
			// root elements
			Document doc = docBuilder.newDocument();
			Element rootElement = doc.createElement("SensorData");
			doc.appendChild(rootElement);

			while(rs.next()){
				String sensor_name = rs.getString("name");
				double sensor_value = rs.getDouble("value");
				Timestamp sensor_data_timestamp = rs.getTimestamp("timestamp");
				String sensor_unit= rs.getString("unit");
				// staff elements
				Element sensorData = doc.createElement("sensorData");
				rootElement.appendChild(sensorData);

				// firstname elements
				Element sensorName = doc.createElement("sensorName");
				sensorName.appendChild(doc.createTextNode(sensor_name));
				sensorData.appendChild(sensorName);
		 
				// lastname elements
				Element sensorDataValue = doc.createElement("dataValue");
				String sensor_value_String=String.valueOf(sensor_value);
				sensorDataValue.appendChild(doc.createTextNode(sensor_value_String));
				sensorData.appendChild(sensorDataValue);
				Element sensorDataValueUnit = doc.createElement("valueUnit");
				sensorDataValueUnit.appendChild(doc.createTextNode(sensor_unit));
				sensorData.appendChild(sensorDataValueUnit);
				Element sensorDataTimestamp = doc.createElement("timestamp");
				sensorDataTimestamp.appendChild(doc.createTextNode(sensor_data_timestamp.toString()));
				sensorData.appendChild(sensorDataTimestamp);
		 
				System.out.println(sensor_name +" | "+ sensor_value +" | "+ sensor_data_timestamp.toString());
				
			}
			// write the content into xml file
			TransformerFactory transformerFactory = TransformerFactory.newInstance();
			Transformer transformer = transformerFactory.newTransformer();
			DOMSource source = new DOMSource(doc);
			String fileName;
			String fileRepository;
			fileName ="SensorData_from_" + dateDebut + "_to_"+dateFin;
			fileRepository ="./Request_results/" + fileName + ".xml";
			StreamResult result = new StreamResult(new File(fileRepository));
	 
			// Output to console for testing
			// StreamResult result = new StreamResult(System.out);
	 
			transformer.transform(source, result);
	 
			System.out.println("File saved!");
			
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		} 
		
	}
	
	
	public int getSensorID(String sensorName)  {
		Statement statement=null;
		ResultSet rs=null;
		
		sensorName=sensorName.toLowerCase();
		
		try{
			statement = con.createStatement();
			String sqlQuery =String.format("SELECT id FROM sensors WHERE name = '"+sensorName+"'");
			System.out.print("sqlQuery:");
			System.out.println(sqlQuery);
			rs=statement.executeQuery(sqlQuery);

			while(rs.next()){
				System.out.print("return rs.getInt: ");
				System.out.println(rs.getInt("id"));
				return rs.getInt("id");
			}
			statement.close();
			
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		} 
		return -1;
	}
	
	
	public void create_sensor_data_table() {
		String sql_query1,sql_query2;
		Statement statement=null;
		
		//Drop table before
		//sql_query1="DROP TABLE sensor_data" ;
		sql_query2="CREATE TABLE sensor_data" +
					" ( "  +
					"sensor_id INTEGER," +
					"value FLOAT," +
					"timestamp TIMESTAMP(0)" +
					 ")" 
					;
		try{
			statement = con.createStatement();
			//statement.executeUpdate(sql_query1);
			statement.executeUpdate(sql_query2);
			System.out.println("creation table: 'sensor_data'");
			statement.close();
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		}
		
	}
	
	public void drop_sensor_data_table() {
		String sql_query;
		Statement statement=null;
		
		//not decided what variables types
		sql_query="DROP TABLE sensor_data" ;
		try{
			statement = con.createStatement();
			statement.executeUpdate(sql_query);
			System.out.println("drop table: 'sensor_data'");
			statement.close();
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		}
		
	}
	
	public void sensors_table_new_insert(int id, String sensor_name,String sensor_unit) {
		String sql_query;
		Statement statement=null;
		//ResultSet rs;
		//Gros probleme => il faut un remplissage auto des id de la table
		//soluce: fetch dernier id et faire +1 --non implementé
		sql_query="INSERT INTO sensors VALUES ("+id+",'"+sensor_name+"','"+sensor_unit+"' )";
		try{
			statement = con.createStatement();
			statement.executeUpdate(sql_query);
			System.out.println("insertion in 'sensors': name: "+ sensor_name+" unit: "+sensor_unit+" sensor id: "+id);
			statement.close();
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		}
		
	}	
	
	public void sensor_data_table_new_insert(int sensor_id, double value,Timestamp timestamp) {
		String sql_query;
		Statement statement=null;
		//ResultSet rs;
		//Gros probleme => il faut un remplissage auto des id de la table
		//soluce: fetch dernier id et faire +1 --non implementé
		sql_query="INSERT INTO sensor_data VALUES ("+sensor_id+","+value+",'"+timestamp+"' )";
		try{
			statement = con.createStatement();
			statement.executeUpdate(sql_query);
			System.out.println("insertion in 'sensor_data': sensor id: "+ sensor_id+" value: "+value+" timestamp: "+timestamp);
			statement.close();
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		}
		
	}
	
	
}

