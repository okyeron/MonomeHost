/*
MONOME HOST modified for Teensy 3.6 with USB Host
*/
#include <USBHost_t36.h>
#include "MonomeController.h"

#define USBBAUD 115200
uint32_t baud = USBBAUD;
uint32_t format = USBHOST_SERIAL_8N1;

USBHost usb;
USBHub hub1(usb); 			// sets up hub compatibility
USBSerial userial1(usb);	// 2 different serial connections (if using hub)
USBSerial userial2(usb);
MonomeController monome(usb);

// Standard USBDriver devices
USBDriver *drivers[] = {&hub1, &userial1, &userial2};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"HUB1", "USERIAL1", "USERIAL2"};
bool driver_active[CNT_DEVICES] = {false, false, false};


// BASIC READ/WRITE FUNCTIONS FOR SERIAL I/O
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
  
}

void loop() { 
  usb.Task();
  /// alternatively, refresh on every tick:
  //  monome.refresh();
}

// EXAMPLE FUNCTION TO PRINT DEVICE INFO 
void deviceInfo(){
  int serialnum;
  char maker[6];

  for (uint8_t i = 0; i < CNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        Serial.printf("*** %s Device - disconnected ***\n", driver_names[i]);
        driver_active[i] = false;
      } else {
        Serial.printf("*** %s Device %x:%x - connected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        sprintf(maker, "%s", psz);
        
        const char *psp = (const char*)drivers[i]->product();
        if (psp && *psp) Serial.printf("  product: %s\n", psp);

        const char *pss = (const char*)drivers[i]->serialNumber();
        if (pss && *pss) Serial.printf("  Serial: %s\n", pss);

        // check for monome types
        if  (String(maker) == "monome") {
          //"m128%*1[-_]%d" = series, "mk%d" = kit, "m40h%d" = 40h, "m%d" = mext
          if (sscanf(pss, "m40h%d", &serialnum)){  
            Serial.println("  40h device");
          } else if (sscanf(pss, "m128%*1[-_]%d", &serialnum)){ 
            Serial.println("  monome series 128 device");
          } else if (sscanf(pss, "mk%d", &serialnum)){ 
            Serial.println("   monome kit device");
          } else if (sscanf(pss, "m%d", &serialnum)){ 
            Serial.println("  mext device");
          }
        }
 
        // If this is a new Serial device. //????
        if (drivers[i] == &userial1) {
          // Lets try first outputting something to our USerial to see if it will go out...
          userial1.begin(baud, format);
        }
      }
    }
  }
}