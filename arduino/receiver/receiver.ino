#include "VirtualWire.h"
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xDD, 0xDD };
IPAddress ip(192,168,0,97);
EthernetServer server(23);

// Virtual Wire PIN
const int receive_pin = 8;

const char valueTxt[ ] = ".value";

char t1[VW_MAX_PAYLOAD] = "";
char t2[VW_MAX_PAYLOAD] = "";
char t3[VW_MAX_PAYLOAD] = "";
char mb[VW_MAX_PAYLOAD] = "";
char hu[VW_MAX_PAYLOAD] = "";
char mV[VW_MAX_PAYLOAD] = "";
char sV[VW_MAX_PAYLOAD] = "";
char bV[VW_MAX_PAYLOAD] = "";
char message[VW_MAX_PAYLOAD] = "";

unsigned long previousMillis = 0;
unsigned long millisInterval = 180000; 
unsigned long previousPush = 0;
unsigned long pushInterval = 30000; 

void setup()
{
  delay(1000);
  //Serial.begin(9600);

  // Initialise the IO and ISR
  vw_set_rx_pin(receive_pin);
  // Bits per sec
  vw_setup(1200);
  // Start the receiver PLL running
  vw_rx_start();

  delay(250);
  Ethernet.begin(mac, ip);
  delay(250);
  server.begin();
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    // message löschen
    memset(message, '\0', sizeof(message));
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
    }
    String s_message = message;
    s_message.replace("  ", " ");
    s_message.replace("  ", " ");
    s_message.replace("  ", " ");
    s_message.toCharArray(message, VW_MAX_PAYLOAD);
    if (message[0] == 't')
    {
      if (message[1] == '1')
      {
        strcpy(t1, message);
      }
      else if (message[1] == '2')
      {
        strcpy(t2, message);
      }
      else if (message[1] == '3')
      {
        strcpy(t3, message);
      }
    }
    else if (message[0] == 'm')
    {
      if (message[1] == 'V')
      {
        strcpy(mV, message);
      }
      else if (message[1] == 'b')
      {
        strcpy(mb, message);
      }
    }
    else if (message[0] == 'h')
    {
      strcpy(hu, message);
    }
    else if (message[0] == 'B')
    {
      strcpy(bV, message);
    }
    else if (message[0] == 'S')
    {
      strcpy(sV, message);
    }
    
    // Daten seriel ausgeben
    //Serial.println(message);
    
    // Zeitpunkt für den Timer setzen
    previousMillis = millis();
  }
  // Länger als interval Millisec. kein Signal, dann die Variablen (Messages) leeren
  if((millis() - previousMillis) > millisInterval)
  {
    memset(t1, '\0', sizeof(t1));
    memset(t2, '\0', sizeof(t2));
    memset(t3, '\0', sizeof(t3));
    memset(mb, '\0', sizeof(mb));
    memset(hu, '\0', sizeof(hu));
    memset(mV, '\0', sizeof(mV));
    memset(bV, '\0', sizeof(bV));
    memset(sV, '\0', sizeof(sV));
  }
  // Ausgabe im Munin-Format erzeugen
  muninEthernet();

  if((millis() - previousPush) > pushInterval)
  {
    pushMessage();
    previousPush = millis();
  }
}

void muninEthernet() {
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      client.print(t1);
      client.print(F(";"));
      client.print(t2);
      client.print(F(";"));
      client.print(t3);
      client.print(F(";"));
      client.print(mb);
      client.print(F(";"));
      client.print(hu);
      client.print(F(";"));
      client.print(mV);
      client.print(F(";"));
      client.print(bV);
      client.print(F(";"));
      client.print(sV);
      client.print(F(";"));
      client.print(F("Last Message: "));
      client.print(message);
      client.print(F(" @ "));
      client.print(previousMillis);
      client.print(F(";"));
      client.println();
      delay(5);
      break;
    }
  }
  client.stop();
}

void pushMessage() {
  String s_t1 = t1;
  s_t1.replace(" ", "=");
  String s_t2 = t2;
  s_t2.replace(" ", "=");
  String s_t3 = t3;
  s_t3.replace(" ", "=");
  String s_mb = mb;
  s_mb.replace(" ", "=");
  String s_hu = hu;
  s_hu.replace(" ", "=");
  String s_mV = mV;
  s_mV.replace(" ", "=");
  String s_bV = bV;
  s_bV.replace(" ", "=");
  String s_sV = sV;
  s_sV.replace(" ", "=");
  EthernetClient client;
  if (client.connect("192.168.0.2", 80)) {
    client.println("GET /wetterinput/input.php?data="+s_t1+","+s_t2+","+s_t3+","+s_mb+","+s_hu+","+s_mV+","+s_bV+","+s_sV+" HTTP/1.1");
    client.println(F("HOST: wetter.nxxt.de"));
    client.println(F("Connection: close"));
    client.println();
    client.stop();
    delay(5);
  }
  else {
    //Serial.println(F("Verbindung fehlgeschlagen"));
  }
}
