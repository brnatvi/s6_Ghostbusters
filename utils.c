#include "utils.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// https://stackoverflow.com/questions/3919009/how-to-read-from-stdin-with-fgets
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_input(char *pText, size_t text)
{
    if (fgets(pText, sizeof(text), stdin) == NULL)
    {
        pText[0] = 0;
    }

    while (*pText)    
    {
        if ((*pText == '\r') || (*pText == '\n'))
        {
            *pText = 0; 
            break;    
        }
        pText++;
    }
}

uint32_t read_input_uint()
{
    char line[256];
    read_input(line, sizeof(line));
    uint32_t ret = 0xFFFFFFFF;
    sscanf(line, "%d", &ret);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base console functions
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// https://gist.github.com/vsajip/1864660
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isKeyPressed(void)
{
    struct termios stOldTerm, stNewTerm;
    tcgetattr(STDIN_FILENO, &stOldTerm);
    stNewTerm = stOldTerm;
    stNewTerm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &stNewTerm);
    int prevFl = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, prevFl | O_NONBLOCK);

    int iChar = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &stOldTerm);
    fcntl(STDIN_FILENO, F_SETFL, prevFl);

    if(iChar != EOF)
    {
        ungetc(iChar, stdin);
        return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////////
//   https://stackoverflow.com/questions/1022957/getting-terminal-width-in-c  ////
//////////////////////////////////////////////////////////////////////////////////
uint32_t getTerminalWidth()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

uint32_t getTerminalHeight()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
}

void setTerminalTextColor(enum eTermColor eNb)
{
    printf("\x1b[38;5;%dm", eNb);
}
