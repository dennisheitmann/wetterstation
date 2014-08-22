#include <VirtualWire.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xDD, 0xDD };
IPAddress ip(192,168,0,97);
EthernetServer server(23);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// Virtual Wire PIN
const int receive_pin = 8;

const char valueTxt[ ] = ".value";

char c1[VW_MAX_MESSAGE_LEN] = "";
char c2[VW_MAX_MESSAGE_LEN] = "";
char mb[VW_MAX_MESSAGE_LEN] = "";
char hu[VW_MAX_MESSAGE_LEN] = "";
char message[VW_MAX_MESSAGE_LEN] = "";

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long interval = 600000; 

void setup()
{
    delay(1000);
    Serial.begin(9600);

    // Initialise the IO and ISR
    vw_set_rx_pin(receive_pin);
    vw_setup(4800);	 // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
    
    lcd.begin(5, 2);
    lcd.print("setup");
    lcd.setCursor(0, 1);
    lcd.print("setup");
    delay(500);
    lcd.clear();
      
    delay(50);
    Ethernet.begin(mac, ip);
    delay(50);
    server.begin();
}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
	int i = 0; 
        int j = 0;
	// Message with a good checksum received, dump it.
	for (i = 0; i < buflen; i++)
	{
            j = i + strlen(valueTxt);
            if (i < 4)
            {
              message[i] = (char)buf[i];
            }
            if (i == 4)
            {
              strcat(message, valueTxt);
              message[j] = (char)buf[i];
            }
            if (i > 4)
            {
              message[j] = (char)buf[i];
            }
            if (i < 6)  
            {
              lcd.setCursor(i, 0);
            } else {
              lcd.setCursor((i - 5), 1);
            }
            lcd.print((char)buf[i]);
	}
	//Serial.println();
        if (message[0] == 't')
        {
          if (message[1] == '1')
          {
            strcpy(c1, message);
          }
          if (message[1] == '2')
          {
            strcpy(c2, message);
          }
        }
        if (message[0] == 'm')
        {
          strcpy(mb, message);
        }
        if (message[0] == 'h')
        {
          strcpy(hu, message);
        }
        Serial.println(c1);
        Serial.println(c2);
        Serial.println(mb);
        Serial.println(hu);
        // message löschen
        memset(message, '\0', sizeof(message));
        // Zeitpunkt für den Timer setzen
        currentMillis = millis();
    }
    // Länger als interval Millisec. kein Signal, dann c1, c2, mb und hu leeren
    if((currentMillis - previousMillis) > interval)
    {
      // message löschen
      memset(message, '\0', sizeof(message));
      strcpy(c1, message);
      strcpy(c2, message);
      strcpy(mb, message);
      strcpy(hu, message);
      // Timer setzen
      previousMillis = currentMillis;
    }


    // Ausgabe im Munin-Format erzeugen
    muninEthernet();
}

void muninEthernet() {
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      client.print(c1);
      client.print(";");
      client.print(c2);
      client.print(";");
      client.print(mb);
      client.print(";");
      client.print(hu);
      client.println();
      delay(1);
      break;
    }
  }
  client.stop();
}