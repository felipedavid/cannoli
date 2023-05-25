#include "defs.h"

extern LiquidCrystal_I2C lcd;
extern RTC_DS3231 rtc;
extern char screen_buffer[32];
extern Keypad keypad;
// extern Screen screen;
extern DateTime now;
extern dht DHT;
extern bool top_screen_printed;
extern unsigned long start_time;
extern Time time;

char user_input[17];
size_t user_input_len;

void home_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    // Exibe uma mensagem de saudação baseada no horário atual
    if ((now.hour() >= 1) && (now.hour() <= 12)) {
      sprintf(screen_buffer, "    Bom dia!");
      lcd.print(screen_buffer);
    } else if ((now.hour() > 12) && (now.hour() <= 18)) {
      sprintf(screen_buffer, "   Boa tarde!");
      lcd.print(screen_buffer);
    } else {
      sprintf(screen_buffer, "   Boa noite!");
      lcd.print(screen_buffer);
    }
    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "A-Ajustes B-Exibicao");
    lcd.print(screen_buffer);
    top_screen_printed = true;
  }

  char ch = keypad.getKey();

  if (ch == 'A') {
    screen_stack_push(CONFIG_SCREEN);
  } else if (ch == 'B') {
    screen_stack_push(EXIB_SCREEN);
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void config_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "2-Data  3-Relog");
    lcd.print(screen_buffer);
    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "#-Volta D-Inicio");
    lcd.print(screen_buffer);
    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == '2') {
    screen_stack_push(DATA_EDIT_SCREEN);
  } else if (ch == '3') {
    screen_stack_push(CLOCK_EDIT_SCREEN);
  } else if (ch == '#') {
    screen_stack_pop();
  } else if (ch == 'D') {
    screen_stack_reset();
  }
}

void exib_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "2-Data  3-Hora");
    lcd.print(screen_buffer);
    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "4-Te/Hu  #-volta");
    lcd.print(screen_buffer);
    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == '2') {
    screen_stack_push(EXIB_DATA_SCREEN);
  } else if (ch == '3') {
    screen_stack_push(EXIB_12H_OR_24H_SCREEN);
  } else if (ch == '4') {
    screen_stack_push(EXIB_TEMP_HUM_SCREEN);
  } else if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void exib_data_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Data: %.2d/%.2d/%.2d", now.day(), now.month(),
            now.year());
    lcd.print(screen_buffer);

    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "Dia: ");
    lcd.print(screen_buffer);
    sprintf(screen_buffer, dia_da_semana[now.dayOfTheWeek()]);
    lcd.print(screen_buffer);

    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void exib_12h_ou_24h_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "2-Formato 12h");
    lcd.print(screen_buffer);
    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "3-Formato 24h");
    lcd.print(screen_buffer);
    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == '2') {
    screen_stack_push(EXIB_12H_SCREEN);
  } else if (ch == '3') {
    screen_stack_push(EXIB_24H_SCREEN);
  } else if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}


void exib_12h_screen() {
  unsigned long current_time = millis();
  if (current_time - start_time > 1000) {
    start_time = millis();
    if ((now.hour() - 12) == (-12)) {
    int hora = 12;
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Hora:%.2d:%.2d:%.2d AM", hora, now.minute(),
            now.second());
    lcd.print(screen_buffer);
  } else if ((now.hour() - 12) == 0) {
    int hora = 12;
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Hora:%.2d:%.2d:%.2d PM", hora, now.minute(),
            now.second());
    lcd.print(screen_buffer);

    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "Dia: ");
    lcd.print(screen_buffer);
    sprintf(screen_buffer, dia_da_semana[now.dayOfTheWeek()]);
    lcd.print(screen_buffer);
  } else if ((now.hour() - 12) > 0) {
    int hora = now.hour() - 12;
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Hora:%.2d:%.2d:%.2d PM", hora, now.minute(),
            now.second());
    lcd.print(screen_buffer);

    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "Dia: ");
    lcd.print(screen_buffer);
    sprintf(screen_buffer, dia_da_semana[now.dayOfTheWeek()]);
    lcd.print(screen_buffer);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Hora:%.2d:%.2d:%.2d AM", now.hour(), now.minute(),
            now.second());
    lcd.print(screen_buffer);

    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "Dia: ");
    lcd.print(screen_buffer);
    sprintf(screen_buffer, dia_da_semana[now.dayOfTheWeek()]);
    lcd.print(screen_buffer);
  }
  }
  

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void exib_24h_acreen() {
  unsigned long current_time = millis();
  if (current_time - start_time > 1000) {
    start_time = millis();
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Hora: %.2d:%.2d:%.2d", now.hour(), now.minute(),
            now.second());
    lcd.print(screen_buffer);

    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "Dia: ");
    lcd.print(screen_buffer);
    sprintf(screen_buffer, dia_da_semana[now.dayOfTheWeek()]);
    lcd.print(screen_buffer);
  }
  }

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void exib_temp_hum_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(GOTA_ICON_INDEX);
    lcd.setCursor(1, 0);
    sprintf(screen_buffer, " Humi: %f", DHT.humidity);
    lcd.print(screen_buffer);
    lcd.setCursor(12, 0);
    lcd.write(PORCENT_ICON_INDEX);

    lcd.setCursor(0, 1);
    lcd.write(TERM_ICON_INDEX);
    lcd.setCursor(1, 1);
    sprintf(screen_buffer, " Temp: %f", DHT.temperature);
    lcd.print(screen_buffer);
    lcd.setCursor(12, 1);
    lcd.write(ORDINAL_ICON_INDEX);
    sprintf(screen_buffer, "C");
    lcd.print(screen_buffer);

    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void data_edit_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "2-Dia  3-Mes");
    lcd.print(screen_buffer);
    lcd.setCursor(0, 1);
    sprintf(screen_buffer, "4-Ano  #-Voltar");
    lcd.print(screen_buffer);
    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == '2') {
    screen_stack_push(DATA_DAY_SCREEN);
  } else if (ch == '3') {
    screen_stack_push(DATA_MONTH_SCREEN);
  } else if (ch == '4') {
    screen_stack_push(DATA_YEAR_SCREEN);
  } else if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

bool is_number(char ch) {
  return (ch >= '0') && (ch <= '9');
}

void data_day_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Digite o Dia:");
    lcd.print(screen_buffer);
    lcd.setCursor(0, 1);

    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  } else if (ch == 'C') {
    user_input[user_input_len++] = '\0';
    time.day = atoi(user_input);
  } else if (is_number(ch)) {
    user_input[user_input_len++] = ch;
    lcd.print(ch);
  }
}
void data_month_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Digite o Mes");
    lcd.print(screen_buffer);
    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void data_year_screen() {
  if (!top_screen_printed) {
    lcd.clear();
    lcd.setCursor(0, 0);
    sprintf(screen_buffer, "Digite o Ano");
    lcd.print(screen_buffer);
    top_screen_printed = true;
  }

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void clock_edit_screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  sprintf(screen_buffer, "2-Hora  3-Min");
  lcd.print(screen_buffer);
  lcd.setCursor(0, 1);
  sprintf(screen_buffer, "4-Sec  #-Voltar");
  lcd.print(screen_buffer);

  char ch = keypad.getKey();
  if (ch == '2') {
    screen_stack_push(CLOCK_HOUR);
  } else if (ch == '3') {
    screen_stack_push(CLOCK_MIN);
  } else if (ch == '4') {
    screen_stack_push(CLOCK_SEC);
  } else if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void clock_hour_screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  sprintf(screen_buffer, "Digite as Hora");
  lcd.print(screen_buffer);

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void clock_min_screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  sprintf(screen_buffer, "Digite os Min");
  lcd.print(screen_buffer);

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}

void clock_sec_screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  sprintf(screen_buffer, "Digite os Seg");
  lcd.print(screen_buffer);

  char ch = keypad.getKey();
  if (ch == 'D') {
    screen_stack_reset();
  } else if (ch == '#') {
    screen_stack_pop();
  }
}
