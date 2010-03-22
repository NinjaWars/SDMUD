/***************************************************************************
*   Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
*   Michael Seifert, Hans Henrik Stærfeldt, Tom Madsen, and Katja Nyboe.   *
*                                                                          *
*   Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael         *
*   Chastain, Michael Quan, and Mitchell Tse.                              *
*                                                                          *
*       ROM 2.4 is copyright 1993-1995 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       ROT 2.0 is copyright 1996-1999 by Russ Walsh                       *
*       DRM 1.0a is copyright 2000-2002 by Joshua Chance Blackwell         *
*        SD 2.0 is copyright 2004-2006 by Patrick Mylund Nielsen           *
***************************************************************************/

#define BUFFER_DEBUG

typedef struct buffer_type BUFFER2;

struct buffer_type
{
	char *data; /* The data */
	
	int len;	/* The current len of the buffer */
	int size;	/* The allocated size of data */
	
	bool overflowed; /* Is the buffer overflowed? */
};

#ifdef BUFFER_DEBUG /* Debugged version */

#define buffer_new(size)           __buffer_new (size, __FILE__, __LINE__)
#define buffer_strcat(buffer,text) __buffer_strcat (buffer, text, __FILE__, __LINE__)

BUFFER2 * __buffer_new (int size, const char *file, unsigned line);
void __buffer_strcat (BUFFER2 *buffer, const char *text, const char *file, unsigned line);

#else  /* not debugged version */

#define buffer_new(size)           __buffer_new (size)
#define buffer_strcat(buffer,text) __buffer_strcat (buffer,text)

BUFFER2 * __buffer_new (int size);
void __buffer_strcat (BUFFER2 *buffer, const char *text);

#endif


void buffer_free (BUFFER2 *buffer);
void buffer_clear (BUFFER2 *buffer);
int find_mem_size (int min_size);
int buffer_printf (BUFFER2 *buffer, char *fmt, ...);
