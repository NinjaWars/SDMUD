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
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "sd.h"
#include "tables.h"
#include "lookup.h"

int flag_lookup ( const char *name, const struct flag_type *flag_table )
{
    int flag;

    for ( flag = 0; flag_table[flag].name != NULL; flag++ )
    {
        if ( LOWER ( name[0] ) == LOWER ( flag_table[flag].name[0] ) &&
             !str_prefix ( name, flag_table[flag].name ) )
            return flag_table[flag].bit;
    }

    return 0;
}

int clan_lookup ( const char *name )
{
    int clan;

    for ( clan = 0; clan < MAX_CLAN; clan++ )
    {
        if ( LOWER ( name[0] ) == LOWER ( clan_table[clan].name[0] ) &&
             !str_prefix ( name, clan_table[clan].name ) )
            return clan;
    }
    return 0;
}

int position_lookup ( const char *name )
{
    int pos;

    for ( pos = 0; position_table[pos].name != NULL; pos++ )
    {
        if ( LOWER ( name[0] ) == LOWER ( position_table[pos].name[0] ) &&
             !str_prefix ( name, position_table[pos].name ) )
            return pos;
    }

    return -1;
}

int sex_lookup ( const char *name )
{
    int sex;

    for ( sex = 0; sex_table[sex].name != NULL; sex++ )
    {
        if ( LOWER ( name[0] ) == LOWER ( sex_table[sex].name[0] ) &&
             !str_prefix ( name, sex_table[sex].name ) )
            return sex;
    }

    return -1;
}

int size_lookup ( const char *name )
{
    int size;

    for ( size = 0; size_table[size].name != NULL; size++ )
    {
        if ( LOWER ( name[0] ) == LOWER ( size_table[size].name[0] ) &&
             !str_prefix ( name, size_table[size].name ) )
            return size;
    }

    return -1;
}

HELP_DATA * help_lookup( char *keyword )
{
        HELP_DATA *pHelp;
        char temp[MIL], argall[MIL];

        argall[0] = '\0';

        while (keyword[0] != '\0' )
        {
                keyword = one_argument(keyword, temp);
                if (argall[0] != '\0')
                        strcat(argall," ");
                strcat(argall, temp);
        }

        for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
                if ( is_name( argall, pHelp->keyword ) )
                        return pHelp;

                return NULL;
}

HELP_AREA * had_lookup( char *arg )
{
        HELP_AREA * temp;
        extern HELP_AREA * had_list;

        for ( temp = had_list; temp; temp = temp->next )
                if ( !str_cmp( arg, temp->filename ) )
                        return temp;

                return NULL;
}

