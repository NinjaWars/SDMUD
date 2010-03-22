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
#include <time.h>
#include "sd.h"
#include "tables.h"
#include "recycle.h"

/* Stance stuff - Rhaelar */
bool	stance_checked = FALSE;
const	char*	stance_name []  =
{
	"None", "Viper", "Crane", "Crab", "Mongoose", "Bull",
	"Mantis", "Dragon", "Tiger", "Monkey", "Swallow"
};

//bool	check_critical  args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool secondary ) );

void skillstance( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf        [MIL];
    char bufskill   [25];
    int stance;

    stance = victim->stance[0];
    
    if (stance < 1 || stance > 10)
    {
	send_to_char("You aren't in a stance!\n\r",ch);
	return;
    }

         if (victim->stance[stance] <= 0  ) sprintf(bufskill,"completely unskilled in");
    else if (victim->stance[stance] <= 12 ) sprintf(bufskill,"an apprentice of");
    else if (victim->stance[stance] <= 26 ) sprintf(bufskill,"a trainee of");
    else if (victim->stance[stance] <= 39 ) sprintf(bufskill,"a student of");
    else if (victim->stance[stance] <= 51 ) sprintf(bufskill,"fairly experienced in");
    else if (victim->stance[stance] <= 64 ) sprintf(bufskill,"well trained in");
    else if (victim->stance[stance] <= 76 ) sprintf(bufskill,"highly skilled in");
    else if (victim->stance[stance] <= 85 ) sprintf(bufskill,"an expert of");
    else if (victim->stance[stance] <= 99 ) sprintf(bufskill,"a master of");
    else if (victim->stance[stance] >= 100) sprintf(bufskill,"a grand master of");
    else
    {

        send_to_char("You aren't in a stance!\n\r",ch);

        return;

    }

    if (ch == victim)
    	sprintf(buf,"You are %s the %s stance [%d%%].",bufskill,

						stance_name[stance],victim->stance[stance]);
    else
    	sprintf(buf,"$N is %s the %s stance [%d%%].",bufskill,

						stance_name[stance] ,victim->stance[stance]);
    act( buf,ch,NULL,victim,TO_CHAR);

    return;

}

int dambonus( CHAR_DATA *ch, CHAR_DATA *victim, int dam, bool realdam)
{
    if (dam < 1) return 0;
    if (ch->stance[0] < 1) return dam;

	if ( IS_STANCE(ch, STANCE_BULL) || IS_STANCE(ch, STANCE_DRAGON)
					|| IS_STANCE(ch, STANCE_TIGER) )

		{
		if (!can_counter(victim, realdam))
			{
 			if (realdam && !(stance_checked)) improve_stance(ch);
			dam += (dam * ch->stance[ch->stance[0]]) / 100;
			}
		}

	else if ( IS_STANCE(ch, STANCE_MONKEY) )
	{
	    int mindam = (int) (dam * 0.25);
	    dam *= (ch->stance[STANCE_MONKEY]+1) / 100;
	    if (dam < mindam) dam = mindam;
	}

	else if (ch->stance[ch->stance[0]] < 100)  dam = (int) (dam * 0.5);
    
    return dam;
}



int damreduce( CHAR_DATA *ch, CHAR_DATA *victim, int dam, bool realdam)
{

	if ( IS_STANCE(victim, STANCE_CRAB) || IS_STANCE(victim, STANCE_DRAGON)
				|| IS_STANCE(victim, STANCE_SWALLOW) )
    {
    	if (!can_counter(ch, realdam))
		{
			if (realdam && !(stance_checked)) improve_stance(victim);
			dam  = dam * 100 / (100 + victim->stance[victim->stance[0]]);
		}
    }
	return dam;
	}

bool can_counter( CHAR_DATA *ch, bool forreal )
{
    if (
				IS_STANCE(ch, STANCE_MONKEY))
		{
 		if (forreal && !(stance_checked)) improve_stance(ch);
		if (number_percent() < ch->stance[ STANCE_MONKEY ])
		return TRUE;
		}
    return FALSE;

}

bool can_bypass( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if (IS_STANCE(ch, STANCE_VIPER) ||
		IS_STANCE(ch, STANCE_MANTIS) ||
		IS_STANCE(ch, STANCE_TIGER))
			if (!can_counter(victim, TRUE))
			{
 				if (!(stance_checked))	improve_stance(ch);
				if (number_percent() < ch->stance[ch->stance[0]])
					return TRUE;
			}
    return FALSE;
}

void improve_stance( CHAR_DATA *ch )
{
//    OBJ_DATA *weapon;
    char buf        [MIL];
    char bufskill   [35];
    int dice1;
    int dice2;
    int dice3;
    int stance;
//	bool immune = FALSE;
//	int dam_type;
    dice1 = number_percent();
    dice2 = number_percent();
    dice3 = number_percent();

    stance = ch->stance[0];

    if (stance < 1 || stance > 10) return;

    if (ch->stance[stance] >= 100)
      {
        ch->stance[stance] = 100;
        return;
      }

	if (ch->fighting == NULL ||
		(IS_NPC(ch->fighting) && ch->fighting->pIndexData == NULL ))
		return;

   if ( IS_SET (ch->in_room->room_flags,ROOM_ARENA ) )
   return;

   if ( !double_stance )
   {
    if ( dice3 > 2) return;
    if ( dice1 < 24 ) return;
    if ( dice2 < 24 ) return;
   }
   else
   {
    if ( dice3 > 4) return;
    if ( dice1 < 12 ) return;
    if ( dice2 < 12 ) return;
   }   
    if ( (dice1 > ch->stance[stance] && dice2 > ch->stance[stance] ) ||

	 ( dice1 == 100 || dice2 == 100 ) )
		ch->stance[stance] += 1;
    else
      return;

    send_to_char("{CYour skill at this fighting stance has improved.{0\n\r",ch);

	stance_checked = TRUE;

         if (ch->stance[stance] == 1  ) sprintf(bufskill,"an apprentice of");
    else if (ch->stance[stance] == 13 ) sprintf(bufskill,"a trainee of");
    else if (ch->stance[stance] == 25 ) sprintf(bufskill,"a student of");
    else if (ch->stance[stance] == 38 ) sprintf(bufskill,"fairly experienced in");
    else if (ch->stance[stance] == 50 ) sprintf(bufskill,"well trained in");
    else if (ch->stance[stance] == 63 ) sprintf(bufskill,"highly skilled in");
    else if (ch->stance[stance] == 75 ) sprintf(bufskill,"an expert of");
    else if (ch->stance[stance] == 84 ) sprintf(bufskill,"a master of");
    else if (ch->stance[stance] == 100) sprintf(bufskill,"a grand master of");
    else return;

    sprintf(buf,"{cYou are now {C%s {cthe {R%s {cstance{x.\n\r",bufskill,
					stance_name[stance]);
    send_to_char(buf,ch);
    return;
}


void do_stance( CHAR_DATA *ch, char *argument )
{
    char            arg      [MIL];
    int             selection;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if (ch->stance[0] == -1)
	{
	    ch->stance[0] = -1;
	    send_to_char("You are not currently in a stance. For a list, type 'sstat'.\n\r",ch);
	}
	else
	{
	    ch->stance[0] = -1;
	    send_to_char("You relax from your fighting stance.\n\r",ch);
	    act("$n relaxes from $s fighting stance.",ch,NULL,NULL,TO_ROOM);
	}

	return;

    }
    if ( ch->stance[0] > 0)
    {
    	if ( !IS_NPC ( ch ) )
	do_stance ( ch, "" );
	else return;
    }

    if (!str_cmp(arg,"none")    ) {selection = -1;
	send_to_char("You drop into a default fighting stance.\n\r",ch);
	act("$n drops into a default fighting stance.",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"viper")   ) {selection = STANCE_VIPER;
	send_to_char("You arch your body into the viper fighting stance.\n\r",ch);
	act("$n arches $s body into the viper fighting stance.",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"crane")   ) {selection = STANCE_CRANE;
	send_to_char("You swing your body into the crane fighting stance.\n\r",ch);
	act("$n swings $s body into the crane fighting stance.",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"crab")    ) {selection = STANCE_CRAB;
	send_to_char("You squat down into the crab fighting stance.\n\r",ch);
	act("$n squats down into the crab fighting stance. ",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"mongoose")) {selection = STANCE_MONGOOSE;
	send_to_char("You twist into the mongoose fighting stance.\n\r",ch);
	act("$n twists into the mongoose fighting stance. ",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"bull")    ) {selection = STANCE_BULL;
	send_to_char("You hunch down into the bull fighting stance.\n\r",ch);
	act("$n hunches down into the bull fighting stance. ",ch,NULL,NULL,TO_ROOM);}
    else
    {
	if (!str_cmp(arg,"mantis") && ch->stance[STANCE_CRANE] >= 100 &&
	    ch->stance[STANCE_VIPER] >= 100)
	{
	    selection = STANCE_MANTIS;
	    send_to_char("You spin your body into the mantis fighting stance.\n\r",ch);
	    act("$n spins $s body into the mantis fighting stance.",ch,NULL,NULL,TO_ROOM);
	}
	else if (!str_cmp(arg,"dragon") && ch->stance[STANCE_BULL] >= 100 &&
	    ch->stance[STANCE_CRAB] >= 100)
	{
	    selection = STANCE_DRAGON;
	    send_to_char("You coil your body into the dragon fighting stance.\n\r",ch);
	    act("$n coils $s body into the dragon fighting stance.",ch,NULL,NULL,TO_ROOM);
	}
	else if (!str_cmp(arg,"tiger") && ch->stance[STANCE_BULL] >= 100 &&
	    ch->stance[STANCE_VIPER] >= 100)
	{
	    selection = STANCE_TIGER;
	    send_to_char("You lunge into the tiger fighting stance.\n\r",ch);
	    act("$n lunges into the tiger fighting stance.",ch,NULL,NULL,TO_ROOM);
	}	
	else if (!str_cmp(arg,"monkey") && ch->stance[STANCE_CRANE] >= 100 &&
	    ch->stance[STANCE_MONGOOSE] >= 100)
	{
	    selection = STANCE_MONKEY;
	    send_to_char("You rotate your body into the monkey fighting stance.\n\r",ch);
	    act("$n rotates $s body into the monkey fighting stance.",ch,NULL,NULL,TO_ROOM);
	}
	else if (!str_cmp(arg,"swallow") && ch->stance[STANCE_CRAB] >= 100 &&
	    ch->stance[STANCE_MONGOOSE] >= 100)
	{
	    selection = STANCE_SWALLOW;
	    send_to_char("You slide into the swallow fighting stance.\n\r",ch);
	    act("$n slides into the swallow fighting stance.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    send_to_char("Syntax is: stance <style>.\n\r",ch);
	    send_to_char("Stance being one of: None, Viper, Crane, Crab, Mongoose, Bull.\n\r",ch);
	    return;
	}
    }
    ch->stance[0] = selection;
    WAIT_STATE(ch,16);
    return;
}



bool check_stance ( CHAR_DATA *ch  )
{
if (ch->stance[0] < 1 || ch->stance[0] > 10)
	return FALSE;

	if ( ch->stance[0] == 9 ) 						
		if ((ch->fighting->stance[0] <= 0) || (ch->fighting->stance[0] == 9))
			return FALSE;
	return TRUE;
} 



/* Stance stuff done - Rhaelar */

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update ( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
        ch_next = ch->next;

        if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
            continue;

        if ( IS_AWAKE ( ch ) && ch->in_room == victim->in_room )
        {
	  if ( !IS_NPC ( ch ) && !IS_NPC ( victim ) )
	  {
/*
	   if ( IS_SET ( victim->act2, PLR2_PKSAFE ) )
	   {
		stop_fighting ( ch, FALSE );
		send_to_char ( "They are safe right now.\n\r", ch );
	   }
	   else */
	if ( IS_SET ( ch->act2, PLR2_PKSAFE ) ) 
	{
	    REMOVE_BIT ( ch->act2, PLR2_PKSAFE );
	    ch->pcdata->pksafetime = 0;
	}
/*    
    if ( reboot_countdown == 0 )
    {
	stop_fighting ( ch, TRUE );
        send_to_char
            ( "{YSorry, the mud is getting ready for a reboot.\n\rPlease finish your fight afterwards.{x\n\r",
              ch );
	return;
    }
*/		
		pkviolent ( ch, 6 );
	}
            multi_hit ( ch, victim, TYPE_UNDEFINED );
            process_shields ( ch, victim );
        }
        else
            stop_fighting ( ch, FALSE );

        if ( ( victim = ch->fighting ) == NULL )
            continue;

        /*
         * Fun for the whole family!
         */
        check_assist ( ch, victim );

        if ( IS_NPC ( ch ) )
        {
            if ( HAS_TRIGGER ( ch, TRIG_FIGHT ) )
                mp_percent_trigger ( ch, victim, NULL, NULL, TRIG_FIGHT );
            if ( HAS_TRIGGER ( ch, TRIG_HPCNT ) )
                mp_hprct_trigger ( ch, victim );
        }
    }

    return;
}

/* for auto assisting */
void check_assist ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    CHAR_DATA *rch, *rch_next;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
    {
        rch_next = rch->next_in_room;

        if ( IS_AWAKE ( rch ) && rch->fighting == NULL &&
             can_see ( rch, victim ) )
        {

            /* quick check for ASSIST_PLAYER */
            if ( !IS_NPC ( ch ) && IS_NPC ( rch ) &&
                 IS_SET ( rch->off_flags, ASSIST_PLAYERS ) &&
                 rch->level + 6 > victim->level )
            {
                do_emote ( rch, "{Rscreams and attacks!{x" );
                multi_hit ( rch, victim, TYPE_UNDEFINED );
                continue;
            }

            /* PCs next */
            if ( !IS_NPC ( ch ) || IS_AFFECTED ( ch, AFF_CHARM ) )
            {
                if ( ( ( !IS_NPC ( rch ) &&
                         IS_SET ( rch->act, PLR_AUTOASSIST ) ) ||
                       IS_AFFECTED ( rch, AFF_CHARM ) ) &&
                     is_same_group ( ch, rch ) && !is_safe ( rch, victim ) )
                    multi_hit ( rch, victim, TYPE_UNDEFINED );

                continue;
            }

            /* now check the NPC cases */

            if ( IS_NPC ( ch ) && !IS_AFFECTED ( ch, AFF_CHARM ) )

            {
                if ( ( IS_NPC ( rch ) &&
                       IS_SET ( rch->off_flags, ASSIST_ALL ) ) ||
                     ( IS_NPC ( rch ) && rch->group &&
                       rch->group == ch->group ) || ( IS_NPC ( rch ) &&
                                                      rch->race == ch->race &&
                                                      IS_SET ( rch->off_flags,
                                                               ASSIST_RACE ) )
                     || ( IS_NPC ( rch ) &&
                          IS_SET ( rch->off_flags, ASSIST_ALIGN ) &&
                          ( ( IS_GOOD ( rch ) && IS_GOOD ( ch ) ) ||
                            ( IS_EVIL ( rch ) && IS_EVIL ( ch ) ) ||
                            ( IS_NEUTRAL ( rch ) && IS_NEUTRAL ( ch ) ) ) ) ||
                     ( rch->pIndexData == ch->pIndexData &&
                       IS_SET ( rch->off_flags, ASSIST_VNUM ) ) )

                {
                    CHAR_DATA *vch;
                    CHAR_DATA *target;
                    int number;

                    if ( number_bits ( 1 ) == 0 )
                        continue;

                    target = NULL;
                    number = 0;
                    for ( vch = ch->in_room->people; vch; vch = vch->next )
                    {
                        if ( can_see ( rch, vch ) &&
                             is_same_group ( vch, victim ) &&
                             number_range ( 0, number ) == 0 )
                        {
                            target = vch;
                            number++;
                        }
                    }

                    if ( target != NULL )
                    {
                        do_emote ( rch, "{Rscreams and attacks!{x" );
                        multi_hit ( rch, target, TYPE_UNDEFINED );
                    }
                }
            }
        }
    }
}

/*
 * Do one group of attacks.
 */
void multi_hit ( CHAR_DATA * ch, CHAR_DATA * victim, int dt )
{
    int chance;

    stance_checked = FALSE; /* ensure stance can improve only once per round */

    
    /* decrement the wait */
    if ( ch->desc == NULL )
        ch->wait = UMAX ( 0, ch->wait - PULSE_VIOLENCE );

    if ( ch->desc == NULL )
        ch->daze = UMAX ( 0, ch->daze - PULSE_VIOLENCE );

    /* no attacks for stunnies -- just a check */
    if ( ch->position < POS_RESTING )
        return;

    if ( ch->stunned )
    {
        ch->stunned--;
        if ( !ch->stunned )
        {
            send_to_char ( "You regain your equilibrium.\n\r", ch );
            act ( "$n regains $m equilibrium.", ch, NULL, NULL, TO_ROOM );
        }
        return;
    }

    if ( IS_NPC ( ch ) )
    {
        mob_hit ( ch, victim, dt );
        return;
    }

    one_hit ( ch, victim, dt, FALSE );

    if ( get_eq_char ( ch, WEAR_SECONDARY ) )
    {
        chance = ( get_skill ( ch, gsn_dual_wield ) / 3 ) * 2;
        chance += 33;
        if ( number_percent (  ) < chance )
        {
            one_hit ( ch, victim, dt, TRUE );
            if ( get_skill ( ch, gsn_dual_wield ) != 0 &&
                 ( !IS_NPC ( ch ) &&
                   ch->level >=
                   skill_table[gsn_dual_wield].skill_level[ch->class] ) )
            {
                check_improve ( ch, gsn_dual_wield, TRUE, 1 );
            }
        }
        if ( ch->fighting != victim )
            return;
    }

    if ( ch->fighting != victim )
        return;

    if ( dt == gsn_strike )
        return;

    if ( IS_AFFECTED ( ch, AFF_HASTE ) )
        one_hit ( ch, victim, dt, FALSE );

    if ( ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle ||
         dt == gsn_whirlwind || dt == gsn_strike || dt == gsn_assassinate )
        return;

    chance = get_skill ( ch, gsn_second_attack ) / 1.1;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance /= 2;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_second_attack, TRUE, 5 );
        if ( ch->fighting != victim )
            return;
    }
    else
    {
        return;
    }

    chance = get_skill ( ch, gsn_third_attack ) / 1.2;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance /= 2;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_third_attack, TRUE, 6 );
        if ( ch->fighting != victim )
            return;
    }
    else
    {
        return;
    }

    chance = get_skill ( ch, gsn_fourth_attack ) / 1.3;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance /= 3;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_fourth_attack, TRUE, 6 );
        if ( ch->fighting != victim )
            return;
    }
    else
    {
        return;
    }

    chance = get_skill ( ch, gsn_fifth_attack ) / 1.4;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance = 0;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_fifth_attack, TRUE, 6 );
        if ( ch->fighting != victim )
            return;
    }

    chance = get_skill ( ch, gsn_sixth_attack ) / 1.5;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance = 0;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_sixth_attack, TRUE, 6 );
        if ( ch->fighting != victim )
            return;
    }
    chance = get_skill ( ch, gsn_seventh_attack ) / 1.6;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance = 0;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_seventh_attack, TRUE, 6 );
        if ( ch->fighting != victim )
            return;
    }
    chance = get_skill ( ch, gsn_eighth_attack ) / 1.7;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance = 0;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_eighth_attack, TRUE, 6 );
        if ( ch->fighting != victim )
            return;
    }
    chance = get_skill ( ch, gsn_ninth_attack ) / 1.8;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance = 0;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_ninth_attack, TRUE, 6 );
        if ( ch->fighting != victim )
            return;
    }
    chance = get_skill ( ch, gsn_tenth_attack ) / 2;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) )
        chance = 0;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        check_improve ( ch, gsn_tenth_attack, TRUE, 6 );
        if ( ch->fighting != victim )
            return;
    }
    if ( dt == gsn_ambush )
        return;
    return;
}

/* procedure for all mobile attacks */
void mob_hit ( CHAR_DATA * ch, CHAR_DATA * victim, int dt )
{
    int chance, number;
    CHAR_DATA *vch, *vch_next;

    one_hit ( ch, victim, dt, FALSE );

    if ( ch->fighting != victim )
        return;

    if ( ch->stunned )
        return;

    /* Area attack -- BALLS nasty! */

    if ( IS_SET ( ch->off_flags, OFF_AREA_ATTACK ) )
    {
        for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;
            if ( ( vch != victim && vch->fighting == ch ) )
                one_hit ( ch, vch, dt, FALSE );
        }
    }

    if ( ch->fighting != victim )
        return;

    if ( get_eq_char ( ch, WEAR_SECONDARY ) )
    {
        chance = ( get_skill ( ch, gsn_dual_wield ) / 3 ) * 2;
        chance += 33;
        if ( number_percent (  ) < chance )
        {
            one_hit ( ch, victim, dt, TRUE );
        }
        if ( ch->fighting != victim )
            return;
    }

    if ( IS_AFFECTED ( ch, AFF_HASTE ) ||
         ( IS_SET ( ch->off_flags, OFF_FAST ) &&
           !IS_AFFECTED ( ch, AFF_SLOW ) ) )
        one_hit ( ch, victim, dt, FALSE );

    if ( ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle ||
         dt == gsn_whirlwind || dt == gsn_strike || dt == gsn_assassinate )
        return;

    chance = get_skill ( ch, gsn_second_attack ) / 2;

    if ( IS_AFFECTED ( ch, AFF_SLOW ) && !IS_SET ( ch->off_flags, OFF_FAST ) )
        chance /= 2;

    if ( number_percent (  ) < chance )
    {
        one_hit ( ch, victim, dt, FALSE );
        if ( ch->fighting != victim )
            return;
        chance = get_skill ( ch, gsn_third_attack ) / 2;

        if ( IS_AFFECTED ( ch, AFF_SLOW ) &&
             !IS_SET ( ch->off_flags, OFF_FAST ) )
            chance /= 2;

        if ( number_percent (  ) < chance )
        {
            one_hit ( ch, victim, dt, FALSE );
            if ( ch->fighting != victim )
                return;

            chance = get_skill ( ch, gsn_fourth_attack ) / 2;

            if ( IS_AFFECTED ( ch, AFF_SLOW ) &&
                 !IS_SET ( ch->off_flags, OFF_FAST ) )
                chance /= 3;

            if ( number_percent (  ) < chance )
            {
                one_hit ( ch, victim, dt, FALSE );
                if ( ch->fighting != victim )
                    return;

                chance = get_skill ( ch, gsn_fifth_attack ) / 2;

                if ( IS_AFFECTED ( ch, AFF_SLOW ) &&
                     !IS_SET ( ch->off_flags, OFF_FAST ) )
                    chance = 0;

                if ( number_percent (  ) < chance )
                {
                    one_hit ( ch, victim, dt, FALSE );
                    if ( ch->fighting != victim )
                        return;
                }

                chance = get_skill ( ch, gsn_sixth_attack ) / 2;

                if ( IS_AFFECTED ( ch, AFF_SLOW ) &&
                     !IS_SET ( ch->off_flags, OFF_FAST ) )
                    chance = 0;

                if ( number_percent (  ) < chance )
                {
                    one_hit ( ch, victim, dt, FALSE );
                    if ( ch->fighting != victim )
                        return;
                }

                chance = get_skill ( ch, gsn_seventh_attack ) / 2;

                if ( IS_AFFECTED ( ch, AFF_SLOW ) &&
                     !IS_SET ( ch->off_flags, OFF_FAST ) )
                    chance = 0;

                if ( number_percent (  ) < chance )
                {
                    one_hit ( ch, victim, dt, FALSE );
                    if ( ch->fighting != victim )
                        return;
                }
            }
        }
    }

    /* oh boy!  Fun stuff! */

    if ( ch->wait > 0 )
        return;

    number = number_range ( 0, 2 );

    if ( number == 1 && IS_SET ( ch->act, ACT_MAGE ) )
    {
        /*  { mob_cast_mage(ch,victim); return; } */ ;
    }

    if ( number == 2 && IS_SET ( ch->act, ACT_CLERIC ) )
    {
        /* { mob_cast_cleric(ch,victim); return; } */ ;
    }

    /* now for the skills */

    number = number_range ( 0, 8 );

    switch ( number )
    {
        case ( 0 ):
            if ( IS_SET ( ch->off_flags, OFF_BASH ) )
                do_bash ( ch, "" );
            break;

        case ( 1 ):
            if ( IS_SET ( ch->off_flags, OFF_BERSERK ) &&
                 !IS_AFFECTED ( ch, AFF_BERSERK ) )
                do_berserk ( ch, "" );
            break;

        case ( 2 ):
            if ( IS_SET ( ch->off_flags, OFF_DISARM ) ||
                 ( get_weapon_sn ( ch ) != gsn_hand_to_hand &&
                   ( IS_SET ( ch->act, ACT_WARRIOR ) ||
                     IS_SET ( ch->act, ACT_VAMPIRE ) ||
                     IS_SET ( ch->act, ACT_THIEF ) ) ) )
                do_disarm ( ch, "" );
            break;

        case ( 3 ):
            if ( IS_SET ( ch->off_flags, OFF_KICK ) )
                do_kick ( ch, "" );
            break;

        case ( 4 ):
            if ( IS_SET ( ch->off_flags, OFF_KICK_DIRT ) )
                do_dirt ( ch, "" );
            break;

        case ( 5 ):
            if ( IS_SET ( ch->off_flags, OFF_TAIL ) )
            {
                /* do_tail(ch,"") */ ;
            }
            break;

        case ( 6 ):
            if ( IS_SET ( ch->off_flags, OFF_TRIP ) )
                do_trip ( ch, "" );
            break;

        case ( 7 ):
            if ( IS_SET ( ch->off_flags, OFF_CRUSH ) )
            {
                /* do_crush(ch,"") */ ;
            }
            break;
        case ( 8 ):
            if ( IS_SET ( ch->off_flags, OFF_BACKSTAB ) )
            {
                do_backstab ( ch, "" );
            }
    }
}

/*
 * Hit one guy once.
 */
void one_hit ( CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int sn, skill;
    int dam_type;
    bool result;

    sn = -1;

    /* just in case */
    if ( victim == ch || ch == NULL || victim == NULL )
        return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
        return;
	
    /*
     * Figure out the type of damage message.
     * if secondary == true, use the second weapon.
     */
    if ( !secondary )
        wield = get_eq_char ( ch, WEAR_WIELD );
    else
        wield = get_eq_char ( ch, WEAR_SECONDARY );
    if ( dt == TYPE_UNDEFINED )
    {
        dt = TYPE_HIT;
        if ( wield != NULL && wield->item_type == ITEM_WEAPON )
            dt += wield->value[3];
        else
            dt += ch->dam_type;
    }

    if ( dt < TYPE_HIT )
        if ( wield != NULL )
            dam_type = attack_table[wield->value[3]].damage;
        else
            dam_type = attack_table[ch->dam_type].damage;
    else
        dam_type = attack_table[dt - TYPE_HIT].damage;

    if ( dam_type == -1 )
        dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn ( ch );
    skill = 20 + get_weapon_skill ( ch, sn );

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC ( ch ) )
    {
        thac0_00 = 20;
        thac0_32 = -4;          /* as good as a thief */
        if ( IS_SET ( ch->act, ACT_VAMPIRE ) )
            thac0_32 = -40;
        else if ( IS_SET ( ch->act, ACT_DRUID ) )
            thac0_32 = 0;
        else if ( IS_SET ( ch->act, ACT_RANGER ) )
            thac0_32 = -8;
        else if ( IS_SET ( ch->act, ACT_WARRIOR ) )
            thac0_32 = -20;
        else if ( IS_SET ( ch->act, ACT_THIEF ) )
            thac0_32 = -8;
        else if ( IS_SET ( ch->act, ACT_CLERIC ) )
            thac0_32 = 4;
        else if ( IS_SET ( ch->act, ACT_MAGE ) )
            thac0_32 = 12;
    }
    else
    {
        thac0_00 = class_table[ch->class].thac0_00;
        thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0 = interpolate ( ch->level, thac0_00, thac0_32 );

    if ( thac0 < 0 )
        thac0 = thac0 / 2;

    if ( thac0 < -5 )
        thac0 = -5 + ( thac0 + 5 ) / 2;

    thac0 -= GET_HITROLL ( ch ) * skill / 100;
    thac0 += 5 * ( 100 - skill ) / 100;

    if ( dt == gsn_backstab )
        thac0 -= 3 * ( 100 - get_skill ( ch, gsn_backstab ) );

    if ( dt == gsn_circle )
        thac0 -= 3.5 * ( 100 - get_skill ( ch, gsn_circle ) );

    if ( dt == gsn_whirlwind )
        thac0 -= 10 * ( 100 - get_skill ( ch, gsn_whirlwind ) );

    if ( dt == gsn_strike )
        thac0 -= 10 * ( 100 - get_skill ( ch, gsn_strike ) );

    if ( dt == gsn_ambush )
        thac0 -= 10 * ( 100 - get_skill ( ch, gsn_ambush ) );
    if ( dt == gsn_assassinate )
        thac0 -= 5 * ( 100 - get_skill ( ch, gsn_assassinate ) );

    switch ( dam_type )
    {
        case ( DAM_PIERCE ):
            victim_ac = GET_AC ( victim, AC_PIERCE ) / 25;
            break;
        case ( DAM_BASH ):
            victim_ac = GET_AC ( victim, AC_BASH ) / 25;
            break;
        case ( DAM_SLASH ):
            victim_ac = GET_AC ( victim, AC_SLASH ) / 25;
            break;
        default:
            victim_ac = GET_AC ( victim, AC_EXOTIC ) / 25;
            break;
    };

    if ( victim_ac < -15 )
        victim_ac = ( victim_ac + 15 ) / 5 - 15;

    if ( !can_see ( ch, victim ) )
        victim_ac -= 12;

    if ( victim->position < POS_FIGHTING )
        victim_ac += 12;

    if ( victim->position < POS_RESTING )
        victim_ac += 18;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits ( 5 ) ) >= 20 )
        ;

    if ( diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
        /* Miss. */
        damage ( ch, victim, 0, dt, dam_type, TRUE, 0 );
        tail_chain (  );
        return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC ( ch ) && ( !ch->pIndexData->new_format || wield == NULL ) )
        if ( !ch->pIndexData->new_format )
        {
            dam = number_range ( ch->level / 2, ch->level * 3 / 2 );
            if ( wield != NULL )
                dam += dam / 2;
        }
        else
            dam = dice ( ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE] );

    else
    {
        if ( sn != -1 )
            check_improve ( ch, sn, TRUE, 5 );
        if ( wield != NULL )
        {
            if ( wield->clan )
            {
                dam = dice ( ch->level / 3, 3 ) * skill / 100;
            }
            else
            {
                if ( wield->pIndexData->new_format )
                    dam =
                        dice ( wield->value[1],
                               wield->value[2] ) * skill / 100;
                else
                    dam =
                        number_range ( wield->value[1] * skill / 100,
                                       wield->value[2] * skill / 100 );
            }

            if ( get_eq_char ( ch, WEAR_SHIELD ) == NULL )  /* no shield = more */
                dam = dam * 11 / 10;

            /* sharpness! */
            if ( IS_WEAPON_STAT ( wield, WEAPON_SHARP ) )
            {
                int percent;

                if ( ( percent = number_percent (  ) ) <= ( skill / 8 ) )
                    dam = 2 * dam + ( dam * 2 * percent / 100 );
            }
            if ( IS_WEAPON_STAT ( wield, WEAPON_VORPAL ) )
            {
                int percent;

                if ( ( percent = number_percent (  ) ) <= ( skill / 8 ) )
                    dam = 3 * dam + ( dam * 3 * percent / 105 );
            }
        }

        else
            dam =
                number_range ( 1 + 4 * skill / 100,
                               2 * ch->level / 3 * skill / 100 );
    }

    /*
     * Bonuses.
     */
    if ( get_skill ( ch, gsn_enhanced_damage ) > 0 )
    {
        diceroll = number_percent (  );
        if ( diceroll <= get_skill ( ch, gsn_enhanced_damage ) )
        {
            check_improve ( ch, gsn_enhanced_damage, TRUE, 6 );
            dam += 2 * ( dam * diceroll / 300 );
        }
    }
    if ( check_critical ( ch, victim ) )
        dam +=
            dam *
            ( ( number_percent (  ) / 2 +
                get_skill ( ch, gsn_critical ) ) / 2 ) / 50;

    if ( !IS_AWAKE ( victim ) )
        dam *= 2;
    else if ( victim->position < POS_FIGHTING )
        dam = dam * 3 / 2;

    if ( dt == gsn_backstab && wield != NULL )
    {
        if ( wield->value[0] != 2 )
            dam *= 2 + ( ch->level / 14 );
        else
            dam *= 2 + ( ch->level / 12 );
    }
    if ( dt == gsn_circle && wield != NULL )
    {
        if ( wield->value[0] != 2 )
            dam *= 5 + ( ch->level / 16 );
        else
            dam *= 5 + ( ch->level / 14 );
        if ( dt == gsn_whirlwind && wield != NULL )
        {
            if ( wield->value[0] != 2 )
                dam *= 2 + ( ch->level / 10 );
            else
                dam *= 2 + ( ch->level / 8 );
        }
    }

    if ( dt == gsn_assassinate && wield != NULL )
    {
/*        if ( IS_SHIELDED ( ch, SHD_SWALK ) )
        {
            send_to_char
                ( "The essence of surprise strengthens your attack!\n\r",
                  ch );
            dam *= 2;
        } */
        if ( chance ( 20 ) )
        {
            act ( "Someone nearly ends $n's miserable life!", victim, NULL,
                  NULL, TO_ROOM );
            act ( "$n barely misses piercing your vital organs!", ch, NULL,
                  victim, TO_VICT );
            printf_to_char ( ch,
                             "{xYour assassination attempt barely misses its mark{x!!\n\r" );
            dam *= 2 + ( ch->level / 12 );
        }
        else
            dam *= 2 + ( ch->level / 10 );
    }
/*    if ( IS_SHIELDED ( ch, SHD_SWALK ) )
    {
        affect_strip ( ch, gsn_swalk );
    } */

    if ( dt == gsn_strike && wield != NULL )
        dam *= ( ch->level / 33 );  /* max * 3 */

    if ( dt == gsn_ambush && wield != NULL )
        dam *= 2 + ( ch->level / 50 );  /* max * 4 */

    dam += GET_DAMROLL ( ch ) * UMIN ( 100, skill ) / 100;

    if ( dam <= 0 )
        dam = 1;

/*    result = damage( ch, victim, dam, dt, dam_type, TRUE );*/

    if ( !check_counter ( ch, victim, dam, dt ) )
        result = damage ( ch, victim, dam, dt, dam_type, TRUE, 1 );

    else
        return;

    /* but do we have a funky weapon? */
    if ( result && wield != NULL )
    {
        int dam;

        if ( ch->fighting == victim &&
             IS_WEAPON_STAT ( wield, WEAPON_POISON ) )
        {
            int level;
            AFFECT_DATA *poison, af;

            if ( ( poison =
                   affect_find ( wield->affected, gsn_poison ) ) == NULL )
                level = wield->level;
            else
                level = poison->level;

            if ( !saves_spell ( level / 2, victim, DAM_POISON ) )
            {
                send_to_char
                    ( "{cYou feel {ypoison{c coursing through your veins.\n\r{x",
                      victim );
                act ( "$n is {ypoisoned{x by the venom on $p.", victim, wield,
                      NULL, TO_ROOM );

                af.where = TO_AFFECTS;
                af.type = gsn_poison;
                af.level = level * 3 / 4;
                af.duration = level / 2;
                af.location = APPLY_STR;
                af.modifier = -1;
                af.bitvector = AFF_POISON;
                affect_join ( victim, &af );
            }

            /* weaken the poison if it's temporary */
            if ( poison != NULL )
            {
                poison->level = UMAX ( 0, poison->level - 2 );
                poison->duration = UMAX ( 0, poison->duration - 1 );

                if ( poison->level == 0 || poison->duration == 0 )
                    act ( "The {ypoison{x on $p has worn off.", ch, wield,
                          NULL, TO_CHAR );
            }
        }

        if ( ch->fighting == victim &&
             IS_WEAPON_STAT ( wield, WEAPON_MANADRAIN ) )
        {
            dam = wield->level / 3;
            xact_new ( "{k$p draws mana from $n.{x", victim, wield, ch,
                       TO_VICT, POS_RESTING, VERBOSE_FLAGS );
            xact_new ( "{iYou feel $p drawing your mana away.{x", victim,
                       wield, ch, TO_VICT, POS_RESTING, VERBOSE_FLAGS ); 
/*            act_new ( "{k$p draws mana from $n.{x", victim, wield, ch, TO_VICT, POS_RESTING );
            act_new ( "{iYou feel $p drawing your mana away.{x", victim, wield, ch, TO_VICT, POS_RESTING ); */
            victim->mana = victim->mana - dam;
            ch->mana += dam / 2;
        }

        if ( ch->fighting == victim &&
             IS_WEAPON_STAT ( wield, WEAPON_VAMPIRIC ) )
        {
            dam = wield->level * 1.50;
            xact_new ( "{k$p draws life from $n.{x", victim, wield, ch,
                       TO_VICT, POS_RESTING, VERBOSE_FLAGS );
            xact_new ( "{iYou feel $p drawing your life away.{x", victim,
                       wield, NULL, TO_CHAR, POS_RESTING, VERBOSE_FLAGS ); 
/*            act_new ( "{k$p draws life from $n.{x", victim, wield, ch, TO_VICT, POS_RESTING );
            act_new ( "{iYou feel $p drawing your life away.{x", victim, wield, NULL, TO_CHAR, POS_RESTING ); */
            damage_old ( ch, victim, dam, 0, DAM_NEGATIVE, FALSE, 1 );
            ch->alignment = UMAX ( -1000, ch->alignment - 1 );
            if ( ch->pet != NULL )
                ch->pet->alignment = ch->alignment;
            ch->hit += dam / 10;
        }

        if ( ch->fighting == victim &&
             IS_WEAPON_STAT ( wield, WEAPON_FLAMING ) )
        {
            dam = number_range ( 1, wield->level / 4 + 1 );
            xact_new ( "{k$n is {rburned{k by $p.{x", victim, wield, ch,
                       TO_VICT, POS_RESTING, VERBOSE_FLAGS );
            xact_new ( "{i$p {rsears{i your flesh.{x", victim, wield, NULL,
                       TO_CHAR, POS_RESTING, VERBOSE_FLAGS ); 
/*            act_new ( "{k$n is {rburned{k by $p.{x", victim, wield, ch, TO_VICT, POS_RESTING );
            act_new ( "{i$p {rsears{i your flesh.{x", victim, wield, NULL, TO_CHAR, POS_RESTING ); */
            fire_effect ( ( void * ) victim, wield->level / 2, dam,
                          TARGET_CHAR );
            damage ( ch, victim, dam, 0, DAM_FIRE, FALSE, 1 );
        }

        if ( ch->fighting == victim &&
             IS_WEAPON_STAT ( wield, WEAPON_FROST ) )
        {
            dam = number_range ( 1, wield->level / 6 + 2 );
            xact_new ( "{k$p {cfreezes{k $n.{x", victim, wield, ch, TO_VICT,
                       POS_RESTING, VERBOSE_FLAGS );
            xact_new
                ( "{iThe {Ccold{i touch of $p surrounds you with {Cice.{x",
                  victim, wield, NULL, TO_CHAR, POS_RESTING, VERBOSE_FLAGS ); 
/*            act_new ( "{k$p {cfreezes{k $n.{x", victim, wield, ch, TO_VICT, POS_RESTING );
            act_new ( "{iThe {Ccold{i touch of $p surrounds you with {Cice.{x", victim, wield, NULL, TO_CHAR, POS_RESTING ); */
            cold_effect ( victim, wield->level / 2, dam, TARGET_CHAR );
            damage ( ch, victim, dam, 0, DAM_COLD, FALSE, 1 );
        }

        if ( ch->fighting == victim &&
             IS_WEAPON_STAT ( wield, WEAPON_SHOCKING ) )
        {
            dam = number_range ( 1, wield->level / 5 + 2 );
            xact_new ( "{k$n is struck by {Ylightning{k from $p.{x", victim,
                       wield, ch, TO_VICT, POS_RESTING, VERBOSE_FLAGS );
            xact_new ( "{iYou are {Yshocked{i by $p.{x", victim, wield, NULL,
                       TO_CHAR, POS_RESTING, VERBOSE_FLAGS ); 
/*            act_new ( "{k$n is struck by {Ylightning{k from $p.{x", victim, wield, ch, TO_VICT, POS_RESTING );
            act_new ( "{iYou are {Yshocked{i by $p.{x", victim, wield, NULL, TO_CHAR, POS_RESTING ); */
            shock_effect ( victim, wield->level / 2, dam, TARGET_CHAR );
            damage ( ch, victim, dam, 0, DAM_LIGHTNING, FALSE, 1 );

        }

    }

    tail_chain (  );
    return;
}

void process_shields ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    int dam, dt;
    int count = 0, total = 0;

    if ( IS_SHIELDED ( victim, SHD_POISON ) )
    {
        if ( ch->fighting != victim )
            return;
        if ( !IS_SHIELDED ( ch, SHD_POISON ) )
        {
            dt = skill_lookup ( "poisonshield" );
            dam = number_range ( 5, 15 );
            total +=
                xdamage ( victim, ch, dam, dt, DAM_POISON, TRUE,
                          VERBOSE_SHIELD, 1 );
            count++;
        }
    }
    if ( IS_SHIELDED ( victim, SHD_ICE ) )
    {
        if ( ch->fighting != victim )
            return;
        if ( !IS_SHIELDED ( ch, SHD_ICE ) )
        {
            dt = skill_lookup ( "iceshield" );
            dam = number_range ( 15, 25 );
            total +=
                xdamage ( victim, ch, dam, dt, DAM_COLD, TRUE,
                          VERBOSE_SHIELD, 1 );
            count++;
        }
    }
    if ( IS_SHIELDED ( victim, SHD_FIRE ) )
    {
        if ( ch->fighting != victim )
            return;
        if ( !IS_SHIELDED ( ch, SHD_FIRE ) )
        {
            dt = skill_lookup ( "fireshield" );
            dam = number_range ( 25, 35 );
            total +=
                xdamage ( victim, ch, dam, dt, DAM_FIRE, TRUE,
                          VERBOSE_SHIELD, 1 );
            count++;

        }
    }
    if ( IS_SHIELDED ( victim, SHD_SHOCK ) )
    {
        if ( ch->fighting != victim )
            return;
        if ( !IS_SHIELDED ( ch, SHD_SHOCK ) )
        {
            dt = skill_lookup ( "shockshield" );

            dam = number_range ( 35, 45 );
            total +=
                xdamage ( victim, ch, dam, dt, DAM_POISON, TRUE,
                          VERBOSE_SHIELD, 1 );
            count++;
        }
    }
    if ( IS_SHIELDED ( victim, SHD_ACID ) )
    {
        if ( ch->fighting != victim )
            return;
        if ( !IS_SHIELDED ( ch, SHD_ACID ) )
        {
            dt = skill_lookup ( "acidshield" );
            dam = number_range ( 45, 85 );
            total +=
                xdamage ( victim, ch, dam, dt, DAM_POISON, TRUE,
                          VERBOSE_SHIELD, 1 );
            count++;
        }
    }
    if ( IS_SHIELDED ( victim, SHD_BRIAR ) )
    {
        if ( ch->fighting != victim )
            return;
        if ( !IS_SHIELDED ( ch, SHD_BRIAR ) )
        {
            dt = skill_lookup ( "briarshield" );
            dam = number_range ( 10, 25 );
            total +=
                xdamage ( victim, ch, dam, dt, DAM_POISON, TRUE,
                          VERBOSE_SHIELD, 1 );
            count++;
        }
    }

    if ( ch->fighting != victim )
        return;
    if ( count > 1 )
        dam_message ( victim, ch, total, -1, FALSE, VERBOSE_SHIELD_COMP,
                      FALSE );
    else if ( count == 1 )
        dam_message ( victim, ch, total, -1, FALSE, VERBOSE_SHIELD_COMP,
                      TRUE );

    return;
}

/*
 * Mock hit one guy once.
 */
void one_hit_mock ( CHAR_DATA * ch, CHAR_DATA * victim, int dt,
                    bool secondary )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int sn, skill;
    int dam_type;
    bool result;

    sn = -1;

    /* just in case */
    if ( ch == NULL || victim == NULL )
        return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
        return;

    /*
     * Figure out the type of damage message.
     * if secondary == true, use the second weapon.
     */
    if ( !secondary )
        wield = get_eq_char ( ch, WEAR_WIELD );
    else
        wield = get_eq_char ( ch, WEAR_SECONDARY );
    if ( dt == TYPE_UNDEFINED )
    {
        dt = TYPE_HIT;
        if ( wield != NULL && wield->item_type == ITEM_WEAPON )
            dt += wield->value[3];
        else
            dt += ch->dam_type;
    }

    if ( dt < TYPE_HIT )
        if ( wield != NULL )
            dam_type = attack_table[wield->value[3]].damage;
        else
            dam_type = attack_table[ch->dam_type].damage;
    else
        dam_type = attack_table[dt - TYPE_HIT].damage;

    if ( dam_type == -1 )
        dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn ( ch );
    skill = 20 + get_weapon_skill ( ch, sn );

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC ( ch ) )
    {
        thac0_00 = 20;
        thac0_32 = -4;          /* as good as a thief */
        if ( IS_SET ( ch->act, ACT_VAMPIRE ) )
            thac0_32 = -30;
        else if ( IS_SET ( ch->act, ACT_DRUID ) )
            thac0_32 = 0;
        else if ( IS_SET ( ch->act, ACT_RANGER ) )
            thac0_32 = -4;
        else if ( IS_SET ( ch->act, ACT_WARRIOR ) )
            thac0_32 = -10;
        else if ( IS_SET ( ch->act, ACT_THIEF ) )
            thac0_32 = -4;
        else if ( IS_SET ( ch->act, ACT_CLERIC ) )
            thac0_32 = 2;
        else if ( IS_SET ( ch->act, ACT_MAGE ) )
            thac0_32 = 6;
    }
    else
    {
        thac0_00 = class_table[ch->class].thac0_00;
        thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0 = interpolate ( ch->level, thac0_00, thac0_32 );

    if ( thac0 < 0 )
        thac0 = thac0 / 2;

    if ( thac0 < -5 )
        thac0 = -5 + ( thac0 + 5 ) / 2;

    thac0 -= GET_HITROLL ( ch ) * skill / 100;
    thac0 += 5 * ( 100 - skill ) / 100;

    if ( dt == gsn_backstab )
        thac0 -= 10 * ( 100 - get_skill ( ch, gsn_backstab ) );

    if ( dt == gsn_strike )
        thac0 -= 10 * ( 100 - get_skill ( ch, gsn_strike ) );

    if ( dt == gsn_circle )
        thac0 -= 10 * ( 100 - get_skill ( ch, gsn_circle ) );
    if ( dt == gsn_whirlwind )
        thac0 -= 10 * ( 100 - get_skill ( ch, gsn_whirlwind ) );
    if ( dt == gsn_assassinate )
        thac0 -= 10 * ( 100 - get_skill ( ch, gsn_assassinate ) );

    switch ( dam_type )
    {
        case ( DAM_PIERCE ):
            victim_ac = GET_AC ( victim, AC_PIERCE ) / 10;
            break;
        case ( DAM_BASH ):
            victim_ac = GET_AC ( victim, AC_BASH ) / 10;
            break;
        case ( DAM_SLASH ):
            victim_ac = GET_AC ( victim, AC_SLASH ) / 10;
            break;
        default:
            victim_ac = GET_AC ( victim, AC_EXOTIC ) / 10;
            break;
    };

    if ( victim_ac < -15 )
        victim_ac = ( victim_ac + 15 ) / 5 - 15;

    if ( !can_see ( ch, victim ) )
        victim_ac -= 4;

    if ( victim->position < POS_FIGHTING )
        victim_ac += 4;

    if ( victim->position < POS_RESTING )
        victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits ( 5 ) ) >= 20 )
        ;

    if ( diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
        /* Miss. */
        damage_mock ( ch, victim, 0, dt, dam_type, TRUE, 1 );
        tail_chain (  );
        return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC ( ch ) && ( !ch->pIndexData->new_format || wield == NULL ) )
        if ( !ch->pIndexData->new_format )
        {
            dam = number_range ( ch->level / 2, ch->level * 3 / 2 );
            if ( wield != NULL )
                dam += dam / 2;
        }
        else
            dam = dice ( ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE] );

    else
    {
        if ( sn != -1 )
            check_improve ( ch, sn, TRUE, 5 );
        if ( wield != NULL )
        {
            if ( wield->clan )
            {
                dam = dice ( ch->level / 3, 3 ) * skill / 100;
            }
            else
            {
                if ( wield->pIndexData->new_format )
                    dam =
                        dice ( wield->value[1],
                               wield->value[2] ) * skill / 100;
                else
                    dam =
                        number_range ( wield->value[1] * skill / 100,
                                       wield->value[2] * skill / 100 );
            }

            if ( get_eq_char ( ch, WEAR_SHIELD ) == NULL )  /* no shield = more */
                dam = dam * 11 / 10;

            /* sharpness! */
            if ( IS_WEAPON_STAT ( wield, WEAPON_SHARP ) )
            {
                int percent;

                if ( ( percent = number_percent (  ) ) <= ( skill / 8 ) )
                    dam = 2 * dam + ( dam * 2 * percent / 100 );
            }
        }

        else
            dam =
                number_range ( 1 + 4 * skill / 100,
                               2 * ch->level / 3 * skill / 100 );
    }

    /*
     * Bonuses.
     */
    if ( get_skill ( ch, gsn_enhanced_damage ) > 0 )
    {
        diceroll = number_percent (  );
        if ( diceroll <= get_skill ( ch, gsn_enhanced_damage ) )
        {
            check_improve ( ch, gsn_enhanced_damage, TRUE, 6 );
            dam += 2 * ( dam * diceroll / 300 );
        }
    }

    if ( check_critical ( ch, victim ) )
        dam *= 2.40;
	/*
	if ( check_critical(ch,victim) )
	{
		dam = (int) (dam * 2.20);
		dam_type = DAM_PIERCE;
		result = damage( ch, victim, dam, gsn_critical, dam_type, TRUE );
	}
	*/

    if ( !IS_AWAKE ( victim ) )
        dam *= 2;
    else if ( victim->position < POS_FIGHTING )
        dam = dam * 3 / 2;

    if ( dt == gsn_backstab && wield != NULL )
    {
        if ( wield->value[0] != 2 )
            dam *= 2 + ( ch->level / 10 );
        else
            dam *= 2 + ( ch->level / 8 );
    }
    if ( dt == gsn_circle && wield != NULL )
    {
        if ( wield->value[0] != 2 )
            dam *= 2 + ( ch->level / 12 );
        else
            dam *= 2 + ( ch->level / 10 );
    }
    if ( dt == gsn_whirlwind && wield != NULL )
    {
        if ( wield->value[0] != 2 )
            dam *= 2 + ( ch->level / 12 );
        else
            dam *= 2 + ( ch->level / 7 );
    }

    if ( dt == gsn_assassinate && wield != NULL )
    {
/*        if ( IS_SHIELDED ( ch, SHD_SWALK ) )
        {
            send_to_char
                ( "The essence of surprise strengthens your attack!\n\r",
                  ch );
            dam *= 2;
        } */
        if ( chance ( 20 ) )
        {
            act ( "Someone nearly ends $n's miserable life!", victim, NULL,
                  NULL, TO_ROOM );
            act ( "$n barely misses piercing your vital organs!", ch, NULL,
                  victim, TO_VICT );
            printf_to_char ( ch,
                             "{xYour assassination attempt barely misses its mark{x!!\n\r" );
            dam *= 2 + ( ch->level / 9 );
        }
        else
            dam *= 2 + ( ch->level / 8 );
    }
/*
    if ( IS_SHIELDED ( ch, SHD_SWALK ) )
    {
        affect_strip ( ch, gsn_swalk );
    }
*/
    if ( dt == gsn_strike && wield != NULL )
        dam *= ( ch->level / 33 );  /* max * 3 */

    dam += GET_DAMROLL ( ch ) * UMIN ( 100, skill ) / 100;

    if ( dam <= 0 )
        dam = 1;

    result = damage_mock ( ch, victim, dam, dt, dam_type, TRUE, 1 );

    tail_chain (  );
    return;
}

bool damage ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
              int dam_type, bool show, bool stance )
{
    return xdamage ( ch, victim, dam, dt, dam_type, show, VERBOSE_STD, stance );
}

bool damage_old ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
                  int dam_type, bool show, bool stance )
{
    return xdamage ( ch, victim, dam, dt, dam_type, show, VERBOSE_STD, stance );
}

/*
 * By Gregor Stipicic for support of verbose 
 * Inflict damage from a hit.
 */
int xdamage ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
              int dam_type, bool show, int verbose, bool stance )
{

    OBJ_DATA *corpse;
    bool immune;

    if ( victim->position == POS_DEAD )
        return 0;

    if ( victim != ch )
    {
        /*
         * Certain attacks are forbidden.
         * Most other attacks are returned.
         */
        if ( is_safe ( ch, victim ) )
            return 0;

        if ( victim->position > POS_STUNNED )
        {
            if ( victim->fighting == NULL )
            {
                set_fighting ( victim, ch );
                if ( IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_KILL ) )
                    mp_percent_trigger ( victim, ch, NULL, NULL, TRIG_KILL );
            }
            if ( victim->timer <= 4 )
                victim->position = POS_FIGHTING;
        }

        if ( victim->position > POS_STUNNED )
        {
            if ( ch->fighting == NULL )
                set_fighting ( ch, victim );

        }

        /*
         * More charm stuff.
         */
        if ( victim->master == ch )
            stop_follower ( victim );
    }
    /*
     * Inviso attacks ... not.
     */
    if ( IS_SHIELDED ( ch, SHD_INVISIBLE ) )
    {
        affect_strip ( ch, gsn_invis );
        affect_strip ( ch, gsn_mass_invis );
	affect_strip ( ch, gsn_ambush );
        REMOVE_BIT ( ch->shielded_by, SHD_INVISIBLE );
        act ( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    /*
     * Damage modifiers.
     */

    if ( double_damage && IS_NPC ( victim ) && !IS_NPC ( ch ) )
        dam = dam * 2;

    if ( dam > 1 && !IS_NPC ( victim ) &&
         victim->pcdata->condition[COND_DRUNK] > 10 )
        dam = 9 * dam / 10;

    if ( dam > 1 && IS_SHIELDED ( victim, SHD_SANCTUARY ) )
        dam /= 1.2;

    if ( dam > 1 &&
         ( ( IS_SHIELDED ( victim, SHD_PROTECT_EVIL ) && IS_EVIL ( ch ) ) ||
           ( IS_SHIELDED ( victim, SHD_PROTECT_GOOD ) && IS_GOOD ( ch ) ) ) )
        dam -= dam / 4;

    // Knights are just too powerful - Rhaelar 01'2005
/*    if ( !IS_NPC ( ch ) && if ( ch->class == class_lookup ( "warrior" ) )
    || if ( ch->class == class_lookup ( "berserker" ) )
    || if ( ch->class == class_lookup ( "gladiator" ) )
    || if ( ch->class == class_lookup ( "knight" ) ) )
       dam = dam * 0.7; */

/*    if ( (dam > 1) && (dt>0) && (dt < TYPE_HIT) &&
       (skill_table[dt].spell_fun!=spell_null))
    {
     if(is_affected(victim,gsn_resistance)
      && (number_percent() < (50-(ch->level/5)+(victim->level/10))))
       dam= dam/5;
    }*/

    immune = FALSE;

    /*
     * Check for parry, and dodge.
     */
     if ( dt >= TYPE_HIT && ch != victim )
     {
       if (!can_bypass(ch,victim))  {
        if ( check_parry ( ch, victim ) )
            return 0;
        if ( check_dodge ( ch, victim ) )
            return 0;
        if ( check_shield_block ( ch, victim ) )
            return 0;
	}
	    if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE)
						&& victim->stance[STANCE_CRANE] >= 100
						&& check_parry( ch, victim ))
				return FALSE;
	    else if ( !IS_NPC(victim)
						&& IS_STANCE(victim, STANCE_MANTIS)
						&& victim->stance[STANCE_MANTIS] >= 100
						&& check_parry( ch, victim ))
				return FALSE;
	    if ( !IS_NPC(victim)
						&& IS_STANCE(victim, STANCE_MONGOOSE)
						&& victim->stance[STANCE_MONGOOSE] >= 100
						&& check_dodge( ch, victim))
				return FALSE;
	    else if ( !IS_NPC(victim)
						&& IS_STANCE(victim, STANCE_SWALLOW)
						&& victim->stance[STANCE_SWALLOW] >= 100
						&& check_dodge( ch, victim ))
				return FALSE;
    }
        
    if ( ( dt >= TYPE_HIT ) && show && dam > 0 )
    {
        if ( check_critical ( ch, victim ) )
        {
            dam +=
                dam *
                ( ( number_percent (  ) / 2 +
                    get_skill ( ch, gsn_critical ) ) / 2 ) / 50;
        }
    }
    if ( ( dt == gsn_circle || dt == gsn_backstab ) && show && dam > 0 )
    {
        if ( check_critical ( ch, victim ) )
        {
            /*dam += dam * number_range(0,get_skill(ch,gsn_critical)/2)/100; */
            dam +=
                dam *
                ( ( number_percent (  ) / 2 +
                    get_skill ( ch, gsn_critical ) ) / 2 ) / 100;
        }
    }
    
    switch ( check_immune ( victim, dam_type ) )
    {
        case ( IS_IMMUNE ):
            immune = TRUE;
            dam = 0;
            break;
        case ( IS_RESISTANT ):
            dam -= dam / 3;
            break;
        case ( IS_VULNERABLE ):
            dam += dam / 2;
            break;
    }
    /* damage reduction */
    if ( dam > 35 )
        dam = ( dam - 35 ) / 2 + 35;
    if ( dam > 80 )
        dam = ( dam - 80 ) / 2 + 80;
    if ( dam > 110)
	dam = ( dam - 110 ) / 2 + 110;

	if ( stance )	
	{
           dam = dambonus( ch, victim, dam, TRUE );
	   dam = damreduce( ch, victim, dam, TRUE );	
	}
		
    if ( IS_SHIELDED ( victim, SHD_WARD ) && show &&
         ( dt >= TYPE_HIT || dt >= gsn_backstab ) && ( dam > 0 ) && ( chance ( 10 ) ) )
    {
        char buf[MAX_STRING_LENGTH];
        sprintf ( buf,
                  "{GYour ward absorbs the damage and vanishes in a bright flash! {D({WAbsorb{D:{W%d{D){x\n\r",
                  dam );
        affect_strip ( victim, skill_lookup ( "ward" ) );
        send_to_char ( buf, victim );
        sprintf ( buf,
                  "{G$n{G's ward absorbs the damage and vanishes in a bright flash! {D({WAbsorb{D:{W%d{D){x",
                  dam );
        act ( buf, victim, NULL, NULL, TO_ROOM );
        return 0;
    }

    if ( IS_SHIELDED ( victim, SHD_MANA ) && show && ( dt >= TYPE_HIT ) &&
         ( dam > 0 ) )
    {
        if ( chance ( 30 ) )
        {
            char buf[MAX_STRING_LENGTH];
            int absorb = ( dam / 10 ) * number_range ( 1, 4 );
            if ( ( victim->mana - absorb >= 0 ) && ( absorb > 0 ) )
            {
                sprintf ( buf,
                          "Your mana shield absorbs some of the damage! {D({WAbsorb{D:{W%d{D){x\n\r",
                          absorb );
                send_to_char ( buf, victim );
                sprintf ( buf,
                          "$n's mana shield absorbs some of the damage! {D({WAbsorb{D:{W%d{D){x",
                          absorb );
                act ( buf, victim, NULL, NULL, TO_ROOM );
                dam -= absorb;
                victim->mana -= absorb;
            }
        }
    }

    if ( show )
        dam_message ( ch, victim, dam, dt, immune, verbose, TRUE );

    if ( dam == 0 )
        return 0;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC ( victim ) && victim->level >= LEVEL_IMMORTAL &&
         victim->hit < 1 
	&& !(IS_SET(victim->in_room->room_flags,ROOM_ARENA) ) )
        victim->hit = 1;
    update_pos ( victim );
    if ( dt == gsn_feed )
    {
        ch->hit = UMIN ( ch->hit + ( ( dam / 3 ) * 2 ), ch->max_hit );
        update_pos ( ch );
    }
    if ( dt == gsn_gorge )
    {
        ch->hit = UMIN ( ch->hit + ( dam / 3 ), ch->max_hit );
        update_pos ( ch );
    }
    
    switch ( victim->position )
    {
        case POS_MORTAL:
            act ( "{c$n is mortally wounded, and will die soon, if not aided.{x", victim, NULL, NULL, TO_ROOM );
            send_to_char
                ( "{cYou are mortally wounded, and will die soon, if not aided.{x\n\r",
                  victim );
            break;

        case POS_INCAP:
            act ( "{c$n is incapacitated and will slowly die, if not aided.{x", victim, NULL, NULL, TO_ROOM );
            send_to_char
                ( "{cYou are incapacitated and will slowly {z{Rdie{x{c, if not aided.{x\n\r",
                  victim );
            break;

        case POS_STUNNED:
            act ( "{c$n is stunned, but will probably recover.{x", victim,
                  NULL, NULL, TO_ROOM );
            send_to_char
                ( "{cYou are stunned, but will probably recover.{x\n\r",
                  victim );
            break;

        case POS_DEAD:
            if ( ( IS_NPC ( victim ) ) && ( victim->die_descr[0] != '\0' ) )
            {
                act ( "{c$n $T{x", victim, 0, victim->die_descr, TO_ROOM );
            }
            else
            {
                act ( "{c$n is {CDEAD!!{x", victim, 0, 0, TO_ROOM );
            }
            send_to_char ( "{cYou have been {RKILLED!!{x\n\r\n\r", victim );
            break;

        default:
            if ( dam > victim->max_hit / 4 )
                send_to_char ( "{cThat really did {RHURT!{x\n\r", victim );
            if ( victim->hit < victim->max_hit / 4 )
                send_to_char ( "{cYou sure are {z{RBLEEDING!{x\n\r", victim );
            break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE ( victim ) )
        stop_fighting ( victim, FALSE );

    if ( victim->position == POS_DEAD )
    {
        if ( !IS_NPC ( victim ) && !IS_NPC ( ch ) &&
             IS_SET ( victim->in_room->room_flags, ROOM_ARENA ) &&
             IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) )
        {
            check_arena ( ch, victim );
            return TRUE;
        }
    }
    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {

        group_gain ( ch, victim );

        if ( !IS_NPC ( victim ) )
        {
            sprintf ( log_buf, "%s killed by %s at %ld", victim->name,
                      ( IS_NPC ( ch ) ? ch->short_descr : ch->name ),
                      ch->in_room->vnum );
            log_string ( log_buf );

            /*
             * Dying penalty:
             * 2/3 way back to previous level.
             */
            if ( victim->exp >
                 exp_per_level ( victim,
                                 victim->pcdata->points ) * victim->level )
                gain_exp ( victim,
                           ( 5 *
                             ( exp_per_level
                               ( victim,
                                 victim->pcdata->points ) * victim->level -
                               victim->exp ) / 6 ) + 50 );

        }
        sprintf ( log_buf, "%s got toasted by %s at %s [room %ld]",
                  ( IS_NPC ( victim ) ? victim->short_descr : victim->name ),
                  ( IS_NPC ( ch ) ? ch->short_descr : ch->name ),
                  ch->in_room->name, ch->in_room->vnum );

        if ( IS_NPC ( victim ) )
            wiznet ( log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0 );
        else
            wiznet ( log_buf, NULL, NULL, WIZ_DEATHS, 0, 0 );

        /*
         * Death trigger
         */
        if ( IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_DEATH ) )
        {
            victim->position = POS_STANDING;
            mp_percent_trigger ( victim, ch, NULL, NULL, TRIG_DEATH );
        }

        /* Make the head for a bountied PC */
        if(!IS_NPC(victim) && !IS_NPC(ch) && victim != ch && is_bountied(victim) && is_real_pkiller ( ch ) )
        {
            char buf[MAX_STRING_LENGTH];
            char *name;
            OBJ_DATA *obj;

            name = str_dup(victim->name);
            obj = create_object(get_obj_index(OBJ_VNUM_SEVERED_BOUNTY_HEAD),0);
	    bug ("No it's not.. :(",0);
            obj->extra_flags |= ITEM_NODROP|ITEM_NOUNCURSE;

            sprintf(buf, "%s %s", "bounty head", name);
            free_string( obj->name );
            obj->name = str_dup( buf);

            sprintf( buf, obj->short_descr, name );
            free_string( obj->short_descr );
            obj->short_descr = str_dup( buf );

            sprintf( buf, obj->description, name );
            free_string( obj->description );
            obj->description = str_dup( buf );

            obj_to_char(obj,ch);
	    send_to_char ( "{R{zTHE SEVERED HEAD APPEARS IN YOUR INVENTORY AS A BOUNTY TROPHY!{x\n\r", ch );
            free_string(name);
        }

        raw_kill ( victim, ch );

        /* dump the flags */
        if ( ch != victim && !IS_NPC ( ch ) &&
             ( !is_same_clan ( ch, victim ) ||
               clan_table[victim->clan].independent ) )
        {
	  if ( !IS_SET ( victim->act2, PLR2_PERM_TWIT ) )
	  {
            if ( IS_SET ( victim->act, PLR_TWIT ) )
            REMOVE_BIT ( victim->act, PLR_TWIT );
	  }
        }

        /* RT new auto commands */

        if ( !IS_NPC ( ch ) && IS_NPC ( victim ) )
        {
            OBJ_DATA *coins;

            corpse = get_obj_list ( ch, "corpse", ch->in_room->contents );

            if ( IS_SET ( ch->act, PLR_AUTOLOOT ) && corpse && corpse->contains )   /* exists and not empty */
                do_get ( ch, "all corpse" );

            if ( IS_SET ( ch->act, PLR_AUTOGOLD ) && corpse && corpse->contains &&  /* exists and not empty */
                 !IS_SET ( ch->act, PLR_AUTOLOOT ) )
                if ( ( coins =
                       get_obj_list ( ch, "gcash",
                                      corpse->contains ) ) != NULL )
                    do_get ( ch, "all.gcash corpse" );

            if ( IS_SET ( ch->act, PLR_AUTOSAC ) )
            {
                if ( IS_SET ( ch->act, PLR_AUTOLOOT ) && corpse &&
                     corpse->contains )
                    return dam; /* leave if corpse has treasure */
                else
                    do_sacrifice ( ch, "corpse" );
            }
        }
        return dam;
    }

    if ( victim == ch )
        return dam;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC ( victim ) && victim->desc == NULL )
    {
        if ( number_range ( 0, victim->wait ) == 0 )
        {
            do_recall ( victim, "" );
            return dam;
        }
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC ( victim ) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2 )
    {
        if ( ( IS_SET ( victim->act, ACT_WIMPY ) && number_bits ( 2 ) == 0 &&
               victim->hit < victim->max_hit / 5 ) ||
             ( IS_AFFECTED ( victim, AFF_CHARM ) && victim->master != NULL &&
               victim->master->in_room != victim->in_room ) )
            do_flee ( victim, "" );
    }

    if ( !IS_NPC ( victim ) && victim->hit > 0 && victim->hit <= victim->wimpy
         && victim->wait < PULSE_VIOLENCE / 2 )
        do_flee ( victim, "" );

    tail_chain (  );
    return dam;
}

/*
 * Show damage from a mock hit.
 */
bool damage_mock ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
                   int dam_type, bool show, bool stance )
{
    long immdam;
    bool immune;
    char buf1[256], buf2[256], buf3[256];
    const char *attack;

    if ( victim->position == POS_DEAD )
        return FALSE;

    if ( dam > 35 )
        dam = ( dam - 35 ) / 2 + 35;
    if ( dam > 80 )
        dam = ( dam - 80 ) / 2 + 80;
    if ( dam > 110)
	dam = ( dam - 110 ) / 2 + 110;

    if ( is_safe_mock ( ch, victim ) )
        return FALSE;
    /*
     * Damage modifiers.
     */

		{

    	dam = dambonus( ch, victim, dam, FALSE );

		dam = damreduce( ch, victim, dam, FALSE );

		}
     
    if ( dam > 1 && !IS_NPC ( victim ) &&
         victim->pcdata->condition[COND_DRUNK] > 10 )
        dam = 9 * dam / 10;

    if ( dam > 1 && IS_SHIELDED ( victim, SHD_SANCTUARY ) )
        dam /= 2;

    if ( dam > 1 &&
         ( ( IS_SHIELDED ( victim, SHD_PROTECT_EVIL ) && IS_EVIL ( ch ) ) ||
           ( IS_SHIELDED ( victim, SHD_PROTECT_GOOD ) && IS_GOOD ( ch ) ) ) )
        dam -= dam / 4;

    immune = FALSE;

    switch ( check_immune ( victim, dam_type ) )
    {
        case ( IS_IMMUNE ):
            immune = TRUE;
            dam = 0;
            break;
        case ( IS_RESISTANT ):
            dam -= dam / 3;
            break;
        case ( IS_VULNERABLE ):
            dam += dam / 2;
            break;
    }

    if ( dt >= 0 && dt < MAX_SKILL )
        attack = skill_table[dt].noun_damage;
    else if ( dt >= TYPE_HIT && dt <= TYPE_HIT + MAX_DAMAGE_MESSAGE )
        attack = attack_table[dt - TYPE_HIT].noun;
    else
    {
        bug ( "Dam_message: bad dt %d.", dt );
        dt = TYPE_HIT;
        attack = attack_table[0].name;
    }
    immdam = 0;
    if ( ch->level == MAX_LEVEL )
    {
        immdam = dam * 63;
    }
    if ( ch == victim )
    {
        sprintf ( buf1,
                  "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {y$mself{g.{x",
                  attack, dam );
        sprintf ( buf2,
                  "{yYour {gmock {B%s{g would have done {R%d hp{g damage to {yyourself{g.{x",
                  attack, dam );
        act ( buf1, ch, NULL, NULL, TO_ROOM );
        act ( buf2, ch, NULL, NULL, TO_CHAR );
    }
    else if ( ch->level < MAX_LEVEL )
    {
        sprintf ( buf1,
                  "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {y$N{g.{x",
                  attack, dam );
        sprintf ( buf2,
                  "{yYour {gmock {B%s{g would have done {R%d hp{g damage to {y$N{g.{x",
                  attack, dam );
        sprintf ( buf3,
                  "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {yyou{g.{x",
                  attack, dam );
        act ( buf1, ch, NULL, victim, TO_NOTVICT );
        act ( buf2, ch, NULL, victim, TO_CHAR );
        act ( buf3, ch, NULL, victim, TO_VICT );
    }
    else
    {
        sprintf ( buf1,
                  "{y$n's {gmock {B%s{g would have done {R%lu hp{g damage to {y$N{g.{x",
                  attack, immdam );
        sprintf ( buf2,
                  "{yYour {gmock {B%s{g would have done {R%lu hp{g damage to {y$N{g.{x",
                  attack, immdam );
        sprintf ( buf3,
                  "{y$n's {gmock {B%s{g would have done {R%lu hp{g damage to {yyou{g.{x",
                  attack, immdam );
        act ( buf1, ch, NULL, victim, TO_NOTVICT );
        act ( buf2, ch, NULL, victim, TO_CHAR );
        act ( buf3, ch, NULL, victim, TO_VICT );
    }

    tail_chain (  );
    return TRUE;
}

bool is_safe ( CHAR_DATA * ch, CHAR_DATA * victim )
{

    if ( victim->in_room == NULL || ch->in_room == NULL )
        return TRUE;

    if ( victim->fighting == ch || victim == ch )
        return FALSE;

    if ( !IS_NPC ( ch ) && IS_IMMORTAL ( ch ) )
        return FALSE;

        if ( IS_SET ( victim->act2, PLR2_PKSAFE ) )
        {
            send_to_char ( "They are safe right now.\n\r", ch );
            return TRUE;
        }

	if ( reboot_countdown == 1 )
	{
	    send_to_char ( "Do the fighting after the reboot.\n\r", ch );
	    return TRUE;
	}
    /* ARENA room checking */
    if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) &&
         IS_SET ( victim->in_room->room_flags, ROOM_ARENA ) )
        return FALSE;

    /* killing mobiles */
    if ( IS_NPC ( victim ) )
    {

        /* safe room? */
        if ( IS_SET ( victim->in_room->room_flags, ROOM_SAFE ) )
        {
            send_to_char ( "Not in this room.\n\r", ch );
            return TRUE;
        }

        if ( victim->pIndexData->pShop != NULL )
        {
            send_to_char ( "The shopkeeper wouldn't like that.\n\r", ch );
            return TRUE;
        }

        /* no killing healers, trainers, etc */
        if ( IS_SET ( victim->act, ACT_TRAIN ) ||
             IS_SET ( victim->act, ACT_PRACTICE ) ||
             IS_SET ( victim->act, ACT_IS_HEALER ) ||
             IS_SET ( victim->act, ACT_IS_CHANGER ) ||
             IS_SET ( victim->act, ACT_IS_SATAN ) ||
             IS_SET ( victim->act, ACT_IS_PRIEST ) )
        {
            act ( "I don't think $G would approve.", ch, NULL, NULL,
                  TO_CHAR );
            return TRUE;
        }

        if ( !IS_NPC ( ch ) )
        {
            /* no pets */
            if ( IS_SET ( victim->act, ACT_PET ) )
            {
                act ( "But $N looks so cute and cuddly...", ch, NULL, victim,
                      TO_CHAR );
                return TRUE;
            }

            /* no charmed creatures unless owner */
            if ( IS_AFFECTED ( victim, AFF_CHARM ) && ch != victim->master )
            {
                send_to_char ( "You don't own that monster.\n\r", ch );
                return TRUE;
            }
            if ( !IS_QUESTOR ( ch ) && victim->isquestmob > 0 )
            {
                send_to_char ( "That is someone's questmob. Leave it alone.\n\r", ch );
                return TRUE;
            }        
	}
    }
    /* killing players */
    else
    {
        /* NPC doing the killing */
        if ( IS_NPC ( ch ) )
        {
            /* safe room check */
            if ( IS_SET ( victim->in_room->room_flags, ROOM_SAFE ) )
            {
                send_to_char ( "Not in this room.\n\r", ch );
                return TRUE;
            }

            /* charmed mobs and pets cannot attack players while owned */
            if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master != NULL &&
                 ch->master->fighting != victim )
            {
                send_to_char ( "Players are your friends!\n\r", ch );
                return TRUE;
            }
        }
        /* player doing the killing */
        else
        {
            if ( IS_SET ( victim->act, PLR_TWIT ) )
                return FALSE;

            if ( ( ( victim->level > 19 ) ||
                   ( ( victim->class >= MAX_CLASS / 2 ) &&
                     ( victim->level > 14 ) ) ) &&
                 ( is_voodood ( ch, victim ) ) )
                return FALSE;

            if ( IS_SET ( victim->in_room->room_flags, ROOM_SAFE ) )
            {
                send_to_char ( "Not in this room.\n\r", ch );
                return TRUE;
            }
            if ( ch->on_quest )
            {
                send_to_char ( "Not while you are on a quest.\n\r", ch );
                return TRUE;
            }
            if ( victim->on_quest )
            {
                send_to_char ( "They are on a quest, leave them alone.\n\r",
                               ch );
                return TRUE;
            }
            if ( !is_clan ( ch ) )
            {
                send_to_char
                    ( "Join a clan if you want to fight players.\n\r", ch );
                return TRUE;
            }

            if ( !is_pkill ( ch ) )
            {
                send_to_char
                    ( "Your clan does not allow player fighting.\n\r", ch );
                return TRUE;
            }

            if ( !is_clan ( victim ) )
            {
                send_to_char ( "They aren't in a clan, leave them alone.\n\r",
                               ch );
                return TRUE;
            }

            if ( !is_pkill ( victim ) )
            {
                send_to_char
                    ( "They are in a no pkill clan, leave them alone.\n\r",
                      ch );
                return TRUE;
            }

            if ( is_same_clan ( ch, victim ) )
            {
                send_to_char ( "You can't fight your own clan members.\n\r",
                               ch );
                return TRUE;
            }

            if ( IS_SET ( victim->act2, PLR2_PKSAFE ) )
            {
                send_to_char ( "They are safe from PK'ing. Leave them alone.\n\r",
                               ch );
                return TRUE;
            }
/*				if ((ch->level > victim->level + 15)
				|| (ch->level < victim->level - 15))
				{
					stc ("Pick on someone your own size.\n\r",ch);
					return TRUE;
				} */
				if ( !can_pk ( ch, victim ) )
				{
					stc ("Pick on someone your own size.\n\r",ch);
					return TRUE;
				}         
	}                       /*
                                   if (can_pk(ch,victim))
                                   return FALSE;      
                                 */
/*         if (!IS_NPC(victim))
         return TRUE;
*/ }
    return FALSE;
}

bool can_pk ( CHAR_DATA * ch, CHAR_DATA * victim )
{
/*    int tvalue;
    int lvalue;
    int pkvalue;

    if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) )
        tvalue = 1;

    else if ( IS_SET ( class_table[ch->class].tier, TIER_02 ) )
        tvalue = 2;

    else if ( IS_SET ( class_table[ch->class].tier, TIER_03 ) )
        tvalue = 3;

    else
        tvalue = 4;

    if ( ch->level >= LEVEL_ANCIENT )
        lvalue = ch->level / 11;
    else
        lvalue = ch->level / 10;
    pkvalue = tvalue + lvalue;

    if ( ( ch->pcdata->pkvalue + 10 ) > ( victim->pcdata->pkvalue - 10 ) )
    {
        send_to_char
            ( "They are far to weak to be considered a worthy target by the likes of you!\n\r",
              ch );
        return FALSE;
    }

    if ( ( ch->pcdata->pkvalue + 15 ) < ( victim->pcdata->pkvalue - 15 ) )
    {
        printf_to_char ( ch,
                         "They are far to strong to be considered an actual target by the likes of you!\n\r" );
        return FALSE;
    }

    else
    {
        return TRUE;
    } */
    
    if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) &&
         !IS_SET ( class_table[victim->class].tier, TIER_01 ) )
	 return FALSE;
    else if ( IS_SET ( class_table[ch->class].tier, TIER_02 ) &&
         !IS_SET ( class_table[victim->class].tier, TIER_02 ) )
	 return FALSE;
    else if ( IS_SET ( class_table[ch->class].tier, TIER_03 ) &&
         !IS_SET ( class_table[victim->class].tier, TIER_03 ) )
	 return FALSE;
    else if ( IS_SET ( class_table[ch->class].tier, TIER_04 ) &&
         !IS_SET ( class_table[victim->class].tier, TIER_04 ) )
	 return FALSE;
    else if ((ch->level > victim->level + 15)
            || (ch->level < victim->level - 15))
	return FALSE;
    else return TRUE;
}

bool is_safe_mock ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    if ( victim->in_room == NULL || ch->in_room == NULL )
        return TRUE;
    if ( !IS_NPC ( ch ) && IS_IMMORTAL ( ch ) )
        return FALSE;
    if ( IS_SET ( victim->in_room->room_flags, ROOM_SAFE ) )
    {
        send_to_char ( "Not in this room.\n\r", ch );
        return TRUE;
    }
    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "{RYou can only use this on a player.{x\n\r", ch );
        return TRUE;
    }
    return FALSE;
}

bool is_voodood ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    OBJ_DATA *object;
    bool found;

    if ( ch->level > HERO )
        return FALSE;

    found = FALSE;
    for ( object = victim->carrying; object != NULL;
          object = object->next_content )
    {
        if ( object->pIndexData->vnum == OBJ_VNUM_VOODOO )
        {
            char arg[MAX_INPUT_LENGTH];

            one_argument ( object->name, arg );
            if ( !str_cmp ( arg, ch->name ) )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool is_safe_spell ( CHAR_DATA * ch, CHAR_DATA * victim, bool area )
{
    if ( victim->in_room == NULL || ch->in_room == NULL )
        return TRUE;

    if ( victim == ch && area )
        return TRUE;

    if ( victim->fighting == ch || victim == ch )
        return FALSE;

    /* ARENA room checking */
    if ( IS_SET ( victim->act2, PLR2_PKSAFE ) )
    {
	send_to_char ( "They are safe!\n\r", ch );
        return FALSE;
    }

    /* ARENA room checking */
    if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) &&
         IS_SET ( victim->in_room->room_flags, ROOM_ARENA ) )
        return FALSE;

    if ( !IS_NPC ( ch ) && IS_IMMORTAL ( ch ) )
        return FALSE;

    /* killing mobiles */
    if ( IS_NPC ( victim ) )
    {
        /* safe room? */
        if ( IS_SET ( victim->in_room->room_flags, ROOM_SAFE ) )
            return TRUE;

        if ( victim->pIndexData->pShop != NULL )
            return TRUE;

        /* no killing healers, trainers, etc */
        if ( IS_SET ( victim->act, ACT_TRAIN ) ||
             IS_SET ( victim->act, ACT_PRACTICE ) ||
             IS_SET ( victim->act, ACT_IS_HEALER ) ||
             IS_SET ( victim->act, ACT_IS_CHANGER ) ||
             IS_SET ( victim->act, ACT_IS_SATAN ) ||
             IS_SET ( victim->act, ACT_IS_PRIEST ) )
            return TRUE;

        if ( !IS_NPC ( ch ) )
        {
            /* no pets */
            if ( IS_SET ( victim->act, ACT_PET ) )
                return TRUE;

            /* no charmed creatures unless owner */
            if ( IS_AFFECTED ( victim, AFF_CHARM ) &&
                 ( area || ch != victim->master ) )
                return TRUE;

            /* legal kill? -- cannot hit mob fighting non-group member */
            if ( victim->fighting != NULL &&
                 !is_same_group ( ch, victim->fighting ) )
                return TRUE;
        }
        else
        {
            /* area effect spells do not hit other mobs */
            if ( area && !is_same_group ( victim, ch->fighting ) )
                return TRUE;
        }
    }
    /* killing players */
    else
    {
        if ( area && IS_IMMORTAL ( victim ) &&
             victim->level > LEVEL_IMMORTAL )
            return TRUE;

        /* NPC doing the killing */
        if ( IS_NPC ( ch ) )
        {
            /* charmed mobs and pets cannot attack players while owned */
            if ( ( ( IS_AFFECTED ( ch, AFF_CHARM ) ) &
                   ( ch->master != NULL ) ) &&
                 ( ch->master->fighting != victim ) )
                return TRUE;

            /* safe room? */
            if ( IS_SET ( victim->in_room->room_flags, ROOM_SAFE ) )
                return TRUE;

            /* legal kill? -- mobs only hit players grouped with opponent */
            if ( ch->fighting != NULL &&
                 !is_same_group ( ch->fighting, victim ) )
                return TRUE;
        }

        /* player doing the killing */
        else
        {                       /*
                                   if (ON_GQUEST(ch) || ON_GQUEST(victim))
                                   return TRUE;
                                 */
            if ( IS_SET ( victim->act, PLR_TWIT ) )
                return FALSE;

            if ( ( ( victim->level > 19 ) ||
                   ( ( victim->class >= MAX_CLASS / 2 ) &&
                     ( victim->level > 14 ) ) ) &&
                 ( is_voodood ( ch, victim ) ) )
                return FALSE;

            if ( !is_clan ( ch ) )
                return TRUE;

            if ( !is_pkill ( ch ) )
                return TRUE;

            if ( IS_SET ( victim->in_room->room_flags, ROOM_SAFE ) )
                return TRUE;

            if ( ch->on_quest )
                return TRUE;

            if ( victim->on_quest )
                return TRUE;

            if ( !is_clan ( victim ) )
                return TRUE;

            if ( !is_pkill ( victim ) )
                return TRUE;

            if ( is_same_clan ( ch, victim ) )
                return TRUE;

            if ( ( ( ch->class < MAX_CLASS / 2 ) &&
                   ( victim->class < MAX_CLASS / 2 ) ) ||
                 ( ( ch->class >= MAX_CLASS / 2 ) &&
                   ( victim->class >= MAX_CLASS / 2 ) ) )
            {
                if ( ch->level > victim->level + 15 )
                {
                    return TRUE;
                }
                if ( ch->level < victim->level - 15 )
                {
                    return TRUE;
                }
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool check_critical_hit ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    int chance;

    if ( IS_NPC ( ch ) )
        return FALSE;

    chance = get_skill ( ch, gsn_critical ) / 6;
    if ( !chance )
        return FALSE;

    if ( !can_see ( ch, victim ) )
        chance /= 2;

    if ( number_percent (  ) >= chance + ch->level - victim->level )
        return FALSE;

    send_to_char
        ( "{BYou score a {R-{D={W* {RCRITICAL HIT {W*{D={R-{B!{x\n\r", ch );
    act ( "{c$n scores a {R-{D={W* {RCRITICAL HIT {W*{D={R-{c!{x", ch, NULL,
          victim, TO_VICT );
    act ( "{x$n scores a {R-{D={W* {RCRITICAL HIT {W*{D={R-{x!", ch, NULL,
          victim, TO_NOTVICT );
    check_improve ( ch, gsn_critical, TRUE, 6 );
    return TRUE;
}

/*
 * Check for parry.
 */
bool check_parry ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    int chance;

    if ( !IS_AWAKE ( victim ) )
        return FALSE;

    chance = get_skill ( victim, gsn_parry ) / 2;

    if ( get_eq_char ( victim, WEAR_WIELD ) == NULL )
    {
        if ( IS_NPC ( victim ) )
            chance /= 2;
        else
            return FALSE;
    }

    if ( victim->stunned )
        return FALSE;

    if (!can_see(ch,victim))
	chance /= 2;


    if (IS_STANCE(victim, STANCE_CRANE)
        && !can_counter(ch, TRUE)
        && !can_bypass(ch,victim))
		{
        chance += (int) (victim->stance[STANCE_CRANE] * 0.25);
		if (!(stance_checked))	improve_stance(victim);
		}
    else if (IS_STANCE(victim, STANCE_MANTIS)
        && !can_counter(ch, TRUE)
        && !can_bypass(ch,victim))
		{
        chance += (int) (victim->stance[STANCE_MANTIS] * 0.25);
		if (!(stance_checked))	improve_stance(victim);
		}
	else if ((victim->stance[0]>0) && (victim->stance[victim->stance[0]]<100))
		chance /= 2;

	chance *= (2*get_weapon_skill(victim,get_weapon_sn(victim)) +
				 get_weapon_skill(victim,get_weapon_sn(ch)) );

	chance /= 300;

    
    if ( number_percent (  ) >= chance + victim->level - ch->level )
        return FALSE;
/*
    act( "{iYou parry $n's attack.{x",  ch, NULL, victim, TO_VICT    );
    act( "{h$N parries your attack.{x", ch, NULL, victim, TO_CHAR    );
*/
    xact_new ( "{iYou parry $n's attack.{x", ch, NULL, victim, TO_VICT,
               POS_RESTING, VERBOSE_DODGE );
    xact_new ( "{h$N parries your attack.{x", ch, NULL, victim, TO_CHAR,
               POS_RESTING, VERBOSE_DODGE ); 
    /*act_new ( "{iYou parry $n's attack.{x", ch, NULL, victim, TO_VICT, POS_RESTING );
    act_new ( "{h$N parries your attack.{x", ch, NULL, victim, TO_CHAR, POS_RESTING ); */
    check_improve ( victim, gsn_parry, TRUE, 6 );
    return TRUE;
}

/*
 * Check for shield block.
 */
bool check_shield_block ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    bool levitate;
    int chance;

    if ( !IS_AWAKE ( victim ) )
        return FALSE;

    levitate = ( get_skill ( victim, gsn_shield_levitation ) != 0 ) &&
        ( skill_table[gsn_shield_levitation].skill_level[ch->class] <=
          ch->level ) && ( get_eq_char ( victim, WEAR_SHIELD ) != NULL ) &&
        ( ( get_eq_char ( victim, WEAR_SECONDARY ) != NULL ) ||
          ( ( get_eq_char ( victim, WEAR_WIELD ) != NULL ) &&
            ( get_eq_char ( victim, WEAR_SECONDARY ) == NULL ) &&
            IS_WEAPON_STAT ( get_eq_char ( victim, WEAR_WIELD ),
                             WEAPON_TWO_HANDS ) ) );

    chance = get_skill ( victim, gsn_shield_block ) / 3 + 3;
    if ( levitate )
        chance =
            ( chance +
              ( get_skill ( victim, gsn_shield_levitation ) / 3 + 3 ) ) / 2;

    if ( get_eq_char ( victim, WEAR_SHIELD ) == NULL )
        return FALSE;

    if ( number_percent (  ) >= chance + victim->level - ch->level )
        return FALSE;

    if ( victim->stunned )
        return FALSE;
/*
    act( "{iYou block $n's attack with your shield.{x",  ch, NULL, victim,TO_VICT);
    act( "{h$N blocks your attack with a shield.{x", ch, NULL, victim,TO_CHAR);
    check_improve(victim,gsn_shield_block,TRUE,6);
  */
    xact_new ( "{iYou block $n's attack with your shield.{x", ch, NULL,
               victim, TO_VICT, POS_RESTING, VERBOSE_DODGE );
    xact_new ( "{h$N blocks your attack with a shield.{x", ch, NULL, victim,
               TO_CHAR, POS_RESTING, VERBOSE_DODGE );
/*    act_new ( "{iYou block $n's attack with your shield.{x", ch, NULL, victim, TO_VICT, POS_RESTING );
    act_new ( "{h$N blocks your attack with a shield.{x", ch, NULL, victim, TO_CHAR, POS_RESTING ); */
    check_improve ( victim, gsn_shield_block, TRUE, 6 );
    if ( levitate )
        check_improve ( victim, gsn_shield_levitation, TRUE, 6 );
    return TRUE;
}

/*
 * Check for dodge.
 */
bool check_dodge ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    int chance;

    if ( !IS_AWAKE ( victim ) )
        return FALSE;

    chance = get_skill ( victim, gsn_dodge ) / 2;
    if (IS_STANCE(victim, STANCE_MONGOOSE)
        && !can_counter(ch, TRUE)
        && !can_bypass(ch,victim))
		{
        chance += (int) (victim->stance[STANCE_MONGOOSE] * 0.25);
		if (!(stance_checked))	improve_stance(victim);
		}
    else if (IS_STANCE(victim, STANCE_SWALLOW)
        && !can_counter(ch, TRUE)
        && !can_bypass(ch,victim))
		{
        chance += (int) (victim->stance[STANCE_SWALLOW] * 0.25);
		if (!(stance_checked))	improve_stance(victim);
		}
	else if ((victim->stance[0]>0) && (victim->stance[victim->stance[0]]<100))
		chance /= 2;
        
    if ( !can_see ( victim, ch ) )
        chance /= 2;

    if ( number_percent (  ) >= chance + victim->level - ch->level )
        return FALSE;

    if ( victim->stunned )
        return FALSE;
/*
    act( "{iYou dodge $n's attack.{x", ch, NULL, victim, TO_VICT    );
    act( "{h$N dodges your attack.{x", ch, NULL, victim, TO_CHAR    );
  */
    xact_new ( "{iYou dodge $n's attack.{x", ch, NULL, victim, TO_VICT,
               POS_RESTING, VERBOSE_DODGE );
    xact_new ( "{h$N dodges your attack.{x", ch, NULL, victim, TO_CHAR,
               POS_RESTING, VERBOSE_DODGE ); 
/*    act_new ( "{iYou dodge $n's attack.{x", ch, NULL, victim, TO_VICT, POS_RESTING );
    act_new ( "{h$N dodges your attack.{x", ch, NULL, victim, TO_CHAR, POS_RESTING ); */

    check_improve ( victim, gsn_dodge, TRUE, 6 );
    return TRUE;
}

/*
 * Set position of a victim.
 */
void update_pos ( CHAR_DATA * victim )
{
    if ( victim->hit > 0 )
    {
        if ( victim->position <= POS_STUNNED )
            victim->position = POS_STANDING;
        return;
    }

    if ( IS_NPC ( victim ) && victim->hit < 1 )
    {
        victim->position = POS_DEAD;
        return;
    }

    if ( victim->hit <= -11 )
    {
        victim->position = POS_DEAD;
        return;
    }

    if ( victim->hit <= -6 )
        victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 )
        victim->position = POS_INCAP;
    else
        victim->position = POS_STUNNED;

    return;
}

/*
 * Start fights.
 */
void set_fighting ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    if ( ch->fighting != NULL )
    {
//        bug ( "Set_fighting: already fighting", 0 );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_SLEEP ) )
        affect_strip ( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;
    ch->stunned = 0;

    return;
}

/*
 * Stop fights.
 */
void stop_fighting ( CHAR_DATA * ch, bool fBoth )
{
    CHAR_DATA *fch;
    char buf[MAX_STRING_LENGTH];

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
        if ( fch == ch || ( fBoth && fch->fighting == ch ) )
        {
            fch->fighting = NULL;
            fch->position = IS_NPC ( fch ) ? fch->default_pos : POS_STANDING;
            fch->stunned = 0;
            update_pos ( fch );
            if ( IS_SET ( fch->comm, COMM_STORE ) )
                if ( fch->tells )
                {
                    sprintf ( buf, "{cYou have {R%d{c tells waiting.{x\n\r",
                              fch->tells );
                    send_to_char ( buf, fch );
                    send_to_char ( "{cType '{Rreplay{c' to see tells.{x\n\r",
                                   fch );
                }
        }
    }

    return;
}

/*
 * Make a corpse out of a character.
 */
void make_corpse ( CHAR_DATA * ch, CHAR_DATA * killer )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;
    ROOM_INDEX_DATA *morgue;

    morgue = get_room_index ( ROOM_VNUM_MORGUE );

    if ( IS_NPC ( ch ) )
    {
        if ( IS_SET ( ch->act, ACT_NO_BODY ) )
        {
            if ( IS_SET ( ch->act, ACT_NB_DROP ) )
            {
                //for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		for ( obj = ch->carrying; obj; obj = obj->next_content )
                {
                    //obj_next = obj->next_content;
                    obj_from_char ( obj );
                    if ( obj->item_type == ITEM_POTION )
                        obj->timer = number_range ( 500, 1000 );
                    if ( obj->item_type == ITEM_SCROLL )
                        obj->timer = number_range ( 1000, 2500 );
                    if ( IS_SET ( obj->extra_flags, ITEM_ROT_DEATH ) )
                    {
                        obj->timer = number_range ( 5, 10 );
                        REMOVE_BIT ( obj->extra_flags, ITEM_ROT_DEATH );
                    }
                    REMOVE_BIT ( obj->extra_flags, ITEM_VIS_DEATH );

                    if ( IS_SET ( obj->extra_flags, ITEM_INVENTORY ) )
                        extract_obj ( obj );
                    act ( "$p falls to the floor.", ch, obj, NULL, TO_ROOM );
                    obj_to_room ( obj, ch->in_room );
                }
            }
            return;
        }
        name = ch->short_descr;
        corpse = create_object ( get_obj_index ( OBJ_VNUM_CORPSE_NPC ), 0 );
        corpse->timer = number_range ( 3, 6 );
        if ( ch->gold > 0 || ch->platinum > 0 || ch->silver > 0 )
        {
            obj_to_obj ( create_money ( ch->platinum, ch->gold, ch->silver ),
                         corpse );
            ch->platinum = 0;
            ch->gold = 0;
            ch->silver = 0;
        }
        corpse->cost = 0;
    }
    else
    {
        name = ch->name;
        corpse = create_object ( get_obj_index ( OBJ_VNUM_CORPSE_PC ), 0 );
        corpse->timer = number_range ( 25, 40 );
        REMOVE_BIT ( ch->act, PLR_CANLOOT );
        if ( !is_clan ( ch ) )
        {
            free_string ( corpse->owner );
            corpse->owner = str_dup ( ch->name );
            corpse->killer = NULL;
        }
        else
        {
            free_string ( corpse->owner );
            corpse->owner = str_dup ( ch->name );
            free_string ( corpse->killer );
            corpse->killer = str_dup ( killer->name );
            if ( ch->platinum > 1 || ch->gold > 1 || ch->silver > 1 )
            {
                obj_to_obj ( create_money
                             ( ch->platinum / 2, ch->gold / 2,
                               ch->silver / 2 ), corpse );
                ch->platinum -= ch->platinum / 2;
                ch->gold -= ch->gold / 2;
                ch->silver -= ch->silver / 2;
            }
        }

        corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf ( buf, corpse->short_descr, name );
    free_string ( corpse->short_descr );
    corpse->short_descr = str_dup ( buf );

    sprintf ( buf, corpse->description, name );
    free_string ( corpse->description );
    corpse->description = str_dup ( buf );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        bool floating = FALSE;

        obj_next = obj->next_content;
        if ( obj->wear_loc == WEAR_FLOAT )
            floating = TRUE;
        obj_from_char ( obj );
        if ( obj->item_type == ITEM_POTION )
            obj->timer = number_range ( 500, 1000 );
        if ( obj->item_type == ITEM_SCROLL )
            obj->timer = number_range ( 1000, 2500 );
        if ( IS_SET ( obj->extra_flags, ITEM_ROT_DEATH ) && !floating )
        {
            obj->timer = number_range ( 5, 10 );
            REMOVE_BIT ( obj->extra_flags, ITEM_ROT_DEATH );
        }
        REMOVE_BIT ( obj->extra_flags, ITEM_VIS_DEATH );

        if ( IS_SET ( obj->extra_flags, ITEM_INVENTORY ) )
            extract_obj ( obj );
        else if ( floating )
        {
            if ( IS_OBJ_STAT ( obj, ITEM_ROT_DEATH ) )  /* get rid of it! */
            {
                if ( obj->contains != NULL )
                {
                    OBJ_DATA *in, *in_next;

                    act ( "$p evaporates,scattering its contents.", ch, obj,
                          NULL, TO_ROOM );
                    for ( in = obj->contains; in != NULL; in = in_next )
                    {
                        in_next = in->next_content;
                        obj_from_obj ( in );
                        obj_to_room ( in, ch->in_room );
                    }
                }
                else
                    act ( "$p evaporates.", ch, obj, NULL, TO_ROOM );
                extract_obj ( obj );
            }

            else
            {
                act ( "$p falls to the floor.", ch, obj, NULL, TO_ROOM );
                obj_to_room ( obj, ch->in_room );
            }
        }
        else
            obj_to_obj ( obj, corpse );
    }

    obj_to_room ( corpse, ch->in_room );

    return;
}

/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry ( CHAR_DATA * ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    msg = "You hear $n's death cry.";
    if ( IS_NPC ( ch ) )
    {
        if ( !IS_SET ( ch->act, ACT_NO_BODY ) )
        {
            switch ( number_bits ( 4 ) )
            {
                case 0:
                    msg = "$n hits the ground ... DEAD.";
                    vnum = OBJ_VNUM_BLOOD;
                    break;
                case 1:
                    msg = "$n splatters blood on your armor.";
                    vnum = OBJ_VNUM_BLOOD;
                    break;
                case 2:
                    if ( IS_SET ( ch->parts, PART_GUTS ) )
                    {
                        msg = "$n spills $s guts all over the floor.";
                        vnum = OBJ_VNUM_GUTS;
                    }
                    break;
                case 3:
                    if ( IS_SET ( ch->parts, PART_HEAD ) )
                    {
                        msg = "$n's severed head plops on the ground.";
                        vnum = OBJ_VNUM_SEVERED_HEAD;
                    }
                    break;
                case 4:
                    if ( IS_SET ( ch->parts, PART_HEART ) )
                    {
                        msg = "$n's heart is torn from $s chest.";
                        vnum = OBJ_VNUM_TORN_HEART;
                    }
                    break;
                case 5:
                    if ( IS_SET ( ch->parts, PART_ARMS ) )
                    {
                        msg = "$n's arm is sliced from $s dead body.";
                        vnum = OBJ_VNUM_SLICED_ARM;
                    }
                    break;
                case 6:
                    if ( IS_SET ( ch->parts, PART_LEGS ) )
                    {
                        msg = "$n's leg is sliced from $s dead body.";
                        vnum = OBJ_VNUM_SLICED_LEG;
                    }
                    break;
                case 7:
                    if ( IS_SET ( ch->parts, PART_BRAINS ) )
                    {
                        msg =
                            "$n's head is shattered, and $s brains splash all over you.";
                        vnum = OBJ_VNUM_BRAINS;
                    }
                    break;
                case 8:
                    msg = "$n hits the ground ... DEAD.";
                    vnum = OBJ_VNUM_BLOOD;
                    break;
                case 9:
                    msg = "$n hits the ground ... DEAD.";
                    vnum = OBJ_VNUM_BLOOD;
            }
        }
    }
    else if ( ch->level > 19 )
    {
        switch ( number_bits ( 4 ) )
        {
            case 0:
                msg = "$n hits the ground ... DEAD.";
                vnum = OBJ_VNUM_BLOOD;
                break;
            case 1:
                msg = "$n splatters blood on your armor.";
                vnum = OBJ_VNUM_BLOOD;
                break;
            case 2:
                if ( IS_SET ( ch->parts, PART_GUTS ) )
                {
                    msg = "$n spills $s guts all over the floor.";
                    vnum = OBJ_VNUM_GUTS;
                }
                break;
            case 3:
                if ( IS_SET ( ch->parts, PART_HEAD ) )
                {
                    msg = "$n's severed head plops on the ground.";
                    vnum = OBJ_VNUM_SEVERED_HEAD;
                }
                break;
            case 4:
                if ( IS_SET ( ch->parts, PART_HEART ) )
                {
                    msg = "$n's heart is torn from $s chest.";
                    vnum = OBJ_VNUM_TORN_HEART;
                }
                break;
            case 5:
                if ( IS_SET ( ch->parts, PART_ARMS ) )
                {
                    msg = "$n's arm is sliced from $s dead body.";
                    vnum = OBJ_VNUM_SLICED_ARM;
                }
                break;
            case 6:
                if ( IS_SET ( ch->parts, PART_LEGS ) )
                {
                    msg = "$n's leg is sliced from $s dead body.";
                    vnum = OBJ_VNUM_SLICED_LEG;
                }
                break;
            case 7:
                if ( IS_SET ( ch->parts, PART_BRAINS ) )
                {
                    msg =
                        "$n's head is shattered, and $s brains splash all over you.";
                    vnum = OBJ_VNUM_BRAINS;
                }
                break;
            case 8:
                msg = "$n hits the ground ... DEAD.";
                vnum = OBJ_VNUM_BLOOD;
                break;
            case 9:
                msg = "$n hits the ground ... DEAD.";
                vnum = OBJ_VNUM_BLOOD;
                break;
            case 10:
                if ( IS_SET ( ch->parts, PART_HEAD ) )
                {
                    msg = "$n's severed head plops on the ground.";
                    vnum = OBJ_VNUM_SEVERED_HEAD;
                }
                break;
            case 11:
                if ( IS_SET ( ch->parts, PART_HEART ) )
                {
                    msg = "$n's heart is torn from $s chest.";
                    vnum = OBJ_VNUM_TORN_HEART;
                }
                break;
            case 12:
                if ( IS_SET ( ch->parts, PART_ARMS ) )
                {
                    msg = "$n's arm is sliced from $s dead body.";
                    vnum = OBJ_VNUM_SLICED_ARM;
                }
                break;
            case 13:
                if ( IS_SET ( ch->parts, PART_LEGS ) )
                {
                    msg = "$n's leg is sliced from $s dead body.";
                    vnum = OBJ_VNUM_SLICED_LEG;
                }
                break;
            case 14:
                if ( IS_SET ( ch->parts, PART_BRAINS ) )
                {
                    msg =
                        "$n's head is shattered, and $s brains splash all over you.";
                    vnum = OBJ_VNUM_BRAINS;
                }
        }
    }

    act ( msg, ch, NULL, NULL, TO_ROOM );

    if ( ( vnum == 0 ) && !IS_SET ( ch->act, ACT_NO_BODY ) )
    {
        switch ( number_bits ( 4 ) )
        {
            case 0:
                vnum = 0;
                break;
            case 1:
                vnum = OBJ_VNUM_BLOOD;
                break;
            case 2:
                vnum = 0;
                break;
            case 3:
                vnum = OBJ_VNUM_BLOOD;
                break;
            case 4:
                vnum = 0;
                break;
            case 5:
                vnum = OBJ_VNUM_BLOOD;
                break;
            case 6:
                vnum = 0;
                break;
            case 7:
                vnum = OBJ_VNUM_BLOOD;
        }
    }

    if ( vnum != 0 )
    {
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        char *name;

        name = IS_NPC ( ch ) ? ch->short_descr : ch->name;
        obj = create_object ( get_obj_index ( vnum ), 0 );
        obj->timer = number_range ( 4, 7 );
        if ( !IS_NPC ( ch ) )
        {
            obj->timer = number_range ( 12, 18 );
        }
        if ( vnum == OBJ_VNUM_BLOOD )
        {
            obj->timer = number_range ( 1, 4 );
        }

        sprintf ( buf, obj->short_descr, name );
        free_string ( obj->short_descr );
        obj->short_descr = str_dup ( buf );

        sprintf ( buf, obj->description, name );
        free_string ( obj->description );
        obj->description = str_dup ( buf );

        sprintf ( buf, obj->name, name );
        free_string ( obj->name );
        obj->name = str_dup ( buf );

        if ( obj->item_type == ITEM_FOOD )
        {
            if ( IS_SET ( ch->form, FORM_POISON ) )
                obj->value[3] = 1;
            else if ( !IS_SET ( ch->form, FORM_EDIBLE ) )
                obj->item_type = ITEM_TRASH;
        }

        if ( IS_NPC ( ch ) )
        {
            obj->value[4] = 0;
        }
        else
        {
            obj->value[4] = 1;
        }

        obj_to_room ( obj, ch->in_room );
    }

    if ( IS_NPC ( ch ) )
        msg = "You hear something's death cry.";
    else
        msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
        EXIT_DATA *pexit;

        if ( ( pexit = was_in_room->exit[door] ) != NULL &&
             pexit->u1.to_room != NULL && pexit->u1.to_room != was_in_room )
        {
            ch->in_room = pexit->u1.to_room;
            act ( msg, ch, NULL, NULL, TO_ROOM );
        }
    }
    ch->in_room = was_in_room;

    return;
}

void raw_kill ( CHAR_DATA * victim, CHAR_DATA * killer )
{
    int i;

//    OBJ_DATA *soul; Needs to be uncommented for soul code to work, scroll down to see what it is
    char buf[MSL];
    death_cry ( victim );
    stop_fighting ( victim, TRUE );

    if ( !IS_NPC ( victim ) && victim->clan == 5 )
    {
	if ( IS_NPC ( killer ) && !IS_NPC ( victim ) )
	make_corpse ( victim, killer );
    }

    else if ( !IS_NPC ( killer ) && killer->clan != 5 )
	make_corpse ( victim, killer );

    if ( IS_NPC ( victim ) && !IS_NPC ( killer ) && killer->clan == 5 )
	make_corpse ( victim, killer );

    if ( !IS_NPC ( victim ) && IS_NPC ( killer ) && victim->clan != 5 )
	make_corpse ( victim, killer );

    if ( !IS_NPC ( killer ) && !IS_NPC ( victim ) && ( victim != killer ) )
    {
	if ( killer->clan != 5 ) 
        victim->pcdata->pkdeaths++;
	if ( victim->clan != 5 ) 
        killer->pcdata->pkkills++;
        sprintf ( buf, "{Djust toasted {R%s{D's sorry ass in a {RPK {Dbattle!\n\r", victim->name );
        do_announce ( killer, buf );

/* Uncomment this if you wish, it's not unstable or anything,
   just be sure to make the soul object (OBJ_ITEM_SOUL in sd.h)
   before going live ;) - Rhaelar 12'2005 */
/*       if ( victim->clan != 5 && killer->clan != 5 )
       {
        pkills_today++;
	soul = create_object(get_obj_index(OBJ_ITEM_SOUL),0); 
	sprintf( buf, soul->short_descr,victim->name,killer->name); 
	free_string( soul->short_descr ); 
	soul->short_descr = str_dup( buf ); 
	sprintf(buf, soul->description,victim->name); 
	free_string( soul->description ); 
	soul->description = str_dup( buf ); 
	sprintf( buf, soul->name, victim->name, killer->name ); 
	free_string( soul->name ); 
	soul->name = str_dup( buf ); 
	obj_to_char(soul,killer); 
	sprintf(buf, "{wYou have {rc{Ra{rp{Rt{ru{Rr{re{Rd {wthe {Ds{wo{Du{wl of {y%s{w!{x\n\r",victim->name); 
	send_to_char(buf,killer); 
	sprintf(buf, "{wYou {rs{Ru{rr{Rr{re{Rn{rd{Re{rr {wyour {Ds{wo{Du{wl to {y%s{w.{x\n\r",killer->name); 
	send_to_char(buf,victim); 
       } 
*/
	stop_fighting ( victim, TRUE );
	stop_fighting ( killer, TRUE );
	REMOVE_BIT ( victim->act2, PLR2_PKVIOLENT );
	REMOVE_BIT ( victim->act, PLR_NORESTORE );        
	victim->pcdata->pkviolenttime = 0;	
	pksafe ( victim, 8 );
/*	if ( !IS_SET ( killer->act2, PLR2_PKVIOLENT ) )
	SET_BIT ( killer->act2, PLR2_PKVIOLENT );
	killer->pcdata->pkviolenttime = 6; */
	pkviolent ( killer, 6 );
    }

  if ( IS_NPC ( victim ) )
  {
        mobkills_today++;
        victim->pIndexData->killed++;
        kill_table[URANGE ( 0, victim->level, MAX_LEVEL - 1 )].killed++;
        extract_char ( victim, TRUE );
    if ( !IS_NPC ( killer ) )
      killer->pcdata->mkill++;
        return;
  }

  if (!IS_NPC (victim))
  {
    if (IS_NPC (killer))  /* CHAR_DATA *killer, added to raw_kill()  */
    {
      kill_table[URANGE (0, killer->level, MAX_LEVEL - 1)].kills++;
      killer->pIndexData->kills++;
      victim->pcdata->mdeath++;
      mobdeaths_today++;
    }
	if ( victim->clan == 5 )
	{
	   if ( IS_NPC ( killer ) ) 
           extract_char ( victim, FALSE );
	   else
	   {
		stop_fighting ( victim, TRUE );
		stop_fighting ( killer, TRUE );
		char_from_room ( victim );
		char_to_room ( victim, get_room_index ( clan_table[victim->clan].hall ) );
	   }
       }
   
    else if ( !IS_NPC ( killer ) && killer->clan == 5 )
    {
                stop_fighting ( victim, TRUE );
                stop_fighting ( killer, TRUE );
                char_from_room ( victim );
                char_to_room ( victim, get_room_index ( clan_table[victim->clan].hall ) );
    }
     else
        extract_char ( victim, FALSE );
  

	 if ( IS_NPC ( killer ) )
	  {
       	   sprintf ( buf, "{Dwas just killed by %s{D.{x\n\r", killer->short_descr );
           do_announce ( victim, buf );
	  }
    }
    while ( victim->affected )
        affect_remove ( victim, victim->affected );
    victim->affected_by = race_table[victim->race].aff;
    victim->shielded_by = race_table[victim->race].shd;
    for ( i = 0; i < 4; i++ )
        victim->armor[i] = 100;
    victim->position = POS_RESTING;
/*    victim->hit = UMAX ( 50, victim->hit );
    victim->mana = UMAX ( 50, victim->mana );
    victim->move = UMAX ( 50, victim->move ); */
    victim->hit = ( victim->max_hit / 5 );
    victim->mana = ( victim->max_mana / 5 );
    victim->move = ( victim->max_move / 5 ); 
    victim->stunned = 0;
/*    if ( !IS_NPC ( victim ) && tattoo != NULL )
    {
        obj_to_char ( tattoo, victim );
        equip_char ( victim, tattoo, WEAR_TATTOO );
    }
    if ( !IS_NPC ( victim ) && ctattoo != NULL )
    {
        obj_to_char ( ctattoo, victim );
        equip_char ( victim, tattoo, WEAR_CTATTOO );
    } */
    save_char_obj( victim );
    save_char_obj( killer );
    return;
}

void group_gain ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    AFFECT_DATA *paf;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( victim == ch )
        return;

    members = 0;
    group_levels = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( is_same_group ( gch, ch ) )
        {
            members++;
            group_levels += IS_NPC ( gch ) ? gch->level / 2 : gch->level;
        }
    }

    if ( members == 0 )
    {
        bug ( "Group_gain: members.", members );
        members = 1;
        group_levels = ch->level;
    }

    lch = ( ch->leader != NULL ) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        if ( !is_same_group ( gch, ch ) || IS_NPC ( gch ) )
            continue;

        if ( gch->level - lch->level >= 15 )
        {
            send_to_char ( "You are too high for this group.\n\r", gch );
            continue;
        }

        if ( gch->level - lch->level <= -15 )
        {
            send_to_char ( "You are too low for this group.\n\r", gch );
            continue;
        }
/*
	if (IS_NPC(victim) && !IS_NPC(ch) &&
	    gquest_info.running == GQUEST_RUNNING && ON_GQUEST(ch) &&
	    (i = is_gqmob(ch, victim->pIndexData->vnum)) != -1)
	{
	    ch->pcdata->gq_mobs[i] = -1;
	    send_to_char("Congratulations, that that mob was part of your global quest!\n\r",ch);
	    send_to_char("You receive an extra 3 Quest Points", ch);
	    ch->pcdata->questpoints += 3;
	    if (chance((MAX_GQUEST_MOB - gquest_info.mob_count) / 2 + gquest_info.mob_count))
	    {
		send_to_char("You receive an {YADITIONAL{x 1 quest point!!\n\r", ch);
		ch->pcdata->questpoints += 1;
	    }
            
	    else
		send_to_char(".\n\r", ch);

	    sprintf(buf, "$N has killed %s, a global questmob.", victim->short_descr);
	    wiznet(buf, ch, NULL, 0, 0, 0);

	    if (count_gqmobs(ch) == gquest_info.mob_count)
		send_to_char("You are now ready to complete the global quest. Type 'GQUEST COMPLETE' to finish.\n\r", ch);
	}
 */
        xp = xp_compute ( gch, victim, group_levels );

	if ( IS_SET ( class_table[ch->class].tier, TIER_01 ) )
	xp = xp * 2;
	else if ( IS_SET ( class_table[ch->class].tier, TIER_02 ) )
	xp = xp;
	else if ( IS_SET ( class_table[ch->class].tier, TIER_03 ) )
	xp = xp / 2;
	else
	xp = xp / 4;
	if ( !IS_NPC ( ch ) && is_real_pkiller ( ch ) )
	{
	    xp = xp * 2;
	    send_to_char ( "{rP{RK {rB{Ro{rn{Ru{rs{R!{x\n\r", ch );
	}
	if ( !IS_SET ( ch->act2, PLR2_NO_EXP ) )
        sprintf ( buf, "{BYou receive {W%d{B experience points.{x\n\r", xp );
	else sprintf ( buf, "\n\r" );
        send_to_char ( buf, gch );
//	save_char_obj ( ch );

        if ( gch->affected )
        {
            for ( paf = gch->affected; paf != NULL; paf = paf->next )
            {

                if ( paf->type && skill_table[paf->type].name &&
                     str_cmp ( skill_table[paf->type].name, "4x" ) == 0 )
                {
                    xp = xp * paf->modifier;
                    sprintf ( buf2,
                              "{MYou recieve {C%d{Wx {MExperience!{x\n\r",
                              paf->modifier );
                    send_to_char ( buf2, gch );
                }
            }
        }
        if ( double_exp && !is_affected ( gch, skill_lookup ( "4x" ) ) )
        {
            sprintf ( buf, "{GYou receive %d double exp!{x\n\r", xp );
            send_to_char ( buf, gch );
            xp = xp * 2;
        }

        if ( IS_QUESTOR ( ch ) && IS_NPC ( victim ) )
        {
            if ( ch->pcdata->questmob == victim->pIndexData->vnum && victim->isquestmob > 0 ) 
            {
                send_to_char
                    ( "{RYou have almost completed your {GQ{gU{GE{gS{GT{Y!{x\n\r",
                      ch );
                send_to_char
                    ( "{RReturn to the {Bq{bu{Be{bs{Bt{bm{Ba{bs{Bt{be{Br{R before your time runs out{Y!{x\n\r",
                      ch );
                ch->pcdata->questmob = -1;
            }
	    else if ( ch->pcdata->questmob == victim->pIndexData->vnum && victim->isquestmob == 0 )
	    {
	    	send_to_char ( "{RYou killed the wrong mob. This was not the one you had to kill.{x\n\r", ch );
	    }
		    
        }
	if ( IS_SET ( ch->act2, PLR2_NO_EXP ) )
	   send_to_char ( "Your exp gain was {Ywithdrawn{x cause you've got {Ynoexp{x on.\n\r", ch );
	else
        gain_exp ( gch, xp );

        for ( obj = gch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if ( obj->wear_loc == WEAR_NONE )
                continue;

            if ( ( IS_OBJ_STAT ( obj, ITEM_ANTI_EVIL ) && IS_EVIL ( gch ) ) ||
                 ( IS_OBJ_STAT ( obj, ITEM_ANTI_GOOD ) && IS_GOOD ( gch ) ) ||
                 ( IS_OBJ_STAT ( obj, ITEM_ANTI_NEUTRAL ) &&
                   IS_NEUTRAL ( gch ) ) )
            {
                act ( "{cYou are {Wzapped{c by $p.{x", gch, obj, NULL,
                      TO_CHAR );
                act ( "$n is {Wzapped{x by $p.", gch, obj, NULL, TO_ROOM );
                obj_from_char ( obj );
                obj_to_room ( obj, gch->in_room );
            }
        }
    }

    return;
}

/*
* Compute xp for a kill.
* Also adjust alignment of killer.
* Edit this function to change xp computations.
*/
int xp_compute ( CHAR_DATA * gch, CHAR_DATA * victim, int total_levels )
{
    int xp, xp90, base_exp;
    int align, level_range;
    int change;

    level_range = victim->level - gch->level;

    if ( !IS_NPC ( gch ) && !IS_NPC ( victim ) )
    {
        xp = 1;
        return xp;
    }

    /* compute the base exp */
    switch ( level_range )
    {
        default:
            base_exp = 0;
            break;
        case -9:
            base_exp = 1;
            break;
        case -8:
            base_exp = 2;
            break;
        case -7:
            base_exp = 5;
            break;
        case -6:
            base_exp = 9;
            break;
        case -5:
            base_exp = 11;
            break;
        case -4:
            base_exp = 22;
            break;
        case -3:
            base_exp = 33;
            break;
        case -2:
            base_exp = 50;
            break;
        case -1:
            base_exp = 66;
            break;
        case 0:
            base_exp = 83;
            break;
        case 1:
            base_exp = 95;
            break;
        case 2:
            base_exp = 110;
            break;
        case 3:
            base_exp = 120;
            break;
        case 4:
            base_exp = 140;
            break;
    }

    if ( level_range > 4 )
        base_exp = 160 + 20 * ( level_range - 4 );

    if ( IS_NPC ( victim ) )    /* at max a mob with all worth 5 level above his own */
    {
        if ( is_affected ( victim, skill_lookup ( "sanctuary" ) ) )
            base_exp = ( base_exp * 130 ) / 100;
        if ( is_affected ( victim, skill_lookup ( "haste" ) ) )
            if ( IS_SET ( victim->off_flags, OFF_AREA_ATTACK ) )
                base_exp = ( base_exp * 120 ) / 100;
        if ( IS_SET ( victim->off_flags, OFF_BACKSTAB ) )
            base_exp = ( base_exp * 120 ) / 100;
        if ( IS_SET ( victim->off_flags, OFF_FAST ) )
            base_exp = ( base_exp * 120 ) / 100;
        if ( IS_SET ( victim->off_flags, OFF_DODGE ) )
            base_exp = ( base_exp * 110 ) / 100;
        if ( IS_SET ( victim->off_flags, OFF_PARRY ) )
            base_exp = ( base_exp * 110 ) / 100;

        if ( victim->spec_fun != 0 )
        {
            if ( !str_cmp
                 ( spec_name ( victim->spec_fun ), "spec_breath_any" ) ||
                 !str_cmp ( spec_name ( victim->spec_fun ),
                            "spec_breath_acid" ) ||
                 !str_cmp ( spec_name ( victim->spec_fun ),
                            "spec_breath_fire" ) ||
                 !str_cmp ( spec_name ( victim->spec_fun ),
                            "spec_breath_frost" ) ||
                 !str_cmp ( spec_name ( victim->spec_fun ),
                            "spec_breath_gas" ) ||
                 !str_cmp ( spec_name ( victim->spec_fun ),
                            "spec_breath_lightning" ) )
                base_exp = ( base_exp * 125 ) / 100;

            else if ( !str_cmp
                      ( spec_name ( victim->spec_fun ), "spec_cast_cleric" )
                      || !str_cmp ( spec_name ( victim->spec_fun ),
                                    "spec_cast_mage" ) ||
                      !str_cmp ( spec_name ( victim->spec_fun ),
                                 "spec_cast_undead" ) )
                base_exp = ( base_exp * 120 ) / 100;

            else if ( !str_cmp
                      ( spec_name ( victim->spec_fun ), "spec_poison" ) )
                base_exp = ( base_exp * 110 ) / 100;
        }
    }
    /*    back to normal code    -------------------- */

    /* do alignment computations */

    align = victim->alignment - gch->alignment;

    if ( IS_SET ( victim->act, ACT_NOALIGN ) )
    {
        /* no change */
    }

    else if ( align > 500 )     /* monster is more good than slayer */
    {
        change = ( align - 500 ) * base_exp / 500 * gch->level / total_levels;
        change = UMAX ( 1, change );
        gch->alignment = UMAX ( -1000, gch->alignment - change );
        if ( gch->pet != NULL )
            gch->pet->alignment = gch->alignment;
    }

    else if ( align < -500 )    /* monster is more evil than slayer */
    {
        change =
            ( -1 * align - 500 ) * base_exp / 500 * gch->level / total_levels;
        change = UMAX ( 1, change );
        gch->alignment = UMIN ( 1000, gch->alignment + change );
        if ( gch->pet != NULL )
            gch->pet->alignment = gch->alignment;
    }

    else
        /* improve this someday */
    {
        change = gch->alignment * base_exp / 500 * gch->level / total_levels;
        gch->alignment -= change;
        if ( gch->pet != NULL )
            gch->pet->alignment = gch->alignment;
    }

    /* calculate exp multiplier for align */
    if ( IS_SET ( victim->act, ACT_NOALIGN ) )
        xp = base_exp;

    else if ( gch->alignment > 500 )    /* for goodie two shoes */
    {
        if ( victim->alignment < -750 )
            xp = ( base_exp * 4 ) / 3;

        else if ( victim->alignment < -500 )
            xp = ( base_exp * 5 ) / 4;

        else if ( victim->alignment > 750 )
            xp = base_exp / 4;

        else if ( victim->alignment > 500 )
            xp = base_exp / 2;

        else if ( victim->alignment > 250 )
            xp = ( base_exp * 3 ) / 4;

        else
            xp = base_exp;
    }

    else if ( gch->alignment < -500 )   /* for baddies */
    {
        if ( victim->alignment > 750 )
            xp = ( base_exp * 5 ) / 4;

        else if ( victim->alignment > 500 )
            xp = ( base_exp * 11 ) / 10;

        else if ( victim->alignment < -750 )
            xp = base_exp / 2;

        else if ( victim->alignment < -500 )
            xp = ( base_exp * 3 ) / 4;

        else if ( victim->alignment < -250 )
            xp = ( base_exp * 9 ) / 10;

        else
            xp = base_exp;
    }

    else if ( gch->alignment > 200 )    /* a little good */
    {

        if ( victim->alignment < -500 )
            xp = ( base_exp * 6 ) / 5;

        else if ( victim->alignment > 750 )
            xp = base_exp / 2;

        else if ( victim->alignment > 0 )
            xp = ( base_exp * 3 ) / 4;

        else
            xp = base_exp;
    }

    else if ( gch->alignment < -200 )   /* a little bad */
    {
        if ( victim->alignment > 500 )
            xp = ( base_exp * 6 ) / 5;

        else if ( victim->alignment < -750 )
            xp = base_exp / 2;

        else if ( victim->alignment < 0 )
            xp = ( base_exp * 3 ) / 4;

        else
            xp = base_exp;
    }

    else
        /* neutral */
    {

        if ( victim->alignment > 500 || victim->alignment < -500 )
            xp = ( base_exp * 4 ) / 3;

        else if ( victim->alignment < 200 && victim->alignment > -200 )
            xp = base_exp / 2;

        else
            xp = base_exp;
    }

/*(   
    if (gch->level < 26)
    	xp = 10 * xp / (gch->level + 20);

    
    if (gch->level > 26 )
	xp =  10 * xp / (gch->level - 60 );
*/
    if ( gch->level > 99 )
        xp = 90 * xp / gch->level;
    /* randomize the rewards */
    xp = number_range ( xp * 8 / 5, xp * 9 / 5 );

    xp90 = 90 * xp / 100;       /* 90% limit for one grouped player   */

    /* adjust for grouping */
    xp = xp * gch->level / ( UMAX ( 1, total_levels - 1 ) );

    /* modified for groups of more than 1 -- Elrac */
    if ( gch->level != total_levels )
    {
        xp = 120 * xp / 100;    /* everyone gets a 20% bonus for grouping */
        if ( xp > xp90 )
            xp = xp90;          /* but no more than the single grouped player limit */
    }

    while ( xp > 5000 )
    {
        xp = xp * .95;
    }

    return xp;
}

void dam_message ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
                   bool immune, int verbose, bool singular )
{
    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;

    if ( ch == NULL || victim == NULL )
        return;

if ( dam ==   0 ) { vs = "{Wmiss{W"; vp = "{Wmisses{W"; }
    else if ( dam <=   4 ) 
    { 
    vs = "{Dscratch{W"; 
    vp = "{Dscratches{W"; 
    }
    
    else if ( dam <=   8 ) 
    { 
    vs = "graze"; 
    vp = "grazes"; 
    }
    
    else if ( dam <=  12 ) 
    { 
    vs = "{chit{W"; 
    vp = "{chits{W"; 
    }
    
    else if ( dam <=  16 ) 
    { 
    vs = "{cinjure{W"; 
    vp = "{cinjures{W"; 
    }
    
    else if ( dam <=  20 ) 
    { 
    vs = "{Cwound{W"; 
    vp = "{cwounds{W"; 
    }
    
    else if ( dam <=  24 ) 
    { 
    vs = "{Cmaul{W"; 
    vp = "{Cmauls{W"; 
    }
    
    else if ( dam <=  28 ) 
    { 
    vs = "{Gdecimate{W"; 
    vp = "{Gdecimates{W"; 
    }
    
    else if ( dam <=  32 ) 
    { 
    vs = "{Gdevastate{W"; 
    vp = "{Gdevastates{W";
    }
    
    else if ( dam <=  36 ) 
    { 
    vs = "{rmaim{W"; 
    vp = "{rmaims{W"; 
    }
    
    else if ( dam <=  40 ) 
    { 
    vs = "{MMUTILATE{W"; 
    vp = "{MMUTILATES{W"; 
    }
    
    else if ( dam <=  44 ) 
    { 
    vs = "{RDISEMBOWEL{W"; 
    vp = "{RDISEMBOWELS{W"; 
    }
    
    else if ( dam <=  48 ) 
    { 
    vs = "{RDISMEMBER{W"; 
    vp = "{RDISMEMBERS{W"; 
    }
    
    else if ( dam <=  52 ) 
    { 
    vs = "{DM{wA{DS{wS{DA{wC{DR{wE{W"; 
    vp = "{DM{wA{DS{wS{DA{wC{DR{wE{DS{W"; 
    }
    
    else if ( dam <=  56 ) 
    { 
    vs = "{RM{rA{RN{rG{RL{rE{W"; 
    vp = "{RM{rA{RN{rG{RL{rE{RS{W"; 
    }
    
    else if ( dam <=  60 ) 
    { 
    vs = "{Y### {RD{YE{RM{YO{RL{YI{RS{YH ###{W";
    vp = "{Y### {RD{YE{RM{YO{RL{YI{RS{YH{RE{YS ###{W"; 
    }
    
    else if ( dam <=  75 ) 
    { 
    vs = "{R### {RD{rEVASTAT{RE ###{W"; 
    vp = "{R### {RD{rEVASTATE{RS ###{W"; 
    }
    
    else if ( dam <= 100 ) 
    {
    vs = "{m<{C|{m> {GO{gB{GL{gI{GT{gE{GR{gA{GT{gE {m<{W|{m>{W";
    vp = "{m<{C|{m> {GO{gB{GL{gI{GT{gE{GR{gA{GT{gE{GS {m<{C|{m>{W"; 
    }
    
    else if ( dam <= 125 ) 
    { 
    vs = "{r>{R|{r< {YA{yN{wNI{WHI{wLA{yT{YE{W {r>{R|{r<{W";
    vp = "{r>{R|{r< {YA{yN{wNI{WHIL{wAT{yE{YS{W {r>{R|{r<{W"; 
    }
    
    else if ( dam <= 150 ) 
    { 
    vs = "{C^{**{C^ {WER{wA{GD{gI{GC{wA{WTE {C^{**{C^{W";
    vp = "{C^{**{C^ {WER{wA{GD{gI{GC{wA{WTES {C^{**{C^{W"; 
    }
    
    else if ( dam <= 200 ) 
    {
    vs = "{G@@{g@{G@@ {MN{m!{MU{m!{MK{m!{ME {G@@{g@{G@@{W";
    vp = "{G@@{g@{G@@ {MN{m!{MU{m!{MK{m!{ME{m!{MS {G@@{g@{G@@{W"; 
    }
    
    else if ( dam <= 250 ) 
    { 
    vs = "{R***{r@@@ {RM{r*{RA{r*{RS{r*{RS{r*{RA{r*{RC{r*{RR{r*{RE {r@@@{R***{W";
    vp = "{R***{r@@@ {RM{r*{RA{r*{RS{r*{RS{r*{RA{r*{RC{r*{RR{r*{RE{r*{RS {r@@@{R***{W"; 
    }
    
    else if ( dam <= 300 ) 
    {
    vs = "{W///{b^^^{x {bI^{BM^{CP^{WL^{CO^{BD^{bE {b^^^{x{W\\\\\\{W";
    vp = "{W///{b^^^{x {bI^{BM^{CP^{WL^O^{CD^{BE^{bS {b^^^{x{W\\\\\\{W"; 
    }
    
    else if ( dam <= 350 ) 
    { 
    vs = "{R***{r@@@@ {WE>r>{wA>{GD>{gI<{GC<{wA<{Wt<E {r@@@@{R***{W";
    vp = "{R***{r@@@@ {WE>r>{wA>{GD>{gI<{GC<{wA<{WT<e<S {r@@@@{R***{W"; 
    }
    
    else if ( dam <= 399 ) 
    { 
    vs = "{G@@{g@{G@@ {MN{m!{MU{m!{MK{m!{ME {G@@{g@{G@@{W {z{Yx2 DAM{x{W to";
    vp = "{G@@{g@{G@@ {MN{m!{MU{m!{MK{m!{ME{m!{MS {G@@{g@{G@@{W {z{Yx2 DAM{x{W to"; 
    }
    
    else if ( dam <= 500 ) 
    { 
    vs = "{R***{r@@@ {RM{r*{RA{r*{RS{r*{RS{r*{RA{r*{RC{r*{RR{r*{RE {r@@@{R***{W {Y{zx2 DAM{x{W to";
    vp = "{R***{r@@@ {RM{r*{RA{r*{RS{r*{RS{r*{RA{r*{RC{r*{RR{r*{RE{r*{RS {r@@@{R***{W {Y{zx2 DAM{x{W to"; 
    }
    
    else if ( dam <= 600 ) 
    { 
    vs = "{W///{B^^^{x {bI^{BM^{CP^{WL^{CO^{BD^{bE {W{B^^^{x{W\\\\\\{W {Y{zx2 DAM{x{W to";
    vp = "{W///{B^^^{x {bI^{BM^{CP^{WL^O^{CD^{BE^{bS {B^^^{x{W\\\\\\{W {Y{zx2 DAM{x{W to"; 
    }
    
    else if ( dam <= 700 ) 
    { 
    vs = "{R***{r@@@@ {WE>r>{wA>{GD>{gI<{GC<{wA<{Wt<E {r@@@@{R***{W {Y{zx2 DAM{x{W to";
    vp = "{R***{r@@@@ {WE>r>{wA>{GD>{gI<{GC<{wA<{WT<e<S {r@@@@{R***{W {Y{zx2 DAM{x{W to"; 
    }
    
    else if ( dam <= 1000 ) 
    { 
    vs = "do {RU*{rN*{yS*{YP*{DE{w*A*{DK{Y*A{y*B{r*L{R*E{W things to";
    vp = "does {RU*{rN*{yS*{YP*{DE{w*A*{DK{Y*A{y*B{r*L{R*E{W things to"; 
    }
    
    else if ( dam <= 2000 ) 
    { 
    vs = "{D|||{w%%% {RU*{rN*{yS*{YP*{DE{w*A*{DK{Y*A{y*B{r*L{R*E {w%%%{D|||{Y{zx2 DAM{x{W to";
    vp = "{D|||{w%%% {RU*{rN*{yS*{YP*{DE{w*A*{DK{Y*A{y*B{r*L{R*E {w%%%{D||| {Y{zx2 DAM{x{W to"; 
    }
    
    else if ( dam <= 1200 ) 
    { 
    vs = "{G@@{g@{G@@ {MN{m!{MU{m!{MK{m!{ME {G@@{g@{G@@{W {z{Rx3 DAM{x{W to";
    vp = "{G@@{g@{G@@ {MN{m!{MU{m!{MK{m!{ME{m!{MS {G@@{g@{G@@{W {z{Rx3 DAM{x{W to"; 
    }
    
    else if ( dam <= 1500 ) 
    { 
    vs = "{R***{r@@@ {RM{r*{RA{r*{RS{r*{RS{r*{RA{r*{RC{r*{RR{r*{RE {r@@@{R***{W{R{zx3 DAM{x{W to";
    vp = "{R***{r@@@ {RM{r*{RA{r*{RS{r*{RS{r*{RA{r*{RC{r*{RR{r*{RE{r*{RS {r@@@{R***{W {R{zx3 DAM{x{W to"; 
    }
    
    else if ( dam <= 1800 ) 
    { 
    vs = "{W///{z^^^{x {bI^{BM^{CP^{WL^{CO^{BD^{bE {z^^^{x{W\\\\\\{W {R{zx3 DAM{x{W to";
    vp = "{W///{z^^^{x {bI^{BM^{CP^{WL^O^{CD^{BE^{bS {z^^^{x{W\\\\\\{W {R{zx3 DAM{x{W to"; 
    }
    
    else if ( dam <= 2100 ) 
    { 
    vs = "{R***{r@@@@ {WE>r>{wA>{GD>{gI<{GC<{wA<{Wt<E {r@@@@{R***{W {R{zx3 DAM{x{W to";
    vp = "{R***{r@@@@ {WE>r>{wA>{GD>{gI<{GC<{wA<{WT<e<S {r@@@@{R***{W {R{zx3 DAM{x{W to"; 
    }
    
    else if ( dam <= 6000 ) 
    { 
    vs = "{D|||{w%%% {RU*{rN*{yS*{YP*{DE{w*A*{DK{Y*A{y*B{r*L{R*E {w%%%{D|||{R{zx3 DAM{x{W to";
    vp = "{D|||{w%%% {RU*{rN*{yS*{YP*{DE{w*A*{DK{Y*A{y*B{r*L{R*E {w%%%{D||| {R{zx3 DAM{x{W to"; 
    }
    
    else if ( dam <= 30000 )
    {
    vs = "does {Y{zINSTANTANEOUS{x {RH{rE{RA{rR{RT {BS{bUR{BG{bER{BY{x{W to";
    vp = "does {Y{zINSTANTANEOUS{x {RH{rE{RA{rR{RT {BS{bUR{BG{bER{BY{x{W to"; 
    }

    else if ( dam <= 100000 )
    {
    vs = "{R====={Y/////{*T{*O{*T{*A{*L{*L{*Y {*F{*U{*C{*K{*S {*U{*P{Y/////{R====={x";
    vp = "{R====={Y/////{*T{*O{*T{*A{*L{*L{*Y {*F{*U{*C{*K{*S {*U{*P{Y/////{R====={x";
    }

    else
    {
    vs = "does {R|||{w%%% {ROGC{x-like {w%%%{R||| {Yd{yamag{Ye{x to";
    vp = "does {R|||{w%%% {ROGC{x-like {w%%%{R||| {Yd{yamag{Ye{x to";
    }

    punct = ( dam <= 24 ) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
        if ( ch == victim )
        {
            sprintf ( buf1, "$n %s $melf for {G*{W%d{G*{x damage%c{x", vp,
                      dam, punct );
            sprintf ( buf2, "You %s yourself for {G*{W%d{G*{x damage%c{x", vs,
                      dam, punct );
        }
        else
        {
            sprintf ( buf1, "$n %s $N for {g>{G>{Y%d{G<{g<{x damage%c{x", vp,
                      dam, punct );
            sprintf ( buf2, "You %s $N for {g>{G>{B%d{G<{g<{x damage%c{x", vs,
                      dam, punct );
            sprintf ( buf3, "$n %s you for {w>{W>{R%d{W<{w<{x damage%c{x", vp,
                      dam, punct );
        }
    }

/*    else
    {
	if ( dt >= 0 && dt < MAX_SKILL )
	    attack	= skill_table[dt].noun_damage;
	else if ( dt >= TYPE_HIT
	&& dt < TYPE_HIT + MAX_DAMAGE_MESSAGE) 
	    attack	= attack_table[dt - TYPE_HIT].noun;
	else
	{
	    bug( "Dam_message: bad dt %d.", dt );
	    dt  = TYPE_HIT;
	    attack  = attack_table[0].name;
	}
*/
    else
    {
        if ( dt >= 0 && dt < MAX_SKILL )
            attack = skill_table[dt].noun_damage;
        else if ( dt >= TYPE_HIT && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE )
            attack = attack_table[dt - TYPE_HIT].noun;
        else if ( dt == -1 && verbose == VERBOSE_SHIELD_COMP )
        {
            if ( singular )
                attack = "shield";
            else
                attack = "shields";
        }
        else
        {
            bug ( "Dam_message: bad dt %d.", dt );
            dt = TYPE_HIT;
            attack = attack_table[0].name;
        }
        if ( immune )
        {
            if ( ch == victim )
            {
                sprintf ( buf1, "$n is unaffected by $s own %s.{x", attack );
                sprintf ( buf2, "Luckily, you are immune to that.{x" );
            }
            else
            {
                sprintf ( buf1, "$N is unaffected by $n's %s!{x", attack );
                sprintf ( buf2, "$N is unaffected by your %s!{x", attack );
                sprintf ( buf3, "$n's %s is powerless against you.{x",
                          attack );
            }
        }
        else
        {
            if ( ch == victim )
            {
                sprintf ( buf1,
                          "$n's %s %s $m for {g>{G>{W%d{G<{g<{x damage%c{x",
                          attack, vp, dam, punct );
                sprintf ( buf2,
                          "Your %s %s you for {w>{W>{R%d{W<{w<{x damage%c{x",
                          attack, vp, dam, punct );
            }
            else
            {
                sprintf ( buf1,
                          "$n's %s %s $N for {g>{G>{Y%d{G<{g<{x damage%c{x",
                          attack, vp, dam, punct );
                sprintf ( buf2,
                          "Your %s %s $N for {g>{G>{W%d{G<{g<{x damage%c{x",
                          attack, vp, dam, punct );
                sprintf ( buf3,
                          "$n's %s %s you for {w>{W>{R%d{W<{w<{x damage%c{x",
                          attack, vp, dam, punct );
            }
        }
    }

    if ( ch == victim )
    {
        xact_new ( buf1, ch, NULL, NULL, TO_ROOM, POS_RESTING, verbose );
        xact_new ( buf2, ch, NULL, NULL, TO_CHAR, POS_RESTING, verbose ); 
/*        act_new ( buf1, ch, NULL, NULL, TO_ROOM, POS_RESTING );
        act_new ( buf2, ch, NULL, NULL, TO_CHAR, POS_RESTING ); */
    }
    else
    {
        xact_new ( buf1, ch, NULL, victim, TO_NOTVICT, POS_RESTING, verbose );
        xact_new ( buf2, ch, NULL, victim, TO_CHAR, POS_RESTING, verbose );
        xact_new ( buf3, ch, NULL, victim, TO_VICT, POS_RESTING, verbose );
/*        act_new ( buf1, ch, NULL, victim, TO_NOTVICT, POS_RESTING );
        act_new ( buf2, ch, NULL, victim, TO_CHAR, POS_RESTING );
        act_new ( buf3, ch, NULL, victim, TO_VICT, POS_RESTING ); */
    }
    if ( dt == -1 && verbose == VERBOSE_SHIELD_COMP )

        return;
}

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char ( victim, WEAR_WIELD ) ) == NULL )
        return;

    if ( IS_OBJ_STAT ( obj, ITEM_NOREMOVE ) )
    {
        act ( "$S weapon won't budge!{x", ch, NULL, victim, TO_CHAR );
        act ( "$n tries to disarm you, but your weapon won't budge!{x", ch,
              NULL, victim, TO_VICT );
        act ( "{k$n tries to disarm $N, but fails.{x", ch, NULL, victim,
              TO_NOTVICT );
        return;
    }

    act ( "{W$n {C|||{Y{zDISARMS{x{C|||{R you and sends your weapon flying!{x", ch, NULL,
          victim, TO_VICT );
    act ( "{YYou disarm $N!{x", ch, NULL, victim, TO_CHAR );
    act ( "{Y$n disarms $N!{x", ch, NULL, victim, TO_NOTVICT );

    obj_from_char ( obj );
/*    if ( IS_OBJ_STAT ( obj, ITEM_NODROP ) ||
         IS_OBJ_STAT ( obj, ITEM_INVENTORY ) )
        obj_to_char ( obj, victim );
    else
    {
        obj_to_room ( obj, victim->in_room );
        if ( IS_NPC ( victim ) && victim->wait == 0 &&
             can_see_obj ( victim, obj ) )
            get_obj ( victim, obj, NULL );
    }*/
     obj_to_char ( obj, victim );

    return;
}

/*
        {RIn a fury of rage $n screams, $s hair turns a {Yb{Wl{Yi{Wn{Yd{Wi{Yn{Wg
{Yy{We{Yl{Wl{Yo{Ww,{R $s eyes turn {GG{gr{Ge{ge{Gn,{R and a spark of {Bl{Wi{Bg{Wh{Bt{Wn{Bi{Wn{Bg{R
surrounds $m!{x
*/

CH_CMD ( do_berserk )
{
    int chance, hp_percent;

    if ( ( chance = get_skill ( ch, gsn_berserk ) ) == 0 ||
         ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_BERSERK ) ) ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_berserk].skill_level[ch->class] ) )
    {
        send_to_char
            ( "{hYou turn {rred{h in the face, but nothing happens.{x\n\r",
              ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_BERSERK ) || is_affected ( ch, gsn_berserk ) ||
         is_affected ( ch, skill_lookup ( "frenzy" ) ) )
    {
        send_to_char ( "{hYou get a little madder.{x\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CALM ) )
    {
        send_to_char ( "{hYou're feeling to mellow to berserk.{x\n\r", ch );
        return;
    }

    if ( ch->mana < 50 )
    {
        send_to_char ( "{hYou can't get up enough energy.{x\n\r", ch );
        return;
    }

    /* modifiers */

    /* fighting */
    if ( ch->position == POS_FIGHTING )
        chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit / ch->max_hit;
    chance += 25 - hp_percent / 2;

    if ( number_percent (  ) < chance )
    {
        AFFECT_DATA af;

        WAIT_STATE ( ch, PULSE_VIOLENCE );
        ch->mana -= 50;
        ch->move /= 2;

        /* heal a little damage */
        ch->hit += ch->level * 2;
        ch->hit = UMIN ( ch->hit, ch->max_hit );

        send_to_char
            ( "{hYour pulse races as you are consumed by {rrage!{x\n\r", ch );
        act ( "{k$n gets a {cw{gi{rl{yd{k look in $s eyes.{x", ch, NULL, NULL,
              TO_ROOM );
        check_improve ( ch, gsn_berserk, TRUE, 2 );

        af.where = TO_AFFECTS;
        af.type = gsn_berserk;
        af.level = ch->level;
        af.duration = number_fuzzy ( ch->level / 8 );
        af.modifier = UMAX ( 1, ch->level / 5 );
        af.bitvector = AFF_BERSERK;

        af.location = APPLY_HITROLL;
        affect_to_char ( ch, &af );

        af.location = APPLY_DAMROLL;
        affect_to_char ( ch, &af );

        af.modifier = UMAX ( 10, 10 * ( ch->level / 5 ) );
        af.location = APPLY_AC;
        affect_to_char ( ch, &af );
    }

    else
    {
        WAIT_STATE ( ch, 3 * PULSE_VIOLENCE );
        ch->mana -= 25;
        ch->move /= 2;

        send_to_char ( "{hYour pulse speeds up, but nothing happens.{x\n\r",
                       ch );
        check_improve ( ch, gsn_berserk, FALSE, 2 );
    }
}

CH_CMD ( do_voodoo )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *doll;

    if ( IS_NPC ( ch ) )
        return;

    doll = get_eq_char ( ch, WEAR_HOLD );
    if ( doll == NULL || ( doll->pIndexData->vnum != OBJ_VNUM_VOODOO ) )
    {
        send_to_char ( "You are not holding a voodoo doll.\n\r", ch );
        return;
    }

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax: voodoo <action>\n\r", ch );
        send_to_char ( "Actions: pin trip throw\n\r", ch );
        return;
    }

    if ( !str_cmp ( arg, "pin" ) )
    {
        do_vdpi ( ch, doll->name );
        return;
    }

    if ( !str_cmp ( arg, "trip" ) )
    {
        do_vdtr ( ch, doll->name );
        return;
    }

    if ( !str_cmp ( arg, "throw" ) )
    {
        do_vdth ( ch, doll->name );
        return;
    }

    do_voodoo ( ch, "" );
}

CH_CMD ( do_vdpi )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    bool found = FALSE;

    argument = one_argument ( argument, arg1 );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;

        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;

        wch = ( d->original != NULL ) ? d->original : d->character;

        if ( !can_see ( ch, wch ) )
            continue;

        if ( !str_cmp ( arg1, wch->name ) && !found )
        {
            if ( IS_NPC ( wch ) )
                continue;

            if ( IS_IMMORTAL ( wch ) && ( wch->level > ch->level ) )
            {
                send_to_char ( "That's not a good idea.\n\r", ch );
                return;
            }

            if ( ( wch->level < 20 ) && !IS_IMMORTAL ( ch ) )
            {
                send_to_char ( "They are a little too young for that.\n\r",
                               ch );
                return;
            }

            if ( IS_SHIELDED ( wch, SHD_PROTECT_VOODOO ) )
            {
                send_to_char
                    ( "They are still realing from a previous voodoo.\n\r",
                      ch );
                return;
            }

            found = TRUE;

            send_to_char ( "You stick a pin into your voodoo doll.\n\r", ch );
            act ( "$n sticks a pin into a voodoo doll.", ch, NULL, NULL,
                  TO_ROOM );
            send_to_char
                ( "{RYou double over with a sudden pain in your gut!{x\n\r",
                  wch );
            act ( "$n suddenly doubles over with a look of extreme pain!",
                  wch, NULL, NULL, TO_ROOM );
            af.where = TO_SHIELDS;
            af.type = skill_lookup ( "protection voodoo" );
            af.level = wch->level;
            af.duration = 1;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = SHD_PROTECT_VOODOO;
            affect_to_char ( wch, &af );
            return;
        }
    }
    send_to_char ( "Your victim doesn't seem to be in the realm.\n\r", ch );
    return;
}

CH_CMD ( do_vdtr )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    bool found = FALSE;

    argument = one_argument ( argument, arg1 );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;

        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;

        wch = ( d->original != NULL ) ? d->original : d->character;

        if ( !can_see ( ch, wch ) )
            continue;

        if ( !str_cmp ( arg1, wch->name ) && !found )
        {
            if ( IS_NPC ( wch ) )
                continue;

            if ( IS_IMMORTAL ( wch ) && ( wch->level > ch->level ) )
            {
                send_to_char ( "That's not a good idea.\n\r", ch );
                return;
            }

            if ( ( wch->level < 20 ) && !IS_IMMORTAL ( ch ) )
            {
                send_to_char ( "They are a little too young for that.\n\r",
                               ch );
                return;
            }

            if ( IS_SHIELDED ( wch, SHD_PROTECT_VOODOO ) )
            {
                send_to_char
                    ( "They are still realing from a previous voodoo.\n\r",
                      ch );
                return;
            }

            found = TRUE;

            send_to_char
                ( "You slam your voodoo doll against the ground.\n\r", ch );
            act ( "$n slams a voodoo doll against the ground.", ch, NULL,
                  NULL, TO_ROOM );
            send_to_char ( "{RYour feet slide out from under you!{x\n\r",
                           wch );
            send_to_char ( "{RYou hit the ground face first!{x\n\r", wch );
            act ( "$n trips over $s own feet, and does a nose dive into the ground!", wch, NULL, NULL, TO_ROOM );
            af.where = TO_SHIELDS;
            af.type = skill_lookup ( "protection voodoo" );
            af.level = wch->level;
            af.duration = 1;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = SHD_PROTECT_VOODOO;
            affect_to_char ( wch, &af );
            return;
        }
    }
    send_to_char ( "Your victim doesn't seem to be in the realm.\n\r", ch );
    return;
}

CH_CMD ( do_vdth )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    bool found = FALSE;
    int attempt;

    argument = one_argument ( argument, arg1 );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;

        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;

        wch = ( d->original != NULL ) ? d->original : d->character;

        if ( !can_see ( ch, wch ) )
            continue;

        if ( !str_cmp ( arg1, wch->name ) && !found )
        {
            if ( IS_NPC ( wch ) )
                continue;

            if ( IS_IMMORTAL ( wch ) && ( wch->level > ch->level ) )
            {
                send_to_char ( "That's not a good idea.\n\r", ch );
                return;
            }

            if ( ( wch->level < 20 ) && !IS_IMMORTAL ( ch ) )
            {
                send_to_char ( "They are a little too young for that.\n\r",
                               ch );
                return;
            }

            if ( IS_SHIELDED ( wch, SHD_PROTECT_VOODOO ) )
            {
                send_to_char
                    ( "They are still realing from a previous voodoo.\n\r",
                      ch );
                return;
            }

            found = TRUE;

            send_to_char ( "You toss your voodoo doll into the air.\n\r",
                           ch );
            act ( "$n tosses a voodoo doll into the air.", ch, NULL, NULL,
                  TO_ROOM );
            af.where = TO_SHIELDS;
            af.type = skill_lookup ( "protection voodoo" );
            af.level = wch->level;
            af.duration = 1;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = SHD_PROTECT_VOODOO;
            affect_to_char ( wch, &af );
            if ( ( wch->fighting != NULL ) || ( number_percent (  ) < 25 ) )
            {
                send_to_char
                    ( "{RA sudden gust of wind throws you through the air!{x\n\r",
                      wch );
                send_to_char
                    ( "{RYou slam face first into the nearest wall!{x\n\r",
                      wch );
                act ( "A sudden gust of wind picks up $n and throws $m into a wall!", wch, NULL, NULL, TO_ROOM );
                return;
            }
            wch->position = POS_STANDING;
            was_in = wch->in_room;
            for ( attempt = 0; attempt < 6; attempt++ )
            {
                EXIT_DATA *pexit;
                int door;

                door = number_door (  );
                if ( ( pexit = was_in->exit[door] ) == 0 ||
                     pexit->u1.to_room == NULL ||
                     IS_SET ( pexit->exit_info, EX_CLOSED ) ||
                     ( IS_NPC ( wch ) &&
                       IS_SET ( pexit->u1.to_room->room_flags,
                                ROOM_NO_MOB ) ) )
                    continue;

                move_char ( wch, door, FALSE, TRUE );
                if ( ( now_in = wch->in_room ) == was_in )
                    continue;

                wch->in_room = was_in;
                sprintf ( buf,
                          "A sudden gust of wind picks up $n and throws $m to the %s.",
                          dir_name[door] );
                act ( buf, wch, NULL, NULL, TO_ROOM );
                send_to_char
                    ( "{RA sudden gust of wind throws you through the air!{x\n\r",
                      wch );
                wch->in_room = now_in;
                act ( "$n sails into the room and slams face first into a wall!", wch, NULL, NULL, TO_ROOM );
                do_look ( wch, "auto" );
                send_to_char
                    ( "{RYou slam face first into the nearest wall!{x\n\r",
                      wch );
                return;
            }
            send_to_char
                ( "{RA sudden gust of wind throws you through the air!{x\n\r",
                  wch );
            send_to_char
                ( "{RYou slam face first into the nearest wall!{x\n\r", wch );
            act ( "A sudden gust of wind picks up $n and throws $m into a wall!", wch, NULL, NULL, TO_ROOM );
            return;
        }
    }
    send_to_char ( "Your victim doesn't seem to be in the realm.\n\r", ch );
    return;
}

CH_CMD ( do_bash )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument ( argument, arg );

    if ( ( chance = get_skill ( ch, gsn_bash ) ) == 0 ||
         ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_BASH ) ) ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_bash].skill_level[ch->class] ) )
    {
        send_to_char ( "Bashing? What's that?\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        victim = ch->fighting;
        if ( victim == NULL )
        {
            send_to_char ( "But you aren't fighting anyone!\n\r", ch );
            return;
        }
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim->position < POS_FIGHTING )
    {
        act ( "You'll have to let $M get back up first.", ch, NULL, victim,
              TO_CHAR );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "You try to bash your brains out, but fail.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim )
    {
        act ( "But $N is your friend!", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char
            ( "You get a running start, and slam right into a wall.\n\r",
              ch );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 200;
    chance -= victim->carry_weight / 150;

    if ( ch->size < victim->size )
        chance += ( ch->size - victim->size ) * 25;
    else
        chance += ( ch->size - victim->size ) * 20;

    /* stats */
    chance += get_curr_stat ( ch, STAT_STR );
    chance -= ( get_curr_stat ( victim, STAT_DEX ) * 4 ) / 3;
    chance += GET_AC ( victim, AC_BASH ) / 20;
    /* speed */
    if ( IS_SET ( ch->off_flags, OFF_FAST ) || IS_AFFECTED ( ch, AFF_HASTE ) )
        chance += 20;
    if ( IS_SET ( victim->off_flags, OFF_FAST ) ||
         IS_AFFECTED ( victim, AFF_HASTE ) )
        chance -= 30;

    /* level */
    chance += ( ch->level - victim->level );

    if ( !IS_NPC ( victim ) && chance < get_skill ( victim, gsn_dodge ) )
    {                           /*
                                   act("{i$n tries to bash you, but you dodge it.{x",ch,NULL,victim,TO_VICT);
                                   act("{h$N dodges your bash, you fall flat on your face.{x",ch,NULL,victim,TO_CHAR);
                                   WAIT_STATE(ch,skill_table[gsn_bash].beats);
                                   return; */
        chance -= 2 * ( get_skill ( victim, gsn_dodge ) - chance );
    }

    chance = ( chance * 4 );

    /* now the attack */
    if ( number_percent (  ) < chance )
    {

        act ( "{i$n sends you sprawling with a powerful bash!{x", ch, NULL,
              victim, TO_VICT );
        act ( "{hYou slam into $N, and send $M flying!{x", ch, NULL, victim,
              TO_CHAR );
        act ( "{k$n sends $N sprawling with a powerful bash.{x", ch, NULL,
              victim, TO_NOTVICT );
        check_improve ( ch, gsn_bash, TRUE, 1 );

        DAZE_STATE ( victim, 3 * PULSE_VIOLENCE );
        WAIT_STATE ( ch, skill_table[gsn_bash].beats );
        victim->position = POS_RESTING;
        damage ( ch, victim,
                 number_range ( 2, 2 + 2 * ch->size + chance / 20 ), gsn_bash,
                 DAM_BASH, FALSE, 1 );
        chance = ( get_skill ( ch, gsn_stun ) / 5 );
        if ( number_percent (  ) < chance )
        {
            chance = ( get_skill ( ch, gsn_stun ) / 5 );
            if ( number_percent (  ) < chance )
            {
                victim->stunned = 2;
            }
            else
            {
                victim->stunned = 1;
            }
            act ( "{iYou are stunned, and have trouble getting back up!{x",
                  ch, NULL, victim, TO_VICT );
            act ( "{h$N is stunned by your bash!{x", ch, NULL, victim,
                  TO_CHAR );
            act ( "{k$N is having trouble getting back up.{x", ch, NULL,
                  victim, TO_NOTVICT );
            check_improve ( ch, gsn_stun, TRUE, 1 );
        }
    }
    else
    {
        damage ( ch, victim, 0, gsn_bash, DAM_BASH, FALSE, 1 );
        act ( "{hYou fall flat on your face!{x", ch, NULL, victim, TO_CHAR );
        act ( "{k$n falls flat on $s face.{x", ch, NULL, victim, TO_NOTVICT );
        act ( "{iYou evade $n's bash, causing $m to fall flat on $s face.{x",
              ch, NULL, victim, TO_VICT );
        check_improve ( ch, gsn_bash, FALSE, 1 );
        ch->position = POS_RESTING;
        WAIT_STATE ( ch, skill_table[gsn_bash].beats * 3 / 2 );
    }
}

CH_CMD ( do_dirt )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument ( argument, arg );

    if ( ( chance = get_skill ( ch, gsn_dirt ) ) == 0 ||
         ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_KICK_DIRT ) ) ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_dirt].skill_level[ch->class] ) )
    {
        send_to_char ( "{hYou get your feet dirty.{x\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        victim = ch->fighting;
        if ( victim == NULL )
        {
            send_to_char ( "But you aren't in combat!\n\r", ch );
            return;
        }
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( victim, AFF_BLIND ) )
    {
        act ( "{h$E's already been blinded.{x", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "Very funny.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim )
    {
        act ( "But $N is such a good friend!", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat ( ch, STAT_DEX );
    chance -= 2 * get_curr_stat ( victim, STAT_DEX );

    /* speed  */
    if ( IS_SET ( ch->off_flags, OFF_FAST ) || IS_AFFECTED ( ch, AFF_HASTE ) )
        chance += 10;
    if ( IS_SET ( victim->off_flags, OFF_FAST ) ||
         IS_AFFECTED ( victim, AFF_HASTE ) )
        chance -= 25;

    /* Making bash hit more - Rhaelar */
	chance += 15;

    /* level */
    chance += ( ch->level - victim->level ) * 2;

    /* sloppy hack to prevent false zeroes */
    if ( chance % 5 == 0 )
        chance += 1;

    /* terrain */

    switch ( ch->in_room->sector_type )
    {
        case ( SECT_INSIDE ):
            chance -= 20;
            break;
        case ( SECT_CITY ):
            chance -= 10;
            break;
        case ( SECT_FIELD ):
            chance += 5;
            break;
        case ( SECT_FOREST ):
            break;
        case ( SECT_HILLS ):
            break;
        case ( SECT_MOUNTAIN ):
            chance -= 10;
            break;
        case ( SECT_WATER_SWIM ):
            chance = 0;
            break;
        case ( SECT_WATER_NOSWIM ):
            chance = 0;
            break;
        case ( SECT_AIR ):
            chance = 0;
            break;
        case ( SECT_DESERT ):
            chance += 10;
            break;
    }

    if ( chance == 0 )
    {
        send_to_char ( "{hThere isn't any dirt to kick.{x\n\r", ch );
        return;
    }

    /* now the attack */
    if ( number_percent (  ) < chance )
    {
        AFFECT_DATA af;
        act ( "{k$n is blinded by the dirt in $s eyes!{x", victim, NULL, NULL,
              TO_ROOM );
        act ( "{i$n kicks dirt in your eyes!{x", ch, NULL, victim, TO_VICT );
        damage ( ch, victim, number_range ( 2, 5 ), gsn_dirt, DAM_NONE,
                 FALSE, 1 );
        send_to_char ( "{DYou can't see a thing!{x\n\r", victim );
        check_improve ( ch, gsn_dirt, TRUE, 2 );
        WAIT_STATE ( ch, skill_table[gsn_dirt].beats );

        af.where = TO_AFFECTS;
        af.type = gsn_dirt;
        af.level = ch->level;
        af.duration = 0;
        af.location = APPLY_HITROLL;
        af.modifier = -4;
        af.bitvector = AFF_BLIND;

        affect_to_char ( victim, &af );
    }
    else
    {
        damage ( ch, victim, 0, gsn_dirt, DAM_NONE, TRUE, 1 );
        check_improve ( ch, gsn_dirt, FALSE, 2 );
        WAIT_STATE ( ch, skill_table[gsn_dirt].beats );
    }
}

CH_CMD ( do_gouge )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument ( argument, arg );

    if ( ( chance = get_skill ( ch, gsn_gouge ) ) == 0 ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_gouge].skill_level[ch->class] ) )
    {
        send_to_char ( "Gouge?  What's that?{x\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        victim = ch->fighting;
        if ( victim == NULL )
        {
            send_to_char ( "But you aren't in combat!\n\r", ch );
            return;
        }
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( victim, AFF_BLIND ) )
    {
        act ( "{h$E's already been blinded.{x", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char ( "You try to gouge their eyes out, but you just poke their chest instead.\n\r", ch );
        return;
    }


    if ( victim == ch )
    {
        send_to_char ( "Very funny.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim )
    {
        act ( "But $N is such a good friend!", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat ( ch, STAT_DEX );
    chance -= 2 * get_curr_stat ( victim, STAT_DEX );

    /* speed  */
    if ( IS_SET ( ch->off_flags, OFF_FAST ) || IS_AFFECTED ( ch, AFF_HASTE ) )
        chance += 10;
    if ( IS_SET ( victim->off_flags, OFF_FAST ) ||
         IS_AFFECTED ( victim, AFF_HASTE ) )
        chance -= 25;

    /* level */
    chance += ( ch->level - victim->level ) * 2;

    /* sloppy hack to prevent false zeroes */
    if ( chance % 5 == 0 )
        chance += 1;

    /* now the attack */
    if ( number_percent (  ) < chance )
    {
        AFFECT_DATA af;
        act ( "{k$n is blinded by a poke in the eyes!{x", victim, NULL, NULL,
              TO_ROOM );
        act ( "{i$n gouges at your eyes!{x", ch, NULL, victim, TO_VICT );
        damage ( ch, victim, number_range ( 2, 5 ), gsn_gouge, DAM_NONE,
                 FALSE, 1 );
        send_to_char ( "{DYou see nothing but stars!{x\n\r", victim );
        check_improve ( ch, gsn_gouge, TRUE, 2 );
        WAIT_STATE ( ch, skill_table[gsn_gouge].beats );

        af.where = TO_AFFECTS;
        af.type = gsn_gouge;
        af.level = ch->level;
        af.duration = 0;
        af.location = APPLY_HITROLL;
        af.modifier = -4;
        af.bitvector = AFF_BLIND;

        affect_to_char ( victim, &af );
    }
    else
    {
        damage ( ch, victim, 0, gsn_gouge, DAM_NONE, TRUE, 1 );
        check_improve ( ch, gsn_gouge, FALSE, 2 );
        WAIT_STATE ( ch, skill_table[gsn_gouge].beats );
    }
}

CH_CMD ( do_trip )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument ( argument, arg );

    if ( ( chance = get_skill ( ch, gsn_trip ) ) == 0 ||
         ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_TRIP ) ) ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_trip].skill_level[ch->class] ) )
    {
        send_to_char ( "Tripping?  What's that?\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        victim = ch->fighting;
        if ( victim == NULL )
        {
            send_to_char ( "But you aren't fighting anyone!\n\r", ch );
            return;
        }
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( victim, AFF_FLYING ) )
    {
        act ( "{h$S feet aren't on the ground.{x", ch, NULL, victim,
              TO_CHAR );
        return;
    }

    if ( victim->position < POS_FIGHTING )
    {
        act ( "{h$N is already down.{c", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "{hYou fall flat on your face!{x\n\r", ch );
        WAIT_STATE ( ch, 2 * skill_table[gsn_trip].beats );
        act ( "{k$n trips over $s own feet!{x", ch, NULL, NULL, TO_ROOM );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim )
    {
        act ( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    /* modifiers */

    /* size */
    if ( ch->size < victim->size )
        chance += ( ch->size - victim->size ) * 10; /* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat ( ch, STAT_DEX );
    chance -= get_curr_stat ( victim, STAT_DEX ) * 3 / 2;

    /* speed */
    if ( IS_SET ( ch->off_flags, OFF_FAST ) || IS_AFFECTED ( ch, AFF_HASTE ) )
        chance += 10;
    if ( IS_SET ( victim->off_flags, OFF_FAST ) ||
         IS_AFFECTED ( victim, AFF_HASTE ) )
        chance -= 20;

    /* level */
    chance += ( ch->level - victim->level ) * 2;

    /* now the attack */
    if ( number_percent (  ) < chance )
    {
        act ( "{i$n trips you and you go down!{x", ch, NULL, victim,
              TO_VICT );
        act ( "{hYou trip $N and $N goes down!{x", ch, NULL, victim,
              TO_CHAR );
        act ( "{k$n trips $N, sending $M to the ground.{x", ch, NULL, victim,
              TO_NOTVICT );
        check_improve ( ch, gsn_trip, TRUE, 1 );

        DAZE_STATE ( victim, 2 * PULSE_VIOLENCE );
        WAIT_STATE ( ch, skill_table[gsn_trip].beats );
        victim->position = POS_RESTING;
        damage ( ch, victim, number_range ( 2 + ch->level * ch->size * 2, ch->size + ch->level + 2 + 2 * victim->size * 2),
                 gsn_trip, DAM_BASH, TRUE, 1 );
    }
    else
    {
        damage ( ch, victim, 0, gsn_trip, DAM_BASH, TRUE, 1 );
        WAIT_STATE ( ch, skill_table[gsn_trip].beats * 2 / 3 );
        check_improve ( ch, gsn_trip, FALSE, 1 );
    }
}

CH_CMD ( do_kill )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Kill whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char ( "{hYou hit yourself.  {z{COuch!{x\n\r", ch );
        multi_hit ( ch, ch, TYPE_UNDEFINED );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( !IS_NPC ( victim ) )
    {
        if ( !IS_SET ( victim->act, PLR_TWIT ) &&
             !IS_SET ( victim->in_room->room_flags, ROOM_ARENA ) )
        {
            send_to_char ( "You must MURDER a player.\n\r", ch );
            return;
        }
    }
    if ( victim->fighting != NULL && !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim )
    {
        act ( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char ( "You do the best you can!\n\r", ch );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    WAIT_STATE ( ch, 1 * PULSE_VIOLENCE );
    multi_hit ( ch, victim, TYPE_UNDEFINED );
    return;
}

CH_CMD ( do_mock )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Mock hit whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }
    if ( is_safe_mock ( ch, victim ) )
        return;

    if ( victim->fighting != NULL )
    {
        send_to_char ( "{gThis player is busy at the moment.{x\n\r", ch );
        return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char ( "{gYou've already got your hands full!{x\n\r", ch );
        return;
    }

    one_hit_mock ( ch, victim, TYPE_UNDEFINED, FALSE );

    return;
}

CH_CMD ( do_murde )
{
    send_to_char ( "If you want to {RMURDER{x, spell it out.\n\r", ch );
    return;
}

CH_CMD ( do_murder )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Murder whom?\n\r", ch );
        return;
    }

    if ( IS_NPC ( ch ) )
        return;

    if ( IS_AFFECTED ( ch, AFF_CHARM ) )
        return;

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "Suicide is a mortal sin.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim )
    {
        act ( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char ( "You do the best you can!\n\r", ch );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    WAIT_STATE ( ch, 1 * PULSE_VIOLENCE );
    if ( IS_NPC ( ch ) )
        sprintf ( buf, "Help! I am being attacked by %s!", ch->short_descr );
    else
        sprintf ( buf, "Help!  I am being attacked by %s!", ch->name );
    do_yell ( victim, buf );
    multi_hit ( ch, victim, TYPE_UNDEFINED );
    return;
}

CH_CMD ( do_assassinate )
{
    if ( !IS_NPC ( ch ) && ch->pcdata->assassinatetimer > 0 )
    {
    	send_to_char ( "You are not ready for another assassination quite yet.", ch );
	return;
    }
    
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument ( argument, arg );

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_assassinate].skill_level[ch->class] )
    {
        send_to_char
            ( "You don't know how to do that!\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Assassinate whom?\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
    {
        send_to_char ( "{hYou are already fighting!{x\n\r", ch );
        return;
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( !IS_NPC ( victim ) && !IS_NPC ( ch ) &&
         IS_SET ( victim->in_room->room_flags, ROOM_ARENA ) &&
         IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) )
    {
        printf_to_char ( ch, "That is not allowed in the arena" );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "You cannot assassinate yourself.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char
            ( "{hYou need to wield a primary weapon to backstab.{x\n\r", ch );
        return;
    }

    if ( victim->hit < victim->max_hit / 1.5 )
    {
        act ( "$N is hurt and suspicious ... you can't sneak up.", ch, NULL,
              victim, TO_CHAR );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    WAIT_STATE ( ch, skill_table[gsn_assassinate].beats );
    if ( number_percent (  ) < get_skill ( ch, gsn_assassinate ) ||
         ( get_skill ( ch, gsn_assassinate ) >= 2 && !IS_AWAKE ( victim ) ) )
    {
        if ( chance ( 15 ) &&
             ( !IS_SHIELDED ( victim, SHD_WARD ) && ( !IS_NPC ( victim ) ) ) )
        {
            act ( "Someone has masterfully ASSASSINATED $n!{x\n\r", victim,
                  NULL, NULL, TO_ROOM );
            act ( "$n thrusts their weapon into your vitals, ending your pitiful existence!\n\r", ch, NULL, victim, TO_VICT );
            printf_to_char ( ch,
                             "{xYou {RA{YSS{RA{YSSIN{RA{YTE{x your helpless victim{x!!\n\r" );
            group_gain ( ch, victim );
            raw_kill ( victim, ch );
/*            victim->hit = -victim->hit - 1; */
	    victim->hit = ( victim->max_hit / 5 );
            check_improve ( ch, gsn_assassinate, TRUE, 1 );
        }
        else
        {
            check_improve ( ch, gsn_assassinate, TRUE, 1 );
            multi_hit ( ch, victim, gsn_assassinate );
        }
    }
    else
    {
        check_improve ( ch, gsn_assassinate, FALSE, 1 );
        damage ( ch, victim, 0, gsn_assassinate, DAM_NONE, TRUE, 1 );
    }
    ch->pcdata->assassinatetimer = 10;
    return;
}

CH_CMD ( do_backstab )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument ( argument, arg );

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_backstab].skill_level[ch->class] )
    {
        send_to_char
            ( "You don't know how to do that!\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Backstab whom?\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
    {
        send_to_char ( "{hYou're facing the wrong end.{x\n\r", ch );
        return;
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "How can you sneak up on yourself?\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char
            ( "{hYou need to wield a primary weapon to backstab.{x\n\r", ch );
        return;
    }

    if ( victim->hit < victim->max_hit / 1.2 )
    {
        act ( "$N is hurt and suspicious ... you can't sneak up.", ch, NULL,
              victim, TO_CHAR );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    WAIT_STATE ( ch, skill_table[gsn_backstab].beats );
    if ( number_percent (  ) < get_skill ( ch, gsn_backstab ) ||
         ( get_skill ( ch, gsn_backstab ) >= 2 && !IS_AWAKE ( victim ) ) )
    {
        check_improve ( ch, gsn_backstab, TRUE, 1 );
        multi_hit ( ch, victim, gsn_backstab );
	if ( ch->race == race_lookup ( "demon" ) && chance ( 70 ) )
        multi_hit ( ch, victim, gsn_backstab );	
    }
    else
    {
        check_improve ( ch, gsn_backstab, FALSE, 1 );
        damage ( ch, victim, 0, gsn_backstab, DAM_NONE, TRUE, 1 );
/*	if ( ch->race == race_lookup ( "demon" ) )
        damage ( ch, victim, 0, gsn_backstab, DAM_NONE, TRUE ); */
    }

    return;
}


/* Create Wednesday November 8th 2000 by Belmakor for Kindred Spirits. 
  Any usage must retain this header */
/*
void do_garrote( CHAR_DATA *ch, char *argument ) 
{ 
   char arg[MAX_INPUT_LENGTH]; 
   CHAR_DATA *victim; 
   int dam; 
   int chance; 

   one_argument( argument, arg ); 

   if ( (chance = get_skill(ch,gsn_garrote)) == 0 
   ||   (!IS_NPC(ch) 
   &&    ch->level < skill_table[gsn_garrote].skill_level[ch->class])) 
   { 
    send_to_char("Garrote?  What's that?{x\n\r",ch); 
    return; 
   } 

   if (arg[0] == '\0') 
   { 
       send_to_char("Garrote whom?\n\r",ch); 
       return; 
   } 

   if (ch->fighting != NULL) 
   { 
    send_to_char("{hYou're facing the wrong end.{x\n\r",ch); 
    return; 
   } 

   else if ((victim = get_char_room(ch,arg)) == NULL) 
   { 
       send_to_char("They aren't here.\n\r",ch); 
       return; 
   } 

   if ( victim == ch ) 
   { 
    send_to_char( "How can you sneak up on yourself?\n\r", ch ); 
    return; 
   } 

   if ( is_safe( ch, victim ) ) 
     return; 

   if (IS_NPC(victim) && 
     victim->fighting != NULL && 
    !is_same_group(ch,victim->fighting) 
       && !IS_SET(victim->in_room->room_flags, ROOM_ARENA)) 
   { 
       send_to_char("Kill stealing is not permitted.\n\r",ch); 
       return; 
   } 
   if ( victim->hit < victim->max_hit / 4) 
   { 
    act( "$N is hurt and suspicious ... you can't sneak up.", 
        ch, NULL, victim, TO_CHAR ); 
    return; 
   } 

   if ( ( ch->fighting == NULL ) 
   && ( !IS_NPC( ch ) ) 
   && ( !IS_NPC( victim ) ) ) 
   { 
    ch->attacker = TRUE; 
    victim->attacker = FALSE; 
   } 

   
   chance += get_curr_stat(ch,STAT_DEX); 
   chance -= get_curr_stat(victim,STAT_DEX); 
 
   if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE)) 
    chance += 25; 
   if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE)) 
    chance -= 10; 
 
   chance += (ch->level - victim->
   if (chance % 5 == 0) 
    chance += 1; 
   if (number_percent() < chance) 
   { 
    AFFECT_DATA af; 
    act("{k$n is choked by a wire around the neck!{x",victim,NULL,NULL,TO_ROOM); 
    act("{i$n slips a wire around your neck!{x",ch,NULL,victim,TO_VICT); 
       dam = number_range ( (ch->level * 50 ), (450 ) ); 
       damage(ch,victim,dam,gsn_garrote,DAM_ENERGY,TRUE); 
    send_to_char("{7You choke and gag!{x\n\r",victim); 
    check_improve(ch,gsn_garrote,TRUE,2); 
    WAIT_STATE(ch,skill_table[gsn_garrote].beats); 

   

    af.where    = TO_AFFECTS; 
    af.type    = gsn_garrote; 
    af.level    = ch->level; 
    af.duration    = 3; 
    af.location    = APPLY_HITROLL; 
    af.modifier    = -6; 
    af.bitvector    = AFF_GARROTE; 

    affect_to_char(victim,&af); 
   } 
   else 
   { 
    damage(ch,victim,0,gsn_garrote,DAM_NONE,TRUE); 
    check_improve(ch,gsn_garrote,FALSE,2); 
    WAIT_STATE(ch,skill_table[gsn_garrote].beats); 
   } 
} 
*/

CH_CMD ( do_strangle )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int stranglechance;

    one_argument ( argument, arg );

    if ( ( stranglechance = get_skill ( ch, gsn_strangle ) ) == 0 )
    {
        send_to_char ( "You cant strangle.\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
    {
        send_to_char ( "{hYou are already fighting!{x\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        victim = ch->fighting;
        if ( victim == NULL )
        {
            send_to_char ( "You must specify a target!\n\r", ch );
            return;
        }
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( victim, AFF_STRANGLE ) )
    {
        act ( "$E's has already been strangled.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "Very funny.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
    {
	send_to_char ( "They are safe right now.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim )
    {
        act ( "But $N is such a good friend!", ch, NULL, victim, TO_CHAR );
        return;
    }

    /* level */
    stranglechance += ( ch->level - victim->level ) * 2;

    WAIT_STATE ( ch, skill_table[gsn_strangle].beats / 1 );    
    
    /* sloppy hack to prevent false zeroes */
    if ( stranglechance % 5 == 0 )
        stranglechance += 1;

    if ( stranglechance == 0 )
    {
        send_to_char ( "You failed horribly!.\n\r", ch );
        return;
    }

    if ( chance ( 80 ) && number_percent (  ) < stranglechance  )
    {
        AFFECT_DATA af;
/*        act("Someone grabs hold of $n's neck and throttles him, $n passes out.",victim,NULL,NULL,TO_ROOM);
        act ( "$n grabs hold of your neck and strangles you, starbursts erupt in front of your eyes as the world fades from view!", ch, NULL, victim, TO_VICT );
        printf_to_char ( ch,
                         "{xYou strangle your opponent!! \n\rThey pass out{x!!\n\r" );

*/
	act("You grab hold of $N's neck and put $M to sleep.",
	    ch,NULL,victim,TO_CHAR);
	act("$n grabs hold of your neck and puts you to sleep.",
	    ch,NULL,victim,TO_VICT);
	act("$n grabs hold of $N's neck and puts $M to sleep.",
	    ch,NULL,victim,TO_NOTVICT);
	if ( IS_SET ( ch->act2, PLR2_PKSAFE ) )
	REMOVE_BIT ( ch->act2, PLR2_PKSAFE );

	if ( !IS_NPC ( ch ) && !IS_NPC ( victim ) )
	{
	pkviolent ( ch, 6 );
	pkviolent ( victim, 4 );
	}
/* This damaged the victim too, but i couldnt get it the numbers right.

        damage(ch,victim,number_range(2,5),gsn_strangle,DAM_NONE,FALSE);
*/

/*        send_to_char ( "Someone grabs hold of your neck and strangles you!\n\r", victim ); */
        check_improve ( ch, gsn_strangle, TRUE, 2 );

        af.where = TO_AFFECTS;
        af.type = gsn_strangle;
        af.level = ch->level;
        af.duration = ch->level / 30;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_STRANGLE;
        affect_to_char ( victim, &af );
        victim->position = POS_SLEEPING;
    }
    else
    {
/* Another try

	damage(ch,victim,0,gsn_strangle,DAM_NONE,TRUE);
*/
	send_to_char ( "You failed.\n\r", ch );
//        check_improve ( ch, gsn_strangle, FALSE, 2 );
    }
}

CH_CMD ( do_circle )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( get_skill ( ch, gsn_circle ) == 0 ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_circle].skill_level[ch->class] ) )
    {
        send_to_char ( "Circle? What's that?\n\r", ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char ( "You need to wield a primary weapon to circle.\n\r",
                       ch );
        return;
    }
/*
    if ( victim->hit < victim->max_hit / 3 )
    {
        act ( "$N is hurt and suspicious ... you can't sneak around.", ch,
              NULL, victim, TO_CHAR );
        return;
    }
*/
    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char ( "You stumble blindly into a wall.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_circle].beats / 1 );
    if ( number_percent (  ) < get_skill ( ch, gsn_circle ) ||
         ( get_skill ( ch, gsn_circle ) >= 2 && !IS_AWAKE ( victim ) ) )
    {
        check_improve ( ch, gsn_circle, TRUE, 1 );
        act ( "{i$n circles around behind you.{x", ch, NULL, victim,
              TO_VICT );
        act ( "{hYou circle around $N.{x", ch, NULL, victim, TO_CHAR );
        act ( "{k$n circles around behind $N.{x", ch, NULL, victim,
              TO_NOTVICT );
        multi_hit ( ch, victim, gsn_circle );
    }
    else
    {
        check_improve ( ch, gsn_circle, FALSE, 1 );
        act ( "{i$n tries to circle around you.{x", ch, NULL, victim,
              TO_VICT );
        act ( "{h$N circles with you.{x", ch, NULL, victim, TO_CHAR );
        act ( "{k$n tries to circle around $N.{x", ch, NULL, victim,
              TO_NOTVICT );
        damage ( ch, victim, 0, gsn_circle, DAM_NONE, TRUE, 1 );
    }

    return;
}

bool check_counter ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt )
{
    int chance;
    int dam_type;
    OBJ_DATA *wield;

    if ( ch->stunned )
    {
        return FALSE;
    }

    if ( ( get_eq_char ( victim, WEAR_WIELD ) == NULL ) ||
         ( !IS_AWAKE ( victim ) ) || ( !can_see ( victim, ch ) ) ||
         ( get_skill ( victim, gsn_counter ) < 1 ) )
        return FALSE;

    wield = get_eq_char ( victim, WEAR_WIELD );

    chance = get_skill ( victim, gsn_counter ) / 6;
    chance += ( victim->level - ch->level ) / 2;
    chance +=
        2 * ( get_curr_stat ( victim, STAT_DEX ) -
              get_curr_stat ( ch, STAT_DEX ) );
    chance +=
        get_weapon_skill ( victim,
                           get_weapon_sn ( victim ) ) - get_weapon_skill ( ch,
                                                                           get_weapon_sn
                                                                           ( ch ) );
    chance +=
        ( get_curr_stat ( victim, STAT_STR ) -
          get_curr_stat ( ch, STAT_STR ) );

    if ( number_percent (  ) >= chance )
        return FALSE;

    dt = gsn_counter;

    if ( dt == TYPE_UNDEFINED )
    {
        dt = TYPE_HIT;
        if ( wield != NULL && wield->item_type == ITEM_WEAPON )
            dt += wield->value[3];
        else
            dt += ch->dam_type;
    }

    if ( dt < TYPE_HIT )
        if ( wield != NULL )
            dam_type = attack_table[wield->value[3]].damage;
        else
            dam_type = attack_table[ch->dam_type].damage;
    else
        dam_type = attack_table[dt - TYPE_HIT].damage;

    if ( dam_type == -1 )
        dam_type = DAM_BASH;

    act ( "You reverse $n's attack and counter with your own!", ch, NULL,
          victim, TO_VICT );
    act ( "$N reverses your attack!", ch, NULL, victim, TO_CHAR );

    damage ( victim, ch, dam / 2, gsn_counter, dam_type, TRUE, 1 );    /* DAM MSG NUMBER!! */

    check_improve ( victim, gsn_counter, TRUE, 6 );

    return TRUE;
}

bool check_critical ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    OBJ_DATA *obj;

    obj = get_eq_char ( ch, WEAR_WIELD );

    if ( ( get_eq_char ( ch, WEAR_WIELD ) == NULL ) ||
         ( get_skill ( ch, gsn_critical ) < 1 ) ||
         ( get_weapon_skill ( ch, get_weapon_sn ( ch ) ) != 100 ) ||
         ( number_range ( 0, 100 ) > get_skill ( ch, gsn_critical ) ) )
        return FALSE;

    if ( number_range ( 0, 100 ) > 25 )
        return FALSE;

    act ( "$p CRITICALLY STRIKES $n!", victim, obj, NULL, TO_NOTVICT );
    act ( "{RCRITICAL {YSTRIKE!{x", ch, NULL, victim, TO_VICT );
    check_improve ( ch, gsn_critical, TRUE, 6 );
    return TRUE;
}

/*
bool check_critical(CHAR_DATA *ch, CHAR_DATA *victim, bool secondary)
{
	OBJ_DATA *obj;
	obj = get_eq_char( ch, WEAR_WIELD );

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL && get_eq_char( ch, WEAR_SECONDARY ) == NULL)
    {
    	return FALSE;
    }

    if (!secondary)
    {
    	obj = get_eq_char( ch, WEAR_WIELD );
    }
    else
    {
    	obj = get_eq_char( ch, WEAR_SECONDARY );
    }

	if (
    	( !IS_AWAKE(victim) ) ||
    	( !can_see(victim,ch) ) ||
    	( victim->stunned) ||
    	( get_skill(ch,gsn_critical)  <  1 ) ||
    	( get_weapon_skill(ch,get_weapon_sn(ch))  <= 85 )||
    	( number_range(0,100) >= get_skill(ch,gsn_critical)||
    	( number_range(0,100) >= 10 ))
    	)
	{
		return FALSE;
	}

	act("$p CRITICALLY STRIKES $n!",victim,obj,NULL,TO_NOTVICT);
	act("CRITICAL STRIKE!",ch,NULL,victim,TO_VICT);
	check_improve(ch,gsn_critical,TRUE,6);
	return TRUE;
}
*/
CH_CMD ( do_feed )
{
    CHAR_DATA *victim;
    int dam;

    if ( get_skill ( ch, gsn_feed ) == 0 ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_feed].skill_level[ch->class] ) )
    {
        send_to_char ( "Feed? What's that?\n\r", ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }
/*
    if ( victim->hit < victim->max_hit / 6 )
    {
        act ( "$N is hurt and suspicious ... you can't get close enough.", ch,
              NULL, victim, TO_CHAR );
        return;
    }
*/
    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_feed].beats );
    if ( number_percent (  ) < get_skill ( ch, gsn_feed ) / 1 ||
         ( get_skill ( ch, gsn_feed ) >= 2 && !IS_AWAKE ( victim ) ) )
    {
        check_improve ( ch, gsn_feed, TRUE, 1 );
        act ( "{i$n bites you.{x", ch, NULL, victim, TO_VICT );
        act ( "{hYou bite $N.{x", ch, NULL, victim, TO_CHAR );
        act ( "{k$n bites $N.{x", ch, NULL, victim, TO_NOTVICT );
        dam =
            number_range ( ( ( ( ch->level / 1 ) +
                               ( victim->level / 1 ) ) / 1 ),
                           ( ( ( ch->level / 1 ) +
                               ( victim->level / 1 ) ) / 1 ) * 30 );
        damage ( ch, victim, dam, gsn_feed, DAM_NONE, TRUE, 0 );

    if ( chance ( 45 ) )
    {
        act("You {Ystun{x $N with your feed.",
           ch,NULL,victim,TO_CHAR);
        act("You are {Ystunned{x by $n's feed.",
            ch,NULL,victim,TO_VICT);
        act("$N is {Ystunned{x.",
            ch,NULL,victim,TO_NOTVICT);
        victim->stunned = 1;
    }

    }
    else
    {
        check_improve ( ch, gsn_feed, FALSE, 1 );
        act ( "{i$n tries to bite you, but hits only air.{x", ch, NULL,
              victim, TO_VICT );
        act ( "{hYou chomp a mouthfull of air.{x", ch, NULL, victim,
              TO_CHAR );
        act ( "{k$n tries to bite $N.{x", ch, NULL, victim, TO_NOTVICT );
        damage ( ch, victim, 0, gsn_feed, DAM_NEGATIVE, TRUE, 0 );
    }

    return;
}

CH_CMD ( do_gorge )
{
    CHAR_DATA *victim;
    int dam;

    if ( get_skill ( ch, gsn_gorge ) == 0 ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_gorge].skill_level[ch->class] ) )
    {
        send_to_char ( "Gorge? What's that?\n\r", ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }
/*
    if ( victim->hit < victim->max_hit / 6 )
    {
        act ( "$N is hurt and suspicious ... you can't get close enough.", ch,
              NULL, victim, TO_CHAR );
        return;
    }
*/
    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_gorge].beats );
    if ( number_percent (  ) < get_skill ( ch, gsn_gorge ) / 1 ||
         ( get_skill ( ch, gsn_gorge ) >= 2 && !IS_AWAKE ( victim ) ) )
    {
        check_improve ( ch, gsn_gorge, TRUE, 1 );
        act ( "{i$n bites you.{x", ch, NULL, victim, TO_VICT );
        act ( "{hYou bite $N.{x", ch, NULL, victim, TO_CHAR );
        act ( "{k$n bites $N.{x", ch, NULL, victim, TO_NOTVICT );
        dam =
            number_range ( ( ( ( ch->level / 1 ) +
                               ( victim->level / 1 ) ) / 1 ) * 25,
                           ( ( ( ch->level / 1 ) +
                               ( victim->level / 1 ) ) / 1 ) * 80 );
			      
        damage ( ch, victim, dam, gsn_gorge, DAM_NONE, TRUE, 0 );

    if ( chance ( 35 ) )
    {
        act("You {Ystun{x $N with your gorge.",
           ch,NULL,victim,TO_CHAR);
        act("You are {Ystunned{x by $n's gorge.",
            ch,NULL,victim,TO_VICT);
        act("$N is {Ystunned{x.",
            ch,NULL,victim,TO_NOTVICT);
        victim->stunned = 3;
    }

    }
    else
    {
        check_improve ( ch, gsn_gorge, FALSE, 1 );
        act ( "{i$n tries to bite you, but hits only air.{x", ch, NULL,
              victim, TO_VICT );
        act ( "{hYou chomp a mouthfull of air.{x", ch, NULL, victim,
              TO_CHAR );
        act ( "{k$n tries to bite $N.{x", ch, NULL, victim, TO_NOTVICT );
        damage ( ch, victim, 0, gsn_gorge, DAM_NEGATIVE, TRUE, 0 );
    }

    return;
}


CH_CMD ( do_flee )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
        if ( ch->position == POS_FIGHTING )
            ch->position = POS_STANDING;
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
	send_to_char ( "You are stunned. You cannot flee!\n\r", ch );
	return;
    }

 if ( number_percent ( ) < get_skill ( ch, gsn_flee ) && chance ( 70 ) )
 {
    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
        EXIT_DATA *pexit;
        int door;

        door = number_door (  );
        if ( ( pexit = was_in->exit[door] ) == 0 || pexit->u1.to_room == NULL
             || IS_SET ( pexit->exit_info, EX_CLOSED ) ||
             number_range ( 0, ch->daze ) != 0 || ( IS_NPC ( ch ) &&
                                                    IS_SET ( pexit->u1.
                                                             to_room->
                                                             room_flags,
                                                             ROOM_NO_MOB ) ) )
            continue;

        move_char ( ch, door, FALSE, FALSE );
        if ( ( now_in = ch->in_room ) == was_in )
            continue;

        ch->in_room = was_in;
        act ( "$n has {Yfled{x!", ch, NULL, NULL, TO_ROOM );
        if ( !IS_NPC ( ch ) )
        {
            send_to_char ( "{BYou {Yflee{B from combat!{x\n\r", ch );
            if ( ( ( ch->class == 2 ) ||
                   ( ch->class == ( MAX_CLASS / 2 ) + 1 ) ) &&
                 ( number_percent (  ) < 3 * ( ch->level / 2 ) ) )
            {
                if ( IS_NPC ( victim ) || ch->attacker == FALSE )
                {
                    send_to_char ( "You {Ysnuck away{x safely.\n\r", ch );
                }
                else
                {
                    send_to_char
                        ( "You feel something singe your butt on the way out.\n\r",
                          ch );
                    act ( "$n is nearly {Yzapped{x in the butt by a lightning bolt from above!", ch, NULL, NULL, TO_ROOM );
                    ch->hit -= ( ch->hit / 8 );
                }
            }
            else
            {
                if ( !IS_NPC ( victim ) && ch->attacker == TRUE )
                {
                    send_to_char
                        ( "The {RWrath of Rhaelar {YZAPS{x your butt on the way out!\n\r",
                          ch );
                    act ( "$n is {Yzapped{x in the butt by a lightning bolt from above!", ch, NULL, NULL, TO_ROOM );
                    ch->hit -= ( ch->hit / 4 );
                }
                send_to_char ( "You lost 100 exp.\n\r", ch );
                gain_exp ( ch, -100 );
            }
        }
        check_improve ( ch, gsn_flee, TRUE, 1 );
        ch->in_room = now_in;
        stop_fighting ( ch, TRUE );
	WAIT_STATE(ch,1 * PULSE_VIOLENCE);
        return;
    }
   }
    send_to_char ( "{z{CPANIC!{x{B You couldn't escape!{x\n\r", ch );
/*    act ( "{R$n{x tries to {Yflee{x, but fails!{x", ch, NULL, victim, TO_NOTVICT ); */
    return;
}

CH_CMD ( do_rescue )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Rescue whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "What about {Yfleeing{x instead?\n\r", ch );
        return;
    }

    if ( !IS_NPC ( ch ) && IS_NPC ( victim ) &&
         !is_same_group ( ch, victim ) )
    {
        send_to_char ( "Doesn't need your help!\n\r", ch );
        return;
    }

    if ( ch->fighting == victim )
    {
        send_to_char ( "Too late.\n\r", ch );
        return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
        send_to_char ( "That person is not fighting right now.\n\r", ch );
        return;
    }

    if ( IS_NPC ( fch ) && !is_same_group ( ch, victim ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_rescue].beats );
    if ( number_percent (  ) > get_skill ( ch, gsn_rescue ) )
    {
        send_to_char ( "You fail the rescue.\n\r", ch );
        check_improve ( ch, gsn_rescue, FALSE, 1 );
        return;
    }

    act ( "{yYou rescue $N!{x", ch, NULL, victim, TO_CHAR );
    act ( "{y$n rescues you!{x", ch, NULL, victim, TO_VICT );
    act ( "{y$n rescues $N!{x", ch, NULL, victim, TO_NOTVICT );
    check_improve ( ch, gsn_rescue, TRUE, 1 );

    stop_fighting ( fch, FALSE );
    stop_fighting ( victim, FALSE );

    set_fighting ( ch, fch );
    set_fighting ( fch, ch );
    return;
}

CH_CMD ( do_kick )
{
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_kick].skill_level[ch->class] )
    {
        send_to_char ( "You better leave the martial arts to fighters.\n\r",
                       ch );
        return;
    }

    if ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_KICK ) )
        return;

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_kick].beats );
    
    if ( get_skill ( ch, gsn_kick ) < number_percent (  ) )
    {
        printf_to_char ( ch,
                         "You try to kick your opponent but they are to fast!\n\r" );
        check_improve ( ch, gsn_kick, FALSE, 1 );
        return;
    }

    dam = 35 + ch->level;
    damage ( ch, victim, number_range ( dam, ( ch->level * 4 ) ), gsn_kick,
             DAM_BASH, TRUE, 1 );
    check_improve ( ch, gsn_kick, TRUE, 1 );

    if ( chance ( 55 ) )
        dam = dam + ch->level * 3 / 2;
    damage ( ch, victim, number_range ( dam, ( ch->level * 3 ) ), gsn_kick,
             DAM_BASH, TRUE, 1 );

    return;
}

CH_CMD ( do_disarm )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance, hth, ch_weapon, vict_weapon, ch_vict_weapon;

    hth = 0;

    if ( ( chance = get_skill ( ch, gsn_disarm ) ) == 0 )
    {
        send_to_char ( "You don't know how to disarm opponents.\n\r", ch );
        return;
    }

    if ( get_eq_char ( ch, WEAR_WIELD ) == NULL &&
         ( ( hth = get_skill ( ch, gsn_hand_to_hand ) ) == 0 ||
           ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_DISARM ) ) ) )
    {
        send_to_char ( "You must wield a weapon to disarm.\n\r", ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( victim, WEAR_WIELD ) ) == NULL )
    {
        send_to_char ( "{hYour opponent is not wielding a weapon.{x\n\r",
                       ch );
        return;
    }

    /* find weapon skills */
    ch_weapon = get_weapon_skill ( ch, get_weapon_sn ( ch ) );
    vict_weapon = get_weapon_skill ( victim, get_weapon_sn ( victim ) );
    ch_vict_weapon = get_weapon_skill ( ch, get_weapon_sn ( victim ) );

    /* modifiers */

    /* skill */
    if ( get_eq_char ( ch, WEAR_WIELD ) == NULL )
        chance = chance * hth / 150;
    else
        chance = chance * ch_weapon / 100;

    chance += ( ch_vict_weapon / 2 - vict_weapon ) / 2;

    /* dex vs. strength */
    chance += get_curr_stat ( ch, STAT_DEX );
    chance -= 2 * get_curr_stat ( victim, STAT_STR );

    /* level */
    chance += ( ch->level - victim->level ) * 2;

    chance /= 2;

    /* and now the attack */
    if ( number_percent (  ) < chance )
    {
        if ( ( ( chance = get_skill ( victim, gsn_grip ) ) == 0 ) ||
             ( !IS_NPC ( victim ) &&
               victim->level <
               skill_table[gsn_grip].skill_level[victim->class] ) )
        {
            WAIT_STATE ( ch, skill_table[gsn_disarm].beats );
            disarm ( ch, victim );
            check_improve ( ch, gsn_disarm, TRUE, 1 );
            return;
        }
        if ( number_percent (  ) > ( chance / 5 ) * 4 )
        {
            WAIT_STATE ( ch, skill_table[gsn_disarm].beats );
            disarm ( ch, victim );
            check_improve ( ch, gsn_disarm, TRUE, 1 );
            check_improve ( victim, gsn_grip, FALSE, 1 );
            return;
        }
        check_improve ( victim, gsn_grip, TRUE, 1 );
    }
    WAIT_STATE ( ch, skill_table[gsn_disarm].beats );
    act ( "{hYou fail to disarm $N.{x", ch, NULL, victim, TO_CHAR );
    act ( "{i$n tries to disarm you, but fails.{x", ch, NULL, victim,
          TO_VICT );
    act ( "{k$n tries to disarm $N, but fails.{x", ch, NULL, victim,
          TO_NOTVICT );
    check_improve ( ch, gsn_disarm, FALSE, 1 );
    return;
}

CH_CMD ( do_surrender )
{
    CHAR_DATA *mob;
    if ( ( mob = ch->fighting ) == NULL )
    {
        send_to_char ( "But you're not fighting!\n\r", ch );
        return;
    }
    act ( "You surrender to $N!", ch, NULL, mob, TO_CHAR );
    act ( "$n surrenders to you!", ch, NULL, mob, TO_VICT );
    act ( "$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT );
    stop_fighting ( ch, TRUE );

    if ( !IS_NPC ( ch ) && IS_NPC ( mob ) &&
         ( !HAS_TRIGGER ( mob, TRIG_SURR ) ||
           !mp_percent_trigger ( mob, ch, NULL, NULL, TRIG_SURR ) ) )
    {
        act ( "$N seems to ignore your cowardly act!", ch, NULL, mob,
              TO_CHAR );
        multi_hit ( mob, ch, TYPE_UNDEFINED );
    }
}

CH_CMD ( do_sla )
{
    send_to_char ( "If you want to {RSLAY{x, spell it out.\n\r", ch );
    return;
}
/*
CH_CMD ( do_slay )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Slay whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( ch == victim )
    {
        send_to_char ( "Suicide is a mortal sin.\n\r", ch );
        return;
    }

    if ( !IS_NPC ( victim ) && victim->level >= get_trust ( ch ) )
    {
        send_to_char ( "{hYou failed.{c\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) || get_trust ( ch ) >= CREATOR )
    {
        act ( "{hYou slay $M in cold blood!{x", ch, NULL, victim, TO_CHAR );
        act ( "{i$n slays you in cold blood!{x", ch, NULL, victim, TO_VICT );
        act ( "{k$n slays $N in cold blood!{x", ch, NULL, victim,
              TO_NOTVICT );
        raw_kill ( victim, ch );
    }
    return;
}
*/

CH_CMD ( do_rub )
{
    int chance;

    if ( ( chance = get_skill ( ch, gsn_rub ) ) == 0 ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_rub].skill_level[ch->class] ) )
    {
        send_to_char ( "You just nearly gouged your own eyes!\n\r", ch );
        return;
    }
    if ( ( !IS_AFFECTED ( ch, gsn_dirt ) ) &&
	 ( !IS_AFFECTED ( ch, gsn_gouge ) ) &&
         ( !IS_AFFECTED ( ch, skill_lookup ( "fire_breath" ) ) ) )
    {
        send_to_char ( "There is nothing in your eyes!\n\r", ch );
        return;
    }
    if ( chance % 5 == 0 )
        chance += 1;
    if ( number_percent (  ) < chance )
    {
        send_to_char
            ( "You rub your eyes, and your vision clears up again.\n\r", ch );
        if ( is_affected ( ch, gsn_dirt ) )
        {
            affect_strip ( ch, gsn_dirt );
            check_improve ( ch, gsn_rub, TRUE, 5 );
            WAIT_STATE ( ch, skill_table[gsn_rub].beats );
        }
        if ( is_affected ( ch, gsn_gouge ) )
        {
            affect_strip ( ch, gsn_gouge );
            check_improve ( ch, gsn_rub, TRUE, 5 );
            WAIT_STATE ( ch, skill_table[gsn_rub].beats );
        }
        if ( is_affected ( ch, skill_lookup ( "fire breath" ) ) )
	{
            affect_strip ( ch, skill_lookup ( "fire breath" ) );
            check_improve ( ch, gsn_rub, TRUE, 5 );
            WAIT_STATE ( ch, skill_table[gsn_rub].beats );
	}
    }
    else
    {
        send_to_char ( "Your attempt to gain vision again failed!\n\r",
                       ch );
        check_improve ( ch, gsn_rub, FALSE, 6 );
        WAIT_STATE ( ch, skill_table[gsn_rub].beats / 2 );
    }
}

/* Coded by Shinji - mudnet.net:7000 */
CH_CMD ( do_sharpen )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int chance;
    AFFECT_DATA af;

    one_argument ( argument, arg );

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_sharpen].skill_level[ch->class] )
    {
        send_to_char
            ( "You had best leave that skill to master warriors.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Sharpen what?\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char ( "Quit fighting! Then try.\n\r", ch );
        return;
    }

    if ( !( obj = get_obj_carry ( ch, arg ) ) )
    {
        send_to_char ( "You do not have that weapon.\n\r", ch );
        return;
    }

    if ( obj->item_type != ITEM_WEAPON )
    {
        send_to_char ( "That item is not a weapon.\n\r", ch );
        return;
    }

    if ( IS_WEAPON_STAT ( obj, WEAPON_SHARP ) )
    {
        send_to_char ( "This weapon is already as sharp as it can get!\n\r",
                       ch );
        return;
    }
    if ( IS_WEAPON_STAT ( obj, WEAPON_VORPAL ) )
    {
        send_to_char ( "This weapon is already as sharp as it can get!\n\r",
                       ch );
        return;
    }

    chance = number_range ( 1, 15 );
/*
    if ( chance <= 10 )
    {
        extract_obj ( obj );
        send_to_char
            ( "You failed miserably and dulled the weapon beyond repair!\n\r",
              ch );
        return;
    }
*/
    if ( chance >= 45 )
    {
        send_to_char
            ( "You slice your finger while trying to sharpen your weapon.\n\r",
              ch );
        damage ( ch, ch, ch->hit / 10, gsn_sharpen, WEAR_NONE, DAM_ENERGY, 1 );
        return;
    }

    if ( chance <= 46 )
    {
        send_to_char
            ( "You sharpen your weapon.\n\r",
              ch );
/*        WAIT_STATE ( ch, PULSE_VIOLENCE * 5 );
        send_to_char ( "Your weapon is now complete.\n\r", ch );
        obj->value[1] += 1;
        obj->value[2] += 1;
           if (chance(10))
           {
           printf_to_char(ch,"You must have been assisted by the gods as you crated a {YVORPAL{x weapon!");
           af.bitvector = WEAPON_VORPAL;
           return;
           }
           else  
         */

        af.where = TO_WEAPON;
        af.type = gsn_sharpen;
        af.level = 0;
        af.duration = -1;
        af.location = 0;
        af.modifier = 0;
        af.bitvector = WEAPON_SHARP;
        affect_to_obj ( obj, &af );
        return;
    }
}

CH_CMD ( do_rampage )
{
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_rampage].skill_level[ch->class] )
    {
        send_to_char
            ( "You better leave that skill to master warriors.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char
            ( "You need to wield a primary weapon to go on a rampage.\n\r",
              ch );
        return;
    }
    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char
            ( "How do you expect to lose your temper when your not in battle.\n\r",
              ch );
        return;
    }
    if ( !can_see ( ch, victim ) )
    {
        send_to_char ( "You stumble blindly into a wall.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_rampage].beats / 2 );

    if ( get_skill ( ch, gsn_rampage ) < number_percent (  ) )
    {
        printf_to_char ( ch,
                         "You prepare for a rampage but your opponent is too quick!\n\r" );
        check_improve ( ch, gsn_rampage, FALSE, 1 );
        return;
    }

    dam = ch->level * 30;

    damage ( ch, victim, number_range ( dam, ch->level * 45 ), gsn_rampage,
             DAM_BASH, TRUE, 1 );
    check_improve ( ch, gsn_rampage, TRUE, 3 );

    dam = ch->level * 30;

    damage ( ch, victim, number_range ( dam, ch->level * 45 ), gsn_rampage,
             DAM_BASH, TRUE, 1 );

    dam = ch->level * 30;

    damage ( ch, victim, number_range ( dam, ch->level * 35 ), gsn_rampage,
             DAM_BASH, TRUE, 1 );

    if ( chance ( 50 ) )
        return;

    dam = ch->level * 35;

    damage ( ch, victim, number_range ( dam, ch->level * 28 ), gsn_rampage,
             DAM_BASH, TRUE, 1 );
    check_improve ( ch, gsn_rampage, TRUE, 2 );

    if ( chance ( 30 ) )
        return;

    dam = ch->level * 25;

    damage ( ch, victim, number_range ( dam, ch->level * 22 ), gsn_rampage,
             DAM_BASH, TRUE, 1 );
/*
    if ( chance ( 35 ) )
    {
        printf_to_char ( ch,
                         "{YYour opponent is stunned by your {Rf{ru{Rr{ri{Ro{ru{Rs{Y{x rampage!{x\n\r" ); */
    if ( chance ( 35 ) )
    {
     if ( ch->level > skill_table[gsn_stun].skill_level[ch->class] )
     {
        act("You {Ystun{x $N with your rampage.",
            ch,NULL,victim,TO_CHAR);
        act("You are {Ystunned{x by $n's rampage.",
            ch,NULL,victim,TO_VICT);
        act("$N is {Ystunned{x.",
            ch,NULL,victim,TO_NOTVICT);
        victim->stunned = 2;
        check_improve ( ch, gsn_stun, TRUE, 1 ); 
     }
    }

    if ( chance ( 35 ) )
        return;

    dam = ch->level * 20;

    damage ( ch, victim, number_range ( dam, ch->level * 16 ), gsn_rampage,
             DAM_BASH, TRUE, 1 );

    if ( chance ( 25 ) )
        return;

    dam = ch->level * 15;

    damage ( ch, victim, number_range ( dam, ch->level * 16 ), gsn_rampage,
             DAM_BASH, TRUE, 1 );

    return;
}
/*
CH_CMD ( do_ambush )
{
    AFFECT_DATA af;
    CHAR_DATA *victim;
    int skill;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument ( argument, arg );

    if ( ( skill = get_skill ( ch, gsn_ambush ) ) == 0 )
    {
        send_to_char ( "You don't know how to ambush.\n\r", ch );
        return;
    }
    if ( ch->fighting != NULL )
    {
        send_to_char ( "{YThey would notice!{x\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        if ( is_affected ( ch, gsn_ambush ) )
        {
            send_to_char ( "You are already lying in ambush.\n\r", ch );
            return;
        }
        WAIT_STATE ( ch, 12 );
        if ( number_percent (  ) < skill )
        {
            af.where = TO_AFFECTS;
            af.location = APPLY_NONE;
            af.type = gsn_ambush;
            af.modifier = 0;
            af.bitvector = 0;
            af.duration = ch->level / 5;
            af.level = ch->level;
            affect_to_char ( ch, &af );
            check_improve ( ch, gsn_ambush, TRUE, 2 );
            send_to_char ( "You hide and prepare an ambush.\n\r", ch );
            return;
        }
        send_to_char
            ( "You look around but can not find a suitable hiding place.\n\r",
              ch );
        check_improve ( ch, gsn_ambush, FALSE, 2 );
        return;
    }
    if ( !is_affected ( ch, gsn_ambush ) )
    {
        send_to_char ( "You haven't even prepared an ambush yet.\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char ( "Ambush yourself? You'd probably notice.\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( get_eq_char ( ch, WEAR_WIELD ) == NULL )
    {
        send_to_char
            ( "{hYou need to wield a primary weapon to ambush.{x\n\r", ch );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    WAIT_STATE ( ch, 24 );
    if ( number_percent (  ) < skill )
    {
        affect_strip ( ch, gsn_ambush );
        check_improve ( ch, gsn_ambush, TRUE, 1 );
        multi_hit ( ch, victim, gsn_ambush );
    }
    else
    {
        affect_strip ( ch, gsn_ambush );
        check_improve ( ch, gsn_ambush, FALSE, 1 );
        damage ( ch, victim, 0, gsn_ambush, DAM_NONE, TRUE );
    }
    return;
}
*/

CH_CMD ( do_ambush )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_ambush].skill_level[ch->class] )
    {
        send_to_char
            ( "You don't know how to do that!\n\r", ch );
        return;
    }
    
    one_argument ( argument, arg );
    
    if ( arg[0] == '\0' )
    {
        send_to_char ( "Ambush whom?\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
    {
        send_to_char ( "{hYou're facing the wrong end.{x\n\r", ch );
        return;
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char ( "How can you sneak up on yourself?\n\r", ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char
            ( "{hYou need to wield a primary weapon to ambush.{x\n\r", ch );
        return;
    }

    if ( victim->hit < victim->max_hit / 1.5 )
    {
        act ( "$N is hurt and suspicious ... you can't sneak up.", ch, NULL,
              victim, TO_CHAR );
        return;
    }

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    WAIT_STATE ( ch, skill_table[gsn_ambush].beats );
    if ( number_percent (  ) < get_skill ( ch, gsn_ambush ) ||
         ( get_skill ( ch, gsn_ambush ) >= 2 && !IS_AWAKE ( victim ) ) )
    {
        check_improve ( ch, gsn_ambush, TRUE, 1 );
        multi_hit ( ch, victim, gsn_ambush );
    }
    else
    {
        check_improve ( ch, gsn_ambush, FALSE, 1 );
        damage ( ch, victim, 0, gsn_ambush, DAM_NONE, TRUE, 1 );
    }

    return;
}

/* STAKE skill by The Mage */
CH_CMD ( do_stake )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument ( argument, arg );

    if ( ( chance = get_skill ( ch, gsn_stake ) ) &&
         ch->level < skill_table[gsn_stake].skill_level[ch->class] )
    {
        send_to_char ( "Stake? What's that?\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        victim = ch->fighting;
        if ( victim == NULL )
        {
            send_to_char ( "Stake What undead??\n\r", ch );
            return;
        }
    }

    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( !IS_NPC ( victim ) &&
         ( !str_cmp ( class_table[victim->class].name, "Vampire" ) ||
           ( !str_cmp ( class_table[victim->class].name, "Cainite" ) ||
             ( !str_cmp ( class_table[victim->class].name, "Revenant" ) ||
               ( !str_cmp
                 ( class_table[victim->class].name, "Lich" ) ) ) ) ) )
    {
        send_to_char ( "You cannot stake a non-vampire player.\n\r", ch );
        return;
    }
    if ( IS_NPC ( victim ) &&
         ( !is_name ( "vampire", victim->name ) &&
           !is_name ( "undead", victim->name ) &&
           !is_name ( "zombie", victim->name ) &&
           !is_name ( "corpse", victim->name ) ) )
    {

        send_to_char ( "You cannot stake this mob.\n\r", ch );
        return;
    }
    /*    if (IS_NPC(victim) && (!IS_SET(victim->act,ACT_UNDEAD)))
       {
       send_to_char("You cannot stake the living.\n\r",ch);
       return;
       }
     */
    if ( victim == ch )
    {
        send_to_char ( "You aren't undead.. you cannot stake yourself.\n\r",
                       ch );
        return;
    }

    if ( is_safe ( ch, victim ) )
        return;

    if ( IS_NPC ( victim ) && victim->fighting != NULL &&
         !is_same_group ( ch, victim->fighting ) )
    {
        send_to_char ( "Kill stealing is not permitted.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim )
    {
        act ( "But $N is your friend!", ch, NULL, victim, TO_CHAR );
        return;
    }

    /* modifiers */

    if ( ch->size < victim->size )
        chance += ( ch->size - victim->size ) * 15;
    else
        chance += ( ch->size - victim->size ) * 10;

    /* stats */
    chance -= GET_AC ( victim, AC_PIERCE ) / 25;
    /* speed */
    if ( IS_SET ( ch->off_flags, OFF_FAST ) || IS_AFFECTED ( ch, AFF_HASTE ) )
        chance += 10;
    if ( IS_SET ( victim->off_flags, OFF_FAST ) ||
         IS_AFFECTED ( victim, AFF_HASTE ) )
        chance -= 10;

    /* level */
    chance += ( ch->level - victim->level );

    /* now the attack */
    if ( number_percent (  ) < chance )
    {

        act ( "$n has stuck a stake in your heart!", ch, NULL, victim,
              TO_VICT );
        act ( "You slam a stake into $N!", ch, NULL, victim, TO_CHAR );
        act ( "$n shoves a stake into $N .", ch, NULL, victim, TO_NOTVICT );
        check_improve ( ch, gsn_stake, TRUE, 1 );

        DAZE_STATE ( victim, 3 * PULSE_VIOLENCE );
        WAIT_STATE ( ch, skill_table[gsn_stake].beats );
        victim->position = POS_RESTING;

        damage ( ch, victim,
                 ( ( ch->level * ( dice ( ( int ) ch->level / 4, 6 ) ) ) +
                   ch->level ), gsn_stake, DAM_PIERCE, TRUE, 1 );

    }
    else
    {
        damage ( ch, victim, 0, gsn_stake, DAM_PIERCE, TRUE, 1 );
        act ( "You fall flat on your face!", ch, NULL, victim, TO_CHAR );
        act ( "$n falls flat on $s face.", ch, NULL, victim, TO_NOTVICT );
        act ( "You evade $n's stake, causing $m to fall flat on $s face.", ch,
              NULL, victim, TO_VICT );
        check_improve ( ch, gsn_stake, FALSE, 1 );
        ch->position = POS_RESTING;
        WAIT_STATE ( ch, skill_table[gsn_stake].beats * 3 / 2 );
    }
}

CH_CMD ( do_strike )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( get_skill ( ch, gsn_strike ) == 0 ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_strike].skill_level[ch->class] ) )
    {
        send_to_char ( "Strike? What's that?\n\r", ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char ( "You need to wield a primary weapon to strike.\n\r",
                       ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char
            ( "You need to see your opponent in order to strike.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_strike].beats );
    if ( number_percent (  ) < get_skill ( ch, gsn_strike ) ||
         ( get_skill ( ch, gsn_strike ) >= 2 && !IS_AWAKE ( victim ) ) )
    {
        check_improve ( ch, gsn_circle, TRUE, 1 );
        act ( "{i$n strikes you with speed and accuracy!{x", ch, NULL, victim,
              TO_VICT );
        act ( "{hYou strike $N with speed and accuracy!{x", ch, NULL, victim,
              TO_CHAR );
        act ( "{k$n strikes $N with speed and accuracy!{x", ch, NULL, victim,
              TO_NOTVICT );
        multi_hit ( ch, victim, gsn_strike );
    }
    else
    {
        check_improve ( ch, gsn_strike, FALSE, 1 );
        act ( "{i$n fails to strike you.{x", ch, NULL, victim, TO_VICT );
        act ( "{h$N dodges your strike.{x", ch, NULL, victim, TO_CHAR );
        act ( "{k$n fails to strike $N.{x", ch, NULL, victim, TO_NOTVICT );
        damage ( ch, victim, 0, gsn_strike, DAM_NONE, TRUE, 1 );
    }

    return;
}

CH_CMD ( do_nervestrike )
{
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_nervestrike].skill_level[ch->class] )
    {
        send_to_char ( "You had best leave that skill to master thieves.\n\r",
                       ch );
        return;
    }
    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "Now that just wouldn't be right!\n\r", ch );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char ( "You stumble blindly into a wall.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_nervestrike].beats );

    if ( get_skill ( ch, gsn_nervestrike ) < number_percent (  ) )
    {
        printf_to_char ( ch,
                         "You prepare for a nervestrike but your opponent is too quick!\n\r" );
        check_improve ( ch, gsn_nervestrike, FALSE, 1 );
        return;
    }

    dam = ch->level * 40;

    damage ( ch, victim, number_range ( dam, ch->level * 46 ),
             gsn_nervestrike, DAM_BASH, TRUE, 1 );
    check_improve ( ch, gsn_nervestrike, TRUE, 3 );

    if ( chance ( 40 ) )
    {
        act("You {Ystun{x $N with your nervestrike.",
            ch,NULL,victim,TO_CHAR);
        act("You are stunned by $n's nervestrike.",
            ch,NULL,victim,TO_VICT);
        act("$N is stunned.",
            ch,NULL,victim,TO_NOTVICT);
/*
        printf_to_char ( ch,
                         "{YYour opponent is stunned by your {Rf{ru{Rr{ri{Ro{ru{Rs{Y{x nervestrike!{x\n\r", ch );
*/ 
        victim->stunned = 2;
    }

    return;
}

CH_CMD ( do_thrust )
{
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_thrust].skill_level[ch->class] )
    {
        send_to_char ( "You better leave the martial arts to fighters.\n\r",
                       ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char ( "You stumble blindly into a wall.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char ( "You need to wield a primary weapon to thrust.\n\r",
                       ch );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char ( "You stumble blindly into a wall.\n\r", ch );
        return;
    }

    dam = 50 + dice ( 100, 250 );
    WAIT_STATE ( ch, skill_table[gsn_thrust].beats / 2 );
    if ( get_skill ( ch, gsn_thrust ) > number_percent (  ) )
    {
        damage ( ch, victim, number_range ( dam, ( ch->level * 55 ) ),
                 gsn_thrust, DAM_PIERCE, TRUE, 1 );
        check_improve ( ch, gsn_thrust, TRUE, 1 );
    }
    else
    {
        damage ( ch, victim, 0, gsn_thrust, DAM_PIERCE, TRUE, 1 );
        check_improve ( ch, gsn_thrust, FALSE, 1 );
    }
    return;
}


CH_CMD ( do_impale )
{
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_impale].skill_level[ch->class] )
    {
        send_to_char ( "Impale? What is that?\n\r",
                       ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char ( "You aren't fighting anyone.\n\r", ch );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char ( "You stumble blindly into a wall.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char ( "You need to wield a primary weapon to impale.\n\r",
                       ch );
        return;
    }

    if ( !can_see ( ch, victim ) )
    {
        send_to_char ( "You stumble blindly into a wall.\n\r", ch );
        return;
    }

    dam = 50 + dice ( 300, 500 );
    WAIT_STATE ( ch, skill_table[gsn_impale].beats / 2 );
    if ( get_skill ( ch, gsn_impale ) > number_percent (  ) )
    {
        damage ( ch, victim, number_range ( dam, ( ch->level * 55 ) ),
                 gsn_impale, DAM_PIERCE, TRUE, 1 );
        check_improve ( ch, gsn_impale, TRUE, 1 );
    }
    else
    {
        damage ( ch, victim, 0, gsn_impale, DAM_PIERCE, TRUE, 1 );
        check_improve ( ch, gsn_impale, FALSE, 1 );
    }
    return;
}

CH_CMD ( do_whirlwind )
{
    CHAR_DATA *pChar;
    CHAR_DATA *pChar_next;
    OBJ_DATA *wield;
    bool found = FALSE;

    if ( !IS_NPC ( ch ) &&
         ch->level < skill_table[gsn_whirlwind].skill_level[ch->class] )
    {
        send_to_char
            ( "You don't know how to do that!\n\r", ch );
        return;
    }
/*
if (!IS_CLAN(victim)
   {
    printf_to_char(ch,"They aren't in a clan");
    return;
    }*/

    if ( ( wield = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char ( "You need to wield a weapon first...\n\r", ch );
        return;
    }

    act ( "$n holds $p firmly, and starts spinning round...", ch, wield, NULL,
          TO_ROOM );
    act ( "You hold $p firmly, and start spinning round...", ch, wield, NULL,
          TO_CHAR );

    pChar_next = NULL;
    for ( pChar = ch->in_room->people; pChar; pChar = pChar_next )
    {
        pChar_next = pChar->next_in_room;
        if ( IS_NPC ( pChar ) )
        {
            found = TRUE;
            act ( "$n turns towards YOU!", ch, NULL, pChar, TO_VICT );
            multi_hit ( ch, pChar, gsn_whirlwind );
            /* one_hit( ch, pChar, gsn_whirlwind, FALSE ); */
        }
        if ( !IS_NPC ( pChar ) && !IS_IMMORTAL ( pChar ) )
        {
            found = TRUE;
            act ( "$n turns towards YOU!", ch, NULL, pChar, TO_VICT );
            multi_hit ( ch, pChar, gsn_whirlwind );
            /* one_hit( ch, pChar, gsn_whirlwind, FALSE ); */
        }
    }

    if ( !found )
    {
        act ( "$n looks dizzy, and a tiny bit embarassed.", ch, NULL, NULL,
              TO_ROOM );
        act ( "You feel dizzy, and a tiny bit embarassed.", ch, NULL, NULL,
              TO_CHAR );
    }

    else check_improve ( ch, gsn_whirlwind, TRUE, 1 );
    
    WAIT_STATE ( ch, skill_table[gsn_whirlwind].beats );

    if ( !found && number_percent (  ) < 25 )
    {
        act ( "$n loses $s balance and falls into a heap.", ch, NULL, NULL,
              TO_ROOM );
        act ( "You lose your balance and fall into a heap.", ch, NULL, NULL,
              TO_CHAR );
        ch->position = POS_STUNNED;
        check_improve ( ch, gsn_whirlwind, FALSE, 1 );
    }

    return;
}

CH_CMD ( do_call_wild )
{
    AFFECT_DATA af;

    if ( get_skill ( ch, gsn_call_wild ) == 0 ||
         ( !IS_NPC ( ch ) &&
           ch->level < skill_table[gsn_call_wild].skill_level[ch->class] ) )
    {
        send_to_char ( "You scream at the top of your lungs!\n\r", ch );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CALM ) )
    {
        send_to_char ( "{hYou're feeling to mellow.{x\n\r", ch );
        return;
    }

    if ( is_affected ( ch, gsn_call_wild ) )
    {
        send_to_char ( "You are already as wild as possible.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, skill_table[gsn_call_wild].beats );

    if ( number_percent (  ) > ch->pcdata->learned[gsn_call_wild] )
    {
        act ( "$n throws back $s head and howls loudly.", ch, NULL, NULL,
              TO_ROOM );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = gsn_call_wild;
    af.level = ch->level;
    af.duration = ch->level / 5;
    af.location = APPLY_AC;
    af.modifier = 0 - ch->level;
    af.bitvector = 0;
    affect_to_char ( ch, &af );

    af.where = TO_AFFECTS;
    af.type = gsn_call_wild;
    af.level = ch->level;
    af.duration = ch->level / 5;
    af.location = APPLY_HITROLL;
    af.modifier = ch->level / 3;
    af.bitvector = 0;
    affect_to_char ( ch, &af );

    af.where = TO_AFFECTS;
    af.type = gsn_call_wild;
    af.level = ch->level;
    af.duration = ch->level / 5;
    af.location = APPLY_DAMROLL;
    af.modifier = ch->level / 3;
    af.bitvector = 0;
    affect_to_char ( ch, &af );
    send_to_char ( "You focus and feel the beast within come forth!\n\r",
                   ch );
    return;
}

void check_arena ( CHAR_DATA * ch, CHAR_DATA * victim )
{
    DESCRIPTOR_DATA *d;         /* needed for Arena bet checking */
    char buf[MAX_STRING_LENGTH];
    float odds;
    float lvl1, lvl2;
    int payoff;

    if ( IS_NPC ( ch ) || IS_NPC ( victim ) )
        return;

    sprintf ( buf, "{R[{WARENA{R] {b%s{x has defeated {b%s{x!\n\r", ch->name,
              victim->name );
    sprintf ( lastwinner, "%s", ch->name );
    REMOVE_BIT ( victim->act2, PLR2_MASS_ARENA );
    REMOVE_BIT ( victim->act2, PLR2_MASS_JOINER );
    REMOVE_BIT ( victim->act2, PLR2_PKVIOLENT );
    REMOVE_BIT ( victim->act2, PLR_NORESTORE );
    victim->pcdata->pkviolenttime = 0;
    victim->pcdata->pksafetime = 0;
    ch->stunned = 0;
    ch->position = POS_STANDING;
/*
    ch->challenge_timer = 5;
    victim->challenge_timer = 5;
*/
    //arenacount = 0;
    do_gmessage ( buf );

    if ( !is_mass_arena )
    {
        ch->pcdata->awins += 1;
        victim->pcdata->alosses += 1;
    }
    else
    {
	ch->pcdata->tkills += 1;
        victim->pcdata->tdeaths += 1;
    }
    lvl1 = ch->hit;
    lvl2 = victim->hit;
    odds = ( lvl2 / lvl1 );
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            if ( d->character->gladiator == ch )
            {
                payoff = d->character->pcdata->plr_wager * ( odds + 1 );
                payoff = abs ( payoff );
                sprintf ( buf,
                          "{WYou won! Your wager: {D%d{W, payoff: {D%d{x\n\r",
                          d->character->pcdata->plr_wager, payoff );
                send_to_char ( buf, d->character );
                d->character->platinum += payoff;
                /* reset the betting info */
                d->character->gladiator = NULL;
                d->character->pcdata->plr_wager = 0;
                payoff = 0;
            }
            if ( d->character->gladiator != ch &&
                 d->character->pcdata->plr_wager >= 1 )
            {
                //int tmp = 0;

                sprintf ( buf, "{WYou lost! Your wager: {D%d{x\n\r",
                          d->character->pcdata->plr_wager );
                send_to_char ( buf, d->character );
/*                if ( d->character->pcdata->plr_wager > d->character->exp )
                {
                    tmp = d->character->pcdata->plr_wager;
                    d->character->pcdata->plr_wager -= tmp;
                }
                if ( tmp > 0 ) */
                    /* d->character->pcdata->quest -= tmp; */
                    d->character->platinum -= d->character->pcdata->plr_wager;
                /* reset the betting info */
                d->character->gladiator = NULL;
                d->character->pcdata->plr_wager = 0;
            }
        }
    }

    /* now move both fighters out of arena and back to the regular "world" be
       sure to define ROOM_VNUM_AWINNER and ROOM_VNUM_ALOSER */
    stop_fighting ( victim, TRUE );
    char_from_room ( victim );
    char_to_room ( victim, get_room_index ( ROOM_VNUM_ALOSER ) );
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    update_pos ( victim );
    do_look ( victim, "auto" );

    if ( !is_mass_arena )
    {
        stop_fighting ( ch, TRUE );
        char_from_room ( ch );
        char_to_room ( ch, get_room_index ( ROOM_VNUM_AWINNER ) );
        ch->hit = ch->max_hit;
        ch->mana = ch->max_mana;
        ch->move = ch->max_move;
        update_pos ( ch );
        do_look ( ch, "auto" );

        if ( IS_SET ( ch->act2, PLR2_CHALLENGER ) )
            REMOVE_BIT ( ch->act2, PLR2_CHALLENGER );
        if ( IS_SET ( ch->act2, PLR2_CHALLENGED ) )
            REMOVE_BIT ( ch->act2, PLR2_CHALLENGED );
        if ( IS_SET ( ch->act2, PLR2_PKVIOLENT ) )
            REMOVE_BIT ( ch->act2, PLR2_PKVIOLENT );
        if ( IS_SET ( ch->act, PLR_NORESTORE ) )
            REMOVE_BIT ( ch->act, PLR_NORESTORE );
        if ( IS_SET ( ch->act2, PLR2_PKSAFE ) )
            REMOVE_BIT ( ch->act2, PLR2_PKSAFE );
	if ( ch->pcdata->pkviolenttime != 0 )
	   ch->pcdata->pkviolenttime = 0;
	if ( ch->pcdata->pksafetime != 0 )
	   ch->pcdata->pksafetime = 0;
    }

    if ( IS_SET ( victim->act2, PLR2_CHALLENGER ) )
        REMOVE_BIT ( victim->act2, PLR2_CHALLENGER );
    if ( IS_SET ( victim->act2, PLR2_CHALLENGED ) )
        REMOVE_BIT ( victim->act2, PLR2_CHALLENGED );

    if ( IS_SET ( victim->act2, PLR2_PKSAFE ) )
        REMOVE_BIT ( victim->act2, PLR2_PKSAFE );
    if ( IS_SET ( victim->act2, PLR2_PKVIOLENT ) )
        REMOVE_BIT ( victim->act2, PLR2_PKVIOLENT );
    if ( IS_SET ( victim->act, PLR_NORESTORE ) )
        REMOVE_BIT ( victim->act, PLR_NORESTORE );
	if ( victim->pcdata->pkviolenttime != 0 )
	   victim->pcdata->pkviolenttime = 0;
	if ( victim->pcdata->pksafetime != 0 )
	   victim->pcdata->pksafetime = 0;

    ch->challenger = NULL;
    ch->challenged = NULL;
    victim->challenger = NULL;
    victim->challenged = NULL;

    arena = FIGHT_OPEN;         /* clear the arena */

    return;
}

/*
 * Pull command with permission from Leif Abbott (Leifab)
 */
CH_CMD ( do_pull )
{
   char bufg[3*MSL];
   char buf[MSL];
   DESCRIPTOR_DATA	*d;
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   OBJ_DATA *map;    /* Map with a list of where the bits went  :) */
   char mapbuf[MSL];
   ROOM_INDEX_DATA *location;
   char *name;
   EXTRA_DESCR_DATA *ed;
/*   EXTRA_DESCR_DATA *new_extra_descr( void ); */

   if (IS_NPC(ch))
     {
	send_to_char("Ummm.. No.\n\r", ch);
	return;
     }

   if (argument[0] == '\0')
     {
	send_to_char("Syntax: pull <victim>\n\r", ch);
	return;
     }

   if ((victim = get_char_room(ch, argument)) == NULL)
     {
	send_to_char("That person is not here.\n\r", ch);
	return;
     }

   if (victim == ch)
     {
	send_to_char("Ummm.. No.\n\r", ch);
	return;
     }

   if ( !IS_NPC ( victim ) && ( ch->level < HEADIMP ) )
   {
	send_to_char ( "Don't you fucking dare.\n\r", ch );
	return;
   }

   if (get_trust(victim) > get_trust(ch))
     {
	send_to_char("Better not... You might upset someone...\n\r", ch);
	return;
     }

   act("$n snaps $s fingers and a shotgun appears in $s hands.", ch, NULL, NULL, TO_ROOM);

	sprintf(bufg, "%s shouts 'PULL!'\n\r", ch->name);
	sprintf(buf, "You notice %s fly high into the air.\n\r", IS_NPC(victim) ? victim->short_descr : 
victim->name);
	strcat(bufg, buf);
   	sprintf(buf, "You hear a loud bang as %s fires, blasting %s into pieces!\n\r", ch->name, IS_NPC(victim) ? 
victim->short_descr : victim->name);
	strcat(bufg, buf);
   	sprintf(buf, "Pieces of %s rain down over the lands.\n\r", IS_NPC(victim) ? victim->short_descr : 
victim->name);
   	strcat(bufg, buf);

   for ( d = descriptor_list ; d ; d = d->next )
   {
   		if ( d->connected == CON_PLAYING )
   		{
   		send_to_char( bufg, d->character );
   		}
   }

   /* This is large and repetitive, but adds a bit of life - Leif */

   sprintf(mapbuf, "{xYour stuff can be found at the following locations!{x\n\r\n\r");


   if (IS_SET(victim->parts, PART_GUTS))
     {
	name            = IS_NPC(ch) ? victim->short_descr : victim->name;
	obj = create_object( get_obj_index( OBJ_VNUM_GUTS ), 0 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	location = pull_random_room(ch);
	obj_to_room(obj, location);

	sprintf(mapbuf, "%s\n\r%s - %s [%ld] (%s)", mapbuf, obj->short_descr,
		location->name, location->vnum, location->area->name);
     }

   if (IS_SET(victim->parts, PART_HEAD))
     {
	name = IS_NPC(ch) ? victim->short_descr : victim->name;
	obj = create_object( get_obj_index( OBJ_VNUM_SEVERED_HEAD ), 0 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	location = pull_random_room(ch);
	obj_to_room(obj, location);

	sprintf(mapbuf, "%s\n\r%s - %s [%ld] (%s)", mapbuf, obj->short_descr,
 	location->name, location->vnum, location->area->name);
     }

   if (IS_SET(victim->parts, PART_HEART))
     {
	name            = IS_NPC(ch) ? victim->short_descr : victim->name;
	obj = create_object( get_obj_index( OBJ_VNUM_TORN_HEART ), 0 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	location = pull_random_room(ch);
	obj_to_room(obj, location);

	sprintf(mapbuf, "%s\n\r%s - %s [%ld] (%s)", mapbuf, obj->short_descr,
		location->name, location->vnum, location->area->name);
     }

    if (IS_SET(victim->parts, PART_ARMS))
     {
	int i;
	for (i=0; i < 2; i++)
	  {
	     name            = IS_NPC(ch) ? victim->short_descr : victim->name;
	     obj = create_object( get_obj_index( OBJ_VNUM_SLICED_ARM ), 0 );

	     sprintf( buf, obj->short_descr, name );
	     free_string( obj->short_descr );
	     obj->short_descr = str_dup( buf );

	     sprintf( buf, obj->description, name );
	     free_string( obj->description );
	     obj->description = str_dup( buf );

	     location = pull_random_room(ch);
	     obj_to_room(obj, location);

	     sprintf(mapbuf, "%s\n\r%s - %s [%ld] (%s)", mapbuf, obj->short_descr,
		     location->name, location->vnum, location->area->name);
	  }
     }

   if (IS_SET(victim->parts, PART_LEGS))
     {
	int i;
	for(i=0; i<2; i++)
	  {

	     name            = IS_NPC(ch) ? victim->short_descr : victim->name;
	     obj = create_object( get_obj_index( OBJ_VNUM_SLICED_LEG ), 0 );

	     sprintf( buf, obj->short_descr, name );
	     free_string( obj->short_descr );
	     obj->short_descr = str_dup( buf );

	     sprintf( buf, obj->description, name );
	     free_string( obj->description );
	     obj->description = str_dup( buf );

	     location = pull_random_room(ch);
	     obj_to_room(obj, location);

	     sprintf(mapbuf, "%s\n\r%s - %s [%ld]  (%s)", mapbuf, obj->short_descr,
		     location->name, location->vnum, location->area->name);
	  }
     }

      if (IS_SET(victim->parts, PART_BRAINS))
     {
	name            = IS_NPC(ch) ? victim->short_descr : victim->name;
	obj = create_object( get_obj_index( OBJ_VNUM_BRAINS ), 0 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	location = pull_random_room(ch);
	obj_to_room(obj, location);

	sprintf(mapbuf, "%s\n\r%s - %s [%ld] (%s)", mapbuf, obj->short_descr,
		location->name, location->vnum, location->area->name);
     }


   /* Ok.. the body parts are scattered.. now scatter the characters
    * belongings */

   for ( obj = victim->carrying; obj != NULL; obj = obj_next )
     {
	obj_next = obj->next_content;
	location = pull_random_room(victim);
	obj_from_char(obj);
	obj_to_room(obj, location);
	sprintf(mapbuf, "%s\n\r%s - %s [%ld] (%s){x", mapbuf, obj->short_descr, location->name, location->vnum, location->area->name);
     }

   map = create_object(get_obj_index(40), 0);
   sprintf(mapbuf, "%s\n\r", mapbuf);

   ed = new_extra_descr();
   ed->keyword = str_dup("map");
   ed->next = map->extra_descr;
   ed->description = str_dup(mapbuf);
   map->extra_descr = ed;

   obj_to_char(map, ch);

   if (IS_NPC(victim))
     extract_char(victim, TRUE);
   else
     extract_char(victim, FALSE);

   return;

}

CH_CMD ( do_knee )
{
CHAR_DATA *victim;
int dam;

if ( !IS_NPC(ch)
&& ch->level < skill_table[gsn_knee].skill_level[ch->class] )
{
send_to_char(
"You better leave the martial arts to fighters.\n\r", ch );
return;
}

if ( ( victim = ch->fighting ) == NULL )
{
send_to_char( "You aren't fighting anyone.\n\r", ch );
return;
}

if (ch->stunned)
{
send_to_char("You're still a little woozy.\n\r",ch);
return;
}

if ( !can_see ( ch, victim ) )
{
send_to_char ( "You can't seem to find their head.\n\r", ch );
return;
}

WAIT_STATE( ch, skill_table[gsn_knee].beats );
if ( get_skill(ch,gsn_knee) > number_percent())
{
check_improve(ch,gsn_knee,TRUE,1);
act( "{i$n grabs your head and slams their {Rk{rn{Re{re{i into your face.{x", ch, NULL, victim, TO_VICT);
act( "{hYou grab $N {hand slam your {Rk{rn{Re{re{h into their face.{x", ch, NULL, victim, TO_CHAR);
act( "{k$n grabs $N {kand slams $s {Rk{rn{Re{re{k into their face.{x", ch, NULL, victim, TO_NOTVICT);
if ( chance ( 30 ) && ch->level > skill_table[gsn_stun].skill_level[ch->class] )
{
   send_to_char ( "{xThey are {Ys{yt{Yu{yn{Yn{ye{Yd{x {xby your knee to their {rf{Ra{rc{Re!{x\n\r", ch );
   send_to_char ( "{xYou are {Ys{yt{Yu{yn{Yn{ye{Yd{x by your opponents knee to your face!\n\r", victim );
   if ( chance ( 50 ) )
   victim->stunned = 5;
   else if ( chance ( 60 ) )
   victim->stunned = 4;
   else if ( chance ( 70 ) )
   victim->stunned = 3;
   else if ( chance ( 80 ) )
   victim->stunned = 2;
   else victim->stunned = 1;
}
dam = number_range ( (ch->level * 90 ), ( 5 ));
damage( ch, victim, dam, gsn_knee,DAM_BASH,TRUE,1);
WAIT_STATE ( ch, skill_table[gsn_knee].beats );
}
else
{
check_improve(ch,gsn_knee,FALSE,1);
act( "{i$n tries to {Rk{rn{Re{re{i you but misses.{x", ch, NULL, victim, TO_VICT);
act( "{hYou try to {Rk{rn{Re{re{h $N but miss.{x", ch, NULL, victim, TO_CHAR);
act( "{k$n tries to {Rk{rn{Re{re{k $N but misses.{x", ch, NULL, victim, TO_NOTVICT);
damage( ch, victim, 0, gsn_knee,DAM_BASH,TRUE,1);
}
return;
}

CH_CMD ( do_windpipe )
{
CHAR_DATA *victim;

if ( !IS_NPC(ch)
&& ch->level < skill_table[gsn_windpipe].skill_level[ch->class] )
{
send_to_char(
"You better leave the martial arts to fighters.\n\r", ch );
return;
}

if ( ( victim = ch->fighting ) == NULL )
{
send_to_char( "You aren't fighting anyone.\n\r", ch );
return;
}

if (ch->stunned)
{
send_to_char("You're still a little woozy.\n\r",ch);
return;
}

if ( !can_see ( ch, victim ) )
{
send_to_char ( "You can't seem to find their throat.\n\r", ch );
return;
}

WAIT_STATE( ch, skill_table[gsn_windpipe].beats );
if ( get_skill(ch,gsn_windpipe) > number_percent( ) && chance ( 30 ) )
{
check_improve(ch,gsn_windpipe,TRUE,1);
act( "{i$n hits you in your throat.{x", ch, NULL, victim, TO_VICT);
act( "{hYou hit $N {hin the throat.{x", ch, NULL, victim, TO_CHAR);
act( "{k$n hits $N {kin the throat.{x", ch, NULL, victim, TO_NOTVICT);
send_to_char ( "{xThey are {Ys{yt{Yu{yn{Yn{ye{Yd{x {xby your hit!{x\n\r", ch );
send_to_char ( "{xYou are {Ys{yt{Yu{yn{Yn{ye{Yd{x by your opponents hit in your windpipe!\n\r", victim );
if ( chance ( 20 ) )
victim->stunned = 3;
else if ( chance ( 35 ) )
victim->stunned = 2;
else
victim->stunned = 1;

/*dam = number_range ( (ch->level * 120 ), ( 5 ));
damage( ch, victim, dam, gsn_windpipe,DAM_BASH,TRUE); */
WAIT_STATE ( ch, skill_table[gsn_windpipe].beats );
}
else
{
check_improve(ch,gsn_windpipe,FALSE,1);
act( "{i$n tries to hit you in the throat, but misses.{x", ch, NULL, victim, TO_VICT);
act( "{hYou try to hit $N in the throat, but miss.{x", ch, NULL, victim, TO_CHAR);
act( "{k$n tries to hit $N in the windpipe but misses.{x", ch, NULL, victim, TO_NOTVICT);
//damage( ch, victim, 0, gsn_windpipe,DAM_BASH,TRUE);
}
return;
}


void do_push_drag( CHAR_DATA *ch, char *argument, char *verb )
{
    char arg1[MIL];
    char arg2[MIL];
    char buf[MSL];
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    //OBJ_DATA *obj;
    int door;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
//    victim = get_char_room(ch, NULL,arg1);
    victim = get_char_room(ch, arg1);
//    obj = get_obj_list( ch, arg1, ch->in_room->contents );

	if (!is_clan(ch))
	{
		send_to_char("Sorry, but {4push{0 and {1drag{x are for pk only.\n\r",ch);
		return;
	}

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	sprintf( buf, "{W%s{x whom or what where?\n\r", capitalize(verb));
	send_to_char( buf, ch );
	return;
    }

    if ( !victim || !can_see(ch,victim) )
    {
	sprintf(buf,"{W%s{x whom or what where?\n\r", capitalize(verb));
	send_to_char( buf, ch );
        return;
    }

         if ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) door = 0;
    else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) door = 1;
    else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) door = 2;
    else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) door = 3;
    else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) door = 4;
    else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) door = 5;
    else
    {
      sprintf( buf, "Alas, you cannot {&%s{x in that direction.\n\r", verb );
      send_to_char( buf, ch );
      return;
    }
  if (victim)

  {

	if ( IS_NPC(victim) )

	{

	send_to_char("You can't push NPCs.\n\r",ch);

	return;

	}
	if ( !IS_SET ( ch->act2, PLR2_PKVIOLENT ) || !IS_SET ( victim->act2, PLR2_PKVIOLENT ) )
	{
	send_to_char("Both you and your target must be {RVIOLENT{x for push or drag to work.\n\r",ch);
		return;
	}
  }
/*  if ( obj )
  {

	send_to_char("You can't push objects.\n\r",ch);

	return;
  }

  else
  { */
    if ( ch == victim )
    {
	act( "You {Y$t{x yourself about the room and look very silly.", ch, verb, NULL, TO_CHAR );
	act( "{^$n{x decides to be silly and {Y$t {&$m{xself about the room.", ch, verb, NULL, TO_ROOM );
	return;
    }

    in_room = victim->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL
    ||	 !can_see_room(victim,pexit->u1.to_room))
    {
        sprintf( buf, "Alas, you cannot {&%s{x them that way.\n\r", verb );
        send_to_char( buf, ch );
	return;
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED)
    &&  (!IS_AFFECTED(victim, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS)))
    {
	act( "You try to {&$t{x them through the {Y$d{x.", ch, verb, pexit->keyword, TO_CHAR );
	act( "{!$n{x decides to {&$t{x you around!", ch, verb, victim, TO_VICT );
	act( "{1$n{x decides to {&$t {Y$N{x around!", ch, verb, victim, TO_NOTVICT );
	return;
    }

    act( "You attempt to {&$t {Y$N{x out of the room.", ch, verb, victim, TO_CHAR );
    act( "{1$n{x is attempting to {&$t{x you out of the room!", ch, verb, victim, TO_VICT );
    act( "{1$n{x is attempting to {&$t {Y$N{x out of the room.", ch, verb, victim, TO_NOTVICT );

    if ( IS_NPC(victim) )
	{
		send_to_char("Not on NPCs.\n\r",ch);
		return;
	}

	if ( !IS_SET ( victim->act2, PLR2_PKVIOLENT ) )
	{
		send_to_char("That player is not violent.\n\r",ch);
		return;
	}

	if (  victim->in_room == NULL )
	{
		send_to_char("They are not here!\n\r",ch);
		return;
	}

	if ( (IS_SET(victim->in_room->room_flags, ROOM_SAFE)
		|| IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
		|| IS_SET(victim->in_room->room_flags, ROOM_SOLITARY))
		&& !IS_SET ( victim->act2, PLR2_PKVIOLENT ) )
	{
		send_to_char("Not in this room with that person!\n\r",ch);
		return;
	}

	if ( ch->size < victim->size )  /* size check for push_drag */
	{
    act( "{Y$N{x is WAY too {RHUGE{x to {&$t{x out of the room.", ch, verb, victim, TO_CHAR );
    act( "{1$n{x can't {&$t{x you, you are too {RHUGE{x!", ch, verb, victim, TO_VICT );
    act( "{1$n{x cant't {&$t {Y$N{x, they are too {RHUGE{x!", ch, verb, victim, TO_NOTVICT );
		return;
	}

	if ((victim->level + 15 < ch->level) && (!IS_IMMORTAL(ch)))
	{
	send_to_char("They are not in your level range.\n\r",ch);
	return;
	}

    if ( (number_percent() > 75) /*random roll*/
	|| ( victim->max_hit > ( ch->max_hit + ( get_curr_stat(ch,STAT_STR)*20) ) ) )/*str check*/
    {
	send_to_char( "They won't budge.\n\r", ch );
	return;
    }

    if ( ch->move > 10 )
    {
	ch->move -= 10;
	send_to_char( "{%You succeed!{x\n\r", ch );
	act( "{!$n succeeds!{x", ch, NULL, NULL, TO_ROOM );
	if (!str_cmp( verb, "drag" ))
	{
	move_char( ch, door, FALSE, FALSE );
	act( "{1$n is dragged {&$T!{x", victim, NULL, dir_name[door], TO_ROOM );
	act( "You are dragged {&$T!{x\n\r", victim, NULL, dir_name[door], TO_CHAR );
	char_from_room( victim );
	char_to_room( victim, pexit->u1.to_room );
	do_look( victim, "auto" );
	act( "$N drags $n into the room.", victim, NULL, ch, TO_NOTVICT );
	}
	else if (!str_cmp( verb, "push" ))
	{
	act( "$n {Wflies{x {&$T!{x", victim, NULL, dir_name[door], TO_ROOM );
	act( "You {Wfly{x {&$T!{x\n\r", victim, NULL, dir_name[door], TO_CHAR );
	char_from_room( victim );
	char_to_room( victim, pexit->u1.to_room );
	do_look( victim, "auto" );
/*	act( "You notice movement from nearby to the $T.",
		victim, NULL, rev_name[door], TO_ROOM );*/
	act( "{%$n{x {Wflies{x into the room!", victim, NULL, NULL, TO_ROOM );
	}
    }
    else
    {
      sprintf( buf, "You are too {1tired{x to {&%s{x anybody around!\n\r", verb );
      send_to_char( buf, ch );
    }
//  }

  return;
}

void do_push( CHAR_DATA *ch, char *argument )
{
   do_push_drag( ch, argument, "push" );
   return;
}
void do_drag( CHAR_DATA *ch, char *argument )
{
   do_push_drag( ch, argument, "drag" );
   return;
}



