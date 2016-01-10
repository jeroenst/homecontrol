#include <string.h>
#include <stdio.h>
#include <RemoteTransmitter.h>
#include <NewRemoteTransmitter.h>

/* 
 * Program for controlling and receiving 433MHz RF devices
 * For library download and technical info see https://bitbucket.org/fuzzillogic/433mhzforarduino
 */


// Create a transmitter on address 123, using digital pin 11 to transmit, 
// with a period duration of 260ms (default), repeating the transmitted
// code 2^3=8 times.
NewRemoteTransmitter transmitter(123, 10, 260, 3);

// Intantiate a new ActionTransmitter remote, use pin 11
ActionTransmitter actionTransmitter(10);

// Intantiate a new KaKuTransmitter remote, also use pin 11 (same transmitter!)
KaKuTransmitter kaKuTransmitter(10);

// Intantiate a new Blokker remote, also use pin 11 (same transmitter!)
BlokkerTransmitter blokkerTransmitter(10);

// Intantiate a new Elro remote, also use pin 11 (same transmitter!)
ElroTransmitter elroTransmitter(10);


// http://www.arduino.cc/en/Tutorial/SerialEvent
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  // *** WAIT FOR COMMAND FROM SERIAL PORT ***
  if (stringComplete) 
  {
    char command[20] = "";
    char action[20] = "";
    byte value = 0;
    bool command_ok = 0;
    sscanf(inputString.c_str(),"%s", command);

    if (strcmp(command,"help") == 0)
    {
      Serial.println ("-------------------------------------------------");
      Serial.println ("               Arduino 433 MHz Help");
      Serial.println ("-------------------------------------------------");
      Serial.println (" kaku on <CODE A..F> <ADRES 1..16>");
      Serial.println (" kaku off <CODE A..F> <ADRES 1..16>");
      Serial.println (" newkaku on <ADRES 0..15>");
      Serial.println (" newkaku off <ADRES 0..15>");
      Serial.println (" newkaku dim <ADRES 0..15> <WAARDE 0..15>");
      Serial.println (" newkaku groupon");
      Serial.println (" newkaku groupoff");
      Serial.println (" action on <CODE 0..31> <ADRES A-E>");
      Serial.println (" action off <CODE 0..31> <ADRES A-E>");
      Serial.println (" blokker on <ADRES 1..8>");
      Serial.println (" blokker off <ADRES 1..8>");
      Serial.println (" elro on <CODE 0..31> <ADRES A..D>");
      Serial.println (" elro off <CODE 0..31> <ADRES A..D>");
      Serial.println ("-------------------------------------------------");
      command_ok=1;
    }

    // *** SWITCH NEW KAKU DEVICES ***
    if (strcmp(command,"newkaku") == 0)
    {
      char code = 0;
      int address = 0;
      sscanf(inputString.c_str(),"%s %s %d %d", command, action, &address, &value);
      if (strcmp(action,"setaddress") == 0)
      {
        sscanf(inputString.c_str(),"%s %d", command, &address);
        if ((address >= 0) && (address <= 67108863))
        {
          // Not yet implemented
          // command_ok = 1;
        }
      }
      
      if (strcmp(action,"on") == 0)
      {
        if ((address >= 0) && (address <= 15))
        {
          // Switch unit on
          transmitter.sendUnit(address, true);
          command_ok = 1;
        }
      }
    
      if (strcmp(action,"off") == 0)
      {
        if ((address >= 0) && (address <= 15))
        {
          // Switch unit off
          transmitter.sendUnit(address, false);
          command_ok = 1;
        }
      }
    
      if (strcmp(action,"dim") == 0)
      {
        if ((address >= 0) && (address <= 15) && (value >= 0) && (value <= 15))
        {
          // Set unit 1 to dim-level
          transmitter.sendDim(address, value);
          command_ok = 1;
        }
      }

      if (strcmp(action,"groupon") == 0)
      {
        // Set group on
        transmitter.sendGroup(true);
        command_ok = 1;
      }

      if (strcmp(action,"groupoff") == 0)
      {
        // Set group off
        transmitter.sendGroup(false);
        command_ok = 1;
      }
    }

    // *** SWITCH KAKU DEVICES ***
    if (strcmp(command,"kaku") == 0)
    {
      char code = 0;
      byte address = 0;
      sscanf(inputString.c_str(),"%s %s %c %d", command, action, &code, &address);
      if ((address >= 1) && (address <= 16) && (code >= 'A') && (code <= 'P'))
      {
        if (strcmp(action,"on") == 0)
        {
          // Switch unit on
          kaKuTransmitter.sendSignal(code,address,true);
          command_ok = 1;
        }
    
        if (strcmp(action,"off") == 0)
        {
          // Switch unit off
          kaKuTransmitter.sendSignal(code,address,false);
          command_ok = 1;
        }
      }
    }

    // *** SWITCH ACTION DEVICES ***
    if (strcmp(command,"action") == 0)
    {
      byte code = 0;
      char address = 0;
      sscanf(inputString.c_str(),"%s %s %d %c", command, action, &code, &address);
      if ((code >= 0) && (code <= 31) && (address >= 'A') && (address <= 'E'))
      {
        if (strcmp(action,"on") == 0)
        {
          // Switch unit on
          actionTransmitter.sendSignal(code,address,true);
          command_ok = 1;
        }
    
        if (strcmp(action,"off") == 0)
        {
          // Switch unit off  
          actionTransmitter.sendSignal(code,address,false);
          command_ok = 1;
        }
      }
    }
    
    // *** SWITCH BLOKKER DEVICES ***
    if (strcmp(command,"blokker") == 0)
    {
      byte address = 0;
      sscanf(inputString.c_str(),"%s %s %d", command, action, &address);
      if ((address >= 1) && (address <= 8))
      {
        if (strcmp(action,"on") == 0)
        {
          // Switch unit on
          blokkerTransmitter.sendSignal(address,true);
          command_ok = 1;
        }
    
        if (strcmp(action,"off") == 0)
        {
          // Switch unit off  
          blokkerTransmitter.sendSignal(address,false);
          command_ok = 1;
        }
      }
    }

    // SWITCH ELRO DEVICES
    if (strcmp(command,"elro") == 0)
    {
      byte code = 0;
      char address = 0;
      sscanf(inputString.c_str(),"%s %s %d %c", command, action, &code, &address);
      if ((code >= 0) && (code <= 31) && (address >= 'A') && (address <= 'D'))
      {
        if (strcmp(action,"on") == 0)
        {
          // Switch unit on
          elroTransmitter.sendSignal(code,address,true); 
          command_ok = 1;
        }
    
        if (strcmp(action,"off") == 0)
        {
          // Switch unit off  
          elroTransmitter.sendSignal(code,address,false); 
          command_ok = 1;
        }
      }
    }
        
    if (command_ok != 0)
    {
      Serial.print("ok: ");
      Serial.print(inputString);
    }
    else
    {
      Serial.print("fail: ");
      Serial.print(inputString);
    }
 
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void serialEventRun(void) {
  if (Serial.available()) serialEvent();
} 

