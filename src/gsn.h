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

#ifndef GSN_H
#define GSN_H

#ifdef IN_DB_C
#define GSN(gsn) sh_int gsn
#else
#define GSN(gsn) extern sh_int gsn
#endif

/*
 * These are skill_lookup return values for common skills and spells.
 */
GSN ( gsn_assassinate );
GSN ( gsn_backstab );
GSN ( gsn_circle );
GSN ( gsn_critical );
GSN ( gsn_counter );
GSN ( gsn_call_wild );
GSN ( gsn_dodge );
GSN ( gsn_envenom );
GSN ( gsn_feed );
GSN ( gsn_hide );
GSN ( gsn_peek );
GSN ( gsn_pick_lock );
GSN ( gsn_sneak );
GSN ( gsn_steal );
GSN ( gsn_nervestrike );
GSN ( gsn_disarm );
GSN ( gsn_dual_wield );
GSN ( gsn_enhanced_damage );
GSN ( gsn_kick );
GSN ( gsn_parry );
GSN ( gsn_rescue );
GSN ( gsn_second_attack );
GSN ( gsn_third_attack );
GSN ( gsn_fourth_attack );
GSN ( gsn_fifth_attack );
GSN ( gsn_sixth_attack );
GSN ( gsn_seventh_attack );
GSN ( gsn_eighth_attack );
GSN ( gsn_ninth_attack );
GSN ( gsn_tenth_attack );
GSN ( gsn_blindness );
GSN ( gsn_charm_person );
GSN ( gsn_curse );
GSN ( gsn_invis );
GSN ( gsn_mass_invis );
GSN ( gsn_plague );
GSN ( gsn_poison );
GSN ( gsn_sleep );
GSN ( gsn_strangle );
GSN ( gsn_fly );
GSN ( gsn_sanctuary );
GSN ( gsn_strike );
GSN ( gsn_sharpen );
GSN ( gsn_stake );
GSN ( gsn_ambush );
GSN ( gsn_garrote );
/* new gsns */
GSN ( gsn_axe );
GSN ( gsn_dagger );
GSN ( gsn_flail );
GSN ( gsn_mace );
GSN ( gsn_polearm );
GSN ( gsn_shield_block );
GSN ( gsn_spear );
GSN ( gsn_sword );
GSN ( gsn_whip );

GSN ( gsn_ssj );
GSN ( gsn_bash );
GSN ( gsn_berserk );
GSN ( gsn_dirt );
GSN ( gsn_hand_to_hand );
GSN ( gsn_trip );

GSN ( gsn_fast_healing );
GSN ( gsn_haggle );
GSN ( gsn_lore );
GSN ( gsn_meditation );

GSN ( gsn_scrolls );
GSN ( gsn_staves );
GSN ( gsn_wands );
GSN ( gsn_recall );
GSN ( gsn_stun );
GSN ( gsn_track );
GSN ( gsn_gouge );
GSN ( gsn_grip );
GSN ( gsn_rub );
GSN ( gsn_rampage );
GSN ( gsn_thrust );
GSN ( gsn_whirlwind );
GSN ( gsn_shield_levitation );
GSN ( gsn_mistblend );
GSN ( gsn_butcher );
GSN ( gsn_flee );
GSN ( gsn_bodyslam );
GSN ( gsn_knee );
GSN ( gsn_windpipe );
GSN ( gsn_impale );
GSN ( gsn_stance );
GSN ( gsn_gorge );

#endif

