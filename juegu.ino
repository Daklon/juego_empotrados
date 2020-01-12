#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define SECONDS 1000

#define JOYSTICK_ANALOG_X 1
#define JOYSTICK_SWITCH 0

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8


#define TFT_RESOLUTION_X 160
#define TFT_RESOLUTION_Y 128

#define PORTRAIT_ROTATION 0
#define LANDSCAPE_ROTATION 1

#define BAR_WIDTH 20
#define BALL_MARGIN 30

#define BALL_SPEED_FACTOR 2
#define WIN_SCORE 5

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
	unsigned pos_y = TFT_RESOLUTION_Y / 2 - BAR_WIDTH / 2;
	unsigned score = 0;
	bool starts = false;
	bool is_left = false;
} player;

typedef struct {
	unsigned pos_x = TFT_RESOLUTION_X / 2;
	unsigned pos_y = TFT_RESOLUTION_Y / 2;
	int angle = 0;
	bool goes_left = false;
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
	clear_ball(pelota);
	tft.drawPixel(pelota.pos_x, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x + 1, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x - 1, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x, pelota.pos_y + 1, color);
	tft.drawPixel(pelota.pos_x, pelota.pos_y - 1, color);
}

// TODO: Arreglar trails que se quedan por detrás
void clear_ball(ball pelota) {
	tft.drawPixel(pelota.pos_x + 1, pelota.pos_y + 1, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x + 1, pelota.pos_y - 1, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x - 1, pelota.pos_y + 1, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x - 1, pelota.pos_y - 1, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x - 1, pelota.pos_y - 1, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x + 2, pelota.pos_y, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x - 2, pelota.pos_y, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x, pelota.pos_y + 2, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x, pelota.pos_y - 2, ST7735_BLACK);
}

void draw_bar(player jugador) {
	unsigned pos_x = (jugador.is_left) ? 20 : TFT_RESOLUTION_X - 20;
	tft.drawPixel(pos_x, jugador.pos_y - 1, ST7735_BLACK);
	tft.drawPixel(pos_x, jugador.pos_y + BAR_WIDTH + 1, ST7735_BLACK);
	tft.drawFastVLine(pos_x, jugador.pos_y, BAR_WIDTH, ST7735_WHITE);
}

void draw_score(player jugador, unsigned color) {
	unsigned score_pos = jugador.is_left ? 40 : TFT_RESOLUTION_X - 48;
	tft.setCursor(score_pos, 10);
	tft.setTextSize(2);
	tft.setTextColor(color);
	tft.print(jugador.score);
}

void move_player(player *jugador) {
	readJoystick();
	if (mando.movimiento > 1 && (jugador->pos_y + BAR_WIDTH) != TFT_RESOLUTION_Y)
	{ jugador->pos_y += 1; }
	else if (mando.movimiento < 1 && jugador->pos_y != 0)
	{ jugador->pos_y -= 1; }
}

// TODO: Nerfear a la máquina
void move_machine(player *machine, ball pelota) {
	if (pelota.pos_y > (machine->pos_y + BAR_WIDTH / 2) && machine->pos_y >= 0)
	{ machine->pos_y += 1; }

	if (pelota.pos_y < (machine->pos_y + BAR_WIDTH / 2) && machine->pos_y <= TFT_RESOLUTION_Y)
	{ machine->pos_y -= 1; }
}

void move_player_ball(player *jugador, ball *pelota) {
	readJoystick();
	if (mando.movimiento > 1 && (jugador->pos_y + BAR_WIDTH) != TFT_RESOLUTION_Y)
	{ jugador->pos_y += 1; pelota->pos_y += 1; }
	else if (mando.movimiento < 1 && jugador->pos_y != 0)
	{ jugador->pos_y -= 1; pelota->pos_y -= 1; }
}

void move_ball(ball *pelota, int sentido) {
	pelota->pos_x += sentido;
	if (pelota->angle != 0) {
		pelota->pos_y += (pelota->angle > 0) ? 1 : -1;
	}
}

bool check_collition_bar(ball *pelota, player a, player b) {
	if (pelota->pos_x == TFT_RESOLUTION_X - 20 || pelota->pos_x == 20) {
		if ((pelota->pos_y >= a.pos_y && pelota->pos_y <= (a.pos_y + BAR_WIDTH) && pelota->goes_left) ||
			(pelota->pos_y >= b.pos_y && pelota->pos_y <= (b.pos_y + BAR_WIDTH)) && !pelota->goes_left)
		{
			if (pelota->goes_left) {
				pelota->angle += (a.pos_y + BAR_WIDTH) - pelota->pos_y;
			} else {
				pelota->angle += (b.pos_y + BAR_WIDTH) - pelota->pos_y;
			}
			return true;
		}
	}
	return false;
}

bool check_collition_vertical(ball *pelota) {
	if (pelota->pos_y - 1 == 0 || pelota->pos_y + 1 == TFT_RESOLUTION_Y) {
		pelota->angle = -pelota->angle;
		return true;
	}
	return false;
}

// Bucle principal del juego
void start_game() {
	player jugador, maquina;
	ball pelota;
	int factor = 0, sentido = 1;

	jugador.starts = true;
	jugador.is_left = true;
	while(jugador.score < WIN_SCORE && maquina.score < WIN_SCORE) {
		tft.fillScreen(ST7735_BLACK);
		tft.drawFastVLine(TFT_RESOLUTION_X / 2, 0, TFT_RESOLUTION_Y, ST7735_WHITE);
		pelota.pos_x = jugador.starts ? BALL_MARGIN : TFT_RESOLUTION_X - BALL_MARGIN;
		pelota.angle = 0;
		// El jugador lanza la pelota con mando.pulsador
		while (!mando.pulsador && jugador.starts) {
			draw_score(jugador, ST7735_WHITE); draw_bar(jugador);
			draw_score(maquina, ST7735_WHITE); draw_bar(maquina);
			draw_ball(pelota, ST7735_GREEN);

			move_player_ball(&jugador, &pelota);
		} 

		// El juego sucede mientras la pelota no toque ninguna pared
		while (pelota.pos_x != 0 && pelota.pos_x != TFT_RESOLUTION_X) {
			tft.drawFastVLine(TFT_RESOLUTION_X / 2, 0, TFT_RESOLUTION_Y, ST7735_WHITE);
			draw_score(jugador, ST7735_WHITE); draw_bar(jugador);
			draw_score(maquina, ST7735_WHITE); draw_bar(maquina);
			draw_ball(pelota, ST7735_GREEN);

			// Control de la velocidad de la pelota
			if (factor % BALL_SPEED_FACTOR == 0) {
				check_collition_vertical(&pelota);
				if (check_collition_bar(&pelota, jugador, maquina)) {
					sentido *= -1;
					pelota.goes_left = !pelota.goes_left;
				}
				move_ball(&pelota, sentido);
			}
			move_player(&jugador);
			move_machine(&maquina, pelota);
			factor += 1;
		}
		draw_ball(pelota, ST7735_BLACK);
		jugador.pos_y = TFT_RESOLUTION_Y / 2 - BAR_WIDTH / 2;
		maquina.pos_y = TFT_RESOLUTION_Y / 2 - BAR_WIDTH / 2;
		pelota.pos_y = TFT_RESOLUTION_Y / 2;
		if (pelota.pos_x == 0) {
			maquina.score += 1; sentido = 1; pelota.goes_left = false;
			maquina.starts = false; jugador.starts = true;
		}
		else if (pelota.pos_x == TFT_RESOLUTION_X) {
			jugador.score += 1; sentido = -1; pelota.goes_left = true;
			jugador.starts = false; maquina.starts = true;
		}
		delay(1 * SECONDS);
	}
	// Pantalla de "YOU WIN/YOU LOSE"
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(5, 30);
	tft.setTextSize(4);
	if (jugador.score == WIN_SCORE) {
	// TODO: Si el jugador gana ==> Enter your name: y (registrar en high score O aumentar contador)
		tft.setTextColor(ST7735_GREEN);
		tft.print("PLAYER");
		tft.setCursor(30, 70);
		tft.print("WINS");
	} else {
		tft.setTextColor(ST7735_RED);
		tft.print("PLAYER");
		tft.setCursor(20, 70);
		tft.print("LOSES");
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
	tft.setCursor(6,BAR_WIDTH);
	tft.print("PONG");
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
