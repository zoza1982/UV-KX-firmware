# UV-KX Firmware — Bug Tracker

Generated: 2026-01-30 | Source: Code review + bug hunt audit

---

## CRITICAL

- [x] **C-1** `driver/i2c.c:140` — `I2C_ReadBuffer(Size=0)` uint8_t underflow → 256-byte buffer overwrite. `Size - 1` wraps to 255 when Size=0. **Fix:** Add `if (Size == 0) return 0;` guard. *(PR #1)*

- [x] **C-2** `app/uart.c:527` — UART DMA ring buffer SMS parsing accesses `[idx+1..+3]` without `DMA_INDEX()` wraparound → out-of-bounds read when index is near 253-255. **Fix:** Use `DMA_INDEX()` macro for all buffer accesses; copy to linear buffer before parsing. *(PR #2)*

- [x] **C-3** `driver/uart.c:125` — `UART_printf` 256-byte stack buffer + unclamped `vsnprintf` return value → `UART_Send` reads past stack buffer. **Fix:** Clamp: `if (len > (int)sizeof(text) - 1) len = sizeof(text) - 1;` *(PR #3)*

- [ ] **C-4** `app/messenger.c:622,629` — Null terminator at `msgFSKBuffer[MAX_RX_MSG_LENGTH-1]` overwrites last char of max-length (30-byte) messages. Station ID copy at offset 32 is fine but message is silently truncated. **Fix:** Adjust offsets or document the 29-char effective limit. *(Skipped — needs design discussion)*

- [x] **C-5** `app/uart.c:291` — UART EEPROM read: `pCmd->Size` not validated against 128-byte `Reply.Data.Data` → stack smash via serial protocol. **Fix:** Add `if (pCmd->Size > sizeof(Reply.Data.Data)) return;` *(PR #4)*

---

## HIGH

- [x] **H-1** `radio/radio.c:403` — Always-false condition (`freq >= X && freq < X`) defeats airband TX-offset protection. Regulatory safety issue. **Fix:** Change first `.upper` to `.lower`. *(PR #5)*

- [x] **H-2** `radio/radio.c:663` — `uint32_t Frequency -= TX_OFFSET` underflow → could configure TX on unintended frequency. **Fix:** Add `if (pInfo->TX_OFFSET_FREQUENCY > Frequency) Frequency = 0;` or clamp. *(PR #6)*

- [x] **H-3** `driver/eeprom.c:54` — All I2C return values ignored in EEPROM read/write → silent data corruption on bus errors. **Fix:** Check return values of `I2C_Write()`, retry or flag error. *(PR #13)*

- [x] **H-4** `app/messenger.c:754` — Delivery notification appends `'+'` without bounds check → unterminated string, display garbage. **Fix:** Check `strlen() < sizeof() - 1` before appending, null-terminate after. *(PR #14)*

- [x] **H-5** `ui/menu.c:1213` — `SETTINGS_FetchChannelName(NULL, ...)` null pointer dereference; channel names never display in MEM_CH menu. **Fix:** Declare `char name[11];` as stack array, check `name[0] != '\0'`. *(PR #7)*

- [x] **H-6** `helper/battery.c:98` — Division `(crv[i-1][0] - crv[i][0])` is zero if two adjacent voltage table entries are equal → div-by-zero. **Fix:** Add guard `if (crv[i-1][0] == crv[i][0]) continue;` *(PR #8)*

---

## MEDIUM

- [x] **M-1** `radio/frequencies.c:163` — TX power interpolation uses `TxpMid +=` instead of `TxpMid =` → power output higher than calibrated. **Fix:** Line 163: `TxpMid = TxpLow + ((TxpMid - TxpLow) * (Frequency - LowerLimit)) / (Middle - LowerLimit);` Line 167: `TxpMid = TxpMid + ((TxpHigh - TxpMid) * (Frequency - Middle)) / (UpperLimit - Middle);` *(PR #9)*

- [x] **M-2** ~~`app/messenger.c:47,58` — `msgStatus` and `hasNewMessage` modified in interrupt context but not `volatile`.~~ **False positive:** All access is from main loop polling, not from ISR.

- [ ] **M-3** `driver/bk4819.c` — SPI bit-bang has no mutual exclusion; SysTick interrupt calls BK4819 register functions → SPI bus corruption mid-transaction. **Fix:** Disable interrupts around SPI transactions, or defer interrupt-context BK4819 access to main loop. *(Skipped — architectural change needed)*

- [x] **M-4** `app/spectrum.c:457` — `GetRssi()` busy-waits on BK4819 register without timeout → radio hangs if hardware locks. **Fix:** Add iteration counter, bail after N attempts. *(PR #15)*

- [x] **M-5** `radio/radio.c:116` — `scanList=0` causes `SCANLIST_PRIORITY_CH[scanList-1]` → index 255 out-of-bounds. **Fix:** Add `if (scanList == 0) return true;` before line 116. *(PR #10)*

- [x] **M-6** `app/spectrum.c:831` — `freqInputIndex--` without underflow guard when index is 0 on KEY_EXIT. **Fix:** Add `if (freqInputIndex == 0) return;` before decrement. *(PR #11)*

- [x] **M-7** ~~`radio/radio.c:557` — `currentPower--` can underflow for OUTPUT_POWER_USER (value 0).~~ **False positive:** OUTPUT_POWER_USER is handled in separate `if` branch before decrement; `else` branch only receives values 1-7.

- [ ] **M-8** `radio/settings.c:510` — Factory reset: 960 sequential EEPROM writes (~7.7s blocking, no radio functions). **Fix:** Consider yielding to main loop periodically or batching. *(Skipped — architectural change needed)*

- [x] **M-9** Multiple files — `sprintf` used without bounds (should be `snprintf`): `app/dtmf.c:265`, `app/spectrum.c:988,991,1114,1119`, `ui/scanner.c:38,49,54`. **Fix:** Replace with `snprintf(buf, sizeof(buf), ...)`. *(PR #12)*

---

## LOW

- [x] **L-1** `app/messenger.c:588` — Unused 30-byte `prefixed` stack buffer (commented-out code). **Fix:** Remove. *(PR #16)*

- [x] **L-2** `app/messenger.c:56` — `gErrorsDuringMSG` declared and extern'd but never written. **Fix:** Remove or implement. *(PR #16)*

- [x] **L-3** `app/messenger.h:26` — Duplicate constant `NEXT_CHAR_DELAY` (dead; only `MSG_NEXT_CHAR_DELAY` used). **Fix:** Remove. *(PR #16)*

- [x] **L-4** `app/uart.c:494` — `remove()` function shadows C stdlib name. **Fix:** Renamed to `str_remove_char()`. *(PR #2)*

- [ ] **L-5** `driver/eeprom.c` — `EEPROM_ReadBuffer` lacks address/null checks (WriteBuffer has them). **Fix:** Add matching guards. *(Partially addressed in H-3 — zero-fill on I2C failure added)*

- [x] **L-6** `helper/battery.c:53` — `gEeprom.BATTERY_TYPE` not bounds-checked before indexing voltage table. **Fix:** Add `if (type >= ARRAY_SIZE(table)) return 0;` *(PR #8)*

- [ ] **L-7** Multiple files — Large commented-out code blocks, magic numbers, mixed coding style. **Fix:** Incremental cleanup. *(Ongoing)*

---

## Progress

| Severity | Total | Fixed | False Positive | Skipped | Remaining |
|----------|-------|-------|----------------|---------|-----------|
| CRITICAL | 5 | 4 | 0 | 1 | 0 |
| HIGH | 6 | 6 | 0 | 0 | 0 |
| MEDIUM | 9 | 6 | 2 | 1 | 0 |
| LOW | 7 | 5 | 0 | 0 | 2 |
| **Total** | **27** | **21** | **2** | **2** | **2** |
