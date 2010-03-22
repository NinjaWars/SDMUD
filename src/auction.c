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

/* Auction.c */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "sd.h"
#include "recycle.h"

CH_CMD ( do_auction )
{
    AUCTION_DATA *auc;
    AUCTION_DATA *p;
    OBJ_DATA *obj = NULL;
    long minbid = 0;
    int count;
    char arg1[MAX_INPUT_LENGTH];
    char buf1[MAX_STRING_LENGTH];

    argument = one_argument ( argument, arg1 );

    if ( ch == NULL || IS_NPC ( ch ) )
        return;

    /* Requesting info on the item */
    if ( arg1[0] == '\0' || !str_cmp ( arg1, "info" ) )
    {
        if ( auction_list == NULL )
        {
            send_to_char ( "There's nothing up for auction right now.\n\r",
                           ch );
            return;
        }

        obj = auction_list->item;

        if ( !obj )
        {
            send_to_char ( "There's nothing up for auction right now.\n\r",
                           ch );
            return;
        }

        if ( ch == auction_list->owner && !IS_IMMORTAL ( ch ) )
        {
            sprintf ( buf1, "You're auctioning %s.\n\r", obj->short_descr );
            send_to_char ( buf1, ch );
            return;
        }

        show_obj_stats ( ch, obj );
        return;
    }                           /* Aborting the auction */
    else if ( !str_cmp ( arg1, "stop" ) && IS_IMMORTAL ( ch ) )
    {
        if ( auction_list == NULL )
        {
            send_to_char ( "There's nothing up for auction right now.\n\r",
                           ch );
            return;
        }

        auction_channel ( "{!A God has stopped the auction!{x\n\r", FALSE );

        if ( auction_list->high_bidder != NULL )
            auction_list->high_bidder->platinum += auction_list->platinum_held;

        if ( auction_list->item != NULL )
        {
            obj_to_char ( auction_list->item, ch );
            sprintf ( buf1, "%s appears in your hands.\n\r",
                      auction_list->item->short_descr );
            send_to_char ( buf1, ch );
        }

        reset_auc ( auction_list );
        return;
    }
    else if ( !str_cmp ( arg1, "talk" ) )
    {                           
       send_to_char ( "{YS{Wo{wr{Wr{Yy{x, this option is not currently available.{x\n\r", ch );
       return;
    }

/* Listing the items for sale */
    else if ( !str_cmp ( arg1, "list" ) )
    {
        if ( auction_list == NULL )
        {
            send_to_char ( "There is nothing up for auction right now!\n\r",
                           ch );
            return;
        }

        if ( auction_list->next == NULL )
        {
            do_auction ( ch, "info" );
            return;
        }

        for ( count = 1, auc = auction_list; auc; auc = auc->next, count++ )
        {
            if ( auc->item != NULL && auc->item->short_descr != NULL &&
                 auc->owner != NULL && auc->owner->name != NULL )
            {

                if ( !IS_IMMORTAL ( ch ) )
                    sprintf ( buf1,
                              "`{-({^%d{8.{x (`8Min Bid: {!%5ld{x) %s\n\r",
                              count, auc->min_bid, auc->item->short_descr );
                else
                    sprintf ( buf1,
                              "{8-({^%d{8.{x ({#N{3ame{8: {#%-20s {8Min Bid: {!%5ld{x) %s\n\r",
                              count, auc->owner->name, auc->min_bid,
                              auc->item->short_descr );

                send_to_char ( buf1, ch );
            }
        }
        return;
    }                           /* Bidding on an item */
    else if ( !str_cmp ( arg1, "off" ) )
    {
        SET_BIT ( ch->comm, COMM_NOAUCTION );
	send_to_char ( "Auction channel is now off.\n\r", ch );
	return;
    }
    else if ( !str_cmp ( arg1, "on" ) )
    {
        REMOVE_BIT ( ch->comm, COMM_NOAUCTION );
	send_to_char ( "Auction channel is now on.\n\r", ch );
	return;
    }
    else if ( !str_cmp ( arg1, "bid" ) )
    {
        long bid = 0;

        if ( auction_list == NULL )
        {
            send_to_char ( "There's nothing up for auction right now.\n\r",
                           ch );
            return;
        }

        obj = auction_list->item;

        if ( !obj )
        {
            send_to_char ( "There's nothing up for auction right now.\n\r",
                           ch );
            return;
        }

        if ( ch == auction_list->high_bidder )
        {
            send_to_char ( "You already have the highest bid!\n\r", ch );
            return;
        }

        if ( ch == auction_list->owner )
        {
            send_to_char ( "You cannot bid on your own items!\n\r", ch );
            return;
        }

        if ( argument[0] == '\0' )
        {
            send_to_char ( "How much did you say you were bidding?\n\r", ch );
            return;
        }

        bid = atol ( argument );

        if ( bid > ( ch->platinum ) )
            /*if ( ch->gold < bid ) */
        {
            send_to_char ( "You can't cover that bid.\n\r", ch );
            return;
        }

        if ( bid < auction_list->min_bid )
        {
            sprintf ( buf1, "The minimum bid is %ld platinum.\n\r",
                      auction_list->min_bid );
            send_to_char ( buf1, ch );
            return;
        }

        if ( bid <= auction_list->current_bid )
        {
            if ( bid < ( auction_list->current_bid + 10 ) )
            {
                send_to_char
                    ( "You have to outbid the current amount by atleast 10 platinum.",
                      ch );
                return;
            }

            sprintf ( buf1,
                      "You must bid above the current bid of %ld platinum.\n\r",
                      auction_list->current_bid );
            return;
        }

        sprintf ( buf1, "%ld platinum has been offered for %s.\n\r", bid,
                  auction_list->item->short_descr );
        auction_channel ( buf1, FALSE );

        if ( auction_list->high_bidder != NULL )
            auction_list->high_bidder->platinum += auction_list->platinum_held;

        deduct_cost ( ch, bid, VALUE_PLATINUM );

        auction_list->platinum_held = bid;
        auction_list->high_bidder = ch;
        auction_list->current_bid = bid;
        auction_list->status = 0;
        return;
    }
    /* Putting up an item for sale */

    if ( ( obj = get_obj_carry ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "You aren't carrying that item.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_NODROP ) )
    {
        send_to_char ( "You can't let go of that item.\n\r", ch );
        return;
    }

    if ( count_auc ( ch ) >= 3 )
    {
        send_to_char ( "You are only allowed to auction 3 items a time!\n\r",
                       ch );
        return;
    }

    if ( argument[0] != '\0' )
        minbid = atol ( argument );

    if ( minbid > 10000 )
        minbid = 10000;

    if ( auction_list == NULL )
    {
        auc = new_auction (  );
        auction_list = auc;
        auction_list->next = NULL;
    }
    else
    {
        auc = new_auction (  );

        for ( p = auction_list; p; p = p->next )
        {
            if ( p->next == NULL )
                p->next = auc;
            auc->next = NULL;
        }
    }

    auc->owner = ch;
    auc->item = obj;
    auc->current_bid = 0;
    auc->status = -1;
    if ( minbid > 0 )
        auc->min_bid = minbid;
    else
        auc->min_bid = 0;

    if ( auc == auction_list )
        update_auc (  );

    obj_from_char ( obj );
    return;
}

void update_auc ( void )
{
    char buf1[MAX_STRING_LENGTH];
    char temp[MAX_STRING_LENGTH];

    if ( auction_list != NULL && auction_list->item != NULL )
    {
        auction_list->status++;

        if ( auction_list->current_bid == 0 )
        {
            if ( auction_list->status == 0 )
            {
                sprintf ( buf1, "Now taking bids on %s%s",
                          auction_list->item->short_descr,
                          auction_list->min_bid > 0 ? "" : ".\n\r" );
                if ( auction_list->min_bid > 0 )
                {
                    sprintf ( temp, " ({8Min Bid {!%ld{x)\n\r",
                              auction_list->min_bid );
                    strcat ( buf1, temp );
                }

                auction_channel ( buf1, FALSE );
            }

            if ( auction_list->status == AUCTION_LENGTH )
            {
                sprintf ( buf1, "No bids on %s - item removed.\n\r",
                          auction_list->item->short_descr );
                auction_channel ( buf1, FALSE );

                obj_to_char ( auction_list->item, auction_list->owner );

                sprintf ( buf1, "%s is returned to you.\n\r",
                          capitalize ( auction_list->item->short_descr ) );
                send_to_char ( buf1, auction_list->owner );

                reset_auc ( auction_list );

                if ( auction_list != NULL )
                    update_auc (  );
                return;
            }

            if ( auction_list->status == AUCTION_LENGTH - 1 )
            {
                sprintf ( buf1,
                          "%s - going twice ({8No Bids Received{x).\n\r",
                          capitalize ( auction_list->item->short_descr ) );
                auction_channel ( buf1, FALSE );
                return;
            }

            if ( auction_list->status == AUCTION_LENGTH - 2 )
            {
                sprintf ( buf1, "%s - going once ({8No Bids Received{x).\n\r",
                          capitalize ( auction_list->item->short_descr ) );
                auction_channel ( buf1, FALSE );
                return;
            }
        }
        else
        {
            if ( auction_list->status == AUCTION_LENGTH )
            {
                sprintf ( buf1, "%s sold to %s for %ld platinum.\n\r",
                          capitalize ( auction_list->item->short_descr ),
                          auction_list->high_bidder->name,
                          auction_list->current_bid );
                auction_channel ( buf1, FALSE );

/*                auction_list->owner->platinum +=
                    ( auction_list->platinum_held * 9 ) / 10; */

                auction_list->owner->platinum += auction_list->platinum_held;

                sprintf ( temp, "You recieve %ld platinum coin%s.\n\r",
                          ( ( auction_list->platinum_held * 9 ) / 10 ) >
                          0 ? ( auction_list->platinum_held * 9 ) / 10 : 0,
                          ( ( auction_list->platinum_held * 9 ) / 10 ) !=
                          1 ? "s" : "" );
                send_to_char ( buf1, auction_list->owner );

                obj_to_char ( auction_list->item, auction_list->high_bidder );

                sprintf ( buf1, "%s appears in your hands.\n\r",
                          capitalize ( auction_list->item->short_descr ) );
                send_to_char ( buf1, auction_list->high_bidder );

                reset_auc ( auction_list );

                if ( auction_list != NULL )
                    update_auc (  );
                return;
            }

            if ( auction_list->status == AUCTION_LENGTH - 1 )
            {
                sprintf ( buf1, "%s - going twice at %ld platinum.\n\r",
                          capitalize ( auction_list->item->short_descr ),
                          auction_list->current_bid );
                auction_channel ( buf1, FALSE );
                return;
            }

            if ( auction_list->status == AUCTION_LENGTH - 2 )
            {
                sprintf ( buf1, "%s - going once at %ld platinum.\n\r",
                          capitalize ( auction_list->item->short_descr ),
                          auction_list->current_bid );
                auction_channel ( buf1, FALSE );
                return;
            }
        }
    }

    return;
}

void auction_channel ( char *msg, bool iauc )
{
    char buf1[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    sprintf ( buf1, "\n\r%s %s", iauc ? "{8({!I{1mmauctio{!n{8){x" : "{8({7Au{&cti{7on{8){x", msg );    /* Add
                                                                                                           color if you wish */

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
             !IS_SET ( victim->comm, COMM_NOAUCTION ) &&
             !IS_SET ( victim->comm, COMM_QUIET ) )
        {
            send_to_char ( buf1, victim );
        }
    }

    return;
}

void reset_auc ( AUCTION_DATA * auc )
{
    if ( !IS_VALID ( auc ) )
        return;

    auc->current_bid = 0;
    auc->platinum_held = 0;
    auc->high_bidder = NULL;
    auc->item = NULL;
    auc->min_bid = 0;
    auc->owner = NULL;
    auc->status = 0;

    if ( auc == auction_list )
    {
        if ( auc->next != NULL )
            auction_list = auc->next;
        else
            auction_list = NULL;

        free_auction ( auc );
        return;
    }

    free_auction ( auc );
    return;
}

int count_auc ( CHAR_DATA * ch )
{
    AUCTION_DATA *q;
    int count;

    q = auction_list;

    if ( !q )
        return 0;

    for ( count = 0; q; q = q->next )
    {
        if ( q->owner == ch )
            count++;
    }

    return count;
}

void show_obj_stats ( CHAR_DATA * ch, OBJ_DATA * obj )
{
    char buf1[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    sprintf ( buf1,
              "{^O{6bject {!%s{6 is type {^%s{6, extra flags {^%s{6.{x\n\r{^W{6eight is {^%d{6, value is {^%d{6, level is {#%d{6.{x\n\r",
              obj->name, item_name ( obj->item_type ),
              extra_bit_name ( obj->extra_flags ), obj->weight / 10,
              obj->cost, obj->level );
    send_to_char ( buf1, ch );

    switch ( obj->item_type )
    {
        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
            sprintf ( buf1, "{^L{6evel {#%ld {6spells of:", obj->value[0] );
            send_to_char ( buf1, ch );

            if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
            {
                send_to_char ( " {6", ch );
                send_to_char ( skill_table[obj->value[1]].name, ch );
                send_to_char ( "{6{x", ch );
            }

            if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
            {
                send_to_char ( " {6{^", ch );
                send_to_char ( skill_table[obj->value[2]].name, ch );
                send_to_char ( "{6{x", ch );
            }

            if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
            {
                send_to_char ( " {6{^", ch );
                send_to_char ( skill_table[obj->value[3]].name, ch );
                send_to_char ( "{6{x", ch );
            }

            if ( obj->value[4] >= 0 && obj->value[4] < MAX_SKILL )
            {
                send_to_char ( " {6{^", ch );
                send_to_char ( skill_table[obj->value[4]].name, ch );
                send_to_char ( "{6{x", ch );
            }

            send_to_char ( "{x.\n\r", ch );
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
            sprintf ( buf1, "{^H{6as {#%ld {6charges of level {^%ld{x",
                      obj->value[2], obj->value[0] );
            send_to_char ( buf1, ch );

            if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
            {
                send_to_char ( " {6{^", ch );
                send_to_char ( skill_table[obj->value[3]].name, ch );
                send_to_char ( "{6{x", ch );
            }

            send_to_char ( "{x.\n\r", ch );
            break;

        case ITEM_DRINK_CON:
            sprintf ( buf1, "{^I{6t holds %s{6-colored {^%s{x.\n\r",
                      liq_table[obj->value[2]].liq_color,
                      liq_table[obj->value[2]].liq_name );
            send_to_char ( buf1, ch );
            break;

        case ITEM_CONTAINER:
            sprintf ( buf1,
                      "{^C{6apacity: {^%ld{6#  Maximum weight: {^%ld{6#  flags: {^%s{x\n\r",
                      obj->value[0], obj->value[3],
                      cont_bit_name ( obj->value[1] ) );
            send_to_char ( buf1, ch );
            if ( obj->value[4] != 100 )
            {
                sprintf ( buf1, "{^W{6eight multiplier: {#%ld{x%%\n\r",
                          obj->value[4] );
                send_to_char ( buf1, ch );
            }
            break;

        case ITEM_WEAPON:
            send_to_char ( "{^W{6eapon type is ", ch );
            switch ( obj->value[0] )
            {
                case ( WEAPON_EXOTIC ):
                    send_to_char ( "{^exotic{6.{x\n\r", ch );
                    break;
                case ( WEAPON_SWORD ):
                    send_to_char ( "{^sword{6.{x\n\r", ch );
                    break;
                case ( WEAPON_DAGGER ):
                    send_to_char ( "{^dagger{6.{x\n\r", ch );
                    break;
                case ( WEAPON_SPEAR ):
                    send_to_char ( "{^spear{/{^staff{6.{x\n\r", ch );
                    break;
                case ( WEAPON_MACE ):
                    send_to_char ( "{^mace{/{^club6.{x\n\r", ch );
                    break;
                case ( WEAPON_AXE ):
                    send_to_char ( "{^axe{6.{x\n\r", ch );
                    break;
                case ( WEAPON_FLAIL ):
                    send_to_char ( "{^flail{6.{x\n\r", ch );
                    break;
                case ( WEAPON_WHIP ):
                    send_to_char ( "{^whip{6.{x\n\r", ch );
                    break;
                case ( WEAPON_POLEARM ):
                    send_to_char ( "{^polearm{x6.{x\n\r", ch );
                    break;
                default:
                    send_to_char ( "{!unknown{6.{x\n\r", ch );
                    break;
            }
            if ( obj->pIndexData->new_format )
                sprintf ( buf1,
                          "{^D{6amage is {!%ld{6d{!%ld {6({8average {#%ld{6).{x\n\r",
                          obj->value[1], obj->value[2],
                          ( 1 + obj->value[2] ) * obj->value[1] / 2 );
            else
                sprintf ( buf1,
                          "{^D{6amage is {!%ld {6to {!%ld {6({8average {#%ld{6).{x\n\r",
                          obj->value[1], obj->value[2],
                          ( obj->value[1] + obj->value[2] ) / 2 );
            send_to_char ( buf1, ch );
            if ( obj->value[4] )    /* weapon flags */
            {
                sprintf ( buf1, "{^W{6eapons flags: {^%s{x\n\r",
                          weapon_bit_name ( obj->value[4] ) );
                send_to_char ( buf1, ch );
            }
            break;

        case ITEM_ARMOR:
            sprintf ( buf1,
                      "{^A{6rmor class is {#%ld {8p{7i{&er{7c{8e{6, {#%ld {8b{7a{&s{8h{6, {#%ld {8s{7l{&a{7s{8h{6, and {#%ld {6vs. {4m{+a{4gi{+c{6.{x\n\r",
                      obj->value[0], obj->value[1], obj->value[2],
                      obj->value[3] );
            send_to_char ( buf1, ch );
            break;
    }

    if ( !obj->enchanted )
        for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        {
            if ( paf->location != APPLY_NONE && paf->modifier != 0 )
            {
                sprintf ( buf1, "{^A{6ffects {^%s {6by {#%d{6.{x\n\r",
                          affect_loc_name ( paf->location ), paf->modifier );
                send_to_char ( buf1, ch );
                if ( paf->bitvector )
                {
                    switch ( paf->where )
                    {
                        case TO_AFFECTS:
                            sprintf ( buf1, "{^A{6dds {!%s {6affect.{x\n\r",
                                      affect_bit_name ( paf->bitvector ) );
                            break;
                        case TO_OBJECT:
                            sprintf ( buf1,
                                      "{^A{6dds {!%s {6object flag.{x\n\r",
                                      extra_bit_name ( paf->bitvector ) );
                            break;
                        case TO_IMMUNE:
                            sprintf ( buf1,
                                      "{^A{6dds {8i{7m{&muni{7t{8y{6 to {!%s{6.{x\n\r",
                                      imm_bit_name ( paf->bitvector ) );
                            break;
                        case TO_RESIST:
                            sprintf ( buf1,
                                      "{^A{6dds {!r{1e{8s{7i{&st{7a{8n{1c{!e {6to {!%s{6.{x\n\r",
                                      imm_bit_name ( paf->bitvector ) );
                            break;
                        case TO_VULN:
                            sprintf ( buf1,
                                      "{^A{6dds {@v{2u{8l{7n{&erabi{7l{8i{2t{@y {6to {!%s{6.{x\n\r",
                                      imm_bit_name ( paf->bitvector ) );
                            break;
                        default:
                            sprintf ( buf1,
                                      "{!U{6nknown bit {#%d{1: {!%ld{1.{x\n\r",
                                      paf->where, paf->bitvector );
                            break;
                    }
                    send_to_char ( buf1, ch );
                }
            }
        }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        if ( paf->location != APPLY_NONE && paf->modifier != 0 )
        {
            sprintf ( buf1, "{^A{6ffects {^%s {6by {#%d{x",
                      affect_loc_name ( paf->location ), paf->modifier );
            send_to_char ( buf1, ch );
            if ( paf->duration > -1 )
                sprintf ( buf1, ", {!%d {6hours.{x\n\r", paf->duration );
            else
                sprintf ( buf1, "{6.{x\n\r" );
            send_to_char ( buf1, ch );
            if ( paf->bitvector )
            {
                switch ( paf->where )
                {
                    case TO_AFFECTS:
                        sprintf ( buf1, "{^A{6dds {!%s {6affect.{x\n\r",
                                  affect_bit_name ( paf->bitvector ) );
                        break;
                    case TO_OBJECT:
                        sprintf ( buf1, "{^A{6dds {!%s {6object flag.{x\n\r",
                                  extra_bit_name ( paf->bitvector ) );
                        break;
                    case TO_WEAPON:
                        sprintf ( buf1, "{^A{6dds {!%s {6weapon flags.{x\n\r",
                                  weapon_bit_name ( paf->bitvector ) );
                        break;
                    case TO_IMMUNE:
                        sprintf ( buf1,
                                  "{^A{6dds {8i{7m{&muni{7t{8y {6to {!%s{6.{x\n\r",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    case TO_RESIST:
                        sprintf ( buf1,
                                  "{^A{6dds {!r{1e{8s{7i{&st{7a{8n{1c{!e {6to {!%s{x6.{x\n\r",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    case TO_VULN:
                        sprintf ( buf1,
                                  "{^A{6dds {@v{2u{8l{7n{&erabi{7l{8i{2t{@y {6to {!%s{6.{x\n\r",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    default:
                        sprintf ( buf1,
                                  "{!U{1nknown bit {#%d{1: {!%ld{1.{x\n\r",
                                  paf->where, paf->bitvector );
                        break;
                }
                send_to_char ( buf1, ch );
            }
        }
    }

    return;
}
