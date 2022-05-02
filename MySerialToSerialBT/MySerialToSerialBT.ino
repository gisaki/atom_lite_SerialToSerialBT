//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

// スケッチ例の以下をカスタマイズ
// ファイル > スケッチ例 > BluetoothSerial > SerialToSerialBT

#include "M5Atom.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define TXPIN  (19)
#define RXPIN  (23)

// Atom Lite
const bool SerialEnable = true;
const bool I2CEnable = false;
const bool LEDEnable = true;

#define LED_YELLOW (0xfff000)
#define LED_RED    (0xff0000)
#define LED_BLUE   (0x0000f0)
#define LED_GREEN  (0x00ff00)

BluetoothSerial SerialBT;

void setup() {
  M5.begin(SerialEnable, I2CEnable, LEDEnable); // モデルごとに引数が異なるので注意（調べること）// M5Stack, M5Stick, ATOM, 等々  
  delay(50);   //delay 50ms.
  M5.dis.drawpix(0, LED_RED);    //Light the LED with the specified RGB color 00ff00(Atom-Matrix has only one light).
  
  Serial.begin(115200);
  while(!Serial) {};
  Serial2.begin(115200, SERIAL_8N1, RXPIN, TXPIN);
  while(!Serial2) {};
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

int8_t MyMode = -1;     //-1: undef, 0(default): UART Serial <-> Bluetooth, 1: USB Serial <-> Bluetooth, 
void loop() {

  // 起動時のみ数秒の間、ボタン状態を判断できる
  if (MyMode < 0){
    delay(2000);
    M5.update();    //Read the press state of the key.
    
    // Check if the key is pressed.
    MyMode = (M5.Btn.wasPressed())? 1: 0;
    delay(50);
    M5.update();    //Read the press state of the key.
  
    M5.dis.drawpix(0, (MyMode == 0)? LED_BLUE: LED_GREEN); //Light the LED with the specified RGB color 00ff00(Atom-Matrix has only one light).
    return;
  }

  // できる限り読みこぼしがないように、
  // 送受信が連続している間は
  // 続けて受信／送信させる
  // なお受信バッファは64byteとのこと、それよりも多くループさせている
  uint16_t cnt = 0;
  bool continuous = true;

  while (continuous && (cnt < 1024)) {
    continuous = false;
    cnt = cnt + 1;
    
    // UART Serial <-> Bluetooth
    if (MyMode == 0) {
      if (Serial2.available()) {
        SerialBT.write(Serial2.read());
        continuous = true;
      }
      if (SerialBT.available()) {
        Serial2.write(SerialBT.read());
        continuous = true;
      }
    }
    // USB Serial <-> Bluetooth
    if (MyMode == 1) {
      if (Serial.available()) {
        SerialBT.write(Serial.read());
        continuous = true;
      }
      if (SerialBT.available()) {
        Serial.write(SerialBT.read());
        continuous = true;
      }
    }
    
  } // while
  
  delay(1); // サンプルプログラムが20だった
}
