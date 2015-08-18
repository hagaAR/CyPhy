import java.util.Date;
import java.util.Scanner;

import com.pi4j.io.serial.Serial;
import com.pi4j.io.serial.SerialDataEvent;
import com.pi4j.io.serial.SerialDataListener;
import com.pi4j.io.serial.SerialFactory;
import com.pi4j.io.serial.SerialPortException;

public class SerialComAPIMode {
	static Serial serial;
	
	static public void sentToArduino(String data){
			serial.writeln(data);
	}
	
	static public void synchroniseTime(){
		
			sentToArduino("setTime;"+ (new Date()).toString());
	}
	

	
	public static void main (String args[])throws InterruptedException {
		// create an instance of the serial communications class
        serial = SerialFactory.createInstance();
		serial.open(serial.DEFAULT_COM_PORT, 9600);
		//serial.open("/dev/ttyS0", 9600);
        // create and register the serial data listener
        serial.addListener(new SerialDataListener() {
            @Override
            public void dataReceived(SerialDataEvent event) {
                // print out the data received to the console
                
                System.out.print("Something is happening:  ");
                System.out.printf(event.getData());
                System.out.println();
            }            
        });
        synchroniseTime();
		while(true){
             serial.writeln("Hello World!");
             
             Thread.sleep(1000);
             
		 }
	}
}


