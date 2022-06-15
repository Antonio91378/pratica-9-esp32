#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "logo.h"
#include "wifi_icon.h"
#include "EspMQTTClient.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED 2

bool _connected = false;
bool *_Pconnected = &_connected; 

int potenciometro = 0;
bool led = false;

EspMQTTClient client(
  "TP-Link_C3D1",
  "60313683",
  "192.168.0.102",  // MQTT Broker server ip
  "",   // Can be omitted if not needed
  "",   // Can be omitted if not needed
  "qualquerNome123",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void setup() {
  Serial.begin(115200);
  client.enableDebuggingMessages();
  pinMode(LED, OUTPUT);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.cp437(true); // usar codepage 437 correto!
  
  showIntro();

  display.clearDisplay();
  display.display();

  rotinaConexao();
}

void onConnectionEstablished() { 
  *_Pconnected = true;
  Serial.println("Conectado :)");
  client.subscribe("bancada1/potenciometro", [](const String & payload) {
    potenciometro = payload.toInt();
  });
  client.subscribe("bancada10/botao", [](const String & payload) {
    led = !led;
    digitalWrite(LED,led);
  });
}

void showIntro() {
  String title = "Lab. Micro";
  display.clearDisplay();
  display.drawXBitmap(
    64 - logo_width / 2, 
    40 - logo_height / 2, 
    logo_bits, logo_width, logo_height, SSD1306_WHITE);
  display.display();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  for(int j = 0; j < title.length(); j++) {
    display.setCursor(4 + 12*j, 0);
    display.print(title[j]);
    delay(200);
    display.display();
  }
  delay(1000);
  for(int i = 1; i < 5; i++) {
    display.invertDisplay(i % 2);
    delay(300);
  }
}

void rotinaConexao() {
  display.clearDisplay();
  display.drawXBitmap(
    64 - wifi_icon_width / 2, 
    40 - wifi_icon_height / 2, 
    wifi_icon_bits, wifi_icon_width, wifi_icon_height, SSD1306_WHITE);
  display.display();
  while(*_Pconnected == false){
    display.invertDisplay(true);  
    delay(500);
    display.invertDisplay(false);
    delay(500);
    client.loop();
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  client.loop(); 
  display.clearDisplay();
  display.setCursor(16, 24);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print(potenciometro);
  display.display();
  
}
