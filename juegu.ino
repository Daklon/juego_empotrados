#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define SECONDS 1000

#define JOYSTICK_ANALOG_Y 1
#define JOYSTICK_SWITCH 0

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

#define TFT_RESOLUTION_X 160
#define TFT_RESOLUTION_Y 128

#define PORTRAIT_ROTATION 2
#define LANDSCAPE_ROTATION 1

#define BAR_WIDTH 20
#define BALL_MARGIN 30

#define BALL_SPEED_FACTOR 1
#define WIN_SCORE 5

// Menú de opciones
typedef enum {
	START_GAME = 0,
	DIFFICULTY,
	HIGH_SCORE,
	QUIT,
	_menu_size
} menu;

char const *menu_strings[_menu_size] = {
	"START GAME",
	"DIFFICULTY",
	"HIGH-SCORES",
	"QUIT"
};

// Dificultad de la IA. Determina la velocidad de movimiento
typedef enum {
	EASY = 0,
	HARD,
	_difficulty_size
} difficulty;

char const *difficulty_strings[_difficulty_size] = {
	"EASY",
	"HARD"
};

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
high_score puntuaciones[10];

void readJoystick(void) {
	int analog = analogRead(JOYSTICK_ANALOG_Y);

	mando.movimiento = analog / 500;
	mando.pulsador = digitalRead(JOYSTICK_SWITCH) == 1 ? 0 : 1;
}

void print_menu(menu opcion) {
	int i, cursor_offset = 35, cursor_margin = 18;
	tft.setCursor(25, cursor_margin);
	tft.setTextSize(4);
	tft.setTextColor(ST7735_RED);
	tft.print("PONG");

	tft.setTextSize(2);
	for (i = 0; i < (int)_menu_size; i++) {
		tft.setCursor(15, cursor_offset + cursor_margin * (i + 1));
		((int)opcion == i) ? tft.setTextColor(ST7735_YELLOW) : tft.setTextColor(ST7735_WHITE);
		tft.print(menu_strings[i]);
	}
}

// Utiliza el joystick para determinar
// la opción escogida.
// Devuelve el valor al pulsar el joystick
menu start_menu() {
	menu seleccion = START_GAME;
	tft.fillScreen(ST7735_BLACK);
	delay(100);
	readJoystick();
	while (!mando.pulsador) {
		print_menu(seleccion);
		do {
			readJoystick();
		} while (mando.movimiento == 1 && !mando.pulsador);

		if (mando.movimiento != 1) {
			switch (seleccion) {
			case START_GAME:
				seleccion = (mando.movimiento > 1) ? DIFFICULTY : QUIT;
				break;
			case DIFFICULTY:
				seleccion = (mando.movimiento > 1) ? HIGH_SCORE : START_GAME;
				break;
			case HIGH_SCORE:
				seleccion = (mando.movimiento > 1) ? QUIT : DIFFICULTY;
				break;
			case QUIT:
				seleccion = (mando.movimiento > 1) ? START_GAME: HIGH_SCORE;
				break;
			}
		}
	delay(200);
	}
	readJoystick();
	return seleccion;
}

void draw_ball(ball pelota, unsigned color) {
	clear_ball(pelota);
	tft.drawPixel(pelota.pos_x, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x + 1, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x - 1, pelota.pos_y, color);
	tft.drawPixel(pelota.pos_x, pelota.pos_y + 1, color);
	tft.drawPixel(pelota.pos_x, pelota.pos_y - 1, color);
}

void clear_ball(ball pelota) {
	tft.drawPixel(pelota.pos_x + 1, pelota.pos_y + 1, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x + 1, pelota.pos_y - 1, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x - 1, pelota.pos_y + 1, ST7735_BLACK);
	tft.drawPixel(pelota.pos_x - 1, pelota.pos_y - 1, ST7735_BLACK);
	tft.drawCircle(pelota.pos_x, pelota.pos_y, 2, ST7735_BLACK);
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

void move_machine(player *machine, ball pelota) {
	static unsigned do_inverse = 0;
	if (pelota.pos_y > (machine->pos_y + BAR_WIDTH / 2) &&
		(machine->pos_y + BAR_WIDTH != TFT_RESOLUTION_Y)) {
		if (do_inverse > 0 && machine->pos_y != 0) {
			machine->pos_y -= 1;
		}
		else if (do_inverse == 0) {
			machine->pos_y += 1;
		}
	}
	if (pelota.pos_y < (machine->pos_y + BAR_WIDTH / 2) && machine->pos_y != 0) {
		if (machine->pos_y + BAR_WIDTH != TFT_RESOLUTION_Y && do_inverse > 0) {
			machine->pos_y += 1;
		}
		else if (do_inverse == 0) {
			machine->pos_y -= 1;
		}
	}

	if (pelota.pos_y == (machine->pos_y + BAR_WIDTH / 2) && machine->pos_y != 0 &&
		machine->pos_y + BAR_WIDTH != TFT_RESOLUTION_Y && do_inverse > 0) {
		if (machine->pos_y != 0) {
			machine->pos_y -= 1;
		}
		else if (machine->pos_y + BAR_WIDTH != 0) {
			machine->pos_y += 1;
		}
	}

	if (do_inverse > 0) { do_inverse -= 1; } else { do_inverse = random(100) == 0 ? 20 : 0; }
}

void move_player_ball(player *jugador, ball *pelota) {
	readJoystick();
	if (mando.movimiento > 1 && (jugador->pos_y + BAR_WIDTH) != TFT_RESOLUTION_Y)
	{ jugador->pos_y += 1; pelota->pos_y += 1; }
	else if (mando.movimiento < 1 && jugador->pos_y != 0)
	{ jugador->pos_y -= 1; pelota->pos_y -= 1; }
}

void move_ball(ball *pelota, int sentido) {
	static unsigned contador_angulo = 0;
	pelota->pos_x += sentido;
	if (pelota->angle != 0) {
		if (contador_angulo % pelota->angle == 2) { // Probablemente la línea más tensa del programa
			pelota->pos_y += (pelota->angle > 0) ? 1 : -1;
			contador_angulo = 0;
		}
		contador_angulo += 1;
	}
}

bool check_collition_bar(ball *pelota, player a, player b) {
	if (pelota->pos_x == TFT_RESOLUTION_X - 20 || pelota->pos_x == 20) {
		if ((pelota->pos_y >= a.pos_y && pelota->pos_y <= (a.pos_y + BAR_WIDTH) && pelota->goes_left) ||
			(pelota->pos_y >= b.pos_y && pelota->pos_y <= (b.pos_y + BAR_WIDTH)) && !pelota->goes_left)
		{
			if (pelota->goes_left) {
				pelota->angle = pelota->pos_y - (a.pos_y + BAR_WIDTH / 2);
			} else {
				pelota->angle = pelota->pos_y - (b.pos_y + BAR_WIDTH / 2);
			}
			pelota->angle = pelota->angle != 0 ? pelota->angle : random(-9, 10);
			return true;
		}
	}
	return false;
}

// TODO: Arreglar bug rebote con poco ángulo
bool check_collition_vertical(ball *pelota) {
	if (pelota->pos_y - 1 == 0 || pelota->pos_y + 1 == TFT_RESOLUTION_Y) {
		pelota->angle = (pelota->angle == 0) ? -3 : -pelota->angle;
		pelota->pos_y += (pelota->pos_y - 1 == 0) ? 1 : -1;
		clear_ball(*pelota); // Borra trailing accidental al desplazar la pelota
		return true;
	}
	return false;
}

// Bucle principal del juego
void start_game(difficulty dificultad) {
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
		do {
			draw_score(jugador, ST7735_WHITE); draw_bar(jugador);
			draw_score(maquina, ST7735_WHITE); draw_bar(maquina);
			draw_ball(pelota, ST7735_GREEN);

			move_player_ball(&jugador, &pelota);
		} while (!mando.pulsador && jugador.starts);

		// El juego sucede mientras la pelota no toque ninguna pared
		while (pelota.pos_x != 0 && pelota.pos_x != TFT_RESOLUTION_X) {
			tft.drawFastVLine(TFT_RESOLUTION_X / 2, 0, TFT_RESOLUTION_Y, ST7735_WHITE);
			draw_score(jugador, ST7735_WHITE); draw_bar(jugador);
			draw_score(maquina, ST7735_WHITE); draw_bar(maquina);
			draw_ball(pelota, ST7735_GREEN);

			// Control de la velocidad de la pelota
			if (factor % (2 - dificultad) == 0) {
				check_collition_vertical(&pelota);
				if (check_collition_bar(&pelota, jugador, maquina)) {
					sentido *= -1;
					pelota.goes_left = !pelota.goes_left;
				}
				move_ball(&pelota, sentido);
			}
			move_player(&jugador);
			if (factor % 2 == 0) {
				move_machine(&maquina, pelota);
			}
			factor += 1;
		}
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
	delay(2 * SECONDS);
}

void print_difficulty_menu(difficulty dificultad) {
	int i, cursor_offset = 35, cursor_margin = 18;
	tft.setCursor(20, cursor_margin);
	tft.setTextSize(2);
	tft.setTextColor(ST7735_RED);
	tft.print("DIFFICULTY");

	tft.setTextSize(2);
	for (i = 0; i < (int)_difficulty_size; i++) {
		tft.setCursor(15, cursor_offset + cursor_margin * (i + 1));
		((int)dificultad == i) ? tft.setTextColor(ST7735_YELLOW) : tft.setTextColor(ST7735_WHITE);
		tft.print(difficulty_strings[i]);
	}
}

// Selecciona entre dificultades
difficulty change_difficulty(difficulty dificultad) {
	tft.fillScreen(ST7735_BLACK);
	delay(100);
	readJoystick();
	while (!mando.pulsador) {
		print_difficulty_menu(dificultad);
		do {
			readJoystick();
		} while (mando.movimiento == 1 && !mando.pulsador);

		if (mando.movimiento != 1) {
			switch (dificultad) {
			case EASY:
				dificultad = HARD;
				break;
			case HARD:
				dificultad = EASY;
				break;
			}
		}
	delay(200);
	}
	readJoystick();
	return dificultad;
}

// Muestra, en orden descendente, las
// mejores puntuaciones
void show_high_scores() {}

void print_bye_screen() {
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(35, 25);
	tft.setTextSize(4);
	tft.setTextColor(ST7735_WHITE);
	tft.print("BYE!");
	tft.setRotation(PORTRAIT_ROTATION);
	tft.setCursor(65, 70);
	tft.print(":)");
}

void setup(void) {
	Serial.begin(9600);
	randomSeed(analogRead(5));
	// Initialize 1.8" TFT
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	tft.fillScreen(ST7735_BLACK);
	tft.setRotation(LANDSCAPE_ROTATION);

	dificultad = EASY;
	mando.pulsador = 0;
	mando.movimiento = 1;
}

//Aquí va el código del juego
void loop(){
	// Menú
	do {
		opcion_menu = start_menu();

		switch (opcion_menu) {
		case START_GAME:
			start_game(dificultad);
			break;
		case DIFFICULTY:
			dificultad = change_difficulty(dificultad);
			break;
		case HIGH_SCORE:
			show_high_scores();
			break;
		}
	}
	while (opcion_menu != QUIT);
	print_bye_screen();
	exit(0);
}
