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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sd.h"
#include "recycle.h"
#include "tables.h"
#include "magic.h"

/* RT part of the corpse looting code */

bool can_loot ( CHAR_DATA * ch, OBJ_DATA * obj )
{
    CHAR_DATA *owner, *wch, *killer;

    if ( IS_IMMORTAL ( ch ) )
        return TRUE;

    if ( !obj->owner || obj->owner == NULL )
        return TRUE;

    owner = NULL;
    for ( wch = char_list; wch != NULL; wch = wch->next )
        if ( !str_cmp ( wch->name, obj->owner ) )
            owner = wch;

    killer = NULL;
    if ( obj->killer || obj->killer != NULL )
        for ( wch = char_list; wch != NULL; wch = wch->next )
            if ( !str_cmp ( wch->name, obj->killer ) )
                killer = wch;

    if ( owner == NULL )
        return TRUE;

    if ( !str_cmp ( ch->name, owner->name ) )
        return TRUE;

    if ( killer != NULL )
        if ( !str_cmp ( ch->name, killer->name ) )
            return TRUE;

    if ( !IS_NPC ( owner ) && IS_SET ( owner->act, PLR_CANLOOT ) )
        return TRUE;

    if ( is_same_group ( ch, owner ) )
        return TRUE;

    return FALSE;
}

BUFFER2 *get_obj ( CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container )
{
    /* variables for AUTOSPLIT */
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    //BUFFER *output;
    BUFFER2 *output;
    CHAR_DATA *gch;
    int members;
    char buffer[100];

    //output = new_buf (  );
    output = buffer_new ( 1337 );
    if ( !CAN_WEAR ( obj, ITEM_TAKE ) && ( ch->level < HEADIMP ) )
    {
        sprintf ( buf, "You can't take that.\n\r" );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        return output;
    }

    if ( ch->carry_number + get_obj_number ( obj ) > can_carry_n ( ch ) )
    {
        sprintf ( buf, "%s: you can't carry that many items.\n\r",
                  obj->short_descr );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        return output;
    }

    if ( get_carry_weight ( ch ) + get_obj_weight ( obj ) >
         can_carry_w ( ch ) )
    {
        sprintf ( buf, "%s: you can't carry that much weight.\n\r",
                  obj->short_descr );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        return output;
    }

    if ( !can_loot ( ch, obj ) )
    {
        sprintf ( buf, "Corpse looting is not permitted.\n\r" );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        return output;
    }

    if ( obj->in_room != NULL )
    {
        for ( gch = obj->in_room->people; gch != NULL;
              gch = gch->next_in_room )
            if ( gch->on == obj )
            {
                sprintf ( buf, "%s appears to be using %s.\n\r", gch->name,
                          obj->short_descr );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
                return output;
            }
    }
/*
    if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) && ch->level <= HERO )
    {
        if ( !can_quest ( ch ) )
        {
            sprintf ( buf, "%s: You already have a quest item.\n\r",
                      obj->short_descr );
            add_buf ( output, buf );
            return output;
        }
    }
*/
    if ( IS_QUESTOR ( ch ) && ch->pcdata->questobj > 0 )
    {
        if ( ch->pcdata->questobj == obj->pIndexData->vnum )
        {

            send_to_char ( "{RYou have almost completed your QUEST!{x\n\r",
                           ch );
            send_to_char
                ( "{RReturn to the questmaster before your time runs out!{x\n\r",
                  ch );
            /*      sprintf(buf, "$N has found %s, their questobject.", obj->short_descr);
               wiznet(buf, ch, NULL, WIZ_QUESTS, 0, 0); */
        }
    }

    if ( container != NULL )
    {
        if ( container->pIndexData->item_type == ITEM_PIT &&
             ( ( ( get_trust ( ch ) < obj->level ) &&
                 ( ch->class < MAX_CLASS / 2 ) && ( obj->level > 19 ) ) ||
               ( ( get_trust ( ch ) < obj->level ) &&
                 ( ch->class >= MAX_CLASS / 2 ) && ( obj->level > 27 ) ) ) )
        {
            sprintf ( buf, "%s: You are not powerful enough to use it.\n\r",
                      obj->short_descr );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
            return output;
        }

        if ( container->pIndexData->item_type == ITEM_PIT &&
             !CAN_WEAR ( container, ITEM_TAKE ) &&
             !IS_OBJ_STAT ( obj, ITEM_HAD_TIMER ) )
            obj->timer = 0;
        sprintf ( buf, "You get %s from %s.\n\r", obj->short_descr,
                  container->short_descr );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        act ( "$n gets $p from $P.", ch, obj, container, TO_ROOM );
        REMOVE_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
        obj_from_obj ( obj );
    }
    else
    {
        sprintf ( buf, "You get %s.\n\r", obj->short_descr );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        act ( "$n gets $p.", ch, obj, container, TO_ROOM );
        obj_from_room ( obj );
    }

    if ( obj->item_type == ITEM_MONEY )
    {
        if ( obj->value[0] > 0 )
            add_cost ( ch, obj->value[0], VALUE_SILVER );
        if ( obj->value[1] > 0 )
            add_cost ( ch, obj->value[1], VALUE_GOLD );
        if ( obj->value[2] > 0 )
            add_cost ( ch, obj->value[2], VALUE_PLATINUM );
        if ( IS_SET ( ch->act, PLR_AUTOSPLIT ) )
        {                       /* AUTOSPLIT code */
            members = 0;
            for ( gch = ch->in_room->people; gch != NULL;
                  gch = gch->next_in_room )
            {
                if ( !IS_AFFECTED ( gch, AFF_CHARM ) &&
                     is_same_group ( gch, ch ) )
                    members++;
            }

            if ( members > 1 &&
                 ( obj->value[0] > 1 || obj->value[1] || obj->value[2] ) )
            {
                sprintf ( buffer, "%ld %ld %ld", obj->value[0], obj->value[1],
                          obj->value[2] );
                do_split ( ch, buffer );
            }
        }

        extract_obj ( obj );
    }
    else
    {
        obj_to_char ( obj, ch );
    }
    if ( IS_OBJ_STAT ( obj, ITEM_FORCED ) ) 
/* && ( ch->level <= HERO ) ) */
    {
	  do_wear ( ch, obj->name );
    }
    if ( ( obj->pIndexData->vnum == OBJ_VNUM_VOODOO ) && !IS_NPC ( ch ) )
    {
        one_argument ( obj->name, arg );
        if ( !str_cmp ( arg, ch->name ) || ch->level < 20 )
        {
            obj->timer = 1;
        }
    }
    return output;
}

CH_CMD ( do_get )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
//    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
//    BUFFER *output;
    BUFFER2 *output;
    BUFFER2 *outlist;
    bool found;

    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( !str_cmp ( arg2, "from" ) )
        argument = one_argument ( argument, arg2 );

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Get what?\n\r", ch );
        return;
    }

//    output = new_buf (  );
    output = buffer_new ( 1337 );
    if ( arg2[0] == '\0' )
    {
        if ( str_cmp ( arg1, "all" ) && str_prefix ( "all.", arg1 ) )
        {
            /* 'get obj' */
            obj = get_obj_list ( ch, arg1, ch->in_room->contents );
            if ( obj == NULL )
            {
                act ( "I see no $T here.", ch, NULL, arg1, TO_CHAR );
            }
            else
            {
                outlist = get_obj ( ch, obj, NULL );
                //send_to_char ( buf_string ( outlist ), ch );
		page_to_char ( buffer_string ( outlist ), ch );
                //free_buf ( outlist );
		buffer_free ( outlist );
            }
        }
        else
        {
            /* 'get all' or 'get all.obj' */
	    int number, max_found;
	    number = 0;
	    max_found = 200;
            found = FALSE;
            for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_content;
                if ( ( arg1[3] == '\0' || is_name ( &arg1[4], obj->name ) ) &&
                     can_see_obj ( ch, obj ) )
                {
                    found = TRUE;
		    number++;
                    outlist = get_obj ( ch, obj, NULL );
//                    add_buf ( output, buf_string ( outlist ) );
		    buffer_strcat ( output, buffer_string ( outlist ) );
                    //free_buf ( outlist );
		    buffer_free ( outlist );
                }
                if ( number >= max_found )
                {
		   buffer_strcat ( output, "\n\rThat's all you can get at once.\n\r" );
                   break;
                }
            }

            if ( !found )
            {
                if ( arg1[3] == '\0' )
                    send_to_char ( "I see nothing here.\n\r", ch );
                else
                    act ( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR );
            }
            else
            {
                //page_to_char ( buf_string ( output ), ch );
		page_to_char ( buffer_string ( output ), ch );
            }
        }
    }
    else
    {
        /* 'get ... container' */
        if ( !str_cmp ( arg2, "all" ) || !str_prefix ( "all.", arg2 ) )
        {
            send_to_char ( "You can't do that.\n\r", ch );
//            free_buf ( output );
	    buffer_free ( output );
            return;
        }

        if ( ( container = get_obj_here ( ch, arg2 ) ) == NULL )
        {
            act ( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
//            free_buf ( output );
	    buffer_free ( output );
            return;
        }

        switch ( container->item_type )
        {
            default:
                send_to_char ( "That's not a container.\n\r", ch );
//              free_buf ( output );
	        buffer_free ( output );
                return;

            case ITEM_CONTAINER:
            case ITEM_PIT:
            case ITEM_CORPSE_NPC:
                break;

            case ITEM_CORPSE_PC:
            {
                if ( !can_loot ( ch, container ) )
                {
                    send_to_char ( "You can't do that.\n\r", ch );
/*                    free_buf ( output ); */
                    buffer_free ( output );
                    return;
                }
            }
        }

        if ( IS_SET ( container->value[1], CONT_CLOSED ) )
        {
            act ( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
//            free_buf ( output );
	    buffer_free ( output );
            return;
        }

        if ( str_cmp ( arg1, "all" ) && str_prefix ( "all.", arg1 ) )
        {
            /* 'get obj container' */
            obj = get_obj_list ( ch, arg1, container->contains );
            if ( obj == NULL )
            {
                act ( "I see nothing like that in the $T.", ch, NULL, arg2,
                      TO_CHAR );
                //free_buf ( output );
		buffer_free ( output );
                return;
            }
            outlist = get_obj ( ch, obj, container );
            //send_to_char ( buf_string ( outlist ), ch );
	    page_to_char ( buffer_string ( outlist ), ch );
            //free_buf ( outlist );
	    buffer_free ( outlist );
        }
        else
        {
            /* 'get all container' or 'get all.obj container' */
            int number, max_found;
            number = 0;
            max_found = 200;
            found = FALSE;
            for ( obj = container->contains; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_content;
                if ( ( arg1[3] == '\0' || is_name ( &arg1[4], obj->name ) ) &&
                     can_see_obj ( ch, obj ) )
                {
                    found = TRUE;
		    number++;
                    if ( container->pIndexData->item_type == ITEM_PIT && !IS_IMMORTAL ( ch ) )
                    {
                        send_to_char ( "Don't be so greedy!\n\r", ch );
		        buffer_free ( output );
                        return;
                    }
/*                    if ( container->pIndexData->item_type == ITEM_CORPSE_PC && !IS_IMMORTAL ( ch ) )
            	    {
                       send_to_char ( "Stop being so greedy!\n\r", ch );
                       buffer_free ( output );
                       return;
                    } */
                                  
		    outlist = get_obj ( ch, obj, container );
//                    add_buf ( output, buf_string ( outlist ) );
		    buffer_strcat ( output, buffer_string ( outlist ) );		    
                    //free_buf ( outlist );
		    buffer_free ( outlist );
                }
                if ( number >= max_found )
                {
		   buffer_strcat ( output, "\n\rThat's all you can get at once.\n\r" );
                   break;
                }
            }

            if ( !found )
            {
                if ( arg1[3] == '\0' )
                    act ( "I see nothing in the $T.", ch, NULL, arg2,
                          TO_CHAR );
                else
                    act ( "I see nothing like that in the $T.", ch, NULL,
                          arg2, TO_CHAR );
            }
            else
            {
//                page_to_char ( buf_string ( output ), ch );
		page_to_char ( buffer_string ( output ), ch );
            }
        }
    }
//    free_buf ( output );
    buffer_free ( output );
    return;
}

CH_CMD ( do_donate )
{
    char arg1[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    bool found = FALSE;

    argument = one_argument ( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Donate what?\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg1, "all" ) || !str_prefix ( "all.", arg1 ) )
    {
        send_to_char ( "One item at a time please.\n\r", ch );
        return;
    }

    for ( container = object_list; container != NULL;
          container = container->next )
    {
        if ( container->pIndexData->item_type != ITEM_PIT ||
             container->pIndexData->vnum != 3010 )
            continue;

        found = TRUE;
        break;
    }

    if ( !found )
    {
        send_to_char ( "The donation pit is missing. Please tell an immortal to load a new pit in the donation room asap.\n\r",ch );
        return;
    }

    if ( ( obj = get_obj_carry ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    if ( !can_drop_obj ( ch, obj ) )
    {
        send_to_char ( "You can't let go of it.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) )
    {
        send_to_char ( "You can't donate a quest item.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_MELT_DROP ) )
    {
        send_to_char ( "You have a feeling noone's going to want that.\n\r",
                       ch );
        return;
    }

    if ( obj->item_type == ITEM_TRASH )
    {
        send_to_char ( "The donation pit is not a trash can.\n\r", ch );
        return;
    }

    if ( WEIGHT_MULT ( obj ) != 100 )
    {
        send_to_char ( "You have a feeling that would be a bad idea.\n\r",
                       ch );
        return;
    }

    if (get_obj_weight( obj ) + get_true_weight( container )
	> (container->value[0] * 10) 
    ||  get_obj_weight(obj) > (container->value[3] * 10))
    {
	send_to_char( "The donation pit can't hold that.\n\r", ch );
	return;
    }

    if ( !CAN_WEAR ( container, ITEM_TAKE ) )
    {
        if ( obj->timer )
            SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
        else
            obj->timer = number_range ( 100, 200 );
    }

    obj_from_char ( obj );
    obj_to_obj ( obj, container );
    act ( "$p glows {Mpurple{x, then disappears from $n's inventory.", ch,
          obj, container, TO_ROOM );
    act ( "$p glows {Mpurple{x, then disappears..", ch, obj, container,
          TO_CHAR );

    return;
}

/*
CH_CMD ( do_clan_donate )
{
    char arg1[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    bool found = FALSE;

    argument = one_argument ( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Clan Donate what?\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg1, "all" ) || !str_prefix ( "all.", arg1 ) )
    {
        send_to_char ( "One item at a time please.\n\r", ch );
        return;
    }

    for ( container = object_list; container != NULL;
          container = container->next )
    {
        if ( container->pIndexData->item_type != ITEM_PIT ||
             container->pIndexData->vnum != clan_table[ch->clan].pit )
            continue;

        found = TRUE;
        break;
    }
    if ( !found )
    {
        for ( container = object_list; container != NULL;
              container = container->next )
        {
            if ( container->pIndexData->item_type != ITEM_PIT ||
                 container->pIndexData->vnum != OBJ_VNUM_PIT )
                continue;

            found = TRUE;
            break;
        }
    }
    if ( !found )
    {
        send_to_char ( "I can't seem to find the donation room!\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    if ( !can_drop_obj ( ch, obj ) )
    {
        send_to_char ( "You can't let go of it.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) )
    {
        send_to_char ( "You can't donate a quest item.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_MELT_DROP ) )
    {
        send_to_char ( "You have a feeling noone's going to want that.\n\r",
                       ch );
        return;
    }

    if ( obj->item_type == ITEM_TRASH )
    {
        send_to_char ( "The donation room is not a trash can.\n\r", ch );
        return;
    }

    if ( WEIGHT_MULT ( obj ) != 100 )
    {
        send_to_char ( "You have a feeling that would be a bad idea.\n\r",
                       ch );
        return;
    }

    if ( get_obj_weight ( obj ) + get_true_weight ( container ) >
         ( container->value[0] * 10 ) ||
         get_obj_weight ( obj ) > ( container->value[3] * 10 ) )
    {
        send_to_char ( "The donation pit can't hold that.\n\r", ch );
        return;
    }

    if ( !CAN_WEAR ( container, ITEM_TAKE ) )
    {
        if ( obj->timer )
            SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
        else
            obj->timer = number_range ( 100, 200 );
    }

    obj_from_char ( obj );
    obj_to_obj ( obj, obj );
    act ( "$p glows {Mpurple{x, then disappears from $n's inventory.", ch,
          obj, container, TO_ROOM );
    act ( "$p glows {Mpurple{x, then disappears..", ch, obj, container,
          TO_CHAR );

    return;
}
*/
CH_CMD ( do_put )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
//    BUFFER *output;
    BUFFER2 *output;
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int count = 0;
    int max_found = 0;
    
    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( !str_cmp ( arg2, "in" ) || !str_cmp ( arg2, "on" ) )
        argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Put what in what?\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg2, "all" ) || !str_prefix ( "all.", arg2 ) )
    {
        send_to_char ( "You can't do that.\n\r", ch );
        return;
    }

    if ( ( container = get_obj_here ( ch, arg2 ) ) == NULL )
    {
        act ( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
        return;
    }

    if ( ( container->item_type != ITEM_CONTAINER ) &&
         ( container->item_type != ITEM_PIT ) )
    {
        send_to_char ( "That's not a container.\n\r", ch );
        return;
    }

    if ( IS_SET ( container->value[1], CONT_CLOSED ) )
    {
        act ( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
        return;
    }

    if ( str_cmp ( arg1, "all" ) && str_prefix ( "all.", arg1 ) )
    {
   
        if ( ( obj = get_obj_carry ( ch, arg1 ) ) == NULL )
        {
            send_to_char ( "You do not have that item.\n\r", ch );
            return;
        }

        if ( obj == container )
        {
            send_to_char ( "You can't fold it into itself.\n\r", ch );
            return;
        }

        if ( !can_drop_obj ( ch, obj ) )
        {
            send_to_char ( "You can't let go of it.\n\r", ch );
            return;
        }

        if ( ( obj->pIndexData->vnum == OBJ_VNUM_CUBIC ) &&
             ( container->pIndexData->vnum != OBJ_VNUM_CPOUCH ) )
        {
            send_to_char
                ( "Cubic Zirconium may only be placed in silk gem pouches.\n\r",
                  ch );
            return;
        }

        if ( ( obj->item_type == ITEM_GEM ) &&
             ( obj->pIndexData->vnum != OBJ_VNUM_CUBIC ) &&
             ( container->pIndexData->vnum != OBJ_VNUM_DPOUCH ) )
        {
            send_to_char
                ( "Gems may only be placed in leather gem pouches.\n\r", ch );
            return;
        }

        if ( ( container->pIndexData->vnum == OBJ_VNUM_DPOUCH ) &&
             ( obj->item_type != ITEM_GEM ) )
        {
            send_to_char
                ( "Only gems may be placed in leather gem pouches.\n\r", ch );
            return;
        }

        if ( ( container->pIndexData->vnum == OBJ_VNUM_CPOUCH ) &&
             ( obj->pIndexData->vnum != OBJ_VNUM_CUBIC ) )
        {
            send_to_char
                ( "Only cubic zirconium may be placed in silk gem pouches.\n\r",
                  ch );
            return;
        }
/*
        if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) && !IS_IMMORTAL ( ch ) )
        {
            send_to_char ( "You can't put a quest item in something.\n\r",
                           ch );
            return;
        }
*/
        if ( WEIGHT_MULT ( obj ) != 100 )
        {
            send_to_char ( "You have a feeling that would be a bad idea.\n\r",
                           ch );
            return;
        }

        if ( get_obj_weight ( obj ) + get_true_weight ( container ) >
             ( container->value[0] * 10 ) ||
             get_obj_weight ( obj ) > ( container->value[3] * 10 ) )
        {
            send_to_char ( "It won't fit.\n\r", ch );
            return;
        }

        if ( container->pIndexData->item_type == ITEM_PIT &&
             !CAN_WEAR ( container, ITEM_TAKE ) )
        {
            if ( obj->timer )
                SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
            else
                obj->timer = number_range ( 100, 200 );
        }
        obj_from_char ( obj );
        obj_to_obj ( obj, container );

        if ( IS_SET ( container->value[1], CONT_PUT_ON ) )
        {
            act ( "$n puts $p on $P.", ch, obj, container, TO_ROOM );
            act ( "You put $p on $P.", ch, obj, container, TO_CHAR );
        }
        else
        {
            act ( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
            act ( "You put $p in $P.", ch, obj, container, TO_CHAR );
        }
    }
    else
    {
        if ( container->pIndexData->vnum == OBJ_VNUM_DPOUCH )
        {
            count = 0;
	    max_found = 200;
            for ( obj = ch->carrying; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_content;

                if ( ( arg1[3] == '\0' || is_name ( &arg1[4], obj->name ) ) &&
                     obj->item_type == ITEM_GEM &&
                     obj->pIndexData->vnum != OBJ_VNUM_CUBIC &&
//                     !IS_OBJ_STAT ( obj, ITEM_QUEST ) &&
                     can_see_obj ( ch, obj ) && WEIGHT_MULT ( obj ) == 100 &&
                     obj->wear_loc == WEAR_NONE && obj != container &&
                     can_drop_obj ( ch, obj ) &&
                     get_obj_weight ( obj ) + get_true_weight ( container ) <=
                     ( container->value[0] * 10 ) &&
                     get_obj_weight ( obj ) <= ( container->value[3] * 10 ) )
                {
                    if ( container->pIndexData->item_type == ITEM_PIT &&
                         !CAN_WEAR ( obj, ITEM_TAKE ) )
                    {
                        if ( obj->timer )
                            SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
                        else
                            obj->timer = number_range ( 100, 200 );
                    }
                    count++;
                    obj_from_char ( obj );
                    obj_to_obj ( obj, container );
                }
                if ( count >= max_found )
                {
		   send_to_char ( "\n\rThat's all you can put at once.\n\r", ch );
                   break;
                }
            }
            if ( count != 0 )
            {
                sprintf ( buf, "You put %d gems in %s.\n\r", count,
                          container->short_descr );
                send_to_char ( buf, ch );
                sprintf ( buf, "$n puts %d gems in %s.\n\r", count,
                          container->short_descr );
                act ( buf, ch, NULL, NULL, TO_ROOM );
            }
            else
            {
                send_to_char ( "You are not carrying any gems.\n\r", ch );
            }
            return;
        }

        if ( container->pIndexData->vnum == OBJ_VNUM_CPOUCH )
        {
            count = 0;
	    max_found = 200;
            for ( obj = ch->carrying; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_content;

                if ( ( arg1[3] == '\0' || is_name ( &arg1[4], obj->name ) ) &&
                     obj->pIndexData->vnum == OBJ_VNUM_CUBIC &&
//                     !IS_OBJ_STAT ( obj, ITEM_QUEST ) &&
                     can_see_obj ( ch, obj ) && WEIGHT_MULT ( obj ) == 100 &&
                     obj->wear_loc == WEAR_NONE && obj != container &&
                     can_drop_obj ( ch, obj ) &&
                     get_obj_weight ( obj ) + get_true_weight ( container ) <=
                     ( container->value[0] * 10 ) &&
                     get_obj_weight ( obj ) < ( container->value[3] * 10 ) )
                {
                    if ( container->pIndexData->item_type == ITEM_PIT &&
                         !CAN_WEAR ( obj, ITEM_TAKE ) )
                    {
                        if ( obj->timer )
                            SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
                        else
                            obj->timer = number_range ( 100, 200 );
                    }
                    count++;
                    obj_from_char ( obj );
                    obj_to_obj ( obj, container );
                }
                if ( count >= max_found )
                {
                   send_to_char ( "\n\rThat's all you can put at once.\n\r", ch );
                   break;
                }
            }
            if ( count != 0 )
            {
                sprintf ( buf, "You put %d cubic zirconiums in %s.\n\r",
                          count, container->short_descr );
                send_to_char ( buf, ch );
                sprintf ( buf, "$n puts %d cubic zirconiums in %s.\n\r",
                          count, container->short_descr );
                act ( buf, ch, NULL, NULL, TO_ROOM );
            }
            else
            {
                send_to_char
                    ( "You are not carrying any cubic zirconiums.\n\r", ch );
            }
            return;
        }
//        output = new_buf (  );
        output = buffer_new ( 1337 );
        count = 0;
	max_found = 200;
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;

            if ( ( arg1[3] == '\0' || is_name ( &arg1[4], obj->name ) ) &&
//                 !IS_OBJ_STAT ( obj, ITEM_QUEST ) && 
		 can_see_obj ( ch, obj )
                 && WEIGHT_MULT ( obj ) == 100 && obj->wear_loc == WEAR_NONE
                 && obj != container && obj->item_type != ITEM_GEM &&
                 obj->pIndexData->vnum != OBJ_VNUM_CUBIC &&
                 can_drop_obj ( ch, obj ) &&
                 get_obj_weight ( obj ) + get_true_weight ( container ) <=
                 ( container->value[0] * 10 ) &&
                 get_obj_weight ( obj ) < ( container->value[3] * 10 ) )
            {
                if ( container->pIndexData->item_type == ITEM_PIT &&
                     !CAN_WEAR ( obj, ITEM_TAKE ) )
                {
                    if ( obj->timer )
                        SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
                    else
                        obj->timer = number_range ( 100, 200 );
                }
                obj_from_char ( obj );
                obj_to_obj ( obj, container );
//                count++;
                if ( count >= max_found )
                {
		   buffer_strcat ( output, "\n\rThat's all you can put at once.\n\r" );
                   break;
                }
                if ( IS_SET ( container->value[1], CONT_PUT_ON ) )
                {
                    sprintf ( buf, "You put %s on %s.\n\r", obj->short_descr,
                              container->short_descr );
                    //add_buf ( output, buf );
		    buffer_strcat ( output, buf );
                }
                else
                {
                    sprintf ( buf, "You put %s in %s.\n\r", obj->short_descr,
                              container->short_descr );
                    //add_buf ( output, buf );
		    buffer_strcat ( output, buf );
                }
            }
        }
        if ( count != 0 )
        {
            if ( IS_SET ( container->value[1], CONT_PUT_ON ) )
            {
                act ( "$n puts some things on $P.", ch, NULL, container,
                      TO_ROOM );
            }
            else
            {
                act ( "$n puts some things in $P.", ch, NULL, container,
                      TO_ROOM );
            }
//            page_to_char ( buf_string ( output ), ch );
	    page_to_char ( buffer_string ( output ), ch );
        }
//        free_buf ( output );
	 buffer_free ( output );
    }
    return;
}

CH_CMD ( do_drop )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
//    BUFFER *output;
    BUFFER2 *output;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    argument = one_argument ( argument, arg );

    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }

    if ( reboot_countdown > 0 && reboot_countdown < 3 )
    {
        send_to_char
            ( "Sorry, the mud is getting ready for a reboot.\n\rPlease try again later.\n\r",
              ch );
        return;
    }
        
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Drop what?\n\r", ch );
        return;
    }

    if ( is_number ( arg ) )
    {
        int amount, platinum = 0, gold = 0, silver = 0;

        amount = atoi ( arg );
        argument = one_argument ( argument, arg );
        if ( amount <= 0 ||
             ( str_cmp ( arg, "coins" ) && str_cmp ( arg, "coin" ) &&
               str_cmp ( arg, "gold" ) && str_cmp ( arg, "silver" ) &&
               str_cmp ( arg, "platinum" ) ) )
        {
            send_to_char ( "Sorry, you can't do that.\n\r", ch );
            return;
        }

        if ( amount > 50 )
        {
            send_to_char ( "You can't drop that much at once.\n\r", ch );
            return;
        }

        if ( !str_cmp ( arg, "coins" ) || !str_cmp ( arg, "coin" ) ||
             !str_cmp ( arg, "silver" ) )
        {
            if ( ( ch->silver + ( 100 * ch->gold ) +
                   ( 10000 * ch->platinum ) ) < amount )
            {
                send_to_char ( "You don't have that much silver.\n\r", ch );
                return;
            }

            deduct_cost ( ch, amount, VALUE_SILVER );
            silver = amount;
        }

        else if ( !str_cmp ( arg, "gold" ) )
        {
            if ( ( ch->silver + ( 100 * ch->gold ) +
                   ( 10000 * ch->platinum ) ) < amount * 100 )
            {
                send_to_char ( "You don't have that much gold.\n\r", ch );
                return;
            }

            deduct_cost ( ch, amount, VALUE_GOLD );
            gold = amount;
        }

        else if ( !str_cmp ( arg, "platinum" ) )
        {
            if ( ( ch->silver + ( 100 * ch->gold ) +
                   ( 10000 * ch->platinum ) ) < amount * 10000 )
            {
                send_to_char ( "You don't have that much platinum.\n\r", ch );
                return;
            }

            deduct_cost ( ch, amount, VALUE_PLATINUM );
            platinum = amount;
        }

        for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;

            switch ( obj->pIndexData->vnum )
            {
                case OBJ_VNUM_SILVER_ONE:
                    silver += 1;
                    extract_obj ( obj );
                    break;

                case OBJ_VNUM_GOLD_ONE:
                    gold += 1;
                    extract_obj ( obj );
                    break;

                case OBJ_VNUM_PLATINUM_ONE:
                    platinum += 1;
                    extract_obj ( obj );
                    break;

                case OBJ_VNUM_SILVER_SOME:
                    silver += obj->value[0];
                    extract_obj ( obj );
                    break;

                case OBJ_VNUM_GOLD_SOME:
                    gold += obj->value[1];
                    extract_obj ( obj );
                    break;

                case OBJ_VNUM_PLATINUM_SOME:
                    platinum += obj->value[2];
                    extract_obj ( obj );
                    break;

                case OBJ_VNUM_COINS:
                    silver += obj->value[0];
                    gold += obj->value[1];
                    platinum += obj->value[2];
                    extract_obj ( obj );
                    break;
            }
        }

        while ( silver >= 100 )
        {
            gold++;
            silver -= 100;
        }
        while ( gold >= 100 )
        {
            platinum++;
            gold -= 100;
        }
        if ( platinum > 50000 )
        {
            platinum = 50000;
	    send_to_char ( "You can't exceed 50000 platinum, dropping 50000...\n\r", ch );
        }
        obj_to_room ( create_money ( platinum, gold, silver ), ch->in_room );
        act ( "$n drops some money.", ch, NULL, NULL, TO_ROOM );
        send_to_char ( "OK.\n\r", ch );
        return;
    }

    if ( str_cmp ( arg, "all" ) && str_prefix ( "all.", arg ) )
    {
        /* 'drop obj' */
        if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
        {
            send_to_char ( "You do not have that item.\n\r", ch );
            return;
        }

        if ( !can_drop_obj ( ch, obj ) )
        {
            send_to_char ( "You can't let go of it.\n\r", ch );
            return;
        }

        obj_from_char ( obj );
        obj_to_room ( obj, ch->in_room );
        act ( "$n drops $p.", ch, obj, NULL, TO_ROOM );
        act ( "You drop $p.", ch, obj, NULL, TO_CHAR );
        if ( IS_OBJ_STAT ( obj, ITEM_MELT_DROP ) )
        {
            act ( "$p dissolves into smoke.", ch, obj, NULL, TO_ROOM );
            act ( "$p dissolves into smoke.", ch, obj, NULL, TO_CHAR );
            extract_obj ( obj );
        }
    }
    else
    {
	int number = 0, max_found;
	number = 0;
	max_found = 200;
        //output = new_buf (  );
	output = buffer_new ( 1337 );
        /* 'drop all' or 'drop all.obj' */
        found = FALSE;
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;

            if ( ( arg[3] == '\0' || is_name ( &arg[4], obj->name ) ) &&
                 can_see_obj ( ch, obj ) && obj->wear_loc == WEAR_NONE &&
                 can_drop_obj ( ch, obj ) )
            {
                found = TRUE;
//		number++;
                obj_from_char ( obj );
                obj_to_room ( obj, ch->in_room );
                sprintf ( buf, "You drop %s\n\r", obj->short_descr );
//                add_buf ( output, buf );
		buffer_strcat ( output, buf );
                if ( IS_OBJ_STAT ( obj, ITEM_MELT_DROP ) )
                {
                    sprintf ( buf, "%s dissolves into smoke.\n\r",
                              obj->short_descr );
//                    add_buf ( output, buf );
		    buffer_strcat ( output, buf );
                    extract_obj ( obj );
                }
		if ( number >= max_found )
		{
		   buffer_strcat ( output, "\n\rThat's all you can drop at once.\n\r" );
		   break;
		}
            }
        }

        if ( !found )
        {
            if ( arg[3] == '\0' )
                act ( "You are not carrying anything.", ch, NULL, arg,
                      TO_CHAR );
            else
                act ( "You are not carrying any $T.", ch, NULL, &arg[4],
                      TO_CHAR );
        }
        else
        {
            act ( "$n drops some things.", ch, NULL, NULL, TO_ROOM );
//            page_to_char ( buf_string ( output ), ch );
            page_to_char ( buffer_string ( output ), ch );
        }
//        free_buf ( output );
	buffer_free ( output );
    }

/*
 * Wiznet 'drops' code by Rhaelar 5'2004
 */   if (!IS_NPC(ch) && (ch->level < CREATOR) && (!str_cmp( arg, "all" )
							|| !str_prefix( "all.", arg ) ) )
	{
	sprintf(log_buf,"%s drops some stuff at %ld.",ch->name,ch->in_room->vnum);
	log_string(log_buf);
	wiznet(log_buf,NULL,NULL,WIZ_DROPS,0,0);
	}
	else if (!IS_NPC(ch) && (ch->level < DEMI))
   	{
	sprintf(log_buf,"%s drops %s at %ld.",
				ch->name,obj->short_descr,ch->in_room->vnum);
	log_string(log_buf);
	wiznet(log_buf,NULL,NULL,WIZ_DROPS,0,0);
	}
/*
 * Wiznet drop code end
 */
    return;
}

CH_CMD ( do_give )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Give what to whom?\n\r", ch );
        return;
    }

    if ( is_number ( arg1 ) )
    {
        /* 'give NNNN coins victim' */
        int amount, cost;
        long fullamount;
        int silver = 0, gold = 0, platinum = 0;

        amount = atoi ( arg1 );
        if ( amount <= 0 ||
             ( str_cmp ( arg2, "coins" ) && str_cmp ( arg2, "coin" ) &&
               str_cmp ( arg2, "gold" ) && str_cmp ( arg2, "silver" ) &&
               str_cmp ( arg2, "platinum" ) ) )
        {
            send_to_char ( "Sorry, you can't do that.\n\r", ch );
            return;
        }

        if ( !str_cmp ( arg2, "gold" ) )
        {
            gold = amount;
            fullamount = amount * 100;
        }
        else if ( !str_cmp ( arg2, "platinum" ) )
        {
            platinum = amount;
            fullamount = amount * 10000;
        }
        else
        {
            silver = amount;
            fullamount = amount;
        }

        argument = one_argument ( argument, arg2 );
        if ( arg2[0] == '\0' )
        {
            send_to_char ( "Give what to whom?\n\r", ch );
            return;
        }

        if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL )
        {
            send_to_char ( "They aren't here.\n\r", ch );
            return;
        }

        if ( ch->silver + ( ch->gold * 100 ) + ( ch->platinum * 10000 ) <
             fullamount )
        {
            send_to_char ( "You haven't got that much.\n\r", ch );
            return;
        }

        if ( amount > 50000 )
        {
            send_to_char ( "You can't give that much all at once.\n\r", ch );
            return;
        }

/*        if ( ( !IS_NPC ( ch ) && !IS_NPC ( victim ) ) &&
             ( !IS_IMMORTAL ( ch ) ) && ( !IS_IMMORTAL ( victim ) ) &&
             ( ch != victim ) &&
             ( !str_cmp ( ch->pcdata->socket, victim->pcdata->socket ) ) )
        {
            act ( "You can't seem to give $N anything.\n\r", ch, NULL, victim,
                  TO_CHAR );
            return;
        } */
        cost = 0;
        if ( silver != 0 )
        {
            cost = silver;
            deduct_cost ( ch, cost, VALUE_SILVER );
            add_cost ( victim, cost, VALUE_SILVER );
        }
        else if ( gold != 0 )
        {
            cost = gold;
            deduct_cost ( ch, cost, VALUE_GOLD );
            add_cost ( victim, cost, VALUE_GOLD );
        }
        else
        {
            cost = platinum;
            deduct_cost ( ch, cost, VALUE_PLATINUM );
            add_cost ( victim, cost, VALUE_PLATINUM );
        }
        act ( "$n gives $N some money.", ch, NULL, victim, TO_NOTVICT );
        if ( platinum != 0 )
        {
            sprintf ( buf, "$n gives you %d platinum.", platinum );
            act ( buf, ch, NULL, victim, TO_VICT );
            sprintf ( buf, "You give $N %d platinum.", platinum );
            act ( buf, ch, NULL, victim, TO_CHAR );
        }
        else if ( gold != 0 )
        {
            sprintf ( buf, "$n gives you %d gold.", gold );
            act ( buf, ch, NULL, victim, TO_VICT );
            sprintf ( buf, "You give $N %d gold.", gold );
            act ( buf, ch, NULL, victim, TO_CHAR );
        }
        else
        {
            sprintf ( buf, "$n gives you %d silver.", silver );
            act ( buf, ch, NULL, victim, TO_VICT );
            sprintf ( buf, "You give $N %d silver.", silver );
            act ( buf, ch, NULL, victim, TO_CHAR );
        }

        /*
         * Bribe trigger
         */
        if ( IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_BRIBE ) )
            mp_bribe_trigger ( victim, ch, silver ? amount : amount * 100 );

        if ( IS_NPC ( victim ) && IS_SET ( victim->act, ACT_IS_CHANGER ) &&
             !IS_NPC ( ch ) )
        {
            int change;

            act ( "$n tells you '{aI'm sorry, I no longer provide this service.{x'.", victim, NULL, ch, TO_VICT );
            ch->reply = victim;
            if ( platinum != 0 )
                sprintf ( buf, "%d platinum %s", platinum, ch->name );
            if ( gold != 0 )
                sprintf ( buf, "%d gold %s", gold, ch->name );
            if ( silver != 0 )
                sprintf ( buf, "%d silver %s", silver, ch->name );
            do_give ( victim, buf );
            return;

            if ( platinum != 0 )
            {
                act ( "$n tells you '{aI'm sorry, I can't convert past platinum.{x'.", victim, NULL, ch, TO_VICT );
                ch->reply = victim;
                sprintf ( buf, "%d platinum %s", platinum, ch->name );
                do_give ( victim, buf );
                return;
            }
            if ( silver != 0 )
            {
                change = ( 95 * silver / 100 / 100 );
            }
            else
            {
                change = ( 95 * gold / 100 / 100 );
            }

            if ( silver != 0 && change > victim->gold )
                victim->gold += change;

            if ( gold != 0 && change > victim->platinum )
                victim->platinum += change;

            if ( change < 1 && can_see ( victim, ch ) )
            {
                act ( "$n tells you '{aI'm sorry, you did not give me enough to change{x'.", victim, NULL, ch, TO_VICT );
                ch->reply = victim;
                sprintf ( buf, "%d %s %s", amount,
                          silver != 0 ? "silver" : "gold", ch->name );
                do_give ( victim, buf );
            }
            else if ( can_see ( victim, ch ) )
            {
                sprintf ( buf, "%d %s %s", change,
                          silver != 0 ? "gold" : "platinum", ch->name );
                do_give ( victim, buf );
                sprintf ( buf, "%d %s %s",
                          ( 95 * amount / 100 - change * 100 ),
                          silver != 0 ? "silver" : "gold", ch->name );
                do_give ( victim, buf );
                act ( "$n tells you '{aThank you, come again{x'.", victim,
                      NULL, ch, TO_VICT );
                ch->reply = victim;
            }
        }
        return;
    }

    if ( ( obj = get_obj_carry ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
        send_to_char ( "You must remove it first.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) && victim->pIndexData->pShop != NULL )
    {
        act ( "$N tells you '{aSorry, you'll have to sell that{x'.", ch, NULL,
              victim, TO_CHAR );
        ch->reply = victim;
        return;
    }

    if ( !can_drop_obj ( ch, obj ) )
    {
        send_to_char ( "You can't let go of it.\n\r", ch );
        return;
    }
/*
    if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) && ch->level <= IMMORTAL )
    {
        send_to_char ( "You can't give quest items.\n\r", ch );
        return;
    }
*/
    if ( ( obj->pIndexData->vnum == OBJ_VNUM_VOODOO ) &&
         ( ch->level <= HERO ) )
    {
        send_to_char ( "You can't give voodoo dolls.\n\r", ch );
        return;
    }

    if ( victim->carry_number + get_obj_number ( obj ) >
         can_carry_n ( victim ) )
    {
        act ( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( get_carry_weight ( victim ) + get_obj_weight ( obj ) >
         can_carry_w ( victim ) )
    {
        act ( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( !can_see_obj ( victim, obj ) )
    {
        act ( "$N can't see it.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ( !IS_NPC ( ch ) && !IS_NPC ( victim ) ) && ( !IS_IMMORTAL ( ch ) )
         && ( !IS_IMMORTAL ( victim ) ) && ( ch != victim ) &&
         ( !str_cmp ( ch->pcdata->socket, victim->pcdata->socket ) ) )
    {
        act ( "You can't seem to give $N anything.\n\r", ch, NULL, victim,
              TO_CHAR );
        return;
    }

    obj_from_char ( obj );
    obj_to_char ( obj, victim );
    MOBtrigger = FALSE;
    act ( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
    act ( "$n gives you $p.", ch, obj, victim, TO_VICT );
    act ( "You give $p to $N.", ch, obj, victim, TO_CHAR );
    MOBtrigger = TRUE;

    /*
     * Give trigger
     */
    if ( IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_GIVE ) )
        mp_give_trigger ( victim, ch, obj );
    if ( IS_OBJ_STAT ( obj, ITEM_FORCED ) && ( victim->level <= HERO ) )
    {
        do_wear ( victim, obj->name );
    }

    return;
}

/* for poisoning weapons and food/drink */
CH_CMD ( do_envenom )
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int percent, skill;

    /* find out what */
    if ( argument == '\0' )
    {
        send_to_char ( "Envenom what item?\n\r", ch );
        return;
    }

    obj = get_obj_list ( ch, argument, ch->carrying );

    if ( obj == NULL )
    {
        send_to_char ( "You don't have that item.\n\r", ch );
        return;
    }

    if ( ( skill = get_skill ( ch, gsn_envenom ) ) < 1 )
    {
        send_to_char ( "Are you crazy? You'd poison yourself!\n\r", ch );
        return;
    }

    if ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON )
    {
        if ( IS_OBJ_STAT ( obj, ITEM_BLESS ) ||
             IS_OBJ_STAT ( obj, ITEM_BURN_PROOF ) )
        {
            act ( "You fail to poison $p.", ch, obj, NULL, TO_CHAR );
            return;
        }

        if ( number_percent (  ) < skill )  /* success! */
        {
            act ( "$n treats $p with deadly poison.", ch, obj, NULL,
                  TO_ROOM );
            act ( "You treat $p with deadly poison.", ch, obj, NULL,
                  TO_CHAR );
            if ( !obj->value[3] )
            {
                obj->value[3] = 1;
                check_improve ( ch, gsn_envenom, TRUE, 4 );
            }
            WAIT_STATE ( ch, skill_table[gsn_envenom].beats );
            return;
        }

        act ( "You fail to poison $p.", ch, obj, NULL, TO_CHAR );
        if ( !obj->value[3] )
            check_improve ( ch, gsn_envenom, FALSE, 4 );
        WAIT_STATE ( ch, skill_table[gsn_envenom].beats );
        return;
    }

    if ( obj->item_type == ITEM_WEAPON )
    {
        if ( IS_WEAPON_STAT ( obj, WEAPON_FLAMING ) ||
             IS_WEAPON_STAT ( obj, WEAPON_FROST ) ||
             IS_WEAPON_STAT ( obj, WEAPON_VAMPIRIC ) ||
             IS_WEAPON_STAT ( obj, WEAPON_SHARP ) ||
             IS_WEAPON_STAT ( obj, WEAPON_VORPAL ) ||
             IS_WEAPON_STAT ( obj, WEAPON_SHOCKING ) ||
             IS_OBJ_STAT ( obj, ITEM_BLESS ) ||
             IS_OBJ_STAT ( obj, ITEM_BURN_PROOF ) )
        {
            act ( "You can't seem to envenom $p.", ch, obj, NULL, TO_CHAR );
            return;
        }

        if ( obj->value[3] < 0 ||
             attack_table[obj->value[3]].damage == DAM_BASH )
        {
            send_to_char ( "You can only envenom edged weapons.\n\r", ch );
            return;
        }

        if ( IS_WEAPON_STAT ( obj, WEAPON_POISON ) )
        {
            act ( "$p is already envenomed.", ch, obj, NULL, TO_CHAR );
            return;
        }

        percent = number_percent (  );
        if ( percent < skill )
        {

            af.where = TO_WEAPON;
            af.type = gsn_poison;
            af.level = ch->level * percent / 100;
            af.duration = ch->level / 2 * percent / 100;
            af.location = 0;
            af.modifier = 0;
            af.bitvector = WEAPON_POISON;
            affect_to_obj ( obj, &af );

            act ( "$n coats $p with deadly venom.", ch, obj, NULL, TO_ROOM );
            act ( "You coat $p with venom.", ch, obj, NULL, TO_CHAR );
            check_improve ( ch, gsn_envenom, TRUE, 3 );
            WAIT_STATE ( ch, skill_table[gsn_envenom].beats );
            return;
        }
        else
        {
            act ( "You fail to envenom $p.", ch, obj, NULL, TO_CHAR );
            check_improve ( ch, gsn_envenom, FALSE, 3 );
            WAIT_STATE ( ch, skill_table[gsn_envenom].beats );
            return;
        }
    }

    act ( "You can't poison $p.", ch, obj, NULL, TO_CHAR );
    return;
}

CH_CMD ( do_fill )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    bool found;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Fill what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    found = FALSE;
    for ( fountain = ch->in_room->contents; fountain != NULL;
          fountain = fountain->next_content )
    {
        if ( fountain->item_type == ITEM_FOUNTAIN )
        {
            found = TRUE;
            break;
        }
    }

    if ( !found )
    {
        send_to_char ( "There is no fountain here!\n\r", ch );
        return;
    }

    if ( obj->item_type != ITEM_DRINK_CON )
    {
        send_to_char ( "You can't fill that.\n\r", ch );
        return;
    }

    if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] )
    {
        send_to_char ( "There is already another liquid in it.\n\r", ch );
        return;
    }

    if ( obj->value[1] >= obj->value[0] )
    {
        send_to_char ( "Your container is full.\n\r", ch );
        return;
    }

    if ( !str_cmp ( liq_table[fountain->value[2]].liq_name, "blood" ) )
    {
        sprintf ( buf, "You get some %s from $P.",
                  liq_table[fountain->value[2]].liq_name );
        act ( buf, ch, obj, fountain, TO_CHAR );
        sprintf ( buf, "$n gets some %s from $P.",
                  liq_table[fountain->value[2]].liq_name );
        act ( buf, ch, obj, fountain, TO_ROOM );
        obj->value[2] = fountain->value[2];
        obj->value[1]++;
        extract_obj ( fountain );
        return;
    }

    sprintf ( buf, "You fill $p with %s from $P.",
              liq_table[fountain->value[2]].liq_name );
    act ( buf, ch, obj, fountain, TO_CHAR );
    sprintf ( buf, "$n fills $p with %s from $P.",
              liq_table[fountain->value[2]].liq_name );
    act ( buf, ch, obj, fountain, TO_ROOM );
    obj->value[2] = fountain->value[2];
    obj->value[1] = obj->value[0];
    return;
}

CH_CMD ( do_pour )
{
    char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
    OBJ_DATA *out, *in;
    CHAR_DATA *vch = NULL;
    int amount;

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char ( "Pour what into what?\n\r", ch );
        return;
    }

    if ( ( out = get_obj_carry ( ch, arg ) ) == NULL )
    {
        send_to_char ( "You don't have that item.\n\r", ch );
        return;
    }

    if ( out->item_type != ITEM_DRINK_CON )
    {
        send_to_char ( "That's not a drink container.\n\r", ch );
        return;
    }

    if ( !str_cmp ( argument, "out" ) )
    {
        if ( out->value[1] == 0 )
        {
            send_to_char ( "It's already empty.\n\r", ch );
            return;
        }

        out->value[1] = 0;
        out->value[3] = 0;
        sprintf ( buf, "You invert $p, spilling %s all over the ground.",
                  liq_table[out->value[2]].liq_name );
        act ( buf, ch, out, NULL, TO_CHAR );

        sprintf ( buf, "$n inverts $p, spilling %s all over the ground.",
                  liq_table[out->value[2]].liq_name );
        act ( buf, ch, out, NULL, TO_ROOM );
        return;
    }

    if ( ( in = get_obj_here ( ch, argument ) ) == NULL )
    {
        vch = get_char_room ( ch, argument );

        if ( vch == NULL )
        {
            send_to_char ( "Pour into what?\n\r", ch );
            return;
        }

        in = get_eq_char ( vch, WEAR_HOLD );

        if ( in == NULL )
        {
            send_to_char ( "They aren't holding anything.", ch );
            return;
        }
    }

    if ( in->item_type != ITEM_DRINK_CON )
    {
        send_to_char ( "You can only pour into other drink containers.\n\r",
                       ch );
        return;
    }

    if ( in == out )
    {
        send_to_char ( "You cannot change the laws of physics!\n\r", ch );
        return;
    }

    if ( in->value[1] != 0 && in->value[2] != out->value[2] )
    {
        send_to_char ( "They don't hold the same liquid.\n\r", ch );
        return;
    }

    if ( out->value[1] == 0 )
    {
        act ( "There's nothing in $p to pour.", ch, out, NULL, TO_CHAR );
        return;
    }

    if ( in->value[1] >= in->value[0] )
    {
        act ( "$p is already filled to the top.", ch, in, NULL, TO_CHAR );
        return;
    }

    amount = UMIN ( out->value[1], in->value[0] - in->value[1] );

    in->value[1] += amount;
    out->value[1] -= amount;
    in->value[2] = out->value[2];

    if ( vch == NULL )
    {
        sprintf ( buf, "You pour %s from $p into $P.",
                  liq_table[out->value[2]].liq_name );
        act ( buf, ch, out, in, TO_CHAR );
        sprintf ( buf, "$n pours %s from $p into $P.",
                  liq_table[out->value[2]].liq_name );
        act ( buf, ch, out, in, TO_ROOM );
    }
    else
    {
        sprintf ( buf, "You pour some %s for $N.",
                  liq_table[out->value[2]].liq_name );
        act ( buf, ch, NULL, vch, TO_CHAR );
        sprintf ( buf, "$n pours you some %s.",
                  liq_table[out->value[2]].liq_name );
        act ( buf, ch, NULL, vch, TO_VICT );
        sprintf ( buf, "$n pours some %s for $N.",
                  liq_table[out->value[2]].liq_name );
        act ( buf, ch, NULL, vch, TO_NOTVICT );

    }
}

CH_CMD ( do_drink )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
        {
            if ( obj->item_type == ITEM_FOUNTAIN )
                break;
        }

        if ( obj == NULL )
        {
            send_to_char ( "Drink what?\n\r", ch );
            return;
        }
    }
    else
    {
        if ( ( obj = get_obj_here ( ch, arg ) ) == NULL )
        {
            send_to_char ( "You can't find it.\n\r", ch );
            return;
        }
    }

    if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
    {
        send_to_char ( "You fail to reach your mouth.  *Hic*\n\r", ch );
        return;
    }

    switch ( obj->item_type )
    {
        default:
            send_to_char ( "You can't drink from that.\n\r", ch );
            return;

        case ITEM_FOUNTAIN:
            if ( ( liquid = obj->value[2] ) < 0 )
            {
                bug ( "Do_drink: bad liquid number %d.", liquid );
                liquid = obj->value[2] = 0;
            }
            amount = liq_table[liquid].liq_affect[4] * 3;
            break;

        case ITEM_DRINK_CON:
            if ( obj->value[1] <= 0 )
            {
                send_to_char ( "It is already empty.\n\r", ch );
                return;
            }

            if ( ( liquid = obj->value[2] ) < 0 )
            {
                bug ( "Do_drink: bad liquid number %d.", liquid );
                liquid = obj->value[2] = 0;
            }

            amount = liq_table[liquid].liq_affect[4];
            amount = UMIN ( amount, obj->value[1] );
            break;
    }
    if ( !IS_NPC ( ch ) && !IS_IMMORTAL ( ch ) &&
         ch->pcdata->condition[COND_FULL] > 45 )
    {
        send_to_char ( "You're too full to drink more.\n\r", ch );
        return;
    }

    act ( "$n drinks $T from $p.", ch, obj, liq_table[liquid].liq_name,
          TO_ROOM );
    act ( "You drink $T from $p.", ch, obj, liq_table[liquid].liq_name,
          TO_CHAR );

    gain_condition ( ch, COND_DRUNK,
                     amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36 );
    gain_condition ( ch, COND_FULL,
                     amount * liq_table[liquid].liq_affect[COND_FULL] / 4 );
    gain_condition ( ch, COND_THIRST,
                     amount * liq_table[liquid].liq_affect[COND_THIRST] /
                     10 );
    gain_condition ( ch, COND_HUNGER,
                     amount * liq_table[liquid].liq_affect[COND_HUNGER] / 2 );

    if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
        send_to_char ( "You feel drunk.\n\r", ch );
    if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_FULL] > 40 )
        send_to_char ( "You are full.\n\r", ch );
    if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_THIRST] > 40 )
        send_to_char ( "Your thirst is quenched.\n\r", ch );

    if ( !str_cmp ( liq_table[liquid].liq_name, "blood" ) &&
         ( !str_cmp ( class_table[ch->class].name, "Vampire" ) ||
           ( !str_cmp ( class_table[ch->class].name, "Cainite" ) ||
             ( !str_cmp ( class_table[ch->class].name, "Revenant" ) ||
               ( !str_cmp ( class_table[ch->class].name, "Lich" ) ) ) ) ) )
    {
        ch->hit += ch->max_hit / 20;
        ch->hit = UMIN ( ch->hit, ch->max_hit );
        ch->mana += ch->max_mana / 15;
        ch->mana = UMIN ( ch->mana, ch->max_mana );
        ch->move += ch->max_move / 15;
        ch->move = UMIN ( ch->move, ch->max_move );
    }

    if ( obj->value[3] != 0 )
    {
        /* The drink was poisoned ! */
        AFFECT_DATA af;

        act ( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
        send_to_char ( "You choke and gag.\n\r", ch );
        af.where = TO_AFFECTS;
        af.type = gsn_poison;
        af.level = number_fuzzy ( amount );
        af.duration = 3 * amount;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_POISON;
        affect_join ( ch, &af );
    }

    if ( obj->value[0] > 0 )
        obj->value[1] -= amount;

    switch ( obj->item_type )
    {
        default:
            send_to_char ( "You can't drink from that.\n\r", ch );
            return;

        case ITEM_FOUNTAIN:
            if ( !str_cmp ( liq_table[liquid].liq_name, "blood" ) )
                extract_obj ( obj );
            break;

        case ITEM_DRINK_CON:
            break;
    }

    return;
}

CH_CMD ( do_restring )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *trainer;

    if ( IS_NPC ( ch ) )
        return;

    argument = one_argument ( argument, arg );

    for ( trainer = ch->in_room->people; trainer != NULL;
          trainer = trainer->next_in_room )
        if ( IS_NPC ( trainer ) && IS_SET ( trainer->act, ACT_GAIN ) )
            break;

    if ( trainer == NULL || !can_see ( ch, trainer ) )
    {
        send_to_char ( "You can't do that here.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char ( "Restring what to what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }
    if ( obj->wear_loc != WEAR_NONE )
    {
        send_to_char ( "You must remove it first.\n\r", ch );
        return;
    }
    if ( ch->qps < 5 )
    {
        send_to_char
            ( "Restrings cost 5 imm quest points, you do not have enough.\n\r",
              ch );
        return;
    }
    smash_tilde ( argument );
    sprintf ( buf, "%s{x", argument );
    act ( "You give $p to $N.", ch, obj, trainer, TO_CHAR );
    act ( "$n gives $p to $N.", ch, obj, trainer, TO_NOTVICT );
    free_string ( obj->short_descr );
    obj->short_descr = str_dup ( buf );
    ch->qps -= 5;
    act ( "$N gives $p to you.", ch, obj, trainer, TO_CHAR );
    act ( "$N gives $p to $n.", ch, obj, trainer, TO_NOTVICT );
    return;
}

CH_CMD ( do_eat )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Eat what?\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    if ( !IS_IMMORTAL ( ch ) )
    {
        if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
        {
            send_to_char ( "That's not edible.\n\r", ch );
            return;
        }
/*
    if ( ch->level < obj->level )
    {
        send_to_char ( "It is too powerful for you to eat.\n\r",
                       ch );
        return;
    }    
*/
    if ( IS_SET(ch->in_room->room_flags,ROOM_ARENA) )
    {
        send_to_char ( "I think you should worry about fighting rather than eating now.\n\r",
                       ch );
        return;
    }


        if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_FULL] > 40 && obj->item_type != ITEM_PILL )
        {
            send_to_char ( "You are too full to eat more.\n\r", ch );
            return;
        }
    }

    act ( "$n eats $p.", ch, obj, NULL, TO_ROOM );
    act ( "You eat $p.", ch, obj, NULL, TO_CHAR );

    switch ( obj->item_type )
    {

        case ITEM_FOOD:
            if ( !IS_NPC ( ch ) )
            {
                int condition;

                condition = ch->pcdata->condition[COND_HUNGER];
                gain_condition ( ch, COND_FULL, obj->value[0] );
                gain_condition ( ch, COND_HUNGER, obj->value[1] );
                if ( condition == 0 &&
                     ch->pcdata->condition[COND_HUNGER] > 0 )
                    send_to_char ( "You are no longer hungry.\n\r", ch );
                else if ( ch->pcdata->condition[COND_FULL] > 40 )
                    send_to_char ( "You are full.\n\r", ch );
            }

            if ( obj->value[3] != 0 )
            {
                /* The food was poisoned! */
                AFFECT_DATA af;

                act ( "$n chokes and gags.", ch, 0, 0, TO_ROOM );
                send_to_char ( "You choke and gag.\n\r", ch );

                af.where = TO_AFFECTS;
                af.type = gsn_poison;
                af.level = number_fuzzy ( obj->value[0] );
                af.duration = 2 * obj->value[0];
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = AFF_POISON;
                affect_join ( ch, &af );
            }
            break;

        case ITEM_PILL:
            obj_cast_spell ( obj->value[1], obj->value[0], ch, ch, NULL );
            obj_cast_spell ( obj->value[2], obj->value[0], ch, ch, NULL );
            obj_cast_spell ( obj->value[3], obj->value[0], ch, ch, NULL );
            obj_cast_spell ( obj->value[4], obj->value[0], ch, ch, NULL );
            break;
    }

    extract_obj ( obj );
    return;
}

/*
 * Remove an object.
 */
bool remove_obj ( CHAR_DATA * ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char ( ch, iWear ) ) == NULL )
        return TRUE;

    if ( !fReplace )
        return FALSE;

    if ( IS_SET ( obj->extra_flags, ITEM_NOREMOVE ) &&
         ( ch->level < LEVEL_IMMORTAL ) )
    {
        act ( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
        return FALSE;
    }

    unequip_char ( ch, obj );
    act ( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act ( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    if ( IS_NPC ( ch ) )
        return TRUE;
    if ( ( obj->item_type == ITEM_DEMON_STONE ) && ( ch->pet != NULL ) &&
         ( ch->pet->pIndexData->vnum == MOB_VNUM_DEMON ) )
    {
        act ( "$N slowly fades away.", ch, NULL, ch->pet, TO_CHAR );
        nuke_pets ( ch );
    }
    return TRUE;
}

/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj ( CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace )
{
    OBJ_DATA *shieldobj;
    char buf[MAX_STRING_LENGTH];

    if ( ( ( ch->level < obj->level ) && ( ch->class < MAX_CLASS / 2 ) &&
           ( obj->level > 19 ) ) || ( ( ch->level < obj->level ) &&
                                      ( ch->class >= MAX_CLASS / 2 ) &&
                                      ( obj->level > 27 ) ) )
    {
        sprintf ( buf, "You must be level %d to use this object.\n\r",
                  obj->level );
        send_to_char ( buf, ch );
        act ( "$n tries to use $p, but is too inexperienced.", ch, obj, NULL,
              TO_ROOM );
        return;
    }

    if ( obj->item_type == ITEM_LIGHT )
    {
        if ( !remove_obj ( ch, WEAR_LIGHT, fReplace ) )
            return;
        act ( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
        act ( "You light $p and hold it.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_LIGHT );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_FINGER ) )
    {
        if ( get_eq_char ( ch, WEAR_FINGER_L ) != NULL &&
             get_eq_char ( ch, WEAR_FINGER_R ) != NULL &&
             !remove_obj ( ch, WEAR_FINGER_L, fReplace ) &&
             !remove_obj ( ch, WEAR_FINGER_R, fReplace ) )
            return;

        if ( get_eq_char ( ch, WEAR_FINGER_L ) == NULL )
        {
            act ( "$n wears $p on $s left finger.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p on your left finger.", ch, obj, NULL,
                  TO_CHAR );
            equip_char ( ch, obj, WEAR_FINGER_L );
            return;
        }

        if ( get_eq_char ( ch, WEAR_FINGER_R ) == NULL )
        {
            act ( "$n wears $p on $s right finger.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p on your right finger.", ch, obj, NULL,
                  TO_CHAR );
            equip_char ( ch, obj, WEAR_FINGER_R );
            return;
        }

        bug ( "Wear_obj: no free finger.", 0 );
        send_to_char ( "You already wear two rings.\n\r", ch );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_NECK ) )
    {
        if ( get_eq_char ( ch, WEAR_NECK_1 ) != NULL &&
             get_eq_char ( ch, WEAR_NECK_2 ) != NULL &&
             !remove_obj ( ch, WEAR_NECK_1, fReplace ) &&
             !remove_obj ( ch, WEAR_NECK_2, fReplace ) )
            return;

        if ( get_eq_char ( ch, WEAR_NECK_1 ) == NULL )
        {
            act ( "$n wears $p around $s neck.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
            equip_char ( ch, obj, WEAR_NECK_1 );
            return;
        }

        if ( get_eq_char ( ch, WEAR_NECK_2 ) == NULL )
        {
            act ( "$n wears $p around $s neck.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
            equip_char ( ch, obj, WEAR_NECK_2 );
            return;
        }

        bug ( "Wear_obj: no free neck.", 0 );
        send_to_char ( "You already wear two neck items.\n\r", ch );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_BODY ) )
    {
        if ( !remove_obj ( ch, WEAR_BODY, fReplace ) )
            return;
        act ( "$n wears $p on $s body.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p on your body.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_BODY );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_FACE ) )
    {
        if ( !remove_obj ( ch, WEAR_FACE, fReplace ) )
            return;
        act ( "$n wears $p on $s face.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p on your face.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_FACE );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_HEAD ) )
    {
        if ( !remove_obj ( ch, WEAR_HEAD, fReplace ) )
            return;
        act ( "$n wears $p on $s head.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p on your head.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_HEAD );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_LEGS ) )
    {
        if ( !remove_obj ( ch, WEAR_LEGS, fReplace ) )
            return;
        act ( "$n wears $p on $s legs.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p on your legs.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_LEGS );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_FEET ) )
    {
        if ( !remove_obj ( ch, WEAR_FEET, fReplace ) )
            return;
        act ( "$n wears $p on $s feet.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_FEET );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_HANDS ) )
    {
        if ( !remove_obj ( ch, WEAR_HANDS, fReplace ) )
            return;
        act ( "$n wears $p on $s hands.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_HANDS );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_ARMS ) )
    {
        if ( !remove_obj ( ch, WEAR_ARMS, fReplace ) )
            return;
        act ( "$n wears $p on $s arms.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_ARMS );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_ABOUT ) )
    {
        if ( !remove_obj ( ch, WEAR_ABOUT, fReplace ) )
            return;
        act ( "$n wears $p about $s torso.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p about your torso.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_ABOUT );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_WAIST ) )
    {
        if ( !remove_obj ( ch, WEAR_WAIST, fReplace ) )
            return;
        act ( "$n wears $p about $s waist.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_WAIST );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_WRIST ) )
    {
        if ( get_eq_char ( ch, WEAR_WRIST_L ) != NULL &&
             get_eq_char ( ch, WEAR_WRIST_R ) != NULL &&
             !remove_obj ( ch, WEAR_WRIST_L, fReplace ) &&
             !remove_obj ( ch, WEAR_WRIST_R, fReplace ) )
            return;

        if ( get_eq_char ( ch, WEAR_WRIST_L ) == NULL )
        {
            act ( "$n wears $p around $s left wrist.", ch, obj, NULL,
                  TO_ROOM );
            act ( "You wear $p around your left wrist.", ch, obj, NULL,
                  TO_CHAR );
            equip_char ( ch, obj, WEAR_WRIST_L );
            return;
        }

        if ( get_eq_char ( ch, WEAR_WRIST_R ) == NULL )
        {
            act ( "$n wears $p around $s right wrist.", ch, obj, NULL,
                  TO_ROOM );
            act ( "You wear $p around your right wrist.", ch, obj, NULL,
                  TO_CHAR );
            equip_char ( ch, obj, WEAR_WRIST_R );
            return;
        }

        bug ( "Wear_obj: no free wrist.", 0 );
        send_to_char ( "You already wear two wrist items.\n\r", ch );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_SHIELD ) )
    {
        OBJ_DATA *weapon;
        bool levitate;

        if ( !remove_obj ( ch, WEAR_SHIELD, fReplace ) )
            return;

        if ( !remove_obj ( ch, WEAR_SHIELD, fReplace ) )
            return;

        weapon = get_eq_char ( ch, WEAR_WIELD );
        levitate = ( ( get_skill ( ch, gsn_shield_levitation ) != 0 ) &&
                     ( skill_table[gsn_shield_levitation].
                       skill_level[ch->class] <= ch->level ) );

        if ( weapon != NULL && IS_WEAPON_STAT ( weapon, WEAPON_TWO_HANDS ) &&
             !levitate )
        {
            send_to_char ( "Your hands are tied up with your weapon!\n\r",
                           ch );
            return;
        }

        if ( ( get_eq_char ( ch, WEAR_SECONDARY ) != NULL ) && !levitate )
        {
            send_to_char
                ( "You cannot use a shield while using 2 weapons.\n\r", ch );
            return;
        }

        if ( !levitate )
        {
            act ( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
        }

        if ( levitate )
        {
            if ( ( weapon != NULL &&
                   IS_WEAPON_STAT ( weapon, WEAPON_TWO_HANDS ) ) ||
                 ( get_eq_char ( ch, WEAR_SECONDARY ) != NULL ) )
            {
                act ( "$n levitates $p in front of $m.", ch, obj, NULL,
                      TO_ROOM );
                act ( "You levitate $p in front of you.", ch, obj, NULL,
                      TO_CHAR );
            }
            else
            {
                act ( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
                act ( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
            }
        }
        equip_char ( ch, obj, WEAR_SHIELD );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WIELD ) )
    {
        int sn, skill;
        bool levitate;

        levitate = ( ( get_skill ( ch, gsn_shield_levitation ) != 0 ) &&
                     ( skill_table[gsn_shield_levitation].
                       skill_level[ch->class] <= ch->level ) );

        if ( !remove_obj ( ch, WEAR_WIELD, fReplace ) )
            return;

        if ( !IS_NPC ( ch ) &&
             get_obj_weight ( obj ) >
             ( str_app[get_curr_stat ( ch, STAT_STR )].wield * 10 ) )
        {
            send_to_char ( "It is too heavy for you to wield.\n\r", ch );
            return;
        }
        if ( !IS_NPC ( ch ) && IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) &&
             ( get_eq_char ( ch, WEAR_SHIELD ) != NULL ||
               get_eq_char ( ch, WEAR_SECONDARY ) != NULL ) )
        {
            if ( !levitate && IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) &&
                 get_eq_char ( ch, WEAR_SHIELD ) != NULL )
            {
                send_to_char ( "You need two hands free for that weapon.\n\r",
                               ch );
                return;
            }
            if ( !IS_NPC ( ch ) && IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) &&
                 get_eq_char ( ch, WEAR_SECONDARY ) != NULL )
            {
                send_to_char ( "You need two hands free for that weapon.\n\r",
                               ch );
                return;
            }
            else
            {
                shieldobj = get_eq_char ( ch, WEAR_SHIELD );
                act ( "$n levitates $p in front of $m.", ch, shieldobj, NULL,
                      TO_ROOM );
                act ( "You levitate $p in front of you.", ch, shieldobj, NULL,
                      TO_CHAR );
            }
        }
        act ( "$n wields $p.", ch, obj, NULL, TO_ROOM );
        act ( "You wield $p.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_WIELD );

        sn = get_weapon_sn ( ch );

        if ( sn == gsn_hand_to_hand )
            return;

        skill = get_weapon_skill ( ch, sn );

        if ( skill >= 100 )
            act ( "$p feels like a part of you!", ch, obj, NULL, TO_CHAR );
        else if ( skill > 85 )
            act ( "You feel quite confident with $p.", ch, obj, NULL,
                  TO_CHAR );
        else if ( skill > 70 )
            act ( "You are skilled with $p.", ch, obj, NULL, TO_CHAR );
        else if ( skill > 50 )
            act ( "Your skill with $p is adequate.", ch, obj, NULL, TO_CHAR );
        else if ( skill > 25 )
            act ( "$p feels a little clumsy in your hands.", ch, obj, NULL,
                  TO_CHAR );
        else if ( skill > 1 )
            act ( "You fumble and almost drop $p.", ch, obj, NULL, TO_CHAR );
        else
            act ( "You don't even know which end is up on $p.", ch, obj, NULL,
                  TO_CHAR );

        return;
    }

    if ( CAN_WEAR ( obj, ITEM_HOLD ) )
    {
        if ( !remove_obj ( ch, WEAR_HOLD, fReplace ) )
            return;

        if ( get_eq_char ( ch, WEAR_SECONDARY ) != NULL )
        {
            send_to_char
                ( "You cannot hold an item while using 2 weapons.\n\r", ch );
            return;
        }

        act ( "$n holds $p in $s hand.", ch, obj, NULL, TO_ROOM );
        act ( "You hold $p in your hand.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_HOLD );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_FLOAT ) )
    {
        if ( !remove_obj ( ch, WEAR_FLOAT, fReplace ) )
            return;
        act ( "$n releases $p to float next to $m.", ch, obj, NULL, TO_ROOM );
        act ( "You release $p and it floats next to you.", ch, obj, NULL,
              TO_CHAR );
        equip_char ( ch, obj, WEAR_FLOAT );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_ANKLE ) )
    {
        if ( get_eq_char ( ch, WEAR_ANKLE_L ) != NULL &&
             get_eq_char ( ch, WEAR_ANKLE_R ) != NULL &&
             !remove_obj ( ch, WEAR_ANKLE_L, fReplace ) &&
             !remove_obj ( ch, WEAR_ANKLE_R, fReplace ) )
            return;

        if ( get_eq_char ( ch, WEAR_ANKLE_L ) == NULL )
        {
            act ( "$n wears $p on $s left ankle.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p on your left ankle.", ch, obj, NULL, TO_CHAR );
            equip_char ( ch, obj, WEAR_ANKLE_L );
            return;
        }

        if ( get_eq_char ( ch, WEAR_ANKLE_R ) == NULL )
        {
            act ( "$n wears $p on $s right ankle.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p on your right ankle.", ch, obj, NULL,
                  TO_CHAR );
            equip_char ( ch, obj, WEAR_ANKLE_R );
            return;
        }
        bug ( "Wear_obj: no free ankle.", 0 );
        send_to_char ( "You already wearing two anklets.\n\r", ch );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_TATTOO ) )
    {
        if ( !remove_obj ( ch, WEAR_TATTOO, fReplace ) )
            return;
        act ( "$n wears $p as a tattoo.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p as a tattoo.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_TATTOO );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_CTATTOO ) )
    {
        if ( !remove_obj ( ch, WEAR_CTATTOO, fReplace ) )
            return;
        act ( "$n wears $p as a clan tattoo.", ch, obj, NULL, TO_ROOM );
        act ( "You wear $p as a clan tattoo.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_CTATTOO );
        return;
    }

    if ( CAN_WEAR ( obj, ITEM_WEAR_AURA ) )
    {
        if ( !remove_obj ( ch, WEAR_AURA, fReplace ) )
            return;
        act ( "$n's aura is now $p.", ch, obj, NULL, TO_ROOM );
        act ( "Your aura is now $p.", ch, obj, NULL, TO_CHAR );
        equip_char ( ch, obj, WEAR_AURA );
        return;
    }
    if ( CAN_WEAR ( obj, ITEM_WEAR_EAR ) )
    {
        if ( get_eq_char ( ch, WEAR_EAR_L ) != NULL &&
             get_eq_char ( ch, WEAR_EAR_R ) != NULL &&
             !remove_obj ( ch, WEAR_EAR_L, fReplace ) &&
             !remove_obj ( ch, WEAR_EAR_R, fReplace ) )
            return;
        if ( get_eq_char ( ch, WEAR_EAR_L ) == NULL )
        {
            act ( "$n wears $p on $s left ear.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p on your left ear.", ch, obj, NULL, TO_CHAR );
            equip_char ( ch, obj, WEAR_EAR_L );
            return;
        }

        if ( get_eq_char ( ch, WEAR_EAR_R ) == NULL )
        {
            act ( "$n wears $p on $s right ear.", ch, obj, NULL, TO_ROOM );
            act ( "You wear $p on your right ear.", ch, obj, NULL, TO_CHAR );
            equip_char ( ch, obj, WEAR_EAR_R );
            return;
        }

        bug ( "Wear_obj: no free ear.", 0 );
        send_to_char ( "You already wearing two earrings.\n\r", ch );
        return;
    }
    if ( fReplace )
        send_to_char ( "You can't wear, wield, or hold that.\n\r", ch );

    return;
}

CH_CMD ( do_wear )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Wear, wield, or hold what?\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "all" ) )
    {
        OBJ_DATA *obj_next;

        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if ( obj->wear_loc == WEAR_NONE && can_see_obj ( ch, obj ) )
                wear_obj ( ch, obj, FALSE );
        }
        return;
    }
    else
    {
        if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
        {
            send_to_char ( "You do not have that item.\n\r", ch );
            return;
        }

        wear_obj ( ch, obj, TRUE );
    }

    return;
}

CH_CMD ( do_remove )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Remove what?\n\r", ch );
        return;
    }

    if ( str_cmp ( arg, "all" ) && str_prefix ( "all.", arg ) )
    {
        if ( ( obj = get_obj_wear ( ch, arg ) ) == NULL )
        {
            send_to_char ( "You do not have that item.\n\r", ch );
            return;
        }
        remove_obj ( ch, obj->wear_loc, TRUE );
    }
    else
    {
        found = FALSE;
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;

            if ( ( arg[3] == '\0' || is_name ( &arg[4], obj->name ) ) &&
                 can_see_obj ( ch, obj ) && obj->wear_loc != WEAR_NONE )
            {
                found = TRUE;
                remove_obj ( ch, obj->wear_loc, TRUE );
            }
        }

        if ( !found )
        {
            if ( arg[3] == '\0' )
                act ( "You are not wearing anything.", ch, NULL, arg,
                      TO_CHAR );
            else
                act ( "You are not wearing any $T.", ch, NULL, &arg[4],
                      TO_CHAR );
        }
    }

    return;
}

CH_CMD ( do_sacrifice )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int silver;

    /* variables for AUTOSPLIT */
    CHAR_DATA *gch;
    int members;
    char buffer[100];

    one_argument ( argument, arg );

    if ( arg[0] == '\0' || !str_cmp ( arg, ch->name ) )
    {
        act ( "$n offers $mself to $G, who graciously declines.", ch, NULL,
              NULL, TO_ROOM );
        act ( "$G appreciates your offer and may accept it later.", ch, NULL,
              NULL, TO_CHAR );
        return;
    }
    if ( !str_cmp ( "all", arg ) || !str_prefix ( "all.", arg ) )
    {
        OBJ_DATA *obj_next;
        bool found = FALSE;
        for ( obj = ch->in_room->contents; obj; obj = obj_next )
        {
            obj_next = obj->next_content;
            if ( arg[3] != '\0' && !is_name ( &arg[4], obj->name ) )
                continue;
            if ( ( !CAN_WEAR ( obj, ITEM_TAKE ) ||
                   CAN_WEAR ( obj, ITEM_NO_SAC ) ) ||
                 ( obj->item_type == ITEM_CORPSE_PC && obj->contains ) )
                continue;
            silver = UMAX ( 1, obj->level * 3 );
            if ( obj->item_type != ITEM_CORPSE_NPC &&
                 obj->item_type != ITEM_CORPSE_PC )
                silver = UMIN ( silver, obj->cost );
            found = TRUE;

            printf_to_char ( ch,
                             "{cThe great gods give you {Y%d{c silver for your sacrifice of {w%s{c.{x\n\r",
                             silver, obj->short_descr );
            act ( "$n sacrifices $p to the gods.", ch, obj, NULL, TO_ROOM ); // Previously Sina
            ch->silver += silver;
            extract_obj ( obj );
            if ( IS_SET ( ch->act, PLR_AUTOSPLIT ) )
            {
                members = 0;
                for ( gch = ch->in_room->people; gch;
                      gch = gch->next_in_room )
                    if ( is_same_group ( ch, gch ) )
                        members++;
                if ( members > 1 && silver > 1 )
                {
                    sprintf ( buf, "%d", silver );
                }
            }
        }
        if ( found )
            wiznet
                ( "$N sends up everything in the room as a burnt offering.",
                  ch, obj, WIZ_SECURE, 0, 0 );

        else

            send_to_char ( "There is nothing sacrificable in this room.\n\r",
                           ch );

        return;

    }

    obj = get_obj_list ( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
        send_to_char ( "You can't find it.\n\r", ch );
        return;
    }

    if ( obj->item_type == ITEM_CORPSE_PC )
    {
        if ( obj->contains )
        {
            act ( "$G wouldn't like that.", ch, NULL, NULL, TO_CHAR );
            return;
        }
    }

    if ( !CAN_WEAR ( obj, ITEM_TAKE ) || CAN_WEAR ( obj, ITEM_NO_SAC ) )
    {
        act ( "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR );
        return;
    }

    silver = UMAX ( 1, obj->level * 3 );

    if ( obj->item_type != ITEM_CORPSE_NPC &&
         obj->item_type != ITEM_CORPSE_PC )
        silver = UMIN ( silver, obj->cost );

    if ( silver == 1 )
    {
        act ( "$G gives you one silver coin for your sacrifice.", ch, NULL,
              NULL, TO_CHAR );
    }
    else
    {
        sprintf ( buf, "$G gives you {g%d{x silver coins for your sacrifice.",
                  silver );
        act ( buf, ch, NULL, NULL, TO_CHAR );
    }

    add_cost ( ch, silver, VALUE_SILVER );

    if ( IS_SET ( ch->act, PLR_AUTOSPLIT ) )
    {                           /* AUTOSPLIT code */
        members = 0;
        for ( gch = ch->in_room->people; gch != NULL;
              gch = gch->next_in_room )
        {
            if ( is_same_group ( gch, ch ) )
                members++;
        }

        if ( members > 1 && silver > 1 )
        {
            sprintf ( buffer, "%d", silver );
            do_split ( ch, buffer );
        }
    }

    act ( "$n sacrifices $p to $G.", ch, obj, NULL, TO_ROOM );
    wiznet ( "$N sends up $p as a burnt offering.", ch, obj, WIZ_SACCING, 0,
             0 );
    extract_obj ( obj );
    return;
}

CH_CMD ( do_quaff )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Quaff what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
    {
        send_to_char ( "You do not have that potion.\n\r", ch );
        return;
    }

    if ( obj->item_type != ITEM_POTION )
    {
        send_to_char ( "You can quaff only potions.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    
    if ( IS_SET(ch->in_room->room_flags,ROOM_ARENA) )
    {
        send_to_char ( "I think you should worry about fighting rather than drinking now.\n\r",
                       ch );
        return;
    }


    if ( ch->level < obj->level )
    {
        send_to_char ( "This liquid is too powerful for you to drink.\n\r",
                       ch );
        return;
    }

    WAIT_STATE ( ch, PULSE_VIOLENCE );

    if ( ch->position == POS_FIGHTING && chance ( 20 ) )
    {
        act ( "You fumble and drop the potion and it shatters into pieces.", ch, obj, NULL, TO_CHAR );
        act ( "$n fumbles and drops a potion they were trying to drink.", ch, obj, NULL, TO_ROOM );
	extract_obj ( obj );
	return;
    }

    act ( "$n quaffs $p.", ch, obj, NULL, TO_ROOM );
    act ( "You quaff $p.", ch, obj, NULL, TO_CHAR );

    obj_cast_spell ( obj->value[1], obj->value[0], ch, ch, NULL );
    obj_cast_spell ( obj->value[2], obj->value[0], ch, ch, NULL );
    obj_cast_spell ( obj->value[3], obj->value[0], ch, ch, NULL );
    obj_cast_spell ( obj->value[4], obj->value[0], ch, ch, NULL );

    extract_obj ( obj );
    return;
}

CH_CMD ( do_recite )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( ( scroll = get_obj_carry ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "You do not have that scroll.\n\r", ch );
        return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
        send_to_char ( "You can recite only scrolls.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( ch->level < scroll->level )
    {
        send_to_char
            ( "This scroll is too complex for you to comprehend.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, 5 * PULSE_VIOLENCE );

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
        victim = ch;
    }
    else
    {
        if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL &&
             ( obj = get_obj_here ( ch, arg2 ) ) == NULL )
        {
            send_to_char ( "You can't find it.\n\r", ch );
            return;
        }
    }

    act ( "$n recites $p.", ch, scroll, NULL, TO_ROOM );
    act ( "You recite $p.", ch, scroll, NULL, TO_CHAR );

    if ( number_percent (  ) >= 20 + get_skill ( ch, gsn_scrolls ) * 4 / 5 )
    {
        send_to_char ( "You mispronounce a syllable.\n\r", ch );
        check_improve ( ch, gsn_scrolls, FALSE, 2 );
    }

    else
    {
        obj_cast_spell ( scroll->value[1], scroll->value[0], ch, victim,
                         obj );
        obj_cast_spell ( scroll->value[2], scroll->value[0], ch, victim,
                         obj );
        obj_cast_spell ( scroll->value[3], scroll->value[0], ch, victim,
                         obj );
        obj_cast_spell ( scroll->value[4], scroll->value[0], ch, victim,
                         obj );
        check_improve ( ch, gsn_scrolls, TRUE, 2 );
    }

    extract_obj ( scroll );
    return;
}

CH_CMD ( do_brandish )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *staff;
    int sn;

    if ( ( staff = get_eq_char ( ch, WEAR_HOLD ) ) == NULL )
    {
        send_to_char ( "You hold nothing in your hand.\n\r", ch );
        return;
    }

    if ( staff->item_type != ITEM_STAFF )
    {
        send_to_char ( "You can brandish only with a staff.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( ( sn = staff->value[3] ) < 0 || sn >= MAX_SKILL ||
         skill_table[sn].spell_fun == 0 )
    {
        bug ( "Do_brandish: bad sn %d.", sn );
        return;
    }

    WAIT_STATE ( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[2] > 0 )
    {
        act ( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
        act ( "You brandish $p.", ch, staff, NULL, TO_CHAR );
        if ( ch->level < staff->level ||
             number_percent (  ) >= 20 + get_skill ( ch,
                                                     gsn_staves ) * 4 / 5 )
        {
            act ( "You fail to invoke $p.", ch, staff, NULL, TO_CHAR );
            act ( "...and nothing happens.", ch, NULL, NULL, TO_ROOM );
            check_improve ( ch, gsn_staves, FALSE, 2 );
        }

        else
            for ( vch = ch->in_room->people; vch; vch = vch_next )
            {
                vch_next = vch->next_in_room;

                switch ( skill_table[sn].target )
                {
                    default:
                        bug ( "Do_brandish: bad target for sn %d.", sn );
                        return;

                    case TAR_IGNORE:
                        if ( vch != ch )
                            continue;
                        break;

                    case TAR_CHAR_OFFENSIVE:
                        if ( IS_NPC ( ch ) ? IS_NPC ( vch ) :
                             !IS_NPC ( vch ) )
                            continue;
                        break;

                    case TAR_CHAR_DEFENSIVE:
                        if ( IS_NPC ( ch ) ? !IS_NPC ( vch ) :
                             IS_NPC ( vch ) )
                            continue;
                        break;

                    case TAR_CHAR_SELF:
                        if ( vch != ch )
                            continue;
                        break;
                }

                obj_cast_spell ( staff->value[3], staff->value[0], ch, vch,
                                 NULL );
                check_improve ( ch, gsn_staves, TRUE, 2 );
            }
    }

    if ( --staff->value[2] <= 0 )
    {
        act ( "$n's $p blazes bright and is gone.", ch, staff, NULL,
              TO_ROOM );
        act ( "Your $p blazes bright and is gone.", ch, staff, NULL,
              TO_CHAR );
        extract_obj ( staff );
    }

    return;
}

CH_CMD ( do_zap )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wand;
    OBJ_DATA *obj;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
        send_to_char ( "Zap whom or what?\n\r", ch );
        return;
    }

    if ( ( wand = get_eq_char ( ch, WEAR_HOLD ) ) == NULL )
    {
        send_to_char ( "You hold nothing in your hand.\n\r", ch );
        return;
    }

    if ( wand->item_type != ITEM_WAND )
    {
        send_to_char ( "You can zap only with a wand.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    obj = NULL;
    if ( arg[0] == '\0' )
    {
        if ( ch->fighting != NULL )
        {
            victim = ch->fighting;
        }
        else
        {
            send_to_char ( "Zap whom or what?\n\r", ch );
            return;
        }
    }
    else
    {
        if ( ( victim = get_char_room ( ch, arg ) ) == NULL &&
             ( obj = get_obj_here ( ch, arg ) ) == NULL )
        {
            send_to_char ( "You can't find it.\n\r", ch );
            return;
        }
    }

    WAIT_STATE ( ch, 2 * PULSE_VIOLENCE );

    if ( wand->value[2] > 0 )
    {
        if ( victim != NULL )
        {
            act ( "$n zaps $N with $p.", ch, wand, victim, TO_ROOM );
            act ( "You zap $N with $p.", ch, wand, victim, TO_CHAR );
        }
        else
        {
            act ( "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
            act ( "You zap $P with $p.", ch, wand, obj, TO_CHAR );
        }

        if ( ch->level < wand->level ||
             number_percent (  ) >= 20 + get_skill ( ch, gsn_wands ) * 4 / 5 )
        {
            act ( "Your efforts with $p produce only smoke and sparks.", ch,
                  wand, NULL, TO_CHAR );
            act ( "$n's efforts with $p produce only smoke and sparks.", ch,
                  wand, NULL, TO_ROOM );
            check_improve ( ch, gsn_wands, FALSE, 2 );
        }
        else
        {
            obj_cast_spell ( wand->value[3], wand->value[0], ch, victim,
                             obj );
            check_improve ( ch, gsn_wands, TRUE, 2 );
        }
    }

    if ( --wand->value[2] <= 0 )
    {
        act ( "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
        act ( "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
        extract_obj ( wand );
    }

    return;
}

CH_CMD ( do_itake )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Take what from whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg2 ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry ( victim, arg1 ) ) == NULL )
    {
        send_to_char ( "You can't find it.\n\r", ch );
        return;
    }

    obj_from_char ( obj );
    obj_to_char ( obj, ch );
    send_to_char ( "Item Taken From Character!\n\r", ch );
    return;
}

CH_CMD ( do_steal )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Steal what from whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "That's pointless.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->position == POS_FIGHTING )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r"
                       "You'd better not -- you might get hit.\n\r", ch );
        return;
    }

    if ( !IS_NPC ( victim ) && victim->clan == 5 )
    {
    	send_to_char ( "You can't steal from Pknewbies!\n\r", ch );
	return;
    }

    WAIT_STATE ( ch, skill_table[gsn_steal].beats );
    percent = number_percent (  );
    if ( get_skill ( ch, gsn_steal ) >= 1 )
        percent += ( IS_AWAKE ( victim ) ? 10 : -50 );

    if ( ( ( ch->level + 7 < victim->level || ch->level - 7 > victim->level )
           && !IS_NPC ( victim ) && !IS_NPC ( ch ) ) || ( !IS_NPC ( ch ) &&
                                                          percent >
                                                          get_skill ( ch,
                                                                      gsn_steal ) )
         || ( !IS_NPC ( ch ) && !is_clan ( ch ) ) )
    {
        /*
         * Failure.
         */
        send_to_char ( "Oops.\n\r", ch );
        act ( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT );
        act ( "$n tried to steal from $N.\n\r", ch, NULL, victim,
              TO_NOTVICT );
        switch ( number_range ( 0, 3 ) )
        {
            case 0:
                sprintf ( buf, "{z{R%s{x{R is a lousy thief!{x", ch->name );
                break;
            case 1:
                sprintf ( buf,
                          "{z{R%s{x{R couldn't rob %s way out of a paper bag!{x",
                          ch->name, ( ch->sex == 2 ) ? "her" : "his" );
                break;
            case 2:
                sprintf ( buf, "{z{R%s{x{R tried to rob me!{x", ch->name );
                break;
            case 3:
                sprintf ( buf, "{RKeep your hands out of there, {z%s{x{R!{x",
                          ch->name );
                break;
        }
        do_yell ( victim, buf );
        if ( !IS_NPC ( ch ) )
        {
            if ( IS_NPC ( victim ) )
            {
                check_improve ( ch, gsn_steal, FALSE, 2 );
                multi_hit ( victim, ch, TYPE_UNDEFINED );
            }
            else
            {
                sprintf ( buf, "{R$N{x tried to steal from {B%s{x.",
                          victim->name );
                wiznet ( buf, ch, NULL, WIZ_FLAGS, 0, 0 );
            }
        }

        return;
    }

    if ( !str_cmp ( arg1, "coin" ) || !str_cmp ( arg1, "coins" ) ||
         !str_cmp ( arg1, "gold" ) || !str_cmp ( arg1, "silver" ) )
    {
        int gold, silver;

        gold = victim->gold * number_range ( 1, ch->level ) / 60;
        silver = victim->silver * number_range ( 1, ch->level ) / 60;
        if ( gold <= 0 && silver <= 0 )
        {
            send_to_char ( "You couldn't get any coins.\n\r", ch );
            return;
        }

        ch->gold += gold;
        ch->silver += silver;
        victim->silver -= silver;
        victim->gold -= gold;
        if ( silver <= 0 )
            sprintf ( buf, "Bingo!  You got {g%d{x gold coins.\n\r", gold );
        else if ( gold <= 0 )
            sprintf ( buf, "Bingo!  You got {g%d{x silver coins.\n\r",
                      silver );
        else
            sprintf ( buf,
                      "Bingo!  You got {g%d{x silver and {g%d{x gold coins.\n\r",
                      silver, gold );

        send_to_char ( buf, ch );
        check_improve ( ch, gsn_steal, TRUE, 2 );
        return;
    }

    if ( ( obj = get_obj_carry ( victim, arg1 ) ) == NULL )
    {
        send_to_char ( "You can't find it.\n\r", ch );
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_NOSTEAL ) )
    {
        send_to_char ( "A magical power protects that item.\n\r", ch );
	return;
    }
    
    if ( !can_drop_obj ( ch, obj ) ||
         IS_SET ( obj->extra_flags, ITEM_INVENTORY ) ||
         obj->level > ch->level )
    {
        send_to_char ( "You can't pry it away.\n\r", ch );
        return;
    }

    if ( ch->carry_number + get_obj_number ( obj ) > can_carry_n ( ch ) )
    {
        send_to_char ( "You have your hands full.\n\r", ch );
        return;
    }

    if ( ch->carry_weight + get_obj_weight ( obj ) > can_carry_w ( ch ) )
    {
        send_to_char ( "You can't carry that much weight.\n\r", ch );
        return;
    }

    obj_from_char ( obj );
    obj_to_char ( obj, ch );
    check_improve ( ch, gsn_steal, TRUE, 2 );
    send_to_char ( "Got it!\n\r", ch );
    return;
}

/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper ( CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;

    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper;
          keeper = keeper->next_in_room )
    {
        if ( IS_NPC ( keeper ) &&
             ( pShop = keeper->pIndexData->pShop ) != NULL )
            break;
    }

    if ( pShop == NULL )
    {
        send_to_char ( "You can't do that here.\n\r", ch );
        return NULL;
    }

    /*
     * Undesirables.
     */
    if ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_TWIT ) )
    {
        do_say ( keeper, "{aTwits are not welcome!{x" );
        sprintf ( buf, "{a%s the {z{RTWIT{x is over here!{x\n\r", ch->name );
        do_yell ( keeper, buf );
        return NULL;
    }
    /*
     * Shop hours.
     */
    if ( time_info.hour < pShop->open_hour )
    {
        do_say ( keeper, "{aSorry, I am closed. Come back later.{x" );
        return NULL;
    }

    if ( time_info.hour > pShop->close_hour )
    {
        do_say ( keeper, "{aSorry, I am closed. Come back tomorrow.{x" );
        return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see ( keeper, ch ) )
    {
        do_say ( keeper, "{aI don't trade with folks I can't see.{x" );
        return NULL;
    }

    return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper ( OBJ_DATA * obj, CHAR_DATA * ch )
{
    OBJ_DATA *t_obj, *t_obj_next;

    /* see if any duplicates are found */
    for ( t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next )
    {
        t_obj_next = t_obj->next_content;

        if ( obj->pIndexData == t_obj->pIndexData &&
             !str_cmp ( obj->short_descr, t_obj->short_descr ) )
        {
            /* if this is an unlimited item, destroy the new one */
            if ( IS_OBJ_STAT ( t_obj, ITEM_INVENTORY ) )
            {
                extract_obj ( obj );
                return;
            }
            obj->cost = t_obj->cost;    /* keep it standard */
            break;
        }
    }

    if ( t_obj == NULL )
    {
        obj->next_content = ch->carrying;
        ch->carrying = obj;
    }
    else
    {
        obj->next_content = t_obj->next_content;
        t_obj->next_content = obj;
    }

    obj->carried_by = ch;
    obj->in_room = NULL;
    obj->in_obj = NULL;
    ch->carry_number += get_obj_number ( obj );
    ch->carry_weight += get_obj_weight ( obj );
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper ( CHAR_DATA * ch, CHAR_DATA * keeper,
                           char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument ( argument, arg );
    count = 0;
    for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( obj->wear_loc == WEAR_NONE && can_see_obj ( keeper, obj ) &&
             can_see_obj ( ch, obj ) && is_name ( arg, obj->name ) )
        {
            if ( ++count == number )
                return obj;

            /* skip other objects of the same name */
            while ( obj->next_content != NULL &&
                    obj->pIndexData == obj->next_content->pIndexData &&
                    !str_cmp ( obj->short_descr,
                               obj->next_content->short_descr ) )
                obj = obj->next_content;
        }
    }

    return NULL;
}

int get_cost ( CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int cost;

    if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
        return 0;

    if ( fBuy )
    {
        cost = obj->cost * pShop->profit_buy / 100;
    }
    else
    {
        OBJ_DATA *obj2;
        int itype;

        cost = 0;
        for ( itype = 0; itype < MAX_TRADE; itype++ )
        {
            if ( obj->item_type == pShop->buy_type[itype] )
            {
                cost = obj->cost * pShop->profit_sell / 100;
                break;
            }
        }

        if ( !IS_OBJ_STAT ( obj, ITEM_SELL_EXTRACT ) )
            for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
            {
                if ( obj->pIndexData == obj2->pIndexData &&
                     !str_cmp ( obj->short_descr, obj2->short_descr ) )
                {
                    if ( IS_OBJ_STAT ( obj2, ITEM_INVENTORY ) )
                        cost /= 2;
                    else
                        cost = cost * 3 / 4;
                }
            }
    }

    if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
    {
        if ( obj->value[1] == 0 )
            cost /= 4;
        else
            cost = cost * obj->value[2] / obj->value[1];
    }

    return cost;
}

CH_CMD ( do_buy )
{
    char buf[MAX_STRING_LENGTH];
    int cost, roll;
    long multicost;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "Buy what?\n\r", ch );
        return;
    }

    smash_tilde ( argument );

    if ( IS_SET ( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *pet;
        ROOM_INDEX_DATA *pRoomIndexNext;
        ROOM_INDEX_DATA *in_room;

        if ( IS_NPC ( ch ) )
            return;

        argument = one_argument ( argument, arg );

        /* hack to make new thalos pets work */
        if ( ch->in_room->vnum == 9621 )
            pRoomIndexNext = get_room_index ( 9706 );
        else
            pRoomIndexNext = get_room_index ( ch->in_room->vnum + 1 );
        if ( pRoomIndexNext == NULL )
        {
            bug ( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
            send_to_char ( "Sorry, you can't buy that here.\n\r", ch );
            return;
        }

        in_room = ch->in_room;
        ch->in_room = pRoomIndexNext;
        pet = get_char_room ( ch, arg );
        ch->in_room = in_room;

        if ( pet == NULL || !IS_SET ( pet->act, ACT_PET ) )
        {
            send_to_char ( "Sorry, you can't buy that here.\n\r", ch );
            return;
        }

        if ( ch->pet != NULL )
        {
            send_to_char ( "You already own a pet.\n\r", ch );
            return;
        }

        cost = 10 * pet->level * pet->level;

        if ( ( ch->silver + ( 100 * ch->gold ) + ( 10000 * ch->platinum ) ) <
             cost )
        {
            send_to_char ( "You can't afford it.\n\r", ch );
            return;
        }

        if ( ch->level < pet->level )
        {
            send_to_char
                ( "You're not powerful enough to master this pet.\n\r", ch );
            return;
        }

        /* haggle */
        roll = number_percent (  );
        if ( roll < get_skill ( ch, gsn_haggle ) )
        {
            cost -= cost / 2 * roll / 100;
            sprintf ( buf, "You haggle the price down to {g%d{x coins.\n\r",
                      cost );
            send_to_char ( buf, ch );
            check_improve ( ch, gsn_haggle, TRUE, 4 );

        }

        deduct_cost ( ch, cost, VALUE_SILVER );
        pet = create_mobile ( pet->pIndexData );
        SET_BIT ( pet->act, ACT_PET );
        SET_BIT ( pet->affected_by, AFF_CHARM );
        pet->comm = COMM_NOTELL | COMM_NOSHOUT | COMM_NOCHANNELS;

        argument = one_argument ( argument, arg );
        if ( arg[0] != '\0' )
        {
            sprintf ( buf, "%s %s", pet->name, arg );
            free_string ( pet->name );
            pet->name = str_dup ( buf );
        }

        sprintf ( buf, "%sA neck tag says '{cI belong to %s{x'.\n\r",
                  pet->description, ch->name );
        free_string ( pet->description );
        pet->description = str_dup ( buf );

        char_to_room ( pet, ch->in_room );
        add_follower ( pet, ch );
        pet->leader = ch;
        ch->pet = pet;
        pet->alignment = ch->alignment;
        send_to_char ( "Enjoy your pet.\n\r", ch );
        act ( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
        return;
    }
    else
    {
        CHAR_DATA *keeper;
        OBJ_DATA *obj, *t_obj;
        char arg[MAX_INPUT_LENGTH];
        int number, count = 1;

        if ( ( keeper = find_keeper ( ch ) ) == NULL )
            return;

        number = mult_argument ( argument, arg );
        obj = get_obj_keeper ( ch, keeper, arg );
        cost = get_cost ( keeper, obj, TRUE );

        if ( cost <= 0 || !can_see_obj ( ch, obj ) )
        {
            act ( "$n tells you '{aI don't sell that -- try '{Mlist{a'{x'.",
                  keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return;
        }

        if ( number < 0 || number > 100 )
        {
            act ( "$n tells you '{aNice try, jackass!{x'.", keeper, NULL, ch,
                  TO_VICT );
            ch->reply = keeper;
            multi_hit ( keeper, ch, TYPE_UNDEFINED );
            return;
        }
        if ( number == 0 )
            number = 1;

        if ( !IS_OBJ_STAT ( obj, ITEM_INVENTORY ) )
        {
            for ( t_obj = obj->next_content; count < number && t_obj != NULL;
                  t_obj = t_obj->next_content )
            {
                if ( t_obj->pIndexData == obj->pIndexData &&
                     !str_cmp ( t_obj->short_descr, obj->short_descr ) )
                    count++;
                else
                    break;
            }

            if ( count < number )
            {
                act ( "$n tells you '{aI don't have that many in stock{x'.",
                      keeper, NULL, ch, TO_VICT );
                ch->reply = keeper;
                return;
            }
        }

        if ( ( ch->silver + ( ch->gold * 100 ) + ( ch->platinum * 10000 ) ) <
             cost * number )
        {
            if ( number > 1 )
                act ( "$n tells you '{aYou can't afford to buy that many{x'.",
                      keeper, obj, ch, TO_VICT );
            else
                act ( "$n tells you '{aYou can't afford to buy $p{x'.",
                      keeper, obj, ch, TO_VICT );
            ch->reply = keeper;
            return;
        }

        if ( ( ( obj->level > ch->level ) && ( ch->class < MAX_CLASS / 2 ) &&
               ( obj->level > 19 ) ) || ( ( obj->level > ch->level ) &&
                                          ( ch->class >= MAX_CLASS / 2 ) &&
                                          ( obj->level > 27 ) ) )
        {
            act ( "$n tells you '{aYou can't use $p {ayet{x'.", keeper, obj,
                  ch, TO_VICT );
            ch->reply = keeper;
            return;
        }

        if ( ch->carry_number + number * get_obj_number ( obj ) >
             can_carry_n ( ch ) )
        {
            send_to_char ( "You can't carry that many items.\n\r", ch );
            return;
        }

        if ( ch->carry_weight + number * get_obj_weight ( obj ) >
             can_carry_w ( ch ) )
        {
            send_to_char ( "You can't carry that much weight.\n\r", ch );
            return;
        }

        /* haggle */
        roll = number_percent (  );
        if ( !IS_OBJ_STAT ( obj, ITEM_SELL_EXTRACT ) &&
             roll < get_skill ( ch, gsn_haggle ) )
        {
            cost -= obj->cost / 2 * roll / 100;
            act ( "You haggle with $N.", ch, NULL, keeper, TO_CHAR );
            check_improve ( ch, gsn_haggle, TRUE, 4 );
        }

        if ( number > 1 )
        {
            sprintf ( buf, "$n buys $p[%d].", number );
            act ( buf, ch, obj, NULL, TO_ROOM );
            sprintf ( buf, "You buy $p[%d] for {g%d{x silver.", number,
                      cost * number );
            act ( buf, ch, obj, NULL, TO_CHAR );
        }
        else
        {
            act ( "$n buys $p.", ch, obj, NULL, TO_ROOM );
            sprintf ( buf, "You buy $p for {g%d{x silver.", cost );
            act ( buf, ch, obj, NULL, TO_CHAR );
        }
        multicost = cost * number;
        while ( multicost >= 100000 )
        {
            deduct_cost ( ch, 10, VALUE_PLATINUM );
            add_cost ( keeper, 10, VALUE_PLATINUM );
            multicost -= 100000;
        }
        while ( multicost >= 10000 )
        {
            deduct_cost ( ch, 1, VALUE_PLATINUM );
            add_cost ( keeper, 1, VALUE_PLATINUM );
            multicost -= 10000;
        }
        while ( multicost >= 1000 )
        {
            deduct_cost ( ch, 10, VALUE_GOLD );
            add_cost ( keeper, 10, VALUE_GOLD );
            multicost -= 1000;
        }
        while ( multicost >= 100 )
        {
            deduct_cost ( ch, 1, VALUE_GOLD );
            add_cost ( keeper, 1, VALUE_GOLD );
            multicost -= 100;
        }
        if ( multicost > 0 )
        {
            roll = multicost;
            deduct_cost ( ch, roll, VALUE_SILVER );
            add_cost ( keeper, roll, VALUE_SILVER );
        }

        for ( count = 0; count < number; count++ )
        {
            if ( IS_SET ( obj->extra_flags, ITEM_INVENTORY ) )
                t_obj = create_object ( obj->pIndexData, obj->level );
            else
            {
                t_obj = obj;
                obj = obj->next_content;
                obj_from_char ( t_obj );
            }

            if ( t_obj->timer > 0 && !IS_OBJ_STAT ( t_obj, ITEM_HAD_TIMER ) )
                t_obj->timer = 0;
            REMOVE_BIT ( t_obj->extra_flags, ITEM_HAD_TIMER );
            obj_to_char ( t_obj, ch );
            if ( cost < t_obj->cost )
                t_obj->cost = cost;
        }
    }
}

CH_CMD ( do_list )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_SET ( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
        ROOM_INDEX_DATA *pRoomIndexNext;
        CHAR_DATA *pet;
        bool found;

        /* hack to make new thalos pets work */
        if ( ch->in_room->vnum == 9621 )
            pRoomIndexNext = get_room_index ( 9706 );
        else
            pRoomIndexNext = get_room_index ( ch->in_room->vnum + 1 );

        if ( pRoomIndexNext == NULL )
        {
            bug ( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
            send_to_char ( "You can't do that here.\n\r", ch );
            return;
        }

        found = FALSE;
        for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
        {
            if ( IS_SET ( pet->act, ACT_PET ) )
            {
                if ( !found )
                {
                    found = TRUE;
                    send_to_char ( "Pets for sale:\n\r", ch );
                }
                sprintf ( buf, "{g[{G%-3d{g]{W %8d{x - %s\n\r", pet->level,
                          10 * pet->level * pet->level, pet->short_descr );
                send_to_char ( buf, ch );
            }
        }
        if ( !found )
            send_to_char ( "Sorry, we're out of pets right now.\n\r", ch );
        return;
    }
    else
    {
        CHAR_DATA *keeper;
        OBJ_DATA *obj;
        int cost, count;
        bool found;
        char arg[MAX_INPUT_LENGTH];

        if ( ( keeper = find_keeper ( ch ) ) == NULL )
            return;
        one_argument ( argument, arg );

        found = FALSE;
        for ( obj = keeper->carrying; obj; obj = obj->next_content )
        {
            if ( obj->wear_loc == WEAR_NONE && can_see_obj ( ch, obj ) &&
                 ( cost = get_cost ( keeper, obj, TRUE ) ) > 0 &&
                 ( arg[0] == '\0' || is_name ( arg, obj->name ) ) )
            {
                if ( !found )
                {
                    found = TRUE;
                    send_to_char ( "{g[{GLvl  {WPrice {yQty{g]{x Item\n\r",
                                   ch );
                }

                if ( IS_OBJ_STAT ( obj, ITEM_INVENTORY ) )
                    sprintf ( buf, "{g[{G%3d {W%6d{y -- {g]{x %s\n\r",
                              obj->level, cost, obj->short_descr );
                else
                {
                    count = 1;

                    while ( obj->next_content != NULL &&
                            obj->pIndexData == obj->next_content->pIndexData
                            && !str_cmp ( obj->short_descr,
                                          obj->next_content->short_descr ) )
                    {
                        obj = obj->next_content;
                        count++;
                    }
                    sprintf ( buf, "{g[{G%3d{W %6d{y %2d {g]{x %s\n\r",
                              obj->level, cost, count, obj->short_descr );
                }
                send_to_char ( buf, ch );
            }
        }

        if ( !found )
            send_to_char ( "You can't buy anything here.\n\r", ch );
        return;
    }
}

CH_CMD ( do_sell )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost, roll;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Sell what?\n\r", ch );
        return;
    }

    if ( ( keeper = find_keeper ( ch ) ) == NULL )
        return;

    if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
    {
        act ( "$n tells you '{aYou don't have that item{x'.", keeper, NULL,
              ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) )
    {
        send_to_char ( "You should sell that to the questor instead!\n\r",
                       ch );
        return;
    }

    if ( !can_drop_obj ( ch, obj ) )
    {
        send_to_char ( "{RYou can't let go of it{z!!{x\n\r", ch );
        return;
    }

    if ( !can_see_obj ( keeper, obj ) )
    {
        act ( "$n doesn't see what you are offering.", keeper, NULL, ch,
              TO_VICT );
        return;
    }

    if ( ( cost = get_cost ( keeper, obj, FALSE ) ) <= 0 )
    {
        act ( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
        return;
    }
    if ( cost >
         ( keeper->silver + ( 100 * keeper->gold ) +
           ( 10000 * keeper->platinum ) ) )
    {
        act ( "$n tells you '{aI'm afraid I don't have enough wealth to buy $p{x'.", keeper, obj, ch, TO_VICT );
        return;
    }

    act ( "$n sells $p.", ch, obj, NULL, TO_ROOM );
    /* haggle */
    roll = number_percent (  );
    if ( !IS_OBJ_STAT ( obj, ITEM_SELL_EXTRACT ) &&
         roll < get_skill ( ch, gsn_haggle ) )
    {
        send_to_char ( "You haggle with the shopkeeper.\n\r", ch );
        cost += obj->cost / 2 * roll / 100;
        cost = UMIN ( cost, 95 * get_cost ( keeper, obj, TRUE ) / 100 );
        cost =
            UMIN ( cost,
                   ( keeper->silver + ( 100 * keeper->gold ) +
                     ( 10000 * keeper->platinum ) ) );
        check_improve ( ch, gsn_haggle, TRUE, 4 );
    }
    sprintf ( buf, "You sell $p for {g%d{x silver piece%s.", cost,
              cost == 1 ? "" : "s" );
    act ( buf, ch, obj, NULL, TO_CHAR );

    while ( cost >= 10000 )
    {
        deduct_cost ( keeper, 1, VALUE_PLATINUM );
        add_cost ( ch, 1, VALUE_PLATINUM );
        cost -= 10000;
    }
    while ( cost >= 1000 )
    {
        deduct_cost ( keeper, 10, VALUE_GOLD );
        add_cost ( ch, 10, VALUE_GOLD );
        cost -= 1000;
    }
    while ( cost >= 100 )
    {
        deduct_cost ( keeper, 1, VALUE_GOLD );
        add_cost ( ch, 1, VALUE_GOLD );
        cost -= 100;
    }
    if ( cost > 0 )
    {
        deduct_cost ( keeper, cost, VALUE_SILVER );
        add_cost ( ch, cost, VALUE_SILVER );
    }

    if ( obj->item_type == ITEM_TRASH ||
         IS_OBJ_STAT ( obj, ITEM_SELL_EXTRACT ) )
    {
        extract_obj ( obj );
    }
    else
    {
        obj_from_char ( obj );
        if ( obj->timer )
            SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
        else
            obj->timer = number_range ( 50, 100 );
        obj_to_keeper ( obj, keeper );
    }

    return;
}

CH_CMD ( do_value )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Value what?\n\r", ch );
        return;
    }

    if ( ( keeper = find_keeper ( ch ) ) == NULL )
        return;

    if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
    {
        act ( "$n tells you '{aYou don't have that item{x'.", keeper, NULL,
              ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    if ( !can_see_obj ( keeper, obj ) )
    {
        act ( "$n doesn't see what you are offering.", keeper, NULL, ch,
              TO_VICT );
        return;
    }

    if ( !can_drop_obj ( ch, obj ) )
    {
        send_to_char ( "You can't let go of it.\n\r", ch );
        return;
    }

    if ( ( cost = get_cost ( keeper, obj, FALSE ) ) <= 0 )
    {
        act ( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
        return;
    }

    sprintf ( buf,
              "$n tells you '{aI'll give you {g%d{a silver coin%s for $p{x'.",
              cost, cost == 1 ? "" : "s" );
    act ( buf, keeper, obj, ch, TO_VICT );
    ch->reply = keeper;

    return;
}

CH_CMD ( do_second )
/* wear object as a secondary weapon */
{
    OBJ_DATA *shieldobj;
    OBJ_DATA *obj;
    int skill, sn;
    char buf[MAX_STRING_LENGTH];    /* overkill, but what the heck */
    bool levitate;

    levitate = ( ( get_skill ( ch, gsn_shield_levitation ) != 0 ) &&
                 ( skill_table[gsn_shield_levitation].
                   skill_level[ch->class] <= ch->level ) );

    if ( argument[0] == '\0' )  /* empty */
    {
        send_to_char ( "Wear which weapon in your off-hand?\n\r", ch );
        return;
    }

    obj = get_obj_carry ( ch, argument );   /* find the obj withing ch's inventory */

    if ( obj == NULL )
    {
        send_to_char ( "You have no such thing in your backpack.\n\r", ch );
        return;
    }
    if ( !CAN_WEAR ( obj, ITEM_WIELD ) )
    {
        send_to_char ( "You can't second that!\n\r", ch );
        return;
    }

    /* check if the char is using a shield or a held weapon */

    if ( get_eq_char ( ch, WEAR_SHIELD ) && !levitate )
    {
        send_to_char
            ( "You cannot use a secondary weapon while using a shield.\n\r",
              ch );
        return;
    }
    if ( get_eq_char ( ch, WEAR_HOLD ) )
    {
        send_to_char
            ( "You cannot use a secondary weapon while holding an item.\n\r",
              ch );
        return;
    }

    if ( ( ( ch->level < obj->level ) && ( ch->class < MAX_CLASS / 2 ) &&
           ( obj->level > 19 ) ) || ( ( ch->level < obj->level ) &&
                                      ( ch->class >= MAX_CLASS / 2 ) &&
                                      ( obj->level > 27 ) ) )
    {
        sprintf ( buf, "You must be level %d to use this object.\n\r",
                  obj->level );
        send_to_char ( buf, ch );
        act ( "$n tries to use $p, but is too inexperienced.", ch, obj, NULL,
              TO_ROOM );
        return;
    }

/* check that the character is using a first weapon at all */
    if ( get_eq_char ( ch, WEAR_WIELD ) == NULL )   /* oops - != here was a bit wrong :) */
    {
        send_to_char
            ( "You need to wield a primary weapon, before using a secondary one!\n\r",
              ch );
        return;
    }

    if ( IS_WEAPON_STAT ( get_eq_char ( ch, WEAR_WIELD ), WEAPON_TWO_HANDS ) )
    {
        send_to_char ( "Your primary weapon requires {z{Bboth{x hands!\n\r",
                       ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) )
    {
        send_to_char ( "This weapon requires {z{Bboth{x hands!\n\r", ch );
        return;
    }

/* check for str - secondary weapons have to be lighter */
    if ( get_obj_weight ( obj ) >
         ( str_app[get_curr_stat ( ch, STAT_STR )].wield * 8 ) )
    {
        send_to_char
            ( "This weapon is too heavy to be used as a secondary weapon by you.\n\r",
              ch );
        return;
    }

/* check if the secondary weapon is heavier than the primary weapon */
/*
    if ( ( get_obj_weight ( obj ) ) >
         get_obj_weight ( get_eq_char ( ch, WEAR_WIELD ) ) )
    {
        send_to_char
            ( "Your secondary weapon cannot be heavier than the primary one.\n\r",
              ch );
        return;
    }
*/
/* at last - the char uses the weapon */

    if ( !remove_obj ( ch, WEAR_SECONDARY, TRUE ) ) /* remove the current weapon if any */
        return;                 /* remove obj tells about any no_remove */

/* char CAN use the item! that didn't take long at aaall */

    if ( ( ( shieldobj = get_eq_char ( ch, WEAR_SHIELD ) ) != NULL ) &&
         levitate )
    {
        act ( "$n levitates $p in front of $m.", ch, shieldobj, NULL,
              TO_ROOM );
        act ( "You levitate $p in front of you.", ch, shieldobj, NULL,
              TO_CHAR );
    }
    act ( "$n wields $p in $s off-hand.", ch, obj, NULL, TO_ROOM );
    act ( "You wield $p in your off-hand.", ch, obj, NULL, TO_CHAR );
    equip_char ( ch, obj, WEAR_SECONDARY );

    sn = get_weapon_sn ( ch );

    if ( sn == gsn_hand_to_hand )
        return;

    skill =
        ( ( get_weapon_skill ( ch, sn ) * get_skill ( ch, gsn_dual_wield ) ) /
          100 );

    if ( skill >= 100 )
        act ( "$p feels like a part of you!", ch, obj, NULL, TO_CHAR );
    else if ( skill > 85 )
        act ( "You feel quite confident with $p.", ch, obj, NULL, TO_CHAR );
    else if ( skill > 70 )
        act ( "You are skilled with $p.", ch, obj, NULL, TO_CHAR );
    else if ( skill > 50 )
        act ( "Your skill with $p is adequate.", ch, obj, NULL, TO_CHAR );
    else if ( skill > 25 )
        act ( "$p feels a little clumsy in your hands.", ch, obj, NULL,
              TO_CHAR );
    else if ( skill > 1 )
        act ( "You fumble and almost drop $p.", ch, obj, NULL, TO_CHAR );
    else
        act ( "You don't even know which end is up on $p.", ch, obj, NULL,
              TO_CHAR );

    return;
}

bool can_quest ( CHAR_DATA * ch )
{
    OBJ_DATA *object;
    bool found;

    if ( ch->desc == NULL )
        return TRUE;

    if ( ch->level > HERO )
        return TRUE;

    /*
     * search the list of objects.
     */
    found = TRUE;
    for ( object = ch->carrying; object != NULL;
          object = object->next_content )
    {
        if ( IS_OBJ_STAT ( object, ITEM_QUEST ) )
            found = FALSE;
    }
    if ( found )
        return TRUE;

    return FALSE;
}

CH_CMD ( do_trash )
{
    OBJ_DATA *obj;

    if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
        send_to_char( "You can't let go of it.\n\r", ch );
        return;
    }

    obj_from_char( obj );
    act( "$n trashes $p.", ch, obj, NULL, TO_ROOM );
    act( "You trash $p.", ch, obj, NULL, TO_CHAR );
        extract_obj(obj);
}

CH_CMD ( do_clean )
{
    OBJ_DATA *obj;
	OBJ_DATA  *obj_next;

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next_content;

		if ( obj == NULL )
			return;
		else if ( obj->item_type == ITEM_CORPSE_PC && obj->contains)
			continue;
		else if ( !CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC))
			continue;
		else if ( !can_see_obj( ch, obj ) )
			continue;
		else extract_obj( obj );
	}
	act( "$n gets out a broom and cleans up the place.", ch, obj, NULL, TO_ROOM );
	wiznet("$N does a good deed trying to keep the dreamy realms clean.", ch, NULL, WIZ_SACCING,0,0);
	send_to_char( "You feel better about yourself as you clean up the place.\n\r", ch );
}

void do_shape( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
	    if ( obj->item_type == ITEM_WEAPON )
		break;
	}

	if ( obj == NULL )
	{
	    send_to_char( "Shape what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    switch ( obj->item_type )
    {
    default:
	send_to_char( "Only weapons can be shaped.\n\r", ch );
	return;

    case ITEM_WEAPON:
	if ( ch->qps < 50 )
	{
		send_to_char( "You need 50 immqp to shape a weapon\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "dagger"  ) )
	{
		obj->value[0] = WEAPON_DAGGER;
		sprintf(buf,"{wYour {x%s{w glows {Wbright white{w then morphs into a {Mdagger{w.{x\n\r",obj->short_descr);
		send_to_char(buf,ch);
		ch->qps -= 50;
		break;
	}
	else if ( !str_cmp( arg2, "m" ) || !str_cmp( arg2, "mace"  ) )
	{
		obj->value[0] = WEAPON_MACE;
		sprintf(buf,"{wYour {x%s{w glows {Wbright white{w then morphs into a {Mmace{w.{x\n\r",obj->short_descr);
		send_to_char(buf,ch);
    	        ch->qps -= 50;
		break;
	}
	else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "sword"  ) )
	{
		obj->value[0] = WEAPON_SWORD;
		sprintf(buf,"{wYour {x%s{w glows {Wbright white{w then morphs into a {Msword{w.{x\n\r",obj->short_descr);
		send_to_char(buf,ch);
    	        ch->qps -= 50;
		break;
	}
	else if ( !str_cmp( arg2, "sp" ) || !str_cmp( arg2, "spear"  ) )
	{
		obj->value[0] = WEAPON_SPEAR;
		sprintf(buf,"{wYour {x%s{w glows {Wbright white{w then morphs into a {Mspear{w.{x\n\r",obj->short_descr);
		send_to_char(buf,ch);
		break;
	}
	else if ( !str_cmp( arg2, "a" ) || !str_cmp( arg2, "axe"  ) )
	{
		obj->value[0] = WEAPON_AXE;
		sprintf(buf,"{wYour {x%s{w glows {Wbright white{w then morphs into a {Maxe{w.{x\n\r",obj->short_descr);
		send_to_char(buf,ch);
    	        ch->qps -= 50;
		break;
	}
	else if ( !str_cmp( arg2, "f" ) || !str_cmp( arg2, "flail"  ) )
	{
		obj->value[0] = WEAPON_FLAIL;
		sprintf(buf,"{wYour {x%s{w glows {Wbright white{w then morphs into a {Mflail{w.{x\n\r",obj->short_descr);
		send_to_char(buf,ch);
    	        ch->qps -= 50;
		break;
	}
	else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "whip"  ) )
	{
		obj->value[0] = WEAPON_WHIP;
		sprintf(buf,"{wYour {x%s{w glows {Wbright white{w then morphs into a {Mwhip{w.{x\n\r",obj->short_descr);
		send_to_char(buf,ch);
	        ch->qps -= 50;
  		break;
	}
	else if ( !str_cmp( arg2, "p" ) || !str_cmp( arg2, "polearm"  ) )
	{
		obj->value[0] = WEAPON_MACE;
		sprintf(buf,"{wYour {x%s{w glows {Wbright white{w then morphs into a {Mpolearm{w.{x\n\r",obj->short_descr);
		send_to_char(buf,ch);
    	        ch->qps -= 50;
		break;
	}
        sprintf(buf,"{wYour {x%s{w glows {Wbright white{w........{x\n\r",obj->short_descr);
        send_to_char(buf,ch);

        /* Modified by Vizzini... i don't think the short_descr was ever supposed to be in this string */
        /* sprintf(buf,"{wthen fizzles exposing no change.\n\r",obj->short_descr); */
        sprintf(buf,"{wthen fizzles, exposing no change.\n\r");
        send_to_char(buf,ch);
        send_to_char("{#[{3An object can be either sword, dagger, polearm, flail, axe, whip, spear or mace{#]{x\n\r",ch);
        break;
        }
}

CH_CMD ( do_butcher )
{
	 /* Butcher skill, created by Argawal                   */
	 /* Original Idea taken fom Carrion Fields Mud          */
	 /* If you have an interest in this skill, feel free    */
	 /* to use it in your mud if you so desire.             */
	 /* All I ask is that Argawal is credited with creating */
	 /* this skill, as I wrote it from scratch.             */

	 char buf[MAX_STRING_LENGTH];
	 char arg[MAX_STRING_LENGTH];
	 int numst = 0;
	 OBJ_DATA *steak;
	 OBJ_DATA *obj;

	 one_argument(argument, arg);
	 if(get_skill(ch,gsn_butcher)==0)
	 {
		 send_to_char("{wButchering is beyond your skills.{x\n\r",ch);
		 return;
	 }

	 if(arg[0]=='\0')
	 {
		 send_to_char("{wButcher what?{x\n\r",ch);
		 return;
	 }

	 obj = get_obj_list( ch, arg, ch->in_room->contents );
	 if ( obj == NULL )
	 {
		  send_to_char( "{wIt's not here.{x\n\r", ch );
		  return;
	 }

	 if( (obj->item_type == ITEM_CORPSE_PC) )
	 {
		  send_to_char ( "Funny. Read the playerkill rules, NOW. 'help pkrules'.{x\n\r", ch );
		  return;
	 }

	 /* create and rename the steak */
	 buf[0]='\0';
	 strcat(buf,"A steak of ");
	 strcat(buf,str_dup(obj->short_descr));
	 strcat(buf," is here.");
	 steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
	 steak->description=str_dup(buf);
	 steak->value[0] = ch->level / 2;
	 steak->value[1] = ch->level;
	 buf[0]='\0';
	 strcat(buf,"A steak of ");
	 strcat(buf,str_dup(obj->short_descr));
	 steak->short_descr=str_dup(buf);

	 /* Check the skill roll, and put a random ammount of steaks here. */
	 if(number_percent( ) < get_skill(ch,gsn_butcher))
	 {
		 numst = dice(1,4);
		 switch(numst)
		 {
		 case 1:

			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			act( "{w$n butchers a corpse and creates a steak.{x\n\r", ch, steak, NULL, TO_ROOM );
			act( "{wYou butcher a corpse and create a steak.{x\n\r",  ch, steak, NULL, TO_CHAR );
			break;

		 case 2:
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			act( "{w$n butchers a corpse and creates two steaks.{x\n\r", ch, steak, NULL, TO_ROOM );
			act( "{wYou butcher a corpse and create two steaks.{x\n\r",  ch, steak, NULL, TO_CHAR );
			break;

		 case 3:
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			act( "{w$n butchers a corpse and creates three steaks.{x\n\r", ch, steak, NULL, TO_ROOM );
			act( "{wYou butcher a corpse and create three steaks.{x\n\r",  ch, steak, NULL, TO_CHAR );
			break;

		 case 4:
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			steak = create_object( get_obj_index( OBJ_VNUM_STEAK ), 0 );
			obj_to_room( steak, ch->in_room );
			act( "{w$n butchers a corpse and creates four steaks.{x\n\r", ch, steak, NULL, TO_ROOM );
			act( "{wYou butcher a corpse and create four steaks.{x\n\r",  ch, steak, NULL, TO_CHAR );
			break;

		}
		check_improve(ch,gsn_butcher,TRUE,1);
	 }
	 else
	 {
		 act( "{w$n fails to butcher a corpse, and destroys it.{x\n\r", ch, steak, NULL, TO_ROOM );
		 act( "{wYou fail to butcher a corpse, and destroy it.{\n\r",   ch, steak, NULL, TO_CHAR );
		 check_improve(ch,gsn_butcher,FALSE,1);
	 }

	 /* dump items caried */
	 /* Taken from the original ROM code and added into here. */
	 if ( obj->item_type == ITEM_CORPSE_PC )
	 {   /* save the contents */
		 {
				OBJ_DATA *t_obj, *next_obj;
				for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
				{
					 next_obj = t_obj->next_content;
					 obj_from_obj(t_obj);
					 if (obj->in_obj) /* in another object */
						  obj_to_obj(t_obj,obj->in_obj);
					 else if (obj->carried_by) /* carried */
						  if (obj->wear_loc == WEAR_FLOAT)
								if (obj->carried_by->in_room == NULL)
									 extract_obj(t_obj);
								else
									 obj_to_room(t_obj,obj->carried_by->in_room);
						  else
								obj_to_char(t_obj,obj->carried_by);
					else if (obj->in_room == NULL) /* destroy it */
						  extract_obj(t_obj);
					 else /* to a room */
						  obj_to_room(t_obj,obj->in_room);
			  }
		}
  }
	 if ( obj->item_type == ITEM_CORPSE_NPC )
	 {
		 {
				OBJ_DATA *t_obj, *next_obj;
				for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
				{
					 next_obj = t_obj->next_content;
					 obj_from_obj(t_obj);
					 if (obj->in_obj) /* in another object */
						  obj_to_obj(t_obj,obj->in_obj);
					 else if (obj->carried_by) /* carried */
						  if (obj->wear_loc == WEAR_FLOAT)
								if (obj->carried_by->in_room == NULL)
									 extract_obj(t_obj);
								else
									 obj_to_room(t_obj,obj->carried_by->in_room);
						  else
								obj_to_char(t_obj,obj->carried_by);
					 else if (obj->in_room == NULL) /* destroy it */
						  extract_obj(t_obj);
					 else /* to a room */
						  obj_to_room(t_obj,obj->in_room);
			}
	  }
  }
	 /* Now remove the corpse */
	 extract_obj(obj);
	 return;
}
