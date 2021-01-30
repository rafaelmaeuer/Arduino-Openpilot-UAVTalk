#include "../lib/UAVTalk/UAVTalk.h"
#include "../lib/UAVTalk/UAVTalk.cpp"
#include <Adafruit_NeoPixel.h>

#define BAUDRATE 57600
#define UAV_FREQUENCY 100 // (HZ)
#define LED_UPDATE 100 // (HZ)

#define POWER_LED 2
#define FLIGHT_LED 3
#define STATUS_LED 4

#define PIXEL 1
#define LED_INTERVAL 250
#define LED_FREQUENZ 50  // 50HZ
#define LED_BRIGHTNESS 20 // 0 (min) - 255 (max)

Adafruit_NeoPixel ring = Adafruit_NeoPixel(PIXEL, STATUS_LED, NEO_GRB + NEO_KHZ800);

bool debug = true;

static uavtalk_message_t msg;

const uint32_t RED = ring.Color(255, 0, 0);
const uint32_t GREEN = ring.Color(0, 255, 0);
const uint32_t BLUE = ring.Color(0, 0, 255);
const uint32_t MAGENTA = ring.Color(255, 0, 255);
const uint32_t YELLOW = ring.Color(255, 255, 0);
const uint32_t DARK_YELLOW = ring.Color(155, 155, 0);
const uint32_t PINK = ring.Color(219,112,147);
const uint32_t SKY_BLUE = ring.Color(135,206,255);
const uint32_t MAIZE = ring.Color(128,158,10);
const uint32_t LAVENDER = ring.Color(88,2,163);
const uint32_t SEA_FOAM = ring.Color(32,178,170);
const uint32_t KELLY_GREEN = ring.Color(102,205,0);
const uint32_t DARK_ORANGE = ring.Color(237,180,6);
const uint32_t ORANGE = ring.Color(237,120,6);
const uint32_t WHITE = ring.Color(255,255,255);
const uint32_t BLACK = ring.Color(0,0,0);

long nextLEDTime = 0;
long nextObjTime = 0;

int ledState = LOW; // ledState used to set the LED
int ledMode = 0; // set LED mode (0:off, 1:blink, 2:on)
unsigned long previousMillis = 0; // will store last time LED was updated

void setup() {
  Serial.begin(BAUDRATE); // serial on USB
  Serial1.begin(BAUDRATE); // serial on 0(RX) and 1(TX)
  setupLEDs();
  initLEDs();
 }

void loop() {
  if (millis() > nextLEDTime) {
    setLedOutputs();
    nextLEDTime = millis() + LED_UPDATE; // LED udpate frequency;
  }

  if (millis() > nextObjTime) {
    // uavtalk_request_object(FLIGHTSTATUS_OBJID_005);
    // uavtalk_request_object(ATTITUDESTATE_OBJID);
    // uavtalk_request_object(MANUALCONTROLCOMMAND_OBJID);
    // uavtalk_request_object(MANUALCONTROLCOMMAND_OBJID_META);
    nextObjTime = millis() + UAV_FREQUENCY; // UAV talk frequency;
    if (debug) printLogs();
    updateCommand();
    // Serial.println(FLIGHTSTATUS_OBJID_005);
    // Serial.print(msg.ObjID);
    // Serial.print(": ");
    // for (int i=0; i<255; i++) {
    //   Serial.print(msg.Data[i]);
    // }
  }
}

void updateCommand() {
  if (osd_chan7_raw > 1000) {
    // msg.Data[MANUALCONTROLCOMMAND_OBJID_META] = 1;
    // msg.ObjID = MANUALCONTROLCOMMAND_OBJID_META;
    // msg.Data[MANUALCONTROLCOMMAND_OBJID_META_ACCESS] = 1;
    // uavtalk_send_manualcontrolcommand();
    // access_readwrite = false;
    // osd_throttle=10;
    // osd_yaw++;
  } else {
    // msg.ObjID = MANUALCONTROLCOMMAND_OBJID_META;
    // msg.Data[MANUALCONTROLCOMMAND_OBJID_META_ACCESS] = 0;
    // access_readwrite = true;
  }
  // uavtalk_send_msg(&msg);
  // uavtalk_request_object(MANUALCONTROLCOMMAND_OBJID_META);
  
}

void serialEvent1(){
  // grabbing data
  while (Serial1.available() > 0) {
    uint8_t c = Serial1.read();
    if (uavtalk_parse_char(c, &msg)) {
      uavtalk_read(&msg);
    }
  }
}

void printLogs() {
  Serial.println("");
  Serial.print("access_readonly "); Serial.println(access_readonly);

  Serial.print("osd_armed "); Serial.println(osd_armed);
  Serial.print("osd_mode "); Serial.println(osd_mode);
  // Serial.print("gcstelemetrystatus "); Serial.println(gcstelemetrystatus);
  // Serial.print("op_alarm "); Serial.println(op_alarm);
  Serial.print("stab_alarm "); Serial.println(stab_alarm);

  // Serial.print("osd_roll "); Serial.println(osd_roll);
  // Serial.print("osd_pitch "); Serial.println(osd_pitch);
  // Serial.print("osd_yaw "); Serial.println(osd_yaw);
  Serial.print("osd_throttle "); Serial.println(osd_throttle);

  // Serial.print("chan1_raw "); Serial.println(chan1_raw);
  // Serial.print("chan2_raw "); Serial.println(chan2_raw);
  Serial.print("osd_chan7_raw "); Serial.println(osd_chan7_raw);
}

void setupLEDs() {
  ring.begin();
  ring.show(); // Initialize all pixels to 'off'
  ring.setBrightness(LED_BRIGHTNESS);

  pinMode(POWER_LED, OUTPUT);
  pinMode(FLIGHT_LED, OUTPUT);
}

void initLEDs() {
  LEDon(POWER_LED);
  LEDon(FLIGHT_LED);
  setAllLeds(WHITE);
  delay(LED_INTERVAL * 2);
  LEDoff(POWER_LED);
  LEDoff(FLIGHT_LED);
  setAllLeds(BLACK);
  delay(LED_INTERVAL);
}

void LEDon(int LED) {
  digitalWrite(LED, HIGH);
}

void LEDoff(int LED) {
  digitalWrite(LED, LOW);
}

void LEDblink(int LED, int interval) {
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // save the last time you blinked the LED

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
      LEDon(LED);
    } else {
      ledState = LOW;
      LEDoff(LED);
    }
  }
}

void setAllLeds(unsigned long new_color) {
  for (int i=0; i < PIXEL; i++){
    ring.setPixelColor(i, new_color);
  }
  ring.show();
}

void setLedOutputs() {

  // stabilization
  if (stab_alarm == 3 && osd_armed == 0) {
    LEDblink(POWER_LED, LED_INTERVAL);
  } else {
    LEDon(POWER_LED);
  }

  // armed
  switch (osd_armed) {
    case 2:
      LEDon(FLIGHT_LED);
      break;
    case 1:
      LEDblink(FLIGHT_LED, LED_INTERVAL * 0.5);
      break;
    default:
      LEDoff(FLIGHT_LED);
  }

  //  if (osd_armed == 1) { // seems, that messages are sent only once on state-change "event"
  //                        // and that these onetime-messages get lost sometimes (same behavior seen at flight mode "osd_mode")
  //                        // Flight Telemetry Update Period can be set in GCS (Default for armend: 5000ms)
  //                        // UPDATE: Changed to 500ms. But this led is not working reliable... grrr
  //    ring.setPixelColor(0, DARK_YELLOW); // arming = DARK_YELLOW
  //  } else if (osd_armed == 2) {
  //    ring.setPixelColor(0, RED); // armed = RED
  //  } else {
  //    ring.setPixelColor(0, BLACK);
  //  }

   // yaw
  //  uint32_t value = (osd_yaw + 180) * 25 / 36; // 0 - 255
  //  if (value > 255) {
  //    value = 255;
  //  }
  //  uint32_t color =  value;
  //  ring.setPixelColor(1, color);
   
   // throttle (0-100)
  //  value = osd_throttle;   // works well, if Flight Telemetry Update Period is set to 300ms in GCS
  //  value = value * 2.55;
  //  color = value << 16;    // red
  //  ring.setPixelColor(2, color);

   // flight mode
   if (osd_mode == 1) {
     setAllLeds(GREEN); // green
   } else if (osd_mode == 2) {
     setAllLeds(YELLOW); // yellow
   } else if (osd_mode == 3) {
     setAllLeds(RED); // red
   } else {
     setAllLeds(LAVENDER); // 
   }
}
