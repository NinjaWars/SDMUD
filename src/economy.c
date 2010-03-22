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


/*
 * The Mythran Mud Economy Snippet Version 2 (used to be banking.c)
 *
 * Copyrights and rules for using the economy system:
 *
 *      The Mythran Mud Economy system was written by The Maniac, it was
 *      loosly based on the rather simple 'Ack!'s banking system'
 *
 *      If you use this code you must follow these rules.
 *              -Keep all the credits in the code.
 *              -Mail Maniac (v942346@si.hhs.nl) to say you use the code
 *              -Send a bug report, if you find 'it'
 *              -Credit me somewhere in your mud.
 *              -Follow the envy/merc/diku license
 *              -If you want to: send me some of your code
 *
 * All my snippets can be found on http://www.hhs.nl/~v942346/snippets.html
 * Check it often because it's growing rapidly  -- Maniac --
 */

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "sd.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

CH_CMD ( do_bank )
{
    /* The Mythran mud economy system (bank and trading) * * based on: * Simple 
       banking system. by -- Stephen -- * * The following changes and additions 
       where * made by the Maniac from Mythran Mud * (v942346@si.hhs.nl) * *
       History: * 18/05/96: Added the transfer option, enables chars to
       transfer * money from their account to other players' accounts *
       18/05/96: Big bug detected, can deposit/withdraw/transfer * negative
       amounts (nice way to steal is * bank transfer -(lots of dogh) * Fixed it 
       (thought this was better... -= Maniac =-) * 21/06/96: Fixed a bug in
       transfer (transfer to MOBS) * Moved balance from ch->balance to
       ch->pcdata->balance * 21/06/96: Started on the invest option, so players 
       can invest * money in shares, using buy, sell and check * Finished
       version 1.0 releasing it monday 24/06/96 * 24/06/96: Mythran Mud Economy 
       System V1.0 released by Maniac * */
    /* 
       BUG FIXES: * 23/07/97: Make it less screwy in general by fixing it so
       that * do_say actually executes, plus cleaned up some of * the
       unnessecary \n\r's.  --Jouster */

    CHAR_DATA *mob = NULL;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    if ( IS_NPC ( ch ) )
    {
        do_mob_tell ( ch, mob, "Banking Services are only available to players!" );
        return;
    }

    /* Check for mob with act->banker */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {

        if ( IS_NPC ( mob ) )   /* 
                                   if ( mob->pIndexData->vnum ==
                                   MOB_VNUM_BANKER ) */
            if ( IS_SET ( mob->act2, ACT2_BANKER ) )
                break;
    }

    if ( mob == NULL )
    {
        send_to_char ( "You can't do that here.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Bank Options:\n\r\n\r", ch );
        send_to_char ( "Bank balance: Displays your balance.\n\r", ch );
        send_to_char ( "Bank deposit : Deposit platinum into your account.\n\r",
                       ch );
        send_to_char
            ( "Bank withdraw : Withdraw platinum from your account.\n\r", ch );
        send_to_char ( "Bank transfer  : Transfer platinum to account.\n\r",
                       ch );
        send_to_char ( "Bank buy #: Buy # shares\n\r", ch );
        send_to_char ( "Bank sell #: Sell # shares\n\r", ch );
        send_to_char ( "Bank check: Check the current rates of the shares.\n\r",
                       ch );
        return;
    }

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    /* Now work out what to do... */
    if ( !str_prefix ( arg1, "balance" ) )
    {
        sprintf ( buf, "Your current balance is: %ld Platinum.",
                  ch->pcdata->balance );
        do_mob_tell ( ch, mob, buf );        
	return;
    }

    if ( !str_prefix ( arg1, "deposit" ) )
    {
        int amount;

        if ( is_number ( arg2 ) )
        {
            amount = atoi ( arg2 );
            if ( amount < 0 || amount > 10000000 ||
                 ( amount + ch->pcdata->balance > 10000000 ) )
            {
                do_mob_tell ( ch, mob, "You can only deposit up to 10000000 platinum." );
                return;
            }

            if ( amount > ch->platinum )
            {
                sprintf ( buf,
                          "How can you deposit %d Platinum when you only have %ld?",
                          amount, ch->platinum );
	        do_mob_tell ( ch, mob, buf );        
                return;
            }

            if ( amount < 0 )
            {
                do_mob_tell ( ch, mob, "Only positive amounts allowed..." );        
                return;
            }

            ch->platinum -= amount;
            ch->pcdata->balance += amount;
            sprintf ( buf,
                      "You deposit %d Platinum.  Your new balance is %ld Platinum.",
                      amount, ch->pcdata->balance );
	    do_mob_tell ( ch, mob, buf );
            return;
        }
    }
    /* We only allow transfers if this is true... so define it... */

    if ( !str_prefix ( arg1, "transfer" ) )
    {
        int amount;
        CHAR_DATA *victim;

        if ( is_number ( arg2 ) )
        {
            amount = atoi ( arg2 );
            if ( amount > ch->pcdata->balance )
            {
                sprintf ( buf,
                          "How can you transfer %d Platinum when your balance is %ld?",
                          amount, ch->pcdata->balance );
		do_mob_tell ( ch, mob, buf );
                return;
            }

            if ( amount < 0 )
            {
                do_mob_tell ( ch, mob, "Only positive amounts allowed..." );
                return;
            }

            if ( !( victim = get_char_world ( ch, argument ) ) )
            {
                sprintf ( buf, "%s doesn't have a bank account.", argument );
                do_mob_tell ( ch, mob, buf );
                return;
            }

            if ( IS_NPC ( victim ) )
            {
                do_mob_tell ( ch, mob, "You can only transfer money to players." );
                return;
            }

            ch->pcdata->balance -= amount;
            victim->pcdata->balance += amount;
            sprintf ( buf,
                      "You transfer %d Platinum. Your new balance is %ld Platinum.",
                      amount, ch->pcdata->balance );
            do_mob_tell ( ch, mob, buf );
            sprintf ( buf,
                      "[BANK] %s has transferred %d Platinum's to your account.",
                      ch->name, amount );
            do_mob_tell ( ch, mob, buf );
            return;
        }
    }

    if ( !str_prefix ( arg1, "withdraw" ) )
    {
        int amount;

        if ( is_number ( arg2 ) )
        {
            amount = atoi ( arg2 );
            if ( amount > ch->pcdata->balance )
            {
                sprintf ( buf,
                          "How can you withdraw %d Platinum when your balance is %ld?",
                          amount, ch->pcdata->balance );
                do_mob_tell ( ch, mob, buf );
                return;
            }

            if ( amount < 0 )
            {
                do_mob_tell ( ch, mob, "Only positive amounts allowed..." );
                return;
            }

            ch->pcdata->balance -= amount;
            ch->platinum += amount;
            sprintf ( buf,
                      "You withdraw %d Platinum.  Your new balance is %ld Platinum.",
                      amount, ch->pcdata->balance );
            do_mob_tell ( ch, mob, buf );
            return;
        }
    }

    /* If you want to have an invest option... define BANK_INVEST */

    if ( !str_prefix ( arg1, "buy" ) )
    {
        int amount;
        int oldgold = 0;
        int oldbal = 0;

        if ( is_number ( arg2 ) )
        {
            amount = atoi ( arg2 );

            if ( amount > 10000 )
            {
                do_mob_tell ( ch, mob, "You can only buy 10000 shares at a time." );
                return;
            }

            if ( ch->gold + ( ch->platinum * 100 ) < ( amount * share_value ) )
            {
                do_mob_tell ( ch, mob, "You do not have enough gold" );
                return;
            }
            TRANSFORM_TO_GOLD ( ch, oldbal, oldgold );

            if ( ( amount * share_value ) > ch->gold )
            {
                sprintf ( buf,
                          "%d shares will cost you %d, put more money into your account.",
                          amount, ( amount * share_value ) );
                do_mob_tell ( ch, mob, buf );
                TRANSFORM_FROM_GOLD ( ch, oldbal, oldgold );
                return;
            }

            if ( amount < 0 )
            {
                do_mob_tell ( ch, mob, "If you want to sell shares you have to say so..." );
                TRANSFORM_FROM_GOLD ( ch, oldbal, oldgold );
                return;
            }

            ch->gold -= ( amount * share_value );
            ch->pcdata->shares += amount;
            sprintf ( buf,
                      "You buy %d shares for %d gold.  You now have %ld shares.",
                      amount, ( amount * share_value ), ch->pcdata->shares );
            do_mob_tell ( ch, mob, buf );
            TRANSFORM_FROM_GOLD ( ch, oldbal, oldgold );
            return;
        }
    }

    if ( !str_prefix ( arg1, "sell" ) )
    {
        int amount;

        if ( is_number ( arg2 ) )
        {

            amount = atoi ( arg2 );

            if ( amount > 10000 )
            {
                do_mob_tell ( ch, mob, "You can only sell 10000 shares at a time." );
                return;
            }

            if ( amount > ch->pcdata->shares )
            {
                sprintf ( buf, "You only have %ld shares.", ch->pcdata->shares );
                do_mob_tell ( ch, mob, buf );
                return;
            }

            if ( amount < 0 )
            {
                do_mob_tell ( ch, mob, "If you want to buy shares you have to say so..." );
                return;
            }

            add_cost ( ch, ( amount * share_value ), VALUE_GOLD );
            ch->pcdata->shares -= amount;
            sprintf ( buf,
                      "You sell %d shares for %d gold, you now have %ld shares.",
                      amount, ( amount * share_value ), ch->pcdata->shares );
	    if ( ch->platinum >= 50000 )
	    {
               do_mob_tell ( ch, mob, "Oh no! I notice you have more than 50000 platinum on you." );
               do_mob_tell ( ch, mob, "The money I just gave back to you went on the floor cause you couldn't carry anymore!" );
	       send_to_char ( "A kid runs past you with an incredible speed, taking your money with him!\n\r", ch );	    
               do_mob_tell ( ch, mob, "Damn kids! I'm sorry but there is nothing I can do about your money." );
               do_mob_tell ( ch, mob, "You should deposit some of your money in my bank to ensure this doesn't happen again." );
	    }
            return;
        }
    }

    if ( !str_prefix ( arg1, "check" ) )
    {
        sprintf ( buf, "The going rate for shares of the Shattered Business Corporation is %d.",
                  share_value );
        do_mob_tell ( ch, mob, buf );
        if ( ch->pcdata->shares )
        {
            sprintf ( buf,
                      "You have %ld shares, (at %d a share), with a total worth of %ld gold.",
                      ch->pcdata->shares, share_value,
                      ( ch->pcdata->shares * share_value ) );
            do_mob_tell ( ch, mob, buf );
        }
        return;
    }

    do_mob_tell ( ch, mob, "I don't know what you mean" );
    do_bank ( ch, "" );         /* Generate Instructions */
    return;
}
