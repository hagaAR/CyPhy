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
	static public void initialiseSensorsTable () {	
		dbcon.drop_sensors_table();
		dbcon.create_sensors_Table();
	}
	static public void initialiseSensorDataTable () {	
		dbcon.drop_sensor_data_table();
		dbcon.create_sensor_data_table();
	}
	
	static public void fillSensorsTable () {	
		dbcon.sensors_table_new_insert(1,"thermo1","celcius");
		dbcon.sensors_table_new_insert(2,"thermo2","celcius");
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
		System.out.println("Received message is: " + data);
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
	}
	

	
	static public void showSensorsTable () {	
		dbcon.show_sensors_table();
	}
	static public void showSensorDataTable () {	
		dbcon.show_sensor_data_table() ;
	}
	
	static public void getDataSampleFromThermo1_insertInSensorDataTable () {
		timeStamp = new Date().getTime();	
		com.getDataFromThermo1();
	}	
	
	static public void getDataSampleFromThermo1_insertInSensorDataTable (int timeDataPeriod) {	
		timeStamp = new Date().getTime();	
		com.getDataFromThermo1(timeDataPeriod);
		
	}
		
	public static void main (String args[])  throws InterruptedException {
		//Begin communication with Arduino & Db
		setUpArduinoCommunication();		
		setUpDataBase ();
		
		//Re-initialize sensors db
		//initialiseSensorsTable ();
		//fillSensorsTable ();
		showSensorsTable () ;
		
		//Re-initialize (drop&re-create table) SensorData db
		initialiseSensorDataTable ();
		
		//ask Arduino for data from thermo1
		getDataSampleFromThermo1_insertInSensorDataTable (40);
		showSensorDataTable ();
	
	}
	
}

