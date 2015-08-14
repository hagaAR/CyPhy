import java.util.Observable;
import java.util.Observer;
import java.sql.Timestamp;
import java.util.*;

public class MainApp {
	
	static ArduinoCommunication com;
	static DBConnexion dbcon;
	static long timeStamp;
	static public void setUpDataBase () {	
		dbcon = new DBConnexion("CPS","haga","a");
		dbcon.connect();
	}
	static public void initialiseSensorTable () {	
		dbcon.create_sensors_Table();
		dbcon.sensor_table_new_insert(1,"thermo1","celcius");
	}
	static public void setUpArduinoCommunication(){
		com = new ArduinoCommunication();
		MainApp observer = new MainApp();
		
		com.addObserver((Observable o, Object arg) -> {
				insertData((String)arg);
				//System.out.println("The message is: " + arg);
		});
		com.setATModeCommunication();
	}
	
	static public void insertData(String data){
		//System.out.println("The message is: " + data);
		String[] dataArray = data.split(";");
		//System.out.println("Size: " + dataArray.length);
		
		if(dataArray.length <= 2){
			//System.out.println("The message is: " + data);
			System.out.println("No data is saved");
			return;
		}

		String resultFromSelect;
		Timestamp timestamp = new Timestamp(timeStamp+ (1000*Integer.parseInt(dataArray[2])));//new Date().getTime());
		int sensorId = dbcon.getSensorID(dataArray[0]);
		if(sensorId == -1){
			System.out.println("ID NOT FOUND");
			return;
		}
		dbcon.sensor_data_table_new_insert(dbcon.getSensorID(dataArray[0]),Double.parseDouble(dataArray[1]),timestamp);
		//timestamp = new Timestamp(new Date().getTime());
		//dbcon.sensor_data_table_new_insert(1,21.0,timestamp);
		//dbcon.show_sensor_data_table();
		
		dbcon.show_sensor_data_table();
	}
	
	static public void dropAndCreateSensorDataTable () {	
		dbcon.drop_sensor_data_table();
		dbcon.create_sensor_data_table();
		dbcon.show_sensor_data_table() ;
	}
	
	static public void showSensorsTable () {	
		dbcon.show_sensors_table();
	}
	
	static public void getDataSample_insertInSensorDataTable () {
		timeStamp = new Date().getTime();	
		com.getData();
	}	
	
	static public void getDataSample_insertInSensorDataTable (int timeDataPeriod) {	
		timeStamp = new Date().getTime();	
		com.getData(timeDataPeriod);
		
	}
		
	public static void main (String args[])  throws InterruptedException {
		setUpArduinoCommunication();		
		setUpDataBase ();
		dropAndCreateSensorDataTable ();
		//initialiseSensorTable ();
		showSensorsTable () ;
		//dbcon.show_sensor_data_table() ;
		getDataSample_insertInSensorDataTable (60);
		
		
		
	/*	
		DBConnexion dbcon = new DBConnexion("CPS","haga","a");
		dbcon.connect();
		//dbcon.create_sensors_Table();
		//dbcon.sensor_table_new_insert(1,"thermometer","celcius");
		//dbcon.insertLine("Microcontrollers", "ArduinoDue");
		dbcon.show_sensors_table();
		dbcon.create_sensor_data_table();
		//java.text.SimpleDateFormat sdf = new java.text.SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Timestamp timestamp = new Timestamp(new Date().getTime());
		dbcon.sensor_data_table_new_insert(1,22.0,timestamp);
		timestamp = new Timestamp(new Date().getTime());
		dbcon.sensor_data_table_new_insert(1,21.0,timestamp);
		dbcon.show_sensor_data_table();
		* */
	}
	
}

