/* LED Football 
 *  20 x Hexagon a 13 LEDs
 *  12 x Pentagon a 11 LEDs
 * ----
 *  32 x PCBs
 *                392 LEDs
 *                
 *  LED Type APA102C
 *  
 *  (C) 2019 Thomas Buck
 *      Thomas.Buck@101010b.de
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <vector>

#include "ledctrl.h"
#include "effect.h"
#include "sparcle.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "XYZ"
#define WLAN_PASS       "1234567890"

#define MQTT_BROKER     "10.1.2.3"
#define MQTT_PORT       1883

#define MQTT_TOPIC_ROOT "lightball/%s"

/************************** Global State *************************************/
enum IOSTATE { 
  OFFLINE=0,
  WIFICONNECTED,
  MQTTCONNECTED
};
uint8_t iostate;

/************************** Functions ***************************************/

short charToHex(char c) {
  if ((c >= '0') && (c <= '9')) return c-'0';
  if ((c >= 'A') && (c <= 'F')) return c-'A' + 10;
  if ((c >= 'a') && (c <= 'f')) return c-'a' + 10;
  return 0;
}

int charToInt(char c) {
  if ((c >= '0') && (c <= '9')) return c-'0';
  return -1;
}

unsigned short decodeHex(String s) {
  unsigned short e=0x0000;
  int i;
  unsigned short val=1;
//  Serial.println(String("decodeHex(") + s + ");");
  for (i=s.length()-1;i>=0;i--) {
    e+=charToHex(s[i])*val;
    val*=16;
  }
//  Serial.println(String("return ") + String(e) + ";");
  return e;
}

int decodeDec(String s) {
  int val = 0;
  int sign = 1;
  int i=0;
  if (s[i] == '-') {
    sign=-1;
    i++;
  } else if (s[i] == '+') {
    i++;
  } 
  for (;i<s.length();i++) {
    int e=charToInt(s[i]);
    if ((e >= 0) && (e <= 9)) {
      val*=10;
      val+= e;
    }
  }
  return sign*val;
}

char hexchar(uint8_t c) {
  static const char hexc[] = "0123456789ABCDEF";
  if (c < 0) return '0';
  if (c > 15) return 'F';
  return hexc[c];
}

char *addHex(char *s, uint8_t t) {
  if (!s) return 0;
  *s = hexchar(t>>4); s++;
  *s = hexchar(t & 0x0F); s++;
  *s = 0;
  return s; 
}

char *addStr(char *s, char *u) {
  while (*u) {
    *s=*u;
    s++;u++;
  }
  *s=0;
  return s;
}

unsigned char isUpper(char c) {
  if ((c >= 'A') && (c <= 'Z')) return 1;
  return 0;
}

WiFiClient client;
PubSubClient psclient(client);
char mqtt_topic_root[64];

/*************************** Sketch Code ************************************/

ledctrl *led;
bool ledon;
char pl[64];
char topicpath[128];
typedef char *pchar;
int topiclistlength;
pchar topiclist[8];
bool publishall;
bool noreceive;

void mqtt_callback(const char* topic, unsigned char* payload, unsigned int length) {
  if (noreceive) return;
  // Payload
  if (length > 63) return; // Ignore
  for (int i=0;i<length;i++)
    pl[i] = payload[i];
  pl[length]=0;
  
  Serial.println("MQTT: " + String(topic) + " = [" + String(pl) + "]");
  // Topic
  // Example lightball/LB_00_11_22_33_44_55/bla/blub
  //         --- ROOT ---------------------
  if (strncmp(topic,mqtt_topic_root,strlen(mqtt_topic_root))) return; // No match, even the first part
  topiclistlength=0;strcpy(topicpath,"");
  if (strlen(topic) == strlen(mqtt_topic_root)) { // empty
  } else {
    // Is longer --> check for '/'
    if (topic[strlen(mqtt_topic_root)] != '/') return; // No --> error;
    if (strlen(topic+strlen(mqtt_topic_root)+1) > 127) return; // Ignore as too long
    strcpy(topicpath, topic+strlen(mqtt_topic_root)+1);
  } 
  // split topicpath 
  if (strlen(topicpath)) {
    // Something to split there
    char *p = topicpath;
    while (*p) {
      char *q = p;
      if (topiclistlength >= 8) return; // too many levels
      topiclist[topiclistlength]=p;
      topiclistlength++;
      while (*q && (*q != '/')) q++;
      if (*q) {
        // There is more
        *q = 0;
        p = q+1;
      } else {
        // End
        p=q;
      }
    }
  }
  
  // Process data
  if (topiclistlength == 0) {
    // On/off
    if ((strcasecmp(pl,"OFF")==0) || (strcmp(pl,"0") == 0)) {
      ledon = 0;
    } else if ((strcasecmp(pl,"ON")==0) || (strcmp(pl,"1") == 0)) {
      ledon = 1;
    } // else ignore
  } else {
    if (strcasecmp(topiclist[0],"state") == 0) {
      if ((strcasecmp(pl,"OFF")==0) || (strcmp(pl,"0") == 0)) {
        ledon = 0;
      } else if ((strcasecmp(pl,"ON")==0) || (strcmp(pl,"1") == 0)) {
        ledon = 1;
      } // else ignore
    } else if (strcasecmp(topiclist[0],"global") == 0) {
      char *ep;
      int g = strtol(pl, &ep, 0);
      if (ep && *ep) return; // Error
      if ((g < 0) || (g > 31)) return; // Error
      if (g == 0) {
        ledon = 0;
        led->GLOBAL = 1;
      } else {
        ledon = 1;
        led->GLOBAL = g;
      }
    } else if (strcasecmp(topiclist[0],"effect") == 0) {
      led->selecteffect(pl);
    } else if (strcasecmp(topiclist[0],"publish") == 0) {
      // Publish all data via MQTT
      publishall = 1;
    } else {
      parametric *sel = led->findparametric(topiclist[0]);
      if (!sel) return; // Not found
      if (topiclistlength != 2) return; // not useful
      param_t *p = sel->findparam(topiclist[1]);
      if (!p) return; // Not found
      if (sel->setparam(p, pl))
        sel->updateparam(p);
    }
  }
}

void effectLoop() {
  if (ledon)
    led->tick();
  else 
    led->allOff();
  led->progLeds();
  delay(10);
}

char switchname[64];

void mqttInitSub() {
  
  uint8_t mac[6];
  WiFi.macAddress(mac);
  switchname[0]=0;
  char *s = &switchname[0];
  s=addStr(&switchname[0],"LB");
  for(int i=0;i<6;i++) {
    s=addStr(s,"_");
    s=addHex(s,mac[i]);
  }
  
  sprintf(mqtt_topic_root,MQTT_TOPIC_ROOT,switchname);

  Serial.println("TopicRoot: " + String(mqtt_topic_root));
}


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing MQTT LightBall...");

  Serial.println("Starting Wifi connection...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  WiFi.softAPdisconnect(false); 
  WiFi.enableAP(false);

  mqttInitSub();

  psclient.setServer(MQTT_BROKER, MQTT_PORT);
  psclient.setCallback(&mqtt_callback);

  led = new ledctrl();
  ledon = false;

  publishall = false;
  noreceive = false;
  iostate = OFFLINE;
}

void publishAllSettings() {
  char pl[32];
  char topic[64];
  
  noreceive=true;
  
  if (ledon) 
    sprintf(pl,"on");
  else
    sprintf(pl,"off");
  sprintf(topic,"%s/state",mqtt_topic_root);
  psclient.publish(topic,pl);

  sprintf(pl,"%i",led->GLOBAL);
  sprintf(topic,"%s/global",mqtt_topic_root);
  psclient.publish(topic,pl);

  effect *sel = led->getselectedeffect();
  if (sel)
    sprintf(pl,"%s", sel->name);
  else
    sprintf(pl,"NONE");
  sprintf(topic,"%s/effect",mqtt_topic_root);
  psclient.publish(topic,pl);

  for (std::list<parametric*>::iterator it = led->globals.begin(); it != led->globals.end(); ++it) {
    param_t *p = (*it)->params;
    while (p) {
      if ((*it)->formatparam(p,pl,32)) {
        sprintf(topic,"%s/%s/%s",mqtt_topic_root,(*it)->name,p->name);
        psclient.publish(topic,pl);        
      }
      p=p->next;
    }
  }
  for (std::list<effect*>::iterator it = led->list.begin(); it != led->list.end(); ++it) {
    param_t *p = (*it)->params;
    while (p) {
      if ((*it)->formatparam(p,pl,32)) {
        sprintf(topic,"%s/%s/%s",mqtt_topic_root,(*it)->name,p->name);
        psclient.publish(topic,pl);        
      }
      p=p->next;
    }
  }
  delay(100);
  noreceive=false;
}

void loop() {
  // Main Loop
  int failcounter = 300;

  if (WiFi.status() != WL_CONNECTED) {
    iostate = OFFLINE;
    Serial.println("Trying to connect to WiFi AP...");
    effectLoop();
    return;
  }

  // Wifi is connected
  if (iostate == OFFLINE) {
    // Must initialize WIFI
    Serial.println("WiFi Connected successfully.");
    Serial.println("Trying to connect to MQTT Broker...");
    iostate = WIFICONNECTED;
  }
  
  psclient.loop();

  if (!psclient.connected()) {
    psclient.connect(switchname);
    effectLoop();
    return;    
  }

  if (iostate == WIFICONNECTED) {
    // Must initialize psclient connection
    Serial.println("Connected to MQTT Broker.");
    char maintpc[64];
    sprintf(maintpc,"%s/#",mqtt_topic_root);
    psclient.subscribe(maintpc);
    iostate = MQTTCONNECTED;
  }
  
  effectLoop();

  if (publishall) {
    publishAllSettings();
    publishall=false;
  }
}

/*
void verifyFingerprint() {
  if (! client.verify(FINGERPRINT, CERT_NAME)) {
    Serial.println("ERROR: Incorrect certificate signature!");
    // Connection insecure!
    blinkError();
    shutdown();
  }
}
*/
/*
void blinkSuccess() {
  for (int i = 4; i < 50; i=(5*i) >> 2) {
    digitalWrite(LED, HIGH);   // turn the LED off
    delay(10*i);               // wait
    digitalWrite(LED, LOW);    // turn the LED on
    delay(10*i);               // wait
  }
}

void blinkError() {
  for (int i = 0; i < 28; i++) {
    digitalWrite(LED, HIGH);   // turn the LED off
    delay(125);                        // wait
    digitalWrite(LED, LOW);    // turn the LED on
    delay(125);                        // wait
  }
}

void blinkSent() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED, LOW);   // turn the LED on
    delay(200);                        // wait
    digitalWrite(LED, HIGH);    // turn the LED off
    delay(200);                        // wait
  }
}
*/
void shutdown() {
  Serial.println("Shutting down.");
  Serial.println("Going to sleep.");
  ESP.deepSleep(0);
  Serial.println("Sleep failed.");
  while(1) {
    // blinkError();
  }
}
