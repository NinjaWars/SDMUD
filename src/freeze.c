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

 /***************************************************************************
 * Automated Freeze Tag Code                                                *
 * Markanth : dlmud@dlmud.com                                               *
 * Devil's Lament : dlmud.com port 3778                                     *
 * Web Page : http://www.dlmud.com                                          *
 *                                                                          *
 * Provides automated freeze tag games in an area.                          *
 * Code orginally done by Nebseni of Clandestine MUD                        *
 * <clandestine.mudnet.net:9476>.                                           *
 *                                                                          *
 * All I ask in return is that you give me credit on your mud somewhere     *
 * or email me if you use it.                                               *
 ****************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sd.h"
// #include "interp.h"

TAG_DATA tag_game;

void do_red(CHAR_DATA *ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC (ch))
    return;

  if (tag_game.status == TAG_OFF)
  {
    send_to_char ("There is no tag game playing.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char ("Syntax:  red <message>\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL (ch)
      && !IS_SET (ch->pcdata->tag_flags, TAG_PLAYING | TAG_WAITING))
  {
    send_to_char ("You must be a freeze tag player to use this channel.\n\r", ch);
    return;
  }

  if (IS_SET (ch->pcdata->tag_flags, TAG_BLUE))
  {
    sprintf (buf, "{{{RRed{x} %s: %s\n\r", ch->name, argument);
    send_to_char (buf, ch);
  }
  else
  {
    sprintf (buf, "{{{RRed{x} %s: %s\n\r", ch->name, argument);
  }

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && d->character && !IS_NPC(d->character)
	&& IS_SET (d->character->pcdata->tag_flags, TAG_RED))
    {
      send_to_char (buf, d->character);
    }
  }
}

void do_blue(CHAR_DATA *ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC (ch))
    return;

  if (tag_game.status == TAG_OFF)
  {
    send_to_char ("There is no tag game playing.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char ("Syntax:  blue <message>\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL (ch)
      && !IS_SET (ch->pcdata->tag_flags, TAG_PLAYING | TAG_WAITING))
  {
    send_to_char ("You must be a freeze tag player to use this channel.\n\r", ch);
    return;
  }

  if (IS_SET (ch->pcdata->tag_flags, TAG_RED))
  {
    sprintf (buf, "{{{BBlue{x} %s: %s\n\r", ch->name, argument);
    send_to_char (buf, ch);
  }
  else
  {
    sprintf (buf, "{{{BBlue{x} %s: %s\n\r", ch->name, argument);
  }

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && d->character && !IS_NPC(d->character)
	&& IS_SET (d->character->pcdata->tag_flags, TAG_BLUE))
    {
      send_to_char (buf, d->character);
    }
  }
}

void end_tag (void)
{
  DESCRIPTOR_DATA *d;

  tag_game.status = TAG_OFF;
  tag_game.timer = -1;
  tag_game.next = number_range (30, 50);
  tag_game.playing = 0;

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected != CON_PLAYING || !d->character || IS_NPC(d->character))
      continue;

    if (IS_SET (d->character->pcdata->tag_flags, TAG_PLAYING | TAG_WAITING))
    {
      char_from_room (d->character);
      char_to_room (d->character, get_room_index (ROOM_VNUM_TEMPLE));
      do_look (d->character, "auto");
      send_to_char ("{R[{WFreeze Tag{R] {CFreeze tag has been stopped!{x\n\r", d->character);
      REMOVE_BIT ( d->character->act2, PLR2_PKSAFE );
    }
    d->character->pcdata->tag_flags = 0;
  }
}

void start_tag (void)
{
  DESCRIPTOR_DATA *d;
  ROOM_INDEX_DATA *loc;
  int count = 0;
  char buf[MAX_INPUT_LENGTH];

  tag_game.status = TAG_ISPLAY;
  tag_game.timer = 5 * tag_game.playing;
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected != CON_PLAYING || !d->character || IS_NPC(d->character))
      continue;

    if (IS_SET (d->character->pcdata->tag_flags, TAG_WAITING))
    {
      count++;
      loc = get_room_index (number_range (FTAG_MIN_VNUM, FTAG_MAX_VNUM));
      REMOVE_BIT (d->character->pcdata->tag_flags, TAG_FROZEN);
      REMOVE_BIT (d->character->pcdata->tag_flags, TAG_WAITING);
      SET_BIT (d->character->pcdata->tag_flags, TAG_PLAYING);
      char_from_room (d->character);
      char_to_room (d->character, loc);
      do_look (d->character, "auto");
    }
  }
  sprintf(buf, "{R[{WFreeze Tag{R] {CFreeze Tag has started! {Y%d{C people playing.{x", count);
  tag_channel (NULL, buf);
}

bool fRed = FALSE;

void check_team_frozen (CHAR_DATA * ch)
{
  DESCRIPTOR_DATA *d;

  if (IS_NPC (ch) || !ch->pcdata)
    return;

  if (IS_SET (ch->pcdata->tag_flags, TAG_BLUE))
  {
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->connected == CON_PLAYING 
      && d->character && !IS_NPC(d->character)
	  && IS_SET (d->character->pcdata->tag_flags, TAG_PLAYING)
	  && IS_SET (d->character->pcdata->tag_flags, TAG_BLUE)
	  && !IS_SET (d->character->pcdata->tag_flags, TAG_FROZEN))
      {
	return;
      }
    }
    tag_channel (NULL, "{R[{WFreeze Tag{R] {CThe {RRED {Cteam has won FREEZE TAG!!!{x");
    end_tag ();
    return;
  }

  if (IS_SET (ch->pcdata->tag_flags, TAG_RED))
  {
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->connected == CON_PLAYING
      && d->character && !IS_NPC(d->character)
	  && IS_SET (d->character->pcdata->tag_flags, TAG_PLAYING)
	  && IS_SET (d->character->pcdata->tag_flags, TAG_RED)
	  && !IS_SET (d->character->pcdata->tag_flags, TAG_FROZEN))
      {
	return;
      }
    }
    tag_channel (NULL, "{R[{WFreeze Tag{R] {CThe {BBLUE {Cteam has won FREEZE TAG!!!{x");
    end_tag ();
    return;
  }
}

void do_ftag(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC (ch))
    return;

  argument = one_argument (argument, arg1);

  if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
  {
     send_to_char ( "You're {Rviolent{x, buzzle off!\n\r", ch );
     return;
  }

  if (arg1[0] == '\0')
  {
    send_to_char ("Syntax:  ftag join\n\r", ch);
    send_to_char ("To communicate use the 'blue' and 'red' channels.\n\r", ch);
    send_to_char("To tag someone once the game has started use the 'tag' comand.\n\r", ch);
    if (IS_IMMORTAL (ch))
    {
      send_to_char ("\n\rSyntax:  ftag reset\n\r", ch);
      send_to_char ("         ftag next\n\r", ch);
      send_to_char ("         ftag start\n\r", ch);
      send_to_char ("         ftag red <player>\n\r", ch);
      send_to_char ("         ftag blue <player>\n\r", ch);
    }
    return;
  }

  if (!str_cmp (arg1, "join"))
  {
    ROOM_INDEX_DATA *loc;

    if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
    {
	send_to_char ( "Not whilst you're violent!\n\r", ch );
	return;
    }

    if (tag_game.status != TAG_ISWAIT)
    {
      send_to_char ("There is no tag game to join.\n\r", ch);
      return;
    }

    if (IS_SET (ch->pcdata->tag_flags, TAG_PLAYING | TAG_WAITING))
    {
      send_to_char ("You're already playing.\n\r", ch);
      return;
    }

    if ((loc = get_room_index (FTAG_WAIT_ROOM)) == NULL)
    {
      send_to_char ("The freeze tag arena hasn't been finished yet.\n\r", ch);
      return;
    }
    send_to_char ("You join freeze tag.\n\r", ch);
    char_from_room (ch);
    char_to_room (ch, loc);
    tag_game.playing += 1;
    do_look (ch, "auto");
    if ((fRed = !fRed))
    {
      SET_BIT (ch->pcdata->tag_flags, TAG_WAITING);
      REMOVE_BIT (ch->pcdata->tag_flags, TAG_FROZEN);
      SET_BIT (ch->pcdata->tag_flags, TAG_RED);
      REMOVE_BIT (ch->pcdata->tag_flags, TAG_BLUE);
      SET_BIT (ch->act2, PLR2_PKSAFE);
      send_to_char ("You are on the {RRED team!\n\r", ch);
      tag_channel (ch, "{R[{WFreeze Tag{R] {C$n{C is now on the {RRED {Cteam!{x");
    }
    else
    {
      SET_BIT (ch->pcdata->tag_flags, TAG_WAITING);
      REMOVE_BIT (ch->pcdata->tag_flags, TAG_FROZEN);
      SET_BIT (ch->pcdata->tag_flags, TAG_BLUE);
      REMOVE_BIT (ch->pcdata->tag_flags, TAG_RED);
      SET_BIT (ch->act2, PLR2_PKSAFE);
      send_to_char ("You are on the {BBLUE {xteam!\n\r", ch);
      tag_channel (ch, "{R[{WFreeze Tag{R] {C$n{C is now on the {BBLUE {Cteam!");
    }
    SET_BIT (ch->pcdata->tag_flags, TAG_WAITING);
    return;
  }
/*
  if (!str_cmp (arg1, "info"))
  {
      if(tag_game.status == TAG_OFF)
      {
    sprintf (buf, "The next game of freeze tag will start in %d minute%s.\n\r",
	      tag_game.next, tag_game.next == 1 ? "" : "s");
    send_to_char(buf, ch);
      }
      else
          send_to_char("A tag game is currently playing.\n\r", ch);
    return;
  }
*/
  if (!str_cmp (arg1, "start"))
  {

    if (!IS_IMMORTAL (ch) && ch->platinum < 200)
    {
        send_to_char("You need 200 platinum to start a freeze tag game.\n\r", ch);
      return;
    }

    if (tag_game.status != TAG_OFF)
    {
      send_to_char ("A game has already started.\n\r", ch);
      return;
    }

    tag_channel (NULL, "{R[{WEvent{R] {CA Freeze Tag Game has started! Type '{cftag join{C' to play!{x");
    tag_game.status = TAG_ISWAIT;
    tag_game.timer = 3;
    tag_game.playing = 0;
    tag_game.next = -1;
    if (!IS_IMMORTAL (ch))
      ch->platinum -= 200;
    return;
  }

  if (!IS_IMMORTAL (ch))
  {
    do_ftag (ch, "");
    return;
  }

  if (!str_cmp (arg1, "next"))
  {
    tag_game.next = atoi (argument);
    sprintf (buf, "Next freeze tag game will start in %d ticks.\n\r",
	      tag_game.next);
    send_to_char(buf, ch);
    return;
  }
  if (!str_cmp (arg1, "reset"))
  {
    end_tag ();
    send_to_char ("All players reset.\n\r", ch);
    return;
  }

  argument = one_argument (argument, arg2);
  if (arg2[0] == '\0' || (str_cmp (arg1, "red") && str_cmp (arg1, "blue")))
  {
    send_to_char ("Syntax:  ftag red <player>\n\r", ch);
    send_to_char ("         ftag blue <player>\n\r", ch);
    return;
  }

  if (tag_game.status == TAG_ISPLAY)
  {
    send_to_char ("The tag game has already started.\n\r", ch);
    return;
  }
  if ((victim = get_char_world (ch, arg2)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }

  if (IS_NPC (victim))
  {
    send_to_char ("They can't play.\n\r", ch);
    return;
  }

  if (!str_cmp (arg1, "red"))
  {
    SET_BIT (victim->pcdata->tag_flags, TAG_WAITING);
    REMOVE_BIT (victim->pcdata->tag_flags, TAG_FROZEN);
    SET_BIT (victim->pcdata->tag_flags, TAG_RED);
    REMOVE_BIT (victim->pcdata->tag_flags, TAG_BLUE);
    act ("You are on the {RRED{x team!", ch, NULL, victim, TO_VICT);
    act ("$N is on the {RRED{x team!", ch, NULL, victim, TO_NOTVICT);
    act ("$N is on the {RRED{x team!", ch, NULL, victim, TO_CHAR);
  }
  else if (!str_cmp (arg1, "blue"))
  {
    SET_BIT (victim->pcdata->tag_flags, TAG_WAITING);
    REMOVE_BIT (victim->pcdata->tag_flags, TAG_FROZEN);
    SET_BIT (victim->pcdata->tag_flags, TAG_BLUE);
    REMOVE_BIT (victim->pcdata->tag_flags, TAG_RED);
    act ("You are on the {BBLUE{x team!", ch, NULL, victim, TO_VICT);
    act ("$N is on the {BBLUE{x team!", ch, NULL, victim, TO_NOTVICT);
    act ("$N is on the {BBLUE{x team!", ch, NULL, victim, TO_CHAR);
  }

  return;
}

void do_tag(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;

  if (IS_NPC (ch))
    return;

  if (tag_game.status == TAG_OFF)
  {
    send_to_char ("There is no tag game playing.\n\r", ch);
    return;
  }

  argument = one_argument (argument, arg);

  if (!IS_SET (ch->pcdata->tag_flags, TAG_PLAYING))
  {
    send_to_char ("You're not playing freeze tag.\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    send_to_char ("Tag whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room (ch, arg)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    send_to_char ("You tag yourself.  How amusing.\n\r", ch);
    return;
  }

  if (IS_NPC (victim))
  {
    send_to_char ("You can't tag them.\n\r", ch);
    return;
  }

  if (!IS_SET (victim->pcdata->tag_flags, TAG_PLAYING))
  {
    send_to_char ("They're not playing freeze tag.\n\r", ch);
    return;
  }

  if (IS_SET (ch->pcdata->tag_flags, TAG_FROZEN))
  {
    send_to_char ("You can't tag, you're frozen!\n\r", ch);
    return;
  }

  act ("$n tags you.", ch, NULL, victim, TO_VICT);
  act ("$n tags $N.", ch, NULL, victim, TO_NOTVICT);
  act ("You tag $N.", ch, NULL, victim, TO_CHAR);

  if ((IS_SET (ch->pcdata->tag_flags, TAG_RED) &&
       IS_SET (victim->pcdata->tag_flags, TAG_RED))
      || (IS_SET (ch->pcdata->tag_flags, TAG_BLUE) &&
	  IS_SET (victim->pcdata->tag_flags, TAG_BLUE)))
  {
    if (IS_SET (victim->pcdata->tag_flags, TAG_FROZEN))
    {
      REMOVE_BIT (victim->pcdata->tag_flags, TAG_FROZEN);
      act ("You are no longer frozen!", ch, NULL, victim, TO_VICT);
      act ("$N is no longer frozen!", ch, NULL, victim, TO_NOTVICT);
      act ("$N is no longer frozen!", ch, NULL, victim, TO_CHAR);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
     if ( d->connected == CON_PLAYING )
     {
       if ( is_tagging ( d->character ) )
	  printf_to_char ( d->character, "{R[{WEvent{R] {R%s {Chas just {Bunfrozen {R%s{C!!!{x\n\r",
		ch->pcdata->cname, victim->pcdata->cname );
     }
    }	 

    }
    else
    {
      act ("$N is not frozen!", ch, NULL, victim, TO_CHAR);
    }
  }
  else
  {
    if (IS_SET (victim->pcdata->tag_flags, TAG_FROZEN))
    {
      act ("$N is already frozen!", ch, NULL, victim, TO_CHAR);
    }
    else
    {

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
     if ( d->connected == CON_PLAYING )
     {
       if ( is_tagging ( d->character ) )
	  printf_to_char ( d->character, "{R[{WEvent{R] {R%s {Chas just {Yfrozen {R%s{C!!!{x\n\r",
		ch->pcdata->cname, victim->pcdata->cname );
     }
    }	 

      SET_BIT (victim->pcdata->tag_flags, TAG_FROZEN);
      act ("You are frozen!", ch, NULL, victim, TO_VICT);
      act ("$N is frozen!", ch, NULL, victim, TO_NOTVICT);
      act ("$N is frozen!", ch, NULL, victim, TO_CHAR);
      check_team_frozen (victim);
    }
  }

  return;
}

void auto_tag (void)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ch;
  int count = 0;

  if ( tag_game.status != TAG_OFF )
  return;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
     if ( d->connected == CON_PLAYING && ( ch = ( d->original ? d->original : d->character ) ) != NULL )
     {
	++count;
     }
    }
  if ( count < 8 ) return;

  tag_channel (NULL, "{R[{WEvent{R] {CA Freeze Tag Game has started! Type '{cftag join{C' to play!{x");
  tag_game.status = TAG_ISWAIT;
  tag_game.timer = 3;
  tag_game.playing = 0;
  tag_game.next = -1;
}

void tag_update ( void )
{
    char buf[MAX_STRING_LENGTH];

    if ( tag_game.next > 0 && tag_game.status == TAG_OFF )
    {
        /*if ( --tag_game.next == 0 )
            auto_tag (  ); */
    } 

    else if ( tag_game.status == TAG_ISWAIT )
    {

        tag_game.timer--;

        if ( tag_game.timer > 0 )
        {
            sprintf ( buf, "{R[{WFreeze Tag{R] {C%d minute%s left to join freeze tag.{x", tag_game.timer,
                       tag_game.timer == 1 ? "" : "s" );
            tag_channel(NULL, buf);
        }
        else
        {
            if ( tag_game.playing < 4 )
            {
                end_tag (  );
                sprintf ( buf, "{R[{WFreeze Tag{R] {CNot enough people for freeze tag. Game aborted.{x" );
                tag_channel(NULL, buf);
                return;
            }
            else
            {
                start_tag (  );
            }
        }
    }
    else if ( tag_game.status == TAG_ISPLAY )
    {
        if ( tag_game.playing == 0 )
        {
            end_tag (  );
            sprintf ( buf, "{R[{WFreeze Tag{R] {CNo one left in freeze tag, next game in %d minutes.{x",
                       tag_game.next );
            tag_channel(NULL, buf);
            return;
        }
        switch ( tag_game.timer )
        {
            case 0:
                end_tag (  );
                sprintf ( buf, "{R[{WFreeze Tag{R] {CTime has run out for freeze tag, next game will start in %d minutes.{x",
                           tag_game.next );
                tag_channel(NULL, buf);
                return;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 10:
            case 15:
                sprintf ( buf, "{R[{WFreeze Tag{R] {C%d minute%s remaining in freeze tag.{x",
                           tag_game.timer, tag_game.timer > 1 ? "s" : "" );
                tag_channel(NULL, buf);
            default:
                tag_game.timer--;
                break;
        }
    }
}


bool is_tagging (CHAR_DATA * ch)
{
  if (!ch || IS_NPC (ch))
    return FALSE;

  if (IS_SET (ch->pcdata->tag_flags, TAG_PLAYING | TAG_WAITING)
      && tag_game.status != TAG_OFF)
    return TRUE;

  return FALSE;
}

void tag_channel(CHAR_DATA *ch, char *message)
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *dch;
    char buf[MAX_INPUT_LENGTH];

    for(d = descriptor_list; d != NULL; d = d->next)
    {
        if(d->connected != CON_PLAYING)
            continue;

        if((dch = d->character) == NULL)
            continue;

        sprintf(buf, "%s", message);

        if(ch) /* don't use $N only $n in message */
            act(buf, ch, NULL, dch, TO_VICT);
        else
        {
            strcat(buf, "\n\r");
            send_to_char(buf, dch);
        }
    }
}
