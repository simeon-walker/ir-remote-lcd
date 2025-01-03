/*
 * Specify which protocol(s) should be used for decoding.
 * If no protocol is defined, all protocols are active.
 */
// #define DECODE_DENON        // Includes Sharp
// #define DECODE_JVC
// #define DECODE_KASEIKYO
#define DECODE_PANASONIC    // the same as DECODE_KASEIKYO
// #define DECODE_LG
#define DECODE_NEC  // Includes Apple and Onkyo
#define DECODE_SAMSUNG
#define DECODE_SONY
#define DECODE_RC5
#define DECODE_RC6
#define DECODE_RC6A
//#define DECODE_BOSEWAVE
//#define DECODE_LEGO_PF
//#define DECODE_MAGIQUEST
//#define DECODE_WHYNTER
//#define DECODE_HASH         // special decoder for all protocols

#include <Arduino.h>

#include <IRremote.hpp>
#define IR_RECEIVE_PIN 2

// #include <Wire.h>
#include <LCD_I2C.h>

#define RELAY_PIN 3

// 20x4 displat at address 0x27
LCD_I2C lcd(0x27, 20, 4);

void lcd_setup() {
  lcd.begin();
  lcd.backlight();
  lcd.home();
  lcd.cursor();
  lcd.print(F("Using IR at pin "));
  lcd.print(IR_RECEIVE_PIN);
}

void setup() {
  Serial.begin(115200);
  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  /*
   * Start the receiver, enable feedback LED and take LED feedback pin from the internal boards definition
   */
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);

  Serial.print(F("Using IR at pin "));
  Serial.println(IR_RECEIVE_PIN);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  lcd_setup();
}


void loop() {
  /*
   * Check if received data is available and if yes, try to decode it.
   * Decoded result is in the IrReceiver.decodedIRData structure.
   *
   * E.g. command is in IrReceiver.decodedIRData.command
   * address is in command is in IrReceiver.decodedIRData.address
   * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
   */
  if (IrReceiver.decode()) {
    // Print a short summary of received data
    
    IrReceiver.printIRResultAsCVariables(&Serial);
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      // We have an unknown protocol here, print more info
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    Serial.println();

    /*
     * !!!Important!!! Enable receiving of the next value,
     * since receiving has stopped after the end of the current received data packet.
     */
    IrReceiver.resume();  // Enable receiving of the next value

    /*
     * Finally, check the received data and perform actions according to the received command
     */
    lcd.setCursor(0, 1);
    lcd.print(F("Proto:              "));
    lcd.setCursor(7, 1);
    lcd.print(getProtocolString(IrReceiver.decodedIRData.protocol));
    lcd.setCursor(0, 2);
    lcd.print(F("Addr:      Cmd:     "));
    lcd.setCursor(6, 2);
    lcd.print(IrReceiver.decodedIRData.address, HEX);
    lcd.setCursor(16, 2);
    lcd.print(IrReceiver.decodedIRData.command, HEX);

    if (IrReceiver.decodedIRData.protocol == NEC) {
      if (IrReceiver.decodedIRData.address == 0) {
        if (IrReceiver.decodedIRData.command == 0x40) {
          // Turn on relay
          Serial.println(F("Relay on "));
          digitalWrite(RELAY_PIN, LOW);
          lcd.setCursor(0, 3);
          lcd.print(F("Relay on "));

        } else if (IrReceiver.decodedIRData.command == 0x19) {
          // Turn off relay
          Serial.println(F("Relay off"));
          digitalWrite(RELAY_PIN, HIGH);
          lcd.setCursor(0, 3);
          lcd.print(F("Relay off"));
        }
      }
    }
  }
}
