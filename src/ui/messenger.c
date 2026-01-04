
#ifdef ENABLE_MESSENGER

#include <string.h>
#include "app/messenger.h"
#include "printf.h"
#include "misc.h"
#include "settings.h"
#include "ui/gui.h"
#include "ui/messenger.h"

void UI_DisplayMSG(void) {

	//memset(gFrameBuffer, 0, sizeof(gFrameBuffer));
	UI_ClearDisplay();
	UI_SetBlackColor();

	/*if (gEeprom.KEY_LOCK && gKeypadLocked > 0)
	{	// tell user how to unlock the keyboard
		UI_SetFont(FONT_8_TR);
		UI_DrawString(UI_TEXT_ALIGN_CENTER, 0, UI_W, 22, true, false, false, "Long press #");
		UI_DrawString(UI_TEXT_ALIGN_CENTER, 0, UI_W, 38, true, false, false, "to unlock");
		UI_UpdateDisplay();
		return;
	}*/

	UI_SetBlackColor();

    UI_DrawBox(0, 0, 128, 7);
	UI_DrawBox(0, 56, 128, 8);

    UI_SetFont(FONT_8B_TR);
    UI_DrawString(UI_TEXT_ALIGN_LEFT, 2, 0, 6, false, false, false, "MESSENGER");
	UI_DrawString(UI_TEXT_ALIGN_RIGHT, 0, 126, 6, true, true, false, keyboardType == NUMERIC ? "-123-" : "-ABC-");

	/*if ( msgStatus == SENDING ) {
		GUI_DisplaySmallest("SENDING", 100, 6, false, true);
	} else if ( msgStatus == RECEIVING ) {
		GUI_DisplaySmallest("RECEIVING", 100, 6, false, true);
	} else {
		GUI_DisplaySmallest("READY", 100, 6, false, true);
	}*/

	UI_SetFont(FONT_5_TR);

	{
		
		const uint8_t vfo_index = gEeprom.TX_VFO;
		const char vfo_letter = (vfo_index == 0) ? 'A' : 'B';

		if (gLastRxStationIdLen > 0) {
			UI_DrawStringf(UI_TEXT_ALIGN_RIGHT, 0, 126, 55, true, false, false, "%s VFO %c", gLastRxStationId, vfo_letter);
		} else {
			UI_DrawStringf(UI_TEXT_ALIGN_RIGHT, 0, 126, 55, true, false, false, "VFO %c", vfo_letter);
		}
		
	}

	uint8_t mPos = 14;
	const uint8_t mLine = 7;
	for (uint8_t i = 0; i < MAX_LINES; ++i) {
		UI_DrawString(UI_TEXT_ALIGN_LEFT, 2, 0, mPos, true, false, false, rxMessage[i]);
		mPos += mLine;
    }

	UI_SetFont(FONT_8_TR);
	if (MSG_IsChoosingChar()) {
		const uint8_t key = MSG_GetPrevKey();
		const uint8_t sel = MSG_GetPrevLetter();
		char chars[MSG_KEY_CHARS_MAX];
		const uint8_t count = MSG_GetKeyChars(key, chars);
		uint8_t x = 2;
		const uint8_t y = 55;
		//UI_SetFont(FONT_5_TR);
		for (uint8_t i = 0; i < count; ++i) {
			char s[2] = {chars[i], '\0'};
			const bool invert = (i == sel);
			UI_DrawString(UI_TEXT_ALIGN_LEFT, x, 0, y, true, invert, false, s);
			x = (uint8_t)(x + 10);
		}
		//UI_SetFont(FONT_8_TR);
		UI_DrawStringf(UI_TEXT_ALIGN_LEFT, 2, 0, 62, false, false, false, "%s", cMessage);
	} else {
		UI_DrawStringf(UI_TEXT_ALIGN_LEFT, 2, 0, 62, false, false, false, "%s_", cMessage);
	}

	UI_UpdateDisplay();
}

#endif
