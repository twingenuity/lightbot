// This #include statement was automatically added by the Particle IDE.
#include "lib1.h"

// This #include statement was automatically added by the Particle IDE.
#include "SparkFunRHT03/SparkFunRHT03.h"

// This #include statement was automatically added by the Particle IDE.
#include "SparkFunPhant/SparkFunPhant.h"

// This #include statement was automatically added by the Particle IDE.
#include "SparkFunMAX17043/SparkFunMAX17043.h"

// This #include statement was automatically added by the Particle IDE.
    #include "Adafruit_TSL2591/Adafruit_TSL2591.h"
    #include "Adafruit_TSL2591/Adafruit_Sensor.h"
    
 
    
// connect SCL to D1
// connect SDA to D0
// connect Vin to 3.3-5V DC
// connect GROUND to common ground

//RTH03 Humidity/Temp Sensor Photon
//VCC (Red) ------------- 3.3V (VCC)
//GND (Black) ----------- GND
//SIG (White) ----------- D4   
  
  
  
PRODUCT_ID(2125);  
PRODUCT_VERSION(1);  

#define RHT03_DATA_PIN D4
    
Adafruit_TSL2591 tsl = Adafruit_TSL2591(0004); // pass in a number for the sensor identifier (for your use later)
RHT03 rht; 


//Global Variables
String device_uid = ""; // photon hard coded device id, UID
double voltage = 0; // Variable to keep track of LiPo voltage
double soc = 0; // Variable to keep track of LiPo state-of-charge (SOC)
float humidity = 0;//humidity from rht03
float tempC = 0;// temperature in C from RHT03
float tempF = 0;// temperature in F from RHT03
int count = 0;//This triggers a post and print on the first time through the loop
float ir  = 0;// IR from tsl2591
float full  = 0;//Full spectrum from tsl2591
float visible  = 0; //Visible light from tsl2591
float lux  = 0; //LUX calculated from TSL2591
float par = 0; //PAR cacluated from LUX TSL2591

////////////PHANT STUFF//////////////////////////////////////////////////////////////////
const char server[] = "ec2-52-40-13-117.us-west-2.compute.amazonaws.com";
const char path[] = "api/metrics/light"; //old = QGyxppE3dKFd4RrmbrXg
const char port[] = "8080"; // old = Jqyx44PGleUzWRr2Mr4A
//String myIDStr = "notsetyet";
PhantRest phant(server, path, port);
/////////////////////////////////////////////////////////////////////////////////////////    

void setup(void)
    {
      Serial.begin(9600);
      
      device_uid.reserve(30);
      

    { /// get deviceID. code block isolated in brackets
      device_uid=System.deviceID();
      Serial.print("Deviceid: ");
      Serial.println(device_uid);
    }
    // RHT03 initializtion
      rht.begin(RHT03_DATA_PIN);
      Serial.println("Starting Adafruit TSL2591 Test!");
    
      if (tsl.begin())
      {
        Serial.println("Found a TSL2591 sensor");
      }
      else
      {
        Serial.println("No sensor found ... check your wiring?");
        while (1);
      }
    
      /* Display some basic information on this sensor */
      displaySensorDetails();
    
      /* Configure the sensor */
      configureSensor();
      
    
    
    // Set up PArticle variables (voltage, soc, and alert):
	Particle.variable("voltage", &voltage, DOUBLE);
	Particle.variable("soc", &soc, DOUBLE);

    // Set up the MAX17043 LiPo fuel gauge:
	lipo.begin(); // Initialize the MAX17043 LiPo fuel gauge
    // Quick start restarts the MAX17043 in hopes of getting a more accurate
    // guess for the SOC.
	lipo.quickStart();
	
    }
    
    
//******************************************************************************************    
void loop(void)
{
    // Call rht.update() to get new humidity and temperature values from the sensor.
	int updateRet = rht.update();
	
	// If successful, the update() function will return 1.
	// If update fails, it will return a value <0
	if (updateRet == 1)
	{
		// The humidity(), tempC(), and tempF() functions can be called -- after 
		// a successful update() -- to get the last humidity and temperature
		// value 
		float humidity = rht.humidity();
		float tempC = rht.tempC();
		float tempF = rht.tempF();
		
		// Now print the values:
		Serial.println("Humidity: " + String(humidity, 1) + " %");
		Serial.println("Temp (F): " + String(tempF, 1) + " deg F");
		Serial.println("Temp (C): " + String(tempC, 1) + " deg C");
	}
	else
	{
		// If the update failed, try delaying for RHT_READ_INTERVAL_MS ms before
		// trying again.
		delay(RHT_READ_INTERVAL_MS);
	}
	
	delay(2000);
        
      displaySensorDetails();
      
      delay(500);
      
      advancedRead();
    
      delay(2000);
      
      getBattery();
    
      delay(500); 
      
      postToPhant();//upload data to Phant
      
      delay(1000);
     // unifiedSensorAPIRead();
      
      //delay(2000); 
      
      
     delay(60000);//stay awake for 60 seconds to allow for App updates
    //Power down between sends to save power, measured in seconds.
     System.sleep(SLEEP_MODE_DEEP, 500);  //for Particle Photon 7+ minutes
    //(includes 20 sec update delay) between postings-change this to alter update rate 
}   
    
void configureSensor(void)
    {
      // You can change the gain on the fly, to adapt to brighter/dimmer light situations
      tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
      //tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
      // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
    
      // Changing the integration time gives you a longer time over which to sense light
      // longer timelines are slower, but are good in very low light situtations!
      tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
      // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
      // tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
      // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
      // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
      // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)
    
      /* Display the gain and integration time for reference sake */
      Serial.println("------------------------------------");
      Serial.print  ("Gain:         ");
      tsl2591Gain_t gain = tsl.getGain();
      switch(gain)
      {
        case TSL2591_GAIN_LOW:
          Serial.println("1x (Low)");
          break;
        case TSL2591_GAIN_MED:
          Serial.println("25x (Medium)");
          break;
        case TSL2591_GAIN_HIGH:
          Serial.println("428x (High)");
          break;
        case TSL2591_GAIN_MAX:
          Serial.println("9876x (Max)");
          break;
      }
      Serial.print  ("Timing:       ");
      Serial.print((tsl.getTiming() + 1) * 100, DEC);
      Serial.println(" ms");
      Serial.println("------------------------------------");
      Serial.println("");
    }
    
  
   
void displaySensorDetails(void)
    {
      sensor_t sensor;
      tsl.getSensor(&sensor);
      Serial.println("------------------------------------");
      Serial.print  ("Sensor:       "); Serial.println(sensor.name);
      Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
      Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
      Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
      Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
      Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");
      Serial.println("------------------------------------");
      Serial.println("");
      delay(500);
    }  
    
    
void advancedRead(void)
    {
      // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
      // That way you can do whatever math and comparisons you want!
      uint32_t lum = tsl.getFullLuminosity();
      uint16_t ir, full;
      ir = lum >> 16;
      full = lum & 0xFFFF;
      Serial.print("[ "); Serial.print(millis()); Serial.print(" ms ] ");
      Serial.print("IR: "); Serial.print(ir);  Serial.print("  ");
      Serial.print("Full: "); Serial.print(full); Serial.print("  ");
      Serial.print("Visible: "); Serial.print(full - ir); Serial.print("  ");
      Serial.print("Lux: "); Serial.println(tsl.calculateLux(full, ir));
      float tLux = (tsl.calculateLux(full, ir));
      Particle.publish("Lux: ",  String(tLux));
        delay(500);
      par = (tLux/54);
      ir = (ir);
      full = (full);
      visible = (full - ir);
      lux = (tLux);
    }

//**************************************************************************************
void unifiedSensorAPIRead(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  tsl.getEvent(&event);
  

  /* Display the results (light is measured in lux) */
  Serial.print("[ "); Serial.print(event.timestamp); Serial.print(" ms ] ");
  if ((event.light == 0) |
      (event.light > 4294966000.0) |
      (event.light <-4294966000.0))
  {
    /* If event.light = 0 lux the sensor is probably saturated */
    /* and no reliable data could be generated! */
    /* if event.light is +/- 4294967040 there was a float over/underflow */
    Serial.println("Invalid data (adjust gain or timing)");
  }
  else
  {
     
    Serial.print(event.light); Serial.println(" lux");
    lux = (event.light);
    
    //PAR conversion***  PAR = Lux/constant = micromols.m^2.s - sunlight's constant = 54 - we'll use this constant for now!
    
    par = (lux/54);
    ir = (ir);
    full = (full);
    visible = (full - ir);
  }
}
//*******************************************************************************************
void getBattery()
    {
    // lipo.getVoltage() returns a voltage value (e.g. 3.93)
    voltage = lipo.getVoltage();
    // lipo.getSOC() returns the estimated state of charge (e.g. 79%)
    soc = lipo.getSOC();


    }    

 
//---------------------------------------------------------------
int postToPhant()//sends data to data.sparkfun.com
{
    
   
    
    phant.add("battery", soc);
    phant.add("infrared", ir);
    phant.add("deviceid", device_uid);
    phant.add("fullspec", full);
    phant.add("par", par);
	phant.add("lux", lux);	 
    phant.add("humidity", rht.humidity(), 1); 
    phant.add("tempc", rht.tempC(), 1); 
    phant.add("tempf", rht.tempF(), 1);   
	phant.add("volts", voltage);
    phant.add("visible", visible);
    
    TCPClient client;
    char response[512];
    int i = 0;
    int retVal = 0;

    if (client.connect(server, 8080))
    {
        Serial.println("Posting!");
        client.print(phant.post());
        delay(1000);
        while (client.available())
        {
            char c = client.read();
            Serial.print(c);
            if (i < 512)
                response[i++] = c;
        }
        if (strstr(response, "200 OK"))
        {
            Serial.println("Post success!");
            retVal = 1;
        }
        else if (strstr(response, "400 Bad Request"))
        {
            Serial.println("Bad request");
            retVal = -1;
        }
        else
        {
            retVal = -2;
        }
    }
    else
    {
        Serial.println("connection failed");
        retVal = -3;
    }
    client.stop();
    return retVal;

}
