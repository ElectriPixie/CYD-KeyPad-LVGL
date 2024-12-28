#include "Arduino.h"
#include "WifiPasswd.h"

#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <esp32_smartdisplay.h>

//#define TFT_BL 21
//#define TFT_CS 15
//#define TFT_DC 2
//#define TFT_MISO 12
//#define TFT_MOSI 13
//#define TFT_SCLK 14
//#define TFT_RST -1

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

#define TS_MINX 300
#define TS_MINY 300
#define TS_MAXX 3800
#define TS_MAXY 3800

SPIClass vspi = SPIClass(VSPI);

XPT2046_Touchscreen ts(XPT2046_CS,XPT2046_IRQ);
TFT_eSPI tft = TFT_eSPI();

struct Button {
    int x, y;       
    int w, h; 
    int number;
    int color;
};

struct KeyPad {
  int x, y;
  int w, h;
  int offset;
  struct Button Buttons[9];
};

struct KeyPad KeyPad;

void drawButton(struct KeyPad* KeyPad, int i, int color)
{
  if(color == 0)
  {
    color = KeyPad->Buttons[i].color;
  }
  tft.fillRect(KeyPad->Buttons[i].x, KeyPad->Buttons[i].y, KeyPad->Buttons[i].w, KeyPad->Buttons[i].h, color);
}

void drawKeyPad(struct KeyPad* KeyPad)
{
  for(int i = 0; i < 9; i++)
  {
    drawButton(KeyPad, i, 0);
  }
}

void addButton(struct KeyPad* KeyPad, int i, int x, int y, int w, int h, int color)
{
  KeyPad->Buttons[i].x = x;
  KeyPad->Buttons[i].y = y;
  KeyPad->Buttons[i].w = w;
  KeyPad->Buttons[i].h = h;
  KeyPad->Buttons[i].number = i+1;
  KeyPad->Buttons[i].color = color;
}

void initKeyPad(struct KeyPad* KeyPad, int offset, int w, int h, int color)
{
  int x = 0;
  int y = 0;
  int xw;
  int yh;
  xw = (w - offset*2 )/ 3; 
  yh = (h - offset*2 )/ 3; 
  KeyPad->w = w;
  KeyPad->h = h;
  KeyPad->offset = offset;

  for(int i = 0; i < 3; i++)
  {
    addButton(KeyPad, i, x, y, xw, yh, color);
    x = x+xw+KeyPad->offset;
  }
  x = 0;
  y = y+yh+KeyPad->offset;
  for(int i = 3; i < 6; i++)
  {
    addButton(KeyPad, i, x, y, xw, yh, color);
    x = x+xw+KeyPad->offset;
  }
  x = 0;  
  y = y+yh+KeyPad->offset;
  for(int i = 6; i < 9; i++)
  {
    addButton(KeyPad, i, x, y, xw, yh, color);
    x = x+xw+KeyPad->offset;
  }
}

int checkKeyPad(struct KeyPad* KeyPad, int x, int y)
{
  for(int i = 0; i < 9; i++)
  {
    if(x > KeyPad->Buttons[i].x && x < KeyPad->Buttons[i].x + KeyPad->Buttons[i].w)
    {
      if(y > KeyPad->Buttons[i].y && y < KeyPad->Buttons[i].y + KeyPad->Buttons[i].h)
      {
        return i;
      }
    }
  }
  return -1;
}

void initWifi(const char* ssid, const char* password, int channel, int ssid_hidden, int max_connections)
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, channel, ssid_hidden, max_connections);
}

void setup()
{
  Serial.begin(115200);
  smart_display_init();
  auto display = lv_display_get_default();
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Setup the VSPI to use custom pins for the touch
  vspi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(vspi);

  ts.setRotation(1);
  while (!Serial && (millis() <= 1000))
    ;
  initKeyPad(&KeyPad, 5, 320, 240, TFT_GREEN);
  drawKeyPad(&KeyPad);
  //initWifi(ssid, password, 11, 0, 4);
}

bool wastouched = true;
int x, y;
int key;
bool key_pressed;
auto lv_last_tick = millis();

void loop()
{
  boolean istouched = ts.touched();
  if (istouched)
  {
    TS_Point p = ts.getPoint();
    x = map(p.x, TS_MINX, TS_MAXX, 0, KeyPad.w);
    y = map(p.y, TS_MINY, TS_MAXY, 0, KeyPad.h);
    if (!wastouched)
    {
      key = checkKeyPad(&KeyPad, x, y);
      if(key+1)
      {
        key_pressed = true;
        drawButton(&KeyPad, key, TFT_BLUE);
      }
    }
    tft.setCursor(100, 150);
  }
  else
  {
    if (wastouched)
    {
      drawButton(&KeyPad, key, 0);
    }
  }
  wastouched = istouched;
  key_pressed = false;
  delay(100);
}
