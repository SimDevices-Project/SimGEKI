#include "hidconfig.h"
#include "roller.h"

#include "string.h"

#include "keyscan.h"
#include "data.h"

uint8_t HIDCFG_Buffer_OUT[64] = {0x00};
uint8_t HIDCFG_Buffer_IN[64]  = {0x00};

static HidconfigData *dataReceive = (HidconfigData *)HIDCFG_Buffer_OUT;
static HidconfigData *dataUpload  = (HidconfigData *)HIDCFG_Buffer_IN;

#define HID_CFG_EP ENDP4

void HIDCONFIG_Upload();
void HIDCONFIG_Receive_Handler();

volatile uint8_t sp_input_state = 0;

void SP_INPUT_Upload()
{
  memset(HIDCFG_Buffer_IN, 0, 64); // Clear buffer

  dataUpload->reportID = HIDCONFIG_REPORT_ID;
  dataUpload->symbol   = 0x00; // No symbol for special input
  dataUpload->command  = SP_INPUT_GET;
  dataUpload->state    = STATE_OK;

  dataUpload->roller_value_sp = Roller_GetValue();
  dataUpload->input_status    = KeyScan_GetAllKeyDebouncedStatus();

  HIDCONFIG_Upload();
}

void SP_INPUT_OnDataUpdate_Handler()
{
  if (sp_input_state == 1) {
    SP_INPUT_Upload();
  }
}

/**
 * Request
 * 1 byte: Report ID
 * 1 byte: Symbol
 * 1 byte: Command
 * 1 byte: Unused
 * 60 bytes: Payload
 *
 * Response (Not Every Command)
 * 1 byte: Report ID
 * 1 byte: Symbol, same as request
 * 1 byte: Command, same as request
 * 1 byte: State
 * 60 bytes: Payload
 */

void HIDCONFIG_Receive_Handler()
{
  switch (dataReceive->reportID) {
    // Custom HIDConfigs
    case HIDCONFIG_REPORT_ID: {
      memset(HIDCFG_Buffer_IN, 0, 64); // Clear buffer

      dataUpload->reportID = dataReceive->reportID;
      dataUpload->symbol   = dataReceive->symbol;

      switch (dataReceive->command) {
        case RELOAD_DATA: {
          // Reload data from flash
          LoadData();
          dataUpload->command = RELOAD_DATA;
          dataUpload->state   = STATE_OK;
          HIDCONFIG_Upload();
          break;
        }
        case SAVE_DATA: {
          // Save data to flash
          SaveData();
          dataUpload->command = SAVE_DATA;
          dataUpload->state   = STATE_OK;
          HIDCONFIG_Upload();
          break;
        }
        case SLEEP_SET_TIMEOUT: {
          // Set sleep timeout
          GlobalData->SleepTimeout = dataReceive->sleep_timeout;
          dataUpload->command      = SLEEP_SET_TIMEOUT;
          dataUpload->state        = STATE_OK;
          HIDCONFIG_Upload();
          break;
        }
        case SLEEP_GET_TIMEOUT: {
          // Get sleep timeout
          dataUpload->sleep_timeout = GlobalData->SleepTimeout;
          dataUpload->command       = SLEEP_GET_TIMEOUT;
          dataUpload->state         = STATE_OK;
          dataUpload->sleep_timeout = GlobalData->SleepTimeout;
          HIDCONFIG_Upload();
          break;
        }
        /**
         * @brief Get roller data : 0xA1
         * This command is used to get the current roller value and raw value.
         * The roller value is the processed value after applying the offset and debouncing.
         * The raw value is the direct reading from the encoder without any processing.
         * The response will contain:
         * - 2 bytes: Roller value (processed value)
         * - 2 bytes: Roller raw value (raw reading)
         */
        case ROLLER_GET_DATA: {
          uint16_t rollerValue    = Roller_GetValue();
          uint16_t rollerRawValue = Roller_GetRawValue();
          // Upload roller data
          dataUpload->command          = ROLLER_GET_DATA;
          dataUpload->state            = STATE_OK;
          dataUpload->roller_value     = rollerValue;
          dataUpload->roller_raw_value = rollerRawValue;
          HIDCONFIG_Upload();
          break;
        }
        /**
         * @brief Set roller offset : 0xA0
         * This command is used to reset the roller offset to 0x8000.
         * After this command, the roller value will be adjusted so that
         * the value at the moment of this command is considered as 0x8000.
         */
        case ROLLER_SET_OFFSET: {
          // Calc roller offset to 0x8000
          Roller_ResetOffset();

          dataUpload->command = ROLLER_SET_OFFSET;
          dataUpload->state   = STATE_OK;
          HIDCONFIG_Upload();
          break;
        }
        /**
         * @brief Set LED mode : 0xB0
         * This command is used to set the color of the LEDs.
         * 1 byte: LED tag, 0xF0 for 7C RGB LED, 0x00-0x02 for RGB ports, 0xFF for all LEDs
         * 1 byte: LED index, 0x00-0x05 for 6 LEDs, 0xff for all LEDs
         * 1 byte: Mode, Always 0x01 for now
         * 1 byte: Brightness, Always 0xFF for now
         * 1 byte: Red color value, 0x00-0xFF
         * 1 byte: Green color value, 0x00-0xFF
         * 1 byte: Blue color value, 0x00-0xFF
         * * The LED tag can be one of the following:
         * - LEDTAG_RGB_PORT_LEFT: Left RGB port
         * - LEDTAG_RGB_PORT_RIGHT: Right RGB port
         * - LEDTAG_RGB_PORT_UART: UART RGB port
         * - LEDTAG_RGB_7C: 7C RGB LED
         * - LEDTAG_ALL: All LEDs
         * * The LED index is used to specify which LED to set the color for.
         * If the LED tag is LEDTAG_RGB_7C, the led_index should be one of the following:
         * - 0x00: LED_7C_L1 - Left 1st LED
         * - 0x02: LED_7C_L2 - Left 2nd LED
         * - 0x04: LED_7C_L3 - Left 3rd LED
         * - 0x01: LED_7C_R1 - Right 1st LED
         * - 0x03: LED_7C_R2 - Right 2nd LED
         * - 0x05: LED_7C_R3 - Right 3rd LED
         * If the LED tag is not LEDTAG_RGB_7C, the led_index should be one of the following:
         * - 0x00-0x05: RGB port 1st to 6th LED
         * - 0xff: All LEDs in the specified port
         */
        case LED_SET_MODE: {
          if (dataReceive->led_tag == LEDTAG_ALL) {
            LED_RGB_SetAll(dataReceive->r, dataReceive->g, dataReceive->b);
            LED_7C_Set(LED_7C_L1, dataReceive->r, dataReceive->g, dataReceive->b);
            LED_7C_Set(LED_7C_L2, dataReceive->r, dataReceive->g, dataReceive->b);
            LED_7C_Set(LED_7C_L3, dataReceive->r, dataReceive->g, dataReceive->b);
            LED_7C_Set(LED_7C_R1, dataReceive->r, dataReceive->g, dataReceive->b);
            LED_7C_Set(LED_7C_R2, dataReceive->r, dataReceive->g, dataReceive->b);
            LED_7C_Set(LED_7C_R3, dataReceive->r, dataReceive->g, dataReceive->b);
          } else if (dataReceive->led_tag == LEDTAG_RGB_7C) {
            LED_7C_Set(dataReceive->led_7c_tag, dataReceive->r, dataReceive->g, dataReceive->b);
          } else {
            if (dataReceive->led_index == 0xff) {
              LED_RGB_SetPort(dataReceive->led_tag, dataReceive->r, dataReceive->g, dataReceive->b);
            } else {
              LED_RGB_Set(dataReceive->led_tag, dataReceive->led_index, dataReceive->r, dataReceive->g, dataReceive->b);
            }
          }

          dataUpload->command = LED_SET_MODE;
          dataUpload->state   = STATE_OK;
          HIDCONFIG_Upload();
          break;
        }
        /**
         * @brief Special LED set command : 0xE0
         * This command is used to set the color of the LEDs in a special way.
         * It is used by the PC DLL to set the colors of the LEDs.
         * The data structure is as follows:
         * - 1 byte: Board ID, 0x00 for RGB ports, 0x01 for 7C RGB LED
         * - 6x3 bytes: Left side RGB port colors, 6 LEDs per port, each with R, G, B values
         * - 6x3 bytes: Right side RGB port colors, 6 LEDs per port, each with R, G, B values
         * - Unused: 4x3 bytes: UART RGB port colors, 4 LEDs per port, each with R, G, B values
         * Or:
         * - 1 byte: Board ID, 0x00 for RGB ports, 0x01 for 7C RGB LED
         * - 6 bytes: 7C LED color values, bit0-2: R, G, B values
         */
        case SP_LED_SET: {
          dataUpload->state   = STATE_OK;
          dataUpload->command = SP_LED_SET;
          switch (dataReceive->board_id) {
            case 0x00: // RGB ports
              LED_RGB_SetPort(LED_RGB_PORT_LEFT, dataReceive->led_rgb_left[0][0], dataReceive->led_rgb_left[0][1], dataReceive->led_rgb_left[0][2]);
              LED_RGB_SetPort(LED_RGB_PORT_LEFT, dataReceive->led_rgb_left[1][0], dataReceive->led_rgb_left[1][1], dataReceive->led_rgb_left[1][2]);
              LED_RGB_SetPort(LED_RGB_PORT_LEFT, dataReceive->led_rgb_left[2][0], dataReceive->led_rgb_left[2][1], dataReceive->led_rgb_left[2][2]);
              LED_RGB_SetPort(LED_RGB_PORT_LEFT, dataReceive->led_rgb_left[3][0], dataReceive->led_rgb_left[3][1], dataReceive->led_rgb_left[3][2]);
              LED_RGB_SetPort(LED_RGB_PORT_LEFT, dataReceive->led_rgb_left[4][0], dataReceive->led_rgb_left[4][1], dataReceive->led_rgb_left[4][2]);
              LED_RGB_SetPort(LED_RGB_PORT_LEFT, dataReceive->led_rgb_left[5][0], dataReceive->led_rgb_left[5][1], dataReceive->led_rgb_left[5][2]);
              LED_RGB_SetPort(LED_RGB_PORT_RIGHT, dataReceive->led_rgb_right[0][0], dataReceive->led_rgb_right[0][1], dataReceive->led_rgb_right[0][2]);
              LED_RGB_SetPort(LED_RGB_PORT_RIGHT, dataReceive->led_rgb_right[1][0], dataReceive->led_rgb_right[1][1], dataReceive->led_rgb_right[1][2]);
              LED_RGB_SetPort(LED_RGB_PORT_RIGHT, dataReceive->led_rgb_right[2][0], dataReceive->led_rgb_right[2][1], dataReceive->led_rgb_right[2][2]);
              LED_RGB_SetPort(LED_RGB_PORT_RIGHT, dataReceive->led_rgb_right[3][0], dataReceive->led_rgb_right[3][1], dataReceive->led_rgb_right[3][2]);
              LED_RGB_SetPort(LED_RGB_PORT_RIGHT, dataReceive->led_rgb_right[4][0], dataReceive->led_rgb_right[4][1], dataReceive->led_rgb_right[4][2]);
              LED_RGB_SetPort(LED_RGB_PORT_RIGHT, dataReceive->led_rgb_right[5][0], dataReceive->led_rgb_right[5][1], dataReceive->led_rgb_right[5][2]);
              // LED_RGB_SetPort(LED_RGB_PORT_UART, dataReceive->led_rgb_uart[0][0], dataReceive->led_rgb_uart[0][1], dataReceive->led_rgb_uart[0][2]);
              // LED_RGB_SetPort(LED_RGB_PORT_UART, dataReceive->led_rgb_uart[1][0], dataReceive->led_rgb_uart[1][1], dataReceive->led_rgb_uart[1][2]);
              // LED_RGB_SetPort(LED_RGB_PORT_UART, dataReceive->led_rgb_uart[2][0], dataReceive->led_rgb_uart[2][1], dataReceive->led_rgb_uart[2][2]);
              // LED_RGB_SetPort(LED_RGB_PORT_UART, dataReceive->led_rgb_uart[3][0], dataReceive->led_rgb_uart[3][1], dataReceive->led_rgb_uart[3][2]);
              break;
            case 0x01: // 7C RGB LED
              LED_7C_Set(LED_7C_L1, dataReceive->led_7c[0] & 0x01, dataReceive->led_7c[0] >> 1 & 0x01, dataReceive->led_7c[0] >> 2 & 0x01);
              LED_7C_Set(LED_7C_L2, dataReceive->led_7c[1] & 0x01, dataReceive->led_7c[1] >> 1 & 0x01, dataReceive->led_7c[1] >> 2 & 0x01);
              LED_7C_Set(LED_7C_L3, dataReceive->led_7c[2] & 0x01, dataReceive->led_7c[2] >> 1 & 0x01, dataReceive->led_7c[2] >> 2 & 0x01);
              LED_7C_Set(LED_7C_R1, dataReceive->led_7c[3] & 0x01, dataReceive->led_7c[3] >> 1 & 0x01, dataReceive->led_7c[3] >> 2 & 0x01);
              LED_7C_Set(LED_7C_R2, dataReceive->led_7c[4] & 0x01, dataReceive->led_7c[4] >> 1 & 0x01, dataReceive->led_7c[4] >> 2 & 0x01);
              LED_7C_Set(LED_7C_R3, dataReceive->led_7c[5] & 0x01, dataReceive->led_7c[5] >> 1 & 0x01, dataReceive->led_7c[5] >> 2 & 0x01);
              break;
            default:
              dataUpload->state = STATE_ERROR;
              break;
          }
          break;
          // No Response
        }
        /**
         * @brief Special input get command : 0xE1
         * This command is used to get the current roller value and input status.
         * The response will contain:
         * - 2 bytes: Roller value
         * - 2 bytes: Input status, each bit represents a button state
         * KeyMap:
         * - Bit 15: ---
         * - Bit 14: LA
         * - Bit 13: LB
         * - Bit 12: LC
         * - Bit 11: RSide
         * - Bit 10: ---
         * - Bit 9: RA
         * - Bit 8: RB
         * - Bit 7: RC
         * - Bit 6: LSide
         * - Bit 5: LMenu
         * - Bit 4: RMenu
         * - Bit 3: Service
         * - Bit 2: Test
         * - Bit 1: ---
         * - Bit 0: Coin
         */
        case SP_INPUT_GET: {
          SP_INPUT_Upload();
          break;
        }
        case SP_INPUT_GET_START: {
          sp_input_state      = 1; // Start special input get
          dataUpload->command = SP_INPUT_GET_START;
          dataUpload->state   = STATE_OK;
          HIDCONFIG_Upload();
          break;
        }
        case SP_INPUT_GET_END: {
          sp_input_state      = 0; // End special input get
          dataUpload->command = SP_INPUT_GET_END;
          dataUpload->state   = STATE_OK;
          HIDCONFIG_Upload();
          break;
        }
        default: {
          dataUpload->command = CMD_NOT_SUPPORT;
          dataUpload->state   = STATE_ERROR;
          HIDCONFIG_Upload();
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
}

void HIDCONFIG_Upload()
{
  USBD_ENDPx_DataUp(HID_CFG_EP, HIDCFG_Buffer_IN, 64);
}