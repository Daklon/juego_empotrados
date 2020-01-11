#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define JOYSTICK_ANALOG_X 1
#define JOYSTICK_SWITCH 0

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8


#define TFT_RESOLUTION_X 160
#define TFT_RESOLUTION_Y 128

#define PORTRAIT_ROTATION 0
#define LANDSCAPE_ROTATION 1

// Menú de opciones
typedef enum {
	START_GAME = 0,
	DIFFICULTY,
	HIGH_SCORE,
	QUIT
} menu;

// Dificultad de la IA. Determina la velocidad de movimiento
typedef enum {
	EASY = 0,
	HARD
} difficulty;

typedef struct {
	int pulsador;
	int movimiento;
} joystick;

typedef struct {
	unsigned pos_x;
	bool starts = false;
	unsigned score = 0;
} player;

typedef struct {
	unsigned pos_x;
	unsigned pos_y;
} ball;

typedef struct {
	char player_name[16];
	unsigned score = 0;
} high_score;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

menu opcion_menu;
difficulty dificultad;
joystick mando;
high_score puntuaciones[5];

void readJoystick(void) {
	int analog = analogRead(JOYSTICK_ANALOG_X);

	mando.movimiento = analog / 500;
	mando.pulsador = digitalRead(JOYSTICK_SWITCH);
}

// Utiliza el joystick para determinar
// la opción escogida.
// Devuelve el valor al pulsar el joystick
menu choose_menu_option() {}

// Bucle principal del juego
void start_game() {
	player jugador, maquina;
	while(jugador.score < 5 || maquina.score < 5){
		// Dibujar jugadores, pelota
		// El jugador lanza la pelota con mando.pulsador
	/*

		readJoystick();
		if (mando.movimiento > 1) {
			pos_y += 10;
		} else if (mando.movimiento < 1) {
			pos_y -= 10;
		}

		if (last_pos != pos_y) {
			tft.fillScreen(ST7735_BLACK);
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(40,pos_y);
			tft.setTextSize(5);
			tft.print("WIN");
			last_pos = pos_y;
		}

	*/
	}
}

// Selecciona entre dificultades
difficulty change_difficulty() {}

// Muestra, en orden descendente, las
// mejores puntuaciones
void show_high_scores() {}


void setup(void) {
	Serial.begin(9600);
	// Initialize 1.8" TFT
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	tft.fillScreen(ST7735_BLACK);
	tft.setRotation(LANDSCAPE_ROTATION);

	/*
	   Serial.println("OK!");
	//Prints the Title, the two walls, and "press joystick" on the title screen
	tft.setTextSize(5);//Prints "PONG"
	tft.setCursor(6,30);
	tft.print("PONG");

	tft.setTextSize(0);//Prints "Press Joystick"
	tft.setCursor(20,140);
	tft.print("Press Joystick");

	tft.drawLine(20,90,20,120,ST7735_WHITE); //Prints the 2 walls in the title screen
	tft.drawLine(21,90,21,120,ST7735_WHITE);
	tft.drawLine(100,90,100,120,ST7735_WHITE);
	tft.drawLine(99,90,99,120,ST7735_WHITE);
	 */
	opcion_menu = START_GAME;
	dificultad = EASY;
	mando.pulsador = 0;
	mando.movimiento = 1;
}

//Aquí va el código del juego
void loop(){
	// Menú
	/*
	   do {
	   print_menu();
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
