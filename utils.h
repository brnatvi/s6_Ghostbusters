#ifndef CLI_WND
#define CLI_WND

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

enum eTermColor
{
    //https://en.wikipedia.org/wiki/ANSI_escape_code
    eTcBlack        = 0,
    eTcBlue         = 12,
    eTcGreen        = 40,
    eTcCyan         = 51,
    eTcRed          = 124,
    eTcMagenta      = 129,
    eTcBrown        = 94,
    eTcLightGray    = 252,
    eTcDarkGray     = 248,
    eTcLightBlue    = 33,
    eTcLightGreen   = 48,
    eTcLightCyan    = 195,
    eTcLightRed     = 196,
    eTcLightMagenta = 204,
    eTcYellow       = 11,
    eTcWhite        = 15
};

void      read_input_text(char *pText, size_t text);
uint32_t  read_input_uint();
bool      isKeyPressed(void);
uint32_t  getTerminalWidth();
uint32_t  getTerminalHeight();
void      setTerminalTextColor(enum eTermColor eFg);

#endif //CLI_WND
