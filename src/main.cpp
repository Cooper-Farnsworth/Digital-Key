#include <Arduino.h>

const int red_light = 25;
const int red_button = 23;
const int green_button = 22;
const int blue_button = 21;
const int yellow_button = 0;

const int door_code[] = {3, 1, 4, 2};
int input_code[] = {0, 0, 0, 0};
int input_index = 0; // Текущая позиция для ввода цифры

uint32_t door_open_timer = 10000;
uint32_t current_time = millis();
uint32_t start_time = 0;
bool door_is_open = true;

// Для обработки нажатий кнопок
bool last_red_state = LOW;
bool last_green_state = LOW;
bool last_blue_state = LOW;
bool last_yellow_state = LOW;

void setup()
{
  pinMode(red_light, OUTPUT);

  pinMode(red_button, INPUT_PULLDOWN);
  pinMode(green_button, INPUT_PULLDOWN);
  pinMode(blue_button, INPUT_PULLDOWN);
  pinMode(yellow_button, INPUT_PULLDOWN);

  Serial.begin(115200);
  Serial.println("System started");
}

bool IsDark()
{
  return true;
}

// Функция для обработки нажатия кнопки
void handle_button_press(int button_value) {
  if (input_index < 4) {
    input_code[input_index] = button_value;
    Serial.print("Button pressed: ");
    Serial.println(button_value);
    Serial.print("Current code: ");
    for (int i = 0; i <= input_index; i++) {
      Serial.print(input_code[i]);
      Serial.print(" ");
    }
    Serial.println();
    input_index++;
  }
}

bool check_input_code() {
  for (int i = 0; i < 4; i++) {
    if (input_code[i] != door_code[i]) {
      return false;
    }
  }
  return true;
}

void loop()
{
  current_time = millis();

  // Обработка нажатий кнопок (только если дверь закрыта)
  if (!door_is_open) {
    // Красная кнопка (1)
    if (digitalRead(red_button) == HIGH && last_red_state == LOW) {
      delay(50); // антидребезг
      if (digitalRead(red_button) == HIGH) {
        handle_button_press(1);
      }
    }
    last_red_state = digitalRead(red_button);
    
    // Зеленая кнопка (2)
    if (digitalRead(green_button) == HIGH && last_green_state == LOW) {
      delay(50);
      if (digitalRead(green_button) == HIGH) {
        handle_button_press(2);
      }
    }
    last_green_state = digitalRead(green_button);
    
    // Синяя кнопка (3)
    if (digitalRead(blue_button) == HIGH && last_blue_state == LOW) {
      delay(50);
      if (digitalRead(blue_button) == HIGH) {
        handle_button_press(3);
      }
    }
    last_blue_state = digitalRead(blue_button);
    
    // Желтая кнопка (4) - РАСКОММЕНТИРОВАТЬ!
    if (digitalRead(yellow_button) == HIGH && last_yellow_state == LOW) {
      delay(50);
      if (digitalRead(yellow_button) == HIGH) {
        handle_button_press(4);
      }
    }
    last_yellow_state = digitalRead(yellow_button);
    
    // Проверяем код, когда введены все 4 цифры
    if (input_index >= 4) {
      if (check_input_code()) {
        door_is_open = true;
        digitalWrite(red_light, HIGH);
        start_time = current_time;
        Serial.println("The code is correct, the door is opened");
      } else {
        Serial.println("Wrong code! Try again.");
      }
      // Сбрасываем ввод независимо от результата
      for (int i = 0; i < 4; i++) {
        input_code[i] = 0;
      }
      input_index = 0;
    }
  }

  // Логика управления дверью
  if (door_is_open) {
    uint32_t time_passed = current_time - start_time;

    if (IsDark() || time_passed >= door_open_timer) {
      door_is_open = false;
      digitalWrite(red_light, LOW);
      Serial.println("The door is closed");
      // Сбрасываем ввод при закрытии двери
      for (int i = 0; i < 4; i++) {
        input_code[i] = 0;
      }
      input_index = 0;
    }
  }

  delay(10);
}