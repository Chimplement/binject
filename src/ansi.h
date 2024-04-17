#ifndef ANSI_H
# define ANSI_H

#define ESC "\x1b"

#define BOLD ESC"[1m"
#define RESET_BOLD ESC"[22m"

#define UNDERLINE ESC"[4m"
#define RESET_UNDERLINE ESC"[24m"

#define RED ESC"[31m"
#define BLUE ESC"[34m"
#define CYAN ESC"[36m"
#define WHITE ESC"[37m"

#endif