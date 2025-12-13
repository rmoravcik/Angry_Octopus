#include <Servo.h>

Servo doorServo;
Servo fingerServo1; 
Servo fingerServo2;
Servo fingerServo3;
Servo fingerServo4;
Servo fingerServo5;

#define NUM_OF_FINGERS 5

typedef struct finger {
  uint8_t switchGpio;
  uint8_t servoGpio;
  Servo * servo;
  uint8_t posHide;
  uint8_t posTease;
  uint8_t posFlip;
} finger_t;

finger_t fingers[NUM_OF_FINGERS] = {
  { 12,  3, &fingerServo1, 180, 130, 100 },
  { 10,  5, &fingerServo2, 190, 120,  90 },
  {  8,  7, &fingerServo3, 180, 120,  70 },
  {  6,  9, &fingerServo4, 180, 120,  70 },
  {  4, 11, &fingerServo5, 180, 120,  70 },
};

typedef struct door {
  uint8_t servoGpio;
  Servo * servo;
  uint8_t posClose;
  uint8_t posOpen;
} door_t;

door_t door = {
//  13, &doorServo, 105, 30
  13, &doorServo, 105, 35
};

enum mode_t {
  SWITCH_FLIP_FLOP_SLOW = 0,
  SWITCH_FLIP_FLOP_SLOW2,
  SWITCH_FLIP_FLOP,
  SWITCH_FLIP_FLOP_FAST,
  TEASE_SWITCH_FLIP_FLOP,
  DOUBLE_TEASE_SWITCH_FLIP_FLOP,
  RANDOM_TEASE_SWITCH_FLIP_FLOP,
  TEASE_ALL_SWITCH_FLIP_FLOP,
  LOOK_SWITCH_FLIP_FLOP,
  MODE_MAX,
};

volatile mode_t mode = SWITCH_FLIP_FLOP_SLOW;
volatile uint8_t angriness = 0;
volatile uint8_t duration = 0;

void setup()
{
  Serial.begin(115200);

  door.servo->attach(door.servoGpio);
  door.servo->write(door.posClose); 

  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    pinMode(fingers[i].switchGpio, INPUT_PULLUP);          
    fingers[i].servo->attach(fingers[i].servoGpio);          
    fingers[i].servo->write(fingers[i].posHide);
  }

  randomSeed(analogRead(0));
}

void printMode(mode_t mode)
{
  Serial.print("mode: ");

  switch (mode)
  {
    case SWITCH_FLIP_FLOP_SLOW:
      Serial.println("SWITCH_FLIP_FLOP_SLOW");
      break;
    case SWITCH_FLIP_FLOP_SLOW2:
      Serial.println("SWITCH_FLIP_FLOP_SLOW2");
      break;
    case SWITCH_FLIP_FLOP:
      Serial.println("SWITCH_FLIP_FLOP");
      break;
    case SWITCH_FLIP_FLOP_FAST:
      Serial.println("SWITCH_FLIP_FLOP_FAST");
      break;
    case TEASE_SWITCH_FLIP_FLOP:
      Serial.println("TEASE_SWITCH_FLIP_FLOP");
      break;
    case DOUBLE_TEASE_SWITCH_FLIP_FLOP:
      Serial.println("DOUBLE_TEASE_SWITCH_FLIP_FLOP");
      break;
    case RANDOM_TEASE_SWITCH_FLIP_FLOP:
      Serial.println("RANDOM_TEASE_SWITCH_FLIP_FLOP");
      break;
    case TEASE_ALL_SWITCH_FLIP_FLOP:
      Serial.println("TEASE_ALL_SWITCH_FLIP_FLOP");
      break;
    case LOOK_SWITCH_FLIP_FLOP:
      Serial.println("LOOK_SWITCH_FLIP_FLOP");
      break;
    default:
      break;
  }
}

void checkAngriness()
{
  static unsigned long lastSwitch = 0;
  unsigned long diff = millis() - lastSwitch;
  lastSwitch = millis();

  if (diff < 5 * 1000)
  {
    angriness+=2;
  }
  else if (diff < 7 * 1000)
  {
    angriness++;
  }
  else
  {
    angriness = 0;
    duration = 0;
  }

  if (angriness == 10)
  {
    duration = random(5, 10);
  }

  if (angriness > 10)
  {
    angriness = 10;
    duration--;
    if (duration == 255)
    {
      duration = 0;
    }
  }

  Serial.print("anriness: ");
  Serial.print(angriness);
  Serial.print(", duration: ");
  Serial.print(duration);
  Serial.print(", ");
  printMode(mode);
}

mode_t checkMode(mode_t mode)
{
  if (angriness == 10)
  {
    if (duration == 0)
    {
      angriness = 0;
      return random(MODE_MAX);
    }
    return SWITCH_FLIP_FLOP;
  }
  else if (angriness > 7)
  {
    return random(SWITCH_FLIP_FLOP, SWITCH_FLIP_FLOP_FAST + 1);
  }
  else if (angriness > 5)
  {
    return random(SWITCH_FLIP_FLOP_SLOW, SWITCH_FLIP_FLOP_SLOW2 + 1);
  }

  return random(MODE_MAX);
}

mode_t runSwitchFlipFlopSlow(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      checkAngriness();
      door.servo->write(door.posOpen);
      delay(500);
      for (uint16_t pos = fingers[i].posHide; pos > fingers[i].posTease; pos = pos - 10)
      {
        fingers[i].servo->write(pos);
        delay(100);
      }
      delay(500);
      fingers[i].servo->write(fingers[i].posFlip - 10);
      delay(100);
      fingers[i].servo->write(fingers[i].posHide);
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

mode_t runSwitchFlipFlopSlow2(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      checkAngriness();
      door.servo->write(door.posOpen);
      delay(500);
      for (uint16_t pos = fingers[i].posHide; pos > fingers[i].posTease; pos = pos - 10)
      {
        fingers[i].servo->write(pos);
        delay(100);
      }
      delay(500);
      fingers[i].servo->write(fingers[i].posFlip - 10);
      delay(500);
      fingers[i].servo->write(fingers[i].posHide);
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

mode_t runSwitchFlipFlop(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      checkAngriness();
      door.servo->write(door.posOpen);
      delay(400);
      fingers[i].servo->write(fingers[i].posFlip);
      delay(175);
      fingers[i].servo->write(fingers[i].posHide);
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

mode_t runSwitchFlipFlopFast(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      checkAngriness();
      delay(400);
      door.servo->write(door.posOpen);
      fingers[i].servo->write(fingers[i].posFlip);
      delay(175);
      fingers[i].servo->write(fingers[i].posHide);
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

mode_t runTeaseSwitchFlipFlop(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      checkAngriness();
      door.servo->write(door.posOpen);
      delay(400);
      fingers[i].servo->write(fingers[i].posTease);
      delay(1000);
      fingers[i].servo->write(fingers[i].posHide);
      delay(100);
      door.servo->write(door.posClose);
      delay(400);
      door.servo->write(door.posOpen);
      delay(100);
      fingers[i].servo->write(fingers[i].posFlip);
      delay(175);
      fingers[i].servo->write(fingers[i].posHide);
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

mode_t runDoubleTeaseSwitchFlipFlop(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      checkAngriness();
      door.servo->write(door.posOpen);
      delay(400);
      fingers[i].servo->write(fingers[i].posTease);
      delay(1000);
      fingers[i].servo->write(fingers[i].posHide);
      delay(400);
      fingers[i].servo->write(fingers[i].posTease);
      delay(1000);
      fingers[i].servo->write(fingers[i].posHide);
      delay(100);
      door.servo->write(door.posClose);
      delay(400);
      door.servo->write(door.posOpen);
      delay(100);
      fingers[i].servo->write(fingers[i].posFlip);
      delay(175);
      fingers[i].servo->write(fingers[i].posHide);
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

mode_t runRandomTeaseSwitchFlipFlop(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      uint8_t randomFinger = i;
      while (randomFinger == i)
      {
        randomFinger = random(NUM_OF_FINGERS);
      }

      checkAngriness();
      door.servo->write(door.posOpen);
      delay(400);
      fingers[randomFinger].servo->write(fingers[randomFinger].posTease);
      delay(1000);
      fingers[randomFinger].servo->write(fingers[randomFinger].posHide);
      delay(400);
      fingers[i].servo->write(fingers[i].posFlip - 10);
      delay(175);
      fingers[i].servo->write(fingers[i].posHide);
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

mode_t runTeaseAllSwitchFlipFlop(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      checkAngriness();
      door.servo->write(door.posOpen);
      delay(400);
      for (uint8_t j = 0; j < NUM_OF_FINGERS; j++)
      {
        fingers[j].servo->write(fingers[j].posTease);
      }
      delay(2000);
      fingers[i].servo->write(fingers[i].posFlip);
      delay(175);
      for (uint8_t j = 0; j < NUM_OF_FINGERS; j++)
      {
        fingers[j].servo->write(fingers[j].posHide);
      }
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

mode_t runLookSwitchFlipFlop(mode_t mode)
{
  for (uint8_t i = 0; i < NUM_OF_FINGERS; i++)
  {
    if (digitalRead(fingers[i].switchGpio) == LOW)
    {
      checkAngriness();
      door.servo->write(door.posOpen);
      delay(2000);
      door.servo->write(door.posClose);
      delay(1000);
      door.servo->write(door.posOpen);
      fingers[i].servo->write(fingers[i].posFlip - 10);
      delay(175);
      fingers[i].servo->write(fingers[i].posHide);
      delay(20);
    }
    else
    {
      door.servo->write(door.posClose);
    }
  }

  return checkMode(mode);
}

void loop()
{
  switch (mode)
  {
    case SWITCH_FLIP_FLOP_SLOW:
      mode = runSwitchFlipFlopSlow(mode);
      break;
    case SWITCH_FLIP_FLOP_SLOW2:
      mode = runSwitchFlipFlopSlow2(mode);
      break;
    case SWITCH_FLIP_FLOP:
      mode = runSwitchFlipFlop(mode);
      break;
    case SWITCH_FLIP_FLOP_FAST:
      mode = runSwitchFlipFlopFast(mode);
      break;
    case TEASE_SWITCH_FLIP_FLOP:
      mode = runTeaseSwitchFlipFlop(mode);
      break;
    case DOUBLE_TEASE_SWITCH_FLIP_FLOP:
      mode = runDoubleTeaseSwitchFlipFlop(mode);
      break;
    case RANDOM_TEASE_SWITCH_FLIP_FLOP:
      mode = runRandomTeaseSwitchFlipFlop(mode);
      break;
    case TEASE_ALL_SWITCH_FLIP_FLOP:
      mode = runTeaseAllSwitchFlipFlop(mode);
      break;
    case LOOK_SWITCH_FLIP_FLOP:
      mode = runLookSwitchFlipFlop(mode);
      break;
    default:
      break;
  }
}