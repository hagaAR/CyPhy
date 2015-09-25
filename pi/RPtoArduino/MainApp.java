import java.io.*;

import java.util.Observable;
import java.util.Observer;
import java.sql.Timestamp;
import java.util.*;
import java.text.SimpleDateFormat;
import org.w3c.dom.Document;

import org.apache.log4j.Logger;
import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.FileAppender;
import org.apache.log4j.Level;
import org.apache.log4j.SimpleLayout;
import org.apache.log4j.xml.XMLLayout;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

public class MainApp {
	
	private static Logger logger= Logger.getLogger(MainApp.class);
	
	static ArduinoCommunication com;
	static DBConnexion dbcon;
	static long timeStamp =0;
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
		dbcon.sensors_table_new_insert(1,"thermocold","celcius");
		dbcon.sensors_table_new_insert(2,"thermomix","celcius");
		dbcon.sensors_table_new_insert(3,"thermohot","celcius");
		//flow in Litres per meter
		dbcon.sensors_table_new_insert(4,"flowmeter1","lpm");
		dbcon.sensors_table_new_insert(5,"flowmeter2","lpm");
	}
	
	static public void setUpArduinoCommunication(){
		com = new ArduinoCommunication();
		MainApp observer = new MainApp();
		
		com.addObserver((Observable o, Object arg) -> {
			insertData((String)arg);
			//System.out.println("notified message is: " + arg);
		});
		com.setATModeCommunication();
	}
	
	static public void insertData(String data){
		byte octet0= 0;
		byte [] dataByteArray=data.getBytes();
		int first0Index=0;
		byte [] newDataByteArray=dataByteArray;
		for(int k=0;k<dataByteArray.length;k++){
			if(dataByteArray[k]==octet0){
				first0Index=k;
			}
		}
		if(first0Index>0)
			newDataByteArray=Arrays.copyOfRange(newDataByteArray,first0Index+1,dataByteArray.length);
				
	
		data=new String(newDataByteArray);
		//System.out.print("new data:");
		//System.out.println(data);
		
		String [] multipleSensorsdata =data.split(",");
		for (String sensorData:multipleSensorsdata){
			String[] dataArray = sensorData.split(";");
			if(dataArray.length <= 2){
				String dataLog="";
				dataLog="No data is saved- data: " +sensorData;
				System.out.println("No data is saved");
				logger.debug(dataLog);
				return;
			}

			String resultFromSelect;
			Timestamp timestamp = new Timestamp(timeStamp);//+ (1000*Integer.parseInt(dataArray[2])));
			
			System.out.println(new Date(timeStamp));
			int sensorId = dbcon.getSensorID(dataArray[0]);
			if(sensorId == -1){
				String dataLog2="";
				dataLog2="ID NOT FOUND"+" No data is saved-data: "+sensorData;
				System.out.println("ID NOT FOUND");
				logger.debug(dataLog2);
				return;
			}
		
			dbcon.sensor_data_table_new_insert(dbcon.getSensorID(dataArray[0]),Double.parseDouble(dataArray[1]),timestamp);
			//logger.debug("Insertion-data: ");
			//logger.debug(data);
		}
		timeStamp+= 1000;
	}

	
	static public void showSensorsTable () {	
		dbcon.show_sensors_table();
	}
	static public void showSensorDataTable () {	
		dbcon.show_sensor_data_table() ;
	}
	static public Document exportSensorDataTableToXMLDocument (String dateDebut,String dateFin) {
		Document resultatXML;	
		resultatXML = dbcon.exportXML_sensor_data_table(dateDebut,dateFin) ;
		//dbcon.writeXMLDocumentFromSensorDataTable(resultatXML,dateDebut,dateFin);
		return resultatXML;
	}
	static public String XMLDocumentToString (Document doc) {
		try{
			TransformerFactory transformerFactory = TransformerFactory.newInstance();
			Transformer transformer = transformerFactory.newTransformer();
			DOMSource source = new DOMSource(doc);
			StringWriter writer =new StringWriter();
			StreamResult result = new StreamResult(writer);
			transformer.transform(source, result);
			return writer.toString();
		}catch(Exception e){
			e.printStackTrace();
		}
		return null;
	}
	static public void exportSensorDataTableToCSV (String dateDebut,String dateFin) {	
		dbcon.exportCSV_sensor_data_table(dateDebut,dateFin) ;
	}
	static public void getDataSampleFromThermo1_insertInSensorDataTable () {
		//timeStamp = new Date().getTime();	
		com.getDataFromThermo1();
	}	
	static public void getDataSampleFromThermo1_insertInSensorDataTable (int timeDataPeriod) {	
		timeStamp = new Date().getTime();	
		com.getDataFromThermo1(timeDataPeriod);	
	}
	
	static public void getDataSampleFromThermo2_insertInSensorDataTable () {
		//timeStamp = new Date().getTime();	
		com.getDataFromThermo2();
	}	
	static public void getDataSampleFromThermo2_insertInSensorDataTable (int timeDataPeriod) {	
		timeStamp = new Date().getTime();	
		com.getDataFromThermo2(timeDataPeriod);
		
	}
	static public void getDataSampleFromAllSensors_insertInSensorDataTable () {
		//timeStamp = new Date().getTime();	
		com.getDataFromAllSensors();
	}	
	static public void getDataSampleFromAllSensors_insertInSensorDataTable (int timeDataPeriod) {	
		if(timeStamp==0)
			timeStamp = new Date().getTime();	
		com.getDataFromAllSensors(timeDataPeriod);	
	}
	
	static public void startDataCollection (Date startDate, int timeDataPeriod) {	
		timeStamp = startDate.getTime();
		for(int i=0;i<timeDataPeriod;i++){
			getDataSampleFromAllSensors_insertInSensorDataTable (1);
			try{
				Thread.sleep(1000);
			}catch(Exception e){
				e.printStackTrace();
			}
		}
		
		/*try{
			SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
			//Date formattedStartDate = dateFormat.format(startDate);
			Date nowDate= new Date();
			String startDateString=sdf.format(startDate);
			String nowDateString=sdf.format(nowDate);
			if (startDate.before(nowDate)){
				String msg="Past startDate for startDataCollection()";
				System.out.println("Past startDate("+ startDateString +") for startDataCollection()");
				return ;
			}
			System.out.println("startDataCollection()---startDate: "+startDateString);
			System.out.println("startDataCollection()---nowDateString: "+nowDateString);
			long waitingTimeBeforeStart;

			//if(nowDate.before(startDate)){
				//nowDate=new Date();
				waitingTimeBeforeStart=startDate.getTime()-nowDate.getTime();
				System.out.println("startDataCollection() -------Waiting : "+waitingTimeBeforeStart+"ms");
				Thread.sleep(waitingTimeBeforeStart);
			//}
			timeStamp = startDate.getTime(); //new Date().getTime();	
			System.out.println("startDataCollection() -------Go for "+timeDataPeriod+" seconds");
			com.getDataFromAllSensors(timeDataPeriod);
		} catch(Exception e){
			e.printStackTrace();
		}	*/
	}
	
	static public void stopArduinoSending () {
		if (isCollectingDataFromArduino()){		
			com.stopSendingDataFromArduino();
			System.out.println("Sends STOP to Arduino!!");
		}
	}
	
	static public boolean isCollectingDataFromArduino () {		
		return com.isCollectingData();
	}
	
		
	public static void main (String args[])  throws InterruptedException {
		//Begin communication with Arduino & Db
		setUpArduinoCommunication();		
		setUpDataBase ();
		//String date1="2015-09-01";
		//String date2="2015-09-05";
		//String datePlusTardString="2015-09-02 14:19:00";
		//SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
		//Document resultXMLDocument;
		//Date nowDate= new Date();
		
		//Re-initialize sensors db
		//initialiseSensorsTable ();
		//fillSensorsTable ();
		//showSensorsTable () ;
		//showSensorDataTable ();
		//Re-initialize (drop&re-create table) SensorData db
		//initialiseSensorDataTable ();
		
		//try{
			//Date datePlusTard = sdf.parse(datePlusTardString);
			//startDataCollection(datePlusTard,1);
		//}catch(Exception e){
			//e.printStackTrace();
		//}
		

		//ask Arduino for data from thermo1
		//getDataSampleFromThermo1_insertInSensorDataTable (4);
		//ask Arduino for data from thermo2
		//getDataSampleFromThermo2_insertInSensorDataTable (4);
		//ask Arduino for data from all sensors
		
		initialiseSensorDataTable ();
		
		//com.setValve((float)0.101,(float)4);
		
		
		//for(int i=0;i<20;i++){
			//getDataSampleFromAllSensors_insertInSensorDataTable (1);
			//Thread.sleep(1000);
		//}
		//Thread.sleep(3000);
		//com.setValve((float)0,0);
		
		//getDataSampleFromAllSensors_insertInSensorDataTable (1200);
		//Thread.sleep(10000);
		//com.setValve((float)0.101,5);
		//Thread.sleep(10000);
		//com.setValve((float)0.101,5);
		//set actuator
		
		
		
		//Stop arduino sendings
		//stopArduinoSending ();
		//resultXMLDocument = exportSensorDataTableToXMLDocument (date1,date2);
		
		//Logger logRoot = Logger.getRootLogger();
		//ConsoleAppender ca = new ConsoleAppender();
		//ca.setName("console");
		//ca.setLayout(new SimpleLayout());
		//ca.activateOptions();
		//logRoot.addAppender(ca);
		//logRoot.setLevel(Level.DEBUG);
		//logRoot.debug("message 1");
		
		//logger.setAdditivity(false);
		try{
			File myLogFile = new File("./logFiles/log.txt");
			myLogFile.delete();
			FileAppender fa = new FileAppender(new XMLLayout(),"./logFiles/log.txt");
			fa.setName("FichierLog");
			logger.addAppender(fa);
		}catch(IOException e){
			e.printStackTrace();
		}
		
		//Stop arduino
		//stopArduinoSending ();
		
	
	}
	
}

