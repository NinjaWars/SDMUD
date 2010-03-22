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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "sd.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "db.h"

#define BAR_1     "{R[{mo{R]{r------------------------------------------------------------------------------{R[{mo{R]{x\n\r"

/* void bprintf args ( ( BUFFER * buffer, char *, ... ) )
    __attribute__ ( ( format ( printf, 2, 3 ) ) ); */

/*
void bprintf ( BUFFER2 * buffer, char *fmt, ... )
{
    char buf[MSL];
    va_list args;

    va_start ( args, fmt );
    vsnprintf ( buf, MSL, fmt, args );
    va_end ( args );

    buffer_strcat ( buffer, fmt );
}
*/
/* 
    Syntax(s):
                help <letter>       - list help files that start with <letter>
                help 2.<keyword>    - find second occurance of <keyword>
                help <keyword>      - find first occurance of <keyword>, put the rest
                                      into 'related helps'.
*/
void do_help ( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    char buf[MSL];
    bool found = FALSE;
    bool list = FALSE;
    char argall[MIL], argone[MIL], argall2[MIL];
    char nohelp[MIL];
    //BUFFER *buffer;
    //BUFFER *related;
    BUFFER2 *buffer;
    BUFFER2 *related;
    int counter = 0, number, count = 0;
    bool secret = FALSE;
//    char string[MSL];

    if ( IS_NULLSTR ( argument ) )
        argument = "newbie";

    argall[0] = '\0';
    while ( !IS_NULLSTR ( argument ) )
    {
        argument = one_argument ( argument, argone );
        if ( !IS_NULLSTR ( argall ) )
            strcat ( argall, " " );
        strcat ( argall, argone );
    }

    if ( !str_cmp ( argall, "credits" ) ||
         !str_cmp ( argall, "imotd" ) ||
	 !str_cmp ( argall, "wlasciipic" ) ||
	 !str_cmp ( argall, "motd" ) ||
	 !str_cmp ( argall, "newchartier" ) ||
	 !str_cmp ( argall, "menu choice" ) ||
	 !str_cmp ( argall, "group header" ) ||
	 !str_cmp ( argall, "group help" ) )
    secret = TRUE;

/*    buffer = new_buf (  );
    related = new_buf (  ); */
    buffer = buffer_new ( 1337 );
    related = buffer_new ( 56 );
    /* allows for help 2.keyword */
    number = number_argument ( argall, argall2 );
    strcpy ( nohelp, argall2 );

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
        if ( pHelp->level > get_trust ( ch ) )
            continue;

        if ( !is_name ( argall2, pHelp->keyword ) )
            continue;

        if ( ( strlen ( argall2 ) == 1 ) )
        {
            counter++;

            //buffer_printf ( buffer, "%3d) %s\n\r", counter, pHelp->keyword );
	    sprintf ( buf, "%3d) %s\n\r", counter, pHelp->keyword );
	    buffer_strcat ( buffer, buf );

            list = TRUE;
            found = TRUE;
            continue;
        }
        if ( found )
        {
            //buffer_printf ( related, "%s, ", pHelp->keyword );
	    sprintf ( buf, "%s, ", pHelp->keyword );
	    buffer_strcat ( related, buf );
            continue;
        }
        if ( ++count == number )
        {
            if ( !secret )
	    {
//	       add_buf ( buffer, BAR_1 );
	       buffer_strcat ( buffer, BAR_1 );
               //buffer_printf ( buffer, "Help Keywords : %s\n\r", pHelp->keyword );
	       sprintf ( buf, "Help Keywords : %s\n\r", pHelp->keyword );
	       buffer_strcat ( buffer, buf );
               //add_buf ( buffer, BAR_1 );
	       buffer_strcat ( buffer, BAR_1 );
	    }

            if ( pHelp->text[0] == '.' )
		buffer_strcat ( buffer, pHelp->text +1 );
            else
		buffer_strcat ( buffer, pHelp->text );
	    if ( !secret )
	    {
               //add_buf ( buffer, BAR_1 );
	       buffer_strcat ( buffer, BAR_1 );
	    }
               found = TRUE;        
	}
    }
    if ( list )
    {
        char *text;
//        char buf[MIL];

        //text = str_dup ( buf_string ( buffer ) );
        text = str_dup ( buffer_string ( buffer ) );
//        clear_buf ( buffer );
	buffer_clear ( buffer );
        //buffer_printf ( buffer, "Help files that start with the letter '%s'.\n\r", argall2 );
	sprintf ( buf, "Help files that start with the letter '%s'.\n\r", argall2 );
	buffer_strcat ( buffer, buf );
        //add_buf ( buffer, BAR_1 );
	buffer_strcat ( buffer, BAR_1 );
//        add_buf ( buffer, text );
	buffer_strcat ( buffer, text );
        //add_buf ( buffer, BAR_1 );
	buffer_strcat ( buffer, BAR_1 );
        //buffer_printf ( buffer, "%d total help files.\n\r", counter );
	sprintf ( buf, "%d total help files.\n\r", counter );
	buffer_strcat ( buffer, buf );
        free_string ( text );
    }

    else if ( !found )
    {
        //buffer_printf ( buffer, "No help found for %s. Try using just the first letter.\n\r", nohelp );
	sprintf ( buf, "No help found for %s. Try using just the first letter.\n\r", nohelp );
	buffer_strcat ( buffer, buf );
        append_file ( ch, HELP_FILE, nohelp );
        sprintf ( log_buf, "Missing Help: %s", nohelp );
        wiznet ( log_buf, ch, NULL, 0, 0, 0 );
    }
    else if ( related->data != NULL && related->data[0] != '\0' )
    {
        related->data[strlen ( related->data ) - 2] = '.';
        related->data[strlen ( related->data ) - 1] = '\0';
        //buffer_printf ( buffer, "See also: %s\n\r%s", buffer_string ( related ), BAR_1 );
	sprintf ( buf, "See also: %s\n\r%s", buffer_string ( related ), BAR_1 );
	buffer_strcat ( buffer, buf );
    }
//    page_to_char ( buf_string ( buffer ), ch );
    //send_to_char ( buf_string ( buffer ), ch );
    if ( !secret ) page_to_char ( buffer_string ( buffer ), ch );
    else send_to_char ( buffer_string ( buffer ), ch );
    //free_buf ( buffer );
    //free_buf ( related );
    buffer_free ( buffer );
    buffer_free ( related );
}

