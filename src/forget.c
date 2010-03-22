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
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sd.h"

CH_CMD ( do_ignor )
{
    send_to_char ( "I'm sorry, forget must be entered in full.\n\r", ch );
    return;
}

CH_CMD ( do_forget )
{
    send_to_char ( "Try 'ignore' instead.\n\r", ch );
    return;
}


CH_CMD ( do_ignore )
{
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int pos;
    bool found = FALSE;

    if ( ch->desc == NULL )
        rch = ch;
    else
        rch = ch->desc->original ? ch->desc->original : ch;

    if ( IS_NPC ( rch ) )
        return;

    smash_tilde ( argument );

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        if ( rch->pcdata->forget[0] == NULL )
        {
            send_to_char ( "You are not ignoring anyone.\n\r", ch );
            return;
        }
        send_to_char ( "You are currently ignoring:\n\r", ch );

        for ( pos = 0; pos < MAX_FORGET; pos++ )
        {
            if ( rch->pcdata->forget[pos] == NULL )
                break;

            sprintf ( buf, "    %s\n\r", rch->pcdata->forget[pos] );
            send_to_char ( buf, ch );
        }
        return;
    }

    for ( pos = 0; pos < MAX_FORGET; pos++ )
    {
        if ( rch->pcdata->forget[pos] == NULL )
            break;

        if ( !str_cmp ( arg, rch->pcdata->forget[pos] ) )
        {
            send_to_char ( "You are already ignoring that person.\n\r",
                           ch );
            return;
        }
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;

        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;

        wch = ( d->original != NULL ) ? d->original : d->character;

        if ( !can_see ( ch, wch ) )
            continue;

        if ( !str_cmp ( arg, wch->name ) )
        {
            found = TRUE;
            if ( wch == ch )
            {
                send_to_char
                    ( "You ignore yourself for a moment, but it passes.\n\r",
                      ch );
                return;
            }
            if ( wch->level >= LEVEL_IMMORTAL )
            {
                send_to_char ( "That person is very hard to ignore.\n\r",
                               ch );
                return;
            }
        }
    }

    if ( !found )
    {
        send_to_char ( "No one by that name is playing.\n\r", ch );
        return;
    }

    for ( pos = 0; pos < MAX_FORGET; pos++ )
    {
        if ( rch->pcdata->forget[pos] == NULL )
            break;
    }

    if ( pos >= MAX_FORGET )
    {
        send_to_char ( "Sorry, you have reached the ignore limit.\n\r", ch );
        return;
    }

    /* make a new forget */
    rch->pcdata->forget[pos] = str_dup ( arg );
    sprintf ( buf, "You are now deaf to %s.\n\r", arg );
    send_to_char ( buf, ch );
}

CH_CMD ( do_remembe )
{
    send_to_char ( "Try 'unignore' instead.\n\r", ch );
    return;
}

CH_CMD ( do_remember )
{
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    int pos;
    bool found = FALSE;

    if ( ch->desc == NULL )
        rch = ch;
    else
        rch = ch->desc->original ? ch->desc->original : ch;

    if ( IS_NPC ( rch ) )
        return;

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        if ( rch->pcdata->forget[0] == NULL )
        {
            send_to_char ( "You are not ignoring anyone.\n\r", ch );
            return;
        }
        send_to_char ( "You are currently ignoring:\n\r", ch );

        for ( pos = 0; pos < MAX_FORGET; pos++ )
        {
            if ( rch->pcdata->forget[pos] == NULL )
                break;

            sprintf ( buf, "    %s\n\r", rch->pcdata->forget[pos] );
            send_to_char ( buf, ch );
        }
        return;
    }

    for ( pos = 0; pos < MAX_FORGET; pos++ )
    {
        if ( rch->pcdata->forget[pos] == NULL )
            break;

        if ( found )
        {
            rch->pcdata->forget[pos - 1] = rch->pcdata->forget[pos];
            rch->pcdata->forget[pos] = NULL;
            continue;
        }

        if ( !str_cmp ( arg, rch->pcdata->forget[pos] ) )
        {
            send_to_char ( "Ignore removed.\n\r", ch );
            free_string ( rch->pcdata->forget[pos] );
            rch->pcdata->forget[pos] = NULL;
            found = TRUE;
        }
    }

    if ( !found )
        send_to_char ( "No one by that name is being ignored.\n\r", ch );
}
