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

public class MainApp{
	
	private static Logger logger= Logger.getLogger(MainApp.class);
	
	static String answer = "";
	static boolean getLiveData = false;
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
	
	 public static void setUpArduinoCommunication(){
		com = new ArduinoCommunication();
		MainApp observer = new MainApp();
		
		com.addObserver((Observable o, Object arg) -> {
			receive_insertDataIntoDB_andNotifyServlet((String)arg);
			if(getLiveData){
				parseToServlet((String)arg);
				getLiveData = false;
			}
			System.out.println("notified message is: " + arg);
		});
		com.setATModeCommunication();
	}
	public static  String cleanSerialData(String data){
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
				
		//System.out.println("new cleanSerialData: "+ new String(newDataByteArray));
		return new String(newDataByteArray);
		//r data;
		//System.out.print("new data:");
		//System.out.println(data);	
	}
	 public static void receive_insertDataIntoDB_andNotifyServlet(String data){
		
		String cleanData = cleanSerialData(data);
		String [] multipleSensorsdata =cleanData.split(",");
		for (String sensorData:multipleSensorsdata){
			ArrayList<String> dataArray = new ArrayList();
			for (String str:sensorData.split(";")){
				dataArray.add(str);
			}
			if(dataArray.size() <= 2){
				String dataLog="";
				dataLog="No data is saved- data: " +sensorData;
				System.out.println("No data is saved");
				logger.debug(dataLog);
				return;
			}

			String resultFromSelect;
			Timestamp timestamp = new Timestamp(timeStamp);//+ (1000*Integer.parseInt(dataArray[2])));
			
			System.out.println(new Date(timeStamp));
			int sensorId = dbcon.getSensorID(dataArray.get(0));
			if(sensorId == -1){
				String dataLog2="";
				dataLog2="ID NOT FOUND"+" No data is saved-data: "+sensorData;
				System.out.println("ID NOT FOUND");
				logger.debug(dataLog2);
				return;
			}
		
			dbcon.sensor_data_table_new_insert(dbcon.getSensorID(dataArray.get(0)),Double.parseDouble(dataArray.get(1)),timestamp);
			//logger.debug("Insertion-data: ");
			//logger.debug(data);
			
			// notify servlet
		}
		
		timeStamp+= 1000;
	}
	public static String getLiveData(){
		getLiveData = true;
		answer = "";
		getDataSampleFromAllSensors_insertInSensorDataTable(0);
		while(answer.equals("")){}
		System.out.println("Received answer: " + answer);
		return answer;
	}
	
	public  static void parseToServlet(String sensors){
		String cleanSensors = cleanSerialData(sensors);
		String [] multipleSensorsdata =cleanSensors.split(",");
		try{
			SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss");
			String timestampString  = dateFormat.format(timeStamp);
			System.out.println("timestampString: " +timestampString);
			Document doc = convertToXML(multipleSensorsdata);
			answer = XMLDocumentToString(doc);
			System.out.println("Prepared answer: " + answer);
		}catch(Exception e)	{
			answer = "ERROR";
			resetCommunication();
			getLiveData = false;
		}
		
	}
	
	/*public String getAnswer(){
		String valueToReturn= "";
		if(!answer.equals("")){
			valueToReturn= answer;
			answer = "";
		}
		return valueToReturn;
	}*/

	public static void resetCommunication(){
			com = null;
			setUpArduinoCommunication();
	}
	
	public  static Document convertToXML(String [] sensorList) throws Exception {
		
		Document resultXMLDocument;
		DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
		// root elements
		resultXMLDocument = docBuilder.newDocument();
		Element rootElement = resultXMLDocument.createElement("Sensors");
		resultXMLDocument.appendChild(rootElement);
		
		for(String sensor: sensorList){
			String[] sensorElements = sensor.split(";");
			String sensor_name = sensorElements[0];
			String sensor_value = sensorElements[1];
			String sensor_data_timestamp = sensorElements[2];
			// sensorData elements
			Element sensorElement = resultXMLDocument.createElement("Sensor");
			rootElement.appendChild(sensorElement);
			// sensorName elements
			Element sensorName = resultXMLDocument.createElement("sensorName");
			sensorName.appendChild(resultXMLDocument.createTextNode(sensor_name));
			sensorElement.appendChild(sensorName);
			// sensorDataValue, sensorDataTimestamp elements
			Element sensorDataValue = resultXMLDocument.createElement("dataValue");
			sensorDataValue.appendChild(resultXMLDocument.createTextNode(sensor_value));
			sensorElement.appendChild(sensorDataValue);
			Element sensorDataTimestamp = resultXMLDocument.createElement("timestamp");
			sensorDataTimestamp.appendChild(resultXMLDocument.createTextNode(sensor_data_timestamp));
			sensorElement.appendChild(sensorDataTimestamp);
		}
		return resultXMLDocument;
	
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
	static public void setValve (float kwh,float flow_rate) {	
		com.setValve(kwh,flow_rate);	
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
		
		//initialiseSensorDataTable ();
		
		//com.setValve((float)0.101,(float)4);
		
		
		for(int i=0;i<20;i++){
			getLiveData=true;
			getDataSampleFromAllSensors_insertInSensorDataTable (1);
			Thread.sleep(1000);
		}
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

