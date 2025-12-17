/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "app/dtmf.h"
#include "app/menu.h"
#include "bitmaps.h"
#include "board.h"
#include "dcs.h"
#include "driver/backlight.h"
#include "driver/bk4819.h"
#include "driver/eeprom.h"
#include "driver/st7565.h"
#include "printf.h"
#include "frequencies.h"
#include "helper/battery.h"
#include "misc.h"
#include "settings.h"
#ifdef ENABLE_FEAT_F4HWN
#include "version.h"
#endif
#include "helper.h"
#include "inputbox.h"
#include "menu.h"
#include "ui.h"
#include "ui/gui.h"

 // Helper bridge to the new GUI drawing primitives (ui/gui.h) so we avoid legacy
 // framebuffer helpers from ui/helper.h in this module.
static inline u8g2_uint_t MENU_LineToY(uint8_t line)
{
    // Align text baselines roughly to the old line-based layout (8px spacing).
    return 6 + (line * 8);
}

static void MENU_DrawString(const char* text, uint8_t start, uint8_t end, uint8_t line, bool bold)
{
    UI_SetFont(bold ? FONT_8B_TR : FONT_8_TR);
    UI_DrawString(UI_TEXT_ALIGN_LEFT, start, end, MENU_LineToY(line), true, false, false, text);
}

static void MENU_DrawStringSmall(const char* text, uint8_t start, uint8_t end, uint8_t line)
{
    UI_SetFont(FONT_8_TR);
    UI_DrawString(UI_TEXT_ALIGN_LEFT, start, end, MENU_LineToY(line), true, false, false, text);
}

// Redirect legacy helpers to GUI-backed implementations locally in this file.
//#define UI_PrintString(str, start, end, line, width) MENU_DrawString(str, start, end, line, false)
//#define UI_PrintStringSmallNormal(str, start, end, line) MENU_DrawStringSmall(str, start, end, line)
const t_menu_item MenuList[] =
{
    //   text,          menu ID
        {"SQL",     MENU_SQL           },
        {"STEP ",   MENU_STEP          },
        {"POWER",   MENU_TXP           }, // was "TXP"
        {"RXDCS",   MENU_R_DCS         }, // was "R_DCS"
        {"RXCTCS",  MENU_R_CTCS        }, // was "R_CTCS"
        {"TXDCS",   MENU_T_DCS         }, // was "T_DCS"
        {"TXCTCS",  MENU_T_CTCS        }, // was "T_CTCS"
        {"TXODIR",  MENU_SFT_D         }, // was "SFT_D"
        {"TXOFFS",  MENU_OFFSET        }, // was "OFFSET"
        {"W/N",     MENU_W_N           },
    #ifndef ENABLE_FEAT_F4HWN
        {"SCRAMB",  MENU_SCR           }, // was "SCR"
    #endif
        {"BUSYCL",  MENU_BCL           }, // was "BCL"
        {"COMPND",  MENU_COMPAND       },
        {"MODE",    MENU_AM            }, // was "AM"
    #ifdef ENABLE_FEAT_F4HWN
        {"TXLOCK",  MENU_TX_LOCK       },
    #endif
        {"SCADD1",  MENU_S_ADD1        },
        {"SCADD2",  MENU_S_ADD2        },
        {"SCADD3",  MENU_S_ADD3        },
        {"CHSAVE",  MENU_MEM_CH        }, // was "MEM-CH"
        {"CHDELE",  MENU_DEL_CH        }, // was "DEL-CH"
        {"CHNAME",  MENU_MEM_NAME      },

        {"SLIST",   MENU_S_LIST        },
        {"SLIST1",  MENU_SLIST1        },
        {"SLIST2",  MENU_SLIST2        },
        {"SLIST3",  MENU_SLIST3        },
        {"SCNREV",  MENU_SC_REV        },
    #ifndef ENABLE_FEAT_F4HWN
        #ifdef ENABLE_NOAA
            {"NOAA-S",  MENU_NOAA_S    },
        #endif
    #endif
        {"F1SHRT",  MENU_F1SHRT        },
        {"F1LONG",  MENU_F1LONG        },
        {"F2SHRT",  MENU_F2SHRT        },
        {"F2LONG",  MENU_F2LONG        },
        {"MLONG",   MENU_MLONG         },

        {"KEYLCK",  MENU_AUTOLK        }, // was "AUTOLk"
        {"TXTOUT",  MENU_TOT           }, // was "TOT"
        {"BATSAV",  MENU_SAVE          }, // was "SAVE"
        //{"BATTXT",  MENU_BAT_TXT       },
        {"MIC",     MENU_MIC           },
        {"MICBAR",  MENU_MIC_BAR       },
        {"CHDISP",  MENU_MDF           }, // was "MDF"
        //{"PONMSG",  MENU_PONMSG        },
        {"BLTIME",  MENU_ABR           }, // was "ABR"
        {"BLMIN",   MENU_ABR_MIN       },
        {"BLMAX",   MENU_ABR_MAX       },
        {"BLTXRX",  MENU_ABR_ON_TX_RX  },
        {"BEEP",    MENU_BEEP          },
    #ifdef ENABLE_VOICE
        {"VOICE",   MENU_VOICE         },
    #endif
        {"ROGER",   MENU_ROGER         },
        {"STE",     MENU_STE           },
        {"RP STE",  MENU_RP_STE        },
        {"1 CALL",  MENU_1_CALL        },
    #ifdef ENABLE_ALARM
        {"ALARM T", MENU_AL_MOD        },
    #endif
    #ifdef ENABLE_DTMF_CALLING
        {"ANI ID",  MENU_ANI_ID        },
    #endif
        {"UPCODE",  MENU_UPCODE        },
        {"DWCODE",  MENU_DWCODE        },
        {"PTT ID",  MENU_PTT_ID        },
        {"D ST",    MENU_D_ST          },
    #ifdef ENABLE_DTMF_CALLING
        {"D RESP",  MENU_D_RSP         },
        {"D HOLD",  MENU_D_HOLD        },
    #endif
        {"D PREL",  MENU_D_PRE         },
    #ifdef ENABLE_DTMF_CALLING
        {"D DECD",  MENU_D_DCD         },
        {"D LIST",  MENU_D_LIST        },
    #endif
        {"D LIVE",  MENU_D_LIVE_DEC    }, // live DTMF decoder
    #ifndef ENABLE_FEAT_F4HWN
        #ifdef ENABLE_AM_FIX
            {"AM FIX",  MENU_AM_FIX        },
        #endif
    #endif
        {"VOX",     MENU_VOX           },
    #ifdef ENABLE_FEAT_F4HWN
        {"SYSINF",  MENU_VOL           }, // was "VOL"
    #else
        {"BATVOL",  MENU_VOL           }, // was "VOL"
    #endif
        {"RXMODE",  MENU_TDR           },
    #ifdef ENABLE_FEAT_F4HWN
        {"SETPWR",  MENU_SET_PWR       },
        {"SETPTT",  MENU_SET_PTT       },
        {"SETTOT",  MENU_SET_TOT       },
        {"SETEOT",  MENU_SET_EOT       },
        {"SETCTR",  MENU_SET_CTR       },
        //{"SETINV",  MENU_SET_INV       },
        {"SETLCK",  MENU_SET_LCK       },
        {"SETMET",  MENU_SET_MET       },
        //{"SETGUI",  MENU_SET_GUI       },
        {"SETTMR",  MENU_SET_TMR       },
    #ifdef ENABLE_FEAT_F4HWN_SLEEP
        {"SETOFF",  MENU_SET_OFF      },
    #endif
    #ifdef ENABLE_FEAT_F4HWN_NARROWER
        {"SETNFM",  MENU_SET_NFM       },
    #endif
    #ifdef ENABLE_FEAT_F4HWN_VOL
        {"SETVOL",  MENU_SET_VOL       },
    #endif
    #ifdef ENABLE_FEAT_F4HWN_RESCUE_OPS
        {"SETKEY",  MENU_SET_KEY       },
    #endif
    #ifdef ENABLE_NOAA
        {"SETNWR",  MENU_NOAA_S    },
    #endif
    #endif
        // hidden menu items from here on
        // enabled if pressing both the PTT and upper side button at power-on
        {"F LOCK",  MENU_F_LOCK        },
    #ifndef ENABLE_FEAT_F4HWN
        {"TX 200",  MENU_200TX         }, // was "200TX"
        {"TX 350",  MENU_350TX         }, // was "350TX"
        {"TX 500",  MENU_500TX         }, // was "500TX"
    #endif
        {"350 EN",  MENU_350EN         }, // was "350EN"
    #ifndef ENABLE_FEAT_F4HWN
        {"SCRAEN",  MENU_SCREN         }, // was "SCREN"
    #endif
    #ifdef ENABLE_F_CAL_MENU
        {"FRCALI",  MENU_F_CALI        }, // reference xtal calibration
    #endif
        {"BATCAL",  MENU_BATCAL        }, // battery voltage calibration
        {"BATTYP",  MENU_BATTYP        }, // battery type 1600/2200mAh
        {"RESET",   MENU_RESET         }, // might be better to move this to the hidden menu items ?

        {"",                              0xff               }  // end of list - DO NOT delete or move this this
};

const uint8_t FIRST_HIDDEN_MENU_ITEM = MENU_F_LOCK;

const char gSubMenu_TXP[][6] =
{
 "USER",
 "LOW 1",
 "LOW 2",
 "LOW 3",
 "LOW 4",
 "LOW 5",
 "MID",
 "HIGH"
};

const char gSubMenu_SFT_D[][4] =
{
 "OFF",
 "+",
 "-"
};

const char gSubMenu_W_N[][7] =
{
 "WIDE",
 "NARROW"
};

const char gSubMenu_OFF_ON[][4] =
{
 "OFF",
 "ON"
};

const char gSubMenu_NA[4] =
{
 "N/A"
};

const char* const gSubMenu_RXMode[] =
{
 "MAIN\nONLY",       // TX and RX on main only
 "DUAL RX\nRESPOND", // Watch both and respond
 "CROSS\nBAND",      // TX on main, RX on secondary
 "MAIN TX\nDUAL RX"  // always TX on main, but RX on both
};

#ifdef ENABLE_VOICE
const char gSubMenu_VOICE[][4] =
{
 "OFF",
 "CHI",
 "ENG"
};
#endif

const char* const gSubMenu_MDF[] =
{
 "FREQ",
 "CHANNEL\nNUMBER",
 "NAME",
 "NAME\n+\nFREQ"
};

#ifdef ENABLE_ALARM
const char gSubMenu_AL_MOD[][5] =
{
 "SITE",
 "TONE"
};
#endif

#ifdef ENABLE_DTMF_CALLING
const char gSubMenu_D_RSP[][11] =
{
 "DO\nNOTHING",
 "RING",
 "REPLY",
 "BOTH"
};
#endif

const char* const gSubMenu_PTT_ID[] =
{
 "OFF",
 "UP CODE",
 "DOWN CODE",
 "UP+DOWN\nCODE",
 "APOLLO\nQUINDAR"
};

const char gSubMenu_PONMSG[][8] =
{
#ifdef ENABLE_FEAT_F4HWN
 "ALL",
 "SOUND",
#else
 "FULL",
#endif
 "MESSAGE",
 "VOLTAGE",
 "NONE"
};

const char gSubMenu_ROGER[][6] =
{
 "OFF",
 "ROGER",
 "MDC"
};

const char gSubMenu_RESET[][4] =
{
 "VFO",
 "ALL"
};

const char* const gSubMenu_F_LOCK[] =
{
 "DEFAULT+\n137-174\n400-470",
 "FCC HAM\n144-148\n420-450",
#ifdef ENABLE_FEAT_F4HWN_CA
 "CA HAM\n144-148\n430-450",
#endif
 "CE HAM\n144-146\n430-440",
 "GB HAM\n144-148\n430-440",
 "137-174\n400-430",
 "137-174\n400-438",
#ifdef ENABLE_FEAT_F4HWN_PMR
 "PMR 446",
#endif
#ifdef ENABLE_FEAT_F4HWN_GMRS_FRS_MURS
 "GMRS\nFRS\nMURS",
#endif
 "DISABLE\nALL",
 "UNLOCK\nALL",
};

const char gSubMenu_RX_TX[][6] =
{
 "OFF",
 "TX",
 "RX",
 "TX/RX"
};

const char gSubMenu_BAT_TXT[][8] =
{
 "NONE",
 "VOLTAGE",
 "PERCENT"
};

const char gSubMenu_BATTYP[][9] =
{
 "1600mAh",
 "2200mAh",
 "3500mAh"
};

#ifndef ENABLE_FEAT_F4HWN
const char gSubMenu_SCRAMBLER[][7] =
{
 "OFF",
 "2600Hz",
 "2700Hz",
 "2800Hz",
 "2900Hz",
 "3000Hz",
 "3100Hz",
 "3200Hz",
 "3300Hz",
 "3400Hz",
 "3500Hz"
};
#endif

#ifdef ENABLE_FEAT_F4HWN
const char gSubMenu_SET_PWR[][6] =
{
 "< 20m",
 "125m",
 "250m",
 "500m",
 "1",
 "2",
 "5"
};

const char gSubMenu_SET_PTT[][8] =
{
 "CLASSIC",
 "ONEPUSH"
};

const char gSubMenu_SET_TOT[][7] =  // Use by SET_EOT too
{
 "OFF",
 "SOUND",
 "VISUAL",
 "ALL"
};

const char gSubMenu_SET_LCK[][9] =
{
 "KEYS",
 "KEYS+PTT"
};

const char gSubMenu_SET_MET[][8] =
{
 "TINY",
 "CLASSIC"
};

#ifdef ENABLE_FEAT_F4HWN_NARROWER
const char gSubMenu_SET_NFM[][9] =
{
 "NARROW",
 "NARROWER"
};
#endif

#ifdef ENABLE_FEAT_F4HWN_RESCUE_OPS
const char gSubMenu_SET_KEY[][9] =
{
 "KEY_MENU",
 "KEY_UP",
 "KEY_DOWN",
 "KEY_EXIT",
 "KEY_STAR"
};
#endif
#endif

const t_sidefunction gSubMenu_SIDEFUNCTIONS[] =
{
    {"NONE",            ACTION_OPT_NONE},
#ifdef ENABLE_FLASHLIGHT
    {"FLASH\nLIGHT",    ACTION_OPT_FLASHLIGHT},
#endif
    {"POWER",           ACTION_OPT_POWER},
    {"MONITOR",         ACTION_OPT_MONITOR},
    {"SCAN",            ACTION_OPT_SCAN},
#ifdef ENABLE_VOX
    {"VOX",             ACTION_OPT_VOX},
#endif
#ifdef ENABLE_ALARM
    {"ALARM",           ACTION_OPT_ALARM},
#endif
#ifdef ENABLE_FMRADIO
    {"FM RADIO",        ACTION_OPT_FM},
#endif
#ifdef ENABLE_TX1750
    {"1750Hz",          ACTION_OPT_1750},
#endif
#ifdef ENABLE_REGA
    {"REGA\nALARM",     ACTION_OPT_REGA_ALARM},
    {"REGA\nTEST",      ACTION_OPT_REGA_TEST},
#endif
    {"LOCK\nKEYPAD",    ACTION_OPT_KEYLOCK},
    {"VFO A\nVFO B",    ACTION_OPT_A_B},
    {"VFO\nMEM",        ACTION_OPT_VFO_MR},
    {"MODE",            ACTION_OPT_SWITCH_DEMODUL},
#ifdef ENABLE_BLMIN_TMP_OFF
    {"BLMIN\nTMP OFF",  ACTION_OPT_BLMIN_TMP_OFF},      //BackLight Minimum Temporay OFF
#endif
#ifdef ENABLE_FEAT_F4HWN
    {"RX MODE",         ACTION_OPT_RXMODE},
    {"MAIN ONLY",       ACTION_OPT_MAINONLY},
    {"PTT",             ACTION_OPT_PTT},
    {"WIDE\nNARROW",    ACTION_OPT_WN},
    #if !defined(ENABLE_SPECTRUM) || !defined(ENABLE_FMRADIO)
    {"MUTE",            ACTION_OPT_MUTE},
    #endif
    #ifdef ENABLE_FEAT_F4HWN_RESCUE_OPS
        {"POWER\nHIGH",    ACTION_OPT_POWER_HIGH},
        {"REMOVE\nOFFSET",  ACTION_OPT_REMOVE_OFFSET},
    #endif
#endif
};

const uint8_t gSubMenu_SIDEFUNCTIONS_size = ARRAY_SIZE(gSubMenu_SIDEFUNCTIONS);

bool    gIsInSubMenu;
uint8_t gMenuCursor;

int UI_MENU_GetCurrentMenuId() {
    if (gMenuCursor < ARRAY_SIZE(MenuList))
        return MenuList[gMenuCursor].menu_id;

    return MenuList[ARRAY_SIZE(MenuList) - 1].menu_id;
}

uint8_t UI_MENU_GetMenuIdx(uint8_t id)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(MenuList); i++)
        if (MenuList[i].menu_id == id)
            return i;
    return 0;
}

int32_t gSubMenuSelection;

// edit box
char    edit_original[17]; // a copy of the text before editing so that we can easily test for changes/difference
char    edit[17];
int     edit_index;


UI_SelectionList menuList;
UI_SelectionList subMenuList;

static const char* UI_MENU_GetMenuListLines(void)
{
    static char s_menuLines[ARRAY_SIZE(MenuList) * 12] = { 0 }; // 10 chars + '\n' + '\0' headroom

    size_t out = 0;
    const size_t cap = sizeof(s_menuLines);

    for (uint8_t i = 0; i < gMenuListCount && MenuList[i].name[0] != '\0'; i++) {
        const char* name = MenuList[i].name;
        const size_t len = strnlen(name, sizeof(MenuList[i].name));

        if (len == 0) {
            continue;
        }

        if (out + len + 1 >= cap) {
            break;
        }

        memcpy(&s_menuLines[out], name, len);
        out += len;

        if ((i + 1) < gMenuListCount && out + 1 < cap) {
            s_menuLines[out++] = '\n';
        }
    }

    s_menuLines[out] = '\0';
    return s_menuLines;
}

static const char* squelchStr = "OFF\n1\n2\n3\n4\n5\n6\n7\n8\n9";

static const char* UI_MENU_JoinFixedList(const char *arr, size_t elem_size, size_t count)
{
    static char buffer[512];
    size_t out = 0;

    for (size_t i = 0; i < count; ++i) {
        const char *item = arr + (i * elem_size);
        const size_t len = strnlen(item, elem_size);

        if (len == 0) {
            continue;
        }

        if (out + len + 1 >= sizeof(buffer)) {
            break;
        }

        memcpy(&buffer[out], item, len);
        out += len;

        if ((i + 1) < count && out + 1 < sizeof(buffer)) {
            buffer[out++] = '\n';
        }
    }

    buffer[out] = '\0';
    return buffer;
}

static const char* UI_MENU_JoinPtrList(const char* const *arr, size_t count)
{
    static char buffer[512];
    size_t out = 0;

    for (size_t i = 0; i < count; ++i) {
        const char *item = arr[i];
        if (item == NULL) {
            continue;
        }
        const size_t len = strlen(item);
        if (len == 0) {
            continue;
        }

        if (out + len + 1 >= sizeof(buffer)) {
            break;
        }

        memcpy(&buffer[out], item, len);
        out += len;

        if ((i + 1) < count && out + 1 < sizeof(buffer)) {
            buffer[out++] = '\n';
        }
    }

    buffer[out] = '\0';
    return buffer;
}

static const char* UI_MENU_JoinSideFunctions(void)
{
    static char buffer[512];
    size_t out = 0;

    for (size_t i = 0; i < gSubMenu_SIDEFUNCTIONS_size; ++i) {
        const char *item = gSubMenu_SIDEFUNCTIONS[i].name;
        const size_t len = strlen(item);

        if (len == 0) {
            continue;
        }

        if (out + len + 1 >= sizeof(buffer)) {
            break;
        }

        memcpy(&buffer[out], item, len);
        out += len;

        if ((i + 1) < gSubMenu_SIDEFUNCTIONS_size && out + 1 < sizeof(buffer)) {
            buffer[out++] = '\n';
        }
    }

    buffer[out] = '\0';
    return buffer;
}

static const char* UI_MENU_GetOptionLinesForId(int menuId)
{
    switch (menuId)
    {
        case MENU_SQL:
            return squelchStr;
        case MENU_TXP:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_TXP, sizeof(gSubMenu_TXP[0]), ARRAY_SIZE(gSubMenu_TXP));
        case MENU_SFT_D:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SFT_D, sizeof(gSubMenu_SFT_D[0]), ARRAY_SIZE(gSubMenu_SFT_D));
        case MENU_W_N:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_W_N, sizeof(gSubMenu_W_N[0]), ARRAY_SIZE(gSubMenu_W_N));
#ifndef ENABLE_FEAT_F4HWN
        case MENU_SCR:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SCRAMBLER, sizeof(gSubMenu_SCRAMBLER[0]), ARRAY_SIZE(gSubMenu_SCRAMBLER));
#endif
        case MENU_BCL:
        case MENU_COMPAND:
        case MENU_MIC_BAR:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_OFF_ON, sizeof(gSubMenu_OFF_ON[0]), ARRAY_SIZE(gSubMenu_OFF_ON));
        case MENU_TDR:
            return UI_MENU_JoinPtrList(gSubMenu_RXMode, ARRAY_SIZE(gSubMenu_RXMode));
#ifdef ENABLE_VOICE
        case MENU_VOICE:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_VOICE, sizeof(gSubMenu_VOICE[0]), ARRAY_SIZE(gSubMenu_VOICE));
#endif
        case MENU_MDF:
            return UI_MENU_JoinPtrList(gSubMenu_MDF, ARRAY_SIZE(gSubMenu_MDF));
        case MENU_ROGER:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_ROGER, sizeof(gSubMenu_ROGER[0]), ARRAY_SIZE(gSubMenu_ROGER));
        case MENU_PONMSG:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_PONMSG, sizeof(gSubMenu_PONMSG[0]), ARRAY_SIZE(gSubMenu_PONMSG));
        case MENU_RESET:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_RESET, sizeof(gSubMenu_RESET[0]), ARRAY_SIZE(gSubMenu_RESET));
        case MENU_F_LOCK:
            return UI_MENU_JoinPtrList(gSubMenu_F_LOCK, ARRAY_SIZE(gSubMenu_F_LOCK));
        case MENU_BAT_TXT:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_BAT_TXT, sizeof(gSubMenu_BAT_TXT[0]), ARRAY_SIZE(gSubMenu_BAT_TXT));
        case MENU_BATTYP:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_BATTYP, sizeof(gSubMenu_BATTYP[0]), ARRAY_SIZE(gSubMenu_BATTYP));
#ifdef ENABLE_ALARM
        case MENU_AL_MOD:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_AL_MOD, sizeof(gSubMenu_AL_MOD[0]), ARRAY_SIZE(gSubMenu_AL_MOD));
#endif
#ifdef ENABLE_DTMF_CALLING
        case MENU_D_RSP:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_D_RSP, sizeof(gSubMenu_D_RSP[0]), ARRAY_SIZE(gSubMenu_D_RSP));
#endif
        case MENU_PTT_ID:
            return UI_MENU_JoinPtrList(gSubMenu_PTT_ID, ARRAY_SIZE(gSubMenu_PTT_ID));
        case MENU_SC_REV:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_OFF_ON, sizeof(gSubMenu_OFF_ON[0]), ARRAY_SIZE(gSubMenu_OFF_ON));
        case MENU_BEEP:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_OFF_ON, sizeof(gSubMenu_OFF_ON[0]), ARRAY_SIZE(gSubMenu_OFF_ON));
        case MENU_SAVE:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_OFF_ON, sizeof(gSubMenu_OFF_ON[0]), ARRAY_SIZE(gSubMenu_OFF_ON));
#ifdef ENABLE_FEAT_F4HWN
        case MENU_SET_PWR:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SET_PWR, sizeof(gSubMenu_SET_PWR[0]), ARRAY_SIZE(gSubMenu_SET_PWR));
        case MENU_SET_PTT:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SET_PTT, sizeof(gSubMenu_SET_PTT[0]), ARRAY_SIZE(gSubMenu_SET_PTT));
        case MENU_SET_TOT:
        case MENU_SET_EOT:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SET_TOT, sizeof(gSubMenu_SET_TOT[0]), ARRAY_SIZE(gSubMenu_SET_TOT));
        case MENU_SET_LCK:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SET_LCK, sizeof(gSubMenu_SET_LCK[0]), ARRAY_SIZE(gSubMenu_SET_LCK));
        case MENU_SET_MET:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SET_MET, sizeof(gSubMenu_SET_MET[0]), ARRAY_SIZE(gSubMenu_SET_MET));
        case MENU_SET_CTR:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SET_MET, sizeof(gSubMenu_SET_MET[0]), ARRAY_SIZE(gSubMenu_SET_MET));
#ifdef ENABLE_FEAT_F4HWN_NARROWER
        case MENU_SET_NFM:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SET_NFM, sizeof(gSubMenu_SET_NFM[0]), ARRAY_SIZE(gSubMenu_SET_NFM));
#endif
#ifdef ENABLE_FEAT_F4HWN_RESCUE_OPS
        case MENU_SET_KEY:
            return UI_MENU_JoinFixedList((const char *)gSubMenu_SET_KEY, sizeof(gSubMenu_SET_KEY[0]), ARRAY_SIZE(gSubMenu_SET_KEY));
#endif
#endif
        case MENU_F1SHRT:
        case MENU_F1LONG:
        case MENU_F2SHRT:
        case MENU_F2LONG:
        case MENU_MLONG:
            return UI_MENU_JoinSideFunctions();
        default:
            break;
    }

    static char numeric[12];
    snprintf(numeric, sizeof(numeric), "%ld", (long)gSubMenuSelection);
    return numeric;
}

const char* getCurrentOption() {
    const char* lines = UI_MENU_GetOptionLinesForId(UI_MENU_GetCurrentMenuId());
    return UI_GetStrValue(lines, gSubMenuSelection);
}

void setSubMenu(void) {
    const char* lines = UI_MENU_GetOptionLinesForId(UI_MENU_GetCurrentMenuId());
    const uint8_t total = u8x8_GetStringLineCnt(lines);
    const uint8_t visible = MIN(total, 5);
    UI_SelectionList_Set(&subMenuList, gSubMenuSelection, visible > 0 ? visible : 1, 122, lines, NULL);
}

void UI_DisplayMenu(void)
{
    // clear the screen
    UI_ClearDisplay();
    // redraw the menu
    UI_SetBlackColor();

    UI_DrawBox(0, 0, 128, 7);

    UI_SetFont(FONT_8B_TR);
    UI_DrawString(UI_TEXT_ALIGN_LEFT, 2, 0, 6, false, false, false, "MENU");
    UI_DrawStringf(UI_TEXT_ALIGN_RIGHT, 0, 126, 6, false, false, false, "%02u / %02u", 1 + gMenuCursor, gMenuListCount);

    UI_SetBlackColor();
    UI_SelectionList_SetCurrentPos(&menuList, gMenuCursor);

    UI_SelectionList_Draw(&menuList, 15, getCurrentOption());

    if (gIsInSubMenu) {
        // gSubMenuSelection
        UI_DrawPopupWindow(36, 6, 90, 52, UI_SelectionList_GetStringLine(&menuList));

        setSubMenu();
        
        UI_SelectionList_Draw(&subMenuList, 20, NULL);
    }

    UI_UpdateDisplay();
}

void UI_MenuInit(void)
{
    UI_SelectionList_Init(&menuList);
    UI_SelectionList_Set(&menuList, gMenuCursor, 6, 127, UI_MENU_GetMenuListLines(), NULL);

    UI_SelectionList_Init(&subMenuList);
    //UI_SelectionList_SetMaxWidth(&subMenuList, 90);
    UI_SelectionList_SetShowLineNumbers(&subMenuList, false);
    UI_SelectionList_SetStartXPos(&subMenuList, 40);
}
