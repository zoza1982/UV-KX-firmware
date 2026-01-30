#ifndef APP_MSG_H
#define APP_MSG_H

#ifdef ENABLE_MESSENGER

#define MSG_NEXT_CHAR_DELAY 100 // 10ms tick

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "driver/keyboard.h"

typedef enum KeyboardType {
	UPPERCASE,
  	NUMERIC,
  	END_TYPE_KBRD
} KeyboardType;

enum { 
	TX_MSG_LENGTH = 30,
	MSG_HEADER_LENGTH = 20,
	MAX_RX_MSG_LENGTH = TX_MSG_LENGTH + 2
};

#define MAX_LINES 6

//const uint8_t TX_MSG_LENGTH = 30;
//const uint8_t MAX_RX_MSG_LENGTH = TX_MSG_LENGTH + 2;

extern char gLastRxStationId[17];
extern uint8_t gLastRxStationIdLen;

extern KeyboardType keyboardType;
extern char cMessage[TX_MSG_LENGTH];
extern char rxMessage[MAX_LINES][MAX_RX_MSG_LENGTH + 2];
extern uint8_t hasNewMessage;
extern uint8_t keyTickCounter;
uint8_t MSG_GetPrevKey(void);
uint8_t MSG_GetPrevLetter(void);
bool MSG_GetLastRxStationId(char *out, size_t out_len);
bool MSG_IsChoosingChar(void);
#define MSG_KEY_CHARS_MAX 4
uint8_t MSG_GetKeyChars(uint8_t key, char out[MSG_KEY_CHARS_MAX]);
void MSG_TimeoutInput(void);

void MSG_EnableRX(const bool enable);
void MSG_StorePacket(const uint16_t interrupt_bits);
void MSG_LoadStationId(void);
void MSG_Init();
void MSG_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld);
void MSG_Send(const char *txMessage, bool bServiceMessage);

#endif

#endif
