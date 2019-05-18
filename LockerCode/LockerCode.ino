#include <SoftwareSerial.h>

//Parallax RFID Reader 
#define OFF 3 //sends a HIGH to shut off entire circuit
#define RedLED 7 // On but locked
#define GreenLED 8 // On and unlocked
#define RFIDEnablePin 2 //Pin that enables reading. Set as OUTPUT and LOW to read an RFID tag
#define RFIDSerialRate 2400 //Parallax RFID Reader Serial Port Speed
#define UnlockPin 12 //UnlockPin HIGH for open
//Using SoftwareSerial Library to locate the serial pins off the default set
//This allows the Arduino to be updated via USB with no conflict
#define RxPin 5 //Pin to read data from Reader 
#define TxPin 4 //Pin to write data to the Reader NOTE: The reader doesn't get written to, don't connect this line.
SoftwareSerial RFIDReader(RxPin,TxPin);



#define key_1 "1F00D70B9D" //0014093213
#define key_2 "20005E777D" //0006190973
#define key_3 "1F00FF6302" //0016737026
#define key_4 "1F00D797D8" 
#define key_5 "20007F6DCF"


unsigned int timenow;
String RFIDTAG=""; //Holds the RFID Code read from a tag
String DisplayTAG = ""; //Holds the last displayed RFID Tag
String DisplayTAG2 = ""; //Holds the last displayed RFID Tag

void setup() 
{
  // RFID reader SOUT pin connected to Serial RX pin at 2400bps
  RFIDReader.begin(RFIDSerialRate);

  // Set Enable pin as OUTPUT to connect it to the RFID /ENABLE pin
  pinMode(RFIDEnablePin,OUTPUT); 
  pinMode(UnlockPin, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(OFF, OUTPUT);


  // Activate the RFID reader
  // Setting the RFIDEnablePin HIGH will deactivate the reader
  // which could be usefull if you wanted to save battery life for
  // example.
  digitalWrite(RFIDEnablePin, LOW);
  digitalWrite(RedLED, LOW);
  digitalWrite(GreenLED, HIGH);
  digitalWrite(OFF, LOW);

  Serial.begin(2400);           // set up Serial library at 9600 bps
  Serial.println("Pin all setup!");

}

void loop() 
{
  timenow = millis();
  if(timenow > 15000){
    digitalWrite(OFF, HIGH);
  }
  
  if(RFIDReader.available() > 0) // If data available from reader
  { 
    ReadSerial(RFIDTAG);  //Read the tag number from the reader. Should return a 10 digit serial number
  }

  //This only displays a tag once, unless another tag is scanned
  //if(DisplayTAG!=RFIDTAG)
  //{
  //  DisplayTAG=RFIDTAG;
  //  Serial.println(RFIDTAG);
  //}

  if((RFIDTAG == key_1 ||
    RFIDTAG == key_2 ||
    RFIDTAG == key_3 || RFIDTAG == key_4 || RFIDTAG == key_5) && DisplayTAG2 != RFIDTAG) {
      DisplayTAG2 = RFIDTAG;
      Serial.println("Key Read");
      digitalWrite(UnlockPin, HIGH);
      digitalWrite(RedLED, HIGH);
      digitalWrite(GreenLED, LOW);
      delay(5000);
      digitalWrite(OFF, HIGH);

      //Main function call Location
    }

    
}

void ReadSerial(String &ReadTagString)
{
  int bytesread = 0;
  int  val = 0; 
  char code[10];
  String TagCode="";

  if(RFIDReader.available() > 0) {          // If data available from reader 
    if((val = RFIDReader.read()) == 10) {   // Check for header 
      bytesread = 0; 
      while(bytesread<10) {                 // Read 10 digit code 
        if( RFIDReader.available() > 0) { 
          val = RFIDReader.read(); 
          if((val == 10)||(val == 13)) {   // If header or stop bytes before the 10 digit reading 
            break;                         // Stop reading 
          } 
          code[bytesread] = val;           // Add the digit           
          bytesread++;                     // Ready to read next digit  
        } 
      } 
      if(bytesread == 10) {                // If 10 digit read is complete 

        for(int x=0;x<10;x++)              //Copy the Chars to a String
        {
          TagCode += code[x];
        }
        ReadTagString = TagCode;          //Update the caller
        while(RFIDReader.available() > 0) //Burn off any characters still in the buffer
        {
          RFIDReader.read();
        } 

      } 
      bytesread = 0;
      TagCode="";
    } 
  } 
}
