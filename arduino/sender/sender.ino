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

// D11 und D10
// Solarzelle laden
const int relaisPin11 = 11;
// Luefter
const int relaisPin10 = 10;
// A0 und A2
const int batPin = 0;
const int solPin = 2;

float solvcc = 0;
float batvcc = 0;

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
  pinMode(relaisPin11, OUTPUT);
  pinMode(relaisPin10, OUTPUT);
  randomSeed(analogRead(3));

  delay(100);
}

void loop() {
  messenSenden();
  for (int i = 0; i < 8; i++) {
    lpDelay();
    checkAkku();
  }
}

void messenSenden() {
  // Akku vom Solarpanel trennen
  digitalWrite(relaisPin11, HIGH);
  delay(50);
  // Solarspannung messen und Luefter anschalten bei ueber 20 V (Sonnenschein)
  solvcc = analogRead(solPin) * 0.0276;
  // Akku wieder anschliessen
  digitalWrite(relaisPin11, LOW);
  if (solvcc > 20) {
    digitalWrite(relaisPin10, HIGH);
    // Serial.println("Sonne!");
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
  }

  // Sensoren einlesen
  temp_bmp = pressure.readTemperature();
  pres_bmp = pressure.readPressure();
  baro_bmp = pressure.readSealevelPressure(535) * 0.01;
  sensors.setResolution(TEMP_12_BIT);
  sensors.requestTemperatures();
  temp_1wi = sensors.getTempCByIndex(0);
  temp_dht = dht.readTemperature();
  humi_dht = dht.readHumidity();

  // Luefter aus
  digitalWrite(relaisPin10, LOW);

  // Messung Batteriespannung mit angeschlossener Solarzelle starten
  digitalWrite(relaisPin11, LOW);
  delay(50);
  batvcc = analogRead(batPin) * 0.0276;

  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  if (batvcc > 14.2) {
    digitalWrite(relaisPin11, HIGH);
    // Serial.println("Voll");
  } else {
    digitalWrite(relaisPin11, LOW);
    // Serial.println("Laden");
  }

  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);

  // Solarzellenleerlaufspannung senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "SVcc ");
  dtostrf(solvcc, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);

  // Batteriespannung mit angeschlossener Solarzelle senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "BVcc ");
  dtostrf(batvcc, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);

  // Versorgungsspannung senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "mVcc ");
  dtostrf(readVcc(), 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);

  // BMP Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t1_C ");
  dtostrf(temp_bmp, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);

  // BMP Druck senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "mbar ");
  dtostrf(baro_bmp, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);

  // DHT Feuchtigkeitswert senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "hum% ");
  dtostrf(humi_dht, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);

  // DHT Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t2_C ");
  dtostrf(temp_dht, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);

  // OneWire Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t3_C ");
  dtostrf(temp_1wi, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  // Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  // Sendeintervall leicht versetzen, um gleichzeitige Kanalbelegung zu verhindern
  delay(random(100, 1900));
}

void lpDelay() {
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
}

void checkAkku() {
  // Check, ob der Akku voll geladen ist (Ladeschlussspannung bei angeschlossenem Panel)
  digitalWrite(relaisPin11, LOW);
  delay(50);
  batvcc = analogRead(batPin) * 0.0276;
  if (batvcc > 14.2) {
    digitalWrite(relaisPin11, HIGH);
    // Serial.println("Der Akku ist voll!");
  } else {
    digitalWrite(relaisPin11, LOW);
    // Serial.println("Laden...");
  }
}

// Versorgungspannung messen mit interner Referenz
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}


