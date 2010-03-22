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
 *  As the Wheel Weaves based on ROM 2.4. Original code by Dalsor, Caxandra,
 *	and Zy of AWW. See changes.log for a list of changes from the original
 *	ROM code. Credits for code created by other authors have been left
 *	intact at the head of each function.
 ***************************************************************************/

/* Begin homes.c */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include "sd.h"
#include "recycle.h"

/* Thanks to Kyndig and Sandi Fallon for assisting with arguments - Dalsor.
 * At the time I started on furnishings, I was about 12 hours straight into
 * the code and very brain dead. Thanks to them for pointing out my mistakes.
 */

/* Homes uses aspects of OLC, and it should be installed, or the necessary
 * componants installed in order for this to work
 */

/* Damn, this is a lot of defines :(
 * Each one of these vnums will need to be created. If this many items
 * aren't desired, removed the uneccesary ones and comment out the code
 */
#define HOME_ITEM_1   11800     /* fish */
#define HOME_ITEM_2   11801     /* turtle */
#define HOME_ITEM_3   11802     /* snake */
#define HOME_ITEM_4   11803     /* spider */
#define HOME_ITEM_5   11804     /* carved chair */
#define HOME_ITEM_6   11805     /* highback chair */
#define HOME_ITEM_7   11806     /* hard chair */
#define HOME_ITEM_8   11807     /* sturdy chair */
#define HOME_ITEM_9   11808     /* oak desk */
#define HOME_ITEM_10  11809     /* pine desk */
#define HOME_ITEM_11  11810     /* carved desk */
#define HOME_ITEM_12  11811     /* polished desk */
#define HOME_ITEM_13  11812     /* teak desk */
#define HOME_ITEM_14  11813     /* plain desk */
#define HOME_ITEM_15  11814     /* plush sofa */
#define HOME_ITEM_16  11815     /* padded sofa */
#define HOME_ITEM_17  11816     /* wood bench */
#define HOME_ITEM_18  11817     /* fluffed sofa */
#define HOME_ITEM_19  11818     /* comfy sofa */
#define HOME_ITEM_25  11824     /* oak dining table */
#define HOME_ITEM_26  11825     /* pine table */
#define HOME_ITEM_27  11826     /* carved table */
#define HOME_ITEM_28  11827     /* polished table */
#define HOME_ITEM_29  11828     /* teak table */
#define HOME_ITEM_30  11829     /* comfy recliner */
#define HOME_ITEM_31  11830     /* lamp */
#define HOME_ITEM_32  11831     /* lantern */
#define HOME_ITEM_33  11832     /* torch */
#define HOME_ITEM_34  11833     /* oak dresser */
#define HOME_ITEM_35  11834     /* pine dresser */
#define HOME_ITEM_36  11835     /* carved dresser */
#define HOME_ITEM_37  11836     /* polished dresser */
#define HOME_ITEM_38  11837     /* teak dresser */
#define HOME_ITEM_58  11857     /* oak book */
#define HOME_ITEM_59  11858     /* pine book */
#define HOME_ITEM_60  11859     /* carved book */
#define HOME_ITEM_61  11860     /* polished book */
#define HOME_ITEM_62  11861     /* teak book */
#define HOME_ITEM_70  11869     /* royalbed */
#define HOME_ITEM_71  11870     /* cot */
#define HOME_ITEM_72  11871     /* featherbed */
#define HOME_ITEM_73  11872     /* canopybed */
#define HOME_ITEM_74  11873     /* postedbed */
#define HOME_ITEM_75  11874     /* twinbed */

CH_CMD ( do_home )
{

    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
    RESET_DATA *loc_reset;
    OBJ_DATA *furn;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MSL];

    loc = ch->in_room;
    loc_area = ch->in_room->area;
    buf[0] = '\0';

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    argument = one_argument ( argument, arg3 );

    if ( IS_NPC ( ch ) || ch == NULL )
        return;

    if ( is_reboot_countdown && reboot_countdown <= 3 )
    {
        send_to_char
            ( "Sorry, we are preparing for a reboot.\n\rPlease use the home command after the reboot.\n\r",
              ch );
        return;
    }

    if ( arg1[0] == '\0' || !str_cmp ( arg1, "list" ) )
    {
        send_to_char ( "What about a home?\n\r", ch );
        send_to_char ( "\n\rSyntax: Home (buy, sell, furnish, describe, access, kick)\n\r",
                       ch );
        send_to_char
            ( "\n\rExtended:\n\rHome (buy, sell, furnish, describe, access, kick)\n\r",
              ch );
        send_to_char
            ( "Buy      (purchase a home in the room you are standing in)\n\r",
              ch );
        send_to_char
            ( "Sell     (confirm - does not prompt for confirmation!)\n\r",
              ch );
        send_to_char ( "Furnish  (allows purchases of items)\n\r", ch );
        send_to_char
            ( "Describe (describe the room - uses the OLC editor)\n\r", ch );
	send_to_char ( "Access   (changes whether your home is private or public)\n\r", ch);
	send_to_char ( "Kick     (kicks a person from your house)\n\r", ch);
        return;
    }
    if ( !is_name ( ch->in_room->area->file_name, "phome.are" ) )
    {
        send_to_char ( "You aren't in SilverXire residential district.\n\r",
                       ch );
        return;
    }
    if ( !IS_SET ( loc->room_flags, ROOM_HOME ) )
    {
        send_to_char ( "You cannot buy a home in this room.\n\r", ch );
        return;
    }

    /* Find out what the argument is, if any */
    if ( !str_cmp ( arg1, "buy" ) )
        home_buy ( ch );
    else if ( !str_cmp ( arg1, "sell" ) )
        home_sell ( ch, arg2 );
    else if ( !str_cmp ( arg1, "describe" ) )
        home_describe ( ch );
    else if ( !str_cmp ( arg1, "kick" ) )
    {
        if ( !is_room_owner ( ch, loc ) )
        {
            send_to_char ( "But you do not own this room!\n\r", ch );
            return;
        }
	
    send_to_char ( "This command is not finished yet.\n\r", ch );
    }
    else if ( !str_cmp ( arg1, "access" ) )
    {
        if ( !is_room_owner ( ch, loc ) )
        {
            send_to_char ( "But you do not own this room!\n\r", ch );
            return;
        }

    TOGGLE_BIT ( loc->room_flags, ROOM_PRIVATE );

	if ( IS_SET ( loc->room_flags, ROOM_PRIVATE ) )
	{
	   send_to_char ( "Your home is now private.\n\r", ch );
	   return;
	}

	if ( !IS_SET ( loc->room_flags, ROOM_PRIVATE ) )
	{
	   send_to_char ( "Your home is now public.\n\r", ch );
	   return;
	}
}
	   
    else if ( !str_cmp ( arg1, "furnish" ) )
        /* Home furnish was left in here because I didn't feel like
         * redoing all the arguments - Dalsor
         */
    {
        if ( !is_room_owner ( ch, loc ) )
        {
            send_to_char ( "But you do not own this room!\n\r", ch );
            return;
        }
        if ( arg2[0] == '\0' )
        {
            send_to_char
                ( "This command allows you to furnish your home.\n\r", ch );
            send_to_char
                ( "You must be carrying platinum to purchase furnishings,\n\r",
                  ch );
            send_to_char
                ( "and be standing in your home. You cannot have more\n\r",
                  ch );
            send_to_char ( "than five items in your home.\n\r", ch );
            send_to_char ( "\n\rSyntax: Home (furnish) (item name)\n\r", ch );
            send_to_char ( "   Window      5000 platinum\n\r", ch );
            send_to_char
                ( "   Aquarium    7500 platinum (fish, turtle, snake, spider)\n\r",
                  ch );
            send_to_char
                ( "   Chair       5000 platinum (sturdy, highback, carved, hard)\n\r",
                  ch );
            send_to_char
                ( "   Desk        7500 platinum (oak, pine, carved, polished, teak)\n\r",
                  ch );
            send_to_char
                ( "   Sofa        7500 platinum (plush, padded, comfortable, fluffed)\n\r",
                  ch );
            send_to_char
                ( "   Table       7500 platinum (oak, pine, carved, polished, teak)\n\r",
                  ch );
            send_to_char ( "   Recliner    7500 platinum\n\r", ch );
            send_to_char ( "   Lamp        2000 platinum\n\r", ch );
            send_to_char ( "   Mirror      2500 platinum\n\r", ch );
            send_to_char ( "   Lantern     1500 platinum\n\r", ch );
            send_to_char ( "   Torch       1000 platinum\n\r", ch );
            send_to_char
                ( "   Dresser     5000 platinum (oak, pine, carved, polished, teak)\n\r",
                  ch );
            send_to_char
                ( "   Bookcase    7500 platinum (oak, pine, carved, polished, teak)\n\r",
                  ch );
            send_to_char ( "   Cot         1000 platinum\n\r", ch );
            send_to_char ( "   Featherbed  2500 platinum\n\r", ch );
            send_to_char ( "   Twinbed     5000 platinum\n\r", ch );
            send_to_char ( "   Postedbed   7500 platinum\n\r", ch );
            send_to_char ( "   Canopybed   8500 platinum\n\r", ch );
            send_to_char ( "   Royalbed    9900 platinum\n\r", ch );
            return;
        }
        else if ( !str_cmp ( arg2, "window" ) )
        {
            send_to_char ( "Windows aren't available yet.\n\r", ch );
            send_to_char
                ( "Windows will allow you to look into or out of a home.\n\r",
                  ch );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "aquarium" ) )
        {
            if ( ch->platinum < 7500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                SET_BIT ( loc_area->area_flags, AREA_CHANGED );
                return;
            }
            if ( arg3[0] != '\0' )
            {
                if ( !str_cmp ( arg3, "fish" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_1 ), 0 );
                }
                else if ( !str_cmp ( arg3, "turtle" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_2 ), 0 );
                }
                else if ( !str_cmp ( arg3, "snake" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_3 ), 0 );
                }
                else if ( !str_cmp ( arg3, "spider" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_4 ), 0 );
                }
                else
                {
                    send_to_char ( "Invalid aquarium type.\n\r", ch );
                    return;
                }
            }
            else
            {
                send_to_char ( "Invalid aquarium type.\n\r", ch );
                return;
            }
            ch->platinum -= 7500;
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "chair" ) )
        {
            if ( ch->platinum < 5000 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            if ( arg3[0] != '\0' )
            {
                if ( !str_cmp ( arg3, "sturdy" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_8 ), 0 );
                }
                else if ( !str_cmp ( arg3, "highback" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_6 ), 0 );
                }
                else if ( !str_cmp ( arg3, "carved" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_5 ), 0 );
                }
                else if ( !str_cmp ( arg3, "hard" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_7 ), 0 );
                }
                else
                {
                    send_to_char ( "Invalid chair type.\n\r", ch );
                    return;
                }
            }
            else
            {
                send_to_char ( "Invalid chair type.\n\r", ch );
                return;
            }
            ch->platinum -= 5000;
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "desk" ) )
        {
            if ( ch->platinum < 7500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            if ( arg3[0] != '\0' )
            {
                if ( !str_cmp ( arg3, "oak" ) )
                {
                    furn = create_object ( get_obj_index ( HOME_ITEM_9 ), 0 );
                }
                else if ( !str_cmp ( arg3, "pine" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_10 ), 0 );
                }
                else if ( !str_cmp ( arg3, "carved" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_11 ), 0 );
                }
                else if ( !str_cmp ( arg3, "polished" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_12 ), 0 );
                }
                else if ( !str_cmp ( arg3, "teak" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_13 ), 0 );
                }
                else
                {
                    send_to_char ( "Invalid desk type.\n\r", ch );
                    return;
                }
            }
            else
            {
                send_to_char ( "Invalid desk type.\n\r", ch );
                return;
            }
            ch->platinum -= 7500;
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "sofa" ) )
        {
            if ( ch->platinum < 7500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            if ( arg3[0] != '\0' )
            {
                if ( !str_cmp ( arg3, "plush" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_15 ), 0 );
                }
                else if ( !str_cmp ( arg3, "padded" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_16 ), 0 );
                }
                else if ( !str_cmp ( arg3, "comfortable" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_17 ), 0 );
                }
                else if ( !str_cmp ( arg3, "fluffed" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_18 ), 0 );
                }
                else
                {
                    send_to_char ( "Invalid sofa type.\n\r", ch );
                    return;
                }
            }
            else
            {
                send_to_char ( "Invalid sofa type.\n\r", ch );
                return;
            }
            ch->platinum -= 7500;
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "table" ) )
        {
            if ( ch->platinum < 7500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            if ( arg3[0] != '\0' )
            {
                if ( !str_cmp ( arg3, "oak" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_25 ), 0 );
                }
                else if ( !str_cmp ( arg3, "pine" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_26 ), 0 );
                }
                else if ( !str_cmp ( arg3, "carved" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_27 ), 0 );
                }
                else if ( !str_cmp ( arg3, "polished" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_28 ), 0 );
                }
                else if ( !str_cmp ( arg3, "teak" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_29 ), 0 );
                }
                else
                {
                    send_to_char ( "Invalid table type.\n\r", ch );
                    return;
                }
            }
            else
            {
                send_to_char ( "Invalid table type.\n\r", ch );
                return;
            }
            ch->platinum -= 7500;
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "recliner" ) )
        {
            if ( ch->platinum < 7500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            ch->platinum -= 7500;
            furn = create_object ( get_obj_index ( HOME_ITEM_30 ), 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "lamp" ) )
        {
            if ( ch->platinum < 2000 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            send_to_char
                ( "Your lamp will provide light for your home for a time.\n\r",
                  ch );
            ch->platinum -= 2000;
            furn = create_object ( get_obj_index ( HOME_ITEM_31 ), 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "mirror" ) )
        {
            send_to_char ( "Mirrors aren't available yet.\n\r", ch );
            send_to_char
                ( "Mirrors will allow you to see your reflection... Pretty useless, eh?\n\r",
                  ch );
            return;
        }
        else if ( !str_cmp ( arg2, "lantern" ) )
        {
            if ( ch->platinum < 1500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            send_to_char
                ( "Your lantern will provide light for your home for a time.\n\r",
                  ch );
            ch->platinum -= 1500;
            furn = create_object ( get_obj_index ( HOME_ITEM_32 ), 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "torch" ) )
        {
            if ( ch->platinum < 1000 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            send_to_char
                ( "Your torch will provide light for your home for a time.\n\r",
                  ch );
            ch->platinum -= 1000;
            furn = create_object ( get_obj_index ( HOME_ITEM_33 ), 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "dresser" ) )
        {
            if ( ch->platinum < 5000 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            if ( arg3[0] != '\0' )
            {
                if ( !str_cmp ( arg3, "oak" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_34 ), 0 );
                }
                else if ( !str_cmp ( arg3, "pine" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_35 ), 0 );
                }
                else if ( !str_cmp ( arg3, "carved" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_36 ), 0 );
                }
                else if ( !str_cmp ( arg3, "polished" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_37 ), 0 );
                }
                else if ( !str_cmp ( arg3, "teak" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_38 ), 0 );
                }
                else
                {
                    send_to_char ( "Invalid dresser type.\n\r", ch );
                    return;
                }
            }
            else
            {
                send_to_char ( "Invalid dresser type.\n\r", ch );
                return;
            }
            ch->platinum -= 5000;
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }

        else if ( !str_cmp ( arg2, "bookcase" ) )
        {
            if ( ch->platinum < 7500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            if ( arg3[0] != '\0' )
            {
                if ( !str_cmp ( arg3, "oak" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_58 ), 0 );
                }
                if ( !str_cmp ( arg3, "pine" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_59 ), 0 );
                }
                if ( !str_cmp ( arg3, "carved" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_60 ), 0 );
                }
                if ( !str_cmp ( arg3, "polished" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_61 ), 0 );
                }
                if ( !str_cmp ( arg3, "teak" ) )
                {
                    furn =
                        create_object ( get_obj_index ( HOME_ITEM_62 ), 0 );
                }
                else
                {
                    send_to_char ( "Invalid bookcase type.\n\r", ch );
                    return;
                }
            }
            else
            {
                send_to_char ( "Invalid bookcase type.\n\r", ch );
                return;
            }
            ch->platinum -= 7500;
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "cot" ) )
        {
            if ( ch->platinum < 1000 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            ch->platinum -= 1000;
            furn = create_object ( get_obj_index ( HOME_ITEM_71 ), 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "featherbed" ) )
        {
            if ( ch->platinum < 2500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            ch->platinum -= 2500;
            furn = create_object ( get_obj_index ( HOME_ITEM_72 ), 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "twinbed" ) )
        {
            if ( ch->platinum < 5000 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            ch->platinum -= 5000;
            furn = create_object ( get_obj_index ( HOME_ITEM_75 ), 0 );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "postedbed" ) )
        {
            if ( ch->platinum < 7500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            ch->platinum -= 7500;
            furn = create_object ( get_obj_index ( HOME_ITEM_74 ), 0 );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "canopybed" ) )
        {
            if ( ch->platinum < 8500 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            ch->platinum -= 8500;
            furn = create_object ( get_obj_index ( HOME_ITEM_73 ), 0 );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else if ( !str_cmp ( arg2, "royalbed" ) )
        {
            if ( ch->platinum < 9900 )
            {
                send_to_char
                    ( "You do not have enough platinum for this purchase.\n\r",
                      ch );
                return;
            }
            ch->platinum -= 9900;
            furn = create_object ( get_obj_index ( HOME_ITEM_70 ), 0 );
            loc_reset = new_reset_data (  );
            loc_reset->command = 'O';
            loc_reset->arg1 = furn->pIndexData->vnum;
            loc_reset->arg2 = 0;
            loc_reset->arg3 = loc->vnum;
            loc_reset->arg4 = 0;
            add_reset ( loc, loc_reset, 0 );
            printf_to_char ( ch,
                             "You have been deducted %d for your purchase.",
                             furn->cost );
            obj_to_room ( furn, ch->in_room );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
        else
        {
            send_to_char
                ( "You have not provided the name of the item to purchase.\n\r",
                  ch );
            return;
        }
    }
    else
    {
        send_to_char
            ( "No such Home Command. See HELP HOME for more informatio.\n\r",
              ch );
        return;
    }
    return;
}

void home_buy ( CHAR_DATA * ch )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
    char buf[MSL];
    loc = ch->in_room;
    loc_area = ch->in_room->area;
    if ( loc->owner[0] == '\0' )
    {
        if ( ch->platinum < 25000 )
        {
            send_to_char ( "This command allows you to buy a home.\n\r", ch );
            send_to_char ( "You must be standing in the room to buy.\n\r",
                           ch );
            send_to_char
                ( "You start with a blank, untitled room that is void of\n\r",
                  ch );
            send_to_char
                ( "furnishings and light. A single door allows entrance.\n\r",
                  ch );
            send_to_char
                ( "A home costs 25,000 platinum. You must be carrying the coins.\n\r",
                  ch );
            send_to_char ( "\n\rSyntax: Home buy\n\r", ch );
            return;
        }
        else
        {
            free_string ( loc->owner );
            loc->owner = str_dup ( ch->name );
            ch->platinum -= 25000;
            free_string ( loc->name );
            sprintf ( buf, "%s's Home", ch->name );
            loc->name = str_dup ( buf );
            send_to_char ( "Congratulations on purchasing your new home!\n\r",
                           ch );
            send_to_char
                ( "You are now 25,000 platinum coins lighter and the owner\n\r",
                  ch );
            send_to_char
                ( "of the room in which you now stand! Be sure to describe\n\r",
                  ch );
            send_to_char ( "your home and purchase furnishings for it.\n\r",
                           ch );
            SET_BIT ( loc_area->area_flags, AREA_CHANGED );
            return;
        }
    }
    else
    {
        send_to_char ( "This home is already owned.\n\r", ch );
        return;
    }
    return;
}

void home_sell ( CHAR_DATA * ch, char *argument )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
    RESET_DATA *current_reset;
    RESET_DATA *next_reset;
    OBJ_DATA *furn;
    OBJ_DATA *furn_next;
    loc = ch->in_room;
    loc_area = ch->in_room->area;
    if ( !is_room_owner ( ch, loc ) )
    {
        send_to_char ( "But you do not own this room!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char ( "This command allows you to sell your home.\n\r", ch );
        send_to_char
            ( "You will no longer own your home once it is sold,\n\r", ch );
        send_to_char
            ( "and you will be compensated half the cost of the home,\n\r",
              ch );
        send_to_char ( "not including items you have purchased.\n\r", ch );
        send_to_char ( "You must be standing in the room which you own.\n\r",
                       ch );
        send_to_char ( "\n\rSyntax: Home (sell) (confirm)\n\r", ch );
        return;
    }
    else if ( !str_cmp ( argument, "confirm" ) )
    {
        free_string ( loc->owner );
        loc->owner = str_dup ( "" );
        ch->platinum += 12500;
        free_string ( loc->name );
        loc->name = str_dup ( "An Abandoned Home" );
        if ( !ch->in_room->reset_first )
        {
            return;
        }
        /* Thanks to Edwin and Kender for the help with killing
         * resets. Pointed out some real problems with my handling
         * of it. Thanks a bunch guys.
         * This is the exact way Kender put it, and it works great!
         */
        for ( current_reset = ch->in_room->reset_first; current_reset;
              current_reset = next_reset )
        {
            next_reset = current_reset->next;
            free_reset_data ( current_reset );
        }

        ch->in_room->reset_first = NULL;
        for ( furn = ch->in_room->contents; furn != NULL; furn = furn_next )
        {
            furn_next = furn->next_content;
            extract_obj ( furn );
        }
	if IS_SET ( ch->in_room->room_flags, ROOM_PRIVATE )
	    TOGGLE_BIT ( ch->in_room->room_flags, ROOM_PRIVATE );
        send_to_char
            ( "Your home has been sold and you are now 12,500 platinum coins richer!\n\r",
              ch );
        SET_BIT ( loc_area->area_flags, AREA_CHANGED );
        return;
    }
    else
    {
        send_to_char ( "This command allows you to sell your home.\n\r", ch );
        send_to_char
            ( "You will no longer own your home once it is sold,\n\r", ch );
        send_to_char
            ( "and you will be compensated half the cost of the home,\n\r",
              ch );
        send_to_char ( "not including items you have purchased.\n\r", ch );
        send_to_char ( "You must be standing in the room which you own.\n\r",
                       ch );
        send_to_char ( "\n\rSyntax: Home (sell) (confirm)\n\r", ch );
        return;
    }
}

void home_describe ( CHAR_DATA * ch )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
    loc = ch->in_room;
    loc_area = ch->in_room->area;
    if ( !is_room_owner ( ch, loc ) )
    {
        send_to_char ( "But you do not own this room!\n\r", ch );
        return;
    }
    else
    {
        send_to_char ( "This command allows you to describe your home.\n\r",
                       ch );
        send_to_char
            ( "You should not describe items that are in the room,\n\r", ch );
        send_to_char
            ( "rather allowing the furnishing of the home to do that.\n\r",
              ch );
        send_to_char
            ( "If you currently own this room, you will be placed into.\n\r",
              ch );
        send_to_char
            ( "the room editor. Be warned that while in the room editor,\n\r",
              ch );
        send_to_char
            ( "you are only allowed to type the description. If you are\n\r",
              ch );
        send_to_char
            ( "unsure or hesitant about this, please note the Immortals,\n\r",
              ch );
        send_to_char ( "or better, discuss the how-to's with a Builder.\n\r",
                       ch );
        send_to_char ( "Syntax: Home (describe)\n\r", ch );
        string_append ( ch, &loc->description );
        SET_BIT ( loc_area->area_flags, AREA_CHANGED );
        return;
    }

}
