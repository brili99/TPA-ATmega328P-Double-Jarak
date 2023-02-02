#include <Arduino.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <DFPlayerMini_Fast.h>
// #include <FireTimer.h>
#include <Ticker.h>

#define DEBUG_LEVEL 1

#define busy_pin A0
#define echo1 2
#define trig1 3
#define echo2 5
#define trig2 6
#define water A1
#define mq A2
#define flame A3
#define but1 8
#define but2 7
#define mot 10
#define lamp 9

#define ADDRESS_EEPROM_MODE1 137
#define ADDRESS_EEPROM_MODE2 138

void initEpprom();
void initPins();
void initTicker();
void read_sens();
void initDfplayer();
void playMusicDfplayer(int music, bool force);
void btn1_action();
void btn2_action();
void mode1SuaraAction(bool force);
void mode2SuaraAction(bool force);
void btnTick();
void ledTick();
void tickGlobal();

#if DEBUG_LEVEL
void print_info();
#endif

// Variables
int air, api, gas, jarak1, jarak2, sw1, sw2;

long duration;
bool toggleVibrate = false;
bool ledState = true; // mulai nyala
ulong dfplayer_ready_counter;
bool dfplayer_ready, dfplayer_really_ready;
bool state_btn1, state_btn2, btn1_clicked, btn2_clicked;

byte mode1 = 0, mode2 = 0;

SoftwareSerial serialDfplayer(13, 4); // serialDfplayer pin df player
DFPlayerMini_Fast myMP3;
// Ticker tickReadSensor(read_sens, 100);
Ticker tickBlinkLed(ledTick, 200, 5);
// FireTimer tickerTest;

void setup()
{
#if DEBUG_LEVEL
  Serial.begin(9600);
  Serial.println("Start Diaktifkan");
#endif

  initEpprom();
  initPins();
  initTicker();
  initDfplayer();
  delay(300);
  playMusicDfplayer(37, false);
  mode1SuaraAction(false);
  mode2SuaraAction(false);

  // delay(50);
  // digitalWrite(lamp, 1);
  // delay(50);
  // digitalWrite(lamp, 0);
  // delay(500);
  // digitalWrite(lamp, 1);
  // delay(500);
  // digitalWrite(lamp, 0);
}

void loop()
{
  tickGlobal();
  read_sens();
  btnTick();
}

void btnTick()
{
  // Btn 1
  if (!state_btn1 && !btn1_clicked)
  {
    btn1_clicked = true;
    btn1_action();
  }
  if (state_btn1)
  {
    btn1_clicked = false;
  }

  // Btn 2
  if (!state_btn2 && !btn2_clicked)
  {
    btn2_clicked = true;
    btn2_action();
  }
  if (state_btn2)
  {
    btn2_clicked = false;
  }
}

#if DEBUG_LEVEL
void print_info()
{
  Serial.print("DFReady:");
  Serial.print(dfplayer_ready);
  Serial.print("\tAir:");
  Serial.print(air);
  Serial.print("\tApi:");
  Serial.print(api);
  Serial.print("\tGas:");
  Serial.print(gas);
  Serial.print(jarak2);
  Serial.print("\tbtn1:");
  Serial.print(state_btn1);
  Serial.print("\tbtn2:");
  Serial.print(state_btn2);
  Serial.print("\tJarak1:");
  Serial.print(jarak1);
  Serial.print("\tJarak2:");
  Serial.println();
}
#endif

void read_sens()
{
  air = analogRead(water);
  api = analogRead(flame);
  gas = analogRead(mq);
  state_btn1 = digitalRead(but1);
  state_btn2 = digitalRead(but2);
  dfplayer_ready = digitalRead(busy_pin);
  if (dfplayer_ready)
  {
    dfplayer_ready_counter++;
  }
  else
  {
    dfplayer_ready_counter = 0;
  }

  digitalWrite(trig1, LOW);
  delayMicroseconds(2);
  digitalWrite(trig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig1, LOW);
  duration = pulseIn(echo1, HIGH);
  jarak1 = duration * 0.034 / 2;

  digitalWrite(trig2, LOW);
  delayMicroseconds(2);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2, LOW);
  duration = pulseIn(echo2, HIGH);
  jarak2 = duration * 0.034 / 2;

#if DEBUG_LEVEL
  print_info();
#endif
}

void initDfplayer()
{
  serialDfplayer.begin(9600);
#if DEBUG_LEVEL == 2
  myMP3.begin(serialDfplayer, true);
#else
  myMP3.begin(serialDfplayer);
#endif

  // delay(500);
  // myMP3.setTimeout(1);
  // delay(500);
#if DEBUG_LEVEL
  Serial.println("Setting volume to max");
#endif
  myMP3.volume(30);
}

void btn1_action()
{
  mode1++;
  mode2 = 0;
  if (mode1 > 5)
    mode1 = 1;
  EEPROM.write(ADDRESS_EEPROM_MODE1, mode1);
  EEPROM.write(ADDRESS_EEPROM_MODE2, mode2);
  mode1SuaraAction(true);
}

void btn2_action()
{
  // // Enable Disable Vibration
  // toggleVibrate = !toggleVibrate;
  // digitalWrite(mot, toggleVibrate);
  mode2++;
  if (mode2 > 5)
    mode2 = 0;
  EEPROM.write(ADDRESS_EEPROM_MODE2, mode2);
  mode2SuaraAction(true);
}

void mode1SuaraAction(bool force)
{
  // Mengeluarkan suara mode
  switch (mode1)
  {
  case 1:
    playMusicDfplayer(1, force);
    break;
  case 2:
    playMusicDfplayer(2, force);
    break;
  case 3:
    playMusicDfplayer(3, force);
    break;
  case 4:
    playMusicDfplayer(4, force);
    break;
  case 5:
    playMusicDfplayer(5, force);
    break;
  case 6:
    playMusicDfplayer(6, force);
    break;

  default:
    break;
  }
  // mode2SuaraAction();
}

void mode2SuaraAction(bool force)
{
  if (mode1 == 1 || mode1 == 2 || mode1 == 3)
  {
    // delay(1400);
    switch (mode2)
    {
    case 0:
      playMusicDfplayer(38, force); // Getar diaktifkan
      break;

    case 1:
      playMusicDfplayer(39, force); // Lampu diaktifkan
      break;
    case 2:
      playMusicDfplayer(40, force); // Getar dan Lampu diaktifkan
      break;
    case 3:
      playMusicDfplayer(41, force); // Getar dan Lampu di non-aktifkan
      break;

    default:
      break;
    }
  }
}

void initPins()
{
  pinMode(trig1, OUTPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(echo2, INPUT);
  pinMode(but1, INPUT_PULLUP);
  pinMode(but2, INPUT_PULLUP);
  pinMode(lamp, OUTPUT);
  pinMode(mot, OUTPUT);
}

void initEpprom()
{
  mode1 = EEPROM.read(ADDRESS_EEPROM_MODE1); // original 1
  mode2 = EEPROM.read(ADDRESS_EEPROM_MODE2); // original 2
  // vol = EEPROM.read(139);   // original 3
}

void playMusicDfplayer(int music, bool force)
{
  dfplayer_ready_counter = 0;
  if (force)
  {
    myMP3.playFromMP3Folder(music);
    // delay(300);
    return;
  }
  // delay(1000);
  while (myMP3.isPlaying() || dfplayer_ready_counter < 4)
  {
    read_sens();
  }
  myMP3.playFromMP3Folder(music);
  // delay(300);
}

void initTicker()
{
  tickBlinkLed.start();
}

void tickGlobal()
{
  tickBlinkLed.update();
}

void ledReset()
{
  ledState = false;
  digitalWrite(lamp, ledState);
}

void ledTick()
{
  ledState = !ledState;
  digitalWrite(lamp, ledState);
}