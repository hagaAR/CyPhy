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
	Serial serial;
	int countingReceivedMsg;
	String message="";
	boolean messageComplete = true;
	String buffer="";
	boolean stopSending=false;
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
				//serial.flush();
                // print out the data received to the console
                if(!stopSending){
					countingReceivedMsg+=1;
					message="";
					message = event.getData();
					checkMessage();
				}
               /* if(messageComplete){
					setChanged();
					notifyObservers(message);
				}*/
            }            
        });	
	}
	
	//static public void setAPIModeCommunication(){
		//xbee = new XBee();
		//try{Thermo1;36.81;62//Thermo1;36.77;63//Thermo1;36.76;64//Thermo1;36.82;65//
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
		System.out.println("============================");
		System.out.print("message received from Serial: ");
		System.out.print(message);
		System.out.println("");
		if(message.contains("/")){
			System.out.println("message contains /");

			/*if(splitMessage.length >= 3){
				splitMessage = message.split(";");
				sensor = splitMessage[0];
				sensor_value_string = splitMessage[1];
			}
			else{
				System.out.print("message.length: ");
				System.out.println(message.length());
				message = "";
			}*/
			if(!messageComplete){
				System.out.println("message isnot complete");
				System.out.print("buffer:");
				System.out.println(buffer);
				System.out.print("message:");
				System.out.println(message);
				message = buffer + message;
				buffer="";
				messageComplete = true;
			}
			
			String[] msgList = message.split("/");
			//System.out.print("msgList.toString(): ");
			for(int k=0;k<msgList.length;k++)
					System.out.println(msgList[k]);
			//System.out.println(msgList.toString());
			//System.out.print("msgList.length: ");
			//System.out.println(msgList.length);
			if(msgList.length> 2){
				System.out.println("message contains at least 2 /");
				//for(String msg: msgList){
				for(int k=0;k<msgList.length;k++){
					String msg="";
					msg=msgList[k];
					if(containsSemiColomns(msg)){
						System.out.println("every split is inserted in db");
						System.out.println(msg);
						setChanged();
						notifyObservers(msg);
						checkArduinoStopped(msg);
						message="";
					}
				}
			}else {
				message = msgList[0];
				System.out.println("message contains 1 /");
				System.out.print("message:");
				System.out.println(message);
				if(msgList.length>1){
					buffer="";
					for(int k=1;k<msgList.length;k++){
						buffer +=msgList [k];
						if(k!=msgList.length-1){
							buffer +="/";
						}
					}
					System.out.print("filling buffer:");
					System.out.println(buffer);
					messageComplete = false;
				}
				setChanged();
				notifyObservers(message);
				checkArduinoStopped(message);
				message="";
			}
			
			
		}else{
			System.out.println("message buffered & set to notComplete because doesnt end with /");
			System.out.print("buffered:");
			System.out.println(message);
			buffer="";
			buffer = message;
			messageComplete= false;
			message="";
		}
		
	}
	
	private boolean containsSemiColomns(String msg){
		if(msg.contains(";"))
			return true;
		return false;
	}
	
	public String getMessage(){
			return message;
	}
	
	public void getDataFromThermo1(){
		sendToArduinoATMode("getData;thermo1;1;");
	}
	
	public void getDataFromThermo1(int timePeriod){
		sendToArduinoATMode("getData;thermo1;" + timePeriod+";");
	}
	public void getDataFromThermo2(){
		sendToArduinoATMode("getData;thermo2;1;");
	}
	
	public void getDataFromThermo2(int timePeriod){
		sendToArduinoATMode("getData;thermo2;" + timePeriod+";");
	}
	public void getDataFromAllSensors(){
		sendToArduinoATMode("getData;all;1;");
	}
	
	public void getDataFromAllSensors(int timePeriod){
		sendToArduinoATMode("getData;all;" + timePeriod+";");
	}
	
	public void setValve(float kWh,float flowSpeed){
		sendToArduinoATMode("setValv;"+kWh+";" + flowSpeed+";");
	}
	
	public void stopSendingDataFromArduino(){
		sendToArduinoATMode("stopAll;all;1;");
		stopSending=true;
	}
	public void checkArduinoStopped(String msg){
		if(msg.equals("stopGetData;all;")){
			stopSending=false;
		}
	}
	public boolean isCollectingData(){
		return !stopSending;
	}
	//static public void synchroniseTime(){
		//sentToArduinoATMode("setTime;"+ (new Date()).toString());
	//}
}

