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

typedef struct {
  int pulsador;
  int movimiento;
} joystick;

joystick mando;
//el pulsador del joystick se lee desde el pin 3
void readJoystick(void) {
  int a = analogRead(1);

  mando.movimiento = a / 500;
/*
  Serial.print("Button read analog = ");
  Serial.print("a: ");
  Serial.println(a);
  Serial.print("movimiento: ");
  Serial.println(mando.movimiento);  
*/
  mando.pulsador = digitalRead(0);
}

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
int choose_menu_option() {

}

// Bucle principal del juego
void start_game() {
  int pos_y = 50, last_pos = 50;
  while(player_score < 5 || game_score < 5){
    
    readJoystick();
    if (mando.movimiento > 1) {
      pos_y += 10;
    } else if (mando.movimiento < 1) {
      pos_y -= 10;
    }
    
    if (last_pos != pos_y) {
      tft.fillScreen(ST7735_BLACK);
      tft.setTextColor(ST7735_WHITE);
      tft.setRotation(1);
      tft.setCursor(40,pos_y);
      tft.setTextSize(5);
      tft.print("WIN");
      last_pos = pos_y;
    }
    
  }
}

// Selecciona entre dificultades
int change_difficulty() {

}

// Muestra, en orden descendente, las
// mejores puntuaciones
void show_high_scores() {

}


void setup(void) {
    Serial.begin(9600);
  // Initialize 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  
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

  randomSeed(analogRead(0));//Set up for future uses of "RNG"
    
    menu = 0;
    dificultad = 0;
    mando.pulsador = 0;
    mando.movimiento = 1;
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


