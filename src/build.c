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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "sd.h"
#include "build.h"
#include "version.h"

/* versionToString provided by Erwin S. Andreasen */
const char *versionToString ( int version )
{
    static char buf[64];

    sprintf ( buf, "%d.%02d.%02d", version / 10000,
              ( version - ( ( 10000 * ( version / 10000 ) ) ) ) / 100,
              version % 100 );

    return buf;
}

CH_CMD ( do_version )
{
    char buf[MSL];

    sprintf ( buf, "{D%s{x       :{d %s %s{x\n\r", MUD_NAME,
               versionToString ( VERSION ), RELEASE );
    send_to_char( buf, ch );
    sprintf ( buf, "{DBuild number     {x: {d%d{x\n\r", LINKCOUNT );
    send_to_char( buf, ch );
    send_to_char ( COMPILE_TIME, ch );
}

