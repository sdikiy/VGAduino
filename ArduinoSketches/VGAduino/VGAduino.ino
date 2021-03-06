extern "C" {
#include "config.h"
#include "microrl.h"
#include "flash.h"
}

// create microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

int inByte = 0; // incoming serial byte

void setup() {
  DDRC = 0xFF;
  PORTC = 0x00;

  //ROM Page select L0..L4, 32 pages
  DDRL |= 0x1F;
  PORTL &= 0xE0; // Page 0

  XMCRA |= (1 << SRE); // EMI on

  DDRH |= (1 << 3);
  DDRH |= (1 << 4);
  DDRH |= (1 << 5);
  DDRH |= (1 << 6);

  PORTH |= (1 << 3); //~CE FLASH Hi
  PORTH |= (1 << 4); //~CE RAM Hi
  PORTH &= ~(1 << 5); //~CE ROM Lo
  PORTH |= (1 << 6); //~CE BUF Hi

  DDRD |= (1 << 4);
  DDRD |= (1 << 5);
  DDRD |= (1 << 6);
  DDRD &= ~(1 << 7); // RB_FLASH_IN

  PORTD |= (1 << 4); // WP FLASH Hi
  PORTD &= ~(1 << 5); // ALE FLASH Lo
  PORTD &= ~(1 << 6); // CLE FLASH Lo
  PORTD |= (1 << 7); // RB_FLASH_IN PULL UP

  // start serial port at 115200 bps:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // call init with ptr to microrl instance and print callback
  microrl_init (prl, print);
  // set callback for execute
  microrl_set_execute_callback (prl, execute);
  // set callback for completion
  microrl_set_complete_callback (prl, complet);
  // set callback for Ctrl+C
  microrl_set_sigint_callback (prl, sigint);

  print_help();
}

void loop() {
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    microrl_insert_char (prl, inByte);
  }
}

void print(const char * str) {
  Serial.print(str);
}
