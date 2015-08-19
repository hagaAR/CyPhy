import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;
import java.sql.Timestamp;

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
	
	public void disconnect(){
		//con.close();
	}
	
	
	public void create_sensors_Table() {
		String sql_query1;
		String sql_query2;
		Statement statement;
		
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
		Statement statement;
		
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
		Statement statement;
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
		Statement statement;
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
	
	
	public int getSensorID(String sensorName) {
		Statement statement;
		ResultSet rs;
		sensorName=sensorName.toLowerCase();
		//System.out.print("sensorName|");
		//System.out.print(sensorName);
		//System.out.println("|");
		try{
			statement = con.createStatement();
			rs=statement.executeQuery("SELECT distinct id FROM sensors WHERE name = '"+sensorName+"'");
			while(rs.next()){
				//System.out.print("return rs.getInt: ");
				//System.out.println(rs.getInt("id"));
				return rs.getInt("id");
			}
			
		} catch (Exception e){
			e.printStackTrace();
			System.err.println(e.getClass().getName()+": "+e.getMessage());
			System.exit(0);
		} 
		return -1;
	}
	
	
	public void create_sensor_data_table() {
		String sql_query1,sql_query2;
		Statement statement;
		
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
		Statement statement;
		
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
		Statement statement;
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
		Statement statement;
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

