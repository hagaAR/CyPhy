import java.util.Date;
import com.pi4j.io.serial.Serial;
import com.pi4j.io.serial.SerialDataEvent;
import com.pi4j.io.serial.SerialDataListener;
import com.pi4j.io.serial.SerialFactory;
import com.pi4j.io.serial.SerialPortException;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import java.util.Observable;
//import com.rapplogic.xbee.api.XBee;
//import com.rapplogic.xbee.api.XBeeAddress16;
//import com.rapplogic.xbee.api.XBeeAddress64;
//import com.rapplogic.xbee.api.XBeeException;
//import com.rapplogic.xbee.api.XBeeResponse;
//import com.rapplogic.xbee.api.zigbee.ZNetExplicitTxRequest;
//import com.rapplogic.xbee.api.zigbee.ZNetTxRequest;
//import com.rapplogic.xbee.util.DoubleByte;


public class ArduinoCommunication extends Observable {
	static Serial serial;
	static int countingReceivedMsg;
	static String message;
	//static XBee xbee;
	
	
	public void ArduinoCommunication(){
		countingReceivedMsg=0;
	}
	
	public void setATModeCommunication(){
		serial = SerialFactory.createInstance();
		//System.out.println("Instance created");
		serial.open(serial.DEFAULT_COM_PORT, 9600);
		//System.out.println("Port Serie n°: "+serial.DEFAULT_COM_PORT);
		 // create and register the serial data listener
        serial.addListener(new SerialDataListener() {
            @Override
            public void dataReceived(SerialDataEvent event) {
                // print out the data received to the console
                countingReceivedMsg+=1;
                //System.out.print("msg #"+countingReceivedMsg+" ");
                //System.out.printf(event.getData());
                message = event.getData();
                checkMessage();
                setChanged();
                notifyObservers(message);
            }            
        });	
	}
	
	//static public void setAPIModeCommunication(){
		//xbee = new XBee();
		//try{
			//xbee.open("/dev/ttyAMA0", 9600);
			//System.out.println("Creation of XBee comm");
			//// replace with end device's 64-bit address (SH + SL)
			//XBeeAddress64 addr64 = new XBeeAddress64(0, 0x13, 0xa2, 0, 0x40, 0xbf, 0x05, 0xae);
			
			//// create an array of arbitrary data to send
			//int[] payload = new int[] { 0, 0x66, 0xee };
		
		//} catch (InterruptedException e){
			//System.out.println("marche pas");
		//}
					
	//}
	
	public void sendToArduinoATMode(String data){
		System.out.println("Sending: '"+data+"'");
		//In AT Mode:
		serial.write(data.getBytes());
		
	}
	
	public void sendToArduinoAPIMode(String data){
		System.out.println("Sending: '"+data+"'");

		//In API Mode:
		serial.write((byte)0x7e);//start byte
		serial.write((byte)0x0);//length MSB always on '0'
		serial.write((byte)0x14);//length LSB
		serial.write((byte)0x92);//0x17 is the frame ID for sending an AT command
		serial.write((byte)0x00); //frame ID (no reply needed)
		serial.write((byte)0x00); //send the 64 bit destination adress
		serial.write((byte)0x00);// (sending 0x000000000000fffe (broadcast))
		serial.write((byte)0x00);
		serial.write((byte)0x00);
		serial.write((byte)0x00);
		serial.write((byte)0x00);
		serial.write((byte)0xff);
		serial.write((byte)0xfe);//on broadcast
		serial.write((byte)0x02);
		serial.write((byte)0x44);// 'D'
		serial.write((byte)0x02);// '2'
		serial.write((byte)0x04);
		serial.write((byte)0xf5);//checksum
		
	}
	
	public void sendCommand(String command){
		sendToArduinoATMode(command);
	}
	
	public void checkMessage(){
		//handle messages
		String sensor = "";
		String sensor_value_string= "";
		float sensor_value;
		//System.out.print("message: ");
		//System.out.println(message);
		//System.out.print("message.message.lastIndexOf: ");
		//System.out.println(message.lastIndexOf(";"));
		//System.out.print("message.message.IndexOf: ");
		//System.out.println(message.indexOf(";"));
		//Split instead of substring
		if(message.indexOf(";")>0 ){//&& message.lastIndexOf(";")>message.indexOf(";") ){
			/*sensor=message.substring(0,message.indexOf(";"));
			sensor_value_string=message.substring(message.indexOf(";")+1,message.lastIndexOf(";")-1);
			//sensor_value=Float.parseFloat(sensor_value_string);
			System.out.print("sensor: ");
			System.out.print(sensor);
			System.out.print(" sensor_value_string: ");
			System.out.println(sensor_value_string);
			//System.out.print(" sensor_value: ");
			//System.out.println(sensor_value);*/
			String[] splitMessage = message.split(";");
			sensor = splitMessage[0];
			sensor_value_string = splitMessage[1];
		}
		else{
			message = "";
		}
		//message = sensor + ";" + sensor_value_string;
	}
	
	public String getMessage(){
			return message;
	}
	
	public void getData(){
		sendToArduinoATMode("getData;Thermo1;1;");
		//sendToArduinoATMode("getData;Thermo1");
		//String receivedData = receiveData();
		//return receiveData();
	}
	
	public void getData(int timePeriod){
		sendToArduinoATMode("getData;Thermo1;" + timePeriod+";");
		//String receivedData = receiveData();
		//return receiveData();
	}
	
	//static public void synchroniseTime(){
		//sentToArduinoATMode("setTime;"+ (new Date()).toString());
	//}
}

