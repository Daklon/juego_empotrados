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
	unsigned pos_y = TFT_RESOLUTION_Y / 2 - 15;
	unsigned score = 0;
	bool starts = false;
	bool is_left = false;
} player;

typedef struct {
	unsigned pos_x = TFT_RESOLUTION_X / 2;
	unsigned pos_y = TFT_RESOLUTION_Y / 2;
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
	mando.pulsador = digitalRead(JOYSTICK_SWITCH) == 1 ? 0 : 1;
}

// Utiliza el joystick para determinar
// la opción escogida.
// Devuelve el valor al pulsar el joystick
menu choose_menu_option() {}

void draw_ball(ball pelota, unsigned color) {
	tft.drawPixel(pelota.pos_x, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x + 1, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x - 1, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x, pelota.pos_y + 1, color);
	tft.drawPixel(pelota.pos_x, pelota.pos_y - 1, color);
}

void draw_bar(player jugador) {
	unsigned pos_x = (jugador.is_left) ? 20 : TFT_RESOLUTION_X - 20;
	tft.drawFastVLine(pos_x, 0, jugador.pos_y, ST7735_BLACK);
	tft.drawFastVLine(pos_x, jugador.pos_y, 30, ST7735_WHITE);
	tft.drawFastVLine(pos_x, jugador.pos_y + 30, TFT_RESOLUTION_Y - (jugador.pos_y + 30), ST7735_BLACK);
}

// Bucle principal del juego

void move_player(player *jugador) {
	readJoystick();
	if (mando.movimiento > 1 && (jugador->pos_y + 30) != TFT_RESOLUTION_Y)
	{ jugador->pos_y += 1; }
	else if (mando.movimiento < 1 && jugador->pos_y != 0)
	{ jugador->pos_y -= 1; }
}

void move_player_pelota(player *jugador, ball *pelota) {
	readJoystick();
	if (mando.movimiento > 1 && (jugador->pos_y + 30) != TFT_RESOLUTION_Y)
	{ draw_ball(*pelota, ST7735_BLACK); jugador->pos_y += 1; pelota->pos_y += 1; }
	else if (mando.movimiento < 1 && jugador->pos_y != 0)
	{ draw_ball(*pelota, ST7735_BLACK); jugador->pos_y -= 1; pelota->pos_y -= 1; }
}

void start_game() {
	player jugador, maquina;
	ball pelota;

	jugador.starts = true;
	jugador.is_left = true;
	pelota.pos_x = jugador.starts ? 30 : TFT_RESOLUTION_X - 30;
	while(jugador.score < 5 || maquina.score < 5){
		// El jugador lanza la pelota con mando.pulsador
		while (!mando.pulsador) {
			draw_bar(jugador);
			draw_bar(maquina);
			draw_ball(pelota, ST7735_GREEN);

			move_player_pelota(&jugador, &pelota);
		}

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
