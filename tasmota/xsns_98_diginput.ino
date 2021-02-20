/*
  xsns_01_counter.ino - Counter sensors (water meters, electricity meters etc.) sensor support for Tasmota

  Copyright (C) 2020  Maarten Damen and Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_DIGINPUT
/*********************************************************************************************\
 * Counter sensors (water meters, electricity meters etc.)
\*********************************************************************************************/

#define XSNS_98             1

struct DIGINPUT {
  uint8_t present = 0;
  uint8_t inverted_mask = 0xFF;
} DigInput;

void DigInputInit(void)
{
  for (uint32_t i = 0; i < MAX_DIGINPUT; i++) {
    if (PinUsed(GPIO_PG, i)) {
      pinMode(Pin(GPIO_PG, i), INPUT_PULLUP);
      DigInput.present++;
    }
    if (PinUsed(GPIO_FLT, i)) {
      pinMode(Pin(GPIO_FLT, i), INPUT_PULLUP);
      DigInput.present++;      
    } 
  }
}

void DigInputLoop(void)
{
  if (DigInput.present) {
  }
}

void DigInputShowInput(bool json, uint8_t diginput_index, const char *kind, const char *kind_verbose, bool pressed, bool *header) {
  {}
      if (json) {
        if (!(*header)) {
          ResponseAppend_P(PSTR(",\"DIGINPUT\":{"));
        }
        ResponseAppend_P(PSTR("%s\"%s%d\":%d"), (*header)?",":"", kind, diginput_index+1, pressed);
        (*header) = true;
#ifdef USE_DOMOTICZ
        // TODO
#endif  // USE_DOMOTICZ
#ifdef USE_WEBSERVER
      } else {
        WSContentSend_PD(PSTR("{s}%s {m}%s{e}"),
          kind_verbose, pressed ? "Yes" : "No");
#endif  // USE_WEBSERVER
      }
}

void DigInputShow(bool json)
{
  bool header = false;
  bool pressed = false;
  const char *kind, *kind_verbose;

  uint8_t dsxflg = 0;
  for (uint8_t diginput_index = 0; diginput_index < MAX_DIGINPUT; diginput_index++) {
      if (PinUsed(GPIO_PG, diginput_index)) {
          pressed = (digitalRead(Pin(GPIO_PG, diginput_index)) != bitRead(DigInput.inverted_mask, diginput_index));
          kind = "PG";
          kind_verbose ="Power Good";
          DigInputShowInput(json, diginput_index, kind, kind_verbose, pressed, &header);
      }
      if (PinUsed(GPIO_FLT, diginput_index)) {
          pressed = (digitalRead(Pin(GPIO_FLT, diginput_index)) != bitRead(DigInput.inverted_mask, diginput_index));
          kind = "FLT";
          kind_verbose = "Fault";
          DigInputShowInput(json, diginput_index, kind, kind_verbose, pressed, &header);
      }
  }
  if (header) {
    ResponseJsonEnd();
  }
}


/*********************************************************************************************\
 * Commands
\*********************************************************************************************/


/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xsns98(uint8_t function)
{
  bool result = false;

  if (DigInput.present > 0) {
    switch (function) {
      case FUNC_EVERY_SECOND:
        break;
      case FUNC_JSON_APPEND:
        DigInputShow(1);
        break;
#ifdef USE_WEBSERVER
      case FUNC_WEB_SENSOR:
        DigInputShow(0);
        break;
#endif  // USE_WEBSERVER
      case FUNC_SAVE_BEFORE_RESTART:
      case FUNC_SAVE_AT_MIDNIGHT:
        break;
      case FUNC_COMMAND:
        result = false;
        break;
    }
  } else {
    switch (function) {
      case FUNC_INIT:
        DigInputInit();
        break;
      case FUNC_PIN_STATE:
        break;
    }
  }
  return result;
}

#endif  // USE_DIGINPUT
