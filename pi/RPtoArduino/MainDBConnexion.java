import java.sql.Timestamp;
import java.util.*;

public class MainDBConnexion {
	
	DBConnexion dbcon;
	
	//public static void main (String args[]) {
	public MainDBConnexion(){
		dbcon = new DBConnexion("CPS","haga","a");
		dbcon.connect();
	}
	public void setUpDataBase () {	
		DBConnexion dbcon = new DBConnexion("CPS","haga","a");
		dbcon.connect();
		//dbcon.create_sensors_Table();
		//dbcon.sensor_table_new_insert(1,"thermometer","celcius");
		//dbcon.insertLine("Microcontrollers", "ArduinoDue");
		//dbcon.show_sensors_table();
		dbcon.create_sensor_data_table();
		//java.text.SimpleDateFormat sdf = new java.text.SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Timestamp timestamp = new Timestamp(new Date().getTime());

	}
	public void insertData(String data){
		dbcon.sensor_data_table_new_insert(1,22.0,timestamp);
		timestamp = new Timestamp(new Date().getTime());
		dbcon.sensor_data_table_new_insert(1,21.0,timestamp);
		dbcon.show_sensor_data_table();
	}
}

