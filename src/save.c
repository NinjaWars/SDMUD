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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sd.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"

#if !defined(macintosh)
extern int _filbuf args ( ( FILE * ) );
#endif

char *print_flags ( int flag )
{
    int count, pos = 0;
    static char buf[52];

    for ( count = 0; count < 32; count++ )
    {
        if ( IS_SET ( flag, 1 << count ) )
        {
            if ( count < 26 )
                buf[pos] = 'A' + count;
            else
                buf[pos] = 'a' + ( count - 26 );
            pos++;
        }
    }

    if ( pos == 0 )
    {
        buf[pos] = '0';
        pos++;
    }

    buf[pos] = '\0';

    return buf;
}

/*
 * Array of containers read for proper re-nesting of objects.
 */
static OBJ_DATA *rgObjNest[MAX_NEST];

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj ( CHAR_DATA * ch )
{
    char strsave[MAX_INPUT_LENGTH];
    FILE *fp=NULL;
    char debug[MSL];

    if ( IS_NPC ( ch ) )
        return;
    
    if ( !IS_VALID ( ch ) )
    {
        bug ( "save_char_obj: trying to save invalid character", 0 );
        return;
    }

    if ( ch->desc != NULL && ch->desc->original != NULL )
        ch = ch->desc->original;

        if ( debug_mode )
	{
           sprintf ( debug, "Now saving %s's character", ch->name );
           debug_log ( debug );        
	}
/*
#if defined(unix)
    if ( IS_IMMORTAL ( ch ) || ch->level >= LEVEL_IMMORTAL )
    {
        sprintf ( strsave, "%s%s", GOD_DIR, capitalize ( ch->name ) );
        if ( ( fp = file_open ( strsave, "w" ) ) == NULL )
        {
            bug ( "Save_char_obj: fopen", 0 );
            perror ( strsave );
        }

        fprintf ( fp, "Lev %2d Trust %2d  %s%s\n", ch->level,
                  get_trust ( ch ), ch->name, ch->pcdata->title );
        file_close ( fp );
    }
#endif
*/
    sprintf ( strsave, "%s%s", PLAYER_DIR, capitalize ( ch->name ) );
    if ( ( fp = file_open ( TEMP_FILE, "w" ) ) == NULL )
    {
        bug ( "Save_char_obj: fopen", 0 );
        perror ( strsave );
    }
    else
    {
	fwrite_char ( ch, fp );
        if ( ch->carrying != NULL )
	{
	  if ( debug_mode )
	  {
	      sprintf ( debug, "Now saving %s's objects", ch->name );
	      debug_log ( debug );
	  }
            fwrite_obj ( ch, ch->carrying, fp, 0 );
	}
        /* save the pets */
        if ( ch->pet != NULL )
	{
	  if ( debug_mode )
	  {
	    sprintf ( debug, "Now saving %s's pet '%s'", ch->name, ch->pet->name );
	    debug_log ( debug );
	  }
	    fwrite_pet ( ch->pet, fp );
	}
        fprintf ( fp, "#END\n" );
    }
    new_fclose ( fp );
    rename ( TEMP_FILE, strsave );
    return;
}

/*
 * Write the char.
 */
void fwrite_char ( CHAR_DATA * ch, FILE * fp )
{
    AFFECT_DATA *paf;
    long sn, gn, pos;
    int i;
    fprintf ( fp, "#%s\n", IS_NPC ( ch ) ? "MOB" : "PLAYER" );

    fprintf ( fp, "Name %s~\n", ch->name );
    fprintf ( fp, "Id %ld\n", ch->id );
    fprintf ( fp, "LogO %ld\n", current_time );
    fprintf ( fp, "Vers %d\n", 6 );
    if ( ch->short_descr[0] != '\0' )
        fprintf ( fp, "ShD %s~\n", ch->short_descr ); 
    if ( ch->long_descr[0] != '\0' )
        fprintf ( fp, "LnD %s~\n", ch->long_descr );
    if ( ch->description[0] != '\0' )
        fprintf ( fp, "Desc %s~\n", ch->description );
    if ( ch->prompt != NULL ||
         !str_cmp ( ch->prompt, "<%{Rhhp {M%mm {G%vmv{x>" ) )
        fprintf ( fp, "Prom %s~\n", ch->prompt );
    fprintf ( fp, "Race %s~\n", pc_race_table[ch->race].name );
    fprintf ( fp, "CName %s~\n", ch->pcdata->cname );
/*    if ( ch->pcdata->rmessage != '\0' )
    fprintf ( fp, "RMsg %s~\n", ch->pcdata->rmessage ); */
    fprintf ( fp, "TRR %d\n", ch->pcdata->times_rerolled );
    if ( ch->pcdata->exiletime > 0 )
    fprintf ( fp, "ETime %d\n", ch->pcdata->exiletime );
    if ( ch->pcdata->noclantime > 0 )
    fprintf ( fp, "NCTime %d\n", ch->pcdata->noclantime );
    if ( ch->pcdata->rerolltimer > 0 )
    fprintf ( fp, "RTimer %d\n", ch->pcdata->rerolltimer );    
    if ( ch->pcdata->assassinatetimer > 0 )
    fprintf ( fp, "ATimer %d\n", ch->pcdata->assassinatetimer );        
    if ( ch->pcdata->jailtime > 0 )
    fprintf ( fp, "JTime %d\n", ch->pcdata->jailtime );        
    fprintf ( fp, "Tpld %d\n", ch->pcdata->timesplayed );
    fprintf ( fp, "MkMd %d %d\n", ch->pcdata->mkill, ch->pcdata->mdeath );
    fprintf (fp, "Extra %s\n", print_flags (ch->extra));    
    if ( ch->clan )
    {
        fprintf ( fp, "CRank %d\n", ch->clan_rank );
        fprintf ( fp, "Clan %s~\n", clan_table[ch->clan].name );
        if ( ch->clead )
            fprintf ( fp, "Clead %s~\n", clan_table[ch->clan].name );
    }                           
    if ( ch->color != 0 )
        fprintf ( fp, "Color %d\n", ch->color );
    if ( ch->color_auc != 0 )
        fprintf ( fp, "Coauc %d\n", ch->color_auc );
    if ( ch->color_cgo != 0 )
        fprintf ( fp, "Cocgo %d\n", ch->color_cgo );
    if ( ch->color_cla != 0 )
        fprintf ( fp, "Cocla %d\n", ch->color_cla );
    if ( ch->color_con != 0 )
        fprintf ( fp, "Cocon %d\n", ch->color_con );
    if ( ch->color_dis != 0 )
        fprintf ( fp, "Codis %d\n", ch->color_dis );
    if ( ch->color_fig != 0 )
        fprintf ( fp, "Cofig %d\n", ch->color_fig );
    if ( ch->color_gos != 0 )
        fprintf ( fp, "Cogos %d\n", ch->color_gos );
    if ( ch->color_gra != 0 )
        fprintf ( fp, "Cogra %d\n", ch->color_gra );
    if ( ch->color_gte != 0 )
        fprintf ( fp, "Cogte %d\n", ch->color_gte );
    if ( ch->color_imm != 0 )
        fprintf ( fp, "Coimm %d\n", ch->color_imm );
    if ( ch->color_mob != 0 )
        fprintf ( fp, "Comob %d\n", ch->color_mob );
    if ( ch->color_mus != 0 )
        fprintf ( fp, "Comus %d\n", ch->color_mus );
    if ( ch->color_opp != 0 )
        fprintf ( fp, "Coopp %d\n", ch->color_opp );
    if ( ch->color_qgo != 0 )
        fprintf ( fp, "Coqgo %d\n", ch->color_qgo );
    if ( ch->color_que != 0 )
        fprintf ( fp, "Coque %d\n", ch->color_que );
    if ( ch->color_quo != 0 )
        fprintf ( fp, "Coquo %d\n", ch->color_quo );
    if ( ch->color_roo != 0 )
        fprintf ( fp, "Coroo %d\n", ch->color_roo );
    if ( ch->color_say != 0 )
        fprintf ( fp, "Cosay %d\n", ch->color_say );
    if ( ch->color_sho != 0 )
        fprintf ( fp, "Cosho %d\n", ch->color_sho );
    if ( ch->color_tel != 0 )
        fprintf ( fp, "Cotel %d\n", ch->color_tel );
    if ( ch->color_wit != 0 )
        fprintf ( fp, "Cowit %d\n", ch->color_wit );
    if ( ch->color_wiz != 0 )
        fprintf ( fp, "Cowiz %d\n", ch->color_wiz );
    fprintf ( fp, "Sex %d\n", ch->sex );
    fprintf ( fp, "Clas %s~\n", class_table[ch->class].name );
    fprintf ( fp, "Levl %d\n", ch->level );
    fprintf ( fp, "Tier %d\n", ch->pcdata->tier );  /* add this in */
    fprintf ( fp, "Ctier %d\n", ch->pcdata->ctier );    /* add this in */
    if ( ch->class < MAX_CLASS / 2 )
        if ( ch->trust != 0 )
            fprintf ( fp, "Tru %d\n", ch->trust );
    fprintf ( fp, "Sec %d\n", ch->pcdata->security );  /* OLC */
    fprintf ( fp, "Plyd %d\n",
              ch->played + ( int ) ( current_time - ch->logon ) );
/*    fprintf ( fp, "Notb %ld %ld %ld %ld %ld %ld %ld\n", ch->pcdata->last_note,
              ch->pcdata->last_idea, ch->pcdata->last_penalty,
              ch->pcdata->last_news, ch->pcdata->last_changes,
              ch->pcdata->last_weddings, ch->pcdata->last_jokes ); */
    fprintf ( fp, "Scro %d\n", ch->lines );
    fprintf ( fp, "Room %ld\n",
              ( ch->in_room == get_room_index ( ROOM_VNUM_LIMBO ) &&
                ch->was_in_room !=
                NULL ) ? ch->was_in_room->vnum : ch->in_room ==
              NULL ? 3001 : ch->in_room->vnum );
    fprintf(fp, "Hmtown %d\n", ch->hometown);

    fprintf ( fp, "HMV %ld %ld %ld %ld %ld %ld\n", ch->hit, ch->max_hit,
              ch->mana, ch->max_mana, ch->move, ch->max_move );
    if ( ch->platinum > 0 )
        fprintf ( fp, "Plat %ld\n", ch->platinum );
    else
        fprintf ( fp, "Plat %d\n", 0 );
    if ( ch->gold > 0 )
        fprintf ( fp, "Gold %ld\n", ch->gold );
    else
        fprintf ( fp, "Gold %d\n", 0 );
    if ( ch->silver > 0 )
        fprintf ( fp, "Silv %ld\n", ch->silver );
    else
        fprintf ( fp, "Silv %d\n", 0 );
    fprintf ( fp, "Exp %ld\n", ch->exp );
    if ( ch->qps != 0 )
        fprintf ( fp, "Qps %d\n", ch->qps );

	fprintf( fp, "Stance %d %d %d %d %d %d %d %d %d %d %d\n",
	ch->stance[0], ch->stance[1], ch->stance[2], ch->stance[3],
	ch->stance[4], ch->stance[5], ch->stance[6], ch->stance[7],
	ch->stance[8], ch->stance[9], ch->stance[10] );

    if ( ch->act != 0 )
        fprintf ( fp, "Act %s\n", print_flags ( ch->act ) );
    if ( ch->act2 != 0 )
        fprintf ( fp, "Act2 %s\n", print_flags ( ch->act2 ) );

    if ( ch->affected_by != 0 )
        fprintf ( fp, "AfBy %s\n", print_flags ( ch->affected_by ) );
    if ( ch->shielded_by != 0 )
        fprintf ( fp, "ShBy %s\n", print_flags ( ch->shielded_by ) );
    fprintf ( fp, "Comm %s\n", print_flags ( ch->comm ) );
    fprintf ( fp, "Verb %s\n", print_flags ( ch->pcdata->verbose ) );
    if ( ch->wiznet )
        fprintf ( fp, "Wizn %s\n", print_flags ( ch->wiznet ) );
    if ( ch->invis_level )
        fprintf ( fp, "Invi %d\n", ch->invis_level );
    if ( ch->incog_level )
        fprintf ( fp, "Inco %d\n", ch->incog_level );
    if ( ch->ghost_level )
        fprintf ( fp, "Ghos %d\n", ch->ghost_level );
    fprintf ( fp, "Pos  %d\n",
              ch->position == POS_FIGHTING ? POS_STANDING : ch->position );
    if ( ch->practice != 0 )
        fprintf ( fp, "Prac %d\n", ch->practice );
    if ( ch->train != 0 )
        fprintf ( fp, "Trai %d\n", ch->train );
    if ( ch->saving_throw != 0 )
        fprintf ( fp, "Save %d\n", ch->saving_throw );
    fprintf ( fp, "Alig %d\n", ch->alignment );
    if ( ch->hitroll != 0 )
        fprintf ( fp, "Hit %d\n", ch->hitroll );
    if ( ch->damroll != 0 )
        fprintf ( fp, "Dam %d\n", ch->damroll );
    fprintf ( fp, "ACs %d %d %d %d\n", ch->armor[0], ch->armor[1],
              ch->armor[2], ch->armor[3] );
    if ( ch->wimpy != 0 )
        fprintf ( fp, "Wimp %d\n", ch->wimpy );
    fprintf ( fp, "Attr %d %d %d %d %d\n", ch->perm_stat[STAT_STR],
              ch->perm_stat[STAT_INT], ch->perm_stat[STAT_WIS],
              ch->perm_stat[STAT_DEX], ch->perm_stat[STAT_CON] );

    fprintf ( fp, "AMod %d %d %d %d %d\n", ch->mod_stat[STAT_STR],
              ch->mod_stat[STAT_INT], ch->mod_stat[STAT_WIS],
              ch->mod_stat[STAT_DEX], ch->mod_stat[STAT_CON] );

    if ( ch->pcdata->questpoints > 0 )
        fprintf ( fp, "QuestPnts %ld\n", ch->pcdata->questpoints );
    if ( ch->pcdata->nextquest > 0 )
        fprintf ( fp, "QuestNext %d\n", ch->pcdata->nextquest );
    else if ( ch->pcdata->countdown > 0 )
    {
        fprintf ( fp, "QuestCount %d\n", ch->pcdata->countdown );
        fprintf ( fp, "QuestGiver %ld\n", ch->pcdata->questgiver );
        fprintf ( fp, "QuestLoc %ld\n", ch->pcdata->questloc );
        if ( ch->pcdata->questobj != 0 )
            fprintf ( fp, "QuestObj %ld\n", ch->pcdata->questobj );
        else if ( ch->pcdata->questmob != 0 )
            fprintf ( fp, "QuestMob %ld\n", ch->pcdata->questmob );
    }

   if ( ch->pcdata->spouse[0] != '\0' )
    fprintf ( fp, "Spouse %s~\n", ch->pcdata->spouse );
    fprintf ( fp, "Balance %ld\n", ch->pcdata->balance );
    fprintf ( fp, "Shares %ld\n", ch->pcdata->shares );

    if ( IS_NPC ( ch ) )
    {
        fprintf ( fp, "Vnum %ld\n", ch->pIndexData->vnum );
    }
    else
    {
//        fprintf ( fp, "Pass %s~\n", ch->pcdata->pwd );
        fprintf ( fp, "Pass %s~\n", ch->pcdata->pwd );
        if ( ch->pcdata->bamfin[0] != '\0' )
            fprintf ( fp, "Bin %s~\n", ch->pcdata->bamfin );
        if ( ch->pcdata->bamfout[0] != '\0' )
            fprintf ( fp, "Bout %s~\n", ch->pcdata->bamfout );
        if ( ch->pcdata->who_descr[0] != '\0' )
            fprintf ( fp, "Whod %s~\n", fix_string ( ch->pcdata->who_descr ) );
        fprintf ( fp, "Titl %s~\n", ch->pcdata->title );
        fprintf ( fp, "Pnts %d\n", ch->pcdata->points );
        fprintf ( fp, "TSex %d\n", ch->pcdata->true_sex );
        fprintf ( fp, "Pkdt %ld\n", ch->pcdata->pkdeaths );
        fprintf ( fp, "Pkkl %ld\n", ch->pcdata->pkkills );
        fprintf ( fp, "LLev %d\n", ch->pcdata->last_level );
        fprintf ( fp, "Reca %d\n", ch->pcdata->recall );
        fprintf ( fp, "HMVP %ld %ld %ld\n", ch->pcdata->perm_hit,
                  ch->pcdata->perm_mana, ch->pcdata->perm_move );
        fprintf ( fp, "Alos %d\n", ch->pcdata->alosses );
        fprintf ( fp, "Awin %d\n", ch->pcdata->awins );
        fprintf ( fp, "Tdth %d\n", ch->pcdata->tdeaths );
        fprintf ( fp, "Tkll %d\n", ch->pcdata->tkills );
        fprintf ( fp, "Twin %d\n", ch->pcdata->twins );
        fprintf ( fp, "Cnd %d %d %d %d\n", ch->pcdata->condition[0],
                  ch->pcdata->condition[1], ch->pcdata->condition[2],
                  ch->pcdata->condition[3] );

        /* write forgets */
        for ( pos = 0; pos < MAX_FORGET; pos++ )
        {
            if ( ch->pcdata->forget[pos] == NULL )
                break;

            fprintf ( fp, "Forge %s~\n", ch->pcdata->forget[pos] );
        }

        /* write dupes */
        for ( pos = 0; pos < MAX_DUPES; pos++ )
        {
            if ( ch->pcdata->dupes[pos] == NULL )
                break;

            fprintf ( fp, "Dupes %s~\n", ch->pcdata->dupes[pos] );
        }

        /* write alias */
        for ( pos = 0; pos < MAX_ALIAS; pos++ )
        {
            if ( ch->pcdata->alias[pos] == NULL ||
                 ch->pcdata->alias_sub[pos] == NULL )
                break;

/*            fprintf ( fp, "Alias %s %s~\n", ch->pcdata->alias[pos] ),
                      ch->pcdata->alias_sub[pos] ); */
            fprintf ( fp, "Alias %s ", ch->pcdata->alias[pos] ); 
	    fprintf ( fp, "%s~\n", ch->pcdata->alias_sub[pos] );
        }

	/* Save last hosts - Rhaelar 12'2004 */
        for (pos = 0; pos < MAX_HOST; pos++)
	{
	    if (ch->pcdata->lasthost[pos] == NULL)
		break;
	    fprintf(fp,"Lasthost %s\n", ch->pcdata->lasthost[pos]);
	}

//	fprintf (fp, "Firsthost %s~\n", ch->pcdata->firsthost );
	/* Save note board status */
	/* Save number of boards in case that number changes */
	fprintf (fp, "Boards %d ", MAX_BOARD);
	for (i = 0; i < MAX_BOARD; i++)
	    fprintf (fp, "%s %ld ", boards[i].short_name, ch->pcdata->last_note[i]);
	fprintf (fp, "\n");
		
        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0 )
            {
                fprintf ( fp, "Sk %d '%s'\n", ch->pcdata->learned[sn],
                          skill_table[sn].name );
            }
        }

        for ( gn = 0; gn < MAX_GROUP; gn++ )
        {
            if ( group_table[gn].name != NULL && ch->pcdata->group_known[gn] )
            {
                fprintf ( fp, "Gr '%s'\n", group_table[gn].name );
            }
        }
    }

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
        if ( paf->type < 0 || paf->type >= MAX_SKILL )
            continue;

        fprintf ( fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
                  skill_table[paf->type].name, paf->where, paf->level,
                  paf->duration, paf->modifier, paf->location,
                  paf->bitvector );
    }
    fwrite_rle ( ch->pcdata->explored, fp );
    fprintf ( fp, "End\n\n" );
    return;
}

/* write a pet */
void fwrite_pet ( CHAR_DATA * pet, FILE * fp )
{
    AFFECT_DATA *paf;
    char debug[MSL];

    if ( debug_mode )
    {
	sprintf ( debug, "Now saving pet '%s'", pet->name );
	debug_log ( debug );    
    }
    
    fprintf ( fp, "#PET\n" );

    fprintf ( fp, "Vnum %ld\n", pet->pIndexData->vnum );

    fprintf ( fp, "Name %s~\n", pet->name );
    fprintf ( fp, "LogO %ld\n", current_time );
    if ( pet->short_descr != pet->pIndexData->short_descr )
        fprintf ( fp, "ShD  %s~\n", pet->short_descr );
    if ( pet->long_descr != pet->pIndexData->long_descr )
        fprintf ( fp, "LnD  %s~\n", pet->long_descr );
    if ( pet->description != pet->pIndexData->description )
        fprintf ( fp, "Desc %s~\n", pet->description );
    if ( pet->race != pet->pIndexData->race )
        fprintf ( fp, "Race %s~\n", race_table[pet->race].name );
    if ( pet->clan )
        fprintf ( fp, "Clan %s~\n", clan_table[pet->clan].name );
    fprintf ( fp, "Sex  %d\n", pet->sex );

	fprintf( fp, "PetStance  %d %d %d %d %d %d %d %d %d %d %d\n",
	pet->stance[0], pet->stance[1], pet->stance[2], pet->stance[3],
	pet->stance[4], pet->stance[5], pet->stance[6], pet->stance[7],
	pet->stance[8], pet->stance[9], pet->stance[10] );    
    
    if ( pet->level != pet->pIndexData->level )
        fprintf ( fp, "Levl %d\n", pet->level );
    fprintf ( fp, "HMV  %ld %ld %ld %ld %ld %ld\n", pet->hit, pet->max_hit,
              pet->mana, pet->max_mana, pet->move, pet->max_move );
    if ( pet->platinum > 0 )
        fprintf ( fp, "Plat %ld\n", pet->platinum );
    if ( pet->gold > 0 )
        fprintf ( fp, "Gold %ld\n", pet->gold );
    if ( pet->silver > 0 )
        fprintf ( fp, "Silv %ld\n", pet->silver );
    if ( pet->exp > 0 )
        fprintf ( fp, "Exp  %ld\n", pet->exp );
    if ( pet->act != pet->pIndexData->act )
        fprintf ( fp, "Act  %s\n", print_flags ( pet->act ) );
    if ( pet->act2 != pet->pIndexData->act2 )
        fprintf ( fp, "Act2  %s\n", print_flags ( pet->act2 ) );
    if ( pet->affected_by != pet->pIndexData->affected_by )
        fprintf ( fp, "AfBy %s\n", print_flags ( pet->affected_by ) );
    if ( pet->shielded_by != pet->pIndexData->shielded_by )
        fprintf ( fp, "ShBy %s\n", print_flags ( pet->shielded_by ) );
    if ( pet->comm != 0 )
        fprintf ( fp, "Comm %s\n", print_flags ( pet->comm ) );
    fprintf ( fp, "Pos  %d\n", pet->position =
              POS_FIGHTING ? POS_STANDING : pet->position );
    if ( pet->saving_throw != 0 )
        fprintf ( fp, "Save %d\n", pet->saving_throw );
    if ( pet->alignment != pet->pIndexData->alignment )
        fprintf ( fp, "Alig %d\n", pet->alignment );
    if ( pet->hitroll != pet->pIndexData->hitroll )
        fprintf ( fp, "Hit  %d\n", pet->hitroll );
    if ( pet->damroll != pet->pIndexData->damage[DICE_BONUS] )
        fprintf ( fp, "Dam  %d\n", pet->damroll );
    fprintf ( fp, "ACs  %d %d %d %d\n", pet->armor[0], pet->armor[1],
              pet->armor[2], pet->armor[3] );
    fprintf ( fp, "Attr %d %d %d %d %d\n", pet->perm_stat[STAT_STR],
              pet->perm_stat[STAT_INT], pet->perm_stat[STAT_WIS],
              pet->perm_stat[STAT_DEX], pet->perm_stat[STAT_CON] );
    fprintf ( fp, "AMod %d %d %d %d %d\n", pet->mod_stat[STAT_STR],
              pet->mod_stat[STAT_INT], pet->mod_stat[STAT_WIS],
              pet->mod_stat[STAT_DEX], pet->mod_stat[STAT_CON] );

    for ( paf = pet->affected; paf != NULL; paf = paf->next )
    {
        if ( paf->type < 0 || paf->type >= MAX_SKILL )
            continue;

        fprintf ( fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
                  skill_table[paf->type].name, paf->where, paf->level,
                  paf->duration, paf->modifier, paf->location,
                  paf->bitvector );
    }

    fprintf ( fp, "End\n" );
    return;
}

/*
 * Write an object and its contents.
 */
void fwrite_obj ( CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest )
{
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;
    char debug[MSL];

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */    
    if ( obj->next_content != NULL )
    {
    	if ( debug_mode )
	{
	   sprintf ( debug, "Recursing obj '%s'", obj->name );
	   debug_log ( debug );
	}
        fwrite_obj ( ch, obj->next_content, fp, iNest );
    }

    /*
     * Castrate storage characters.
     */
/*
      if ( ( ch->level < obj->level - 2 && obj->level > 19 &&
           ( obj->item_type != ITEM_CONTAINER &&
             obj->item_type != ITEM_PIT ) ) || obj->item_type == ITEM_KEY ||
         ( obj->item_type == ITEM_MAP && !obj->value[0] ) )
        return;
*/
	if ( debug_mode )
	{
	   sprintf ( debug, "Now saving obj '%s'", obj->name );
	   debug_log ( debug );
	}

    fprintf ( fp, "#O\n" );
    fprintf ( fp, "Vnum %ld\n", obj->pIndexData->vnum );
    if ( !obj->pIndexData->new_format )
        fprintf ( fp, "Oldstyle\n" );
    if ( obj->enchanted )
        fprintf ( fp, "Enchanted\n" );
    fprintf ( fp, "Nest %d\n", iNest );

    /* these data are only used if they do not match the defaults */

    if ( obj->name != obj->pIndexData->name )
        fprintf ( fp, "Name %s~\n", obj->name );
    if ( obj->short_descr != obj->pIndexData->short_descr )
        fprintf ( fp, "ShD  %s~\n", obj->short_descr );
    if ( obj->description != obj->pIndexData->description )
        fprintf ( fp, "Desc %s~\n", obj->description );
    if ( obj->extra_flags != obj->pIndexData->extra_flags )
        fprintf ( fp, "ExtF %ld\n", obj->extra_flags );
    if ( obj->wear_flags != obj->pIndexData->wear_flags )
        fprintf ( fp, "WeaF %d\n", obj->wear_flags );
    if ( obj->item_type != obj->pIndexData->item_type )
        fprintf ( fp, "Ityp %d\n", obj->item_type );
    if ( obj->weight != obj->pIndexData->weight )
        fprintf ( fp, "Wt   %d\n", obj->weight );
    if ( obj->condition != obj->pIndexData->condition )
        fprintf ( fp, "Cond %d\n", obj->condition );

    /* variable data */

    fprintf ( fp, "Wear %d\n", obj->wear_loc );
    if ( obj->level != obj->pIndexData->level )
        fprintf ( fp, "Lev  %d\n", obj->level );
    if ( obj->timer != 0 )
        fprintf ( fp, "Time %d\n", obj->timer );
    fprintf ( fp, "Cost %d\n", obj->cost );
    if ( obj->value[0] != obj->pIndexData->value[0] ||
         obj->value[1] != obj->pIndexData->value[1] ||
         obj->value[2] != obj->pIndexData->value[2] ||
         obj->value[3] != obj->pIndexData->value[3] ||
         obj->value[4] != obj->pIndexData->value[4] )
        fprintf ( fp, "Val  %ld %ld %ld %ld %ld\n", obj->value[0],
                  obj->value[1], obj->value[2], obj->value[3],
                  obj->value[4] );

    switch ( obj->item_type )
    {
        case ITEM_POTION:
        case ITEM_SCROLL:
	if ( debug_mode )
	{
	   sprintf ( debug, "Now saving scroll affects for obj '%s'", obj->name );
	   debug_log ( debug );
	}

            if ( obj->value[1] > 0 )
            {
                fprintf ( fp, "Spell 1 '%s'\n",
                          skill_table[obj->value[1]].name );
            }

            if ( obj->value[2] > 0 )
            {
                fprintf ( fp, "Spell 2 '%s'\n",
                          skill_table[obj->value[2]].name );
            }

            if ( obj->value[3] > 0 )
            {
                fprintf ( fp, "Spell 3 '%s'\n",
                          skill_table[obj->value[3]].name );
            }

            break;

        case ITEM_PILL:
        case ITEM_STAFF:
        case ITEM_WAND:
	if ( debug_mode )
	{
	   sprintf ( debug, "Now saving wand affects for obj '%s'", obj->name );
	   debug_log ( debug );
	}
            
	    if ( obj->value[3] > 0 )
            {
                fprintf ( fp, "Spell 3 '%s'\n",
                          skill_table[obj->value[3]].name );
            }

            break;
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        if ( paf->type < 0 || paf->type >= MAX_SKILL )
            continue;
	if ( debug_mode )
	{
	   sprintf ( debug, "Now saving affects on obj '%s'", obj->name );
	   debug_log ( debug );
	}

        fprintf ( fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
                  skill_table[paf->type].name, paf->where, paf->level,
                  paf->duration, paf->modifier, paf->location,
                  paf->bitvector );
    }

    for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
    {
	if ( debug_mode )
	{
	   sprintf ( debug, "Now saving extra desc on obj '%s'", obj->name );
	   debug_log ( debug );
	}    
        fprintf ( fp, "ExDe %s~ .%s~\n", ed->keyword, ed->description );
    }
/*
    if(!ch && obj->in_room)
    {
    	if ( debug_mode )
	{
	   sprintf ( debug, "Now saving inroom for obj '%s'", obj->name );
	   debug_log ( debug );
	}
        fprintf(fp, "Inrm %d\n", obj->in_room->vnum );
    }*/
	if ( debug_mode )
	{
	   sprintf ( debug, "Now ending obj '%s'", obj->name );
	   debug_log ( debug );
	}

    fprintf ( fp, "End\n\n" );

    if ( obj->contains != NULL )
    {
    	if ( debug_mode )
	{
	   sprintf ( debug, "Saving contains of obj '%s'", obj->name );
	   debug_log ( debug );
	}
        fwrite_obj ( ch, obj->contains, fp, iNest + 1 );
    }
    tail_chain (  );
    return;
}

/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj ( DESCRIPTOR_DATA * d, char *name )
{
    char strsave[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *ch;
    FILE *fp=NULL;
    bool found;
    int stat;

    ch = new_char (  );
    ch->pcdata = new_pcdata (  );

    d->character = ch;
    ch->desc = d;
    ch->name = str_dup ( name );
    ch->pcdata->cname = str_dup ( name );
    ch->id = get_pc_id (  );
    ch->race = race_lookup ( "human" );
    ch->act = PLR_NOSUMMON;
    ch->comm = COMM_COMBINE | COMM_PROMPT | COMM_STORE;
    ch->hometown = 0;
    ch->pcdata->ctier = TIER_01;
    ch->prompt = str_dup ( "<{R%hhp {M%mm {G%vmv{x>" );
    ch->pcdata->confirm_delete = FALSE;
    ch->pcdata->confirm_reroll = FALSE;
    ch->pcdata->board = &boards[DEFAULT_BOARD];
    ch->pcdata->pwd = str_dup ( "" );
    ch->pcdata->bamfin = str_dup ( "" );
    ch->pcdata->bamfout = str_dup ( "" );
    ch->pcdata->who_descr = str_dup ( "" );
    ch->pcdata->title = str_dup ( "" );
    ch->pcdata->spouse = str_dup ( "" );
//    ch->pcdata->rmessage = str_dup ( "" );
    ch->pcdata->tier = 0;
    ch->pcdata->verbose = VERBOSE_SHIELD_COMP;
    for ( stat = 0; stat < MAX_STATS; stat++ )
        ch->perm_stat[stat] = 13;

    ch->pcdata->condition[COND_THIRST] = 48;
    ch->pcdata->condition[COND_FULL] = 48;
    ch->pcdata->condition[COND_HUNGER] = 48;
    ch->pcdata->security = 0;   /* OLC */
    ch->pcdata->pkdeaths = 0;
    ch->pcdata->pkkills = 0;
    ch->pcdata->awins = 0;
    ch->pcdata->alosses = 0;
    ch->pcdata->twins = 0;
    ch->pcdata->tdeaths = 0;
    ch->pcdata->tkills = 0;
    ch->pcdata->tracking = NULL;
    ch->pcdata->track_count = 0;
    ch->pcdata->times_rerolled = 0;
    ch->pcdata->timesplayed = 0;
    ch->pcdata->mkill = 0;
    ch->pcdata->mdeath = 0;
    ch->pcdata->exiletime = 0;
    ch->pcdata->noclantime = 0;
    ch->pcdata->pkviolenttime = 0;
    ch->pcdata->pksafetime = 0;
    ch->pcdata->rerolltimer = 0;
    ch->pcdata->assassinatetimer = 0;
    ch->pcdata->jailtime = 0;
    ch->pcdata->nextquest = 0;
    ch->pcdata->countdown = 0;
    ch->pcdata->pkset = 0;
    ch->pcdata->shares = 0;
    ch->pcdata->plr_wager = 0;
    ch->pcdata->questobj = 0;
    ch->pcdata->questmob = 0;
    ch->pcdata->questgiver = 0;
    ch->pcdata->questloc = 0;
    ch->pcdata->questpoints = 0;
    ch->pcdata->tag_flags = 0;
    ch->pcdata->in_progress = 0;
    ch->pcdata->fake_stance = 0;
    ch->pcdata->recall = 0;
    ch->pcdata->last_level = 0;
    ch->pcdata->true_sex = 0;
    ch->pcdata->valid = 0;
    ch->pcdata->perm_hit = 0;
    ch->pcdata->perm_mana = 0;
    ch->pcdata->perm_move = 0;
    ch->pcdata->points = 0;
    ch->pcdata->balance = 0;
    ch->pcdata->pkvalue = 0;
    
    bzero ( ch->pcdata->lasthost, MAX_HOST );
    bzero ( ch->pcdata->last_note, MAX_BOARD );
    bzero ( ch->pcdata->explored, MAX_EXPLORE );
    bzero ( ch->pcdata->forget, MAX_FORGET );
    bzero ( ch->pcdata->alias, MAX_ALIAS );
    bzero ( ch->pcdata->alias_sub, MAX_ALIAS );
    bzero ( ch->pcdata->dupes, MAX_DUPES );
    bzero ( ch->pcdata->group_known, MAX_GROUP );
    found = FALSE;

#if defined(unix)
    /* decompress if .gz file exists */
    sprintf ( strsave, "%s%s%s", PLAYER_DIR, capitalize ( name ), ".gz" );
    if ( ( fp = file_open ( strsave, "r" ) ) != NULL )
    {
        new_fclose ( fp );
        sprintf ( buf, "gzip -dfq %s", strsave );
        system ( buf );
    }
#endif

    sprintf ( strsave, "%s%s", PLAYER_DIR, capitalize ( name ) );
    if ( ( fp = file_open ( strsave, "r" ) ) != NULL )
    {
        int iNest;

        AttemptJump = true;

                if (setjmp(jump_env) == 1)
                {
                        size_t iLine, iChar;
                        char c;

                        iChar = ftell(fp);
                        fseek(fp, 0, SEEK_SET);
                        for (iLine = 0; (size_t) ftell(fp) < iChar; iLine++)
                        {
                                while ((c = fgetc(fp)) != '\n' && c != EOF)
                                        ;
                        }
                        fseek(fp, iChar, SEEK_SET);

                        bugf("[*PFILE*] Error in file '%s' at line %d", capitalize(name),
                                 iLine);

			wiznet ( "Please report this to Rhaelar, if he's online, tell him to get off his lazy ass and fix it!", NULL, NULL, WIZ_BUGS, 0, 0 );
                        new_fclose(fp);
                        return found;
                }

        for ( iNest = 0; iNest < MAX_NEST; iNest++ )
            rgObjNest[iNest] = NULL;

        found = TRUE;
        for ( ;; )
        {
            char letter;
            char *word;

            letter = fread_letter ( fp );
            if ( letter == '*' )
            {
                fread_to_eol ( fp );
                continue;
            }

            if ( letter != '#' )
            {
                bug ( "Load_char_obj: # not found.", 0 );
                break;
            }

            word = fread_word ( fp );
            if ( !str_cmp ( word, "PLAYER" ) )
                fread_char ( ch, fp );
            else if ( !str_cmp ( word, "OBJECT" ) )
                fread_obj ( ch, fp );
            else if ( !str_cmp ( word, "O" ) )
                fread_obj ( ch, fp );
            else if ( !str_cmp ( word, "PET" ) )
                fread_pet ( ch, fp );
            else if ( !str_cmp ( word, "END" ) )
                break;
            else
            {
                bug ( "Load_char_obj: bad section.", 0 );
                break;
            }
        }
        new_fclose ( fp );
    }

    AttemptJump = false;

    /* initialize race */
    if ( found )
    {
        int i;

        if ( ch->race == 0 )
            ch->race = race_lookup ( "crystal-dragon" );

        ch->size = pc_race_table[ch->race].size;
        ch->dam_type = 17;      /*punch */

        for ( i = 0; i < 5; i++ )
        {
            if ( pc_race_table[ch->race].skills[i] == NULL )
                break;
            group_add ( ch, pc_race_table[ch->race].skills[i], FALSE );
        }
        ch->affected_by = ch->affected_by | race_table[ch->race].aff;
        ch->shielded_by = ch->shielded_by | race_table[ch->race].shd;
        ch->imm_flags = ch->imm_flags | race_table[ch->race].imm;
        ch->res_flags = ch->res_flags | race_table[ch->race].res;
        ch->vuln_flags = ch->vuln_flags | race_table[ch->race].vuln;
        ch->form = race_table[ch->race].form;
        ch->parts = race_table[ch->race].parts;
/*	if (gquest_info.running == GQUEST_OFF && IS_SET(ch->act, PLR_GQUEST))
	{
	    REMOVE_BIT(ch->act, PLR_GQUEST);
	    reset_gqmob(ch, 0);
	}
*/

        /* ream gold */
        if ( found && ch->version < 4 )
            ch->gold /= 100;

        if ( ch->version < 6 )
            if ( IS_SET ( ch->act, PLR_NOCOLOUR ) && d )
                d->ansi = FALSE;
    }
/*    REMOVE_BIT ( ch->comm, COMM_AFK ); */
    REMOVE_BIT ( ch->act2, PLR2_PKVIOLENT );
    REMOVE_BIT ( ch->act2, PLR2_PKSAFE );
    REMOVE_BIT ( ch->act, PLR_NORESTORE );
/*    REMOVE_BIT ( ch->act, ACT_QUESTOR ); */
/*        end_quest(ch, 0); */ /* Omg, how could I have overlooked this for so long... waaaaah */

    if ( found && !IS_SET ( ch->pcdata->verbose, VERBOSE_SHIELD ) &&
         !IS_SET ( ch->pcdata->verbose, VERBOSE_SHIELD_COMP ) )
        SET_BIT ( ch->pcdata->verbose, VERBOSE_SHIELD_COMP );
    return found;
}

/*
bool load_char_reroll( DESCRIPTOR_DATA *d, char *name )
{
    CHAR_DATA *ch;
    bool found;
    int stat;

    ch = new_char();
    ch->pcdata = new_pcdata();

    d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( name );
    ch->id				= get_pc_id();
    ch->race				= race_lookup("human");
    ch->act				= PLR_NOSUMMON;
    ch->comm				= COMM_COMBINE 
					| COMM_PROMPT
					| COMM_STORE;
    ch->prompt 				= str_dup("<{R%hhp {M%mm {G%vmv{x>");
    ch->pcdata->confirm_delete		= FALSE;
    ch->pcdata->pwd			= str_dup( "" );
    ch->pcdata->bamfin			= str_dup( "" );
    ch->pcdata->bamfout			= str_dup( "" );
    ch->pcdata->who_descr		= str_dup( "" );
    ch->pcdata->title			= str_dup( "" );
    ch->pcdata->tier			= 1;
    for (stat =0; stat < MAX_STATS; stat++)
	ch->perm_stat[stat]		= 13;
    ch->pcdata->condition[COND_THIRST]	= 48; 
    ch->pcdata->condition[COND_FULL]	= 48;
    ch->pcdata->condition[COND_HUNGER]	= 48;
    ch->pcdata->pkdeaths               = 0;
    ch->pcdata->pkkills                  = 0;

    found = FALSE;
    fclose( fpReserve );
    return found;
}*/

/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

#ifdef KEYS
#undef KEYS
#endif

#define KEYS( literal, field, value )		\
		if(!str_cmp(word, literal))	\
		{				\
			_free_string(field, __FILE__, __LINE__);	\
			field = value;		\
			fMatch = TRUE;		\
			break;			\
		}

void fread_char ( CHAR_DATA * ch, FILE * fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    int count = 0;
    int dcount = 0;
    int fcount = 0;
    int hcount = 0;
    int lastlogoff = current_time;
    int percent;

    sprintf ( buf, "Loading %s.", ch->name );
//    wiznet ( buf, NULL, NULL, WIZ_SECURE, 0, 0 );
    if ( str_cmp ( ch->name, "" ) )
    {
        log_string ( buf );
    }

    for ( ;; )
    {
        word = feof ( fp ) ? "End" : fread_word ( fp );
        fMatch = FALSE;

        switch ( UPPER ( word[0] ) )
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol ( fp );
                break;

            case 'A':
                KEY ( "Act", ch->act, fread_flag ( fp ) );
                KEY ( "Act2", ch->act2, fread_flag ( fp ) );
                KEY ( "AffectedBy", ch->affected_by, fread_flag ( fp ) );
                KEY ( "AfBy", ch->affected_by, fread_flag ( fp ) );
                KEY ( "Alignment", ch->alignment, fread_number ( fp ) );
                KEY ( "Alig", ch->alignment, fread_number ( fp ) );
                KEY ( "Awin", ch->pcdata->awins, fread_number ( fp ) );
                KEY ( "Alos", ch->pcdata->alosses, fread_number ( fp ) );
                KEY ( "ATimer", ch->pcdata->assassinatetimer, fread_number ( fp ) );
		if ( !str_cmp ( word, "Alia" ) )
                {
                    if ( count >= MAX_ALIAS )
                    {
                        fread_to_eol ( fp );
                        fMatch = TRUE;
                        break;
                    }

                    ch->pcdata->alias[count] = str_dup ( fread_word ( fp ) );
                    ch->pcdata->alias_sub[count] =
                        str_dup ( fread_word ( fp ) );
                    count++;
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "Alias" ) )
                {
                    if ( count >= MAX_ALIAS )
                    {
                        fread_to_eol ( fp );
                        fMatch = TRUE;
                        break;
                    }

                    ch->pcdata->alias[count] = str_dup ( fread_word ( fp ) );
                    ch->pcdata->alias_sub[count] = fread_string ( fp );
                    count++;
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "AC" ) || !str_cmp ( word, "Armor" ) )
                {
                    fread_to_eol ( fp );
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "ACs" ) )
                {
                    int i;

                    for ( i = 0; i < 4; i++ )
                        ch->armor[i] = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "AffD" ) )
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect (  );

                    sn = skill_lookup ( fread_word ( fp ) );
                    if ( sn < 0 )
                        bug ( "Fread_char: unknown skill.", 0 );
                    else
                        paf->type = sn;

                    paf->level = fread_number ( fp );
                    paf->duration = fread_number ( fp );
                    paf->modifier = fread_number ( fp );
                    paf->location = fread_number ( fp );
                    paf->bitvector = fread_long ( fp );
                    paf->next = ch->affected;
                    ch->affected = paf;
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "Affc" ) )
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect (  );

                    sn = skill_lookup ( fread_word ( fp ) );
                    if ( sn < 0 )
                        bug ( "Fread_char: unknown skill.", 0 );
                    else
                        paf->type = sn;

                    paf->where = fread_number ( fp );
                    paf->level = fread_number ( fp );
                    paf->duration = fread_number ( fp );
                    paf->modifier = fread_number ( fp );
                    paf->location = fread_number ( fp );
                    paf->bitvector = fread_long ( fp );
                    paf->next = ch->affected;
                    ch->affected = paf;
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "AttrMod" ) ||
                     !str_cmp ( word, "AMod" ) )
                {
                    int stat;
                    for ( stat = 0; stat < MAX_STATS; stat++ )
                        ch->mod_stat[stat] = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "AttrPerm" ) ||
                     !str_cmp ( word, "Attr" ) )
                {
                    int stat;

                    for ( stat = 0; stat < MAX_STATS; stat++ )
                        ch->perm_stat[stat] = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'B':
                KEYS ( "Bamfin", ch->pcdata->bamfin, fread_string ( fp ) );
                KEYS ( "Bamfout", ch->pcdata->bamfout, fread_string ( fp ) );
                KEYS ( "Bin", ch->pcdata->bamfin, fread_string ( fp ) );
                KEYS ( "Bout", ch->pcdata->bamfout, fread_string ( fp ) );
                KEY ( "Balance", ch->pcdata->balance, fread_number ( fp ) );

	    /* Read in board status */
	    if (!str_cmp(word, "Boards" ))
	    {
		int i,num = fread_number (fp); /* number of boards saved */
                char *boardname;

                for (; num ; num-- ) /* for each of the board saved */
                {
		    boardname = fread_word (fp);
		    i = board_lookup (boardname); /* find board number */

		    if (i == BOARD_NOTFOUND) /* Does board still exist ? */
                    {
			sprintf (buf, "fread_char: %s had unknown board name: %s. Skipped.",
			    ch->name, boardname);
			log_string (buf);
			fread_number (fp); /* read last_note and skip info */
		    }
		    else /* Save it */
			ch->pcdata->last_note[i] = fread_number (fp);
		} /* for */

                fMatch = TRUE;
	      } /* Boards */

                break;

            case 'C':
                if ( !str_cmp ( word, "Clas" ) )
                {
                    char *tmp = fread_string ( fp );
                    ch->class = class_lookup ( tmp );
                    free_string ( tmp );
                    fMatch = TRUE;
                    break;
                }
                KEY ( "Class", ch->class, fread_number ( fp ) );
                KEY ( "Cla", ch->class, fread_number ( fp ) );
                if ( !str_cmp ( word, "Clan" ) )
                {
                    char *tmp = fread_string ( fp );
                    ch->clan = clan_lookup ( tmp );
                    free_string ( tmp );
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp ( word, "Clead" ) )
                {
                    char *tmp = fread_string ( fp );
                    ch->clead = clan_lookup ( tmp );
                    free_string ( tmp );
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp ( word, "CName" ) )
                {
                    ch->pcdata->cname = fread_string( fp );
                    fMatch = TRUE;
                    break;
                }
                KEY ( "CRank", ch->clan_rank, fread_number ( fp ) );
                KEY ( "Coauc", ch->color_auc, fread_number ( fp ) );
                KEY ( "Cocgo", ch->color_cgo, fread_number ( fp ) );
                KEY ( "Cocla", ch->color_cla, fread_number ( fp ) );
                KEY ( "Cocon", ch->color_con, fread_number ( fp ) );
                KEY ( "Codis", ch->color_dis, fread_number ( fp ) );
                KEY ( "Cofig", ch->color_fig, fread_number ( fp ) );
                KEY ( "Cogos", ch->color_gos, fread_number ( fp ) );
                KEY ( "Cogra", ch->color_gra, fread_number ( fp ) );
                KEY ( "Cogte", ch->color_gte, fread_number ( fp ) );
                KEY ( "Coimm", ch->color_imm, fread_number ( fp ) );
                KEY ( "Comob", ch->color_mob, fread_number ( fp ) );
                KEY ( "Comus", ch->color_mus, fread_number ( fp ) );
                KEY ( "Coopp", ch->color_opp, fread_number ( fp ) );
                KEY ( "Coqgo", ch->color_qgo, fread_number ( fp ) );
                KEY ( "Coque", ch->color_que, fread_number ( fp ) );
                KEY ( "Coquo", ch->color_quo, fread_number ( fp ) );
                KEY ( "Coroo", ch->color_roo, fread_number ( fp ) );
                KEY ( "Cosay", ch->color_say, fread_number ( fp ) );
                KEY ( "Cosho", ch->color_sho, fread_number ( fp ) );
                KEY ( "Cotel", ch->color_tel, fread_number ( fp ) );
                KEY ( "Cowit", ch->color_wit, fread_number ( fp ) );
                KEY ( "Cowiz", ch->color_wiz, fread_number ( fp ) );
                KEY ( "Ctier", ch->pcdata->ctier, fread_number ( fp ) );
		if ( !str_cmp ( word, "Condition" ) ||
                     !str_cmp ( word, "Cond" ) )
                {
                    ch->pcdata->condition[0] = fread_number ( fp );
                    ch->pcdata->condition[1] = fread_number ( fp );
                    ch->pcdata->condition[2] = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp ( word, "Cnd" ) )
                {
                    ch->pcdata->condition[0] = fread_number ( fp );
                    ch->pcdata->condition[1] = fread_number ( fp );
                    ch->pcdata->condition[2] = fread_number ( fp );
                    ch->pcdata->condition[3] = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }
                KEY ( "Comm", ch->comm, fread_flag ( fp ) );

                break;

            case 'D':
                KEY ( "Damroll", ch->damroll, fread_number ( fp ) );
                KEY ( "Dam", ch->damroll, fread_number ( fp ) );
                KEYS ( "Description", ch->description, fread_string ( fp ) );
                KEYS ( "Desc", ch->description, fread_string ( fp ) );
                if ( !str_cmp ( word, "Dupes" ) )
                {
                    if ( dcount >= MAX_DUPES )
                    {
                        fread_to_eol ( fp );
                        fMatch = TRUE;
                        break;
                    }

                    ch->pcdata->dupes[dcount] = fread_string ( fp );
                    dcount++;
                    fMatch = TRUE;
                }
                break;

            case 'E':
                if ( !str_cmp ( word, "End" ) )
                {
                    /* adjust hp mana move up  -- here for speed's sake */
                    percent =
                        ( current_time - lastlogoff ) * 25 / ( 2 * 60 * 60 );

                    percent = UMIN ( percent, 100 );

                    if ( percent > 0 && !IS_AFFECTED ( ch, AFF_POISON ) &&
                         !IS_AFFECTED ( ch, AFF_PLAGUE ) )
                    {
                        ch->hit += ( ch->max_hit - ch->hit ) * percent / 100;
                        ch->mana +=
                            ( ch->max_mana - ch->mana ) * percent / 100;
                        ch->move +=
                            ( ch->max_move - ch->move ) * percent / 100;
                    }
                    return;
                }
		KEY ( "ETime", ch->pcdata->exiletime, fread_number ( fp ) );
                KEY ( "Exp", ch->exp, fread_number ( fp ) );
	        KEY ("Extra", ch->extra, fread_flag (fp));
                break;

            case 'F':
                if ( !str_cmp ( word, "Forge" ) )
                {
                    if ( fcount >= MAX_FORGET )
                    {
                        fread_to_eol ( fp );
                        fMatch = TRUE;
                        break;
                    }

                    ch->pcdata->forget[fcount] = fread_string ( fp );
                    fcount++;
                    fMatch = TRUE;
                }
//		KEYS ( "Firsthost", ch->pcdata->firsthost, fread_string ( fp ) );
                break;

            case 'G':
                KEY ( "Ghos", ch->ghost_level, fread_number ( fp ) );
                KEY ( "Gold", ch->gold, fread_number ( fp ) );
                if ( !str_cmp ( word, "Group" ) || !str_cmp ( word, "Gr" ) )
                {
                    int gn;
                    char *temp;

                    temp = fread_word ( fp );
                    gn = group_lookup ( temp );
                    /* gn    = group_lookup( fread_word( fp ) ); */
                    if ( gn < 0 )
                    {
                        fprintf ( stderr, "%s", temp );
                        bug ( "Fread_char: unknown group. ", 0 );
                    }
                    else
                        gn_add ( ch, gn );
                    fMatch = TRUE;
                }
                /*
                   if (!str_cmp(word, "GQmobs"))
                   {
                   int maxMob = fread_number(fp);
                   int i;

                   for (i = 0; i < maxMob; i++)
                   ch->pcdata->gq_mobs[i] = fread_long(fp);
                   fMatch = TRUE;
                   }      
                 */
                break;

            case 'H':
                KEY ( "Hitroll", ch->hitroll, fread_number ( fp ) );
                KEY ( "Hit", ch->hitroll, fread_number ( fp ) );
	        KEY( "Hmtown", ch->hometown, fread_number( fp ) );
                if ( !str_cmp ( word, "HpManaMove" ) ||
                     !str_cmp ( word, "HMV" ) )
                {
                    ch->hit = fread_number ( fp );
                    ch->max_hit = fread_number ( fp );
                    ch->mana = fread_number ( fp );
                    ch->max_mana = fread_number ( fp );
                    ch->move = fread_number ( fp );
                    ch->max_move = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "HpManaMovePerm" ) ||
                     !str_cmp ( word, "HMVP" ) )
                {
                    ch->pcdata->perm_hit = fread_number ( fp );
                    ch->pcdata->perm_mana = fread_number ( fp );
                    ch->pcdata->perm_move = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }

                break;

            case 'I':
                KEY ( "Id", ch->id, fread_number ( fp ) );
                KEY ( "InvisLevel", ch->invis_level, fread_number ( fp ) );
                KEY ( "Inco", ch->incog_level, fread_number ( fp ) );
                KEY ( "Invi", ch->invis_level, fread_number ( fp ) );
                break;

	    case 'J':
	    	KEY ( "JTime", ch->pcdata->jailtime, fread_number ( fp ) );
		break;
		
            case 'L':
                KEY ( "LastLevel", ch->pcdata->last_level,
                      fread_number ( fp ) );
            
	    if (!str_cmp( word, "Lasthost"))
            {
                if (hcount >= MAX_HOST)
                {
                    fread_to_eol(fp);
                    fMatch = TRUE;
                    break;
                }
                ch->pcdata->lasthost[hcount] = str_dup(fread_word(fp));
                hcount++;
                fMatch = TRUE;
                break;
            }

                KEY ( "LLev", ch->pcdata->last_level, fread_number ( fp ) );
                KEY ( "Level", ch->level, fread_number ( fp ) );
                KEY ( "Lev", ch->level, fread_number ( fp ) );
                KEY ( "Levl", ch->level, fread_number ( fp ) );
                if ( !str_cmp ( word, "LogO" ) )
                {
                    lastlogoff = fread_number ( fp );
                    ch->llogoff = ( time_t ) lastlogoff;
                    fMatch = TRUE;
                    break;
                }
                KEYS ( "LongDescr", ch->long_descr, fread_string ( fp ) );
                KEYS ( "LnD", ch->long_descr, fread_string ( fp ) );
                break;

	    case 'M':
	       if ( !str_cmp( word, "MkMd" ) )
	       {
		   ch->pcdata->mkill	= fread_number( fp );
		   ch->pcdata->mdeath	= fread_number( fp );
	 	   fMatch = TRUE;
	 	   break;
	       }
	       break;

            case 'N':
                KEYS ( "Name", ch->name, fread_string ( fp ) );
/*                KEY ( "Note", ch->pcdata->last_note, fread_number ( fp ) );
                if ( !str_cmp ( word, "Not" ) )
                {
                    ch->pcdata->last_note = fread_number ( fp );
                    ch->pcdata->last_idea = fread_number ( fp );
                    ch->pcdata->last_penalty = fread_number ( fp );
                    ch->pcdata->last_news = fread_number ( fp );
                    ch->pcdata->last_changes = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp ( word, "Notb" ) )
                {
                    ch->pcdata->last_note = fread_number ( fp );
                    ch->pcdata->last_idea = fread_number ( fp );
                    ch->pcdata->last_penalty = fread_number ( fp );
                    ch->pcdata->last_news = fread_number ( fp );
                    ch->pcdata->last_changes = fread_number ( fp );
                    ch->pcdata->last_weddings = fread_number ( fp );
                    ch->pcdata->last_jokes = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }
*/
		KEY ( "NCTime", ch->pcdata->noclantime, fread_number ( fp ) );
                break;

            case 'P':
                KEYS ( "Password", ch->pcdata->pwd, fread_string ( fp ) );
                KEYS ( "Pass", ch->pcdata->pwd, fread_string ( fp ) );
                KEY ( "Plat", ch->platinum, fread_number ( fp ) );
                KEY ( "Played", ch->played, fread_number ( fp ) );
                KEY ( "Plyd", ch->played, fread_number ( fp ) );
                KEY ( "Points", ch->pcdata->points, fread_number ( fp ) );
                KEY ( "Pnts", ch->pcdata->points, fread_number ( fp ) );
                KEY ( "Position", ch->position, fread_number ( fp ) );
                KEY ( "Pos", ch->position, fread_number ( fp ) );
                KEY ( "Practice", ch->practice, fread_number ( fp ) );
                KEY ( "Prac", ch->practice, fread_number ( fp ) );
                KEYS ( "Prompt", ch->prompt, fread_string ( fp ) );
                KEYS ( "Prom", ch->prompt, fread_string ( fp ) );
                KEY ( "Pkdt", ch->pcdata->pkdeaths, fread_number ( fp ) );
                KEY ( "Pkkl", ch->pcdata->pkkills, fread_number ( fp ) );
                break;

            case 'Q':
                KEY ( "QuestPnts", ch->pcdata->questpoints,
                      fread_number ( fp ) );
                KEY ( "QuestNext", ch->pcdata->nextquest,
                      fread_number ( fp ) );
                KEY ( "QuestCount", ch->pcdata->countdown,
                      fread_number ( fp ) );
                KEY ( "QuestLoc", ch->pcdata->questloc, fread_number ( fp ) );
                KEY ( "QuestObj", ch->pcdata->questobj, fread_number ( fp ) );
                KEY ( "QuestGiver", ch->pcdata->questgiver,
                      fread_number ( fp ) );
                KEY ( "QuestMob", ch->pcdata->questmob, fread_number ( fp ) );
                KEY ( "QPS", ch->qps, fread_number ( fp ) );
            case 'R':
                if ( !str_cmp ( word, "Race" ) )
                {
                    char *tmp = fread_string ( fp );
                    ch->race = race_lookup ( tmp );
                    free_string ( tmp );
                    fMatch = TRUE;
                    break;
                }
                KEY ( "Reca", ch->pcdata->recall, fread_number ( fp ) );

                if ( !str_cmp ( word, "Room" ) )
                {
                    ch->in_room = get_room_index ( fread_number ( fp ) );
                    if ( ch->in_room == NULL )
                        ch->in_room = get_room_index ( ROOM_VNUM_LIMBO );
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp ( word, "RoomRLE" ) )
                {
                    fread_rle ( ch->pcdata->explored, fp );
                    fMatch = TRUE;
                    break;
                }
/*                if ( !str_cmp ( word, "RMsg" ) )
                {
                    ch->pcdata->rmessage = fread_string( fp );
                    fMatch = TRUE;
                    break;
                }
*/
		KEY ( "RTimer", ch->pcdata->rerolltimer, fread_number ( fp ) );
                break;

            case 'S':
                KEY ( "SavingThrow", ch->saving_throw, fread_number ( fp ) );
                KEY ( "Save", ch->saving_throw, fread_number ( fp ) );
                KEY ( "Scro", ch->lines, fread_number ( fp ) );
                KEY ( "Sex", ch->sex, fread_number ( fp ) );
                KEY ( "ShBy", ch->shielded_by, fread_flag ( fp ) );
                KEY ( "ShieldedBy", ch->shielded_by, fread_flag ( fp ) );
                KEYS ( "ShortDescr", ch->short_descr, fread_string ( fp ) ); 
                KEYS ( "ShD", ch->short_descr, fread_string ( fp ) ); 
                KEY ( "Sec", ch->pcdata->security, fread_number ( fp ) );   /* OLC */
                KEY ( "Silv", ch->silver, fread_number ( fp ) );
                KEY ( "Shares", ch->pcdata->shares, fread_number ( fp ) );

                if ( !str_cmp ( word, "Skill" ) || !str_cmp ( word, "Sk" ) )
                {
                    int sn;
                    int value;
                    char *temp;

                    value = fread_number ( fp );
                    temp = fread_word ( fp );
                    sn = skill_lookup ( temp );
                    /* sn    = skill_lookup( fread_word( fp ) ); */
                    if ( sn < 0 )
                    {
                        fprintf ( stderr, "%s", temp );
                        bug ( "Fread_char: unknown skill. ", 0 );
                    }
                    else
                        ch->pcdata->learned[sn] = value;
                    fMatch = TRUE;
                }
                KEYS ( "Spouse", ch->pcdata->spouse, fread_string ( fp ) );
		if (!str_cmp( word, "Stance" ) )
            {
                ch->stance[0] = fread_number( fp );
                ch->stance[1] = fread_number( fp );
                ch->stance[2] = fread_number( fp );
                ch->stance[3] = fread_number( fp );
                ch->stance[4] = fread_number( fp );
                ch->stance[5] = fread_number( fp );
                ch->stance[6] = fread_number( fp );
                ch->stance[7] = fread_number( fp );
                ch->stance[8] = fread_number( fp );
                ch->stance[9] = fread_number( fp );
                ch->stance[10] = fread_number( fp );
                fMatch = TRUE;
                break;
            }
		
                break;

            case 'T':
                KEY ( "Tier", ch->pcdata->tier, fread_number ( fp ) );
                KEY ( "Tdth", ch->pcdata->tdeaths, fread_number ( fp ) );
                KEY ( "Tkll", ch->pcdata->tkills, fread_number ( fp ) );
                KEY ( "Twin", ch->pcdata->twins, fread_number ( fp ) );
/*	    KEY( "Tier",	ch->pcdata->tier,	fread_number( fp ) );*/
                KEY ( "TrueSex", ch->pcdata->true_sex, fread_number ( fp ) );
                KEY ( "TSex", ch->pcdata->true_sex, fread_number ( fp ) );
                KEY ( "Trai", ch->train, fread_number ( fp ) );
		KEY ( "TRR", ch->pcdata->times_rerolled, fread_number ( fp ) );
                KEY ( "Trust", ch->trust, fread_number ( fp ) );
                KEY ( "Tru", ch->trust, fread_number ( fp ) );
                KEY ( "Tpld", ch->pcdata->timesplayed, fread_number ( fp ) );

                if ( !str_cmp ( word, "Title" ) || !str_cmp ( word, "Titl" ) )
                {
                    ch->pcdata->title = fread_string ( fp );
                    if ( ch->pcdata->title[0] != '.' &&
                         ch->pcdata->title[0] != ',' &&
                         ch->pcdata->title[0] != '!' &&
                         ch->pcdata->title[0] != '?' )
                    {
                        sprintf ( buf, " %s", ch->pcdata->title );
                        free_string ( ch->pcdata->title );
                        ch->pcdata->title = str_dup ( buf );
                    }
                    fMatch = TRUE;
                    break;
                }

                break;

            case 'V':
                KEY ( "Version", ch->version, fread_number ( fp ) );
                KEY ( "Vers", ch->version, fread_number ( fp ) );
                KEY ( "Verb", ch->pcdata->verbose, fread_flag ( fp ) );
                if ( !str_cmp ( word, "Vnum" ) )
                {
                    ch->pIndexData = get_mob_index ( fread_number ( fp ) );
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'W':
                KEYS ( "Whod", ch->pcdata->who_descr, fread_string ( fp ) );
                KEY ( "Wimpy", ch->wimpy, fread_number ( fp ) );
                KEY ( "Wimp", ch->wimpy, fread_number ( fp ) );
                KEY ( "Wizn", ch->wiznet, fread_flag ( fp ) );
                break;
        }

        if ( !fMatch )
        {
            bug ( "Fread_char: no match.", 0 );
            fread_to_eol ( fp );
        }
    }
}

/* load a pet from the forgotten reaches */
void fread_pet ( CHAR_DATA * ch, FILE * fp )
{
    char *word;
    CHAR_DATA *pet;
    bool fMatch;
    int lastlogoff = current_time;
    int percent;

    /* first entry had BETTER be the vnum or we barf */
    word = feof ( fp ) ? "END" : fread_word ( fp );
    if ( !str_cmp ( word, "Vnum" ) )
    {
        long vnum;

        vnum = fread_long ( fp );
        if ( get_mob_index ( vnum ) == NULL )
        {
            bug ( "Fread_pet: bad vnum %ld.", vnum );
            pet = create_mobile ( get_mob_index ( MOB_VNUM_FIDO ) );
        }
        else
            pet = create_mobile ( get_mob_index ( vnum ) );
    }
    else
    {
        bug ( "Fread_pet: no vnum in file.", 0 );
        pet = create_mobile ( get_mob_index ( MOB_VNUM_FIDO ) );
    }

    for ( ;; )
    {
        word = feof ( fp ) ? "END" : fread_word ( fp );
        fMatch = FALSE;

        switch ( UPPER ( word[0] ) )
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol ( fp );
                break;

            case 'A':
                KEY ( "Act", pet->act, fread_flag ( fp ) );
                KEY ( "Act2", pet->act2, fread_flag ( fp ) );
                KEY ( "AfBy", pet->affected_by, fread_flag ( fp ) );
                KEY ( "Alig", pet->alignment, fread_number ( fp ) );

                if ( !str_cmp ( word, "ACs" ) )
                {
                    int i;

                    for ( i = 0; i < 4; i++ )
                        pet->armor[i] = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "AffD" ) )
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect (  );

                    sn = skill_lookup ( fread_word ( fp ) );
                    if ( sn < 0 )
                        bug ( "Fread_char: unknown skill.", 0 );
                    else
                        paf->type = sn;

                    paf->level = fread_number ( fp );
                    paf->duration = fread_number ( fp );
                    paf->modifier = fread_number ( fp );
                    paf->location = fread_number ( fp );
                    paf->bitvector = fread_long ( fp );
                    paf->next = pet->affected;
                    pet->affected = paf;
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "Affc" ) )
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect (  );

                    sn = skill_lookup ( fread_word ( fp ) );
                    if ( sn < 0 )
                        bug ( "Fread_char: unknown skill.", 0 );
                    else
                        paf->type = sn;

                    paf->where = fread_number ( fp );
                    paf->level = fread_number ( fp );
                    paf->duration = fread_number ( fp );
                    paf->modifier = fread_number ( fp );
                    paf->location = fread_number ( fp );
                    paf->bitvector = fread_long ( fp );
                    paf->next = pet->affected;
                    pet->affected = paf;
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "AMod" ) )
                {
                    int stat;

                    for ( stat = 0; stat < MAX_STATS; stat++ )
                        pet->mod_stat[stat] = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "Attr" ) )
                {
                    int stat;

                    for ( stat = 0; stat < MAX_STATS; stat++ )
                        pet->perm_stat[stat] = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'C':
                if ( !str_cmp ( word, "Clan" ) )
                {
                    char *tmp = fread_string ( fp );
                    pet->clan = clan_lookup ( tmp );
                    free_string ( tmp );
                    fMatch = TRUE;
                    break;
                }
                KEY ( "Comm", pet->comm, fread_flag ( fp ) );
                break;

            case 'D':
                KEY ( "Dam", pet->damroll, fread_number ( fp ) );
                KEYS ( "Desc", pet->description, fread_string ( fp ) );
                break;

            case 'E':
                if ( !str_cmp ( word, "End" ) )
                {
                    pet->leader = ch;
                    pet->master = ch;
                    ch->pet = pet;
                    /* adjust hp mana move up  -- here for speed's sake */
                    percent =
                        ( current_time - lastlogoff ) * 25 / ( 2 * 60 * 60 );

                    if ( percent > 0 && !IS_AFFECTED ( ch, AFF_POISON ) &&
                         !IS_AFFECTED ( ch, AFF_PLAGUE ) )
                    {
                        percent = UMIN ( percent, 100 );
                        pet->hit +=
                            ( pet->max_hit - pet->hit ) * percent / 100;
                        pet->mana +=
                            ( pet->max_mana - pet->mana ) * percent / 100;
                        pet->move +=
                            ( pet->max_move - pet->move ) * percent / 100;
                    }
                    return;
                }
                KEY ( "Exp", pet->exp, fread_number ( fp ) );
                break;

            case 'G':
                KEY ( "Gold", pet->gold, fread_number ( fp ) );
                break;

            case 'H':
                KEY ( "Hit", pet->hitroll, fread_number ( fp ) );

                if ( !str_cmp ( word, "HMV" ) )
                {
                    pet->hit = fread_number ( fp );
                    pet->max_hit = fread_number ( fp );
                    pet->mana = fread_number ( fp );
                    pet->max_mana = fread_number ( fp );
                    pet->move = fread_number ( fp );
                    pet->max_move = fread_number ( fp );
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'L':
                KEY ( "Levl", pet->level, fread_number ( fp ) );
                KEYS ( "LnD", pet->long_descr, fread_string ( fp ) );
                KEY ( "LogO", lastlogoff, fread_number ( fp ) );
                break;

            case 'N':
                KEYS ( "Name", pet->name, fread_string ( fp ) );
                break;

            case 'P':
                KEY ( "Plat", pet->platinum, fread_number ( fp ) );
                KEY ( "Pos", pet->position, fread_number ( fp ) );
		if (!str_cmp( word, "PetStance" ) )
            {
                pet->stance[0] = fread_number( fp );
                pet->stance[1] = fread_number( fp );
                pet->stance[2] = fread_number( fp );
                pet->stance[3] = fread_number( fp );
                pet->stance[4] = fread_number( fp );
                pet->stance[5] = fread_number( fp );
                pet->stance[6] = fread_number( fp );
                pet->stance[7] = fread_number( fp );
                pet->stance[8] = fread_number( fp );
                pet->stance[9] = fread_number( fp );
                pet->stance[10] = fread_number( fp );
                fMatch = TRUE;
                break;
            }
                break;

            case 'R':
                if ( !str_cmp ( word, "Race" ) )
                {
                    char *tmp = fread_string ( fp );
                    pet->race = race_lookup ( tmp );
                    free_string ( tmp );
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'S':
                KEY ( "Save", pet->saving_throw, fread_number ( fp ) );
                KEY ( "Sex", pet->sex, fread_number ( fp ) );
                KEYS ( "ShD", pet->short_descr, fread_string ( fp ) ); 
                KEY ( "ShBy", pet->shielded_by, fread_flag ( fp ) );
                KEY ( "Silv", pet->silver, fread_number ( fp ) );
                break;

                if ( !fMatch )
                {
                    bug ( "Fread_pet: no match.", 0 );
                    fread_to_eol ( fp );
                }

        }
    }
}

void fread_obj ( CHAR_DATA * ch, FILE * fp )
{
    OBJ_DATA *obj;
    char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    bool first;
    bool new_format;            /* to prevent errors */
    bool make_new;              /* update object */

    fVnum = FALSE;
    obj = NULL;
    first = TRUE;               /* used to counter fp offset */
    new_format = FALSE;
    make_new = FALSE;

    word = feof ( fp ) ? "End" : fread_word ( fp );
    if ( !str_cmp ( word, "Vnum" ) )
    {
        long vnum;
        first = FALSE;          /* fp will be in right place */

        vnum = fread_long ( fp );
        if ( get_obj_index ( vnum ) == NULL )
        {
            bug ( "Fread_obj: bad vnum %ld.", vnum );
            obj = create_object ( get_obj_index ( OBJ_VNUM_BAG ), -1 );
            new_format = TRUE;
        }
        else
        {
            obj = create_object ( get_obj_index ( vnum ), -1 );
            new_format = TRUE;
        }

    }

    if ( obj == NULL )          /* either not found or old style */
    {
        obj = new_obj (  );
        obj->name = str_dup ( "" );
        obj->short_descr = str_dup ( "" );
        obj->description = str_dup ( "" );
    }

    fNest = FALSE;
    fVnum = TRUE;
    iNest = 0;

    for ( ;; )
    {
        if ( first )
            first = FALSE;
        else
            word = feof ( fp ) ? "End" : fread_word ( fp );
        fMatch = FALSE;

        switch ( UPPER ( word[0] ) )
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol ( fp );
                break;

            case 'A':
                if ( !str_cmp ( word, "AffD" ) )
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect (  );

                    sn = skill_lookup ( fread_word ( fp ) );
                    if ( sn < 0 )
                        bug ( "Fread_obj: unknown skill.", 0 );
                    else
                        paf->type = sn;

                    paf->level = fread_number ( fp );
                    paf->duration = fread_number ( fp );
                    paf->modifier = fread_number ( fp );
                    paf->location = fread_number ( fp );
                    paf->bitvector = fread_long ( fp );
                    paf->next = obj->affected;
                    obj->affected = paf;
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp ( word, "Affc" ) )
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect (  );

                    sn = skill_lookup ( fread_word ( fp ) );
                    if ( sn < 0 )
                        bug ( "Fread_obj: unknown skill.", 0 );
                    else
                        paf->type = sn;

                    paf->where = fread_number ( fp );
                    paf->level = fread_number ( fp );
                    paf->duration = fread_number ( fp );
                    paf->modifier = fread_number ( fp );
                    paf->location = fread_number ( fp );
                    paf->bitvector = fread_long ( fp );
                    paf->next = obj->affected;
                    obj->affected = paf;
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'C':
                KEY ( "Cond", obj->condition, fread_number ( fp ) );
                KEY ( "Cost", obj->cost, fread_number ( fp ) );
                break;

            case 'D':
                KEYS ( "Description", obj->description, fread_string ( fp ) );
                KEYS ( "Desc", obj->description, fread_string ( fp ) );
                break;

            case 'E':

                if ( !str_cmp ( word, "Enchanted" ) )
                {
                    obj->enchanted = TRUE;
                    fMatch = TRUE;
                    break;
                }

                KEY ( "ExtraFlags", obj->extra_flags, fread_number ( fp ) );
                KEY ( "ExtF", obj->extra_flags, fread_number ( fp ) );

                if ( !str_cmp ( word, "ExtraDescr" ) ||
                     !str_cmp ( word, "ExDe" ) )
                {
                    EXTRA_DESCR_DATA *ed;

                    ed = new_extra_descr (  );

                    ed->keyword = fread_string ( fp );
                    ed->description = fread_string ( fp );
				// chop off the . if it starts with that 
		    if (ed->description[0]=='.')
		    {
			char *temp_string;
			temp_string = str_dup(ed->description);
			free_string(ed->description);
			ed->description= str_dup(temp_string+1);
			free_string(temp_string);
		    }		    
                    ed->next = obj->extra_descr;
                    obj->extra_descr = ed;
                    fMatch = TRUE;
                }

                if ( !str_cmp ( word, "End" ) )
                {
                    if ( !fNest || ( fVnum && obj->pIndexData == NULL ) )
                    {
                        bug ( "Fread_obj: incomplete object.", 0 );
                        free_obj ( obj );
                        return;
                    }
                    else
                    {
                        if ( !fVnum )
                        {
                            free_obj ( obj );
                            obj =
                                create_object ( get_obj_index
                                                ( OBJ_VNUM_DUMMY ), 0 );
                        }

                        if ( !new_format )
                        {
                            obj->next = object_list;
                            object_list = obj;
                            obj->pIndexData->count++;
                        }

                        if ( !obj->pIndexData->new_format &&
                             obj->item_type == ITEM_ARMOR &&
                             obj->value[1] == 0 )
                        {
                            obj->value[1] = obj->value[0];
                            obj->value[2] = obj->value[0];
                        }
                        if ( make_new )
                        {
                            int wear;

                            wear = obj->wear_loc;
                            extract_obj ( obj );

                            obj = create_object ( obj->pIndexData, 0 );
                            obj->wear_loc = wear;
                        }
/*                        if ( iNest == 0 || rgObjNest[iNest] == NULL )
                            obj_to_char ( obj, ch );
                        else
                            obj_to_obj ( obj, rgObjNest[iNest - 1] ); */
if ( iNest == 0 || rgObjNest[iNest] == NULL )
{   if(ch)
		obj_to_char( obj, ch );
	else
		obj_to_room(obj, obj->in_room );
}
else
	obj_to_obj( obj, rgObjNest[iNest-1] );
                        if ( obj->pIndexData->vnum == OBJ_VNUM_QDIAMOND )
                        {
                            ch->platinum++;
                            extract_obj ( obj );
                        }
                        return;
                    }
                }
                break;

            case 'I':
                KEY ( "ItemType", obj->item_type, fread_number ( fp ) );
                KEY ( "Ityp", obj->item_type, fread_number ( fp ) );
/*if(!str_cmp(word, "Inrm" ) )
{   obj->in_room = get_room_index(fread_number(fp) );
	fMatch = TRUE;
	break;
}*/
                break;

            case 'L':
                KEY ( "Level", obj->level, fread_number ( fp ) );
                KEY ( "Lev", obj->level, fread_number ( fp ) );
                break;

            case 'N':
                KEYS ( "Name", obj->name, fread_string ( fp ) );

                if ( !str_cmp ( word, "Nest" ) )
                {
                    iNest = fread_number ( fp );
                    if ( iNest < 0 || iNest >= MAX_NEST )
                    {
                        bug ( "Fread_obj: bad nest %d.", iNest );
                    }
                    else
                    {
                        rgObjNest[iNest] = obj;
                        fNest = TRUE;
                    }
                    fMatch = TRUE;
                }
                break;

            case 'O':
                if ( !str_cmp ( word, "Oldstyle" ) )
                {
                    if ( obj->pIndexData != NULL &&
                         obj->pIndexData->new_format )
                        make_new = TRUE;
                    fMatch = TRUE;
                }
                break;

            case 'S':
                KEYS ( "ShortDescr", obj->short_descr, fread_string ( fp ) );
                KEYS ( "ShD", obj->short_descr, fread_string ( fp ) );

                if ( !str_cmp ( word, "Spell" ) )
                {
                    int iValue;
                    int sn;

                    iValue = fread_number ( fp );
                    sn = skill_lookup ( fread_word ( fp ) );
                    if ( iValue < 0 || iValue > 3 )
                    {
                        bug ( "Fread_obj: bad iValue %d.", iValue );
                    }
                    else if ( sn < 0 )
                    {
                        bug ( "Fread_obj: unknown skill.", 0 );
                    }
                    else
                    {
                        obj->value[iValue] = sn;
                    }
                    fMatch = TRUE;
                    break;
                }

                break;

            case 'T':
                KEY ( "Timer", obj->timer, fread_number ( fp ) );
                KEY ( "Time", obj->timer, fread_number ( fp ) );
                break;

            case 'V':
                /*if ( !str_cmp ( word, "Values" ) ||
                     !str_cmp ( word, "Vals" ) )
                {
                    obj->value[0] = fread_long ( fp );
                    obj->value[1] = fread_long ( fp );
                    obj->value[2] = fread_long ( fp );
                    obj->value[3] = fread_long ( fp );
                    if ( obj->item_type == ITEM_WEAPON && obj->value[0] == 0 )
                        obj->value[0] = obj->pIndexData->value[0];
                    fMatch = TRUE;
                    break;
                }*/

                if ( !str_cmp ( word, "Val" ) )
                {
                    obj->value[0] = fread_long ( fp );
                    obj->value[1] = fread_long ( fp );
                    obj->value[2] = fread_long ( fp );
                    obj->value[3] = fread_long ( fp );
                    obj->value[4] = fread_long ( fp );
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp ( word, "Vnum" ) )
                {
                    long vnum;

                    vnum = fread_long ( fp );
                    if ( ( obj->pIndexData =
                           get_obj_index ( vnum ) ) == NULL )
                        bug ( "Fread_obj: bad vnum %ld.", vnum );
                    else
                        fVnum = TRUE;
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'W':
                KEY ( "WearFlags", obj->wear_flags, fread_number ( fp ) );
                KEY ( "WeaF", obj->wear_flags, fread_number ( fp ) );
//                KEY ( "WearLoc", obj->wear_loc, fread_number ( fp ) );
                KEY ( "Wear", obj->wear_loc, fread_number ( fp ) );
                KEY ( "Weight", obj->weight, fread_number ( fp ) );
                KEY ( "Wt", obj->weight, fread_number ( fp ) );
                break;

        }

        if ( !fMatch )
        {
            bug ( "Fread_obj: no match.", 0 );
            fread_to_eol ( fp );
        }
    }
    if ( obj->pIndexData->vnum == OBJ_VNUM_QDIAMOND )
    {
        ch->platinum++;
        extract_obj ( obj );
    }
}
void backup_char_obj ( CHAR_DATA * ch )
{
    char strbackup[MAX_INPUT_LENGTH];
    FILE *fp=NULL;

    if ( IS_NPC ( ch ) )
        return;

    if ( !IS_VALID ( ch ) )
    {
        bug ( "backup_char_obj: trying to save invalid character", 0 );
        return;
    }

    if ( ch->desc != NULL && ch->desc->original != NULL )
        ch = ch->desc->original;

    sprintf ( strbackup, "%s%s", BACKUP_DIR, capitalize ( ch->name ) );
    if ( ( fp = file_open ( strbackup, "w" ) ) == NULL )
    {
        bug ( "Backup_char_obj: fopen", 0 );
        perror ( strbackup );
    }
    else
    {
        fwrite_char ( ch, fp );
        if ( ch->carrying != NULL )
            fwrite_obj ( ch, ch->carrying, fp, 0 );
        /* save the pets */
        if ( ch->pet != NULL && ch->pet->in_room == ch->in_room )
            fwrite_pet ( ch->pet, fp );
        fprintf ( fp, "#END\n" );
    }
    new_fclose ( fp );
    rename ( TEMP_FILE, strbackup );
    return;
}
