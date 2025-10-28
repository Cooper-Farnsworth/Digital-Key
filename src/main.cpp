#include <Arduino.h>

const int red_light = 25;
const int red_button = 23;
const int green_button = 22;
const int blue_button = 21;
const int yellow_button = 0;

const int door_code[] = {3, 1, 4, 2};
int input_code[] = {0, 0, 0, 0};

uint32_t door_open_timer = 10000;
uint32_t current_time = millis();
uint32_t start_time = 0;
bool door_is_open = true;

void setup()
{
  pinMode(red_light, OUTPUT);

  pinMode(red_button, INPUT_PULLDOWN);
  pinMode(green_button, INPUT_PULLDOWN);
  pinMode(blue_button, INPUT_PULLDOWN);
  pinMode(yellow_button, INPUT_PULLDOWN);

  Serial.begin(115200);
};

bool IsDark()
{
  return true;
};

bool get_input_code()
{
  if (digitalRead(red_button) == HIGH)
  {
    input_code[0] = 1;
    Serial.println("1");
  };
  if (digitalRead(green_button) == HIGH)
  {
    input_code[1] = 2;
    Serial.println("2");
  };
  if (digitalRead(blue_button) == HIGH)
  {
    input_code[2] = 3;
    Serial.println("3");
  };
  // if (digitalRead(yellow_button) == HIGH)
  // {
  //   input_code[3] = 4;
  //   Serial.println("4");
  // };
  for (int i = 0; i < 4; i++)
  {
    if (input_code[i] != door_code[i])
    {
      return false;
    }
  }
  return true;
};

void loop()
{
  current_time = millis();

  if (door_is_open)
  {
    uint32_t time_passed = current_time - start_time;

    if (IsDark() || time_passed >= door_open_timer)
    {
      door_is_open = false;
      digitalWrite(red_light, LOW);
      Serial.println("the door is closed");
    }
  }
  else
  {
    if (get_input_code())
    {
      door_is_open = true;
      digitalWrite(red_light, HIGH);
      start_time = current_time;
      Serial.println("the code is correct, the door is opened");
      for (int i = 0; i < 4; i++)
      {
        input_code[i] = 0;
      }
    }
  }

  delay(100);
}