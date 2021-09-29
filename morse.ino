#include <SoftwareSerial.h>
#define RxD 10
#define TxD 6
SoftwareSerial BlueToothSerial(RxD,TxD);

#define serial_BaudRate 9600
#define bt_BaudRate 38400

int dot = 1;
int dash = 2;

int dictionary[36][6] =
{
  // A - Z
  {dot,dash,0},
  {dash,dot,dot,dot,0},
  {dash,dot,dash,dot,0},
  {dash,dot,dot,0},
  {dot,0},
  {dot,dot,dash,dot,0},
  {dash,dash,dot,0},
  {dot,dot,dot,dot,0},
  {dot,dot,0},
  {dot,dash,dash,dash,0},
  {dash,dot,dash,0},
  {dot,dash,dot,dot,0},
  {dash,dash,0},
  {dash,dot,0},
  {dash,dash,dash,0},
  {dot,dash,dash,dot,0},
  {dash,dash,dot,dash,0},
  {dot,dash,dot,0},
  {dot,dot,dot,0},
  {dash,0},
  {dot,dot,dash,0},
  {dot,dot,dot,dash,0},
  {dot,dash,dash,0},
  {dash,dot,dot,dash,0},
  {dash,dot,dash,dash,0},
  {dash,dash,dot,dot,0},

  // 0 - 9
  {dash,dash,dash,dash,dash,0},
  {dot,dash,dash,dash,dash,0},
  {dot,dot,dash,dash,dash,0},
  {dot,dot,dot,dash,dash,0},
  {dot,dot,dot,dot,dash,0},
  {dot,dot,dot,dot,dot,0},
  {dash,dot,dot,dot,dot,0},
  {dash,dash,dot,dot,dot,0},
  {dash,dash,dash,dot,dot,0},
  {dash,dash,dash,dash,dot,0}
};

bool doDot = false;
int timeUnit = 200;
int dotTime = timeUnit * 1;
int dashTime = timeUnit * 3;
int letterSpace = timeUnit * 3;
int wordSpace = timeUnit * 7;
int delayTime = 100;

void ledOn(int delayTime)
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayTime);
    digitalWrite(LED_BUILTIN, LOW);
    delay(timeUnit);
}

void messageToMorse(char message[])
{
  int charIndex = 0;
  while (message[charIndex] != '\0')
  {
    int charValue = message[charIndex++];

    // Is this a space between words?
    if (charValue == ' ')
    {
      // Yes, so delay for a word space.
      delay(wordSpace);
    }
    else
    {
      // No, so we're looking at a character.

      // If the character is uppercase, make it lowercase.
      if (charValue >= 'A' && charValue <= 'Z')
      {
        charValue = charValue - 'A' + 'a';
      }

      // If the character is numeric, remap into our array (after the 26 alphabetic characters).
      if (charValue >= '0' && charValue <= '9')
      {
        charValue = charValue - '0' + 'a' + 26;
      }
      
      charValue = charValue - 'a';
  
      int dotDashIndex = 0;
      while (dictionary[charValue][dotDashIndex] != 0)
      {
        if (dictionary[charValue][dotDashIndex++] == dot)
        {
          ledOn(dotTime);
        }
        else
        {
          ledOn(dashTime);
        }
      }
      
      delay(letterSpace);
    }

  }
  
  delay(wordSpace);
}

void sendBlueToothCommand(char* Command) {

 BlueToothSerial.print(Command);
 Serial.print(Command); 
 delay(100);
}

void setupBlueTooth() 
{
  Serial.println("Bluetooth Initialization ...");      
  sendBlueToothCommand("AT+NAME=LinkSprite\r\n");
  sendBlueToothCommand("AT+ROLE=0\r\n");
  sendBlueToothCommand("AT+CMODE=0\r\n");
  sendBlueToothCommand("AT+PSWD=1234\r\n");
  String uartString = "AT+UART=" + String(bt_BaudRate) + ",0,0\r\n";
  sendBlueToothCommand(uartString.c_str());
  delay(500);
  Serial.println("Bluetooth Initialized Successfully !\r\n");
}
/*
int charsInMessage = 0;
char messageFromBT[200];

char myMessage[] = "SOS";
*/

void setup() 
{
  pinMode(RxD, INPUT); // Setup the Arduino to receive INPUT from the bluetooth shield on Digital Pin 10
  pinMode(TxD, OUTPUT); // Setup the Arduino to send data (OUTPUT) to the bluetooth shield on Digital Pin 6
  pinMode(LED_BUILTIN, OUTPUT); // Use onboard LED if required.

  // Open serial communications and wait for port to open:
  Serial.begin(serial_BaudRate);
  while (!Serial) 
  { // wait for serial port to connect. Needed for Native USB only 
  }
  Serial.println("Serial ready!");

  BlueToothSerial.begin(bt_BaudRate);
  setupBlueTooth();
  Serial.println("BlueToothSerial ready!");
}

char receivedMessage[2000];


int charCount = 0;
bool finished = false;

void loop() 
{
  if (Serial.available())
  {
    int nextChar = Serial.read();
    if (nextChar == 10)
    {
      receivedMessage[charCount++] = '\0';
      finished = true;
    }
    else
    {
      receivedMessage[charCount++] = nextChar;
    }
  }

  if (BlueToothSerial.available())
  {
    Serial.println("Received something on BlueTooth!");
    int nextChar = BlueToothSerial.read();
    if (nextChar == 0)
    {
      receivedMessage[charCount++] = '\0';
      finished = true;
    }
    else
    {
      receivedMessage[charCount++] = nextChar;
    }
    Serial.println(nextChar, DEC);
  }

  if (finished)
  {
    Serial.print("Received:");
    Serial.println(receivedMessage);
    BlueToothSerial.print("Received:");
    BlueToothSerial.println(receivedMessage);
    
    messageToMorse(receivedMessage);
    
    charCount = 0;
    finished = false;
  }
}
/*
  Serial.begin(9600); // Allow Serial communication via USB cable to computer (if required)
  pinMode(RxD, INPUT); // Setup the Arduino to receive INPUT from the bluetooth shield on Digital Pin 6
  pinMode(TxD, OUTPUT); // Setup the Arduino to send data (OUTPUT) to the bluetooth shield on Digital Pin 7
  pinMode(13,OUTPUT); // Use onboard LED if required.
  delay(500);
  setupBlueTooth();
  delay(500);

  bool finished = false;
  while (BlueToothSerial.available() > 0)
  {
    Serial.println("Bluetooth data received!\r\n");
    messageToMorse(myMessage);
    char nextChar = BlueToothSerial.read();
    finished = (nextChar == '\0');
    messageFromBT[charsInMessage++] = nextChar;
  }

  if (finished)
  {
//    messageToMorse(messageFromBT);
  }
}
*/
