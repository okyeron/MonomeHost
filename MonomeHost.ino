/*
MONOME HOST modified for Teensy 3.6 with USB Host
*/
#include <USBHost_t36.h>
#include "MonomeController.h"

#define USBBAUD 115200
uint32_t baud = USBBAUD;
uint32_t format = USBHOST_SERIAL_8N1;

USBHost usb;
USBHub hub1(usb);
USBSerial userial1(usb);
USBSerial userial2(usb);
MonomeController monome(usb);

// Standard USBDriver devices
USBDriver *drivers[] = {&hub1, &userial1};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"HUB1", "USERIAL1", "USERIAL2"};
bool driver_active[CNT_DEVICES] = {false, false, false};


uint8_t readInt(USBSerial &thisSerial) {
  uint8_t val = thisSerial.read();
  return val; 
}
void writeInt(uint8_t value, USBSerial &thisSerial) {
   thisSerial.write(value);          
}

void GridKeyCallback(byte x, byte y, byte z) { 
  Serial.print("grid key: ");
  Serial.print(x);
  Serial.print(" , ");
  Serial.print(y);
  Serial.print(" , ");
  Serial.print(z);
  Serial.print("\r\n");
  
  monome.led_clear();
  monome.led_set(x, y, 15);
  monome.refresh();
}

void ConnectCallback(const char * name, byte cols, byte rows) {
  Serial.print("\r\nmonome device connected; type: ");
  Serial.print(name);
  Serial.print(" ; columns: ");
  Serial.print(cols);
  Serial.print(" ; rows: ");
  Serial.print(rows);
  Serial.print("\r\n");
}


void setup() { 

  // set connection callback
  monome.SetConnectCallback(&ConnectCallback);
  // set key event callback
  monome.SetGridKeyCallback(&GridKeyCallback);

  Serial.begin(115200);

  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  Serial.print("\n\nStarting monome host...\n");
  delay(10);
  usb.begin();
  
  userial1.begin(baud, format);

}

void loop() { 
  usb.Task();
  /// alternatively, refresh on every tick:
  //  monome.refresh();
}
