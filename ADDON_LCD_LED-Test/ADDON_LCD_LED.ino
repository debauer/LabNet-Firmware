#include <SPI.h>
#include <MCP23S17.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

// Color definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF

TFT_ILI9163C tft = TFT_ILI9163C(8, 14);

MCP23S17 expander(&SPI, 9, 0);

int i;
void setup() {
  // start serial port
  Serial.begin(9600);
  Serial.print("Booting LabNet Basis LCD LED Test");
  
	expander.begin();
  tft.begin();
  tft.fillScreen(0xF81F);
  tft.setRotation(0);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.defineScrollArea(23,50);

  expander.begin();
  for(i=0;i<16;i++){
	  expander.pinMode(i,OUTPUT);
  }
}

void loop() {
  expander.writePort(0xFFFF);
  tft.setCursor(0, 0);
  tft.println("Hello World!");
  delay(1000);
  expander.writePort(0x0000);
  tft.println("Hello World!");
  delay(1000);
}
