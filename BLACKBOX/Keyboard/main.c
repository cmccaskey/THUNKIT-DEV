#include "Keyboard/Keyboard.h"

#include <avr/eeprom.h>

int timerCount = 0;
int commandIndex = 0;

bool keyFlag = false;
bool finishFlag = true;

char command_a[] = "          <terminal>          wget -O wallpaper.jpg 'https://lh3.googleusercontent.com/-pq4KoTJ-r-4/AAAAAAAAAAI/AAAAAAAAAAA/dv4p5T44FmI/w320-h180-p/photo.jpg'>          gsettings set org.gnome.desktop.background picture-uri 'file:///home/'$USER'/wallpaper.jpg'>          exit>";
//char command_a[] = "          <terminal>          wget -O wallpaper.jpg 'https://tinyurl.com/y8gdeelp'>          gsettings set org.gnome.desktop.background picture-uri 'file:///home/'$USER'/wallpaper.jpg'>          exit>";
//char command_b[] = "          <terminal>          wget -O wallpaper.jpg 'https://tinyurl.com/ybn7w7gf'>          gsettings set org.gnome.desktop.background picture-uri 'file:///home/'$USER'/wallpaper.jpg'>          exit>";
//char command_c[] = "          <terminal>          wget -O wallpaper.jpg 'https://tinyurl.com/y9ythclm'>          gsettings set org.gnome.desktop.background picture-uri 'file:///home/'$USER'/wallpaper.jpg'>          exit>";
//char command_d[] = "          <terminal>          wget -O wallpaper.jpg 'https://tinyurl.com/ydeqsr8r'>          gsettings set org.gnome.desktop.background picture-uri 'file:///home/'$USER'/wallpaper.jpg'>          exit>";

static uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_Keyboard,
				.ReportINEndpoint             =
					{
						.Address              = KEYBOARD_EPADDR,
						.Size                 = KEYBOARD_EPSIZE,
						.Banks                = 1,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
			},
	};

ISR(TIMER0_OVF_vect) { //allows for keypress delay
  cli();
  timerCount ++;
  if (timerCount > 4) {
    keyFlag = true;
    timerCount = 0;
  }
  sei();
}

int timer1 = 0;

ISR(TIMER1_OVF_vect) { //allows for keypress delay
  cli();
  timer1 ++;
  	if (timer1 >= 200){
	  timer1 = 0;
    HID_Device_USBTask(&Keyboard_HID_Interface);
    USB_USBTask();
   }
  sei();
}


int main(void)
{
  if (eeprom_read_byte (( uint8_t *)0) == 0) {
    eeprom_write_byte (( uint8_t *)0, 'a');
  }
  SetupHardware();
  TCCR0B |= (1 << CS02); //Table 15-9 clk/256 prescale
  TIMSK0 |= (1 << TOIE0); //timer 0 overflow interrupt enable
  TCCR1B |= ((1 << CS02) | (1 << CS00)); //Table 15-9 clk/1024 prescale
  TIMSK1 |= (1 << TOIE0); //timer 1 overflow interrupt enable
  sei();
  GlobalInterruptEnable();
  for (;;)
  {
    //USB is handled by Timer 1 as well
    HID_Device_USBTask(&Keyboard_HID_Interface);
    USB_USBTask();
    timer1 = 0;
  }
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware()
{
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	clock_prescale_set(clock_div_1);
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);

	USB_Device_EnableSOFEvents();

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
    uint8_t* const ReportID,
    const uint8_t ReportType,
    void* ReportData,
    uint16_t* const ReportSize)
{
  USB_KeyboardReport_Data_t* KeyboardReport = (USB_KeyboardReport_Data_t*)ReportData;//

  uint8_t UsedKeyCodes = 0;
if (keyFlag && !finishFlag) {
    char charChar = command_a[commandIndex];
   char commandEEPROM = eeprom_read_byte (( uint8_t *)0);
    /*if (commandEEPROM == 'a') {
      charChar = command_a[commandIndex];
      eeprom_write_byte (( uint8_t *)0, 'b');
    if (commandIndex >= sizeof(command_a) / sizeof(char))
      finishFlag = true;
    }
    if (commandEEPROM == 'b') {
      charChar = command_b[commandIndex];
      eeprom_write_byte (( uint8_t *)0, 'c');
    if (commandIndex >= sizeof(command_b) / sizeof(char))
      finishFlag = true;
    }
    if (commandEEPROM == 'c') {
      charChar = command_c[commandIndex];
      eeprom_write_byte (( uint8_t *)0, 'a');
    if (commandIndex >= sizeof(command_c) / sizeof(char))
      finishFlag = true;
    }*/
  //charChar = command_a[commandIndex];
  commandIndex ++;
  if (commandIndex >= sizeof(command_a) / sizeof(char))
          finishFlag = true;
    keyFlag = false;
    if (charChar <= 90 && charChar >= 65) { //capital
      KeyboardReport->KeyCode[UsedKeyCodes++] = (int) charChar - 61;
      KeyboardReport->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;
    } else if (charChar <= 122 && charChar >= 97) { //lower case
      KeyboardReport->KeyCode[UsedKeyCodes++] = (int) charChar - 93;
    } else if (charChar <= 57 && charChar >= 49) { //1-9
      KeyboardReport->KeyCode[UsedKeyCodes++] = (int) charChar - 19;
    } else if (charChar == 60) { //super key...use <   key
      KeyboardReport->KeyCode[UsedKeyCodes++] = 0xE3;
    } else if (charChar == 62) { //enter key...use > key
      KeyboardReport->KeyCode[UsedKeyCodes++] = 0x28;
    } else if (charChar == 35) { //cmd + space...use # key
      KeyboardReport->KeyCode[UsedKeyCodes++] = 0x2C; //space
      KeyboardReport->Modifier = 0xE3; //super key
    } else {
      int charInt;
      bool charShift = false;
      switch (charChar) {
        case 48:
          charInt = 0x27;
          break;
        case 32:
          charInt = 0x2C;
          break;
        case 33:
          charInt = 0x1E;
          charShift = true;
          break;
        case 34:
          charInt = 0x34;
          charShift = true;
          break;
        case 35:
          charInt = 0x20;
          charShift = true;
          break;
        case 36:
          charInt = 0x21;
          charShift = true;
          break;
        case 37:
          charInt = 0x22;
          charShift = true;
          break;
        case 38:
          charInt = 0x24;
          charShift = true;
          break;
        case 39:
          charInt = 0x34;
          break;
        case 40:
          charInt = 0x26;
          charShift = true;
          break;
        case 41:
          charInt = 0x27;
          charShift = true;
          break;
        case 42:
          charInt = 0x55;
          break;
        case 43:
          charInt = 0x57;
          break;
        case 44:
          charInt = 0x36;
          break;
        case 45:
          charInt = 0x2D;
          break;
        case 46:
          charInt = 0x37;
          break;
        case 47:
          charInt = 0x38;
          break;
        case 58:
          charInt = 0x33;
          charShift = true;
          break;
        case 59:
          charInt = 0x33;
          break;
        case 60:
          charInt = 0x36;
          charShift = true;
          break;
        case 61:
          charInt = 0x2E;
          break;
        case 62:
          charInt = 0x37;
          charShift = true;
        case 63:
          charInt = 0x38;
          charShift = true;
          break;
        case 64:
          charInt = 0x1F;
          charShift = true;
          break;
        case 91:
          charInt = 0x2F;
          break;
        case 92:
          charInt = 0x31;
          break;
        case 93:
          charInt = 0x30;
          break;
        case 94:
          charInt = 0x23;
          charShift = true;
          break;
        case 95:
          charInt = 0x2D;
          charShift = true;
          break;
        case 123:
          charInt = 0x2F;
          charShift = true;
          break;
        case 124:
          charInt = 0x31;
          charShift = true;
          break;
        case 125:
          charInt = 0x30;
          charShift = true;
          break;
        case 126:
          charInt = 0x32;
          charShift = true;
          break;
      }
      KeyboardReport->KeyCode[UsedKeyCodes++] = charInt;
      if (charShift) {
        KeyboardReport->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;
      }
    }
  }
  *ReportSize = sizeof(USB_KeyboardReport_Data_t);
  return false;
}

void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
  uint8_t* KEYReport = (uint8_t*)ReportData;

  if (*KEYReport & 0x82) { //detect caps lock
    _delay_ms(1000);
    finishFlag = false;
    commandIndex = 0;
  }
}
