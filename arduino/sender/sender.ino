#include "dht.h"
#include "VirtualWire.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "LowPower.h"
#include <Wire.h>

dht DHT;

#define BMP085_ADDRESS 0x77  // I2C address of BMP085
#define DHT11_PIN 2

#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
int ac1;
int ac2; 
int ac3; 
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1; 
int b2;
int mb;
int mc;
int md;

// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5; 

float temperature;
float pressure;
const float altitudecorrection = 61; // Correction for 530 m in Grabenstaett

//float voltage;

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
  Wire.begin();
  bmp085Calibration();

  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(4800);  // Bits per sec

  // Initialisieren der Dallas Temperature library
  sensors.begin();

  // Relais-Output-Pin setzen
  pinMode(relaisPin, OUTPUT);
}

void loop()
{
  // BMP Daten einlesen
  temperature = bmp085GetTemperature(bmp085ReadUT());
  temperature = temperature / 10;

  pressure = bmp085GetPressure(bmp085ReadUP());
  pressure = pressure / 100 + altitudecorrection;

  // DHT Daten einlesen
  int chk = DHT.read11(DHT11_PIN);
  
  // OneWire Temperatur abfragen
  sensors.requestTemperatures();

  // Leerlaufspannung der Solarzelle mit nicht angeschlossenem Akku messen
  digitalWrite(relaisPin, HIGH);
  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);
  delay(80);
  float solvcc = analogRead(solPin)*0.0276;

  // Messung Batteriespannung mit angeschlossener Solarzelle starten
  digitalWrite(relaisPin, LOW);
  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON);
  delay(80);
  float batvcc = analogRead(batPin)*0.0276;
  
  // Check, ob der Akku voll geladen ist (Ladeschlussspannung)
  if (batvcc > 14.3) {
    digitalWrite(relaisPin, HIGH);
    // Serial.println("Voll");
  } else {
    digitalWrite(relaisPin, LOW);
    // Serial.println("Laden");
  }

  // Solarzellenleerlaufspannung senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "SVcc ");
  dtostrf(solvcc, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Batteriespannung mit angeschlossener Solarzelle senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "BVcc ");
  dtostrf(batvcc, 4, 1, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // Versorgungsspannung senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "mVcc ");
  dtostrf(readVcc(), 4, 0, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // BMP Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t1_C ");
  dtostrf(temperature, 4, 0, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // BMP Druck senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "mbar ");
  dtostrf(pressure, 4, 0, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // DHT Feuchtigkeitswert senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "hum% ");
  dtostrf(DHT.humidity, 4, 0, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // DHT Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t2_C ");
  dtostrf(DHT.temperature, 4, 0, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);
  delay(100);

  // OneWire Temperatur senden
  memset(msgStr, '\0', sizeof(msgStr));
  memset(msg, '\0', sizeof(msg));
  strcpy(msgStr, "t3_C ");
  dtostrf(sensors.getTempCByIndex(0), 4, 0, msg_num);
  sprintf(msg, "%s %s", msgStr, msg_num);
  //Serial.println(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();

  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_ON);
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


// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void bmp085Calibration()
{
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
short bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;

  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);  
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;

  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  return p;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;

  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();

  return (int) msb<<8 | lsb;
}

// Read the uncompensated temperature value
unsigned int bmp085ReadUT()
{
  unsigned int ut;

  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  // Wait at least 4.5ms
  delay(10);

  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(4 + (3<<OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);

  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();

  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);

  return up;
}


