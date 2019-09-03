/*
 * This file was originally part of John the Ripper password cracker,
 * Copyright (c) 1996-99,2010 by Solar Designer
 *
 * ...modifications made for the Pretty Cool Fuzzy Guesser by Matt Weir
 */

/*
 * Terminal support routines.
 */

#ifndef _TTY_H
#define _TTY_H

/*
 * Initializes the terminal for unbuffered non-blocking input. Also registers
 * tty_done() via atexit().
 */
extern void tty_init(int turn_on);

/*
 * Reads a character, returns -1 if no data available or on error.
 */
extern int tty_getchar(void);

/*
 * Restores the terminal parameters and closes the file descriptor.
 */
extern void tty_done(void);

#endif
