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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "sd.h"
#include "magic.h"
#include "recycle.h"
/*#include "interp.h"*/

ILLEGAL_DATA *illegal_first;

/*
* Parse a name for acceptability.
*/
bool check_parse_name(char *name)
{
	//bool found = FALSE;
	//int count = 0;

	if (check_illegal(name))
	return FALSE;

	if (name[0] == '\0')
	return FALSE;

	if ( strlen(name) <  3 )
	return FALSE;

#if defined(MSDOS)
	if ( strlen(name) >  8 )
	return FALSE;
#endif

#if defined(macintosh) || defined(unix)
	if ( strlen(name) > 22 )
	return FALSE;
#endif

	{
		char *pc;
		bool fIll,adjcaps = FALSE,cleancaps = FALSE;
		int total_caps = 0;

		fIll = TRUE;
		for ( pc = name; *pc != '\0'; pc++ )
			{		
			if ( !isalpha(*pc) )
			return FALSE;

			if ( isupper(*pc))
				{
				if (adjcaps)
				cleancaps = TRUE;
				total_caps++;
				adjcaps = TRUE;
				}
			else
				adjcaps = FALSE;

				if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
				fIll = FALSE;
			}

		if ( fIll )
		return FALSE;

		if (cleancaps || ((unsigned)total_caps > (strlen(name)) / 2 
		&&	strlen(name)< 3))
		return FALSE;
		}

		{
/*		extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH]; */
		MOB_INDEX_DATA *pMobIndex;
		int iHash;

		for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
			{
			for ( pMobIndex  = mob_index_hash[iHash];
			pMobIndex != NULL;
			pMobIndex  = pMobIndex->next )
				{
				if ( is_name( name, pMobIndex->player_name ) )
				return FALSE;
				}
			}
	}

	return TRUE;
}

CH_CMD ( do_nameban )
{
	ILLEGAL_DATA *p,*q;
	char pWord[MAX_INPUT_LENGTH];
	char pMode[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char kar;
	argument = one_argument(argument,pWord);
	argument = one_argument(argument,pMode);
	argument = one_argument(argument,arg3);
	if (IS_NPC(ch))
		{
		send_to_char ("RETURN first.\n\r",ch);
		return;
		}


	if (pWord[0] == '\0' )
		{
		send_to_char ("Syntax: Nameban <name> <mode> \"reason\"\n\r",ch);
		send_to_char ("        Nameban <list>.\n\r",ch);
		send_to_char ("	reason {R**{CMUST{R**{x \"be formatted like this. \"\n\r",ch);
		send_to_char ("mode is prefix, equal, delete or never.\n\r",ch);
		send_to_char ("example: {rnameban {cass {ynever{x \"Stupid Name\"\n\r",ch);
		send_to_char ("will never allow any name with ass in it.\n\r",ch);
		send_to_char ("example2: {rnameban {cass {yprefix{x \"Stupid Name\" \n\r",ch);
		send_to_char ("will never allow any name beginning with ass.\n\r",ch);
		send_to_char ("example3: {rnameban {cass {yequal{x \"Stupid Name\"\n\r",ch);
		send_to_char ("simply bans the name ass.\n\r",ch);
		return;
		}
	if (!str_cmp(pWord,"list"))
		{
		if (!illegal_first)
			{
			send_to_char ("There are no illegal names.\n\r",ch);
			return;
			}
		send_to_char("Who:             Name:         Mode:      Why:\n\r",ch);
		for (p = illegal_first; p; p = p->next)
			{
			 sprintf(buf,"%-16s %-13s %-10s %s \n\r",p->who, p->word, p->mode == '#' ? "equal": 
			 p->mode == '$' ? "never":p->mode == '@' ?"prefix":"null", p->why);
      		send_to_char(buf,ch);
			}
		send_to_char("\n\r",ch);

		return;
		}

	if (!str_cmp(pMode,"delete"))
	{
	for (p = illegal_first; p ; p = p->next)
	if (!str_cmp(pWord, p->word))
	break;

	if (p)
		{
		if (illegal_first == p) /* node to be removed == head ? */
		illegal_first = p->next;
		else /* Find the node before this one */
			{
			for (q = illegal_first; q->next != p; q = q->next); /* empty for */
			q->next = p->next;
			}

		free_string (p->word);
		free_mem (p ,sizeof(ILLEGAL_DATA)); /* free node */
		save_illegal(); /* save to disk */
		send_to_char ("Word deleted.\n\r",ch);
		return;
		}
		else
		{
		send_to_char("Word not found in the list.\n\r",ch);
		return;
		}
	}

	if (strlen(pWord) > 22)      /* This was 11 it should be 12 - JEC */
	{
	send_to_char("Word too long.\n\r",ch);
	return;
	}
	if (strlen(arg3) > 26)
	{
	send_to_char("Reason too long. Not banned.\n\r",ch);
	return;
	}

	if (check_illegal(pWord))
	{
	send_to_char("That word is already illegal.\n\r",ch);
	return;
	}

	if (!str_cmp(ch->name,pWord) && ch->level >= MAX_LEVEL)
	{
	send_to_char("You were actually going to illegalize yourself?!\n\r",ch);
	return;
	}

	if (!str_cmp(pMode,"equal"))
	kar = '#';
	else if (!str_cmp(pMode,"never"))
	kar = '$';
	else if (!str_cmp(pMode,"prefix"))
	kar = '@';
	else
	{
	send_to_char("Wrong mode.\n\r",ch);
	return;
	}

	p = alloc_mem (sizeof(ILLEGAL_DATA));
	p->mode = kar;
	p->word = str_dup(pWord);
	p->who  = ch->name;
	p->why  = str_dup(arg3);
	p->next = illegal_first;
	illegal_first = p; /* add before the current first element */

	send_to_char ("Word illegalized.\n\r",ch);
	save_illegal(); /* save to disk */
	return;
}


bool check_illegal (char *name)
{
ILLEGAL_DATA *p;
DESCRIPTOR_DATA *d;
//bool found = FALSE;
int count = 0;

for (p = illegal_first; p ; p = p->next)
{
if(p->mode == '#')
{
if (!str_cmp(name,p->word)
|| is_name(name,p->word)
|| is_exact_name(name,p->word))
return TRUE;
}
else if(p->mode == '@')
{
if (!str_prefix(p->word,name))
return TRUE;
}
else if(p->mode == '$')
{
if (!str_cmp(name,p->word)
|| is_exact_name(name,p->word)
|| is_name(name,p->word)
|| !str_infix(name,p->word)
|| !str_prefix(p->word,name)
|| strstr(name,p->word))
return TRUE;
}
}
    /*
     * check names of people playing. Yes, this is necessary for multiple
     * newbies with the same name (thanks Saro)
     */
    if (descriptor_list) {
        count=0;
        for (d = descriptor_list; d != NULL; d = d->next) {
            d->next=d->next;
            if 
(d->connected!=CON_PLAYING&&d->character&&d->character->name
                && d->character->name[0] && !str_cmp(d->character->name,name))
{
                count++;
                //close_socket(d);
		return TRUE;
            }
        }
        if (count) {
            sprintf(log_buf,"Double newbie alert (%s)",name);
            wiznet(log_buf,NULL,NULL,WIZ_LOGINS,0,0);
            return FALSE;
        }
    }
return FALSE;
}

void load_illegal()
{
	FILE *fp=NULL;
	ILLEGAL_DATA *p;
	char kar;
	illegal_first = NULL;
//	fclose(fpReserve);
	fp = fopen (ILLEGAL_FILE, "r");

	if (!fp)
	return;

	do
	{
	kar = fread_letter(fp);
	if (kar == EOF)
	break;

	if (kar == '#' || kar == '$' || kar == '@')
		{
		p = alloc_mem(sizeof(ILLEGAL_DATA));
		p->mode = kar;
		p->word = str_dup(fread_word(fp));
		p->who = str_dup(fread_word(fp));
		p->why = str_dup(fread_word(fp));
		p->next = illegal_first;
		illegal_first = p;
		}
	}
	while (kar != EOF);

	new_fclose (fp);
//	fpReserve = fopen( NULL_FILE, "r" );
}

void save_illegal()
{
	FILE *fp=NULL;
	ILLEGAL_DATA *p;

	if (!illegal_first)
	{
	unlink (ILLEGAL_FILE);
	return;
	}
//	fclose(fpReserve);
	fp = fopen (ILLEGAL_FILE, "w");

	if (!fp)
	{
	bug ("Could not open " ILLEGAL_FILE " for writing",0);
	return;
	}

	for (p = illegal_first; p ; p = p->next)
	fprintf (fp, "%c \'%s\'\'%s\'\'%s\' \n", p->mode, p->word, p->who, p->why);
	new_fclose (fp);
//	fpReserve = fopen( NULL_FILE, "r" );
}

