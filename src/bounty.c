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
#include <string.h>
#include <unistd.h>
#include "sd.h"
#include "recycle.h"

/* Global variables */
BOUNTY_DATA	*bounty_list;

/* Function definitions */
int     clan_lookup     args( (const char *name) );

/* And some local defines */
#define BOUNTY_PLACE_CHARGE	3
#define BOUNTY_REMOVE_CHARGE	100
#define BOUNTY_ADD_CHARGE	5
#define MIN_BOUNTY		500

void save_bounties()
{
  FILE *fp=NULL;
  BOUNTY_DATA *bounty;
  bool found = FALSE;

  if ( ( fp = fopen( BOUNTY_FILE, "w" ) ) == NULL )
  {
    perror( BOUNTY_FILE );
  }

  for( bounty = bounty_list; bounty != NULL; bounty = bounty->next)
  {
    found = TRUE;
    fprintf(fp,"%-12s %d\n",bounty->name,bounty->amount);
  }

  new_fclose(fp);
  if(!found)
    unlink(BOUNTY_FILE);
}

void load_bounties()
{
  FILE *fp=NULL;
  BOUNTY_DATA *blist;

  if ( ( fp = fopen( BOUNTY_FILE, "r" ) ) == NULL )
	return;

  blist = NULL;
  for( ; ; )
  {
    BOUNTY_DATA *bounty;
    if ( feof(fp) )
    {
      new_fclose(fp);
      return;
    }

    bounty = new_bounty();

    bounty->name = str_dup(fread_word(fp));
    bounty->amount = fread_number(fp);
    fread_to_eol(fp);

    if(bounty_list == NULL)
	bounty_list = bounty;
    else
	blist->next = bounty;
    blist = bounty;
  }
}

bool is_bountied(CHAR_DATA *ch)
{
  BOUNTY_DATA *bounty;
  bool found = FALSE;

  if(IS_NPC(ch))
    return FALSE;

  for(bounty = bounty_list; bounty != NULL; bounty = bounty->next)
  {
    if( !str_cmp(ch->name,bounty->name) )
      found = TRUE;
  }

  return found;
}

static OBJ_DATA *rgObjNest[MAX_NEST];

void do_bounty(CHAR_DATA *ch, char *argument)
{
  FILE *fp=NULL;
  CHAR_DATA *vch;
  BOUNTY_DATA *bounty;
  BOUNTY_DATA *prev=NULL;
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
/*  char strsave[MAX_STRING_LENGTH];
  char *word;
  bool found = FALSE;
  bool clanned = FALSE; */
  int cash, amount, surcharge;

  if( IS_NPC(ch) )
    return;

  /* Check for the bounty collector */
  for(vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
  {
    if ( !IS_NPC ( vch ) )
      continue;    
    if(IS_SET(vch->act2, ACT2_BOUNTY))
      break;
  }

  if(vch == NULL)
  {
    send_to_char("You cannot do that here.\n\r",ch);
    return;
  }


  /* Only clanned chars/pkillers can place bounties. */
/*  if( !is_clan(ch) )
  {
    do_mob_tell ( ch, vch, "You must be in a clan to place or collect a bounty.\n\r" );
    return;
  } */
  if ( !is_real_pkiller ( ch ) )
  { 
    do_mob_tell ( ch, vch, "Pfftt... ONLY real pkillers collect bounties, fool!" );
    return;
  }

  argument = one_argument(argument, arg);
  one_argument(argument, arg2);

  if(arg[0] == '\0')
  {
    bool found = FALSE;
    BUFFER2 *output = buffer_new ( 1337 );
    do_mob_tell ( ch, vch, "I currently have these bounties registered in my book;");
    buffer_strcat(output,"\n\r  {rName              Bounty\n\r");
    buffer_strcat(output,"  {r============      ==================={x\n\r");
    
    for(bounty = bounty_list;bounty != NULL;bounty=bounty->next)
    {
      sprintf(buf,"  {W%-12s      {C%d platinum{x\n\r",bounty->name,bounty->amount);
      buffer_strcat(output,buf);
      found = TRUE;
    }
    if ( !found )
    buffer_strcat ( output, "  None at the moment.\n\r" );
    page_to_char ( buffer_string ( output ), ch );
    buffer_free ( output );
    return;
  }

  if( arg2[0] == '\0' 
  ||  (str_cmp(arg,"remove") && str_cmp(arg,"collect") && !is_number(arg2)) )
  {
    send_to_char("Syntax: bounty\n\r",ch);
    send_to_char("        bounty remove <player>\n\r",ch);
    send_to_char("        bounty collect <player>\n\r",ch);
    send_to_char("        bounty <player> <amount>\n\r",ch);
    return;
  }

  /* For collecting bounties. */
  if( !str_cmp(arg,"collect") )
  {
    OBJ_DATA *obj;

    for(bounty = bounty_list; bounty != NULL; prev=bounty, bounty=bounty->next)
    {
      if( !str_cmp(capitalize(arg2),capitalize(bounty->name)) )
	break;
    }

    if( bounty == NULL )
    {
      do_mob_tell ( ch, vch, "That individual is not in the books.");
      return;
    }

    for(obj = ch->carrying; obj != NULL; obj = obj->next)
    {
      if( is_name(arg2,obj->name)
      &&  (obj->pIndexData->vnum == OBJ_VNUM_SEVERED_BOUNTY_HEAD) )
	break;
    }

    if( obj == NULL )
    {
      do_mob_tell ( ch, vch, "I'm afraid I'll need proof of the death.");
      return;
    }

    extract_obj(obj);
    do_mob_tell ( ch, vch, "Very well, here is your payment.");
    
    sprintf ( buf, "{Djust got {W%d{D platinum added to their bank account for slaying {R%s{D and collecting their bounty!", bounty->amount, bounty->name );
    do_announce ( ch, buf );
    
    ch->pcdata->balance += bounty->amount;

    /*
     * OK, we've got a valid bounty and they've
     * been paid.  Time to remove the bounty.
     */
    if(prev == NULL)
      bounty_list = bounty_list->next;
    else
      prev->next = bounty->next;

    free_bounty(bounty);
    save_bounties();
    return;
  }

  /* For buying off bounties. */
  if( !str_cmp(arg,"remove") )
  {
    for(bounty = bounty_list; bounty != NULL; prev=bounty, bounty=bounty->next)
    {
      if( !str_cmp(arg2,bounty->name) )
	break;
    }

    if( bounty == NULL )
    {
      do_mob_tell ( ch, vch, "That individual is not in the books.");
      return;
    }

    surcharge = BOUNTY_REMOVE_CHARGE;
    amount = bounty->amount+(bounty->amount*surcharge/100);
    //cash = ch->gold + (ch->silver/100) + ch->pcdata->bank_gold + (ch->pcdata->bank_silver/100);
    cash = ch->pcdata->balance;

    if(cash < amount)
    {
      sprintf ( buf, "It would cost you %d platinum. You don't have enough money.", amount );
      do_mob_tell ( ch, vch, buf);
      return;
    }

/*    if(ch->pcdata->bank_silver < (amount*100))
    {
      amount -= (ch->pcdata->bank_silver/100);
      ch->pcdata->bank_silver %= 100;
    }
    else
    {
      ch->pcdata->bank_silver -= amount*100;
      amount = 0;
    }

    if(ch->pcdata->bank_gold < amount)
    {
      amount -= ch->pcdata->bank_gold;
      ch->pcdata->bank_silver = 0;
    }
    else
    {
      ch->pcdata->bank_gold -= amount;
      amount = 0;
    }

    if(ch->silver < (amount*100))
    {
      amount -= (ch->silver/100);
      ch->silver %= 100;
    }
    else
    {
      ch->silver -= amount*100;
      amount = 0;
    }

    if(ch->gold < amount)
    {
      amount -= ch->gold;
      ch->gold = 0;
    }
    else
    {
      ch->gold -= amount;
      amount = 0;
    }
*/
    ch->pcdata->balance -= amount;
    /*
     * OK, we've got a valid bounty and they can
     * pay.  Time to remove the bounty.
     */
    sprintf ( buf, "{Dhas just removed {R%s{D's {W%d{D-platinum bounty for {W%d{D platinum.{x", bounty->name, bounty->amount, amount );
    do_announce ( ch, buf );
    if(prev == NULL)
      bounty_list = bounty_list->next;
    else
      prev->next = bounty->next;

    free_bounty(bounty);
    do_mob_tell ( ch, vch, "The name has been removed from the books.");
    save_bounties();
    return;
  }

  /*
   * This code allows you to bounty those
   * offline as well as those not visible.
   */
/*
  sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(arg));
  if ( ( fp = fopen( strsave, "r" ) ) == NULL )
  {
    do_mob_tell ( ch, vch, "I am afraid there is no such person.");
    return;
  }

  strcpy(arg,capitalize(arg));


  while( !found )
  {
    char *clanName;

    word = feof(fp) ? "End" : fread_word( fp );

    KEY ( "Levl", ch->level, fread_number ( fp ) );
    if( !str_cmp(word, "Clan") )
    {
      found = TRUE;
      clanName = fread_string(fp);
      clanned = clan_lookup(clanName) ? TRUE : FALSE;
      free_string(clanName);
    }
    else if( !str_cmp(word, "End") )
    {
      found = TRUE;
    }
    else
    {
      fread_to_eol( fp );
    }
  }

  new_fclose( fp );
*/
    strcpy(arg,capitalize(arg));
    
    CHAR_DATA *victim = new_char ( );
    victim->pcdata = new_pcdata();
    bool fOld = FALSE;

    sprintf( buf, "%s%s", PLAYER_DIR, arg );

    if ( ( fp = fopen( buf, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	fOld = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if      ( !str_cmp( word, "PLAYER" ) ) fread_char( victim, fp );
	    else if ( !str_cmp( word, "OBJECT" ) ) break;
	    else if ( !str_cmp( word, "O"      ) ) break;
	    else if ( !str_cmp( word, "PET"    ) ) break;
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	new_fclose( fp );
    }
    if ( !fOld )
    {
        do_mob_tell ( ch, vch, "I am afraid there is no such person.");
	free_pcdata(victim->pcdata);
	free_char(victim);
	return;
    }
    if ( IS_IMMORTAL ( victim ) )
    {
        do_mob_tell ( ch, vch, "Putting bounties on immortals is not allowed!");
        do_mob_tell ( ch, vch, "You should show some respect!");
	free_pcdata(victim->pcdata);
	free_char(victim);
	return;
    }

  /* Only clanned chars can be bountied */
  if ( !is_real_pkiller ( victim ) )
  {
    do_mob_tell ( ch, vch, "You cannot place bounties on NPK/Not-clanned characters or Pknewbies.");
    return;
  }
  free_pcdata(victim->pcdata);
  free_char(victim);

/*  if( !clanned )
  {
    do_mob_tell ( ch, vch, "You cannot place bounties on non-clanned characters.");
    return;
  } */

  /* Minimum bounty, change as desired */
  if( (amount = atoi(arg2)) < MIN_BOUNTY)
  {
    sprintf(buf,"I do not accept bounties under %d platinum.",MIN_BOUNTY);
    do_mob_tell ( ch, vch, buf);
    return;
  }

  /*
   * Char's cash total in gold, bounties
   * are always set in gold. Notice the
   */
  //cash = ch->gold + (ch->silver/100) + ch->pcdata->bank_gold + (ch->pcdata->bank_silver/100);
  cash = ch->pcdata->balance;

  /*
   * Does the bounty already exist?
   * Let's find out.
   */
  for(bounty = bounty_list;bounty != NULL;bounty = bounty->next)
  {
    if( !str_cmp(bounty->name,arg) )
      break;
  }

  /*
   * Either create a new bounty
   * or add to an existing one.
   * Notice the charge added to
   * place a bounty and the
   * additional one to add to a
   * bounty.  Don't want to make
   * it too cheap.  Again, change
   * as desired.
   */

  if(bounty == NULL)
  {
    surcharge = BOUNTY_PLACE_CHARGE;
    if(cash < (amount+(amount*surcharge/100)))
    {
      do_mob_tell ( ch, vch, "You cannot afford to place that bounty.");
      return;
    }
    if ( is_same_clan ( ch, vch ) )
    {
      send_to_char ( "Why would you want to bounty someone IN YOUR OWN CLAN?\n\r", ch );
      return;
    }
    bounty = new_bounty();
    bounty->name = str_dup(arg);
    bounty->amount = amount;
    bounty->next = bounty_list;
    bounty_list = bounty;
    amount += amount*surcharge/100;
    sprintf ( buf, "{Dhas just added a {W%d{D-platinum bounty for {R%s{D's head for {W%d{D platinum.{x", bounty->amount, bounty->name, amount );
    do_announce ( ch, buf );
  }
  else
  {
    sprintf(buf,"There is a %d%% surcharge to add to an existing bounty.",BOUNTY_ADD_CHARGE);
    do_mob_tell ( ch, vch ,buf);
    surcharge = BOUNTY_PLACE_CHARGE + BOUNTY_ADD_CHARGE;
    amount += amount*surcharge/100;
    if(cash < amount)
    {
      do_mob_tell ( ch, vch, "You cannot afford to place that bounty.");
      return;
    }
    sprintf ( buf, "{Dhas just added {W%d{D platinum to {R%s{D's existing bounty at {W%d{D platinum{x", amount, bounty->name, bounty->amount );
    do_announce ( ch, buf );      
    bounty->amount += amount;
  }

  save_bounties();

  /*
   * Now make them pay, start with bank
   * then go to cash on hand.  Also use
   * up that annoying silver first.
   */
/*  if(ch->pcdata->bank_silver < (amount*100))
  {
    amount -= (ch->pcdata->bank_silver/100);
    ch->pcdata->bank_silver %= 100;
  }
  else
  {
    ch->pcdata->bank_silver -= amount*100;
    amount = 0;
  }

  if(ch->pcdata->bank_gold < amount)
  {
    amount -= ch->pcdata->bank_gold;
    ch->pcdata->bank_silver = 0;
  }
  else
  {
    ch->pcdata->bank_gold -= amount;
    amount = 0;
  }

  if(ch->silver < (amount*100))
  {
    amount -= (ch->silver/100);
    ch->silver %= 100;
  }
  else
  {
    ch->silver -= amount*100;
    amount = 0;
  }

  if(ch->gold < amount)
  {
    amount -= ch->gold;
    ch->gold = 0;
  }
  else
  {
    ch->gold -= amount;
    amount = 0;
  } */

  ch->pcdata->balance -= amount;
  do_mob_tell ( ch, vch, "Your bounty has been recorded in the books.");
}
