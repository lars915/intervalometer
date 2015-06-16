/*
 * lcd.h
 *
 * Created: 5/30/2015 6:47:28 AM
 *  Author: Larry
 */


#ifndef LCD_H_
#define LCD_H_

void LCDInit(void);
void LCDSendCommand(unsigned char command);
void LCDSendChar(unsigned char character);
void LCDSendText(char *text);
void LCDToggleEnable(void);
void LCDWaitWhileBusy(void);
void LCDSetPos(unsigned char row, unsigned char col);
void LCDClearHome(void);

#endif /* LCD_H_ */