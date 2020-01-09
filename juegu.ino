#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
    Serial.begin(9600);
    Serial.print(F("Hello! ST7735 TFT Test"));

    tft.initR(INITR_BLACKTAB);

    Serial.print(F("Initialized"));
    tft.fillScreeen(ST77XX_BLACK);
    drawtext("texto");
}

void loop(){
    //Aquí va el código del juego

}

void drawtext(char *text, uint16_t color){
    tft.setCursor(0,0);
    tft.setTextColor(color);
    tft.setWrap(true);
    tft.print(text);

}
