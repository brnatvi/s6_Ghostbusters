#ifndef COMMONS
#define COMMONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#define CLOSE(File) if (File >= 0) {close(File); File = -1; }

#endif //COMMONS