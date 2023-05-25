#include <Keypad.h>            // Biblioteca do teclado matricial 4x4
#include <LiquidCrystal_I2C.h> // Biblioteca do display lcd 2x16
#include <RTClib.h>            // Biblioteca do relogio em tempo real DS3231
#include <dht.h> // Biblioteca do sensor de humidade e temperatura DHT11

#include "custom_icons.h"
#include "defs.h"

const char *dia_da_semana[7] = {"Domingo", "Segunda", "Terca", "Quarta",
                                "Quinta",  "Sexta",   "Sabado"};

typedef struct {
	int day;
	int month;
	int year;
	int hour;
	int minute;
	int second;
} Time;

Time time;

const char *screen_type_str[] = {
    "HOME_SCREEN",   // Tela inicial
    "CONFIG_SCREEN", // Tela de ajustes

    "EXIB_SCREEN",            // Tela de exibição
    "EXIB_DATA_SCREEN",       // Exibição de data
    "EXIB_TEMP_HUM_SCREEN",   // Exibição de temperatura e umidade
    "EXIB_12H_OR_24H_SCREEN", // Exibe a seleção do tipo de formato das horas
    "EXIB_12H_SCREEN",        // Exibe em padrão 12h
    "EXIB_24H_SCREEN",        // Exibe em padrão 24h

    "DATA_EDIT_SCREEN",  // Tela de edição de data
    "DATA_DAY_SCREEN",   // Tela de edição de dia
    "DATA_MONTH_SCREEN", // Tela de edição de mês
    "DATA_YEAR_SCREEN",  // Tela de edição de ano

    "CLOCK_EDIT_SCREEN", // Tela de edição do relógio
    "CLOCK_HOUR",        // Tela de edição da hora
    "CLOCK_MIN",         // Tela de ediçao dos minutos
    "CLOCK_SEC",         // Tela de edição dos segundos
};

typedef enum {
  NONE_SCREEN,
  HOME_SCREEN,   // Tela inicial
  CONFIG_SCREEN, // Tela de ajustes

  EXIB_SCREEN,            // Tela de exibição
  EXIB_DATA_SCREEN,       // Exibição de data
  EXIB_TEMP_HUM_SCREEN,   // Exibição de temperatura e umidade
  EXIB_12H_OR_24H_SCREEN, // Exibe a seleção do tipo de formato das horas
  EXIB_12H_SCREEN,        // Exibe em padrão 12h
  EXIB_24H_SCREEN,        // Exibe em padrão 24h

  DATA_EDIT_SCREEN,  // Tela de edição de data
  DATA_DAY_SCREEN,   // Tela de edição de dia
  DATA_MONTH_SCREEN, // Tela de edição de mês
  DATA_YEAR_SCREEN,  // Tela de edição de ano

  CLOCK_EDIT_SCREEN, // Tela de edição do relógio
  CLOCK_HOUR,        // Tela de edição da hora
  CLOCK_MIN,         // Tela de ediçao dos minutos
  CLOCK_SEC,         // Tela de edição dos segundos
} Screen;

static void (*screens_fun[])(void) = {
    [NONE_SCREEN] = NULL,
    [HOME_SCREEN] = home_screen,
    [CONFIG_SCREEN] = config_screen,

    [EXIB_SCREEN] = exib_screen,
    [EXIB_DATA_SCREEN] = exib_data_screen,
    [EXIB_TEMP_HUM_SCREEN] = exib_temp_hum_screen,
    [EXIB_12H_OR_24H_SCREEN] = exib_12h_ou_24h_screen,
    [EXIB_12H_SCREEN] = exib_12h_screen,
    [EXIB_24H_SCREEN] = exib_24h_acreen,

    [DATA_EDIT_SCREEN] = data_edit_screen,
    [DATA_DAY_SCREEN] = data_day_screen,
    [DATA_MONTH_SCREEN] = data_month_screen,
    [DATA_YEAR_SCREEN] = data_year_screen,

    [CLOCK_EDIT_SCREEN] = clock_edit_screen,
    [CLOCK_HOUR] = clock_hour_screen,
    [CLOCK_MIN] = clock_min_screen,
    [CLOCK_SEC] = clock_sec_screen,
};

const byte LINHAS = 4;
const byte COLUNAS = 4;

const char TECLAS_MATRIZ[LINHAS][COLUNAS] = {{'1', '2', '3', 'A'},
                                             {'4', '5', '6', 'B'},
                                             {'7', '8', '9', 'C'},
                                             {'*', '0', '#', 'D'}};

#define DHT_PIN A3
const byte PINOS_LINHAS[LINHAS] = {13, 12, 11, 10};
const byte PINOS_COLUNAS[COLUNAS] = {9, 8, 7, 6};

LiquidCrystal_I2C lcd(0x3E, 16, 2); // Inicia o display lcd no endereço 0x3E
RTC_DS3231 rtc;                     // Cria um objeto do tipo RTC_DS3231
dht DHT;
char screen_buffer[32];
Keypad keypad = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS,
                       LINHAS, COLUNAS);
DateTime now;
bool top_screen_printed = false;

Screen *top_screen = NULL;
size_t stack_len = 0;
Screen screens_stack[20];

void screen_stack_push(Screen screen) {
  if (stack_len >= 256) {
    printf("Pilha cheia!!!");
    return;
  }
  screens_stack[stack_len] = screen;
  top_screen = &screens_stack[stack_len];
  stack_len++;
  top_screen_printed = false;
}

void screen_stack_pop() {
  stack_len--;
  top_screen_printed = false;
  top_screen = &screens_stack[stack_len - 1];
}

void screen_stack_reset() {
  stack_len = 0;
  screen_stack_push(HOME_SCREEN);
  Serial.println(stack_len);
  Serial.println(*top_screen);
}

unsigned long start_time;

void setup() {
  // put your setup code here, to run once:
  lcd.setBacklight(HIGH);
  lcd.init();
  lcd.createChar(TERM_ICON_INDEX, term);
  lcd.createChar(GOTA_ICON_INDEX, gota);
  lcd.createChar(PORCENT_ICON_INDEX, porcent);
  lcd.createChar(ORDINAL_ICON_INDEX, ordinal);
  Serial.begin(9600);
  rtc.begin();


  rtc.adjust(
      DateTime(F(__DATE__),
               F(__TIME__))); // Captura a hora e a data e hora da compilação
	
  DateTime now = rtc.now();
  
  time.year = now.year();
  time.month = now.month();
  time.day = now.day();
  time.hour = now.hour();
  time.minute = now.minute();
  time.second = now.second();
  
  screen_stack_push(HOME_SCREEN);
  //screen_stack_push(DATA_DAY_SCREEN);
  start_time = millis();
}

void loop() {
  Serial.println(screen_type_str[*top_screen]);
  now = rtc.now();
  screens_fun[*top_screen](); // assert(top_screen != NONE_SCREEN);
}
