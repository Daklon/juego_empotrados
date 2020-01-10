#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
int analog_pin = A3; // TODO: cambiar
int joystick_value = 0; // Valor del joystick al recibirlo con analogRead(analog_pin)
int player_score = 0, game_score = 0; // Puntuaciones del jugador y de la máquina

// Menú de opciones
#define START_GAME	0
#define DIFFICULTY	1
#define HIGH_SCORE	2
#define QUIT		3

// Dificultad de la IA. Determina la velocidad de movimiento
#define EASY	0
#define HARD	1

typedef struct {
	char player_name[16];
	unsigned score;
} high_score;

int menu;
int dificultad;
high_score scores[5];

void drawtext(char *text, uint16_t color){
    tft.setCursor(0,0);
    tft.setTextColor(color);
    // tft.setWrap(true);
    tft.print(text);

}

// Utiliza el joystick para determinar
// la opción escogida.
// Devuelve el valor al pulsar el joystick
game_menu choose_menu_option() {

}

// Bucle principal del juego
void start_game() {

}

// Selecciona entre dificultades
difficulty change_difficulty() {

}

// Muestra, en orden descendente, las
// mejores puntuaciones
void show_high_scores() {

}


void setup(void) {
    Serial.begin(9600);
    Serial.print(F("Hello! ST7735 TFT Test"));

    tft.initR(INITR_BLACKTAB);

    Serial.print(F("Initialized"));
    tft.fillScreen(ST77XX_BLACK);
    // drawtext("texto");
	menu = 0;
	dificultad = 0;
}

//Aquí va el código del juego
void loop(){
	// Menú
	/*
	do {
		menu = choose_menu_option();
		switch (menu) {
		case START_GAME:
			start_game();
			break;
		case DIFFICULTY:
			difficulty = change_difficulty();
			break;
		case HIGH_SCORE:
			show_high_scores();
			break;
		}
	}
	while (option != QUIT);
	*/
	start_game();
	exit(0);
}

