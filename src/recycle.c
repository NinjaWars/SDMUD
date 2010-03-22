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
#include <unistd.h>
#include "sd.h"
#include "recycle.h"


/* stuff for recycling ban structures */
BAN_DATA *ban_free;

BAN_DATA *new_ban ( void )
{
    static BAN_DATA ban_zero;
    BAN_DATA *ban;

    if ( ban_free == NULL )
        ban = alloc_perm ( sizeof ( *ban ) );
    else
    {
        ban = ban_free;
        ban_free = ban_free->next;
    }

    *ban = ban_zero;
    VALIDATE ( ban );
    ban->name = &str_empty[0];
    return ban;
}

void free_ban ( BAN_DATA * ban )
{
    if ( !IS_VALID ( ban ) )
        return;

    free_string ( ban->name );
    INVALIDATE ( ban );

    ban->next = ban_free;
    ban_free = ban;
}

/* stuff for recycling wizlist structures */
WIZ_DATA *wiz_free;
ANC_DATA *anc_free;

WIZ_DATA *new_wiz ( void )
{
    static WIZ_DATA wiz_zero;
    WIZ_DATA *wiz;

    if ( wiz_free == NULL )
        wiz = alloc_perm ( sizeof ( *wiz ) );
    else
    {
        wiz = wiz_free;
        wiz_free = wiz_free->next;
    }

    *wiz = wiz_zero;
    VALIDATE ( wiz );
    wiz->name = &str_empty[0];
    wiz->job = str_dup ( "Not yet assigned." );
    return wiz;
}

ANC_DATA *new_anc ( void )
{
    static ANC_DATA anc_zero;
    ANC_DATA *anc;

    if ( anc_free == NULL )
        anc = alloc_perm ( sizeof ( *anc ) );
    else
    {
        anc = anc_free;
        anc_free = anc_free->next;
    }

    *anc = anc_zero;
    VALIDATE ( anc );
    anc->name = &str_empty[0];
    return anc;
}

void free_wiz ( WIZ_DATA * wiz )
{
    if ( !IS_VALID ( wiz ) )
        return;

    free_string ( wiz->name );
    free_string ( wiz->job );
    INVALIDATE ( wiz );

    wiz->next = wiz_free;
    wiz_free = wiz;
}

void free_anc ( ANC_DATA * anc )
{
    if ( !IS_VALID ( anc ) )
        return;

    free_string ( anc->name );
    INVALIDATE ( anc );

    anc->next = anc_free;
    anc_free = anc;
}

/* stuff for recycling clanlist structures */
CLN_DATA *cln_free;

CLN_DATA *new_cln ( void )
{
    static CLN_DATA cln_zero;
    CLN_DATA *cln;

    if ( cln_free == NULL )
        cln = alloc_perm ( sizeof ( *cln ) );
    else
    {
        cln = cln_free;
        cln_free = cln_free->next;
    }

    *cln = cln_zero;
    VALIDATE ( cln );
    cln->name = &str_empty[0];
    return cln;
}

MBR_DATA *mbr_free;

MBR_DATA *new_mbr ( void )
{
    static MBR_DATA mbr_zero;
    MBR_DATA *mbr;

    if ( mbr_free == NULL )
        mbr = alloc_perm ( sizeof ( *mbr ) );
    else
    {
        mbr = mbr_free;
        mbr_free = mbr_free->next;
    }

    *mbr = mbr_zero;
    VALIDATE ( mbr );
    mbr->name = &str_empty[0];
    return mbr;
}

void free_mbr ( MBR_DATA * mbr )
{
    if ( !IS_VALID ( mbr ) )
        return;

    free_string ( mbr->name );
    INVALIDATE ( mbr );

    mbr->next = mbr_free;
    mbr_free = mbr;
}

/* stuff for recycling descriptors */
DESCRIPTOR_DATA *descriptor_free;

DESCRIPTOR_DATA *new_descriptor ( void )
{
    static DESCRIPTOR_DATA d_zero;
    DESCRIPTOR_DATA *d;

    if ( descriptor_free == NULL )
        d = alloc_perm ( sizeof ( *d ) );
    else
    {
        d = descriptor_free;
        descriptor_free = descriptor_free->next;
    }

    *d = d_zero;
    VALIDATE ( d );

//    d->connected = CON_GET_NAME;
    d->connected = CON_COLOUR_LOGIN;
    d->lookup_status = STATUS_LOOKUP;
    d->showstr_head = NULL;
    d->showstr_point = NULL;
    d->outsize = 2000;
/*    d->outsize = 6000; */ /* Trying to fix bug - Rhaelar */
    d->ansi = TRUE;
    d->outbuf = alloc_mem ( d->outsize );

    return d;
}

void free_descriptor ( DESCRIPTOR_DATA * d )
{
    if ( !IS_VALID ( d ) )
        return;

    free_string ( d->host );
    free_mem ( d->outbuf, d->outsize );
    INVALIDATE ( d );
    d->next = descriptor_free;
    descriptor_free = d;
}

/* stuff for recycling gen_data */
GEN_DATA *gen_data_free;

GEN_DATA *new_gen_data ( void )
{
    static GEN_DATA gen_zero;
    GEN_DATA *gen;

    if ( gen_data_free == NULL )
        gen = alloc_perm ( sizeof ( *gen ) );
    else
    {
        gen = gen_data_free;
        gen_data_free = gen_data_free->next;
    }
    *gen = gen_zero;
    VALIDATE ( gen );
    return gen;
}

void free_gen_data ( GEN_DATA * gen )
{
    if ( !IS_VALID ( gen ) )
        return;

    INVALIDATE ( gen );

    gen->next = gen_data_free;
    gen_data_free = gen;
}

/* stuff for recycling extended descs */
EXTRA_DESCR_DATA *extra_descr_free;

EXTRA_DESCR_DATA *new_extra_descr ( void )
{
    EXTRA_DESCR_DATA *ed;

    if ( extra_descr_free == NULL )
        ed = alloc_perm ( sizeof ( *ed ) );
    else
    {
        ed = extra_descr_free;
        extra_descr_free = extra_descr_free->next;
    }

    ed->keyword = &str_empty[0];
    ed->description = &str_empty[0];
    VALIDATE ( ed );
    return ed;
}

void free_extra_descr ( EXTRA_DESCR_DATA * ed )
{
    if ( !IS_VALID ( ed ) )
        return;

    free_string ( ed->keyword );
    free_string ( ed->description );
    INVALIDATE ( ed );

    ed->next = extra_descr_free;
    extra_descr_free = ed;
}

/* stuff for recycling affects */
AFFECT_DATA *affect_free;

AFFECT_DATA *new_affect ( void )
{
    static AFFECT_DATA af_zero;
    AFFECT_DATA *af;

    if ( affect_free == NULL )
        af = alloc_perm ( sizeof ( *af ) );
    else
    {
        af = affect_free;
        affect_free = affect_free->next;
    }

    *af = af_zero;

    VALIDATE ( af );
    return af;
}

void free_affect ( AFFECT_DATA * af )
{
    if ( !IS_VALID ( af ) )
        return;

    INVALIDATE ( af );
    af->next = affect_free;
    affect_free = af;
}

/* stuff for recycling objects */
OBJ_DATA *obj_free;

OBJ_DATA *new_obj ( void )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;

    if ( obj_free == NULL )
        obj = alloc_perm ( sizeof ( *obj ) );
    else
    {
        obj = obj_free;
        obj_free = obj_free->next;
    }
    *obj = obj_zero;
    VALIDATE ( obj );

    obj->name = &str_empty[0];    
    obj->short_descr = &str_empty[0];    
    obj->material = &str_empty[0];
    obj->description = &str_empty[0];    
        
    return obj;
}

void free_obj ( OBJ_DATA * obj )
{
    AFFECT_DATA *paf, *paf_next;
    EXTRA_DESCR_DATA *ed, *ed_next;

    if ( !IS_VALID ( obj ) )
        return;

    for ( paf = obj->affected; paf != NULL; paf = paf_next )
    {
        paf_next = paf->next;
        free_affect ( paf );
    }
    obj->affected = NULL;

    for ( ed = obj->extra_descr; ed != NULL; ed = ed_next )
    {
        ed_next = ed->next;
        free_extra_descr ( ed );
    }
    obj->extra_descr = NULL;

    /* GODDAMMIT ! */
    /*free_string ( obj->name );
    if ( obj->description[0] != '\0')    
    free_string ( obj->description );
    free_string ( obj->short_descr );*/
    
    free_string ( obj->owner );
    free_string ( obj->killer );
    /*
    if ( obj->material != '\0')    
    free_string ( obj->material );
    */
   
    INVALIDATE ( obj );

    obj->next = obj_free;
    obj_free = obj;
}

/* stuff for recyling characters */
CHAR_DATA *char_free;

CHAR_DATA *new_char ( void )
{
    static CHAR_DATA ch_zero;
    CHAR_DATA *ch;
    int i;

    if ( char_free == NULL )
        ch = alloc_perm ( sizeof ( *ch ) );
    else
    {
        ch = char_free;
        char_free = char_free->next;
    }

    *ch = ch_zero;
    VALIDATE ( ch );
    ch->name = &str_empty[0];
    ch->short_descr = &str_empty[0];
    ch->long_descr = &str_empty[0];
    ch->description = &str_empty[0];
    ch->prompt = &str_empty[0];
    ch->prefix = &str_empty[0];
    ch->die_descr = &str_empty[0];
    ch->say_descr = &str_empty[0];
    ch->material = &str_empty[0];
    ch->logon = current_time;
    ch->lines = PAGELEN;
    for ( i = 0; i < 4; i++ )
        ch->armor[i] = 100;
    ch->position = POS_STANDING;
    ch->hit = 100;
    ch->max_hit = 100;
    ch->mana = 100;
    ch->max_mana = 100;
    ch->move = 100;
    ch->max_move = 100;
    for ( i = 0; i < MAX_STATS; i++ )
    {
        ch->perm_stat[i] = 13;
        ch->mod_stat[i] = 0;
    }

    return ch;
}

void free_char ( CHAR_DATA * ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    if ( !IS_VALID ( ch ) )
        return;

    if ( IS_NPC ( ch ) )
        mobile_count--;

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        extract_obj ( obj );
    }

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
        paf_next = paf->next;
        affect_remove ( ch, paf );
    }

    free_string ( ch->name );
    free_string ( ch->short_descr );
    free_string ( ch->long_descr );
    free_string ( ch->description );
    free_string ( ch->prompt );
    free_string ( ch->prefix );
    
    if ( ch->die_descr[0] != '\0')
    free_string( ch->die_descr );
    if ( ch->say_descr[0] != '\0')
    free_string( ch->say_descr );
    
    /*if ( ch->material != '\0' )
    free_string ( ch->material ); */
    
    free_pcdata ( ch->pcdata );

    ch->next = char_free;
    char_free = ch;

    INVALIDATE ( ch );
    return;
}

PC_DATA *pcdata_free;

PC_DATA *new_pcdata ( void )
{
    int alias;

    static PC_DATA pcdata_zero;
    PC_DATA *pcdata;

    if ( pcdata_free == NULL )
        pcdata = alloc_perm ( sizeof ( *pcdata ) );
    else
    {
        pcdata = pcdata_free;
        pcdata_free = pcdata_free->next;
    }

    *pcdata = pcdata_zero;

    for ( alias = 0; alias < MAX_ALIAS; alias++ )
    {
        pcdata->alias[alias] = NULL;
        pcdata->alias_sub[alias] = NULL;
    }
    for ( alias = 0; alias < MAX_FORGET; alias++ )
    {
        pcdata->forget[alias] = NULL;
    }
    for ( alias = 0; alias < MAX_DUPES; alias++ )
    {
        pcdata->dupes[alias] = NULL;
    }

    //pcdata->buffer = new_buf (  );
    pcdata->buffer = buffer_new ( 1337 );

    VALIDATE ( pcdata );
    return pcdata;
}

void free_pcdata ( PC_DATA * pcdata )
{
//    CHAR_DATA *ch;
    int alias;

    if ( !IS_VALID ( pcdata ) )
        return;

    free_string ( pcdata->pwd );
    free_string ( pcdata->bamfin );
    free_string ( pcdata->bamfout );
    free_string ( pcdata->who_descr );
    free_string ( pcdata->title );
    free_string ( pcdata->socket );
    free_string ( pcdata->cname );
//    free_string ( pcdata->firsthost );
/*  free_string ( pcdata->rmessage ); */

//    free_string ( pcdata->spouse );
    //free_buf ( pcdata->buffer );
    buffer_free ( pcdata->buffer );
/*
    for ( alias = 0; alias < MAX_EXPLORE; alias++ )
    {
        free_string ( pcdata->explored[explored] );
    }
*/
    for ( alias = 0; alias < MAX_ALIAS; alias++ )
    {
        free_string ( pcdata->alias[alias] );
        free_string ( pcdata->alias_sub[alias] );
    }
    for ( alias = 0; alias < MAX_FORGET; alias++ )
    {
        free_string ( pcdata->forget[alias] );
    }
    for ( alias = 0; alias < MAX_DUPES; alias++ )
    {
        free_string ( pcdata->dupes[alias] );
    }
    for ( alias = 0; alias < MAX_HOST; alias++)
    {
        free_string(pcdata->lasthost[alias]);
    }
    
    INVALIDATE ( pcdata );
    pcdata->next = pcdata_free;
    pcdata_free = pcdata;

    return;
}

/* stuff for recycling bounties */
BOUNTY_DATA *bounty_free;

BOUNTY_DATA *new_bounty(void)
{
    static BOUNTY_DATA b_zero;
    BOUNTY_DATA *b;

    if (bounty_free == NULL)
        b = alloc_perm(sizeof(*b));
    else
    {
        b = bounty_free;
        bounty_free = bounty_free->next;
    }

    *b = b_zero;
    VALIDATE(b);
    return b;
}

void free_bounty(BOUNTY_DATA *b)
{
    if (!IS_VALID(b))
        return;

    free_string( b->name );
    INVALIDATE(b);
    b->next = bounty_free;
    bounty_free = b;
}

/* stuff for setting ids */
long last_pc_id;
long last_mob_id;

long get_pc_id ( void )
{
    int val;

    val = ( current_time <= last_pc_id ) ? last_pc_id + 1 : current_time;
    last_pc_id = val;
    return val;
}

long get_mob_id ( void )
{
    last_mob_id++;
    return last_mob_id;
}

MEM_DATA *mem_data_free;

/* procedures and constants needed for buffering */

//BUFFER *buf_free;

MEM_DATA *new_mem_data ( void )
{
    MEM_DATA *memory;

    if ( mem_data_free == NULL )
        memory = alloc_mem ( sizeof ( *memory ) );
    else
    {
        memory = mem_data_free;
        mem_data_free = mem_data_free->next;
    }

    memory->next = NULL;
    memory->id = 0;
    memory->reaction = 0;
    memory->when = 0;
    VALIDATE ( memory );

    return memory;
}

void free_mem_data ( MEM_DATA * memory )
{
    if ( !IS_VALID ( memory ) )
        return;

    memory->next = mem_data_free;
    mem_data_free = memory;
    INVALIDATE ( memory );
}

/* buffer sizes */
/*const int buf_size[MAX_BUF_LIST] = {
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536
};*/

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
/*int get_size ( int val )
{
    int i;

    for ( i = 0; i < MAX_BUF_LIST; i++ )
        if ( buf_size[i] >= val )
        {
            return buf_size[i];
        }

    return -1;
}*/

/*BUFFER *new_buf (  )
{
    BUFFER *buffer;

    if ( buf_free == NULL )
        buffer = alloc_perm ( sizeof ( *buffer ) );
    else
    {
        buffer = buf_free;
        buf_free = buf_free->next;
    }

    buffer->next = NULL;
    buffer->state = BUFFER_SAFE;
    buffer->size = get_size ( BASE_BUF );

    buffer->string = alloc_mem ( buffer->size );
    buffer->string[0] = '\0';
    VALIDATE ( buffer );

    return buffer;
}

BUFFER *new_buf_size ( int size )
{
    BUFFER *buffer;

    if ( buf_free == NULL )
        buffer = alloc_perm ( sizeof ( *buffer ) );
    else
    {
        buffer = buf_free;
        buf_free = buf_free->next;
    }

    buffer->next = NULL;
    buffer->state = BUFFER_SAFE;
    buffer->size = get_size ( size );
    if ( buffer->size == -1 )
    {
        bug ( "new_buf: buffer size %d too large.", size );
        quit ( 1 );
    }
    buffer->string = alloc_mem ( buffer->size );
    buffer->string[0] = '\0';
    VALIDATE ( buffer );

    return buffer;
}


void free_buf ( BUFFER * buffer )
{
    if ( !IS_VALID ( buffer ) )
        return;

    free_mem ( buffer->string, buffer->size );
    buffer->string = NULL;
    buffer->size = 0;
    buffer->state = BUFFER_FREED;
    INVALIDATE ( buffer );

    buffer->next = buf_free;
    buf_free = buffer;
}

bool add_buf ( BUFFER * buffer, char *string )
{
    int len;
    char *oldstr;
    int oldsize;

    oldstr = buffer->string;
    oldsize = buffer->size;

    if ( buffer->state == BUFFER_OVERFLOW )
        return FALSE;

    len = strlen ( buffer->string ) + strlen ( string ) + 1;

    while ( len >= buffer->size )
    {
        buffer->size = get_size ( buffer->size + 1 );
        {
            if ( buffer->size == -1 )
            {
                buffer->size = oldsize;
                buffer->state = BUFFER_OVERFLOW;
                bug ( "buffer overflow past size %d", buffer->size );
                return FALSE;
            }
        }
    }

    if ( buffer->size != oldsize )
    {
        buffer->string = alloc_mem ( buffer->size );

        strcpy ( buffer->string, oldstr );
        free_mem ( oldstr, oldsize );
    }

    strcat ( buffer->string, string );
    return TRUE;
}

void clear_buf ( BUFFER * buffer )
{
    buffer->string[0] = '\0';
    buffer->state = BUFFER_SAFE;
}

char *buf_string ( BUFFER * buffer )
{
    return buffer->string;
}
*/
char *buffer_string ( BUFFER2 * buffer )
{
    return buffer->data;
}

/* stuff for recycling mobprograms */
MPROG_LIST *mprog_free;

MPROG_LIST *new_mprog ( void )
{
    static MPROG_LIST mp_zero;
    MPROG_LIST *mp;

    if ( mprog_free == NULL )
        mp = alloc_perm ( sizeof ( *mp ) );
    else
    {
        mp = mprog_free;
        mprog_free = mprog_free->next;
    }

    *mp = mp_zero;
    mp->vnum = 0;
    mp->trig_type = 0;
    mp->code = &str_empty[0];
    mp->trig_phrase = &str_empty[0];
    VALIDATE ( mp );
    return mp;
}

void free_mprog ( MPROG_LIST * mp )
{
    if ( !IS_VALID ( mp ) )
        return;

    free_string ( mp->code );
    free_string ( mp->trig_phrase );
    INVALIDATE ( mp );
    mp->next = mprog_free;
    mprog_free = mp;
}

HELP_AREA * had_free;

HELP_AREA * new_had ( void )
{
        HELP_AREA * had;
        static  HELP_AREA   zHad;

        if ( had_free )
        {
                had             = had_free;
                had_free        = had_free->next;
        }
        else
                had             = alloc_perm( sizeof( *had ) );

        *had = zHad;

        return had;
}
HELP_DATA * new_help ( void )
{
        HELP_DATA * help;

        if ( help_free )
        {
                help            = help_free;
                help_free       = help_free->next;
        }
        else
                help            = alloc_perm( sizeof( *help ) );

        return help;
}

void free_help(HELP_DATA *help)
{
        free_string(help->keyword);
        free_string(help->text);
        help->next = help_free;
        help_free = help;
}

/* Stuff for recycling imm/auction shit */
AUCTION_DATA *auction_free;
AUCTION_DATA *new_auction ( void )
{
    static AUCTION_DATA auc_zero;
    AUCTION_DATA *auction;

    if ( auction_free == NULL )
        auction = alloc_perm ( sizeof ( *auction ) );
    else
    {
        auction = auction_free;
        auction_free = auction_free->next;
    }

    *auction = auc_zero;
    VALIDATE ( auction );
    return auction;
}

void free_auction ( AUCTION_DATA * auction )
{
    if ( !IS_VALID ( auction ) )
        return;

    auction->current_bid = 0;
    auction->platinum_held = 0;
    auction->high_bidder = NULL;
    auction->item = NULL;
    auction->min_bid = 0;
    auction->owner = NULL;
    auction->status = 0;
    INVALIDATE ( auction );

    auction->next = auction_free;
    auction_free = auction;
}

/* recycles closed descriptors */
void recycle_descriptors()
{
  DESCRIPTOR_DATA *dclose;
  DESCRIPTOR_DATA *dclose_next;
         
  for (dclose = descriptor_list; dclose; dclose = dclose_next)
  {
    dclose_next = dclose->next;
    if (dclose->lookup_status != STATUS_CLOSED) continue;
       
    /*
     * First let's get it out of the descriptor list.
     */
    if ( dclose == descriptor_list )
    {
      descriptor_list = descriptor_list->next;
    }
    else
    {
      DESCRIPTOR_DATA *d;
  
      for (d = descriptor_list; d && d->next != dclose; d = d->next)
        ;
      if (d != NULL)
        d->next = dclose->next;
      else
      {
        bug( "Recycle_descriptors: dclose not found.", 0 );
        continue;
      }
    }

    /*
     * Clear out that memory
     */
/*    free_string( dclose->host );
    free_mem( dclose->outbuf, dclose->outsize ); */
    free_descriptor ( dclose );

    /* 
     * Bye bye mr. Descriptor.
     */
    close ( dclose->descriptor );
       
    /*
     * And then we recycle
     */
/*    dclose->next        = descriptor_free;
    descriptor_free     = dclose; */ /* Already done by free_descriptor */
  }
}

/* recycles used dns arguments */
void recycle_dnss()
{
  DNS_LOOKUP *dns;
  DNS_LOOKUP *dns_next;
   
  for (dns = dns_list; dns; dns = dns_next)
  {
    dns_next = dns->next;
    if (dns->status == 1) continue;  // being used

    if (dns == dns_list)
    {
      dns_list = dns_list->next;
    }
    else
    {
      DNS_LOOKUP *prev;
     
      /* we find the prev dns arg */
      for (prev = dns_list; prev && prev->next != dns; prev = prev->next)
        ;
      if (prev != NULL) 
        prev->next = dns->next;
      else
      {
        bug( "Recycle_dnss: dns not found.", 0 );
        continue;
      } 
      /* recycle */
      dns->next = dns_free;
      dns_free = dns;
    }   
  }  
}
