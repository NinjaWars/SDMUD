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
#include "db.h"

char *const where_name[] = {
    "{R<{rused as light       {R:{x",
    "{R<{rworn on finger      {R:{x",
    "{R<{rworn on finger      {R:{x",
    "{R<{rworn around neck    {R:{x",
    "{R<{rworn around neck    {R:{x",
    "{R<{rworn on torso       {R:{x",
    "{R<{rworn on head        {R:{x",
    "{R<{rworn on legs        {R:{x",
    "{R<{rworn on feet        {R:{x",
    "{R<{rworn on hands       {R:{x",
    "{R<{rworn on arms        {R:{x",
    "{R<{rworn as shield      {R:{x",
    "{R<{rworn about body     {R:{x",
    "{R<{rworn about waist    {R:{x",
    "{R<{rworn around wrist   {R:{x",
    "{R<{rworn around wrist   {R:{x",
    "{R<{rprimary wield       {R:{x",
    "{R<{rheld                {R:{x",
    "{R<{rfloating nearby     {R:{x",
    "{R<{rsecondary wield     {R:{x",
    "{R<{rworn on face        {R:{x",
    "{R<{rworn on the ankle   {R:{x",
    "{R<{rworn on the ankle   {R:{x",
    "{R<{rworn as tattoo      {R:{x",
    "{R<{rworn as aura        {R:{x",
    "{R<{rworn on left ear    {R:{x",
    "{R<{rworn on right ear   {R:{x",
    "{R<{rworn as clan tattoo {R:{x"
};

sh_int const where_order[] = {
/*	 1,  2,  3,  4,  5, 25, 26, 6, 20,  7,  8,  9, 21, 22, 10, 11, 23, 12, 13, 14, 15, 16, 19, 17, 18, 24, 0 */
    /* SD style baby! */
    6, 25, 26, 20, 3, 4, 10, 14, 15, 9, 1, 2, 0, 5, 12, 13, 7, 21, 22, 8, 18, 24, 23, 27, 11, 17, 16, 19
};

/* for do_count */
//int max_on = 0;
bool is_pm = FALSE;

/* for stances */
extern char* stance_name[];

char *format_obj_to_char ( OBJ_DATA * obj, CHAR_DATA * ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ( ( fShort &&
           ( obj->short_descr == NULL || obj->short_descr[0] == '\0' ) ) ||
         ( obj->description == NULL || obj->description[0] == '\0' ) )
        return buf;

    if ( !IS_SET ( ch->comm, COMM_LONG ) )
    {
        strcat ( buf, "{x[{y.{R.{B.{M.{Y.{W.{G.{x]" );
        if ( IS_OBJ_STAT ( obj, ITEM_INVIS ) )
            buf[5] = 'V';
        if ( IS_AFFECTED ( ch, AFF_DETECT_EVIL ) &&
             IS_OBJ_STAT ( obj, ITEM_EVIL ) )
            buf[8] = 'E';
        if ( IS_AFFECTED ( ch, AFF_DETECT_GOOD ) &&
             IS_OBJ_STAT ( obj, ITEM_BLESS ) )
            buf[11] = 'B';
        if ( IS_AFFECTED ( ch, AFF_DETECT_MAGIC ) &&
             IS_OBJ_STAT ( obj, ITEM_MAGIC ) )
            buf[14] = 'M';
        if ( IS_OBJ_STAT ( obj, ITEM_GLOW ) )
            buf[17] = 'G';
        if ( IS_OBJ_STAT ( obj, ITEM_HUM ) )
            buf[20] = 'H';
        if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) )
            buf[23] = 'Q';
        if ( !str_cmp ( buf, "{x[{y.{R.{B.{M.{Y.{W.{G.{x]" ) )
            buf[0] = '\0';
        if ( !IS_NPC ( ch ) && ( ch->pcdata->questobj > 0 ) &&
             ( obj->pIndexData->vnum == ch->pcdata->questobj ) )
            strcat ( buf, " {W[{RT{rARGE{RT{W]{x" );
    }
    else
    {
        if ( IS_OBJ_STAT ( obj, ITEM_INVIS ) )
            strcat ( buf, "({yInvis{x)" );
        if ( IS_OBJ_STAT ( obj, ITEM_DARK ) )
            strcat ( buf, "({DHidden{x)" );
        if ( IS_AFFECTED ( ch, AFF_DETECT_EVIL ) &&
             IS_OBJ_STAT ( obj, ITEM_EVIL ) )
            strcat ( buf, "({RRed Aura{x)" );
        if ( IS_AFFECTED ( ch, AFF_DETECT_GOOD ) &&
             IS_OBJ_STAT ( obj, ITEM_BLESS ) )
            strcat ( buf, "({BBlue Aura{x)" );
        if ( IS_AFFECTED ( ch, AFF_DETECT_MAGIC ) &&
             IS_OBJ_STAT ( obj, ITEM_MAGIC ) )
            strcat ( buf, "({yMagical{x)" );
        if ( IS_OBJ_STAT ( obj, ITEM_GLOW ) )
            strcat ( buf, "({YGlowing{x)" );
        if ( IS_OBJ_STAT ( obj, ITEM_HUM ) )
            strcat ( buf, "({yHumming{x)" );
        if ( IS_OBJ_STAT ( obj, ITEM_QUEST ) )
            strcat ( buf, "({GQuest{x)" );
        if ( !IS_NPC ( ch ) && ( ch->pcdata->questobj > 0 ) &&
             ( obj->pIndexData->vnum == ch->pcdata->questobj ) )
            strcat ( buf, "{W[{RT{rARGE{RT{W]{x" );
    }

    if ( buf[0] != '\0' )
    {
        strcat ( buf, " " );
    }

    if ( fShort )
    {
        if ( obj->short_descr != NULL )
            strcat ( buf, obj->short_descr );
    }
    else
    {
        if ( obj->description != NULL )
            strcat ( buf, obj->description );
    }
    if ( strlen ( buf ) <= 0 )
        strcat ( buf,
                 "This object has no description. Please inform Rhaelar." );

    return buf;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
BUFFER2 *show_list_to_char ( OBJ_DATA * list, CHAR_DATA * ch, bool fShort,
                            bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
//    BUFFER *output;
    BUFFER2 *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    /*
     * Alloc space for output lines.
     */
    //output = new_buf (  );
    output = buffer_new ( 1337 );
    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
        count++;
/*
    if ( count > 350 )
    {
        send_to_char( "That is WAY too much junk!  Drop some of it!\n\r", ch );
        return;
    }
*/
    prgpstrShow = alloc_mem ( count * sizeof ( char * ) );
    prgnShow = alloc_mem ( count * sizeof ( int ) );
    nShow = 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
        if ( obj->wear_loc == WEAR_NONE && can_see_obj ( ch, obj ) )
        {
            pstrShow = format_obj_to_char ( obj, ch, fShort );

            fCombine = FALSE;

            if ( IS_NPC ( ch ) || IS_SET ( ch->comm, COMM_COMBINE ) )
            {
                /*
                 * Look for duplicates, case sensitive.
                 * Matches tend to be near end so run loop backwords.
                 */
                for ( iShow = nShow - 1; iShow >= 0; iShow-- )
                {
                    if ( !str_cmp ( prgpstrShow[iShow], pstrShow ) )
                    {
                        prgnShow[iShow]++;
                        fCombine = TRUE;
                        break;
                    }
                }
            }

            /*
             * Couldn't combine, or didn't want to.
             */
            if ( !fCombine )
            {
                prgpstrShow[nShow] = str_dup ( pstrShow );
                prgnShow[nShow] = 1;
                nShow++;
            }
        }
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
        if ( prgpstrShow[iShow][0] == '\0' )
        {
            free_string ( prgpstrShow[iShow] );
            continue;
        }

        if ( IS_NPC ( ch ) || IS_SET ( ch->comm, COMM_COMBINE ) )
        {
            if ( prgnShow[iShow] != 1 )
            {
                sprintf ( buf, "(%2d) ", prgnShow[iShow] );
		buffer_strcat ( output, buf );
                //add_buf ( output, buf );            
	    }
            else
            {
                //add_buf ( output, "     " );
		buffer_strcat ( output, "     " );
            }
        }
        /*add_buf ( output, prgpstrShow[iShow] );
        add_buf ( output, "\n\r" ); */
        buffer_strcat ( output, prgpstrShow[iShow] );
        buffer_strcat ( output, "\n\r" );
        free_string ( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
        if ( IS_NPC ( ch ) || IS_SET ( ch->comm, COMM_COMBINE ) )
	    buffer_strcat ( output, "     " );
            //add_buf ( output, "     " );        
	//add_buf ( output, "Nothing.\n\r" );
	buffer_strcat ( output, "Nothing.\n\r" );
/*            send_to_char ( "     ", ch );
        send_to_char ( "Nothing.\n\r", ch ); */
    }
    /*
     * Clean up.
     */
    free_mem ( prgpstrShow, count * sizeof ( char * ) );
    free_mem ( prgnShow, count * sizeof ( int ) );

    return output;
}

void show_char_to_char_0 ( CHAR_DATA * victim, CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH];
    int vict_condition;

    buf[0] = '\0';

    if ( victim->max_hit > 0 )
        vict_condition = victim->hit * 100 / victim->max_hit;
    else
        vict_condition = -1;

    if ( !IS_SET ( ch->comm, COMM_LONG ) )
    {
        strcat ( buf, "{x[{y.{D.{c.{b.{w.{C.{r.{B.{g.{m.{R.{Y.{W.{G.{x]" );
        if ( IS_SHIELDED ( victim, SHD_INVISIBLE ) )
            buf[5] = 'V';
        if ( IS_AFFECTED ( victim, AFF_HIDE ) ||
             IS_AFFECTED ( victim, AFF_SNEAK ) )
            buf[8] = 'H';
        if ( IS_AFFECTED ( victim, AFF_CHARM ) )
            buf[11] = 'C';
        if ( IS_AFFECTED ( victim, AFF_PASS_DOOR ) )
            buf[14] = 'T';
        if ( IS_AFFECTED ( victim, AFF_FAERIE_FIRE ) )
            buf[17] = 'P';
        if ( IS_SHIELDED ( victim, SHD_ICE ) )
            buf[20] = 'I';
        if ( IS_SHIELDED ( victim, SHD_FIRE ) )
            buf[23] = 'F';
        if ( IS_SHIELDED ( victim, SHD_SHOCK ) )
            buf[26] = 'L';
        if ( IS_SHIELDED ( victim, SHD_ACID ) )
            buf[29] = 'A';
        if ( IS_SHIELDED ( victim, SHD_POISON ) )
            buf[32] = 'P';
        if ( IS_EVIL ( victim ) && IS_AFFECTED ( ch, AFF_DETECT_EVIL ) )
            buf[35] = 'E';
        if ( IS_GOOD ( victim ) && IS_AFFECTED ( ch, AFF_DETECT_GOOD ) )
            buf[38] = 'G';
        if ( IS_SHIELDED ( victim, SHD_SANCTUARY ) )
            buf[41] = 'S';
        if ( victim->on_quest )
            buf[44] = 'Q';
        if ( !str_cmp ( buf, "{x[{y.{D.{c.{b.{w.{C.{r.{B.{R.{Y.{W.{G.{x]" ) )
            buf[0] = '\0';
        if ( IS_SET ( victim->comm, COMM_AFK ) )
            strcat ( buf, "[{yAFK{x]" );
        if ( victim->invis_level >= LEVEL_ANCIENT )
            strcat ( buf, "({WWizi{x)" );
        if ( vict_condition < 33 )
            strcat ( buf, "{R(Wounded){x " );
        if ( !IS_NPC ( victim ) && !victim->desc )
            strcat ( buf, "{G({cLinkdead{G){x" );
        if ( !IS_NPC ( ch ) && IS_NPC ( victim ) &&
             ( ch->pcdata->questmob > 0 ) &&
	     ( victim->isquestmob > 0 ) &&
             ( victim->pIndexData->vnum == ch->pcdata->questmob ) )
            strcat ( buf, "{W[{RT{rARGE{RT{x{W]{x" );
    }
    else
    {
        if ( IS_SET ( victim->comm, COMM_AFK ) )
            strcat ( buf, "[{yAFK{x]" );
        if ( IS_SHIELDED ( victim, SHD_INVISIBLE ) )
            strcat ( buf, "({yInvis{x)" );
        if ( victim->invis_level >= LEVEL_ANCIENT )
            strcat ( buf, "({WWizi{x)" );
        if ( IS_AFFECTED ( victim, AFF_HIDE ) )
            strcat ( buf, "({DHide{x)" );
        if ( IS_AFFECTED ( victim, AFF_CHARM ) )
            strcat ( buf, "({cCharmed{x)" );
        if ( IS_AFFECTED ( victim, AFF_PASS_DOOR ) )
            strcat ( buf, "({bTranslucent{x)" );
        if ( IS_AFFECTED ( victim, AFF_FAERIE_FIRE ) )
            strcat ( buf, "({wPink Aura{x)" );
        if ( IS_SHIELDED ( victim, SHD_ICE ) )
            strcat ( buf, "({CI{DS{x)" );
        if ( IS_SHIELDED ( victim, SHD_FIRE ) )
            strcat ( buf, "({rF{DS{x)" );
        if ( IS_SHIELDED ( victim, SHD_SHOCK ) )
            strcat ( buf, "({BS{DS{x)" );
        if ( IS_SHIELDED ( victim, SHD_ACID ) )
            strcat ( buf, "({GA{DS{x)" );
        if ( IS_SHIELDED ( victim, SHD_POISON ) )
            strcat ( buf, "({MP{DS{x)" );
        if ( IS_SHIELDED ( victim, SHD_BRIAR ) )
            strcat ( buf, "({gT{DS{x)" );
        if ( IS_EVIL ( victim ) && IS_AFFECTED ( ch, AFF_DETECT_EVIL ) )
            strcat ( buf, "({RRed Aura{x)" );
        if ( IS_GOOD ( victim ) && IS_AFFECTED ( ch, AFF_DETECT_GOOD ) )
            strcat ( buf, "({YGolden Aura{x)" );
        if ( IS_SHIELDED ( victim, SHD_SANCTUARY ) )
            strcat ( buf, "({WWhite Aura{x)" );
        if ( victim->on_quest )
            strcat ( buf, "({GQuest{x)" );
        if ( vict_condition < 33 )
            strcat ( buf, "{R(Wounded){x " );
        if ( !IS_NPC ( victim ) && !victim->desc )
            strcat ( buf, "{G({cLinkdead{G){x" );
        if ( !IS_NPC ( ch ) && IS_NPC ( victim ) &&
             ( ch->pcdata->questmob > 0 ) &&
             ( victim->isquestmob > 0 ) &&
	     ( victim->pIndexData->vnum == ch->pcdata->questmob ) )
            strcat ( buf, "{W[{RT{rARGE{RT{x{W]{x" );
        if ( IS_SHIELDED ( victim, SHD_WARD ) )
            strcat ( buf, "({WWard{x)" );
    }
    if ( !IS_NPC ( victim ) && IS_SET ( victim->act2, PLR2_PKVIOLENT ) )
        strcat ( buf, "({RVIOLENT{x)" );
    if ( !IS_NPC ( victim ) && IS_SET ( victim->act2, PLR2_PKSAFE ) )
        strcat ( buf, "({wSAFE{x)" );
    if ( !IS_NPC ( victim ) && IS_SET ( victim->act, PLR_TWIT ) )
        strcat ( buf, "({rTWIT{x)" );
    if (!IS_NPC(victim) && IS_SET (victim->pcdata->tag_flags, TAG_RED))
      strcat (buf, "({RRed{x)");
    if (!IS_NPC(victim) && IS_SET (victim->pcdata->tag_flags, TAG_BLUE))
      strcat (buf, "({BBlue{x)");
    if ( !IS_NPC(victim) && IS_SET ( victim->pcdata->tag_flags, TAG_FROZEN ) )
      strcat (buf, "({CFr{coz{Ce{cn{x)");
    if ( !IS_NPC ( victim ) && IS_SET ( victim->act2, PLR2_HELPER ) )
        strcat ( buf, "({yHelper{x)" );    

    if ( buf[0] != '\0' )
    {
        strcat ( buf, " " );
    }
    if ( victim->position == victim->start_pos &&
         victim->long_descr[0] != '\0' )
    {
        strcat ( buf, victim->long_descr );
        send_to_char ( buf, ch );
        return;
    }

    strcat ( buf, PERS ( victim, ch ) );
    if ( !IS_NPC ( victim ) && !IS_SET ( ch->comm, COMM_BRIEF ) &&
         victim->position == POS_STANDING && ch->on == NULL )
        strcat ( buf, victim->pcdata->title );

    switch ( victim->position )
    {
        case POS_DEAD:
            strcat ( buf, " is DEAD!!" );
            break;
        case POS_MORTAL:
            strcat ( buf, " is mortally wounded." );
            break;
        case POS_INCAP:
            strcat ( buf, " is incapacitated." );
            break;
        case POS_STUNNED:
            strcat ( buf, " is lying here stunned." );
            break;
        case POS_SLEEPING:
            if ( victim->on != NULL )
            {
                if ( IS_SET ( victim->on->value[2], SLEEP_AT ) )
                {
                    sprintf ( message, " is sleeping at %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
                else if ( IS_SET ( victim->on->value[2], SLEEP_ON ) )
                {
                    sprintf ( message, " is sleeping on %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
                else
                {
                    sprintf ( message, " is sleeping in %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
            }
            else
                strcat ( buf, " is sleeping here." );
            break;
        case POS_RESTING:
            if ( victim->on != NULL )
            {
                if ( IS_SET ( victim->on->value[2], REST_AT ) )
                {
                    sprintf ( message, " is resting at %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
                else if ( IS_SET ( victim->on->value[2], REST_ON ) )
                {
                    sprintf ( message, " is resting on %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
                else
                {
                    sprintf ( message, " is resting in %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
            }
            else
                strcat ( buf, " is resting here." );
            break;
        case POS_SITTING:
            if ( victim->on != NULL )
            {
                if ( IS_SET ( victim->on->value[2], SIT_AT ) )
                {
                    sprintf ( message, " is sitting at %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
                else if ( IS_SET ( victim->on->value[2], SIT_ON ) )
                {
                    sprintf ( message, " is sitting on %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
                else
                {
                    sprintf ( message, " is sitting in %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
            }
            else
                strcat ( buf, " is sitting here." );
            break;
        case POS_STANDING:
            if ( victim->on != NULL )
            {
                if ( IS_SET ( victim->on->value[2], STAND_AT ) )
                {
                    sprintf ( message, " is standing at %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
                else if ( IS_SET ( victim->on->value[2], STAND_ON ) )
                {
                    sprintf ( message, " is standing on %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
                else
                {
                    sprintf ( message, " is standing in %s.",
                              victim->on->short_descr );
                    strcat ( buf, message );
                }
            }
            else
                strcat ( buf, " is here." );
            break;
        case POS_FIGHTING:
            strcat ( buf, " is here, fighting " );
            if ( victim->fighting == NULL )
                strcat ( buf, "thin air??" );
            else if ( victim->fighting == ch )
                strcat ( buf, "YOU!" );
            else if ( victim->in_room == victim->fighting->in_room )
            {
                strcat ( buf, PERS ( victim->fighting, ch ) );
                strcat ( buf, "." );
            }
            else
                strcat ( buf, "someone who left??" );
            break;
    }

    strcat ( buf, "\n\r" );
    buf[0] = UPPER ( buf[0] );
    send_to_char ( buf, ch );
    return;
}

void show_char_to_char_1 ( CHAR_DATA * victim, CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH]/*, buf2[MAX_STRING_LENGTH]*/;
    OBJ_DATA *obj;
/*    BUFFER *output;
    BUFFER *outlist; */
    BUFFER2 *output;
    BUFFER2 *outlist;
    int iWear;
    int oWear;
    int percent;
    bool found;

    if ( can_see ( victim, ch ) && get_trust ( victim ) >= ch->ghost_level )
    {
        if ( ch == victim )
            act ( "$n looks at $mself.", ch, NULL, NULL, TO_ROOM );
        else
        {
            act ( "$n looks at you.", ch, NULL, victim, TO_VICT );
            act ( "$n looks at $N.", ch, NULL, victim, TO_NOTVICT );
        }
    }

    //output = new_buf (  );
    output = buffer_new ( 1337 );
    if ( victim->description[0] != '\0' )
    {
        sprintf ( buf, "{C%s{x", victim->description );
    }
    else
    {
	sprintf ( buf, "{CYou see nothing special about {R%s{x\n\r",
                  victim->name );
    }

    //add_buf ( output, buf );
    buffer_strcat ( output, buf );

    if (victim->stance[0] >=1 && victim->stance[0]<= 10)
    {
/*    	strcpy ( buf, PERS ( victim, ch ) );    	
	sprintf(buf2," is in the {R%s{x stance.\n\r", stance_name[victim->stance[0]]);
	strcpy ( buf, buf2 );*/
	sprintf(buf, "{R%s {xis in the {C%s{x stance.{x\n\r", victim->name, stance_name[victim->stance[0]]);
     buf[0] = UPPER(buf[0]);
   	//add_buf(output,buf); 
	buffer_strcat ( output, buf );
    }    
    else 
    {
    	strcpy ( buf, PERS ( victim, ch ) );    	    	
//	sprintf(buf2," is in {Rno{x stance.\n\r"me);
	strcat(buf," is in {Rno{x stance.\n\r" );
//	strcpy ( buf, buf2 );     
     buf[0] = UPPER(buf[0]);
   	//add_buf(output,buf);
	buffer_strcat ( output, buf );
    }    

    if ( victim->max_hit > 0 )
        percent = ( 100 * victim->hit ) / victim->max_hit;
    else
        percent = -1;

    buf[0] = '\0';
    strcpy ( buf, PERS ( victim, ch ) );

    if ( percent >= 100 )
        strcat ( buf, " {fis in excellent condition.{x\n\r" );
    else if ( percent >= 90 )
        strcat ( buf, " {fhas a few scratches.{x\n\r" );
    else if ( percent >= 75 )
        strcat ( buf, " {fhas some small wounds and bruises.{x\n\r" );
    else if ( percent >= 50 )
        strcat ( buf, " {fhas quite a few wounds.{x\n\r" );
    else if ( percent >= 30 )
        strcat ( buf, " {fhas some big nasty wounds and scratches.{x\n\r" );
    else if ( percent >= 15 )
        strcat ( buf, " {flooks pretty hurt.{x\n\r" );
    else if ( percent >= 0 )
        strcat ( buf, " {fis in awful condition.{x\n\r" );
    else
        strcat ( buf, " {fis bleeding to death.{x\n\r" );

    buf[0] = UPPER ( buf[0] );
    //add_buf ( output, buf );
    buffer_strcat ( output, buf );

    if ( IS_SHIELDED ( victim, SHD_ICE ) )
    {
        sprintf ( buf, "%s is surrounded by an {Cicy{x shield.\n\r",
                  PERS ( victim, ch ) );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }
    if ( IS_SHIELDED ( victim, SHD_FIRE ) )
    {
        sprintf ( buf, "%s is surrounded by a {Rfiery{x shield.\n\r",
                  PERS ( victim, ch ) );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }
    if ( IS_SHIELDED ( victim, SHD_SHOCK ) )
    {
        sprintf ( buf, "%s is surrounded by a {Bcrackling{x shield.\n\r",
                  PERS ( victim, ch ) );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }
    if ( IS_SHIELDED ( victim, SHD_ACID ) )
    {
        sprintf ( buf, "%s is surrounded by a {GA{gcidic{x shield.\n\r",
                  PERS ( victim, ch ) );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }
    if ( IS_SHIELDED ( victim, SHD_POISON ) )
    {
        sprintf ( buf, "%s is surrounded by a {MP{moisonous{x shield.\n\r",
                  PERS ( victim, ch ) );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }
    if ( IS_SHIELDED ( victim, SHD_BRIAR ) )
    {
        sprintf ( buf, "%s is surrounded by a {gt{yh{go{yr{gn{yy shield.\n\r",
                  PERS ( victim, ch ) );
        buf[0] = UPPER ( buf[0] );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }

    found = FALSE;
    for ( oWear = 0; oWear < MAX_WEAR; oWear++ )
    {
        iWear = where_order[oWear];
        if ( ( obj = get_eq_char ( victim, iWear ) ) != NULL &&
             can_see_obj ( ch, obj ) )
        {
            if ( !found )
            {
/*                sprintf ( buf, "\n\r" );
                add_buf ( output, buf );
	    	strcpy ( buf, PERS ( victim, ch ) );    	
		sprintf ( buf2, " {xis using:{x\n\r", victim->name );
		strcpy ( buf, buf2 );		
                add_buf ( output, buf );		
                found = TRUE; */
                sprintf ( buf, "\n\r" );
                //add_buf ( output, buf );
		buffer_strcat ( output, buf );
                sprintf ( buf, "{G%s is using:{x\n\r", victim->name );
                //add_buf ( output, buf );
		buffer_strcat ( output, buf );
                found = TRUE;		
            }
            sprintf ( buf, "%s%s\n\r", where_name[iWear],
                      format_obj_to_char ( obj, ch, TRUE ) );
	    buffer_strcat ( output, buf );
            //add_buf ( output, buf );        
	}
    }

    if ( victim != ch && !IS_NPC ( ch ) &&
         number_percent (  ) < get_skill ( ch, gsn_peek ) &&
         IS_SET ( ch->act, PLR_AUTOPEEK ) )
    {
        sprintf ( buf, "\n\r{GYou peek at the inventory:{x\n\r" );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        check_improve ( ch, gsn_peek, TRUE, 4 );
        outlist = show_list_to_char ( victim->carrying, ch, TRUE, TRUE );
        //add_buf ( output, buf_string ( outlist ) );
	buffer_strcat ( output, buffer_string ( outlist ) );
        //free_buf ( outlist );
        buffer_free ( outlist );
    }
    //page_to_char ( buf_string ( output ), ch );
    page_to_char ( buffer_string ( output ), ch );
    //free_buf ( output );
    buffer_free ( output );
    return;
}

void show_char_to_char ( CHAR_DATA * list, CHAR_DATA * ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
        if ( rch == ch )
            continue;

        if ( get_trust ( ch ) < rch->invis_level )
            continue;

        if ( get_trust ( ch ) < rch->ghost_level )
            continue;

        if ( can_see ( ch, rch ) )
        {
            show_char_to_char_0 ( rch, ch );
        }
        else if ( room_is_dark ( ch->in_room ) &&
                  IS_AFFECTED ( rch, AFF_INFRARED ) )
        {
            send_to_char ( "You spot {Rglowing red{x eyes in the darkness, watching YOU!\n\r",
                           ch );
        }
    }
    return;
}

CH_CMD ( do_peek )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
/*    BUFFER *output;
    BUFFER *outlist; */
    BUFFER2 *output;
    BUFFER2 *outlist; 
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( IS_NPC ( ch ) )
        return;

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Peek at who?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        do_inventory ( ch, "" );
        return;
    }

    if ( can_see ( victim, ch ) && get_trust ( victim ) >= ch->ghost_level )
    {
        act ( "$n peers intently at you.", ch, NULL, victim, TO_VICT );
        act ( "$n peers intently at $N.", ch, NULL, victim, TO_NOTVICT );
    }

    //output = new_buf (  );
    output = buffer_new ( 1337 );

    if ( number_percent (  ) < get_skill ( ch, gsn_peek ) )
    {
        sprintf ( buf, "\n\r{GYou peek at the inventory:{x\n\r" );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        check_improve ( ch, gsn_peek, TRUE, 4 );
        outlist = show_list_to_char ( victim->carrying, ch, TRUE, TRUE );
        //add_buf ( output, buf_string ( outlist ) );
	buffer_strcat ( output, buffer_string ( outlist ) );
        //free_buf ( outlist );
	buffer_free ( outlist );
    }
    else
    {
        sprintf ( buf, "{RYou fail to see anything.{x\n\r" );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
        check_improve ( ch, gsn_peek, FALSE, 2 );
    }
    //page_to_char ( buf_string ( output ), ch );
    page_to_char ( buffer_string ( output ), ch );
    //free_buf ( output );
    buffer_free ( output );
    return;
}

bool check_blind ( CHAR_DATA * ch )
{

    if ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_HOLYLIGHT ) )
        return TRUE;

    if ( IS_AFFECTED ( ch, AFF_BLIND ) )
    {
        send_to_char ( "You can't see a thing!\n\r", ch );
        return FALSE;
    }

    return TRUE;
}

/* changes your scroll */
CH_CMD ( do_scroll )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        if ( ch->lines == 0 )
            send_to_char ( "You do not page long messages.\n\r", ch );
        else
        {
            sprintf ( buf, "You currently display %d lines per page.\n\r",
                      ch->lines + 2 );
            send_to_char ( buf, ch );
        }
        return;
    }

    if ( !is_number ( arg ) )
    {
        send_to_char ( "You must provide a number.\n\r", ch );
        return;
    }

    lines = atoi ( arg );

    if ( lines == 0 )
    {
        send_to_char ( "Paging disabled.\n\r", ch );
        ch->lines = 0;
        return;
    }

    if ( lines < 10 || lines > 100 )
    {
        send_to_char ( "You must provide a reasonable number.\n\r", ch );
        return;
    }

    sprintf ( buf, "Scroll set to %d lines.\n\r", lines );
    send_to_char ( buf, ch );
    ch->lines = lines - 2;
}

/* RT does socials */
CH_CMD ( do_socials )
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col, count = 0;

    col = 0;

    for ( iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++ )
    {
        sprintf ( buf, "%-12s", social_table[iSocial].name );
	count++;
        send_to_char ( buf, ch );
        if ( ++col % 6 == 0 )
            send_to_char ( "\n\r", ch );
    }

    if ( col % 6 != 0 )
        send_to_char ( "\n\r", ch );
    printf_to_char( ch, "\n\r\n\r{RSocials found{x: {Y%d{x\n\r", count );

    return;
}

/* RT Commands to replace news, motd, imotd, etc from ROM */

CH_CMD ( do_motd )
{
    do_help ( ch, "motd" );
}

CH_CMD ( do_imotd )
{
    do_help ( ch, "imotd" );
}

CH_CMD ( do_rules )
{
    do_help ( ch, "rules" );
}

CH_CMD ( do_story )
{
    do_help ( ch, "story" );
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

CH_CMD ( do_autolist )
{
    /* lists most player flags */
    if ( IS_NPC ( ch ) )
        return;

    send_to_char ( "   {cOption     {YS{Dtatus\n\r", ch );
    send_to_char ( "{B=={b--{B=={b--{B=={b--{B=={b--{B=={b--{B=={x\n\r", ch );

    send_to_char ( "{cautoassist{x:     ", ch );
    if ( IS_SET ( ch->act, PLR_AUTOASSIST ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{cautoexit{x:       ", ch );
    if ( IS_SET ( ch->act, PLR_AUTOEXIT ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{cautogold{x:       ", ch );
    if ( IS_SET ( ch->act, PLR_AUTOGOLD ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{cautoloot{x:       ", ch );
    if ( IS_SET ( ch->act, PLR_AUTOLOOT ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{cautosac{x:        ", ch );
    if ( IS_SET ( ch->act, PLR_AUTOSAC ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{cautosplit{x:      ", ch );
    if ( IS_SET ( ch->act, PLR_AUTOSPLIT ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{cautopeek{x:       ", ch );
    if ( IS_SET ( ch->act, PLR_AUTOPEEK ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{cautostore{x:      ", ch );
    if ( IS_SET ( ch->comm, COMM_STORE ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{ccompact mode{x:   ", ch );
    if ( IS_SET ( ch->comm, COMM_COMPACT ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{cprompt{x:         ", ch );
    if ( IS_SET ( ch->comm, COMM_PROMPT ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{ccombine items{x:  ", ch );
    if ( IS_SET ( ch->comm, COMM_COMBINE ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    send_to_char ( "{clong flags{x:     ", ch );
    if ( IS_SET ( ch->comm, COMM_LONG ) )
        send_to_char ( "{YON{x\n\r", ch );
    else
        send_to_char ( "{DOFF{x\n\r", ch );

    if ( !IS_SET ( ch->act, PLR_CANLOOT ) )
        send_to_char ( "{cCorpse{x:{Y         No Loot{x\n\r", ch );
    else
        send_to_char ( "{cCorpse{x: {D        Loot{x\n\r", ch );

    if ( IS_SET ( ch->act, PLR_NOSUMMON ) )
        send_to_char ( "{cSummonable{x:  {D   No{x\n\r", ch );
    else
        send_to_char ( "{cSummonable{x:  {Y   Yes\n\r", ch );

    if ( IS_SET ( ch->act, PLR_NOFOLLOW ) )
        send_to_char ( "{cFollowers{x: {D     No\n\r", ch );
    else
        send_to_char ( "{cFollowers{x: {Y     Yes{x\n\r", ch );

    if ( IS_SET ( ch->act2, PLR2_NO_EXP ) )
        send_to_char ( "{cExp from kills{x: {DNo\n\r", ch );
    else
        send_to_char ( "{cExp from kills{x: {YYes{x\n\r", ch );
}

CH_CMD ( do_autoassist )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_AUTOASSIST ) )
    {
        send_to_char ( "Autoassist removed.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_AUTOASSIST );
    }
    else
    {
        send_to_char ( "You will now assist when needed.\n\r", ch );
        SET_BIT ( ch->act, PLR_AUTOASSIST );
    }
}

CH_CMD ( do_autoexit )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_AUTOEXIT ) )
    {
        send_to_char ( "Exits will no longer be displayed.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_AUTOEXIT );
    }
    else
    {
        send_to_char ( "Exits will now be displayed.\n\r", ch );
        SET_BIT ( ch->act, PLR_AUTOEXIT );
    }
}

CH_CMD ( do_autogold )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_AUTOGOLD ) )
    {
        send_to_char ( "Autogold removed.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_AUTOGOLD );
    }
    else
    {
        send_to_char ( "Automatic gold looting set.\n\r", ch );
        SET_BIT ( ch->act, PLR_AUTOGOLD );
    }
}

CH_CMD ( do_autoloot )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_AUTOLOOT ) )
    {
        send_to_char ( "Autolooting removed.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_AUTOLOOT );
    }
    else
    {
        send_to_char ( "Automatic corpse looting set.\n\r", ch );
        SET_BIT ( ch->act, PLR_AUTOLOOT );
    }
}

CH_CMD ( do_autosac )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_AUTOSAC ) )
    {
        send_to_char ( "Autosacrificing removed.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_AUTOSAC );
    }
    else
    {
        send_to_char ( "Automatic corpse sacrificing set.\n\r", ch );
        SET_BIT ( ch->act, PLR_AUTOSAC );
    }
}

CH_CMD ( do_autosplit )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_AUTOSPLIT ) )
    {
        send_to_char ( "Autosplitting removed.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_AUTOSPLIT );
    }
    else
    {
        send_to_char ( "Automatic gold splitting set.\n\r", ch );
        SET_BIT ( ch->act, PLR_AUTOSPLIT );
    }
}

CH_CMD ( do_autopeek )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_AUTOPEEK ) )
    {
        send_to_char ( "Autopeek removed.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_AUTOPEEK );
    }
    else
    {
        send_to_char ( "Automatic peek set.\n\r", ch );
        SET_BIT ( ch->act, PLR_AUTOPEEK );
    }
}

CH_CMD ( do_brief )
{
    if ( IS_SET ( ch->comm, COMM_BRIEF ) )
    {
        send_to_char ( "Full descriptions activated.\n\r", ch );
        REMOVE_BIT ( ch->comm, COMM_BRIEF );
    }
    else
    {
        send_to_char ( "Short descriptions activated.\n\r", ch );
        SET_BIT ( ch->comm, COMM_BRIEF );
    }
}

CH_CMD ( do_compact )
{
    if ( IS_SET ( ch->comm, COMM_COMPACT ) )
    {
        send_to_char ( "Compact mode removed.\n\r", ch );
        REMOVE_BIT ( ch->comm, COMM_COMPACT );
    }
    else
    {
        send_to_char ( "Compact mode set.\n\r", ch );
        SET_BIT ( ch->comm, COMM_COMPACT );
    }
}

CH_CMD ( do_long )
{
    if ( !IS_SET ( ch->comm, COMM_LONG ) )
    {
        send_to_char ( "Long flags activated.\n\r", ch );
        SET_BIT ( ch->comm, COMM_LONG );
    }
    else
    {
        send_to_char ( "Short flags activated.\n\r", ch );
        REMOVE_BIT ( ch->comm, COMM_LONG );
    }
}

CH_CMD ( do_show )
{
    if ( IS_SET ( ch->comm, COMM_SHOW_AFFECTS ) )
    {
        send_to_char ( "Affects will no longer be shown in score.\n\r", ch );
        REMOVE_BIT ( ch->comm, COMM_SHOW_AFFECTS );
    }
    else
    {
        send_to_char ( "Affects will now be shown in score.\n\r", ch );
        SET_BIT ( ch->comm, COMM_SHOW_AFFECTS );
    }
}

CH_CMD ( do_prompt )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_PROMPT ) )
        {
            send_to_char ( "You will no longer see prompts.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_PROMPT );
        }
        else
        {
            send_to_char ( "You will now see prompts.\n\r", ch );
            SET_BIT ( ch->comm, COMM_PROMPT );
        }
        return;
    }

    if ( !str_cmp ( argument, "default" ) )
        strcpy ( buf,
                 "{W<{r%h{W/{R%H{whp {m%m{W/{M%M{wm {y%v{W/{Y%V{wmv {g({B%X{wtnl{g){W>{x" );
    else if ( !str_cmp ( argument, "default2" ) )
        strcpy ( buf,
                 "{c({r%h{c/{R%H{chp {b%m{c/{B%M{cm {y%v{c/{Y%V{cmv{C ({g%X{ctnl{C){c>{x" );
    else if ( !str_cmp ( argument, "questor" ) )
        strcpy ( buf,
                 "{W<{r%h{W/{R%H{whp {m%m{W/{M%M{wm {y%v{W/{Y%V{wmv {g({B%X{wtnl{g) {y({W%q{y){W>{x" );
    else if ( !str_cmp ( argument, "imm" ) )
    {
        if ( !IS_IMMORTAL ( ch ) )
        {
            return;
        }
        else
        {
            strcpy ( buf,
                     "{Y({x%z{Y)({x%r{Y)({x%e{Y){x%c{Y({r%h{Y/{R%H{Dhp{Y){x" );
        }
    }
    else if ( !str_cmp ( argument, "imm2" ) )
    {
        if ( !IS_IMMORTAL ( ch ) )
        {
            return;
        }
        else
        {
            strcpy ( buf,
                     "{g({x%z{g)({x%r{g){x%c{g({r%h{g/{R%H {Dhp{b %m{g/{B%M {Dmn{g){x" );
        }
    }
    else
    {
        if ( strlen ( argument ) > 50 )
            argument[50] = '\0';
        strcpy ( buf, argument );
        smash_tilde ( buf );
        if ( str_suffix ( "%c", buf ) )
            strcat ( buf, "{x " );

    }

    free_string ( ch->prompt );
    ch->prompt = str_dup ( buf );
    sprintf ( buf, "Prompt set to %s\n\r", ch->prompt );
    send_to_char ( buf, ch );
    return;
}

CH_CMD ( do_combine )
{
    if ( IS_SET ( ch->comm, COMM_COMBINE ) )
    {
        send_to_char ( "Long inventory selected.\n\r", ch );
        REMOVE_BIT ( ch->comm, COMM_COMBINE );
    }
    else
    {
        send_to_char ( "Combined inventory selected.\n\r", ch );
        SET_BIT ( ch->comm, COMM_COMBINE );
    }
}

CH_CMD ( do_noloot )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act, PLR_CANLOOT ) )
    {
        send_to_char ( "Your corpse is now safe from thieves.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_CANLOOT );
    }
    else
    {
        send_to_char ( "Your corpse may now be looted.\n\r", ch );
        SET_BIT ( ch->act, PLR_CANLOOT );
    }
}

CH_CMD ( do_noexp )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act2, PLR2_NO_EXP ) )
    {
        send_to_char ( "You will now receive exp from kills.\n\r", ch );
        REMOVE_BIT ( ch->act2, PLR2_NO_EXP );
    }
    else
    {
        send_to_char ( "You will no longer receive exp from kills.\n\r", ch );
        SET_BIT ( ch->act2, PLR2_NO_EXP );
    }
}

CH_CMD ( do_nofollow )
{
    if ( IS_NPC(ch) || IS_AFFECTED( ch, AFF_CHARM ) )
        return;       

    if ( IS_SET ( ch->act, PLR_NOFOLLOW ) )
    {
        send_to_char ( "You now accept followers.\n\r", ch );
        REMOVE_BIT ( ch->act, PLR_NOFOLLOW );
    }
    else
    {
        send_to_char ( "You no longer accept followers.\n\r", ch );
        SET_BIT ( ch->act, PLR_NOFOLLOW );
        die_follower ( ch );
    }
}

CH_CMD ( do_nosummon )
{
    if ( IS_NPC ( ch ) )
    {
        if ( IS_SET ( ch->imm_flags, IMM_SUMMON ) )
        {
            send_to_char ( "You are no longer immune to summon.\n\r", ch );
            REMOVE_BIT ( ch->imm_flags, IMM_SUMMON );
        }
        else
        {
            send_to_char ( "You are now immune to summoning.\n\r", ch );
            SET_BIT ( ch->imm_flags, IMM_SUMMON );
        }
    }
    else
    {
        if ( IS_SET ( ch->act, PLR_NOSUMMON ) )
        {
            send_to_char ( "You are no longer immune to summon.\n\r", ch );
            REMOVE_BIT ( ch->act, PLR_NOSUMMON );
        }
        else
        {
            send_to_char ( "You are now immune to summoning.\n\r", ch );
            SET_BIT ( ch->act, PLR_NOSUMMON );
        }
    }
}

CH_CMD ( do_notran )
{
    if ( IS_NPC ( ch ) )
    {
        return;
    }
    else
    {
        if ( IS_SET ( ch->act, PLR_NOTRAN ) )
        {
            send_to_char ( "You are no longer immune to transport.\n\r", ch );
            REMOVE_BIT ( ch->act, PLR_NOTRAN );
        }
        else
        {
            send_to_char ( "You are now immune to transport.\n\r", ch );
            SET_BIT ( ch->act, PLR_NOTRAN );
        }
    }
}

CH_CMD ( do_look )
{
    char buf[MAX_STRING_LENGTH * 100];
    char arg1[MAX_INPUT_LENGTH * 100];
    char arg2[MAX_INPUT_LENGTH * 100];
    char arg3[MAX_INPUT_LENGTH * 100];
    //BUFFER *outlist;
    BUFFER2 *outlist;
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number, count;
    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }

    if ( ch->desc == NULL )
        return;

    if ( ch->position < POS_SLEEPING )
    {
        send_to_char ( "You can't see anything but stars!\n\r", ch );
        return;
    }

    if ( ch->position == POS_SLEEPING )
    {
        send_to_char ( "You can't see anything, you're sleeping!\n\r", ch );
        return;
    }

    if ( !check_blind ( ch ) )
        return;

    if ( !IS_NPC ( ch ) && !IS_SET ( ch->act, PLR_HOLYLIGHT ) &&
         room_is_dark ( ch->in_room ) )
    {
        send_to_char ( "{xIt is pitch {Dblack{x ... \n\r", ch );
        show_char_to_char ( ch->in_room->people, ch );
        return;
    }

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    number = number_argument ( arg1, arg3 );
    count = 0;

    if ( arg1[0] == '\0' || !str_cmp ( arg1, "auto" ) )
    {
    
        /* 'look' or 'look auto' */
        send_to_char ( "{w", ch );
        send_to_char ( ch->in_room->name, ch );
        send_to_char ( "{x", ch );

        if ( IS_IMMORTAL ( ch ) &&
             ( IS_NPC ( ch ) || IS_SET ( ch->act, PLR_HOLYLIGHT ) ) )
        {
            sprintf ( buf, " [Room %ld]", ch->in_room->vnum );
            send_to_char ( buf, ch );
        }

        send_to_char ( "\n\r", ch );

        if ( arg1[0] == '\0'
        || ( !IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF) ) )
        {
          if (ch->desc && !ch->desc->run_buf)
          {
            send_to_char( "  ",ch);
            send_to_char( ch->in_room->description, ch );
          }
        }

        if ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_AUTOEXIT ) )
        {
            send_to_char ( "\n\r", ch );
            do_exits ( ch, "auto" );
        }

        outlist = show_list_to_char ( ch->in_room->contents, ch, FALSE, FALSE );
	page_to_char ( buffer_string ( outlist ), ch );
        //page_to_char ( buf_string ( outlist ), ch );        
	//free_buf ( outlist );
	//buffer_free ( outlist );
        show_char_to_char ( ch->in_room->people, ch );
        return;
    }

    if ( !str_cmp ( arg1, "i" ) || !str_cmp ( arg1, "in" ) ||
         !str_cmp ( arg1, "on" ) )
    {
        /* 'look in' */
        if ( arg2[0] == '\0' )
        {
            send_to_char ( "Look in what?\n\r", ch );
            return;
        }

        if ( ( obj = get_obj_here ( ch, arg2 ) ) == NULL )
        {
            send_to_char ( "You do not see that here.\n\r", ch );
            return;
        }

        switch ( obj->item_type )
        {
            default:
                send_to_char ( "That is not a container.\n\r", ch );
                break;

            case ITEM_DRINK_CON:
                if ( obj->value[1] <= 0 )
                {
                    send_to_char ( "It is empty.\n\r", ch );
                    break;
                }

                sprintf ( buf, "It's %sfilled with  a %s liquid.\n\r",
                          obj->value[1] <
                          obj->value[0] /
                          4 ? "less than half-" : obj->value[1] <
                          3 * obj->value[0] /
                          4 ? "about half-" : "more than half-",
                          liq_table[obj->value[2]].liq_color );

                send_to_char ( buf, ch );
                break;

            case ITEM_CONTAINER:
            case ITEM_PIT:
            case ITEM_CORPSE_NPC:
            case ITEM_CORPSE_PC:
                if ( IS_SET ( obj->value[1], CONT_CLOSED ) )
                {
                    send_to_char ( "It is closed.\n\r", ch );
                    break;
                }

                act ( "$p holds:", ch, obj, NULL, TO_CHAR );
                outlist = show_list_to_char ( obj->contains, ch, TRUE, TRUE );
                //page_to_char ( buf_string ( outlist ), ch );
		page_to_char ( buffer_string ( outlist ), ch );
                //free_buf ( outlist );
		buffer_free ( outlist );
                break;
        }
        return;
    }

    if ( ( victim = get_char_room ( ch, arg1 ) ) != NULL )
    {
        show_char_to_char_1 ( victim, ch );
        return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( can_see_obj ( ch, obj ) )
        {                       /* player can see object */
            pdesc = get_extra_descr ( arg3, obj->extra_descr );
            if ( pdesc != NULL )
            {
                if ( ++count == number )
                {
                    send_to_char ( pdesc, ch );
                    return;
                }
                else
                    continue;
            }
            pdesc = get_extra_descr ( arg3, obj->pIndexData->extra_descr );
            if ( pdesc != NULL )
            {
                if ( ++count == number )
                {
                    send_to_char ( pdesc, ch );
                    return;
                }
                else
                    continue;
            }
            if ( is_name ( arg3, obj->name ) )
                if ( ++count == number )
                {
                    send_to_char ( obj->description, ch );
                    send_to_char ( "\n\r", ch );
                    return;
                }
        }
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
        if ( can_see_obj ( ch, obj ) )
        {
            pdesc = get_extra_descr ( arg3, obj->extra_descr );
            if ( pdesc != NULL )
                if ( ++count == number )
                {
                    send_to_char ( pdesc, ch );
                    return;
                }

            pdesc = get_extra_descr ( arg3, obj->pIndexData->extra_descr );
            if ( pdesc != NULL )
                if ( ++count == number )
                {
                    send_to_char ( pdesc, ch );
                    return;
                }
            if ( is_name ( arg3, obj->name ) )
                if ( ++count == number )
                {
                    send_to_char ( obj->description, ch );
                    send_to_char ( "\n\r", ch );
                    return;
                }
        }
    }

    pdesc = get_extra_descr ( arg3, ch->in_room->extra_descr );
    if ( pdesc != NULL )
    {
        if ( ++count == number )
        {
            send_to_char ( pdesc, ch );
            return;
        }
    }

    if ( count > 0 && count != number )
    {
        if ( count == 1 )
            sprintf ( buf, "You only see one %s here.\n\r", arg3 );
        else
            sprintf ( buf, "You only see %d of those here.\n\r", count );

        send_to_char ( buf, ch );
        return;
    }

    if ( !str_cmp ( arg1, "n" ) || !str_cmp ( arg1, "north" ) )
        door = 0;
    else if ( !str_cmp ( arg1, "e" ) || !str_cmp ( arg1, "east" ) )
        door = 1;
    else if ( !str_cmp ( arg1, "s" ) || !str_cmp ( arg1, "south" ) )
        door = 2;
    else if ( !str_cmp ( arg1, "w" ) || !str_cmp ( arg1, "west" ) )
        door = 3;
    else if ( !str_cmp ( arg1, "u" ) || !str_cmp ( arg1, "up" ) )
        door = 4;
    else if ( !str_cmp ( arg1, "d" ) || !str_cmp ( arg1, "down" ) )
        door = 5;
    else
    {
        send_to_char ( "You do not see that here.\n\r", ch );
        return;
    }

    /* 'look direction' */
    if ( ( ch->alignment < 0 ) &&
         ( pexit = ch->in_room->exit[door + 6] ) != NULL )
        door += 6;
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
        send_to_char ( "Nothing special there.\n\r", ch );
        return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
        send_to_char ( pexit->description, ch );
    else
        send_to_char ( "Nothing special there.\n\r", ch );

    if ( pexit->keyword != NULL && pexit->keyword[0] != '\0' &&
         pexit->keyword[0] != ' ' )
    {
        if ( IS_SET ( pexit->exit_info, EX_CLOSED ) )
        {
            act ( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
        }
        else if ( IS_SET ( pexit->exit_info, EX_ISDOOR ) )
        {
            act ( "The $d is open.", ch, NULL, pexit->keyword, TO_CHAR );
        }
    }
    return;
}

/* RT added back for the hell of it */
CH_CMD ( do_read )
{
    do_look ( ch, argument );
}

CH_CMD ( do_examine )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Examine what?\n\r", ch );
        return;
    }

    do_look ( ch, arg );

    if ( ( obj = get_obj_here ( ch, arg ) ) != NULL )
    {
        switch ( obj->item_type )
        {
            default:
                break;
/*
            case ITEM_JUKEBOX:
                do_play ( ch, "list" );
                break;
*/
            case ITEM_MONEY:
                if ( obj->value[0] == 0 )
                {
                    if ( obj->value[1] == 0 )
                        sprintf ( buf,
                                  "Odd...there's no coins in the pile.\n\r" );
                    else if ( obj->value[1] == 1 )
                        sprintf ( buf, "Wow. One gold coin.\n\r" );
                    else
                        sprintf ( buf,
                                  "There are %ld gold coins in the pile.\n\r",
                                  obj->value[1] );
                }
                else if ( obj->value[1] == 0 )
                {
                    if ( obj->value[0] == 1 )
                        sprintf ( buf, "Wow. One silver coin.\n\r" );
                    else
                        sprintf ( buf,
                                  "There are %ld silver coins in the pile.\n\r",
                                  obj->value[0] );
                }
                else
                    sprintf ( buf,
                              "There are %ld gold and %ld silver coins in the pile.\n\r",
                              obj->value[1], obj->value[0] );
                send_to_char ( buf, ch );
                break;

            case ITEM_DRINK_CON:
            case ITEM_CONTAINER:
            case ITEM_PIT:
            case ITEM_CORPSE_NPC:
            case ITEM_CORPSE_PC:
                sprintf ( buf, "in %s", argument );
                do_look ( ch, buf );
        }
    }

    return;
}

CH_CMD ( do_lore )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_lore].skill_level[ch->class] )
    {
        send_to_char ( "You would like to what?\n\r", ch );
        return;
    }

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "What would you like to know more about?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry ( ch, arg ) ) == NULL )
    {
        send_to_char
            ( "You cannot lore that because you do not have that.\n\r", ch );
        return;
    }
    if ( number_percent (  ) < get_skill ( ch, gsn_lore ) )
    {
        send_to_char ( "You learn more about this object:\n\r", ch );
        check_improve ( ch, gsn_lore, TRUE, 4 );
        sprintf ( buf, "Name(s): %s\n\r", obj->name );
        send_to_char ( buf, ch );
        sprintf ( buf, "Short description: %s\n\rLong description: %s\n\r",
                  obj->short_descr, obj->description );
        send_to_char ( buf, ch );
//    sprintf( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
        sprintf ( buf, "Extra Flags: %s\n\r",
//        wear_bit_name(obj->wear_flags), 
                  extra_bit_name ( obj->extra_flags ) );
        send_to_char ( buf, ch );
        sprintf ( buf, "Weight: %d\n\r", obj->weight / 10 );
        send_to_char ( buf, ch );
        sprintf ( buf, "Level: %d  Cost: %d\n\r", obj->level, obj->cost );
        send_to_char ( buf, ch );
        /* now give out vital statistics as per identify */
        switch ( obj->item_type )
        {
            case ITEM_SCROLL:
            case ITEM_POTION:
            case ITEM_PILL:
                sprintf ( buf, "Level %ld spells of:", obj->value[0] );
                send_to_char ( buf, ch );
                if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
                {
                    send_to_char ( " '", ch );
                    send_to_char ( skill_table[obj->value[1]].name, ch );
                    send_to_char ( "'", ch );
                }

                if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
                {
                    send_to_char ( " '", ch );
                    send_to_char ( skill_table[obj->value[2]].name, ch );
                    send_to_char ( "'", ch );
                }

                if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
                {
                    send_to_char ( " '", ch );
                    send_to_char ( skill_table[obj->value[3]].name, ch );
                    send_to_char ( "'", ch );
                }

                if ( obj->value[4] >= 0 && obj->value[4] < MAX_SKILL )
                {
                    send_to_char ( " '", ch );
                    send_to_char ( skill_table[obj->value[4]].name, ch );
                    send_to_char ( "'", ch );
                }

                send_to_char ( ".\n\r", ch );
                break;
            case ITEM_WAND:
            case ITEM_STAFF:
                sprintf ( buf, "Has %ld(%ld) charges of level %ld",
                          obj->value[1], obj->value[2], obj->value[0] );
                send_to_char ( buf, ch );
                if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
                {
                    send_to_char ( " '", ch );
                    send_to_char ( skill_table[obj->value[3]].name, ch );
                    send_to_char ( "'", ch );
                }

                send_to_char ( ".\n\r", ch );
                break;
            case ITEM_DRINK_CON:
                sprintf ( buf, "It holds %s-colored %s.\n\r",
                          liq_table[obj->value[2]].liq_color,
                          liq_table[obj->value[2]].liq_name );
                send_to_char ( buf, ch );
                break;
            case ITEM_WEAPON:
                send_to_char ( "Weapon type is ", ch );
                switch ( obj->value[0] )
                {
                    case ( WEAPON_EXOTIC ):
                        send_to_char ( "exotic\n\r", ch );
                        break;
                    case ( WEAPON_SWORD ):
                        send_to_char ( "sword\n\r", ch );
                        break;
                    case ( WEAPON_DAGGER ):
                        send_to_char ( "dagger\n\r", ch );
                        break;
                    case ( WEAPON_SPEAR ):
                        send_to_char ( "spear/staff\n\r", ch );
                        break;
                    case ( WEAPON_MACE ):
                        send_to_char ( "mace/club\n\r", ch );
                        break;
                    case ( WEAPON_AXE ):
                        send_to_char ( "axe\n\r", ch );
                        break;
                    case ( WEAPON_FLAIL ):
                        send_to_char ( "flail\n\r", ch );
                        break;
                    case ( WEAPON_WHIP ):
                        send_to_char ( "whip\n\r", ch );
                        break;
                    case ( WEAPON_POLEARM ):
                        send_to_char ( "polearm\n\r", ch );
                        break;
                    default:
                        send_to_char ( "unknown\n\r", ch );
                        break;
                }
                if ( obj->pIndexData->new_format )
                    sprintf ( buf, "Damage is %ldd%ld (average %ld)\n\r",
                              obj->value[1], obj->value[2],
                              ( 1 + obj->value[2] ) * obj->value[1] / 2 );
                else
                    sprintf ( buf, "Damage is %ld to %ld (average %ld)\n\r",
                              obj->value[1], obj->value[2],
                              ( obj->value[1] + obj->value[2] ) / 2 );
                send_to_char ( buf, ch );
                /*      sprintf(buf,"Weapon type is %s.\n\r",
                   (obj->value[3] > 0 && obj->value[3] <
                   MAX_DAMAGE_MESSAGE) ?
                   attack_table[obj->value[3]].noun : "undefined");
                   send_to_char(buf,ch);
                 */
                if ( obj->value[4] )    /* weapon flags */
                {
                    sprintf ( buf, "Weapons flags: %s\n\r",
                              weapon_bit_name ( obj->value[4] ) );
                    send_to_char ( buf, ch );
                }
                break;
            case ITEM_ARMOR:
                sprintf ( buf,
                          "Armor class is %ld pierce, %ld bash, %ld slash, and %ld vs. magic\n\r",
                          obj->value[0], obj->value[1], obj->value[2],
                          obj->value[3] );
                send_to_char ( buf, ch );
                break;
            case ITEM_CONTAINER:
                sprintf ( buf,
                          "Capacity: %ld#  Maximum weight: %ld# flags: %s\n\r",
                          obj->value[0], obj->value[3],
                          cont_bit_name ( obj->value[1] ) );
                send_to_char ( buf, ch );
                if ( obj->value[4] != 100 )
                {
                    sprintf ( buf, "Weight multiplier: %ld%%\n\r",
                              obj->value[4] );
                    send_to_char ( buf, ch );
                }
                break;
        }

        if ( obj->extra_descr != NULL ||
             obj->pIndexData->extra_descr != NULL )
        {
            EXTRA_DESCR_DATA *ed;
            send_to_char ( "Extra description keywords: '", ch );
            for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
            {
                send_to_char ( ed->keyword, ch );
                if ( ed->next != NULL )
                    send_to_char ( " ", ch );
            }

            for ( ed = obj->pIndexData->extra_descr; ed != NULL;
                  ed = ed->next )
            {
                send_to_char ( ed->keyword, ch );
                if ( ed->next != NULL )
                    send_to_char ( " ", ch );
            }

            send_to_char ( "'\n\r", ch );
        }

        for ( paf = obj->affected; paf != NULL; paf = paf->next )
        {
            sprintf ( buf, "Affects %s by %d, level %d",
                      affect_loc_name ( paf->location ), paf->modifier,
                      paf->level );
            send_to_char ( buf, ch );
            if ( paf->duration > -1 )
                sprintf ( buf, ", %d hours.\n\r", paf->duration );
            else
                sprintf ( buf, ".\n\r" );
            send_to_char ( buf, ch );
            if ( paf->bitvector )
            {
                switch ( paf->where )
                {
                    case TO_AFFECTS:
                        sprintf ( buf, "Adds %s affect.\n",
                                  affect_bit_name ( paf->bitvector ) );
                        break;
                    case TO_WEAPON:
                        sprintf ( buf, "Adds %s weapon flags.\n",
                                  weapon_bit_name ( paf->bitvector ) );
                        break;
                    case TO_OBJECT:
                        sprintf ( buf, "Adds %s object flag.\n",
                                  extra_bit_name ( paf->bitvector ) );
                        break;
                    case TO_IMMUNE:
                        sprintf ( buf, "Adds immunity to %s.\n",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    case TO_RESIST:
                        sprintf ( buf, "Adds resistance to %s.\n\r",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    case TO_VULN:
                        sprintf ( buf, "Adds vulnerability to %s.\n\r",
                                  imm_bit_name ( paf->bitvector ) );
                        break;
                    default:
                        sprintf ( buf, "Affect bit %d: %ld\n\r", paf->where,
                                  paf->bitvector );
                        break;
                }
                send_to_char ( buf, ch );
            }
        }

        if ( !obj->enchanted )
            for ( paf = obj->pIndexData->affected; paf != NULL;
                  paf = paf->next )
            {
                sprintf ( buf, "Affects %s by %d, level %d.\n\r",
                          affect_loc_name ( paf->location ), paf->modifier,
                          paf->level );
                send_to_char ( buf, ch );
                if ( paf->bitvector )
                {
                    switch ( paf->where )
                    {
                        case TO_AFFECTS:
                            sprintf ( buf, "Adds %s affect.\n",
                                      affect_bit_name ( paf->bitvector ) );
                            break;
                        case TO_OBJECT:
                            sprintf ( buf, "Adds %s object flag.\n",
                                      extra_bit_name ( paf->bitvector ) );
                            break;
                        case TO_IMMUNE:
                            sprintf ( buf, "Adds immunity to %s.\n",
                                      imm_bit_name ( paf->bitvector ) );
                            break;
                        case TO_RESIST:
                            sprintf ( buf, "Adds resistance to %s.\n\r",
                                      imm_bit_name ( paf->bitvector ) );
                            break;
                        case TO_VULN:
                            sprintf ( buf, "Adds vulnerability to %s.\n\r",
                                      imm_bit_name ( paf->bitvector ) );
                            break;
                        default:
                            sprintf ( buf, "Unknown bit %d: %ld\n\r",
                                      paf->where, paf->bitvector );
                            break;
                    }
                    send_to_char ( buf, ch );
                }
            }
    }
    return;
}


/*
 * Thanks to Zrin for auto-exit part.
 *
 */

CH_CMD ( do_exits )
{
    char buf[MAX_STRING_LENGTH * 100];
    EXIT_DATA *pexit;
    bool found;
    bool round;
    bool fAuto;
    int door;
    int outlet;
    fAuto = !str_cmp ( argument, "auto" );
    if ( !check_blind ( ch ) )
        return;
    if ( fAuto )
        sprintf ( buf, "[Exits:" );
    else if ( IS_IMMORTAL ( ch ) )
        sprintf ( buf, "Obvious exits from room %ld:\n\r",
                  ch->in_room->vnum );
    else
        sprintf ( buf, "Obvious exits:\n\r" );
    found = FALSE;
    for ( door = 0; door < 6; door++ )
    {
        round = FALSE;
        outlet = door;
        if ( ( ch->alignment < 0 ) &&
             ( pexit = ch->in_room->exit[door + 6] ) != NULL )
            outlet += 6;
        if ( ( pexit = ch->in_room->exit[outlet] ) != NULL &&
             pexit->u1.to_room != NULL &&
             can_see_room ( ch, pexit->u1.to_room ) &&
             !IS_SET ( pexit->exit_info, EX_CLOSED ) )
        {
            found = TRUE;
            round = TRUE;
            if ( fAuto )
            {
                strcat ( buf, " " );
                strcat ( buf, dir_name[outlet] );
            }
            else
            {
/*                sprintf ( buf + strlen ( buf ), "%-5s{x - %s",
                          capitalize ( dir_name[outlet] ),
                          room_is_dark ( pexit->u1.
                                         to_room ) ? "Too dark to tell" :
                          pexit->u1.to_room->name ); */
                sprintf ( buf + strlen ( buf ), "%-5s{x - %s",
                          dir_name[outlet],
                          room_is_dark ( pexit->u1.
                                         to_room ) ? "Too dark to tell" :
                          pexit->u1.to_room->name );
                if ( IS_IMMORTAL ( ch ) )
                    sprintf ( buf + strlen ( buf ), " (room %ld)\n\r",
                              pexit->u1.to_room->vnum );
                else
                    sprintf ( buf + strlen ( buf ), "\n\r" );
            }
        }
        if ( !round )
        {
            OBJ_DATA *portal;
            ROOM_INDEX_DATA *to_room;
            portal = get_obj_exit ( dir_name[door], ch->in_room->contents );
            if ( portal != NULL )
            {
                found = TRUE;
                round = TRUE;
                if ( fAuto )
                {
                    strcat ( buf, " " );
                    strcat ( buf, dir_name[door] );
                }
                else
                {
                    to_room = get_room_index ( portal->value[0] );
                    sprintf ( buf + strlen ( buf ), "%-5s {x- %s",
                              capitalize ( dir_name[door] ),
                              room_is_dark ( to_room ) ? "Too dark to tell" :
                              to_room->name );
                    if ( IS_IMMORTAL ( ch ) )
                        sprintf ( buf + strlen ( buf ), " (room %ld)\n\r",
                                  to_room->vnum );
                    else
                        sprintf ( buf + strlen ( buf ), "\n\r" );
                }
            }
        }
    }

    if ( !found )
        strcat ( buf, fAuto ? " none" : "None.\n\r" );
    if ( fAuto )
        strcat ( buf, "]\n\r" );
    send_to_char ( buf, ch );
    return;
}

CH_CMD ( do_worth )
{
    char buf[MAX_STRING_LENGTH];
    if ( IS_NPC ( ch ) )
    {
        sprintf ( buf,
                  "{yYou have {C%ld {yplatinum, {Y%ld{y gold and {W%ld{y silver.{x\n\r",
                  ch->platinum, ch->gold, ch->silver );
        send_to_char ( buf, ch );
        return;
    }

    sprintf ( buf,
              "{yYou have {C%ld{y platinum, {Y%ld {ygold, {W%ld{y silver.\n\r{yYou have {G%ld{y experience.{W({G%ld{y exp to level{W){y\n\r{yYour bank balance is {C%ld{y platinum.{x\n\r{yYou have {W%ld{y quest points.{x\n\r",
              ch->platinum, ch->gold, ch->silver, ch->exp,
              ( ch->level + 1 ) * exp_per_level ( ch,
                                                  ch->pcdata->points ) -
              ch->exp, ch->pcdata->balance, ch->pcdata->questpoints );
    send_to_char ( buf, ch );
    return;
}

CH_CMD ( do_charinfo )
{
    int i;
    printf_to_char ( ch,
                     "\n\r                           Character Info{x\n\r" );
    printf_to_char ( ch,
                     "{W-------------------------------------------------------------------------{x\n\r" );
    printf_to_char ( ch,
                     "{W|                            {GStats{x\n\r"
                     "{W| {RCon{x:{b %d{y({B%d{y) {RInt:{b %d{y({B%d{y) {RWis{x:{b %d{y({B%d{y){x"
                     "{RDex{x:{b %d{y({B%d{y) {RStr{x:{b %d{y({B%d{y){x\n\r",
                     ch->perm_stat[STAT_CON], get_curr_stat ( ch, STAT_CON ),
                     ch->perm_stat[STAT_INT], get_curr_stat ( ch, STAT_INT ),
                     ch->perm_stat[STAT_WIS], get_curr_stat ( ch, STAT_WIS ),
                     ch->perm_stat[STAT_DEX], get_curr_stat ( ch, STAT_DEX ),
                     ch->perm_stat[STAT_STR], get_curr_stat ( ch,
                                                              STAT_STR ) );
    printf_to_char ( ch,
                     "{W|\n\r{W| {G                           Armor{x\n\r" );
    if ( ch->level >= 1 )
    {
        printf_to_char ( ch,
                         "{W| {RPierce{x:{B %d{R   Bash{x:{B %d{x   {RSlash {x:{B %d{R   Magic{x:{B %d{x\n\r",
                         GET_AC ( ch, AC_PIERCE ), GET_AC ( ch, AC_BASH ),
                         GET_AC ( ch, AC_SLASH ), GET_AC ( ch, AC_EXOTIC ) );
    }
    for ( i = 0; i < 4; i++ )
    {
        char *temp;

        switch ( i )
        {
            case ( AC_PIERCE ):
                temp = "piercing";
                break;
            case ( AC_BASH ):
                temp = "bashing";
                break;
            case ( AC_SLASH ):
                temp = "slashing";
                break;
            case ( AC_EXOTIC ):
                temp = "magic";
                break;
            default:
                temp = "error";
                break;
        }
    }

    printf_to_char ( ch,
                     "{W---------------------------------------------------------------------------{x\n\r" );
}

/* 
New score function 
copyright of Loki of Death Rising.
If you use this score you must leave this header intact.*/
CH_CMD ( do_score )
{
    if ( IS_NPC ( ch ) ) return;

    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }
    int i;
    printf_to_char ( ch,
                     "\n\r{R=============================================================================={x\n\r" );
    printf_to_char ( ch, "{R|                   {x%s%s{x\n\r", IS_NPC ( ch ) ? ch->name : ch->pcdata->cname,
                     IS_NPC ( ch ) ? ", the mobile." : ch->pcdata->title );
    printf_to_char ( ch,
                     "{R+-----------------------------------------------------------------------------{x\n\r" );
    printf_to_char ( ch,
                     "{R| {rRace {x:{D %-15s{R| {rAQP{x :{D %-8ld{R| {rHP{x     :{D %-6ld{x/{D%ld\n\r"
                     "{R| {rAge  {x:{D %-15d{R| {rIQP{x :{D %-8d{R| {rMN{x     :{D %-6ld{x/{D%ld\n\r"
                     "{R| {rSex  {x:{D %-15s{R| {rDamr{x:{D %-8d{R| {rMV{x     :{D %-6ld{x/{D%ld\n\r"
                     "{R| {rLevel{x:{D %-15d{R| {rHitr{x:{D %-8d{R| {rItems{x  :{D %-6d{x/{D%d\n\r"
                     "{R| {rClass{x:{D %-15s{R| {rPlat{x:{D %-8ld{R| {rWeight{x :{D %-6ld{x/{D%d\n\r"
                     "{R| {rAlign{x:{D %-15d{R| {rGold{x:{D %-8ld{R| {rSaves{x  :{D %-6d{x\n\r"
                     "{R| {rWimpy{x:{D %-15d{R| {rSilv{x:{D %-8ld{R| {rBank{x   :{D %ld{x\n\r",
                     IS_NPC ( ch ) ? "mobile" : race_table[ch->race].name, 
		     ch->pcdata->questpoints, ch->hit, ch->max_hit, get_age ( ch ), 
		     ch->qps, ch->mana, ch->max_mana, 
		     ch->sex == 0 ? "Sexless" : ch->sex == 1 ? "Male" : "Female", 
		     GET_DAMROLL ( ch ), ch->move, ch->max_move, ch->level, 
		     GET_HITROLL ( ch ), ch->carry_number, can_carry_n ( ch ), 
		     class_table[ch->class].name, ch->platinum, get_carry_weight ( ch ) / 10, can_carry_w ( ch ) / 10, ch->alignment, ch->gold, ch->saving_throw, ch->wimpy, 
		     ch->silver, ch->pcdata->balance );
   printf_to_char ( ch,
		     "{R| {rTrain{x:{D %-15d{R| {rPrac{x:{D %-8d{R| {rHours  {x:{D %d{x\n\r",
		     ch->train, ch->practice, ( ch->played + ( int ) ( current_time - ch->logon ) ) / 3600 );
/*    printf_to_char ( ch,                
"{R+-----------------------------------------------------------------------------{x\n\r" ); */
/* Doesn't work...hmm */ /*
if ( ch->pcdata->condition[COND_FULL] >= 0 )
{
   printf_to_char ( ch,
		     "{R| {rYour hunger is at {D%d{r, thirst {D%d{r, and you're {D%d{d% {rfull.{x\n\r",
	ch->pcdata->condition[COND_HUNGER],
	ch->pcdata->condition[COND_THIRST],
	ch->pcdata->condition[COND_FULL] );
}
if ( ch->pcdata->condition[COND_DRUNK] > 0 )
   printf_to_char ( ch,
		     "{R| {rYou are {Gd{gr{Gu{gn{Gk{g! {r({D%d{d%{r){x\n\r", ch->pcdata->condition[COND_DRUNK] );
*/

/*stats*/
    printf_to_char ( ch,
                     "{R+-----------------------------------------------------------------------------{x\n\r" );
    printf_to_char ( ch,
                     "{R| {rStr{x:{D %d{x({d%d{x) {rInt{x: {D%d{x({d%d{x) {rWis{x: {D%d{x({d%d{x) {rDex{x: {D%d{x({d%d{x) {rCon{x: {D%d{x({d%d{x)\n\r",
                     ch->perm_stat[STAT_STR], get_curr_stat ( ch, STAT_STR ),
                     ch->perm_stat[STAT_INT], get_curr_stat ( ch, STAT_INT ),
                     ch->perm_stat[STAT_WIS], get_curr_stat ( ch, STAT_WIS ),
                     ch->perm_stat[STAT_DEX], get_curr_stat ( ch, STAT_DEX ),
                     ch->perm_stat[STAT_CON], get_curr_stat ( ch, STAT_CON ) );
     /*AC*/ if ( ch->level >= 1 )
    {
        printf_to_char ( ch,
                         "{R| {rPierce{x:{D %d{r   Bash{x:{D %d{x   {rSlash {x:{D %d{r   Magic{x:{D %d{x\n\r",
                         GET_AC ( ch, AC_PIERCE ), GET_AC ( ch, AC_BASH ),
                         GET_AC ( ch, AC_SLASH ), GET_AC ( ch, AC_EXOTIC ) );
    }
    for ( i = 0; i < 4; i++ )
    {
        char *temp;
        switch ( i )
        {
            case ( AC_PIERCE ):
                temp = "piercing";
                break;
            case ( AC_BASH ):
                temp = "bashing";
                break;
            case ( AC_SLASH ):
                temp = "slashing";
                break;
            case ( AC_EXOTIC ):
                temp = "magic";
                break;
            default:
                temp = "error";
                break;
        }
    }
    printf_to_char ( ch,
                     "{R+-----------------------------------------------------------------------------{x\n\r" );
    /* RT wizinvis and holy light */
    if ( IS_IMMORTAL ( ch ) )
    {
        printf_to_char ( ch, "{R| {rHoly Light{x: " );
        if ( IS_SET ( ch->act, PLR_HOLYLIGHT ) )
            printf_to_char ( ch, "{Don{x" );
        else
            printf_to_char ( ch, "{Doff{x" );
    }
    if ( IS_IMMORTAL ( ch ) )
    {
        printf_to_char ( ch,
                         "{r   Invis{x:{D %d   {rIncognito{x:{D %d   {rGhost{x:{D %d\n\r",
                         ch->invis_level, ch->incog_level, ch->ghost_level );
        printf_to_char ( ch,
                         "{R+-----------------------------------------------------------------------------{x\n\r" );
    }
    if ( !IS_NPC ( ch ) )
    {
        printf_to_char ( ch,
                         "{R| {d[{rVersusMobs{d]{r Kills{x: {D%d  {rDeaths{x:{D %d{x\n\r",
                         ch->pcdata->mkill, ch->pcdata->mdeath );
    }
    if ( !IS_NPC ( ch ) )
    {
        printf_to_char ( ch,
                         "{R| {d[{rTournament{d]{r Wins{x: {D%d  {rKills{x:{D %d  {rDeaths{x: {D%d{x\n\r",
                         ch->pcdata->twins, ch->pcdata->tkills, ch->pcdata->tdeaths );
    }
    if ( !IS_NPC ( ch ) )
    {
        printf_to_char ( ch,
                         "{R| {d[{r1vs1 Arena{d]{r Wins{x:{D %d  {rLosses{x:{D %d{x\n\r",
                         ch->pcdata->awins, ch->pcdata->alosses );
        if ( ch->challenger != NULL )
        {
            printf_to_char ( ch,
                             "{R| {d[{r1vs1 Arena{d]{r You have been challenged by{r %s{c.{x\n\r",
                             ch->challenger->name );
        }
        if ( ch->gladiator != NULL )
        {
            printf_to_char ( ch,
                             "{R| {d[{rArena bet{d]{r You have a {Y%d platinum{r bet on %s.{x\n\r",
                             ch->pcdata->plr_wager, ch->gladiator->name );
        }
    }

    if ( !IS_NPC ( ch ) )
    {
        printf_to_char ( ch,
                         "{R| {d[{rPlayerkill{d]{r Kills{x: {D%ld  {rDeaths{x: {D%ld{x\n\r",
                         ch->pcdata->pkkills, ch->pcdata->pkdeaths );
    }
        printf_to_char ( ch,
                         "{R+-----------------------------------------------------------------------------{x\n\r" );
    
    if ( is_clan ( ch ) )
    {
        printf_to_char ( ch, "{R| {rClan{x: %s {rRank{x: %s{x, %s {x\n\r",
                         clan_table[ch->clan].who_name,
                         clan_rank_table[ch->clan_rank].title_of_rank[ch->
                                                                      sex],
                         is_pkill ( ch ) ? "{RPkill{c.{x" :
                         "{mNon-Pkill{c.{x" );
    }
    if ( ch->invited )
    {
        printf_to_char ( ch,
                         "{R| {RYou have been invited to join clan {x[{%s%s{x]\n\r",
                         clan_table[ch->invited].pkill ? "B" : "M",
                         clan_table[ch->invited].who_name );
    }
    if ( !IS_NPC ( ch ) )
    {
        int rcnt = roomcount ( ch );
        int arcnt = areacount ( ch );
        double rooms = top_room, percent = ( double ) rcnt / ( rooms / 100 );
        double arooms = ( double ) ( arearooms ( ch ) ), apercent =
            ( double ) arcnt / ( arooms / 100 );
        printf_to_char ( ch,
                         "{R| {rExplored{x: {D%4d{c/{d%4d{r rooms. {r({d%5.2f%%{r of the world{r) ({d%5.2f%%{r of current area)\n\r{x",
                         rcnt, top_room, percent, apercent );
    }

        printf_to_char ( ch,
                         "{R+-----------------------------------------------------------------------------{x\n\r" );

if ( ch->level < LEVEL_HERO && !IS_SET ( ch->act2, PLR2_NO_EXP ) )
   printf_to_char ( ch,
		     "{R| {rYou have gained a total of {D%ld{r experience, and need {D%ld{r to level.{x\n\r", 
ch->exp, ( ch->level + 1 ) * exp_per_level ( ch, ch->pcdata->points ) - ch->exp );

	if ( ch->pcdata->timesplayed == 1 )
	    printf_to_char ( ch, "{R|{r This is the first time ever you've played on Shattered Dreams!{x\n\r" );
	else
	    printf_to_char ( ch, "{R|{r You have played on Shattered Dreams a total of {D%d {rtimes.{x\n\r", ch->pcdata->timesplayed );



	if ( ch->pcdata->times_rerolled <= 0 )
	{
	   printf_to_char ( ch, "{R| {rYou haven't rerolled yet. Type 'help reroll' for more info.{x\n\r" );
	}
	else if ( ch->pcdata->times_rerolled == 1 )
	{
	   printf_to_char ( ch, "{R| {rYou have rerolled once so far.{x\n\r" );
	}
	else
	{
	   printf_to_char ( ch, "{R| {rYou have rerolled a total of {D%d{r times so far.{x\n\r", ch->pcdata->times_rerolled );
	}

    if ( IS_MARRIED ( ch ) )
        printf_to_char ( ch, "{R| {rYou are happily married to {D%s{r.{x\n\r", ch->pcdata->spouse );

    if ( ch->pcdata->exiletime > 0 )
    {
    	printf_to_char ( ch, "{R| {rYou have {D%d {rminutes of {YEXILE {rremaining. {d({D%d hours{d){r.{x\n\r",
	ch->pcdata->exiletime, ( ch->pcdata->exiletime / 60 ) );
    }	
    if ( ch->pcdata->noclantime > 0 )
    {
    	printf_to_char ( ch, "{R| {rYou have {D%d {rminutes of {Yno-clan {rremaining. {d({D%d hours{d){r.{x\n\r",
	ch->pcdata->noclantime, ( ch->pcdata->noclantime / 60 ) );
    }	
        if ( ch->pcdata->questmob == -1 || has_questobj ( ch ) )    /* killed target mob */
            send_to_char ( 
                      "{R| {rYour quest is {fALMOST{x{r complete! {rGet back to your questmaster before your time runs out!{x\n\r", ch );

        else if ( ch->pcdata->questobj > 0 )    /* questing for an object */
        {
	    OBJ_INDEX_DATA *questinfoobj;
            questinfoobj = get_obj_index ( ch->pcdata->questobj );
            if ( questinfoobj != NULL )
                printf_to_char ( ch,
                          "{R| {rYou are on a quest to recover the fabled %s{r!{x\n\r",
                          questinfoobj->short_descr );
	}
        else if ( ch->pcdata->questmob > 0 )    /* questing for a mob */
        {
	    MOB_INDEX_DATA *questinfo;
            questinfo = get_mob_index ( ch->pcdata->questmob );
            if ( questinfo != NULL )
                printf_to_char ( ch,
                          "{R| {rYou are on a quest to slay the dreaded {D%s{r!{x\n\r",
                          questinfo->short_descr );

	}

        if ( !IS_SET ( ch->act, PLR_QUESTOR ) )
        {
//            send_to_char ( "{R| {rYou aren't currently on a quest.{x\n\r", ch );
            if ( ch->pcdata->nextquest > 1 )
                printf_to_char ( ch,
                          "{R| {rThere are {D%d {rminutes remaining until you can go on another quest.{x\n\r",
                          ch->pcdata->nextquest );

            else if ( ch->pcdata->nextquest == 1 )
                printf_to_char ( ch,
                          "{R| {rThere is less than a minute remaining until you can go on another quest.{x\n\r" );
	}
        else if ( ch->pcdata->countdown > 0 )
            printf_to_char ( ch, "{R| {rTime left for current quest: {D%d{x\n\r",
                      ch->pcdata->countdown );

    printf_to_char ( ch,
                     "{R=============================================================================={x\n\r" );
    if ( IS_SET ( ch->comm, COMM_SHOW_AFFECTS ) )
        do_affects ( ch, "" );
}

CH_CMD ( do_affects )
{
    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];
    char *buf4;
    char buf3[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    bool found = FALSE;
    long cheat = 0;
    long filter;
    long printme;
//    BUFFER *buffer;
    BUFFER2 *buffer;
    OBJ_DATA *obj;
    int iWear;
    //buffer = new_buf (  );
    buffer = buffer_new ( 1337 );
    cheat = ch->affected_by;
    if ( ch->affected != NULL )
    {
        send_to_char
            ( "{R==============================================================================\n\r{x",
              ch );
        //add_buf ( buffer, "{dYou are affected by the following spells:{x\n\r" );
        buffer_strcat ( buffer, "{dYou are affected by the following spells:{x\n\r" );
        for ( paf = ch->affected; paf != NULL; paf = paf->next )
        {
            if ( paf_last != NULL && paf->type == paf_last->type )
            {
                if ( ch->level >= 20 )
                    sprintf ( buf, "                          " );
                else
                    continue;
            }
            else
                sprintf ( buf, "{rSpell{x: {D%-19s{x",
                          skill_table[paf->type].name );
            //add_buf ( buffer, buf );
	    buffer_strcat ( buffer, buf );
            if ( IS_SET ( cheat, paf->bitvector ) )
                cheat -= paf->bitvector;
            if ( ch->level >= 20 )
            {
                sprintf ( buf, "{x: {rmodifies {D%-16s {rby {D%-6d{x ",
                          affect_loc_name ( paf->location ), paf->modifier );
                //add_buf ( buffer, buf );
		buffer_strcat ( buffer, buf );
                if ( paf->duration == -1 )
                    sprintf ( buf, "{Dpermanently{x" );
                else
                    sprintf ( buf, "{rfor{D %-4d {rhours{x", paf->duration );
                //add_buf ( buffer, buf );
		buffer_strcat ( buffer, buf );
            }

            //add_buf ( buffer, "\n\r" );
	    buffer_strcat ( buffer, "\n\r" );
            paf_last = paf;
        }
        found = TRUE;
        //add_buf ( buffer, "\n\r" );
	buffer_strcat ( buffer, "\n\r" );
    }
    if ( race_table[ch->race].aff != 0 &&
         IS_AFFECTED ( ch, race_table[ch->race].aff ) )
    {
/*        add_buf ( buffer, "{R------------------------------------------------------------------------------{x\n\r" );
        add_buf ( buffer, "{dYou are affected by the following racial abilities:{x\n\r" ); */
	buffer_strcat ( buffer, "{R------------------------------------------------------------------------------{x\n\r" );
        buffer_strcat ( buffer, "{dYou are affected by the following racial abilities:{x\n\r" );
	if ( IS_SET ( cheat, race_table[ch->race].aff ) );
        cheat -= race_table[ch->race].aff;
        strcpy ( buf3, affect_bit_name ( race_table[ch->race].aff ) );
        buf4 = buf3;
        buf4 = one_argument ( buf4, buf2 );
        while ( buf2[0] )
        {
            sprintf ( buf, "{rSpell: {D%-19s{x", buf2 );
            //add_buf ( buffer, buf );
	    buffer_strcat ( buffer, buf );
            //add_buf ( buffer, "\n\r" );
	    buffer_strcat ( buffer, "\n\r" );
            buf4 = one_argument ( buf4, buf2 );
        }
        found = TRUE;
        //add_buf ( buffer, "\n\r" );
	buffer_strcat ( buffer, "\n\r" );
    }
    if ( race_table[ch->race].shd != 0 &&
         IS_SHIELDED ( ch, race_table[ch->race].shd ) )
    {
/*        add_buf ( buffer, "{R------------------------------------------------------------------------------{x\n\r" );
        add_buf ( buffer, "{dYou are affected by the following racial shieldings:{x\n\r" ); */
        buffer_strcat ( buffer, "{R------------------------------------------------------------------------------{x\n\r" );
        buffer_strcat ( buffer, "{dYou are affected by the following racial shieldings:{x\n\r" );
        if ( IS_SET ( cheat, race_table[ch->race].shd ) );
        cheat -= race_table[ch->race].shd;
        strcpy ( buf3, shield_bit_name ( race_table[ch->race].shd ) );
        buf4 = buf3;
        buf4 = one_argument ( buf4, buf2 );
        while ( buf2[0] )
        {
            sprintf ( buf, "{rSpell: {D%-19s{x", buf2 );
            //add_buf ( buffer, buf );
	    buffer_strcat ( buffer, buf );
            //add_buf ( buffer, "\n\r" );
	    buffer_strcat ( buffer, "\n\r" );
            buf4 = one_argument ( buf4, buf2 );
        }
        found = TRUE;
        //add_buf ( buffer, "\n\r" );
	buffer_strcat ( buffer, "\n\r" );
    }    
    if ( ch->affected_by != 0 &&
         ( ch->affected_by != race_table[ch->race].aff ) )
    {
        bool print = FALSE;
        for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
        {
            if ( ( obj = get_eq_char ( ch, iWear ) ) != NULL )
            {
                for ( paf = obj->affected; paf != NULL; paf = paf->next )
                {
                    if ( !IS_SET ( ch->affected_by, paf->bitvector ) )
                        continue;
                    if ( paf->where != TO_AFFECTS )
                        continue;
                    filter = paf->bitvector;
                    filter &= ch->affected_by;
                    printme = filter;
                    filter &= cheat;
                    cheat -= filter;
                    if ( !print )
                    {
/*                        add_buf ( buffer, "{R------------------------------------------------------------------------------{x\n\r" );
                        add_buf ( buffer, "{dYou are affected by the following equipment spells:{x\n\r" ); */
                        buffer_strcat ( buffer, "{R------------------------------------------------------------------------------{x\n\r" );
                        buffer_strcat ( buffer, "{dYou are affected by the following equipment spells:{x\n\r" );
                        print = TRUE;
                    }

                    strcpy ( buf3, affect_bit_name ( printme ) );
                    buf4 = buf3;
                    buf4 = one_argument ( buf4, buf2 );
                    while ( buf2[0] )
                    {
                        sprintf ( buf, "{rSpell: {D%-19s{x:{x %s", buf2,
                                  obj->short_descr );
                        //add_buf ( buffer, buf );
			buffer_strcat ( buffer, buf );
                        //add_buf ( buffer, "\n\r" );
			buffer_strcat ( buffer, "\n\r" );
                        buf4 = one_argument ( buf4, buf2 );
                    }
                }
                if ( !obj->enchanted )
                {
                    for ( paf = obj->pIndexData->affected; paf != NULL;
                          paf = paf->next )
                    {
                        if ( !IS_SET ( ch->affected_by, paf->bitvector ) )
                            continue;
                        if ( paf->where != TO_AFFECTS )
                            continue;
                        filter = paf->bitvector;
                        filter &= ch->affected_by;
                        printme = filter;
                        filter &= cheat;
                        cheat -= filter;
                        if ( !print )
                        {
                            //add_buf ( buffer, "{dYou are affected by the following equipment spells:{x\n\r" );
                            buffer_strcat ( buffer, "{dYou are affected by the following equipment spells:{x\n\r" );
                            print = TRUE;
                        }

                        strcpy ( buf3, affect_bit_name ( printme ) );
                        buf4 = buf3;
                        buf4 = one_argument ( buf4, buf2 );
                        while ( buf2[0] )
                        {
                            sprintf ( buf, "{rSpell: {D%-19s:{x %s", buf2,
                                      obj->short_descr );
                            //add_buf ( buffer, buf );
			    buffer_strcat ( buffer, buf );
                            //add_buf ( buffer, "\n\r" );
			    buffer_strcat ( buffer, "\n\r" );
                            buf4 = one_argument ( buf4, buf2 );
                        }
                    }
                }
/*                if ( !print )
                {
                   buffer_strcat ( buffer, "{R------------------------------------------------------------------------------{x\n\r" );
                   buffer_strcat ( buffer, "{dYou are affected by the following shieldings:{x\n\r" );
			if ( IS_SHIELDED ( ch, SHD_INVISIBLE ) )
			buffer_strcat ( buffer, "Invisibility\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_ICE ) )
			buffer_strcat ( buffer, "Iceshield\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_FIRE ) )
			buffer_strcat ( buffer, "Fireshield\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_SHOCK ) )
			buffer_strcat ( buffer, "Shockshield\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_ACID ) )
			buffer_strcat ( buffer, "Acidshield\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_POISON ) )
			buffer_strcat ( buffer, "Poisonshield\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_SANCTUARY ) )
			buffer_strcat ( buffer, "Sanctuary\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_BRIAR ) )
			buffer_strcat ( buffer, "Briarshield\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_PROTECT_VOODOO ) )
			buffer_strcat ( buffer, "Protection Voodoo\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_PROTECT_EVIL ) )
			buffer_strcat ( buffer, "Protection evil\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_PROTECT_GOOD ) )
			buffer_strcat ( buffer, "Protection good\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_DIVINE_PROTECTION ) )	
			buffer_strcat ( buffer, "Divine Protection\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_MANA ) )	
			buffer_strcat ( buffer, "Manashield\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_WARD ) )	
			buffer_strcat ( buffer, "Ward\n\r" );
        		if ( IS_SHIELDED ( ch, SHD_MISTBLEND ) )		     
			buffer_strcat ( buffer, "Mistblend\n\r" );
                   print = TRUE;
                } */
            }
        }

        found = TRUE;
        if ( print )
            //add_buf ( buffer, "\n\r" );
	    buffer_strcat ( buffer, "\n\r" );
    }                           /*
                                   if (cheat != 0)
                                   {
                                   add_buf(buffer,"{Y------------------------------------------------------------------------------{x\n\r");
                                   add_buf(buffer,"{cYou are affected by the following immortal abilities:{x\n\r");
                                   strcpy(buf3,affect_bit_name(cheat));
                                   buf4=buf3;
                                   buf4= one_argument(buf4, buf2);
                                   while (buf2[0])
                                   {
                                   sprintf( buf, "{cSpell: {W%-19s{x",buf2);
                                   add_buf(buffer, buf );
                                   add_buf(buffer,"\n\r");
                                   buf4 = one_argument(buf4,buf2);
                                   }
                                   found=TRUE;
                                   add_buf(buffer,"\n\r");
                                   } */

    if ( !found )
        send_to_char ( "{dYou are not affected by any spells.{x\n\r", ch );
    else
    {
        //page_to_char ( buf_string ( buffer ), ch );
	page_to_char ( buffer_string ( buffer ), ch );
        //free_buf ( buffer );
	buffer_free ( buffer );
    }
    return;
}

char *const day_name[] = {
    "the Moon",
    "the Bull",
    "Deception",
    "Thunder",
    "Freedom",
    "the Great Gods",
    "the Sun"
};
char *const month_name[] = {
    "Winter",
    "the Winter Wolf",
    "the Frost Giant",
    "the Old Forces",
    "the Grand Struggle",
    "the Spring",
    "Nature",
    "Futility",
    "the Dragon",
    "the Sun",
    "the Heat",
    "the Battle",
    "the Dark Shades",
    "the Shadows",
    "the Long Shadows",
    "the Ancient Darkness",
    "the Great Evil"
};

CH_CMD ( do_time )
{
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;
    day = time_info.day + 1;
    if ( day > 4 && day < 20 )
        suf = "th";
    else if ( day % 10 == 1 )
        suf = "st";
    else if ( day % 10 == 2 )
        suf = "nd";
    else if ( day % 10 == 3 )
        suf = "rd";
    else
        suf = "th";
    sprintf ( buf,
              "{cIt is {W%d{c o'clock {W%s{c, Day of {W%s{c,{W %d%s{c the Month of{W %s{c.{x\n\r",
              ( time_info.hour % 12 == 0 ) ? 12 : time_info.hour % 12,
              time_info.hour >= 12 ? "pm" : "am", day_name[day % 7], day, suf,
              month_name[time_info.month] );
    send_to_char ( buf, ch );
    sprintf ( buf, "\n\r{cThe system time is {W%s{x\n\r", ( char * ) ctime ( &current_time ) );
    send_to_char ( buf, ch );
    return;
}

CH_CMD ( do_weather )
{
    char buf[MAX_STRING_LENGTH];
    static char *const sky_look[4] = {
        "cloudless",
        "cloudy",
        "rainy",
        "lit by flashes of lightning"
    };
    if ( !IS_OUTSIDE ( ch ) )
    {
        send_to_char ( "You can't see the weather indoors.\n\r", ch );
        return;
    }

    sprintf ( buf, "The sky is %s and %s.\n\r", sky_look[weather_info.sky],
              weather_info.change >=
              0 ? "a warm southerly breeze blows" :
              "a cold northern gust blows" );
    send_to_char ( buf, ch );
    return;
}

/* whois command */
CH_CMD ( do_whois )
{
    char arg[MAX_INPUT_LENGTH];
    //BUFFER *output;
    BUFFER2 *output;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char clandat[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
    one_argument ( argument, arg );
    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
	send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
	return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char ( "You must provide a name.\n\r", ch );
        return;
    }

    //output = new_buf (  );
    output = buffer_new ( 1337 );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
        char class[MAX_STRING_LENGTH];
        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;
        wch = ( d->original != NULL ) ? d->original : d->character;
        if ( !can_see ( ch, wch ) )
            continue;
        if ( !str_prefix ( arg, wch->name ) )
        {
            found = TRUE;
            /* work out the printing */
            sprintf ( buf2, "%3d", wch->level );
            switch ( wch->level )
            {
                case MAX_LEVEL - 0:
                    sprintf ( buf2, "{R216{x" );
                    break;
                case MAX_LEVEL - 1:
                    sprintf ( buf2, "{G215{x" );
                    break;
                case MAX_LEVEL - 2:
                    sprintf ( buf2, "{G214{x" );
                    break;
                case MAX_LEVEL - 3:
                    sprintf ( buf2, "{G213{x" );
                    break;
                case MAX_LEVEL - 4:
                    sprintf ( buf2, "{G212{x" );
                    break;
                case MAX_LEVEL - 5:
                    sprintf ( buf2, "{G211{x" );
                    break;
                case MAX_LEVEL - 6:
                    sprintf ( buf2, "{G210{x" );
                    break;
                case MAX_LEVEL - 7:
                    sprintf ( buf2, "{G209{x" );
                    break;
                case MAX_LEVEL - 8:
                    sprintf ( buf2, "{G208{x" );
                    break;
                case MAX_LEVEL - 9:
                    sprintf ( buf2, "{G207{x" );
                    break;
                case MAX_LEVEL - 10:
                    sprintf ( buf2, "{G206{x" );
                    break;
                case MAX_LEVEL - 11:
                    sprintf ( buf2, "{G205{x" );
                    break;
                case MAX_LEVEL - 12:
                    sprintf ( buf2, "{G204{x" );
                    break;
		case MAX_LEVEL - 13:
		    sprintf ( buf2, "{C203{x" );
		    break;
                case MAX_LEVEL - 14:
 		    if ( IS_SET ( class_table[wch->class].tier, TIER_01 ) )
                    sprintf ( buf2, "{GA{gnc{x" );
		    else if ( IS_SET ( class_table[wch->class].tier, TIER_02 ) )
		    sprintf ( buf2, "{YA{ync{x" );
		    else if ( IS_SET ( class_table[wch->class].tier, TIER_03 ) )
		    sprintf ( buf2, "{RA{rnc{x" );
		    else
		    sprintf ( buf2, "{BA{bnc{x" );
                    break;
                case MAX_LEVEL - 15:
                    sprintf ( buf2, "{YH{yro{x" );
                    break;
            }

            if ( IS_SET ( class_table[wch->class].tier, TIER_03 ) )
            {
                sprintf ( class, "{R%c{r%c%c{x",
                          class_table[wch->class].who_name[0],
                          class_table[wch->class].who_name[1],
                          class_table[wch->class].who_name[2] );
            }
            else if ( IS_SET ( class_table[wch->class].tier, TIER_02 ) )
            {
                sprintf ( class, "{Y%c{y%c%c{x",
                          class_table[wch->class].who_name[0],
                          class_table[wch->class].who_name[1],
                          class_table[wch->class].who_name[2] );
            }
            else if ( IS_SET ( class_table[wch->class].tier, TIER_04 ) )
            {
                sprintf ( class, "{B%c{b%c%c{x",
                          class_table[wch->class].who_name[0],
                          class_table[wch->class].who_name[1],
                          class_table[wch->class].who_name[2] );
            }
            else
            {
                sprintf ( class, "{G%c{g%c%c{x",
                          class_table[wch->class].who_name[0],
                          class_table[wch->class].who_name[1],
                          class_table[wch->class].who_name[2] );
            }

            /* a little formatting */
            if ( !is_clan ( wch ) )
            {
                sprintf ( clandat, "%s", clan_table[wch->clan].who_name );
            }
            else
            {
                if ( clan_table[wch->clan].independent )
                {
                    sprintf ( clandat, "{r[{x%s{r]{x ",
                              clan_table[wch->clan].who_name );
                }
                else if ( is_clead ( wch ) && is_pkill ( wch ) )
                {
                    sprintf ( clandat, "{R[{x%s{R]{x ",
                              clan_table[wch->clan].who_name );
                }
                else if ( is_clead ( wch ) && !is_pkill ( wch ) )
                {
                    sprintf ( clandat, "{M[{x%s{M]{x ",
                              clan_table[wch->clan].who_name );
                }
                else if ( is_pkill ( wch ) )
                {
                    sprintf ( clandat, "{r[{x%s{r]{x ",
                              clan_table[wch->clan].who_name );
                }
                else
                {
                    sprintf ( clandat, "{m[%s{m]{x ",
                              clan_table[wch->clan].who_name );
                }
            }
            sprintf ( buf, "{c<{C<{x%s %-6s %s {r[%s{r]{C>{c>{x %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n\r",
                      buf2,
                      wch->race <
                      MAX_PC_RACE ? pc_race_table[wch->race].
                      who_name : "     ", class,
		      wch->sex == 0 ? "{xN" : wch->sex == 1 ? "{RM" : "{MF",
//                ON_GQUEST(wch) ? "(GQuest)" : "",
                      ( ( wch->ghost_level >= LEVEL_ANCIENT ) &&
                        ( ch->level >= wch->level ) ) ? "(Ghost) " : "",
                      wch->incog_level >= LEVEL_ANCIENT ? "(Incog) " : "",
                      wch->invis_level >= LEVEL_ANCIENT ? "(Wizi) " : "",
                      clandat,
		      IS_SET ( wch->act2, PLR2_HELPER ) ? "({yHelper{x) " : "",
                      IS_SET ( wch->act2, PLR2_MASS_ARENA ) ? "({rVote{x) " : "", 
                      IS_SET ( wch->act2, PLR2_MASS_JOINER ) ? "({RTournament{x) " : "", 
		      IS_SET ( wch->comm, COMM_AFK ) ? "[{yAFK{x] " : "",
                      IS_SET ( wch->act2, PLR2_PKSAFE ) ? "({wSAFE{x) " : "",
                      IS_SET ( wch->act2, PLR2_PKVIOLENT ) ? "({RVIOLENT{x) " : "",
                      IS_SET ( wch->act, PLR_TWIT ) ? "({RTWIT{x) " : "",
		      IS_SET (wch->pcdata->tag_flags, TAG_RED) ? "({RRed{x) " : "",
		      IS_SET (wch->pcdata->tag_flags, TAG_BLUE) ? "({BBlue{x) " : "",
		      IS_SET (wch->pcdata->tag_flags, TAG_FROZEN) ? "({CFr{coz{Ce{cn{x) " : "",
                      wch->pcdata->cname, IS_NPC ( wch ) ? "" : wch->pcdata->title );

            //add_buf ( output, buf );
	    buffer_strcat ( output, buf );
//    if ( !IS_IMMORTAL ( wch ) )

/* New stuff by Rhaelar */
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
                     "{R====================={x\n\r",
                     wch->pcdata->cname, wch->pcdata->title,
                     wch->sex == 0 ? "Sexless" : wch->sex == 1 ? "Male" : "Female", wch->level,
		     get_age ( wch ),
                     class_table[wch->class].name,
                     pc_race_table[wch->race].name,
                     clan_table[wch->clan].name,
                     clan_rank_table[wch->clan_rank].title_of_rank[wch->sex],
		     IS_MARRIED ( wch ) ? wch->pcdata->spouse : "Not married",
		     wch->pcdata->pkkills, wch->pcdata->pkdeaths,
                     wch->pcdata->awins, wch->pcdata->alosses,
                     wch->pcdata->twins, wch->pcdata->tkills, wch->pcdata->tdeaths,
		     wch->pcdata->mkill, wch->pcdata->mdeath );
        printf_to_char ( ch, "\n\r" );
        }
    }

    if ( !found )
    {
        send_to_char ( "No one of that name is playing, or that person is linkdead.\n\r", ch );
        return;
    }

    //page_to_char ( buf_string ( output ), ch );
    page_to_char ( buffer_string ( output ), ch );
    //free_buf ( output );
    buffer_free ( output );
}

/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
CH_CMD ( do_who )
{
    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char clandat[MAX_STRING_LENGTH];
    char questdat[MAX_STRING_LENGTH];
    /*BUFFER *output;
    BUFFER *outputimm; */
    BUFFER2 *output;
    BUFFER2 *outputimm;
    DESCRIPTOR_DATA *d;
    int iClass;
    int iRace;
    int iClan;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_PC_RACE];
    bool rgfClan[MAX_CLAN];
    bool fClassRestrict = FALSE;
    bool fClanRestrict = FALSE;
    bool fClan = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;
    int count;
    int countimm;
    int hour;
            
    if ( IS_AFFECTED ( ch, AFF_BLIND ) )
    {
        send_to_char ( "You cant see a thing!\n\r", ch );
        return;
    }
        
    /*
     * Set default arguments.
     */
    iLevelLower = 0;
    iLevelUpper = MAX_LEVEL;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        rgfClass[iClass] = FALSE;
    for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
        rgfRace[iRace] = FALSE;
    for ( iClan = 0; iClan < MAX_CLAN; iClan++ )
        rgfClan[iClan] = FALSE;
    /*
     * Parse arguments.
     */
    count = 0;
    countimm = 0;
    nNumber = 0;
    for ( ;; )
    {
        char arg[MAX_STRING_LENGTH];
        argument = one_argument ( argument, arg );
        if ( arg[0] == '\0' )
            break;
        if ( is_number ( arg ) )
        {
            switch ( ++nNumber )
            {
                case 1:
                    iLevelLower = atoi ( arg );
                    break;
                case 2:
                    iLevelUpper = atoi ( arg );
                    break;
                default:
                    send_to_char ( "Only two level numbers allowed.\n\r",
                                   ch );
                    return;
            }
        }
        else
        {

            /*
             * Look for classes to turn on.
             */
            if ( !str_prefix ( arg, "immortals" ) )
            {
                fImmortalOnly = TRUE;
            }
            else
            {
                iClass = class_lookup ( arg );
                if ( iClass == -1 )
                {
                    iRace = race_lookup ( arg );
                    if ( iRace == 0 || iRace >= MAX_PC_RACE )
                    {
                        if ( !str_prefix ( arg, "clan" ) )
                            fClan = TRUE;
                        else
                        {
                            iClan = clan_lookup ( arg );
                            if ( iClan )
                            {
                                fClanRestrict = TRUE;
                                rgfClan[iClan] = TRUE;
                            }
                            else
                            {
                                send_to_char
                                    ( "That's not a valid race, class, or clan.\n\r",
                                      ch );
                                return;
                            }
                        }
                    }
                    else
                    {
                        fRaceRestrict = TRUE;
                        rgfRace[iRace] = TRUE;
                    }
                }
                else
                {
                    fClassRestrict = TRUE;
                    rgfClass[iClass] = TRUE;
                }
            }
        }
    }
       
    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
/*    output = new_buf (  );
    outputimm = new_buf (  ); */
    output = buffer_new ( 1337 );
    outputimm = buffer_new ( 1337 );
    
/*    add_buf ( outputimm, "{W[{bIMM{BORT{bALS{W]{x\n\r" );
    add_buf ( output, "{W[{rMOR{RT{rALS{W]{x\n\r" ); */
    buffer_strcat ( outputimm, "{W[{bIMM{BORT{bALS{W]{x\n\r" );
    buffer_strcat ( output, "{W[{rMOR{RT{rALS{W]{x\n\r" );
    for ( d = descriptor_list; d != NULL; d = d->next ) 
    {
        CHAR_DATA *wch;
        char class[MAX_STRING_LENGTH];
        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) ) 
/*        if ( !can_see ( ch, d->character ) ) */
            continue;
        wch = ( d->original != NULL ) ? d->original : d->character;
        if ( !can_see ( ch, wch ) && IS_SHIELDED ( wch, SHD_INVISIBLE ) )
            continue;
        if ( !can_see ( ch, wch ) && wch->level >= SQUIRE &&
             ch->level < wch->level )
            continue;
        if ( !IS_IMMORTAL ( wch ) )
            continue;
        if ( wch->level < iLevelLower || wch->level > iLevelUpper ||
             ( fImmortalOnly && wch->level < LEVEL_IMMORTAL ) ||
             ( fClassRestrict && !rgfClass[wch->class] ) || ( fRaceRestrict &&
                                                              !rgfRace[wch->
                                                                       race] )
             || ( fClan && !is_clan ( wch ) ) || ( fClanRestrict &&
                                                   !rgfClan[wch->clan] ) )
            continue;
        countimm++;
        nMatch++;
        /*
         * Figure out what to print for class.
         */
        sprintf ( buf2, "%3d", wch->level );
        switch ( wch->level )
        {
            default:
                break;
                {
                case MAX_LEVEL - 0:
                    sprintf ( buf2, "{R216{x" );
                    break;
                case MAX_LEVEL - 1:
                    sprintf ( buf2, "{G215{x" );
                    break;
                case MAX_LEVEL - 2:
                    sprintf ( buf2, "{G214{x" );
                    break;
                case MAX_LEVEL - 3:
                    sprintf ( buf2, "{G213{x" );
                    break;
                case MAX_LEVEL - 4:
                    sprintf ( buf2, "{G212{x" );
                    break;
                case MAX_LEVEL - 5:
                    sprintf ( buf2, "{G211{x" );
                    break;
                case MAX_LEVEL - 6:
                    sprintf ( buf2, "{G210{x" );
                    break;
                case MAX_LEVEL - 7:
                    sprintf ( buf2, "{G209{x" );
                    break;
                case MAX_LEVEL - 8:
                    sprintf ( buf2, "{G208{x" );
                    break;
                case MAX_LEVEL - 9:
                    sprintf ( buf2, "{G207{x" );
                    break;
                case MAX_LEVEL - 10:
                    sprintf ( buf2, "{G206{x" );
                    break;
                case MAX_LEVEL - 11:
                    sprintf ( buf2, "{G205{x" );
                    break;
                case MAX_LEVEL - 12:
                    sprintf ( buf2, "{G204{x" );
                    break;
		case MAX_LEVEL - 13:
		    sprintf ( buf2, "{C203{x" );
		    break;
                }
        }

        if ( IS_SET ( class_table[wch->class].tier, TIER_03 ) )
        {
            sprintf ( class, "{R%c{r%c%c{x",
                      class_table[wch->class].who_name[0],
                      class_table[wch->class].who_name[1],
                      class_table[wch->class].who_name[2] );
        }
        else if ( IS_SET ( class_table[wch->class].tier, TIER_04 ) )
        {
            sprintf ( class, "{B%c{b%c%c{x",
                      class_table[wch->class].who_name[0],
                      class_table[wch->class].who_name[1],
                      class_table[wch->class].who_name[2] );
        }

        else
        {
            sprintf ( class, "{G%c{g%c%c{x",
                      class_table[wch->class].who_name[0],
                      class_table[wch->class].who_name[1],
                      class_table[wch->class].who_name[2] );
        }

        /*
         * Format it up.
         */
        if ( !wch->on_quest )
        {
            questdat[0] = '\0';
        }
        else
        {
            sprintf ( questdat, "[{GQ{x] " );
        }
        if ( !is_clan ( wch ) )
        {
            sprintf ( clandat, "%s", clan_table[wch->clan].who_name );
        }
        else
        {
            if ( clan_table[wch->clan].independent )
            {
                sprintf ( clandat, "{r[{x%s{r]{x ",
                          clan_table[wch->clan].who_name );
            }
            else if ( is_clead ( wch ) && is_pkill ( wch ) )
            {
                sprintf ( clandat, "{R[{x%s{R]{x ",
                          clan_table[wch->clan].who_name );
            }
            else if ( is_clead ( wch ) && !is_pkill ( wch ) )
            {
                sprintf ( clandat, "{M[{x%s{M]{x ",
                          clan_table[wch->clan].who_name );
            }
            else if ( is_pkill ( wch ) )
            {
                sprintf ( clandat, "{r[{x%s{r]{x ",
                          clan_table[wch->clan].who_name );
            }
            else
            {
                sprintf ( clandat, "{m[{x%s{m]{x ",
                          clan_table[wch->clan].who_name );
            }
        }
        if ( wch->pcdata->who_descr[0] != '\0' )
        {
            sprintf ( buf,
                      "{b<{B<{x%s %s {r[%s{r]{B>{b>{x %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s{x\n\r",
                      buf2, wch->pcdata->who_descr,
		      wch->sex == 0 ? "{xN" : wch->sex == 1 ? "{RM" : "{MF",
		  questdat,
//                ON_GQUEST(wch) ? "(GQuest)" : "",
                      clandat, IS_SET ( wch->act2,
                                        PLR2_CHALLENGED ) ? "{W[{RARENA{W]{x "
                      : "", IS_SET ( wch->act2,
                                     PLR2_CHALLENGER ) ? "{W[{RARENA{W]{x " :
                      "", ( ( wch->ghost_level >= LEVEL_ANCIENT ) &&
                            ( ch->level >=
                              wch->level ) ) ? "({bGhost{x){x " : "",
		      !wch->desc ? "[{DLD{x] " : "",
                      wch->incog_level >= LEVEL_ANCIENT ? "({DIncog{x) " : "",
                      wch->invis_level >= LEVEL_ANCIENT ? "({WWizi{x) " : "",
      		      IS_SET ( wch->act2, PLR2_HELPER ) ? "({yHelper{x) " : "",
                      IS_SET ( wch->act2, PLR2_MASS_ARENA ) ? "({rVote{x) " : "", 
                      IS_SET ( wch->act2, PLR2_MASS_JOINER ) ? "({RTournament{x) " : "", 
                      IS_SET ( wch->comm, COMM_AFK ) ? "[{yA{YF{yK{x] " : "", 
                      IS_SET ( wch->act2, PLR2_PKSAFE ) ? "({wSAFE{x) " : "",
                      IS_SET ( wch->act2, PLR2_PKVIOLENT ) ? "({RVIOLENT{x) " : "",
                      IS_SET ( wch->act, PLR_TWIT ) ? "({RTWIT{x) " : "",
                      IS_SET (wch->pcdata->tag_flags, TAG_RED) ? "({RRed{x) " : "",
                      IS_SET (wch->pcdata->tag_flags, TAG_BLUE) ? "({BBlue{x) " : "",
		      IS_SET (wch->pcdata->tag_flags, TAG_FROZEN) ? "({CFr{coz{Ce{cn{x) " : "",
                      wch->pcdata->cname, IS_NPC ( wch ) ? "" : wch->pcdata->title );

        }
        else
        {
            sprintf ( buf,
                      "{b<{B<{x%s %-6s %s {r[%s{r]{B>{b>{x %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n\r",
                      buf2,
                      wch->race <
                      MAX_PC_RACE ? pc_race_table[wch->race].
                      who_name : "     ", class,
		      wch->sex == 0 ? "{xN" : wch->sex == 1 ? "{RM" : "{MF",
		  questdat,
//                ON_GQUEST(wch) ? "(GQuest)" : "",
			clandat,
                      IS_SET ( wch->act2,
                               PLR2_CHALLENGED ) ? "{W[{RARENA{W]{x " : "",
                      IS_SET ( wch->act2,
                               PLR2_CHALLENGER ) ? "{W[{RARENA{W]{x " : "",
                      ( ( wch->ghost_level >= LEVEL_ANCIENT ) &&
                                 ( ch->level >=
                                   wch->level ) ) ? "({bGhost{x) " : "",
		      !wch->desc ? "[{DLD{x] " : "",
                      wch->incog_level >= LEVEL_ANCIENT ? "({DIncog{x) " : "",
                      wch->invis_level >= LEVEL_ANCIENT ? "({WWizi{x) " : "",
		      IS_SET ( wch->act2, PLR2_HELPER ) ? "({yHelper{x) " : "",
                      IS_SET ( wch->act2, PLR2_MASS_ARENA ) ? "({rVote{x) " : "", 
                      IS_SET ( wch->act2, PLR2_MASS_JOINER ) ? "({RTournament{x) " : "", 
                      IS_SET ( wch->comm, COMM_AFK ) ? "[{yA{YF{yK{x] " : "",
                      IS_SET ( wch->act2, PLR2_PKSAFE ) ? "({wSAFE{x) " : "",
                      IS_SET ( wch->act2, PLR2_PKVIOLENT ) ? "({RVIOLENT{x) " : "",
                      IS_SET ( wch->act, PLR_TWIT ) ? "({RTWIT{x) " : "",
                      IS_SET (wch->pcdata->tag_flags, TAG_RED) ? "({RRed{x) " : "",
                      IS_SET (wch->pcdata->tag_flags, TAG_BLUE) ? "({BBlue{x) " : "",
		      IS_SET (wch->pcdata->tag_flags, TAG_FROZEN) ? "({CFr{coz{Ce{cn{x) " : "",
                      wch->pcdata->cname, IS_NPC ( wch ) ? "" : wch->pcdata->title );
        }
        //add_buf ( outputimm, buf );
	buffer_strcat ( outputimm, buf );
    }
    //add_buf ( outputimm, "\n\r" );
    buffer_strcat ( outputimm, "\n\r" );
    buf[0] = '\0';
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
        char class[MAX_STRING_LENGTH];
        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;
        wch = ( d->original != NULL ) ? d->original : d->character;
        if ( !can_see ( ch, wch ) && IS_SHIELDED ( wch, SHD_INVISIBLE ) )
            continue;
        if ( !can_see ( ch, wch ) && wch->level >= SQUIRE &&
             ch->level < wch->level )
            continue;
        if ( IS_IMMORTAL ( wch ) )
            continue;
        count++;
        if ( ( wch->in_room->vnum == ROOM_VNUM_CORNER ) &&
             ( !IS_IMMORTAL ( ch ) ) )
            continue;
        if ( wch->level < iLevelLower || wch->level > iLevelUpper ||
             ( fImmortalOnly && wch->level < LEVEL_IMMORTAL ) ||
             ( fClassRestrict && !rgfClass[wch->class] ) || ( fRaceRestrict &&
                                                              !rgfRace[wch->
                                                                       race] )
             || ( fClan && !is_clan ( wch ) ) || ( fClanRestrict &&
                                                   !rgfClan[wch->clan] ) )
            continue;
        nMatch++;
        /*
         * Figure out what to print for class.
         */
        sprintf ( buf2, "%3d", wch->level );
        switch ( wch->level )
        {
            default:
                break;
                {
                case MAX_LEVEL - 14:
 		    if ( IS_SET ( class_table[wch->class].tier, TIER_01 ) )
                    sprintf ( buf2, "{GA{gnc{x" );
		    else if ( IS_SET ( class_table[wch->class].tier, TIER_02 ) )
		    sprintf ( buf2, "{YA{ync{x" );
		    else if ( IS_SET ( class_table[wch->class].tier, TIER_03 ) )
		    sprintf ( buf2, "{RA{rnc{x" );
		    else
		    sprintf ( buf2, "{BA{bnc{x" );
                    break;
            case MAX_LEVEL - 15:
                    sprintf ( buf2, "{YH{yro{x" );
                    break;
                }
        }

        if ( IS_SET ( class_table[wch->class].tier, TIER_03 ) )
        {
            sprintf ( class, "{R%c{r%c%c{x",
                      class_table[wch->class].who_name[0],
                      class_table[wch->class].who_name[1],
                      class_table[wch->class].who_name[2] );
        }
        else if ( IS_SET ( class_table[wch->class].tier, TIER_02 ) )
        {
            sprintf ( class, "{Y%c{y%c%c{x",
                      class_table[wch->class].who_name[0],
                      class_table[wch->class].who_name[1],
                      class_table[wch->class].who_name[2] );
        }
        else if ( IS_SET ( class_table[wch->class].tier, TIER_04 ) )
        {
            sprintf ( class, "{B%c{b%c%c{x",
                      class_table[wch->class].who_name[0],
                      class_table[wch->class].who_name[1],
                      class_table[wch->class].who_name[2] );
        }
        else
        {
            sprintf ( class, "{G%c{g%c%c{x",
                      class_table[wch->class].who_name[0],
                      class_table[wch->class].who_name[1],
                      class_table[wch->class].who_name[2] );
        }

/*	if (wch->class < MAX_CLASS/2)
	{
	    sprintf(class, "{G%c{g%c%c{x",
		class_table[wch->class].who_name[0],
		class_table[wch->class].who_name[1],
		class_table[wch->class].who_name[2]);
	} else {
	    sprintf(class, "{Y%c{y%c%c{x",
		class_table[wch->class].who_name[0],
		class_table[wch->class].who_name[1],
		class_table[wch->class].who_name[2]);
	}*/

        /*
         * Format it up.
         */
        if ( !wch->on_quest )
        {
            questdat[0] = '\0';
        }
        else
        {
            sprintf ( questdat, "[{GQ{x] " );
        }
        if ( !is_clan ( wch ) )
        {
            sprintf ( clandat, "%s", clan_table[wch->clan].who_name );
        }
        else
        {
            if ( clan_table[wch->clan].independent )
            {
                sprintf ( clandat, "{r[{x%s{r]{x ",
                          clan_table[wch->clan].who_name );
            }
            else if ( is_clead ( wch ) && is_pkill ( wch ) )
            {
                sprintf ( clandat, "{R[{x%s{R]{x ",
                          clan_table[wch->clan].who_name );
            }
            else if ( is_clead ( wch ) && !is_pkill ( wch ) )
            {
                sprintf ( clandat, "{M[{x%s{M]{x ",
                          clan_table[wch->clan].who_name );
            }
            else if ( is_pkill ( wch ) )
            {
                sprintf ( clandat, "{r[{x%s{r]{x ",
                          clan_table[wch->clan].who_name );
            }
            else
            {
                sprintf ( clandat, "{m[{x%s{m]{x ",
                          clan_table[wch->clan].who_name );
            }
        }
        if ( wch->pcdata->who_descr[0] != '\0' )
        {
            sprintf ( buf,
                      "{r<{R<{x%s %s {r[%s{r]{R>{r>{x %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s{x\n\r",
                      buf2, wch->pcdata->who_descr,
		      wch->sex == 0 ? "{xN" : wch->sex == 1 ? "{RM" : "{MF",
		      questdat,
//                ON_GQUEST(wch) ? "(GQuest)" : "",
			clandat,
                      IS_SET ( wch->act2,
                               PLR2_CHALLENGED ) ? "{W[{RARENA{W]{x " : "",
                      IS_SET ( wch->act2,
                               PLR2_CHALLENGER ) ? "{W[{RARENA{W]{x " : "",
                      ( ( wch->ghost_level >= LEVEL_ANCIENT ) &&
                                 ( ch->level >=
                                   wch->level ) ) ? "({bGhost{x) " : "",
		      !wch->desc ? "[{DLD{x] " : "",
                      wch->incog_level >= LEVEL_ANCIENT ? "({DIncog{x) " : "",
                      wch->invis_level >= LEVEL_ANCIENT ? "({WWizi{x) " : "",
		      IS_SET ( wch->act2, PLR2_HELPER ) ? "({yHelper{x) " : "",
                      IS_SET ( wch->act2, PLR2_MASS_ARENA ) ? "({rVote{x) " : "", 
                      IS_SET ( wch->act2, PLR2_MASS_JOINER ) ? "({RTournament{x) " : "", 
                      IS_SET ( wch->comm, COMM_AFK ) ? "[{yA{YF{yK{x] " : "",
                      IS_SET ( wch->act2, PLR2_PKSAFE ) ? "({wSAFE{x) " : "",
                      IS_SET ( wch->act2, PLR2_PKVIOLENT ) ? "({RVIOLENT{x) " : "",
                      IS_SET ( wch->act, PLR_TWIT ) ? "({RTWIT{x) " : "",
                      IS_SET (wch->pcdata->tag_flags, TAG_RED) ? "({RRed{x) " : "",
                      IS_SET (wch->pcdata->tag_flags, TAG_BLUE) ? "({BBlue{x) " : "",
		      IS_SET (wch->pcdata->tag_flags, TAG_FROZEN) ? "({CFr{coz{Ce{cn{x) " : "",
                      wch->pcdata->cname, IS_NPC ( wch ) ? "" : wch->pcdata->title );
        }
        else
        {
            sprintf ( buf,
                      "{r<{R<{x%s %-6s %s {r[%s{r]{R>{r>{x %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s{x\n\r",
                      buf2,
                      wch->race <
                      MAX_PC_RACE ? pc_race_table[wch->race].
                      who_name : "     ", class, 
		      wch->sex == 0 ? "{xN" : wch->sex == 1 ? "{RM" : "{MF",
		      questdat,
                      //ON_GQUEST(wch) ? "(GQuest)" : "",
			clandat,
                      IS_SET ( wch->act2,
                               PLR2_CHALLENGED ) ? "{W[{RARENA{W]{x " : "",
                      IS_SET ( wch->act2,
                               PLR2_CHALLENGER ) ? "{W[{RARENA{W]{x " : "",
                      ( ( wch->ghost_level >= LEVEL_ANCIENT ) &&
                                 ( ch->level >=
                                   wch->level ) ) ? "({bGhost{x) " : "",
		      !wch->desc ? "[{DLD{x] " : "",
                      wch->incog_level >= LEVEL_ANCIENT ? "({DIncog{x) " : "",
                      wch->invis_level >= LEVEL_ANCIENT ? "({WWizi{x) " : "",
		      IS_SET ( wch->act2, PLR2_HELPER ) ? "({yHelper{x) " : "",
                      IS_SET ( wch->act2, PLR2_MASS_ARENA ) ? "({rVote{x) " : "", 
                      IS_SET ( wch->act2, PLR2_MASS_JOINER ) ? "({RTournament{x) " : "", 
                      IS_SET ( wch->comm, COMM_AFK ) ? "[{yA{YF{yK{x] " : "",
                      IS_SET ( wch->act2, PLR2_PKSAFE ) ? "({wSAFE{x) " : "",
                      IS_SET ( wch->act2, PLR2_PKVIOLENT ) ? "({RVIOLENT{x) " : "",
                      IS_SET ( wch->act, PLR_TWIT ) ? "({RTWIT{x) " : "",
                      IS_SET (wch->pcdata->tag_flags, TAG_RED) ? "({RRed{x) " : "",
                      IS_SET (wch->pcdata->tag_flags, TAG_BLUE) ? "({BBlue{x) " : "",
		      IS_SET (wch->pcdata->tag_flags, TAG_FROZEN) ? "({CFr{coz{Ce{cn{x) " : "",
                      wch->pcdata->cname, IS_NPC ( wch ) ? "" : wch->pcdata->title );
        }
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }

    count += countimm;
    hour = ( int ) ( struct tm * ) localtime ( &current_time )->tm_hour;

    if ( hour < 12 )
    {
        if ( is_pm )
        {
            is_pm = FALSE;
/*            max_on = 0;  */
//            expire_notes (  );
//           randomize_entrances ( ROOM_VNUM_CLANS ); 
        }
    }
    else
    {
        is_pm = TRUE;
    }
    
    max_on = UMAX ( count, max_on );

    if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
    {
        sprintf ( buf2, "\n\r{DYou are {RVIOLENT{D!{x" );
	buffer_strcat ( output, buf2 );	
    }

    if ( IS_SET ( ch->act2, PLR2_PKSAFE ) )
    {
        sprintf ( buf2, "\n\r{DYou are {wsafe{D.{x" );
	buffer_strcat ( output, buf2 );	
    }

    if ( IS_QUESTOR ( ch ) )
    {
        sprintf ( buf2, "\n\r{DYou are on an autoquest.{x" );
	buffer_strcat ( output, buf2 );	
    }

    if ( IS_SET ( ch->comm, COMM_AFK ) )
    {
        sprintf ( buf2, "\n\r{DYou are {yA{YF{yK{D. Better turn it off now that you're back?{x" );
	buffer_strcat ( output, buf2 );	
    }
    
    
     if ( is_reboot_countdown == TRUE )
     {

        if ( reboot_countdown > 0 && reboot_countdown != 1 &&
             is_reboot_countdown == TRUE && reboot_countdown != 2 )
            sprintf ( buf2,
                             "\n\r{DReboot{d:           {c[{d%d mins{c]{x",
                      reboot_countdown );

        if ( reboot_countdown == 1 && is_reboot_countdown == TRUE )
            sprintf ( buf2,
                      "\n\r{DReboot{d:           {c[{d60 secs{c]{x\n\r{DCommand Lockdown{d: {D{c[{don{c]{x\n\r{DLogin Lockdown{d:   {D{c[{don{c]{x" );

        if ( reboot_countdown == 2 && is_reboot_countdown == TRUE )
            sprintf ( buf2,
                      "\n\r{DReboot{d:           {c[{d2 mins{c]{x\n\r{DCommand Lockdown{d: {D{c[{don{c]{x" );

	buffer_strcat ( output, buf2 );	
     }

    if ( its_christmas )
    {
        sprintf ( buf2, "\n\r         {*IT'S {*CHRISTMAS{*!!! {*LETS {*CELEBRATE {*WITH {*ALL {*DOUBLES{*!!!{x\n\r" );
	buffer_strcat ( output, buf2 );
    }

    if ( special_day )
    {
        sprintf ( buf2, "\n\r            {YIT'S A SPECIAL DAY TODAY - ALL DOUBLES :){x\n\r" );
	buffer_strcat ( output, buf2 );
    }
	 
    if ( is_mass_arena && !is_mass_arena_fight )
    {
        sprintf ( buf2, "\n\r{DTournament{d:       {c[{Went{c]{x  " );
	buffer_strcat ( output, buf2 );	
    }                           
    
    else if ( is_mass_arena_fight )
    {
        sprintf ( buf2, "\n\r{DTournament{d:       {c[{Won {c]{x  " );
	buffer_strcat ( output, buf2 );	
    }                           

    else
    {
        sprintf ( buf2, "\n\r{DTournament{d:       {c[{Doff{c]{x  " );
	buffer_strcat ( output, buf2 );	
    }                           
             
    if ( global_quest )	
    {
        sprintf ( buf2, "{DGlobal Quest{d:     {c[{Won {c]{x" );
	buffer_strcat ( output, buf2 );	
    }                           

    else
    {
        sprintf ( buf2, "{DGlobal Quest{d:     {c[{Doff{x{c]{x" );
	buffer_strcat ( output, buf2 );	
    }                           
    
    if (tag_game.status != TAG_OFF)
    {
	sprintf ( buf2, "  {DFreeze Tag{d:       {c[{Won {x{c]{x" );
	buffer_strcat ( output, buf2 );	
    }
				
    else
    {
	sprintf ( buf2, "  {DFreeze Tag{d:       {c[{Doff{x{c]{x" );
	buffer_strcat ( output, buf2 );	
    }
    
    if ( no_wait_questing )
    {
        sprintf ( buf2, "\n\r{DNo wait questing{d: {c[{Won {c]{x" );
	buffer_strcat ( output, buf2 );	
    }
    else
    {
        sprintf ( buf2, "\n\r{DNo wait questing{d: {c[{Doff{x{c]" );
	buffer_strcat ( output, buf2 );	
    }
    
    if ( double_stance )
    {
        sprintf ( buf2, "  {DDouble stance{d:    {c[{Won {c]{x" );
	buffer_strcat ( output, buf2 );	
    }    
    
    else
    {
        sprintf ( buf2, "  {DDouble stance{d:    {c[{Doff{c]{x" );
	buffer_strcat ( output, buf2 );	
    }
        
    if ( double_qp )
    {
        sprintf ( buf2, "  {DDouble qp{d:        {c[{Won {c]{x" );
	buffer_strcat ( output, buf2 );	
    }

    else
    {
        sprintf ( buf2, "  {DDouble qp{d:        {c[{Doff{c]{»" );
	buffer_strcat ( output, buf2 );	
    }
        
    if ( double_exp )
    {
        sprintf ( buf2, "\n\r{DDouble exp{d:       {c[{Won {c]{x" );
	buffer_strcat ( output, buf2 );	
    }
    
    else
    {
        sprintf ( buf2, "\n\r{DDouble exp{d:       {c[{Doff{x{c]" );
	buffer_strcat ( output, buf2 );	
    }
    
    if ( double_damage )
    {
        sprintf ( buf2, "  {DDouble damage{d:    {c[{Won {c]{x" );
	buffer_strcat ( output, buf2 );	
    }
    
    else
    {
        sprintf ( buf2, "  {DDouble damage{d:    {c[{Doff{x{c]" );
	buffer_strcat ( output, buf2 );	
    }

    if ( double_pracs )
    {
        sprintf ( buf2, "  {DDouble pracs{d:     {c[{Won {c]{x" );
	buffer_strcat ( output, buf2 );	
    }
    
    else
    {
        sprintf ( buf2, "  {DDouble pracs{d:     {c[{Doff{x{c]" );
	buffer_strcat ( output, buf2 );	
    }
    
        
/*    if ( nMatch != count )
    {
        sprintf ( buf2, "\n\r{DPlayers visible{x:{Y %d{x\n\r", nMatch );
        add_buf ( output, buf2 );
    } 
    else
    {  */
/*      sprintf ( buf2, "\n\r{WPlayers visible{x:{Y %d{x", count );
      add_buf ( output, buf2 ); */
/*        sprintf ( buf2,
                  "\n\r{WImmortals found{x      :{Y %d\n\r{WMost on today        {x: {Y%d{x\n\r{WMost on ever {x        :{Y% d\n\r{WLogins today{x         :{Y %d\n\r",
                  countimm, max_on, max_ever, logins_today ); */
        sprintf ( buf2,
                  "\n\r{DPlayers visible{d:  {c[{d%-3d{c]{x  {DMost since boot{d:  {c[{d%-3d{c]{d  Mudinfo{D for more stats.{x\n\r",
                  count, max_on );
	buffer_strcat ( output, buf2 );	
//    } 

    if ( count > max_ever )
    {
        sync_stats (  );
    }
    
    if ( countimm >= 1 )
    {
//        add_buf ( outputimm, buf_string ( output ) );
        buffer_strcat ( outputimm, buffer_string ( output ) );
        //page_to_char ( buf_string ( outputimm ), ch );
	page_to_char ( buffer_string ( outputimm ), ch );
    }
    else
    {
        //page_to_char ( buf_string ( output ), ch );
	page_to_char ( buffer_string ( output ), ch );
    }
/*    free_buf ( output );
    free_buf ( outputimm ); */
    buffer_free ( output );
    buffer_free ( outputimm );
    return;
}
/*
CH_CMD ( do_count )
{
    int count;
    int hour;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    count = 0;
    hour = ( int ) ( struct tm * ) localtime ( &current_time )->tm_hour;
    if ( hour < 12 )
    {
        if ( is_pm )
        {
            is_pm = FALSE;
        }
    }
    else
    {
        is_pm = TRUE;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && can_see ( ch, d->character ) )
            count++;
    max_on = UMAX ( count, max_on );
    if ( max_on == count )
        sprintf ( buf,
                  "{BThere are {W%d {Bcharacters on, the most so far today.{x\n\r",
                  count );
    else
        sprintf ( buf,
                  "{BThere are {W%d {Bcharacters on, the most on today was {W%d{x.\n\r",
                  count, max_on );
    send_to_char ( buf, ch );
}
*/
CH_CMD ( do_inventory )
{
    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }
    //BUFFER *outlist;
    BUFFER2 *outlist;
    send_to_char ( "You are carrying:\n\r", ch );
    outlist = show_list_to_char ( ch->carrying, ch, TRUE, TRUE );
    //page_to_char ( buf_string ( outlist ), ch );
    page_to_char ( buffer_string ( outlist ), ch );
    //free_buf ( outlist );
    buffer_free ( outlist );
    return;
}

CH_CMD ( do_equipment )
{
    if ( IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }
    OBJ_DATA *obj;
    int iWear;
    int oWear;
    bool found;
    send_to_char ( "You are using:\n\r", ch );
    found = FALSE;
    for ( oWear = 0; oWear < MAX_WEAR; oWear++ )
    {
        iWear = where_order[oWear];
        send_to_char ( where_name[iWear], ch );
        if ( ( obj = get_eq_char ( ch, iWear ) ) == NULL )
        {
            send_to_char ( "{W[{BE{bmpt{By{W]{x\n\r", ch );
        }
        else if ( can_see_obj ( ch, obj ) )
        {
            send_to_char ( format_obj_to_char ( obj, ch, TRUE ), ch );
            send_to_char ( "\n\r", ch );
        }
        else
        {
            send_to_char ( "something.\n\r", ch );
        }
        found = TRUE;
    }

    if ( !found )
        send_to_char ( "Nothing.\n\r", ch );
    return;
}

CH_CMD ( do_compare )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;
    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Compare what to what?\n\r", ch );
        return;
    }

    if ( ( obj1 = get_obj_carry ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        for ( obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content )
        {
            if ( obj2->wear_loc != WEAR_NONE && can_see_obj ( ch, obj2 ) &&
                 obj1->item_type == obj2->item_type &&
                 ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE ) != 0 )
                break;
        }

        if ( obj2 == NULL )
        {
            send_to_char ( "You aren't wearing anything comparable.\n\r",
                           ch );
            return;
        }
    }

    else if ( ( obj2 = get_obj_carry ( ch, arg2 ) ) == NULL )
    {
        send_to_char ( "You do not have that item.\n\r", ch );
        return;
    }

    msg = NULL;
    value1 = 0;
    value2 = 0;
    if ( obj1 == obj2 )
    {
        msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
        msg = "You can't compare $p and $P.";
    }
    else
    {
        switch ( obj1->item_type )
        {
            default:
                msg = "You can't compare $p and $P.";
                break;
            case ITEM_ARMOR:
                if ( obj1->clan )
                {
                    value1 = apply_ac ( ch->level, obj1, WEAR_HOLD, 0 );
                    value1 += apply_ac ( ch->level, obj1, WEAR_HOLD, 1 );
                    value1 += apply_ac ( ch->level, obj1, WEAR_HOLD, 2 );
                }
                else
                {
                    value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
                }
                if ( obj2->clan )
                {
                    value2 = apply_ac ( ch->level, obj2, WEAR_HOLD, 0 );
                    value2 += apply_ac ( ch->level, obj2, WEAR_HOLD, 1 );
                    value2 += apply_ac ( ch->level, obj2, WEAR_HOLD, 2 );
                }
                else
                {
                    value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
                }
                break;
            case ITEM_WEAPON:
                if ( obj1->clan )
                {
                    value1 = 4 * ( ch->level / 3 );
                }
                else
                {
                    if ( obj1->pIndexData->new_format )
                        value1 = ( 1 + obj1->value[2] ) * obj1->value[1];
                    else
                        value1 = obj1->value[1] + obj1->value[2];
                }
                if ( obj2->clan )
                {
                    value2 = 4 * ( ch->level / 3 );
                }
                else
                {
                    if ( obj2->pIndexData->new_format )
                        value2 = ( 1 + obj2->value[2] ) * obj2->value[1];
                    else
                        value2 = obj2->value[1] + obj2->value[2];
                }
                break;
        }
    }

    if ( msg == NULL )
    {
        if ( value1 == value2 )
            msg = "$p and $P look about the same.";
        else if ( value1 > value2 )
            msg = "$p looks better than $P.";
        else
            msg = "$p looks worse than $P.";
    }

    act ( msg, ch, obj1, obj2, TO_CHAR );
    return;
}

CH_CMD ( do_credits )
{
    do_help ( ch, "credits" );
    return;
}

CH_CMD ( do_copyright )
{
    do_help ( ch, "copyright" );
    return;
}

CH_CMD ( do_where )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;
    one_argument ( argument, arg );
    if ( IS_AFFECTED ( ch, AFF_BLIND ) )
    {
	send_to_char ( "You are blind, you don't know where you are.\n\r", ch );
	return;
    }
    if ( arg[0] == '\0' )
    {
        sprintf( buf, "Players near you in %s:\n\r", ch->in_room->area->credits );
	send_to_char( buf, ch );
        found = FALSE;
        for ( d = descriptor_list; d; d = d->next )
        {
            if ( d->connected == CON_PLAYING &&
                 ( victim = d->character ) != NULL && !IS_NPC ( victim ) &&
                 victim->in_room != NULL &&
                 !IS_SET ( victim->in_room->room_flags, ROOM_NOWHERE ) &&
                 ( is_room_owner ( ch, victim->in_room ) ||
                   !room_is_private ( ch, victim->in_room ) ) &&
                 victim->in_room->area == ch->in_room->area &&
                 get_trust ( ch ) >= victim->ghost_level &&
                 can_see ( ch, victim ) )
            {
                found = TRUE;
                sprintf ( buf, "%-28s %s\n\r", victim->name,
                          victim->in_room->name );
                send_to_char ( buf, ch );
            }
        }
        if ( !found )
            send_to_char ( "None\n\r", ch );
    }
    else
    {
        found = FALSE;
        for ( victim = char_list; victim != NULL; victim = victim->next )
        {
            if ( victim->in_room != NULL &&
                 victim->in_room->area == ch->in_room->area &&
                 !IS_AFFECTED ( victim, AFF_HIDE ) &&
                 !IS_AFFECTED ( victim, AFF_SNEAK ) &&
                 get_trust ( ch ) >= victim->ghost_level &&
                 can_see ( ch, victim ) && is_name ( arg, victim->name ) )
            {
                found = TRUE;
                sprintf ( buf, "%-28s %s\n\r", PERS ( victim, ch ),
                          victim->in_room->name );
                send_to_char ( buf, ch );
                break;
            }
        }
        if ( !found )
            act ( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}

CH_CMD ( do_track )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *in_room;
    long track_vnum;
    int door, move, chance, track;
    one_argument ( argument, arg );
    if ( ( chance = get_skill ( ch, gsn_track ) ) == 0 )
    {
        send_to_char ( "You don't know how to track.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Track whom?\n\r", ch );
        return;
    }

    in_room = ch->in_room;
    track_vnum = in_room->vnum;
    move = movement_loss[UMIN ( SECT_MAX - 1, in_room->sector_type )];
    if ( ch->move < move )
    {
        send_to_char ( "You are too exhausted.\n\r", ch );
        return;
    }

    if ( number_percent (  ) < ( 100 - chance ) )
    {
        sprintf ( buf, "You can find no recent tracks for %s.\n\r", arg );
        send_to_char ( buf, ch );
        check_improve ( ch, gsn_track, FALSE, 1 );
        WAIT_STATE ( ch, 1 );
        ch->move -= move / 2;
        return;
    }

    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
        if ( victim->in_room != NULL && !IS_NPC ( victim ) &&
             can_see ( ch, victim ) && is_name ( arg, victim->name ) )
        {
            if ( victim->in_room->vnum == track_vnum )
            {
                act ( "The tracks end right under $S feet.", ch, NULL, victim,
                      TO_CHAR );
                return;
            }
            for ( track = 0; track < MAX_TRACK; track++ )
            {
                if ( victim->track_from[track] == track_vnum )
                {
                    for ( door = 0; door < 12; door++ )
                    {
                        if ( ( pexit = in_room->exit[door] ) != NULL )
                        {
                            if ( pexit->u1.to_room->vnum ==
                                 victim->track_to[track] )
                            {
                                sprintf ( buf,
                                          "Some tracks lead off to the %s.\n\r",
                                          dir_name[door] );
                                send_to_char ( buf, ch );
                                check_improve ( ch, gsn_track, TRUE, 1 );
                                WAIT_STATE ( ch, 1 );
                                ch->move -= move;
                                return;
                            }
                        }
                    }
                    act ( "$N seems to have vanished here.", ch, NULL, victim,
                          TO_CHAR );
                    check_improve ( ch, gsn_track, TRUE, 1 );
                    WAIT_STATE ( ch, 1 );
                    ch->move -= move;
                    return;
                }
            }
            act ( "You can find no recent tracks for $N.", ch, NULL, victim,
                  TO_CHAR );
            check_improve ( ch, gsn_track, FALSE, 1 );
            WAIT_STATE ( ch, 1 );
            ch->move -= move / 2;
            return;
        }
    }
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
        if ( ( victim->in_room != NULL ) && IS_NPC ( victim ) &&
             can_see ( ch, victim ) &&
             ( victim->in_room->area == ch->in_room->area ) &&
             is_name ( arg, victim->name ) )
        {
            if ( victim->in_room->vnum == track_vnum )
            {
                act ( "The tracks end right under $S feet.", ch, NULL, victim,
                      TO_CHAR );
                return;
            }
            for ( track = 0; track < MAX_TRACK; track++ )
            {
                if ( victim->track_from[track] == track_vnum )
                {
                    for ( door = 0; door < 12; door++ )
                    {
                        if ( ( pexit = in_room->exit[door] ) != NULL )
                        {
                            if ( pexit->u1.to_room->vnum ==
                                 victim->track_to[track] )
                            {
                                sprintf ( buf,
                                          "Some tracks lead off to the %s.\n\r",
                                          dir_name[door] );
                                send_to_char ( buf, ch );
                                check_improve ( ch, gsn_track, TRUE, 1 );
                                WAIT_STATE ( ch, 1 );
                                ch->move -= move;
                                return;
                            }
                        }
                    }
                    act ( "$N seems to have vanished here.", ch, NULL, victim,
                          TO_CHAR );
                    check_improve ( ch, gsn_track, TRUE, 1 );
                    WAIT_STATE ( ch, 1 );
                    ch->move -= move;
                    return;
                }
            }
        }
    }
    sprintf ( buf, "You can find no recent tracks for %s.\n\r", arg );
    send_to_char ( buf, ch );
    check_improve ( ch, gsn_track, FALSE, 1 );
    WAIT_STATE ( ch, 1 );
    ch->move -= move / 2;
    return;
}

CH_CMD ( do_consider )
{
//    BUFFER *output;
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff, vac, cac;
//    char buf[MAX_STRING_LENGTH];
    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Consider killing whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They're not here.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
    {
        send_to_char ( "Don't even think about it.\n\r", ch );
        return;
    }

    diff = ( ( victim->hit / 50 ) - ( ch->hit / 50 ) );
    vac =
        -( GET_AC ( victim, AC_PIERCE ) + GET_AC ( victim, AC_BASH ) +
           GET_AC ( victim, AC_SLASH ) + GET_AC ( victim, AC_EXOTIC ) );
    cac =
        -( GET_AC ( ch, AC_PIERCE ) + GET_AC ( ch, AC_BASH ) +
           GET_AC ( ch, AC_SLASH ) + GET_AC ( ch, AC_EXOTIC ) );
    diff += ( vac - cac );
    diff += ( GET_DAMROLL ( victim ) - GET_DAMROLL ( ch ) );
    diff += ( GET_HITROLL ( victim ) - GET_HITROLL ( ch ) );
    diff +=
        ( get_curr_stat ( victim, STAT_STR ) -
          get_curr_stat ( ch, STAT_STR ) );
    if ( diff <= -60 )
        msg = "You can kill $N naked and weaponless.";
    else if ( diff <= -20 )
        msg = "$N is no match for you.";
    else if ( diff <= 10 )
        msg = "$N looks like an easy kill.";
    else if ( diff <= 40 )
        msg = "$N can hold $S own against you.";
    else if ( diff <= 120 )
        msg = "$N says '{aDo you feel lucky, punk?{x'.";
    else if ( diff <= 160 )
        msg = "$N laughs at you mercilessly.";
    else
        msg = "Death will thank you for your gift.";
    act ( msg, ch, NULL, victim, TO_CHAR );
/*    sprintf ( buf, "Hp: %ld/%ld  Mana: %ld/%ld  Lv: %d  Align: %d\n\r",
              victim->hit, victim->max_hit, victim->mana, victim->max_mana,
              victim->level, victim->alignment );
    send_to_char ( buf, ch );
    if ( victim->imm_flags )
    {
        sprintf ( buf, "Immune: %s\n\r", imm_bit_name ( victim->imm_flags ) );
        send_to_char ( buf, ch );
    }

    if ( victim->res_flags )
    {
        sprintf ( buf, "Resist: %s\n\r", imm_bit_name ( victim->res_flags ) );
        send_to_char ( buf, ch );
    }
    if ( victim->vuln_flags )
    {
        sprintf ( buf, "Vulnerable: %s\n\r",
                  imm_bit_name ( victim->vuln_flags ) );
        send_to_char ( buf, ch );
    }
*/
    return;
}

void set_title ( CHAR_DATA * ch, char *title )
{
    char buf[MAX_STRING_LENGTH];
    if ( IS_NPC ( ch ) )
    {
        bug ( "Set_title: NPC.", 0 );
        return;
    }

    if ( title[0] != '.' && title[0] != ',' && title[0] != '!' &&
         title[0] != '?' )
    {
        buf[0] = ' ';
        strcpy ( buf + 1, title );
    }
    else
    {
        strcpy ( buf, title );
    }

    free_string ( ch->pcdata->title );
    ch->pcdata->title = str_dup ( buf );
    return;
}

CH_CMD ( do_title )
{
    int value;
    if ( IS_NPC ( ch ) )
        return;
    if ( IS_SET ( ch->act, PLR_NOTITLE ) )
        return;
    if ( ( ch->in_room->vnum == ROOM_VNUM_CORNER ) &&
         ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "Just keep your nose in the corner like a good little player.\n\r",
              ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
/*        send_to_char ( "Change your title to what?\n\r", ch );
        return; */
	set_title ( ch, argument ); /* Changed for easier clearing of your title - Rhaelar 5'2004 */
    }

    if ( strlen ( argument ) > 77 )
    {
        argument[77] = '{';
        argument[78] = 'x';
        argument[79] = '\0';
    }
    else
    {
        value = strlen ( argument );
        argument[value] = '{';
        argument[value + 1] = 'x';
        argument[value + 2] = '\0';
    }
    smash_tilde ( argument );
    set_title ( ch, argument );
    send_to_char ( "Ok.\n\r", ch );
}

CH_CMD ( do_description )
{
    char buf[MAX_STRING_LENGTH];
    if ( argument[0] != '\0' )
    {
        buf[0] = '\0';
        smash_tilde ( argument );
        if ( argument[0] == '-' )
        {
            int len;
            bool found = FALSE;
            if ( ch->description == NULL || ch->description[0] == '\0' )
            {
                send_to_char ( "No lines left to remove.\n\r", ch );
                return;
            }

            strcpy ( buf, ch->description );
            for ( len = strlen ( buf ); len > 0; len-- )
            {
                if ( buf[len] == '\r' )
                {
                    if ( !found )   /* back it up */
                    {
                        if ( len > 0 )
                            len--;
                        found = TRUE;
                    }
                    else        /* found the second one */
                    {
                        buf[len + 1] = '\0';
                        free_string ( ch->description );
                        ch->description = str_dup ( buf );
                        send_to_char ( "Your description is:\n\r", ch );
                        send_to_char ( ch->description ? ch->
                                       description : "(None).\n\r", ch );
                        return;
                    }
                }
            }
            buf[0] = '\0';
            free_string ( ch->description );
            ch->description = str_dup ( buf );
            send_to_char ( "Description cleared.\n\r", ch );
            return;
        }

        if ( argument[0] == '+' )
        {
            if ( ch->description != NULL )
                strcat ( buf, ch->description );
            argument++;
            while ( isspace ( *argument ) )
                argument++;
        }

        if ( strlen ( buf ) + strlen ( argument ) >= MAX_STRING_LENGTH - 2 )
        {
            send_to_char ( "Description too long.\n\r", ch );
            return;
        }

        strcat ( buf, argument );
        strcat ( buf, "\n\r" );
        free_string ( ch->description );
        ch->description = str_dup ( buf );
    }

    send_to_char ( "Your description is:\n\r", ch );
    send_to_char ( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}

CH_CMD ( do_report )
{
    char buf[MAX_INPUT_LENGTH];
    sprintf ( buf,
              "You say '{SI have %ld/%ld hp %ld/%ld mana %ld/%ld mv %ld xp.{x'\n\r",
              ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
              ch->max_move, ch->exp );
    send_to_char ( buf, ch );
    sprintf ( buf,
              "$n says '{SI have %ld/%ld hp %ld/%ld mana %ld/%ld mv %ld xp.{x'",
              ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
              ch->max_move, ch->exp );
    act ( buf, ch, NULL, NULL, TO_ROOM );
    return;
}

CH_CMD ( do_practice )
{
    char buf[MAX_STRING_LENGTH];
/*    char practiceresult[MAX_STRING_LENGTH*4]; */
    int sn;
    if ( IS_NPC ( ch ) )
        return;
    if ( argument[0] == '\0' )
    {
        //BUFFER *output;
	BUFFER2 *output;
        int col;
	int count;
	int adept;
        col = 0;
	count = 0;
	//output = new_buf (  );
	output = buffer_new ( 1337 );
	adept = IS_NPC ( ch ) ? 100 : class_table[ch->class].skill_adept;
        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[sn].name == NULL )
                break;
            if ( ch->level < skill_table[sn].skill_level[ch->class] ||
                 ch->pcdata->learned[sn] < 1 /* skill is not known */  )
                continue;
            if ( ch->pcdata->learned[sn] >= adept )
            sprintf ( buf, "{r%4d%%{D: ", ch->pcdata->learned[sn] );
	    else
            sprintf ( buf, "{R%4d%%{D: ", ch->pcdata->learned[sn] );
/*	    strcat(practiceresult,buf); */
	    //add_buf(output,buf);
	    buffer_strcat ( output, buf );
	    count++;

	    if ( count < 25 )
            sprintf ( buf, "{D" );
	    if ( count >= 25 )
            sprintf ( buf, "{c" );
	    if ( count >= 50 )
            sprintf ( buf, "{y" );
	    if ( count >= 75 )
            sprintf ( buf, "{r" );
	    if ( count >= 100 )
            sprintf ( buf, "{m" );
	    if ( count >= 125 )
            sprintf ( buf, "{c" );
	    if ( count >= 150 )
            sprintf ( buf, "{D" );
	    if ( count >= 175 )
            sprintf ( buf, "{b" );
	    if ( count >= 200 )
            sprintf ( buf, "{c" );
	    if ( count >= 225 )
            sprintf ( buf, "{y" );
	    if ( count >= 250 )
            sprintf ( buf, "{r" );
	    if ( count >= 275 )
            sprintf ( buf, "{m" );
	    if ( count >= 300 )
            sprintf ( buf, "{c" );
	    if ( count >= 325 )
            sprintf ( buf, "{D" );
	    if ( count >= 350 )
            sprintf ( buf, "{c" );

	    //add_buf(output,buf);
	    buffer_strcat ( output, buf );
/*
	    strcat(practiceresult,buf);
*/
            sprintf ( buf, "%-17s{D|", skill_table[sn].name );
/*	    strcat(practiceresult,buf); */
	    //add_buf(output,buf);
	    buffer_strcat ( output, buf );
            /*skill_table[sn].name, ch->pcdata->learned[sn] ); */
       /*     send_to_char ( buf, ch ); */
            if ( ++col % 4 == 0 )
            {
	     sprintf ( buf, "\n\r" );
/*	     strcat(practiceresult,buf); */
             //add_buf(output,buf);
	     buffer_strcat ( output, buf );
	    }
        }
	
        if ( col % 4 != 0 )
	{
            sprintf ( buf, "\n\r" );
	/*    strcat(practiceresult,buf); */
	     //add_buf(output,buf);
	     buffer_strcat ( output, buf );
	}
/*	send_to_char ( "\n\r", ch ); */
        sprintf ( buf, "{xYou have {R%d{x practice sessions left.\n\r",
                  ch->practice );
	    /*strcat(practiceresult,buf);*/
	    //add_buf(output,buf);
	    buffer_strcat ( output, buf );
/*	page_to_char ( output, ch );*/
	//page_to_char ( buf_string ( output ), ch );
	page_to_char ( buffer_string ( output ), ch );
	//free_buf ( output );
	buffer_free ( output );
/*	sprintf ( practiceresult, "\n\r" );  */
 /*       send_to_char ( buf, ch ); */
    }
    else
    {
        CHAR_DATA *mob;
        int adept;
        if ( !IS_AWAKE ( ch ) )
        {
            send_to_char ( "In your dreams, or what?\n\r", ch );
            return;
        }

        for ( mob = ch->in_room->people; mob != NULL;
              mob = mob->next_in_room )
        {
            if ( IS_NPC ( mob ) && IS_SET ( mob->act, ACT_PRACTICE ) )
                break;
        }

        if ( mob == NULL )
        {
            send_to_char ( "You can't do that here.\n\r", ch );
            return;
        }

        if ( ch->practice <= 0 )
        {
            send_to_char ( "You have no practice sessions left.\n\r", ch );
            return;
        }

        if ( ( sn = find_spell ( ch, argument ) ) < 0 || ( !IS_NPC ( ch ) && ( ch->level < skill_table[sn].skill_level[ch->class] || ch->pcdata->learned[sn] < 1    /* skill is not known */
                                                                               ||
                                                                               skill_table
                                                                               [sn].
                                                                               rating
                                                                               [ch->
                                                                                class]
                                                                               ==
                                                                               0 ) ) )
        {
            send_to_char ( "You can't practice that.\n\r", ch );
            return;
        }

        adept = IS_NPC ( ch ) ? 100 : class_table[ch->class].skill_adept;
        if ( ch->pcdata->learned[sn] >= adept )
        {
            sprintf ( buf, "You are already learned at %s.\n\r",
                      skill_table[sn].name );
            send_to_char ( buf, ch );
        }
        else
        {
            ch->practice--;
            ch->pcdata->learned[sn] +=
                int_app[get_curr_stat ( ch, STAT_INT )].learn /
                skill_table[sn].rating[ch->class] / 3;
            if ( ch->pcdata->learned[sn] < adept )
            {
                act ( "You practice $T.", ch, NULL, skill_table[sn].name,
                      TO_CHAR );
/*                act ( "$n practices $T.", ch, NULL, skill_table[sn].name,
                      TO_ROOM ); */
            }
            else
            {
                ch->pcdata->learned[sn] = adept;
                act ( "You are now learned at $T.", ch, NULL,
                      skill_table[sn].name, TO_CHAR );
/*                act ( "$n is now learned at $T.", ch, NULL,
                      skill_table[sn].name, TO_ROOM ); */
            }
        }
    }
    return;
}

/*
 * 'Wimpy' originally by Dionysos.
 */
CH_CMD ( do_wimpy )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;
    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
        wimpy = ch->max_hit / 5;
    else
        wimpy = atoi ( arg );
    if ( wimpy < 0 )
    {
        send_to_char ( "Your courage exceeds your wisdom.\n\r", ch );
        return;
    }

    if ( wimpy > ch->max_hit / 2 )
    {
        send_to_char ( "Such cowardice ill becomes you.\n\r", ch );
        return;
    }

    ch->wimpy = wimpy;
    sprintf ( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char ( buf, ch );
    return;
}

CH_CMD ( do_password )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;
    if ( IS_NPC ( ch ) )
        return;
    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace ( *argument ) )
        argument++;
    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
        cEnd = *argument++;
    while ( *argument != '\0' )
    {
        if ( *argument == cEnd )
        {
            argument++;
            break;
        }
        *pArg++ = *argument++;
    }
    *pArg = '\0';
    pArg = arg2;
    while ( isspace ( *argument ) )
        argument++;
    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
        cEnd = *argument++;
    while ( *argument != '\0' )
    {
        if ( *argument == cEnd )
        {
            argument++;
            break;
        }
        *pArg++ = *argument++;
    }
    *pArg = '\0';
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Syntax: password <old> <new>.\n\r", ch );
        return;
    }

    if ( str_cmp ( md5crypt ( ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
        WAIT_STATE ( ch, 40 );
        send_to_char ( "Wrong password.  Wait 10 seconds.\n\r", ch );
        return;
    }

    if ( strlen ( arg2 ) < 5 )
    {
        send_to_char
            ( "New password must be at least five characters long.\n\r", ch );
        return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = md5crypt ( arg2 );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
        if ( *p == '~' )
        {
            send_to_char ( "New password not acceptable, try again.\n\r",
                           ch );
            return;
        }
    }

    free_string ( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup ( pwdnew );
    save_char_obj ( ch );
    send_to_char ( "Ok.\n\r", ch );
    return;
}

CH_CMD ( do_autoall )
{
    if ( IS_NPC ( ch ) )
        return;
    SET_BIT ( ch->act, PLR_AUTOASSIST );
    SET_BIT ( ch->act, PLR_AUTOEXIT );
    SET_BIT ( ch->act, PLR_AUTOGOLD );
    SET_BIT ( ch->act, PLR_AUTOLOOT );
    SET_BIT ( ch->act, PLR_AUTOPEEK );
    SET_BIT ( ch->act, PLR_AUTOSAC );
    SET_BIT ( ch->act, PLR_AUTOSPLIT );
    send_to_char ( "All autos turned on.\n\r", ch );
}

CH_CMD ( do_omni )
{
    char buf[MAX_STRING_LENGTH * 100];
    char buf2[MAX_STRING_LENGTH * 100];
    //BUFFER *output;
    BUFFER2 *output;
    DESCRIPTOR_DATA *d;
    int immmatch;
    int mortmatch;
    int hptemp;
    /*
       * Initalize Variables.
     */
    immmatch = 0;
    mortmatch = 0;
    buf[0] = '\0';
    //output = new_buf (  );
    output = buffer_new ( 1337 );
    /*
       * Count and output the IMMs.
     */
    sprintf ( buf,
              "{D-{w={D-{w={D-{w={D-{w={D-{w={D-{W>{RImmortals{W<{D-{w={D-{w={D-{w={D-{w={D-{w={D-{x\n\r" );
    //add_buf ( output, buf );
    buffer_strcat ( output, buf );
    sprintf ( buf,
              "{GName           {YLevel    {WWiz     {rIncog   {w[{RVnum{w]{x\n\r" );
    //add_buf ( output, buf );
    buffer_strcat ( output, buf );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;
        wch = ( d->original != NULL ) ? d->original : d->character;
        if ( !can_see ( ch, wch ) || wch->level < 203 )
            continue;
        immmatch++;
        sprintf ( buf,
                  "{G%-14s  {Y%-3d     {W%-3d     {r%-3d{w     [{R%-6ld{w]{x\n\r",
                  wch->name, wch->level, wch->invis_level, wch->incog_level,
                  wch->in_room->vnum );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }

    /*
       * Count and output the Morts.
     */
    sprintf ( buf,
              "\n\r{D-{w={D-{w={D-{w={D-{w={D-{w={D-{W>{RMortals{W<{w-{D={w-{D={w-{D={w-{D={w-{D={w-\n\r" );
    //add_buf ( output, buf );
    buffer_strcat ( output, buf );
    sprintf ( buf,
              "{GName           {DRace{w/{YClass   {gPosition      {mLevel  {R%%hps {w[{yVnum{w]{x\n\r" );
    //add_buf ( output, buf );
    buffer_strcat ( output, buf );
    hptemp = 0;
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
        char const *class;
        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;
        wch = ( d->original != NULL ) ? d->original : d->character;
        if ( !can_see ( ch, wch ) || wch->level > ch->level ||
             wch->level > 202 )
            continue;
        mortmatch++;
        if ( ( wch->max_hit != wch->hit ) && ( wch->hit > 0 ) )
            hptemp = ( wch->hit * 100 ) / wch->max_hit;
        else if ( wch->max_hit == wch->hit )
            hptemp = 100;
        else if ( wch->hit < 0 )
            hptemp = 0;
        class = class_table[wch->class].who_name;
        sprintf ( buf,
                  "{G%-14s {D%6s{w/{Y%3s    {g%-15s {m%-3d   {R%3d%%   {w[{y%ld{w]{x\n\r",
                  wch->name,
                  wch->race <
                  MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
                  class, capitalize ( position_table[wch->position].name ),
                  wch->level, hptemp, wch->in_room->vnum );
        //add_buf ( output, buf );
	buffer_strcat ( output, buf );
    }

/*
 * Tally the counts and send the whole list out.
 */
    sprintf ( buf2, "\n\r{RIMMs {wfound{x: {M%d{x\n\r", immmatch );
    //add_buf ( output, buf2 );
    buffer_strcat ( output, buf2 );
    sprintf ( buf2, "{BMorts {wfound{x: {M%d{x\n\r", mortmatch );
    //add_buf ( output, buf2 );
    buffer_strcat ( output, buf2 );
//    page_to_char ( buf_string ( output ), ch );
    page_to_char ( buffer_string ( output ), ch );
    //free_buf ( output );
    buffer_free ( output );
    return;
}

CH_CMD ( do_anew )
{

    char buf[MAX_STRING_LENGTH];
    char result[MAX_STRING_LENGTH * 2];
    AREA_DATA *pArea;
    sprintf ( result, "{m[{W%-9s{m] {m({W%-27s{m  ){W %-10s{x\n\r\n\r",
              "Lvl Range", "Area Name", "Credits" );
    //sprintf( result,"{c************************************************************{x\n\r");
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        sprintf ( buf, "{m[{W%-9s{m] {m({W%-29.29s{m){W %-10.10s{x\n\r",
                  "Lvl Range", pArea->name, pArea->builders );
        strcat ( result, buf );
    }
//    sprintf( result,"{c************************************************************{x\n\r");
    send_to_char ( result, ch );
    return;
}

CH_CMD ( do_map )
{
		if ( !str_cmp ( argument, "\0" ) )
		{
    send_to_char("Which area do you want the path for? See 'areas' for a list of areas.\n\r",ch);
    		}
		else if  ( !str_cmp( argument, "gangland") )
		{
    send_to_char("Go 4se3se.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "mahn-tor") )
		{
    send_to_char("Go 11ws10ws.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "elemental") )
		{
    send_to_char("Go 16w8ne.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "drakyri") )
		{
    send_to_char("Go 11ws3wn.\n\r",ch);
		}		
		else if  ( !str_cmp( argument, "pirate") )
		{
    send_to_char("Go 4s2e3s2w7se3s.\n\r",ch);
		}				
		else if  ( !str_cmp( argument, "trials") )
		{
    send_to_char("Go 12wn.\n\r",ch);
		}		
		else if  ( !str_cmp( argument, "trials") )
		{
    send_to_char("Go 12wn.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "battlefield") )
		{
    send_to_char("Go 11ws7wn.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "mutant") )
		{
    send_to_char("Go 16w9nwn.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "tenilwyn") )
		{
    send_to_char("Go 13e3nene5n.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "warrior") )
		{
    send_to_char("(Warrior Castle, 'map morlok' for Morlok Castle) Go 3e4n2w2n3en.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "zubie") )
		{
    send_to_char("(Zubie's Weed Farm), 11w3se.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "farm") )
		{
    send_to_char("(Zubie's Weed Farm), 11w3se.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "weed") )
		{
    send_to_char("(Zubie's Weed Farm), 11w3se.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "castle") )
		{
    send_to_char("(Warrior Castle, 'map morlok' for Morlok Castle) Go 3e4n2w2n3en.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "kargaranth") )
		{
    send_to_char("Go 16w9n4wuwnun6wn.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "maerkai") )
		{
    send_to_char("Go 16w3nw.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "inklar") )
		{
    send_to_char("Go 11ws10w.\n\r",ch);
		}	
		else if  ( !str_cmp( argument, "celestial") )
		{
    send_to_char("Go 13e3nene4n2en.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "eagle") )
		{
    send_to_char("Go 11ws9w2n2e.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "morlok") )
		{
    send_to_char("Go 16w9n4wu2wn.\n\r",ch);
		}	
		else if  ( !str_cmp( argument, "yoshiuki") )
		{
    send_to_char("Go 16w8n2e.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "vrugar") )
		{
    send_to_char("Go 16w9n4wuwnun3es.\n\r",ch);
		}		
		else if  ( !str_cmp( argument, "lemtorak") )
		{
    send_to_char("Go 13e3nenen2en5es4e2ne.\n\r",ch);
		}
		else if  ( !str_cmp( argument, "tree") )
		{
    send_to_char("(Tree of Death) Go 13e3ne2n.\n\r",ch);
		}
                else if  ( !str_cmp( argument, "smurf") )
                {
    send_to_char("Go 6en.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "saitok") )
                {
    send_to_char("Go 8en.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "midian") )
                {
    send_to_char("Go 15en.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "mapola") )
                {
    send_to_char("Go 16w8nw.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "union") )
                {
    send_to_char("Go to the future, then 2n5wn.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "kraven") )
                {
    send_to_char("Go 11en.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "mansion") )
                {
    send_to_char("Go 13e3nw.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "torture") )
                {
    send_to_char("Go 13e3nw.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "future") )
                {
    send_to_char("Go 14w, enter portal.\n\r",ch);
                }
                else if  ( !str_cmp( argument, "newbie" ) )
                {
    send_to_char("Go 2nu.\n\r",ch);
                }		
                else if  ( !str_cmp( argument, "school" ) )
                {
    send_to_char("Go 2nu.\n\r",ch);
                }		
                else if  ( !str_cmp( argument, "everlasting" ) )
                {
    send_to_char("Go 4s2e3s2w5se.\n\r",ch);
                }		
                else if  ( !str_cmp( argument, "silence" ) )
                {
    send_to_char("Go 4s2e3s2w5se.\n\r",ch);
                }		
		else 
		{
    send_to_char("Can't find that location.\n\r",ch);
		}
}

CH_CMD ( do_slist )
{
//BUFFER *output;
BUFFER2 *output;
char arg1[MIL],group[MSL];
char buf [MSL];
int skill,class = 0,cl = 0;
int sn = 0,col = 0,level = 0, sp,gr;
int skCount = 0,spCount = 0, class_name = 0, gn = 0;
bool found = FALSE;
argument = one_argument(argument,arg1);
//output = new_buf();
output = buffer_new ( 1337 );

    if (arg1[0] == '\0') {
        printf_to_char(ch,"Enter the name of the spell or class.\n\r");
        return;
    }
        if (((skill = skill_lookup(arg1)) == -1) && (cl = class_lookup(arg1)) == -1) {
        printf_to_char(ch,"That is not a valid class or skill.\n\r");
        return;
    }

    if (((skill = skill_lookup(arg1)) == -1) && (cl = class_lookup(arg1)) != -1) {
      for (class_name = 0; class_name < MAX_CLASS; class_name++) {
          if (!str_cmp(arg1, class_table[class_name].name))
            break;
      }
      if (class_name == MAX_CLASS) {
        send_to_char( "Please spell out the full class name.\n\r",ch);
        return;
      } else {
          sprintf(buf,"{CAvailable skill/spells for the class{c: {W%s{x\n\r",  class_table[cl].name);
          //tjw
          	//add_buf(output,buf);
		buffer_strcat ( output, buf );
          sprintf(buf,"{cSpells{W:{x\n\r");
                  	//add_buf(output,buf);
			buffer_strcat ( output, buf );
          for (level = 1 ;level <= LEVEL_ANCIENT;level++) {
              for (sn = 0; sn < MAX_SKILL; sn++) {

                  if (skill_table[sn].skill_level[cl] > LEVEL_ANCIENT)
                     continue;

                  if (skill_table[sn].skill_level[cl] != level)
                      continue;
                  if (skill_table[sn].spell_fun == spell_null)
                      continue;
                  sprintf(buf, "{B[{C%3d{B] {W%-18s{x",
                  level,
                  skill_table[sn].name);
                  spCount++;
                  //add_buf(output,buf);
		  buffer_strcat ( output, buf );
                  col++;
                  if (col==3)
 			{
                  sprintf(buf,"\n\r");
                  //add_buf(output,buf);
		  buffer_strcat ( output, buf );
                  col = 0;
                  }
              }
          }
          col = 0;
          sprintf(buf, "\n\r{CTotal spells found{g: {W%d{x\n\r",spCount);
          //add_buf(output,buf);
	  buffer_strcat ( output, buf );
          sprintf(buf, "{cSkills{W:{x\n\r");
          //add_buf(output,buf);
	  buffer_strcat ( output, buf );
          for (level = 1 ;level <= LEVEL_ANCIENT;level++) {
              for (sn = 0; sn < MAX_SKILL; sn++) {

                  if (skill_table[sn].skill_level[cl] > LEVEL_ANCIENT)
                      continue;
                  if (skill_table[sn].skill_level[cl] != level)
                      continue;
                  if (skill_table[sn].spell_fun != spell_null)
                      continue;
                 sprintf(buf, "{B[{C%3d{B] {W%-18s{x",
                  level,
                  skill_table[sn].name);
                  skCount++;
		//add_buf(output,buf);
		buffer_strcat ( output, buf );
                  col++;
                  if (col==3)
                  {
                        sprintf(buf,"\n\r");
                     	//add_buf(output,buf);
			buffer_strcat ( output, buf );
		col = 0;
   	}

              }
          }

          sprintf(buf, "\n\r{cBase Group{W: {W%s{x\n\r",class_table[cl].base_group);
          //add_buf(output,buf);
	  buffer_strcat ( output, buf );
          gn = 0; col = 0; level = 1;
          gn = group_lookup(class_table[class].base_group);
          sprintf(buf, "{CSkills included with {W%s{x.{x\n\r",class_table[cl].base_group);
          //add_buf(output, buf);
	  buffer_strcat ( output, buf );
              for (sn = 0; sn < MAX_IN_GROUP; sn++) {
                  if (group_table[gn].spells[sn] == NULL)
                      continue;
                  sprintf(buf,"{W%-18s{x",group_table[gn].spells[sn]);
                  //add_buf(output,buf);
		  buffer_strcat ( output, buf );
                  col++;
                  if (col==3)
                  {
                      sprintf(buf,"\n\r");
                     	//add_buf(output,buf);
			buffer_strcat ( output, buf );
                  }
              }
          sprintf(buf,"\n\r");
                     	//add_buf(output,buf);
			buffer_strcat ( output, buf );
          sprintf(buf, "{cDefault Group{W: {W%s{x\n\r",class_table[cl].default_group);
          //add_buf(output, buf);
	  buffer_strcat ( output, buf );
          gn = 0; col = 0; level = 1;
          gn = group_lookup(class_table[cl].default_group);
          sprintf(buf, "{CSkills included with {W%s{x.{x\n\r",class_table[cl].default_group);
                  //add_buf(output,buf);
		  buffer_strcat ( output, buf );
              for (sn = 0; sn < MAX_IN_GROUP; sn++) {
                  if (group_table[gn].spells[sn] == NULL)
                      continue;
                  sprintf(buf,"{W%-18s{x",group_table[gn].spells[sn]);
                  //add_buf(output,buf);
		  buffer_strcat ( output, buf );
                  col++;
                  if (col==3)
                      {
                      sprintf(buf,"\n\r");
                     	//add_buf(output,buf);
			buffer_strcat ( output, buf );
                     	col=0;
                     }
              }
          sprintf(buf,"\n\r");
                     	//add_buf(output,buf);
			buffer_strcat ( output, buf );
          sprintf(buf, "{CTotal skills found{g: {W%d{x\n\r",skCount);
          	//add_buf(output,buf);
		buffer_strcat ( output, buf );
          sprintf(buf, "{WTotal skills/spells found: {B%d{x\n\r",(spCount + skCount));
          //add_buf(output,buf);
	  buffer_strcat ( output, buf );
           //page_to_char( buf_string(output), ch );
	   page_to_char ( buffer_string ( output ), ch );
        //free_buf(output);
	buffer_free ( output );
        return;

      }
    }
    if (((skill = skill_lookup(arg1)) != -1) && (cl = class_lookup(arg1)) == -1) {
        for (gr = 1; gr < MAX_GROUP; gr++) {
          if (group_table[gr].name == NULL) {
            continue;
          }
          for (sp = 0; sp < MAX_IN_GROUP; sp++) {
            if (group_table[gr].spells[sp] == NULL) {
              continue;
            }
            if(group_table[gr].spells[sp] == skill_table[skill].name) {
              found = TRUE;
              sprintf(group,"%s",capitalize(group_table[gr].name));
              break;
            }
          }

        }
        sprintf(buf,"{CSettings for skill/spell{c: {W%s{x\n\r",skill_table[skill].name);
        //add_buf(output,buf);
	buffer_strcat ( output, buf );
        if (found){
          sprintf(buf,"{CIn the group: {W%s{C.{x\n\r",group);
          //add_buf(output,buf);
	  buffer_strcat ( output, buf );
          }
        else
          sprintf(buf,"{CIn the group: {WUnknown{C!{x\n\r");
          //add_buf(output,buf);
	  buffer_strcat ( output, buf );
        send_to_char("{cClass       Level Gain Wait-Time Min-Mana Rating{x\n\r",ch);
        sprintf(buf,"{W----------- ----- ---- --------- -------- ------{x\n\r");
        //add_buf(output,buf);
	buffer_strcat ( output, buf );
        for (class = 0;class < MAX_CLASS;class++) {
        if (skill_table[skill].skill_level[class] > LEVEL_ANCIENT)
            continue;
        sprintf(buf,"{G%-11s {g%5d %4d %9d %8d %6d{x\n\r",
        class_table[class].name,
        skill_table[skill].skill_level[class],
        skill_table[skill].rating[class],
        skill_table[skill].beats,
        skill_table[skill].min_mana,
        skill_table[skill].rating[class]);
        //add_buf(output,buf);
	buffer_strcat ( output, buf );
        }

        //page_to_char( buf_string(output), ch );
	page_to_char ( buffer_string ( output ), ch );
	
        //free_buf(output);
	buffer_free ( output );
        return;

    }


}

int compare_mdeaths(const void *v1, const void *v2)
{
    return (*(MOB_INDEX_DATA * *)v2)->killed - (*(MOB_INDEX_DATA * *)v1)->killed;
}

/*
 * Command by Rhaelar of Shattered Dreams, sdmud.com 7000
 * This command will show the starting stats of a given race.
 */
CH_CMD ( do_showrace )
{
    send_to_char ( "Still working on this command.\n\r", ch );
    return;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int race;
//    int i;

    strcpy ( buf, "" );
    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax: showrace [race]\n\r", ch );
        return;
    }
    if ( ( race = race_lookup ( arg ) ) == -1 )
    {
        send_to_char ( "Race not found.\n\r", ch );
        return;
    }
    send_to_char ( "The command works, now extend it!\n\r", ch );
    return;
}

CH_CMD ( do_mobdeaths )
{
//    extern int top_mob_index;
    //BUFFER *buffer;
    char buf[MSL];
    BUFFER2 *buffer;
    MOB_INDEX_DATA *pMobIndex;
    MOB_INDEX_DATA *mobs[top_mob_index];
    long vnum;
    int nMatch,
     count,
     i;
    bool found;

    found = FALSE;
    nMatch = 0;
    count = 0;
    i = 0;
    //buffer = new_buf();
    buffer = buffer_new ( 1337 );

/*    add_buf(buffer, "{R                  Todays Most Popular Monsters                   {x\n\r");
    add_buf(buffer, "{GNum  Mob Name                  Level  Area Name            Deaths{x\n\r");
    add_buf(buffer, "{m---  ------------------------- -----  -------------------- ------{x\n\r"); */
    buffer_strcat(buffer, "{R                  Todays Most Popular Monsters                   {x\n\r");
    buffer_strcat(buffer, "{GNum  Mob Name                  Level  Area Name            Deaths{x\n\r");
    buffer_strcat(buffer, "{m---  ------------------------- -----  -------------------- ------{x\n\r");
    
    for (vnum = 0; nMatch < top_mob_index; vnum++)
    {
	if ((pMobIndex = get_mob_index(vnum)) != NULL)
	{
	    nMatch++;

	    if (pMobIndex->killed > 0)
	    {
		mobs[count] = pMobIndex;
		count++;
		found = TRUE;
	    }
	}
    }

    qsort(mobs, count, sizeof(pMobIndex), compare_mdeaths);

    for (i = 0; i < count; i++)
    {
	if (i == 49)
	    break;
/*	buffer_printf(buffer, "%3d) %-25s %5d  %-20s {R%6d{x\n\r", i + 1, mobs[i]->short_descr, mobs[i]->level, mobs[i]->area->name, mobs[i]->killed); */
	sprintf ( buf, "%3d) %-25s %5d  %-20s {R%6d{x\n\r", i + 1, mobs[i]->short_descr, mobs[i]->level, mobs[i]->area->name, mobs[i]->killed);
	buffer_strcat ( buffer, buf );
    }

    if (!found)
	//add_buf(buffer, "\n\rNo Mobs listed yet.\n\r");
	buffer_strcat ( buffer, "\n\rNo mobs listed yet.\n\r" );

    //page_to_char(buf_string(buffer), ch);
    page_to_char ( buffer_string ( buffer ), ch );
    //free_buf(buffer);
    buffer_free ( buffer );

    return;
}

int compare_mkills(const void *v1, const void *v2)
{
    return (*(MOB_INDEX_DATA * *)v2)->kills - (*(MOB_INDEX_DATA * *)v1)->kills;
}

CH_CMD ( do_mobkills )
{
//    extern int top_mob_index;
    //BUFFER *buffer;
    char buf[MSL];
    BUFFER2 *buffer;
    MOB_INDEX_DATA *pMobIndex;
    MOB_INDEX_DATA *mobs[top_mob_index];
    long vnum;
    int nMatch,
     count,
     i;
    bool found;

    found = FALSE;
    nMatch = 0;
    count = 0;
    i = 0;
    //buffer = new_buf();
    buffer = buffer_new ( 1337 );

/*    add_buf(buffer, "{R                  Todays Most Dangerous Monsters                 {x\n\r");
    add_buf(buffer, "{GNum  Mob Name                  Level  Area Name            Deaths{x\n\r");
    add_buf(buffer, "{m---  ------------------------- -----  -------------------- ------{x\n\r"); */
    buffer_strcat(buffer, "{R                  Todays Most Dangerous Monsters                 {x\n\r");
    buffer_strcat(buffer, "{GNum  Mob Name                  Level  Area Name            Deaths{x\n\r");
    buffer_strcat(buffer, "{m---  ------------------------- -----  -------------------- ------{x\n\r");

    for (vnum = 0; nMatch < top_mob_index; vnum++)
    {
	if ((pMobIndex = get_mob_index(vnum)) != NULL)
	{
	    nMatch++;

	    if (pMobIndex->kills > 0)
	    {
		mobs[count] = pMobIndex;
		count++;
		found = TRUE;
	    }
	}
    }

    qsort(mobs, count, sizeof(pMobIndex), compare_mkills);

    for (i = 0; i < count; i++)
    {
	if (i == 49)
	    break;
	/*buffer_printf(buffer, "%3d) %-25s %5d  %-20s {R%6d{x\n\r", i + 1, mobs[i]->short_descr, mobs[i]->level, mobs[i]->area->name, mobs[i]->kills); */
	sprintf ( buf, "%3d) %-25s %5d  %-20s {R%6d{x\n\r", i + 1, mobs[i]->short_descr, mobs[i]->level, mobs[i]->area->name, mobs[i]->kills);
	buffer_strcat ( buffer, buf );
    }

    if (!found)
	//add_buf(buffer, "\n\rNo Mobs listed yet.\n\r");
	buffer_strcat ( buffer, "\n\rNo mobs listed yet.\n\r" );

    //page_to_char(buf_string(buffer), ch);
    page_to_char(buffer_string(buffer), ch);
    //free_buf(buffer);
    buffer_free ( buffer );
    return;
}

CH_CMD ( do_rtimer )
{
    char buf[MAX_STRING_LENGTH];

    if ( is_reboot_countdown == TRUE )
    {
        sprintf ( buf, "Current countdown has %d minute(s) remaining.\n\r",
                  ( reboot_countdown - 1 ) );
        send_to_char ( buf, ch );
        return;
    }
    else
    {
        send_to_char ( "There is no pending reboot at the moment.\n\r", ch );
        send_to_char ( "The next maintenance reboot will happen at:\n\r", ch );
        send_to_char ( "13:00 Central European, 04:00am Pacific Time, 06:00am Central and 07:00am Eastern (System time)\n\r", ch );
	send_to_char ( "..only if changes were done to the source of course.\n\r", ch );
        return;
    }
}

CH_CMD ( do_sstat )
{
char buf[MSL];
int stance;

send_to_char(
"            {dBasic Stances{x\n\r",ch);
send_to_char(
"{c+--------------------------------------+{x\n\r",ch);
sprintf( buf,
"{rViper{x:     {D%d{x\n\r{rCrane{x:     {D%d{x\n\r{rCrab{x:      {D%d{x\n\r{rMongoose{x:  {D%d{x\n\r{rBull{x:      {D%d{x\n\r",
ch->stance[1],ch->stance[2],ch->stance[3],ch->stance[4],ch->stance[5]);
send_to_char(buf,ch);
send_to_char( "\n\r          {dAdvanced Stances{x\n\r",ch);
send_to_char( "{c+--------------------------------------+{x\n\r",ch);
sprintf( buf,
"{rMantis{x:    {D%d{x\n\r{rDragon{x:    {D%d{x\n\r{rTiger{x:     {D%d{x\n\r{rMonkey{x:    {D%d{x\n\r{rSwallow{x:   {D%d{x\n\r",
ch->stance[6],ch->stance[7],ch->stance[8],ch->stance[9],ch->stance[10]);
send_to_char(buf,ch);
send_to_char( "{c+--------------------------------------+{x\n\r",ch);

stance = ch->stance[0];
if (stance >=1 && stance <= 10)
{
	sprintf(buf,"%s{x\n\r", stance_name[stance]);

send_to_char("{xCurrent stance:{! ",ch);
send_to_char(buf,ch);
}
else
{
send_to_char("{xCurrent stance:{! None{x\n\r",ch);
}


return;
}

CH_CMD ( do_stances )
{
    do_help ( ch, "stances" );
    return;
}

