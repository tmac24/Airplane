#pragma once

unsigned short queryMessageFilterValue(unsigned char * buf);
unsigned short setMessageFilterValue(unsigned char * buf, unsigned char setting);
unsigned short setInitializationMessage(unsigned char * buf);
unsigned short queryLEDBrightness(unsigned char * buf);
unsigned short setLEDBrightness(unsigned char * buf, unsigned char brightLevel, unsigned char dimLevel);

