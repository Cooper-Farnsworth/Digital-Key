#include <Arduino.h>

const int red_light = 25;
const int red_button = 23;
const int green_button = 22;
const int blue_button = 21;
const int yellow_button = 15;

const int door_code[] = {3, 1, 4, 2};
int input_code[] = {0, 0, 0, 0};
int input_index = 0;

uint32_t door_open_timer = 10000;
uint32_t current_time = millis();
uint32_t start_time = 0;
bool door_is_open = true;

// Для обработки нажатий кнопок
bool last_red_state = LOW;    // ИЗМЕНИТЬ на HIGH
bool last_green_state = HIGH;  // ИЗМЕНИТЬ на HIGH
bool last_blue_state = HIGH;   // ИЗМЕНИТЬ на HIGH
bool last_yellow_state = HIGH; // ИЗМЕНИТЬ на HIGH

void setup()
{
  pinMode(red_light, OUTPUT);

  // ИЗМЕНИТЬ на INPUT_PULLUP
  pinMode(red_button, INPUT_PULLUP);
  pinMode(green_button, INPUT_PULLUP);
  pinMode(blue_button, INPUT_PULLUP);
  pinMode(yellow_button, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("System started");
  
  // Вывод начальных состояний кнопок для отладки
  Serial.print("Initial button states - Red: ");
  Serial.print(digitalRead(red_button));
  Serial.print(" Green: ");
  Serial.print(digitalRead(green_button));
  Serial.print(" Blue: ");
  Serial.print(digitalRead(blue_button));
  Serial.print(" Yellow: ");
  Serial.println(digitalRead(yellow_button));
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

// Функция для проверки нажатия кнопки с антидребезгом (ИЗМЕНИТЬ логику)
bool buttonPressed(int pin, bool &lastState) {
  bool currentState = digitalRead(pin);
  // ИЗМЕНИТЬ: нажатие - когда кнопка замыкает на GND (LOW)
  if (currentState == LOW && lastState == HIGH) {
    delay(50); // антидребезг
    currentState = digitalRead(pin);
    if (currentState == LOW) {
      lastState = currentState;
      return true;
    }
  }
  lastState = currentState;
  return false;
}

bool check_input_code() {
  Serial.print("Checking code: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(input_code[i]);
    if (input_code[i] != door_code[i]) {
      Serial.println(" - WRONG!");
      return false;
    }
  }
  Serial.println(" - CORRECT!");
  return true;
}

void loop()
{
  current_time = millis();

  // Добавим отладку состояний кнопок
  static uint32_t last_debug_time = 0;
  if (current_time - last_debug_time > 1000) {
    last_debug_time = current_time;
    Serial.print("Button states - R:");
    Serial.print(digitalRead(red_button));
    Serial.print(" G:");
    Serial.print(digitalRead(green_button));
    Serial.print(" B:");
    Serial.print(digitalRead(blue_button));
    Serial.print(" Y:");
    Serial.println(digitalRead(yellow_button));
  }

  // Обработка нажатий кнопок (только если дверь закрыта)
  if (!door_is_open) {
    // Проверяем все кнопки
    if (buttonPressed(red_button, last_red_state)) {
      handle_button_press(1);
    }
    
    if (buttonPressed(green_button, last_green_state)) {
      handle_button_press(2);
    }
    
    if (buttonPressed(blue_button, last_blue_state)) {
      handle_button_press(3);
    }
    
    if (buttonPressed(yellow_button, last_yellow_state)) {
      handle_button_press(4);
    }
    
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
      Serial.println("Input reset");
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