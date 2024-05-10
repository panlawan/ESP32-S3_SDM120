#define DEBUG

#include <ModbusMaster.h>
float x, V, Q, S, kWh, F = 0.0, I, P, PF, kVArh; //S=VA, Q=VAr

#define MAX485_DE 16 // DE RE ต่อขาเดียวกันเลย แล้วต่อเข้าขา D5 ของ ESP8266
/*-----------------------------------------------------------------------*/
#define RXD1  (18)
#define TXD1  (17)

#define reg_V 0x0000
#define reg_I 0x0006
#define reg_P 0x000C
#define reg_F 0x0046
#define reg_kWh 0x0156
#define reg_PF 0x001E

ModbusMaster node;

void preTransmission() {
  digitalWrite(MAX485_DE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_DE, 0);
}

float reform_uint16_2_float32(uint16_t u1, uint16_t u2)
{
  uint32_t num = ((uint32_t)u1 & 0xFFFF) << 16 | ((uint32_t)u2 & 0xFFFF);
  float numf;
  memcpy(&numf, &num, 4);
  return numf;
}

float getRTU(uint16_t m_startAddress) {
  uint8_t m_length = 2;
  uint16_t result;
  float x;
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  result = node.readInputRegisters(m_startAddress, m_length);  //readInputRegisters readHoldingRegisters

  if (result == node.ku8MBSuccess) {
    return reform_uint16_2_float32(node.getResponseBuffer(0), node.getResponseBuffer(1));
  }
}

void readData() {

  V = getRTU(reg_V);
  delay(10);
  I = getRTU(reg_I);
  delay(10);
  PF = getRTU(reg_PF);
  delay(500);
  P = getRTU(reg_P);
  delay(200);
  kWh = getRTU(reg_kWh);
  delay(200);
}

void setup() {
  pinMode(MAX485_DE, OUTPUT);
  node.preTransmission(postTransmission);

  delay(2000);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  node.begin(2, Serial1);
  delay(2000);
  Serial.begin(9600);
  digitalWrite(MAX485_DE, 0);
  node.preTransmission(preTransmission);
  Serial.println("Leave setup!");
}

void loop() {
  readData();

#ifdef DEBUG
  Serial.print("V: ");     Serial.print(V);     Serial.print("\t");
  Serial.print("I: ");     Serial.print(I);     Serial.print("\t");
  Serial.print("Energy: "); Serial.println(kWh);
  Serial.print("P: ");     Serial.print(P);     Serial.print("\t");
  Serial.print("PF: ");    Serial.print(PF, 6);    Serial.print("\n\n");
#endif

  delay(1000);
}
