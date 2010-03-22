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
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

static OBJ_DATA *rgObjNest[MAX_NEST];

CH_CMD ( do_finger )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    FILE *fp=NULL;
    bool fOld;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Finger whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) != NULL )
    {
        if ( !IS_NPC ( victim ) )
        {
	    if ( victim == ch )
	    {
		send_to_char ( "You stick a finger up your ass. Ahh, the pleasure.{x\n\r", ch );
		return;
	    }
	     do_whois ( ch, victim->name );
//            act ( "$N is on right now!", ch, NULL, victim, TO_CHAR );
            return; 
        }
    }

    victim = new_char (  );
    victim->pcdata = new_pcdata (  );
    fOld = FALSE;

    sprintf ( buf, "%s%s", PLAYER_DIR, capitalize ( arg ) );
    if ( ( fp = file_open ( buf, "r" ) ) != NULL )
    {
                AttemptJump = true;

                if (setjmp(jump_env) == 1)
                {
                        bugf("[*FINGER*] pFile: '%s' error", capitalize(arg));
                        printf_to_char(ch, "An error occured reading %s's pfile.",
                                           capitalize(arg));
                        new_fclose(fp);
                        free_char(victim);
                        return;
                }

        int iNest;

        for ( iNest = 0; iNest < MAX_NEST; iNest++ )
            rgObjNest[iNest] = NULL;

        fOld = TRUE;
        for ( ;; )
        {
            char letter;
            char *word;

            letter = fread_letter ( fp );
            if ( letter == '*' )
            {
                fread_to_eol ( fp );
                continue;
            }

            if ( letter != '#' )
            {
                bug ( "Load_char_obj: # not found.", 0 );
                break;
            }

            word = fread_word ( fp );
            if ( !str_cmp ( word, "PLAYER" ) )
                fread_char ( victim, fp );
            else if ( !str_cmp ( word, "OBJECT" ) )
                break;
            else if ( !str_cmp ( word, "O" ) )
                break;
            else if ( !str_cmp ( word, "PET" ) )
                break;
            else if ( !str_cmp ( word, "END" ) )
                break;
            else
            {
                bug ( "Load_char_obj: bad section.", 0 );
                break;
            }
        }
        new_fclose ( fp );
    }
    if ( !fOld )
    {
        send_to_char ( "No player by that name exists.\n\r", ch );
        free_pcdata ( victim->pcdata );
        free_char ( victim );
        return;
    }

    if ( ( victim->level > LEVEL_ANCIENT ) && ( victim->level > ch->level ) )
    {
/*	if ( is_name ( victim->name, "Rhaelar" ) )
	send_to_char ( "I seriously doubt Rhaelar would appreciate your finger in his butt.\n\r", ch );
	else if ( is_name ( victim->name, "Sina" ) )
	send_to_char ( "Rhaelar is gonna make your head roll for even trying that! You filthy person!\n\r", ch ); 
	else */
        send_to_char ( "The gods wouldn't like that.\n\r", ch );
        free_pcdata ( victim->pcdata );
        free_char ( victim );
        return;
    }

    printf_to_char ( ch,
                     "{R===================={x\n\r"
                     "{R|{DName{x  :{r %-12s{x\n\r"
                     "{R|{DTitle{x :{r%-12s{x\n\r"
                     "{R|{DSex{x   :{r %-12s{x\n\r"
                     "{R|{DLevel{x :{r %-12d{x\n\r"
                     "{R|{DAge  {x :{r %-12d{x\n\r"
                     "{R|{DClass{x :{r %-12s{x\n\r"
                     "{R|{DRace{x  :{r %-12s{x\n\r"
                     "{R|{DClan{x  :{r %-12s{x\n\r"
                     "{R|{DRank{x  :{r %-12s{x\n\r"
                     "{R|{DSpouse{x:{r %-12s{x\n\r"
                     "{R|{DPK{x    :{r %-12ld{x\n\r"
                     "{R|{DPD{x    :{r %-12ld{x\n\r"
                     "{R|{DAWin{x  :{r %-12d{x\n\r"
                     "{R|{DAloss{x :{r %-12d{x\n\r"
                     "{R|{DTWin{x  :{r %-12d{x\n\r"
                     "{R|{DTKills{x:{r %-12d{x\n\r"
                     "{R|{DTDths{x :{r %-12d{x\n\r"
                     "{R|{DMKills{x:{r %-12d{x\n\r"
                     "{R|{DMDths{x :{r %-12d{x\n\r"
                     "{R====================={x\n\r" "{xLast Logged off %s\r",
                     victim->pcdata->cname, victim->pcdata->title, 
		     victim->sex == 0 ? "Sexless" : victim->sex == 1 ? "Male" : "Female", victim->level,
		     get_age ( victim ),
                     class_table[victim->class].name,
                     pc_race_table[victim->race].name,
                     clan_table[victim->clan].name,
                     clan_rank_table[victim->clan_rank].title_of_rank[victim->
                                                                      sex],
                     IS_MARRIED ( victim ) ? victim->pcdata->spouse : "Not married",
		     victim->pcdata->pkkills, victim->pcdata->pkdeaths,
                     victim->pcdata->awins, victim->pcdata->alosses,
                     victim->pcdata->twins, victim->pcdata->tkills, victim->pcdata->tdeaths,
		     victim->pcdata->mkill, victim->pcdata->mdeath, 
                     ( char * ) ctime ( &victim->llogoff ) );
    	if ( IS_IMMORTAL ( ch ) )
    	{
	 int lasthost = 0, host = 0;
      	 for (host=0; host < MAX_HOST; host++)
      	 {
	     if (victim->pcdata->lasthost[host]!= NULL)
       	     {
			lasthost++;
       	     }
      	 }
      	 sprintf(buf, "%s's last host was %s\n\r", victim->name, victim->pcdata->lasthost[lasthost-1]);      	
      	 stc (buf,ch);
        }

    printf_to_char ( ch, "\n\r" );
    free_pcdata ( victim->pcdata );
    free_char ( victim );
    return;
}
