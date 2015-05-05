#include <Wire.h>
#include "VirtualWire.h"
#include "LowPower.h"
#include "Adafruit_BMP085.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "DHT.h"

Adafruit_BMP085 pressure;

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float temp_bmp;
float pres_bmp;
float baro_bmp;
float temp_1wi;
float temp_dht;
float humi_dht;

const int transmit_pin = 12;

char msg_num[8];
char msg[8];
char msgStr[8];

// D11
const int relaisPin = 11;
// A0 und A2
const int batPin = 0;
const int solPin = 2;

void setup()
{
  //Serial.begin(9600);

  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(1200);  // Bits per sec

  pressure.begin();  
  sensors.begin();
  dht.begin();

  // Relais-Output-Pin setzen
  pinMode(relaisPin, OUTPUT);
  randomSeed(analogRead(3));
}

void loop()
{
  delay(100);

  // Sensoren einlesen
  temp_bmp = pressure.readTemperature();
  pres_bmp = pressure.readPressure();
  baro_bmp = pressure.readSealevelPressure(535) * 0.01;
  sensors.setResolution(TEMP_12_BIT);
  sensors.requestTemperatures();
  temp_1wi = sensors.getTempCByIndex(0);
  temp_dht = dht.readTemperature();
  humi_dht = dht.readHumidity();
 
  // Leerlaufspannung der Solarzelle mit nicht angeschlossenem Akku messen
  digitalWrite(relaisPin, HIGH);
  delay(250);
  float solvcc = analogRead(solPin)*0.0276;

  // Messung Batteriespannung mit angeschlossener Solarzelle starten
  digitalWrite(relaisPin, LOW);
  delay(250);
  float batvcc = analogRead(batPin)*0.0276;
  
  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  if (batvcc > 14.0) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  } else {
    digitalWrite(relaisPin, LOW);
    // Serial.println("Laden");
  }
  
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Solarzellenleerlaufspannung senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "SVcc ");
  dtostrf(solvcc, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  batvcc = analogRead(batPin)*0.0276;
  if (batvcc > 14.0) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  }
  
  // Batteriespannung mit angeschlossener Solarzelle senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "BVcc ");
  dtostrf(batvcc, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  batvcc = analogRead(batPin)*0.0276;
  if (batvcc > 14.0) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  }

  // Versorgungsspannung senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "mVcc ");
  dtostrf(readVcc(), 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  batvcc = analogRead(batPin)*0.0276;
  if (batvcc > 14.0) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  }

  // BMP Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t1_C ");
  dtostrf(temp_bmp, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  batvcc = analogRead(batPin)*0.0276;
  if (batvcc > 14.0) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  }

  // BMP Druck senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "mbar ");
  dtostrf(baro_bmp, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  batvcc = analogRead(batPin)*0.0276;
  if (batvcc > 14.0) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  }

  // DHT Feuchtigkeitswert senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "hum% ");
  dtostrf(humi_dht, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  batvcc = analogRead(batPin)*0.0276;
  if (batvcc > 14.0) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  }

  // DHT Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t2_C ");
  dtostrf(temp_dht, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  batvcc = analogRead(batPin)*0.0276;
  if (batvcc > 14.0) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  }

  // OneWire Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t3_C ");
  dtostrf(temp_1wi, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  // Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  // Sendeintervall leicht versetzen, um gleichzeitige Kanalbelegung zu verhindern
  delay(random(100, 1900));
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

}

// Versorgungspannung messen mit interner Referenz
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}


