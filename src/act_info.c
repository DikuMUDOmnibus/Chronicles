/***************************************************************************
 *                                                                         *
 *   .oOOOo.    o                                        o                 *
 *  .O     o   O                             o          O                  *
 *  o          o                                        o                  *
 *  o          O                                        O                  *
 *  o          OoOo.  `OoOo.  .oOo.  'OoOo.  O   .oOo   o   .oOo.  .oOo    *
 *  O          o   o   o      O   o   o   O  o   O      O   OooO'  `Ooo.   *
 *  `o     .o  o   O   O      o   O   O   o  O   o      o   O          O   *
 *   `OoooO'   O   o   o      `OoO'   o   O  o'  `OoO'  Oo  `OoO'  `OoO'   *
 *                                                                         *
 ***************************************************************************
 * - Chronicles Copyright 2001-2003 by Brad Ensley (Orion Elder)           *
 ***************************************************************************
 * - SMAUG 1.4  Copyright 1994, 1995, 1996, 1998 by Derek Snider           *
 * - Merc  2.1  Copyright 1992, 1993 by Michael Chastain, Michael Quan,    *
 *   and Mitchell Tse.                                                     *
 * - DikuMud    Copyright 1990, 1991 by Sebastian Hammer, Michael Seifert, *
 *   Hans-Henrik St�rfeldt, Tom Madsen, and Katja Nyboe.                   *
 ***************************************************************************
 * - Informational module                                                  *
 ***************************************************************************/

#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <crypt.h>
#endif
#include "mud.h"
#include "councils.h"
#include "deity.h"
#include "factions.h"
#include "files.h"
#include "polymorph.h"
#include "quest.h"

/*
 * Needed in the do_ignore function. -Orion
 */
bool	check_parse_name	args( ( char *name, bool newchar ) );
char	*str_replace		args( ( char *search, char *replace, char *body, bool cs ) );

/*
 * Keep players from defeating examine progs -Druid
 * False = do not trigger
 * True = Trigger
 */
bool EXA_prog_trigger = TRUE;

/* Had to add unknowns because someone added new positions and didn't
 * update them.  Just a band-aid till I have time to fix it right.
 * This was found thanks to mud@mini.axcomp.com pointing it out :)
 * --Shaddai
 */

char *	const	where_name	[] =
{
    "[used as light    ] ",
    "[worn on finger   ] ",
    "[worn on finger   ] ",
    "[worn around neck ] ",
    "[worn around neck ] ",
    "[worn on body     ] ",
    "[worn on head     ] ",
    "[worn on legs     ] ",
    "[worn on feet     ] ",
    "[worn on hands    ] ",
    "[worn on arms     ] ",
    "[worn as shield   ] ",
    "[worn about body  ] ",
    "[worn about waist ] ",
    "[worn around wrist] ",
    "[worn around wrist] ",
    "[wielded          ] ",
    "[held             ] ",
    "[dual wielded     ] ",
    "[worn on ears     ] ",
    "[worn on eyes     ] ",
    "[missile wielded  ] ",
    "[worn on back     ] ",
    "[worn over face   ] ",
    "[worn around ankle] ",
    "[worn around ankle] "
};


/*
 * Local functions.
 */
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );
void    show_condition          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];
    bool glowsee = FALSE;

    /* can see glowing invis items in the dark */
    if ( IS_OBJ_STAT(obj, ITEM_GLOW) && IS_OBJ_STAT(obj, ITEM_INVIS)
    &&  !IS_AFFECTED(ch, AFF_TRUESIGHT) && !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
	glowsee = TRUE;

    buf[0] = '\0';
    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "(Invis) "     );
    if (  (IS_AFFECTED(ch, AFF_DETECT_EVIL) || ch->class==CLASS_PALADIN)
	 && IS_OBJ_STAT(obj, ITEM_EVIL)   )   strcat( buf, "(Red Aura) "  );

    if ( ch->class==CLASS_PALADIN
	 && ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Flaming Red) "  );
    if ( ch->class==CLASS_PALADIN
	 && ( !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Flaming Grey) "  );
    if ( ch->class==CLASS_PALADIN
	 && (!IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Flaming White) "  );


    if ( ch->class==CLASS_PALADIN
	 && ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Smouldering Red-Grey) "  );
    if ( ch->class==CLASS_PALADIN
	 && ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Smouldering Red-White) "  );
    if ( ch->class==CLASS_PALADIN
	 && ( !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD))   )
          strcat( buf, "(Smouldering Grey-White) "  );

    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
	 && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, "(Magical) "   );
    if ( !glowsee && IS_OBJ_STAT(obj, ITEM_GLOW) )   strcat( buf, "(Glowing) "   );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, "(Humming) "   );
    if ( IS_OBJ_STAT(obj, ITEM_HIDDEN)	  )   strcat( buf, "(Hidden) "	  );
    if ( IS_OBJ_STAT(obj, ITEM_BURIED)	  )   strcat( buf, "(Buried) "	  );
    if ( IS_IMMORTAL(ch)
	 && IS_OBJ_STAT(obj, ITEM_PROTOTYPE) ) strcat( buf, "(PROTO) "	  );
    if ( IS_AFFECTED(ch, AFF_DETECTTRAPS)
	 && is_trapped(obj)   )   strcat( buf, "(Trap) "  );

    if ( fShort )
    {
	if ( glowsee && !IS_IMMORTAL(ch) )
	    strcat( buf, "the faint glow of something" );
	else
	if ( obj->short_descr )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( glowsee )
	    strcat( buf, "You see the faint glow of something nearby." );
	if ( obj->description )
	    strcat( buf, obj->description );
    }

    return buf;
}


/*
 * Some increasingly freaky hallucinated objects		-Thoric
 * (Hats off to Albert Hoffman's "problem child")
 */
char *hallucinated_object( int ms, bool fShort )
{
    int sms = URANGE( 1, (ms+10)/5, 20 );

    if ( fShort )
    switch( number_range( 6-URANGE(1,sms/2,5), sms ) )
    {
	case  1: return "a sword";
	case  2: return "a stick";
	case  3: return "something shiny";
	case  4: return "something";
	case  5: return "something interesting";
	case  6: return "something colorful";
	case  7: return "something that looks cool";
	case  8: return "a nifty thing";
	case  9: return "a cloak of flowing colors";
	case 10: return "a mystical flaming sword";
	case 11: return "a swarm of insects";
	case 12: return "a deathbane";
	case 13: return "a figment of your imagination";
	case 14: return "your gravestone";
	case 15: return "the long lost boots of Ranger Thoric";
	case 16: return "a glowing tome of arcane knowledge";
	case 17: return "a long sought secret";
	case 18: return "the meaning of it all";
	case 19: return "the answer";
	case 20: return "the key to life, the universe and everything";
    }
    switch( number_range( 6-URANGE(1,sms/2,5), sms ) )
    {
	case  1: return "A nice looking sword catches your eye.";
	case  2: return "The ground is covered in small sticks.";
	case  3: return "Something shiny catches your eye.";
	case  4: return "Something catches your attention.";
	case  5: return "Something interesting catches your eye.";
	case  6: return "Something colorful flows by.";
	case  7: return "Something that looks cool calls out to you.";
	case  8: return "A nifty thing of great importance stands here.";
	case  9: return "A cloak of flowing colors asks you to wear it.";
	case 10: return "A mystical flaming sword awaits your grasp.";
	case 11: return "A swarm of insects buzzes in your face!";
	case 12: return "The extremely rare Deathbane lies at your feet.";
	case 13: return "A figment of your imagination is at your command.";
	case 14: return "You notice a gravestone here... upon closer examination, it reads your name.";
	case 15: return "The long lost boots of Ranger Thoric lie off to the side.";
	case 16: return "A glowing tome of arcane knowledge hovers in the air before you.";
	case 17: return "A long sought secret of all mankind is now clear to you.";
	case 18: return "The meaning of it all, so simple, so clear... of course!";
	case 19: return "The answer.  One.  It's always been One.";
	case 20: return "The key to life, the universe and everything awaits your hand.";
    }
    return "Whoa!!!";
}


/* This is the punct snippet from Desden el Chaman Tibetano - Nov 1998
   Email: jlalbatros@mx2.redestb.es
*/
char *num_punct(int foo)
{
    int index, index_new, rest;
    char buf[16];
    static char buf_new[16];

    sprintf(buf,"%d",foo);
    rest = strlen(buf)%3;

    for (index=index_new=0;index<strlen(buf);index++,index_new++)
    {
	if (index!=0 && (index-rest)%3==0 )
	{
	    buf_new[index_new]=',';
	    index_new++;
	    buf_new[index_new]=buf[index];
        }
        else
	    buf_new[index_new] = buf[index];
    }
    buf_new[index_new]='\0';
    return buf_new;
}


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char **prgpstrShow;
    int *prgnShow;
    int *pitShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count, offcount, tmp, ms, cnt;
    bool fCombine;

    if ( !ch->desc )
	return;

    /*
     * if there's no list... then don't do all this crap!  -Thoric
     */
    if ( !list )
    {
    	if ( fShowNothing )
    	{
	   if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) )
	      send_to_char( "     ", ch );
	   set_char_color( AT_OBJECT, ch );
	   send_to_char( "Nothing.\n\r", ch );
	}
	return;
    }
    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj; obj = obj->next_content )
	count++;

    ms  = (ch->mental_state ? ch->mental_state : 1)
	* (IS_NPC(ch) ? 1 : (ch->pcdata->condition[COND_DRUNK] ? (ch->pcdata->condition[COND_DRUNK]/12) : 1));

    /*
     * If not mentally stable...
     */
    if ( abs(ms) > 40 )
    {
	offcount = URANGE( -(count), (count * ms) / 100, count*2 );
	if ( offcount < 0 )
	  offcount += number_range(0, abs(offcount));
	else
	if ( offcount > 0 )
	  offcount -= number_range(0, offcount);
    }
    else
	offcount = 0;

    if ( count + offcount <= 0 )
    {
    	if ( fShowNothing )
    	{
	   if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) )
	      send_to_char( "     ", ch );
	   set_char_color( AT_OBJECT, ch );
	   send_to_char( "Nothing.\n\r", ch );
	}
	return;
    }

    CREATE( prgpstrShow,	char*,	count + ((offcount > 0) ? offcount : 0) );
    CREATE( prgnShow,		int,	count + ((offcount > 0) ? offcount : 0) );
    CREATE( pitShow,		int,	count + ((offcount > 0) ? offcount : 0) );
    nShow	= 0;
    tmp		= (offcount > 0) ? offcount : 0;
    cnt		= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( offcount < 0 && ++cnt > (count + offcount) )
	    break;
	if ( tmp > 0 && number_bits(1) == 0 )
	{
	    prgpstrShow [nShow] = str_dup( hallucinated_object(ms, fShort) );
	    prgnShow	[nShow] = 1;
	    pitShow	[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
	    nShow++;
	    --tmp;
	}
	if ( obj->wear_loc == WEAR_NONE
	&& can_see_obj( ch, obj )
	&& (obj->item_type != ITEM_TRAP || IS_AFFECTED(ch, AFF_DETECTTRAPS) ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow] += obj->count;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    pitShow[nShow] = obj->item_type;
	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = obj->count;
		nShow++;
	    }
	}
    }
    if ( tmp > 0 )
    {
	int x;
	for ( x = 0; x < tmp; x++ )
	{
	    prgpstrShow [nShow] = str_dup( hallucinated_object(ms, fShort) );
	    prgnShow	[nShow] = 1;
	    pitShow	[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
	    nShow++;
	}
    }

    /*
     * Output the formatted list.		-Color support by Thoric
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	switch(pitShow[iShow]) {
	default:
	  set_char_color( AT_OBJECT, ch );
	  break;
	case ITEM_BLOOD:
	  set_char_color( AT_BLOOD, ch );
	  break;
	case ITEM_MONEY:
	case ITEM_TREASURE:
	  set_char_color( AT_YELLOW, ch );
	  break;
	case ITEM_COOK:
	case ITEM_FOOD:
	  set_char_color( AT_HUNGRY, ch );
	  break;
	case ITEM_DRINK_CON:
	case ITEM_FOUNTAIN:
	  set_char_color( AT_THIRSTY, ch );
	  break;
	case ITEM_FIRE:
	  set_char_color( AT_FIRE, ch );
	  break;
	case ITEM_SCROLL:
	case ITEM_WAND:
	case ITEM_STAFF:
	  set_char_color( AT_MAGIC, ch );
	  break;
	}
	if ( fShowNothing )
	    send_to_char( "     ", ch );
	send_to_char( prgpstrShow[iShow], ch );
/*	if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) ) */
	{
	    if ( prgnShow[iShow] != 1 )
		ch_printf( ch, " (%d)", prgnShow[iShow] );
	}

	send_to_char( "\n\r", ch );
	DISPOSE( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || xIS_SET(ch->act, PLR_COMBINE) )
	    send_to_char( "     ", ch );
	set_char_color( AT_OBJECT, ch );
	send_to_char( "Nothing.\n\r", ch );
    }

    /*
     * Clean up.
     */
    DISPOSE( prgpstrShow );
    DISPOSE( prgnShow	 );
    DISPOSE( pitShow	 );
    return;
}


/*
 * Show fancy descriptions for certain spell affects		-Thoric
 */
void show_visible_affects_to_char( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char name[MAX_STRING_LENGTH];

    if ( IS_NPC( victim ) )
      strcpy( name, victim->short_descr );
    else
      strcpy( name, victim->name);
    name[0] = toupper(name[0]);

    if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
    {
	set_char_color( AT_WHITE, ch );
        if ( IS_GOOD(victim) )
            ch_printf( ch, "%s glows with an aura of divine radiance.\n\r", name );
        else if ( IS_EVIL(victim) )
            ch_printf( ch, "%s shimmers beneath an aura of dark energy.\n\r", name );
        else
            ch_printf( ch, "%s is shrouded in flowing shadow and light.\n\r", name );
    }
    if ( IS_AFFECTED(victim, AFF_FIRESHIELD) )
    {
        set_char_color( AT_FIRE, ch );
        ch_printf( ch, "%s is engulfed within a blaze of mystical flame.\n\r", name );
    }
    if ( IS_AFFECTED(victim, AFF_SHOCKSHIELD) )
    {
        set_char_color( AT_BLUE, ch );
	ch_printf( ch, "%s is surrounded by cascading torrents of energy.\n\r", name );
    }
    if ( IS_AFFECTED(victim, AFF_ACIDMIST) )
    {
	set_char_color( AT_GREEN, ch );
	ch_printf( ch, "%s is visible through a cloud of churning mist.\n\r", name );
    }
/*Scryn 8/13*/
    if ( IS_AFFECTED(victim, AFF_ICESHIELD) )
    {
        set_char_color( AT_LBLUE, ch );
        ch_printf( ch, "%s is ensphered by shards of glistening ice.\n\r", name );
    }
    if ( IS_AFFECTED(victim, AFF_CHARM)       )
    {
	set_char_color( AT_MAGIC, ch );
	ch_printf( ch, "%s wanders in a dazed, zombie-like state.\n\r", name );
    }
    if ( !IS_NPC(victim) && !victim->desc
    &&    victim->switched && IS_AFFECTED(victim->switched, AFF_POSSESS) )
    {
	set_char_color( AT_MAGIC, ch );
	strcpy( buf, PERS( victim, ch ) );
	strcat( buf, " appears to be in a deep trance...\n\r" );
    }
}

void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];

    buf[0] = '\0';

    set_char_color( AT_PERSON, ch );

    if ( IS_QUESTING( ch ) && ( IS_NPC( victim ) &&
	( ch->pcdata->quest_mob == victim->pIndexData->vnum ) &&
	( ch->pcdata->quest_type != QT_COMPLETE && ch->pcdata->quest_type != QT_FAILED &&
	  ch->pcdata->quest_type != QT_MOB_GIVE_COMPLETE ) ) )
	strcat( buf, "&r&Y&G&P[&Y&GQUEST&G&P] ");

    if ( !IS_NPC(victim) && !victim->desc )
    {
	if ( !victim->switched )         send_to_char_color( "&P[(Link Dead)] ", ch );
	else if ( !IS_AFFECTED(victim, AFF_POSSESS) )
						strcat( buf, "(Switched) " );
    }
    if ( IS_NPC(victim) && IS_AFFECTED(victim, AFF_POSSESS) && IS_IMMORTAL(ch)
	 && victim->desc )
    {
			sprintf( buf1, "(%s)",victim->desc->original->name );
			strcat( buf, buf1 );
    }
    if ( !IS_NPC(victim)
    &&   xIS_SET(victim->act, PLR_AFK) )	strcat( buf, "[AFK] ");        

    if ( (!IS_NPC(victim) && xIS_SET(victim->act, PLR_WIZINVIS))
      || (IS_NPC(victim) && xIS_SET(victim->act, ACT_MOBINVIS)) ) 
    {
        if (!IS_NPC(victim))
	sprintf( buf1,"(Invis %d) ", victim->pcdata->wizinvis );
        else sprintf( buf1,"(Mobinvis %d) ", victim->mobinvis);
	strcat( buf, buf1 );
    }
 

    if ( !IS_NPC( victim) )
    {
	if ( victim->pcdata->faction
        &&   IS_SET( victim->pcdata->flags, PCFLAG_DEADLY )
	&&   victim->pcdata->faction->badge
	&&   IS_CLANNED( victim ) )
                                        ch_printf_color( ch, "%s ", victim->pcdata->faction->badge );
    }

    set_char_color( AT_PERSON, ch );

    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(Invis) "      );
    if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "(Hide) "       );
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "(Translucent) ");
    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "(Pink Aura) "  );
    if ( IS_EVIL(victim)
    &&   (  IS_AFFECTED(ch, AFF_DETECT_EVIL) || ch->class==CLASS_PALADIN)     ) strcat( buf, "(Red Aura) "   );
    if ( IS_NEUTRAL(victim)
    &&   ch->class==CLASS_PALADIN  ) strcat( buf, "(Grey Aura) "   );
    if ( IS_GOOD(victim)
    &&   ch->class==CLASS_PALADIN  ) strcat( buf, "(White Aura) "   );


    if ( IS_AFFECTED(victim, AFF_BERSERK)     ) strcat( buf, "(Wild-eyed) "  );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_ATTACKER ) )
						strcat( buf, "(ATTACKER) "   );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_KILLER ) )
						strcat( buf, "(KILLER) "     );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_THIEF  ) )
						strcat( buf, "(THIEF) "      );
    if ( !IS_NPC(victim) && xIS_SET(victim->act, PLR_LITTERBUG  ) )
						strcat( buf, "(LITTERBUG) "  );
    if ( IS_NPC(victim) && IS_IMMORTAL(ch)
	 && xIS_SET(victim->act, ACT_PROTOTYPE) )strcat( buf, "(PROTO) "      );
    if ( IS_NPC(victim) && ch->mount && ch->mount == victim
         && ch->in_room == ch->mount->in_room ) strcat( buf, "(Mount) "      );
    if ( victim->desc && victim->desc->connected == CON_EDITING )
						strcat( buf, "(Writing) "    );
    if ( victim->morph != NULL )
      strcat (buf, "(Morphed) ");

    set_char_color( AT_PERSON, ch );
  if ((victim->position == victim->defposition && victim->long_descr[0] != '\0')
      || ( victim->morph && victim->morph->morph &&
           victim->morph->morph->defpos == victim->position ) )
    {
      if ( victim->morph != NULL )
      {
        if ( !IS_IMMORTAL(ch) )
        {
          if ( victim->morph->morph != NULL)
                strcat ( buf, victim->morph->morph->long_desc );
          else
                strcat ( buf, victim->long_descr );
        }
        else
	{
           strcat (buf, PERS(victim, ch) );
    	   if ( !IS_NPC(victim) && !xIS_SET(ch->act, PLR_BRIEF) )
		strcat( buf, victim->pcdata->title );
	   strcat( buf, ".\n\r" );
	}
      }
      else
           strcat (buf, victim->long_descr);
	send_to_char( buf, ch );
	show_visible_affects_to_char( victim, ch );
	return;
    }
    else {
    if ( victim->morph != NULL && victim->morph->morph != NULL &&
	 !IS_IMMORTAL( ch ) )
	strcat( buf, MORPHPERS( victim, ch ) );
    else 
    	strcat( buf, PERS( victim, ch ) );
    }

    if ( !IS_NPC(victim) && !xIS_SET(ch->act, PLR_BRIEF) )
	strcat( buf, victim->pcdata->title );

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is DEAD!!" );			break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded." );		break;
    case POS_INCAP:    strcat( buf, " is incapacitated." );		break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." );	break;
    case POS_SLEEPING:
        if (ch->position == POS_SITTING
        ||  ch->position == POS_RESTING )
            strcat( buf, " is sleeping nearby." );
	else
            strcat( buf, " is deep in slumber here." );
        break;
    case POS_RESTING:
        if (ch->position == POS_RESTING)
            strcat ( buf, " is sprawled out alongside you." );
        else
	if (ch->position == POS_MOUNTED)
	    strcat ( buf, " is sprawled out at the foot of your mount." );
	else
            strcat (buf, " is sprawled out here." );
        break;
    case POS_SITTING:
        if (ch->position == POS_SITTING)
            strcat( buf, " sits here with you." );
        else
        if (ch->position == POS_RESTING)
            strcat( buf, " sits nearby as you lie around." );
        else
            strcat( buf, " sits upright here." );
        break;
    case POS_STANDING:
	if ( IS_IMMORTAL(victim) )
            strcat( buf, " is here before you." );
	else
        if ( ( victim->in_room->sector_type == SECT_UNDERWATER )
        && !IS_AFFECTED(victim, AFF_AQUA_BREATH) && !IS_NPC(victim) )
            strcat( buf, " is drowning here." );
	else
	if ( victim->in_room->sector_type == SECT_UNDERWATER )
            strcat( buf, " is here in the water." );
	else
	if ( ( victim->in_room->sector_type == SECT_OCEANFLOOR )
	&& !IS_AFFECTED(victim, AFF_AQUA_BREATH) && !IS_NPC(victim) )
	    strcat( buf, " is drowning here." );
	else
	if ( victim->in_room->sector_type == SECT_OCEANFLOOR )
	    strcat( buf, " is standing here in the water." );
	else
	if ( IS_AFFECTED(victim, AFF_FLOATING)
        || IS_AFFECTED(victim, AFF_FLYING) )
          strcat( buf, " is hovering here." );
        else
          strcat( buf, " is standing here." );
        break;
    case POS_SHOVE:    strcat( buf, " is being shoved around." );	break;
    case POS_DRAG:     strcat( buf, " is being dragged around." );	break;
    case POS_MOUNTED:
	strcat( buf, " is here, upon " );
	if ( !victim->mount )
	    strcat( buf, "thin air???" );
	else
	if ( victim->mount == ch )
	    strcat( buf, "your back." );
	else
	if ( victim->in_room == victim->mount->in_room )
	{
	    strcat( buf, PERS( victim->mount, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    case POS_FIGHTING:
    case POS_EVASIVE:
    case POS_DEFENSIVE:
    case POS_AGGRESSIVE:
    case POS_BERSERK:
	strcat( buf, " is here, fighting " );
	if ( !victim->fighting ) 
        {
	    strcat( buf, "thin air???" );

            /* some bug somewhere.... kinda hackey fix -h */
            if(! victim->mount)
               victim->position = POS_STANDING;
            else
               victim->position = POS_MOUNTED;
        }
	else if ( who_fighting( victim ) == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->who->in_room )
	{
	    strcat( buf, PERS( victim->fighting->who, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    }

    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    show_visible_affects_to_char( victim, ch );
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    if ( can_see( victim, ch ) && !IS_NPC( ch ) && !xIS_SET( ch->act, PLR_WIZINVIS ) )
    {
      act( AT_ACTION, "$n looks at you.", ch, NULL, victim, TO_VICT    );
      if ( victim != ch )
	act( AT_ACTION, "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
      else
        act( AT_ACTION, "$n looks at $mself.", ch, NULL, victim, TO_NOTVICT );
    }

    if ( victim->description[0] != '\0' )
    {
      if ( victim->morph != NULL && victim->morph->morph != NULL)
        send_to_char ( victim->morph->morph->description , ch );
      else
        send_to_char (victim->description, ch);
    }
    else
    {
        if ( victim->morph != NULL && victim->morph->morph != NULL)
           send_to_char ( victim->morph->morph->description , ch );
	else if ( IS_NPC( victim ) )
	  act( AT_PLAIN, "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
	else if ( ch != victim )
	  act( AT_PLAIN, "$E isn't much to look at...", ch, NULL, victim, TO_CHAR );
	else
	  act( AT_PLAIN, "You're not much to look at...", ch, NULL, NULL, TO_CHAR );
    }

    show_race_line( ch, victim );
    show_condition( ch, victim );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		if ( victim != ch )
		  act( AT_PLAIN, "$N is using:", ch, NULL, victim, TO_CHAR );
		else
		  act( AT_PLAIN, "You are using:", ch, NULL, NULL, TO_CHAR );
		found = TRUE;
	    }
            if( (!IS_NPC(victim)) && (victim->race>0) && (victim->race<MAX_PC_RACE))
		send_to_char(race_table[victim->race]->where_name[iWear], ch);
	    else
	        send_to_char( where_name[iWear], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    /*
     * Crash fix here by Thoric
     */
    if ( IS_NPC(ch) || victim == ch )
      return;

    if ( IS_IMMORTAL( ch ) )
    {
      if ( IS_NPC( victim ) )
	ch_printf( ch, "\n\rMobile #%d '%s' ",
	    victim->pIndexData->vnum,
	    victim->name );
	else
	  ch_printf( ch, "\n\r%s ", victim->name );
      ch_printf( ch, "is a level %d %s %s.\n\r",
	victim->level,
	IS_NPC(victim)?victim->race<MAX_NPC_RACE&&victim->race>=0?
	npc_race[victim->race]:"unknown":victim->race<MAX_PC_RACE&&
	race_table[victim->race]->race_name&&
	race_table[victim->race]->race_name[0] != '\0'?
	race_table[victim->race]->race_name:"unknown",
	IS_NPC(victim)?victim->class<MAX_NPC_CLASS&&victim->class>=0?
	npc_class[victim->class] : "unknown":victim->class<MAX_PC_CLASS&&
	class_table[victim->class]->who_name&&
	class_table[victim->class]->who_name[0] != '\0'?
	class_table[victim->class]->who_name:"unknown");
/* Fix so it shows what is in class table
	victim->race<MAX_NPC_RACE&&victim->race>=0?npc_race[victim->race] : "unknown",
	victim->class<MAX_NPC_CLASS&&victim->class>=0?npc_class[victim->class] : "unknown" );
*/
    }

    if ( number_percent( ) < LEARNED(ch, gsn_peek) )
    {
	ch_printf( ch, "\n\rYou peek at %s inventory:\n\r",
	  victim->sex == 1 ? "his" : victim->sex == 2 ? "her" : "its" );
	show_list_to_char( victim->first_carrying, ch, TRUE, TRUE );
	learn_from_success( ch, gsn_peek );
    }
    else
      if ( ch->pcdata->learned[gsn_peek] > 0 )
        learn_from_failure( ch, gsn_peek );

    return;
}


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( rch == supermob && !IS_IMMORTAL( ch ) )
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if ( room_is_dark( ch->in_room )
	&&        IS_AFFECTED(rch, AFF_INFRARED ) 
        && !(!IS_NPC(rch) && IS_IMMORTAL(rch)) )
	{
	    set_char_color( AT_BLOOD, ch );
	    send_to_char( "The red form of a living creature is here.\n\r", ch );
	}
    }

    return;
}



bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;
	
    if ( IS_AFFECTED(ch, AFF_TRUESIGHT) )
      return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}

/*
 * Returns classical DIKU door direction based on text in arg	-Thoric
 */
int get_door( char *arg )
{
    int door;

	 if ( !str_cmp( arg, "n"  ) || !str_cmp( arg, "north"	  ) ) door = 0;
    else if ( !str_cmp( arg, "e"  ) || !str_cmp( arg, "east"	  ) ) door = 1;
    else if ( !str_cmp( arg, "s"  ) || !str_cmp( arg, "south"	  ) ) door = 2;
    else if ( !str_cmp( arg, "w"  ) || !str_cmp( arg, "west"	  ) ) door = 3;
    else if ( !str_cmp( arg, "u"  ) || !str_cmp( arg, "up"	  ) ) door = 4;
    else if ( !str_cmp( arg, "d"  ) || !str_cmp( arg, "down"	  ) ) door = 5;
    else if ( !str_cmp( arg, "ne" ) || !str_cmp( arg, "northeast" ) ) door = 6;
    else if ( !str_cmp( arg, "nw" ) || !str_cmp( arg, "northwest" ) ) door = 7;
    else if ( !str_cmp( arg, "se" ) || !str_cmp( arg, "southeast" ) ) door = 8;
    else if ( !str_cmp( arg, "sw" ) || !str_cmp( arg, "southwest" ) ) door = 9;
    else door = -1;
    return door;
}

void do_look( CHAR_DATA *ch, char *argument )
{
    char arg  [MAX_INPUT_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *original;
    char *pdesc;
    sh_int door;
    int number, cnt;

    if ( !ch->desc )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !xIS_SET(ch->act, PLR_HOLYLIGHT)
    &&   !IS_AFFECTED(ch, AFF_TRUESIGHT)
    &&   room_is_dark( ch->in_room ) )
    {
	set_char_color( AT_DGREY, ch );
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->first_person, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	set_char_color( AT_RMNAME, ch );
	if ( ch->in_room->area->room_name_format )
	    ch_printf_color( ch, "%s\n\r", str_replace( "<%room_name%>", ch->in_room->name, ch->in_room->area->room_name_format, FALSE ) );
	else
	    ch_printf      ( ch, "%s\n\r", ch->in_room->name );
	set_char_color( AT_RMDESC, ch );

	if ( arg1[0] == '\0' || ( !IS_NPC(ch) && !xIS_SET(ch->act, PLR_BRIEF) ) )
	{
	    if ( ch->in_room->area->room_desc_format )
		ch_printf_color( ch, "%s%s", ch->in_room->area->room_desc_format, ch->in_room->description );
	    else
		send_to_char( ch->in_room->description, ch );
	}

	if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOEXIT) )
	    do_exits( ch, "auto" );


	show_list_to_char( ch->in_room->first_content, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->first_person,  ch );
	return;
    }

    if ( !str_cmp( arg1, "under" ) )
    {
	int count;

	/* 'look under' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look beneath what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}
	if ( !CAN_WEAR( obj, ITEM_TAKE ) && ch->level < sysdata.level_getobjnotake )
	{
	    send_to_char( "You can't seem to get a grip on it.\n\r", ch );
	    return;
	}
	if ( ch->carry_weight + obj->weight > can_carry_w( ch ) )
	{
	    send_to_char( "It's too heavy for you to look under.\n\r", ch );
	    return;
	}
	count = obj->count;
	obj->count = 1;
	act( AT_PLAIN, "You lift $p and look beneath it:", ch, obj, NULL, TO_CHAR );
	act( AT_PLAIN, "$n lifts $p and looks beneath it:", ch, obj, NULL, TO_ROOM );
	obj->count = count;
	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
	   show_list_to_char( obj->first_content, ch, TRUE, TRUE );
	else
	   send_to_char( "Nothing.\n\r", ch );
	if ( EXA_prog_trigger )
	   oprog_examine_trigger( ch, obj );
	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
    {
	int count;

	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
	        if ( EXA_prog_trigger )
		    oprog_examine_trigger( ch, obj );
		break;
	    }

	    ch_printf( ch, "It's %s full of a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about"     : "more than",
		liq_table[obj->value[2]].liq_color
		);

	    if ( EXA_prog_trigger )
		oprog_examine_trigger( ch, obj );
	    break;

	case ITEM_PORTAL:
	    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    {
		if ( pexit->vdir == DIR_PORTAL
		&&   IS_SET(pexit->exit_info, EX_PORTAL) )
		{
		    if ( room_is_private( pexit->to_room )
		    &&   get_trust(ch) < sysdata.level_override_private )
		    {
			set_char_color( AT_WHITE, ch );
			send_to_char( "That room is private buster!\n\r", ch );
			return;
		    }
		    original = ch->in_room;
		    char_from_room( ch );
		    char_to_room( ch, pexit->to_room );
		    do_look( ch, "auto" );
		    char_from_room( ch );
		    char_to_room( ch, original );
		    return;
		}
	    }
	    send_to_char( "You see swirling chaos...\n\r", ch );
	    break;
	case ITEM_CONTAINER:
	case ITEM_QUIVER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	case ITEM_KEYRING:
	    count = obj->count;
	    obj->count = 1;
	    if ( obj->item_type == ITEM_CONTAINER )
		act( AT_PLAIN, "$p contains:", ch, obj, NULL, TO_CHAR );
	    else
		act( AT_PLAIN, "$p holds:", ch, obj, NULL, TO_CHAR );
	    obj->count = count;
	    show_list_to_char( obj->first_content, ch, TRUE, TRUE );
	    if ( EXA_prog_trigger )
		oprog_examine_trigger( ch, obj );
	    break;
	}
	return;
    }

    if ( (pdesc=get_extra_descr(arg1, ch->in_room->first_extradesc)) != NULL )
    {
	send_to_char_color( pdesc, ch );
	return;
    }

    door = get_door(arg1);
    if ( (pexit=find_door(ch, arg1, TRUE)) != NULL )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED)
	&&  !IS_SET(pexit->exit_info, EX_WINDOW) )
	{
	    if ( (IS_SET(pexit->exit_info, EX_SECRET)
	    ||    IS_SET(pexit->exit_info, EX_DIG)) && door != -1 )
		send_to_char( "Nothing special there.\n\r", ch );
	    else
		act( AT_PLAIN, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	    return;
	}
	if ( IS_SET( pexit->exit_info, EX_BASHED ) )
	    act(AT_RED, "The $d has been bashed from its hinges!",ch, NULL, pexit->keyword, TO_CHAR);

	if ( pexit->description && pexit->description[0] != '\0' )
	    send_to_char( pexit->description, ch );
	else
	    send_to_char( "Nothing special there.\n\r", ch );

	/*
	 * Ability to look into the next room			-Thoric
	 */
	if ( pexit->to_room
	&& ( IS_AFFECTED( ch, AFF_SCRYING )
	||   ch->class == CLASS_THIEF
	||   IS_SET( pexit->exit_info, EX_xLOOK )
	||   get_trust(ch) >= LEVEL_IMMORTAL ) )
	{
	    if ( !IS_SET( pexit->exit_info, EX_xLOOK )
	    &&    get_trust( ch ) < LEVEL_IMMORTAL )
	    {
		set_char_color( AT_MAGIC, ch );
		send_to_char( "You attempt to scry...\n\r", ch );
		/*
		 * Change by Narn, Sept 96 to allow characters who don't have the
		 * scry spell to benefit from objects that are affected by scry.
		 */
		if (!IS_NPC(ch) )
		{
		    int percent = LEARNED(ch, skill_lookup("scry") );
		    if ( !percent )
		    {
			if ( ch->class == CLASS_THIEF )
			    percent = 95;
			else
			    percent = 55;	/* 95 was too good -Thoric */
		    }
 
		    if ( number_percent( ) > percent ) 
		    {
			send_to_char( "You fail.\n\r", ch );
			return;
		    }
		}
	    }
	    if ( room_is_private( pexit->to_room )
	    &&   get_trust(ch) < sysdata.level_override_private )
	    {
		set_char_color( AT_WHITE, ch );
		send_to_char( "That room is private buster!\n\r", ch );
		return;
	    }
	    original = ch->in_room;
	    char_from_room( ch );
	    char_to_room( ch, pexit->to_room );
	    do_look( ch, "auto" );
	    char_from_room( ch );
	    char_to_room( ch, original );
	}
	return;
    }
    else
    if ( door != -1 )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( (victim = get_char_room(ch, arg1)) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }


    /* finally fixed the annoying look 2.obj desc bug	-Thoric */
    number = number_argument( arg1, arg );
    for ( cnt = 0, obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    if ( (pdesc=get_extra_descr(arg, obj->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char_color( pdesc, ch );
	        if ( EXA_prog_trigger )
		    oprog_examine_trigger( ch, obj );
		return;
	    }

	    if ( (pdesc=get_extra_descr(arg, obj->pIndexData->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char_color( pdesc, ch );
	        if ( EXA_prog_trigger )
		    oprog_examine_trigger( ch, obj );
		return;
	    }
	    if ( nifty_is_name_prefix( arg, obj->name ) )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		pdesc = get_extra_descr( obj->name, obj->pIndexData->first_extradesc );
		if ( !pdesc )
		  pdesc = get_extra_descr( obj->name, obj->first_extradesc );
		if ( !pdesc )
		  send_to_char_color( "You see nothing special.\r\n", ch );
		else
		  send_to_char_color( pdesc, ch );
		if ( EXA_prog_trigger )
		    oprog_examine_trigger( ch, obj );
		  return;
	    }
	}
    }

    for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    if ( (pdesc=get_extra_descr(arg, obj->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char_color( pdesc, ch );
	        if ( EXA_prog_trigger )
		    oprog_examine_trigger( ch, obj );
		return;
	    }

	    if ( (pdesc=get_extra_descr(arg, obj->pIndexData->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char_color( pdesc, ch );
	        if ( EXA_prog_trigger )
		    oprog_examine_trigger( ch, obj );
		return;
	    }
	    if ( nifty_is_name_prefix( arg, obj->name ) )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		pdesc = get_extra_descr( obj->name, obj->pIndexData->first_extradesc );
		if ( !pdesc )
		  pdesc = get_extra_descr( obj->name, obj->first_extradesc );
		if ( !pdesc )
		  send_to_char( "You see nothing special.\r\n", ch );
		else
		  send_to_char_color( pdesc, ch );
		if ( EXA_prog_trigger )
		    oprog_examine_trigger( ch, obj );
		  return;
	    }
	}
    }

    send_to_char( "You do not see that here.\n\r", ch );
    return;
}

void show_race_line( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    int feet, inches;


	if ( !IS_NPC(victim) && (victim != ch) )
	{
		feet =  victim->height / 12;
		inches = victim->height % 12;
		sprintf( buf, "%s is %d'%d\" and weighs %d pounds.\n\r", PERS(victim, ch), feet, inches, victim->weight );
		send_to_char( buf, ch);
                return;
        } 
	if ( !IS_NPC(victim) && (victim == ch) )
	{
		feet =  victim->height / 12;
		inches = victim->height % 12;
		sprintf( buf, "You are %d'%d\" and weigh %d pounds.\n\r",  feet, inches, victim->weight );
		send_to_char( buf, ch);
                return;
        } 

}


void show_condition( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    int percent;

    if ( victim->max_hit > 0 )
        percent = ( 100 * victim->hit ) / victim->max_hit;
    else
        percent = -1;


    if ( victim != ch )
    {
	strcpy( buf, PERS(victim, ch) );
             if ( percent >= 100 ) strcat( buf, " is in perfect health.\n\r" );
	else if ( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
        else if ( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
	else if ( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
	else if ( percent >=  60 ) strcat( buf, " has several wounds.\n\r"    );
	else if ( percent >=  50 ) strcat( buf, " has many nasty wounds.\n\r" );
	else if ( percent >=  40 ) strcat( buf, " is bleeding freely.\n\r"    );
	else if ( percent >=  30 ) strcat( buf, " is covered in blood.\n\r"   );
	else if ( percent >=  20 ) strcat( buf, " is leaking guts.\n\r"       );
	else if ( percent >=  10 ) strcat( buf, " is almost dead.\n\r"        );
	else                       strcat( buf, " is DYING.\n\r"              );
    }
    else
    {
	strcpy( buf, "You" );
	     if ( percent >= 100 ) strcat( buf, " are in perfect health.\n\r" );
	else if ( percent >=  90 ) strcat( buf, " are slightly scratched.\n\r");
        else if ( percent >=  80 ) strcat( buf, " have a few bruises.\n\r"    );
        else if ( percent >=  70 ) strcat( buf, " have some cuts.\n\r"        );
        else if ( percent >=  60 ) strcat( buf, " have several wounds.\n\r"   );
        else if ( percent >=  50 ) strcat( buf, " have many nasty wounds.\n\r");
	else if ( percent >=  40 ) strcat( buf, " are bleeding freely.\n\r"   );
	else if ( percent >=  30 ) strcat( buf, " are covered in blood.\n\r"  );
	else if ( percent >=  20 ) strcat( buf, " are leaking guts.\n\r"      );
	else if ( percent >=  10 ) strcat( buf, " are almost dead.\n\r"       );
	else                       strcat( buf, " are DYING.\n\r"             );
    }

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}

/* A much simpler version of look, this function will show you only
the condition of a mob or pc, or if used without an argument, the
same you would see if you enter the room and have config +brief.
-- Narn, winter '96
*/
void do_glance( CHAR_DATA *ch, char *argument )
{
  char arg1 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  bool brief;
 
  if ( !ch->desc )
    return;
 
  if ( ch->position < POS_SLEEPING )
  {
    send_to_char( "You can't see anything but stars!\n\r", ch );
    return;
  }
 
  if ( ch->position == POS_SLEEPING )
  {
    send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
    return;
  }
 
  if ( !check_blind( ch ) )
    return;

  set_char_color( AT_ACTION, ch ); 
  argument = one_argument( argument, arg1 );
 
  if ( arg1[0] == '\0' )
  {
    if ( xIS_SET(ch->act, PLR_BRIEF) )
	brief = TRUE;
    else
	brief = FALSE;
    xSET_BIT( ch->act, PLR_BRIEF );
    do_look( ch, "auto" );
    if ( !brief )
	xREMOVE_BIT(ch->act, PLR_BRIEF);
    return;
  }
 
  if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "They're not here.\n\r", ch );
    return;
  }
  else
  {
    if ( can_see( victim, ch ) )
    {
	act( AT_ACTION, "$n glances at you.", ch, NULL, victim, TO_VICT );
	act( AT_ACTION, "$n glances at $N.",  ch, NULL, victim, TO_NOTVICT );
    }
    if ( IS_IMMORTAL( ch ) && victim != ch )
    {
	if ( IS_NPC( victim ) )
	    ch_printf( ch, "Mobile #%d '%s' ",
		victim->pIndexData->vnum, victim->name );
	else
	    ch_printf( ch, "%s ", victim->name );
	ch_printf( ch, "is a level %d %s %s.\n\r",
	    victim->level,
	    IS_NPC(victim)?victim->race<MAX_NPC_RACE&&victim->race>=0?
            npc_race[victim->race] : "unknown":victim->race<MAX_PC_RACE&&
            race_table[victim->race]->race_name&&
            race_table[victim->race]->race_name[0] != '\0'?
            race_table[victim->race]->race_name:"unknown",
            IS_NPC(victim)?victim->class<MAX_NPC_CLASS&&victim->class>=0?
            npc_class[victim->class] : "unknown":victim->class<MAX_PC_CLASS&&
            class_table[victim->class]->who_name&&
            class_table[victim->class]->who_name[0] != '\0'?
            class_table[victim->class]->who_name:"unknown");
/* New Change
	    victim->race<MAX_NPC_RACE&&victim->race>=0?npc_race[victim->race] : "unknown",
	    victim->class<MAX_NPC_CLASS&&victim->class>=0?npc_class[victim->class] : "unknown" );
*/
    }
    show_condition( ch, victim );

    return;
  }

  return;
}


void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    BOARD_DATA *board;
    sh_int dam;

    if ( !argument )
    {
	bug( "do_examine: null argument.", 0);
	return;
    }

    if ( !ch )
    {
	bug( "do_examine: null ch.", 0);
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    EXA_prog_trigger = FALSE;
    do_look( ch, buf );
    EXA_prog_trigger = TRUE;

    /*
     * Support for looking at boards, checking equipment conditions,
     * and support for trigger positions by Thoric
     */
    if ( (obj = get_obj_here(ch, arg)) != NULL )
    {
	if ( (board = get_board(obj)) != NULL )
	{
	    if ( board->num_posts )
		ch_printf( ch, "There are about %d notes posted here.  Type 'note list' to list them.\n\r", board->num_posts );
	    else
		send_to_char( "There aren't any notes posted here.\n\r", ch );
	}

	switch ( obj->item_type )
	{
	default:
	    break;

	case ITEM_ARMOR:
	    if ( obj->value[1] == 0 )
	      obj->value[1] = obj->value[0];
	    if ( obj->value[1] == 0 )
	      obj->value[1] = 1;
	    dam = (sh_int) ((obj->value[0] * 10) / obj->value[1]);
	    strcpy( buf, "As you look more closely, you notice that it is ");
	    if (dam >= 10) strcat( buf, "in superb condition.");
       else if (dam ==  9) strcat( buf, "in very good condition.");
       else if (dam ==  8) strcat( buf, "in good shape.");
       else if (dam ==  7) strcat( buf, "showing a bit of wear.");
       else if (dam ==  6) strcat( buf, "a little run down.");
       else if (dam ==  5) strcat( buf, "in need of repair.");
       else if (dam ==  4) strcat( buf, "in great need of repair.");
       else if (dam ==  3) strcat( buf, "in dire need of repair.");
       else if (dam ==  2) strcat( buf, "very badly worn.");
       else if (dam ==  1) strcat( buf, "practically worthless.");
       else if (dam <=  0) strcat( buf, "broken.");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_WEAPON:
	    dam = INIT_WEAPON_CONDITION - obj->value[0];
	    strcpy( buf, "As you look more closely, you notice that it is ");
	    if (dam ==  0) strcat( buf, "in superb condition.");
       else if (dam ==  1) strcat( buf, "in excellent condition.");
       else if (dam ==  2) strcat( buf, "in very good condition.");
       else if (dam ==  3) strcat( buf, "in good shape.");
       else if (dam ==  4) strcat( buf, "showing a bit of wear.");
       else if (dam ==  5) strcat( buf, "a little run down.");
       else if (dam ==  6) strcat( buf, "in need of repair.");
       else if (dam ==  7) strcat( buf, "in great need of repair.");
       else if (dam ==  8) strcat( buf, "in dire need of repair.");
       else if (dam ==  9) strcat( buf, "very badly worn.");
       else if (dam == 10) strcat( buf, "practically worthless.");
       else if (dam == 11) strcat( buf, "almost broken.");
       else if (dam == 12) strcat( buf, "broken.");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_COOK:
	    strcpy( buf, "As you examine it carefully you notice that it " );
	    dam = obj->value[2];
	     if (dam >= 3) strcat( buf, "is burned to a crisp.");
	else if (dam == 2) strcat( buf, "is a little over cooked."); 
	else if (dam == 1) strcat( buf, "is perfectly roasted.");
	else		   strcat( buf, "is raw.");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	case ITEM_FOOD:
	    if ( obj->timer > 0 && obj->value[1] > 0 )
	      dam = (obj->timer * 10) / obj->value[1];
	    else
	      dam = 10;
	    if ( obj->item_type == ITEM_FOOD )	
	      strcpy( buf, "As you examine it carefully you notice that it " );
	    else
	      strcpy( buf, "Also it " );
	    if (dam >= 10) strcat( buf, "is fresh.");
       else if (dam ==  9) strcat( buf, "is nearly fresh.");
       else if (dam ==  8) strcat( buf, "is perfectly fine.");
       else if (dam ==  7) strcat( buf, "looks good.");
       else if (dam ==  6) strcat( buf, "looks ok.");
       else if (dam ==  5) strcat( buf, "is a little stale.");
       else if (dam ==  4) strcat( buf, "is a bit stale.");
       else if (dam ==  3) strcat( buf, "smells slightly off.");
       else if (dam ==  2) strcat( buf, "smells quite rank.");
       else if (dam ==  1) strcat( buf, "smells revolting!");
       else if (dam <=  0) strcat( buf, "is crawling with maggots!");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	    break;


	case ITEM_SWITCH:
	case ITEM_LEVER:
	case ITEM_PULLCHAIN:
	    if ( IS_SET( obj->value[0], TRIG_UP ) )
		send_to_char( "You notice that it is in the up position.\n\r", ch );
	    else
		send_to_char( "You notice that it is in the down position.\n\r", ch );
	    break;
	case ITEM_BUTTON:
	    if ( IS_SET( obj->value[0], TRIG_UP ) )
		send_to_char( "You notice that it is depressed.\n\r", ch );
	    else
		send_to_char( "You notice that it is not depressed.\n\r", ch );
	    break;

/* Not needed due to check in do_look already
	case ITEM_PORTAL:
	    EXA_prog_trigger = FALSE;
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	    EXA_prog_trigger = TRUE;
	    break;
*/

        case ITEM_CORPSE_PC:
	case ITEM_CORPSE_NPC:
            {
		sh_int timerfrac = obj->timer;
		if ( obj->item_type == ITEM_CORPSE_PC )
		timerfrac = (int)obj->timer / 8 + 1; 

		switch (timerfrac)
		{
		    default:
			send_to_char( "This corpse has recently been slain.\n\r", ch );
			break;
		    case 4:
			send_to_char( "This corpse was slain a little while ago.\n\r", ch );
			break;
		    case 3:
			send_to_char( "A foul smell rises from the corpse, and it is covered in flies.\n\r", ch );
			break;
		    case 2:
			send_to_char( "A writhing mass of maggots and decay, you can barely go near this corpse.\n\r", ch );
			break;
		    case 1:
		    case 0:
			send_to_char( "Little more than bones, there isn't much left of this corpse.\n\r", ch );
			break;
		}
            }
	case ITEM_CONTAINER:
	    if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
		break;
	case ITEM_DRINK_CON:
	case ITEM_QUIVER:
	    send_to_char( "When you look inside, you see:\n\r", ch );
	case ITEM_KEYRING:
	    EXA_prog_trigger = FALSE;
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	    EXA_prog_trigger = TRUE;
	    break;
	}
	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
	{
	    EXA_prog_trigger = FALSE;
	    sprintf( buf, "under %s", arg );
	    do_look( ch, buf );
	    EXA_prog_trigger = TRUE;
	}
	oprog_examine_trigger( ch, obj );
	if ( char_died(ch) || obj_extracted(obj) )
	    return;

	check_for_trap( ch, obj, TRAP_EXAMINE );
    }
    return;
}


void do_exits( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;

    set_char_color( AT_EXITS, ch );
    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind(ch) )
	return;

    strcpy( buf, fAuto ? "Exits:" : "Obvious exits:\n\r" );

    found = FALSE;
    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
    {
	if ( pexit->to_room
	&&  !IS_SET(pexit->exit_info, EX_CLOSED)
	&& (!IS_SET(pexit->exit_info, EX_WINDOW)
	||   IS_SET(pexit->exit_info, EX_ISDOOR))
	&&  !IS_SET(pexit->exit_info, EX_HIDDEN) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[pexit->vdir] );
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s\n\r",
		    capitalize( dir_name[pexit->vdir] ),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none.\n\r" : "None.\n\r" );
    else
      if ( fAuto )
	strcat( buf, ".\n\r" );
    send_to_char( buf, ch );
    return;
}

char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    extern char reboot_time[];
    char *suf;
    int day;

    day     = time_info.day + 1;

	 if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    set_char_color( AT_YELLOW, ch );
    ch_printf( ch,
	"It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r"  
        "The mud started up at:    %s\r"
        "The system time (E.S.T.): %s\r"
        "Next Reboot is set for:   %s\r",

	(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month],
	str_boot_time,
	(char *) ctime( &current_time ),
	reboot_time
	);

    return;
}

/*
void do_weather( CHAR_DATA *ch, char *argument )
{
    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the sky from here.\n\r", ch );
	return;
    }

    set_char_color( AT_BLUE, ch );
    ch_printf( ch, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    return;
}
*/

/*
 * Produce a description of the weather based on area weather using
 * the following sentence format:
 *		<combo-phrase> and <single-phrase>.
 * Where the combo-phrase describes either the precipitation and
 * temperature or the wind and temperature. The single-phrase
 * describes either the wind or precipitation depending upon the
 * combo-phrase.
 * Last Modified: July 31, 1997
 * Fireblade - Under Construction
 */
void do_weather(CHAR_DATA *ch, char *argument)
{
	char *combo, *single;
	char buf[MAX_INPUT_LENGTH];
	int temp, precip, wind;	

	if ( !IS_OUTSIDE(ch) )
	{
	    ch_printf(ch, "You can't see the sky from here.\n\r");
	    return;
	}

	temp = (ch->in_room->area->weather->temp + 3*weath_unit - 1)/
		weath_unit;
	precip = (ch->in_room->area->weather->precip + 3*weath_unit - 1)/
		weath_unit;
	wind = (ch->in_room->area->weather->wind + 3*weath_unit - 1)/
		weath_unit;
	
	if ( precip >= 3 )
	{
	    combo = preciptemp_msg[precip][temp];
	    single = wind_msg[wind];
	}
	else
	{
	    combo = windtemp_msg[wind][temp];
	    single = precip_msg[precip];
	}
	
	sprintf(buf, "%s and %s.\n\r", combo, single);
	
	set_char_color(AT_BLUE, ch);
	
	ch_printf(ch, buf);
}

/*
 * Moved into a separate function so it can be used for other things
 * ie: online help editing				-Thoric
 */
HELP_DATA *get_help( CHAR_DATA *ch, char *argument )
{
    char argall[MAX_INPUT_LENGTH];
    char argone[MAX_INPUT_LENGTH];
    char argnew[MAX_INPUT_LENGTH];
    HELP_DATA *pHelp;
    int lev;

    if ( argument[0] == '\0' )
	argument = "summary";

    if ( isdigit(argument[0]) )
    {
	lev = number_argument( argument, argnew );
	argument = argnew;
    }
    else
	lev = -2;
    /*
     * Tricky argument handling so 'help a b' doesn't match a.
     */
    argall[0] = '\0';
    while ( argument[0] != '\0' )
    {
	argument = one_argument( argument, argone );
	if ( argall[0] != '\0' )
	    strcat( argall, " " );
	strcat( argall, argone );
    }

    for ( pHelp = first_help; pHelp; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;
	if ( lev != -2 && pHelp->level != lev )
	    continue;

	if ( is_name( argall, pHelp->keyword ) )
	    return pHelp;
    }

    return NULL;
}

/*
 * LAWS command
 */
void do_laws( CHAR_DATA *ch, char *argument )
{
    char buf[1024];

    if ( argument == NULL)
	do_help( ch, "laws" );
    else
    {
	sprintf( buf, "law %s", argument );
	do_help( ch, buf );
    }
}

/*
 * Now this is cleaner
 */
void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;

    if ( (pHelp = get_help( ch, argument )) == NULL )
    {
	send_to_char( "No help on that word.\n\r", ch );
	return;
    }

    /* Make newbies do a help start. --Shaddai */ 
    if ( !IS_NPC(ch) && !str_cmp( argument, "start" ) )
	SET_BIT(ch->pcdata->flags, PCFLAG_HELPSTART);

    if ( pHelp->level >= 0 && str_cmp( argument, "imotd" ) )
    {
	set_pager_color( AT_WHITE, ch );
	send_to_pager( pHelp->keyword, ch );
	send_to_pager( "\n\r", ch );
    }

    /*
     * Strip leading '.' to allow initial blanks.
     */
    set_pager_color( AT_GREY, ch );
    if ( pHelp->text[0] == '.' )
	send_to_pager_color( pHelp->text+1, ch );
    else
	send_to_pager_color( pHelp->text  , ch );
    return;
}

extern char * help_greeting;	/* so we can edit the greeting online */

/*
 * Help editor							-Thoric
 */
void do_hedit( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor.\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_HELP_EDIT:
	  if ( (pHelp = ch->dest_buf) == NULL )
	  {
		bug( "hedit: sub_help_edit: NULL ch->dest_buf", 0 );
		stop_editing( ch );
		return;
	  }
	  if ( help_greeting == pHelp->text )
		help_greeting = NULL;
	  STRFREE( pHelp->text );
	  pHelp->text = copy_buffer( ch );
	  if ( !help_greeting )
		help_greeting = pHelp->text;
	  stop_editing( ch );
	  return;
    }
    if ( (pHelp = get_help(ch, argument)) == NULL )     /* new help */
    {
        HELP_DATA *tHelp;
        char argnew[MAX_INPUT_LENGTH];
        int lev;
        bool new_help = TRUE;

        for ( tHelp=first_help; tHelp; tHelp = tHelp->next )
           if ( !str_cmp( argument, tHelp->keyword) )
           {
                pHelp = tHelp;
                new_help = FALSE;
                break;
           }
        if ( new_help ) {
        if ( isdigit(argument[0]) )
        {
            lev = number_argument( argument, argnew );
            argument = argnew;
        }
        else
            lev = get_trust(ch);
        CREATE( pHelp, HELP_DATA, 1 );
        pHelp->keyword = STRALLOC( strupper(argument) );
        pHelp->text    = STRALLOC( "" );
        pHelp->level   = lev;
        add_help( pHelp );
        }
    }

    ch->substate = SUB_HELP_EDIT;
    ch->dest_buf = pHelp;
    start_editing( ch, pHelp->text );
}

/*
 * Stupid leading space muncher fix				-Thoric
 */
char *help_fix( char *text )
{
    char *fixed;

    if ( !text )
	return "";
    fixed = strip_cr(text);
    if ( fixed[0] == ' ' )
	fixed[0] = '.';
    return fixed;
}

void do_hset( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: hset <field> [value] [help page]\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  level keyword remove save\n\r",		ch );
	return;
    }

    if ( !str_cmp( arg1, "save" ) )
    {
	FILE *fpout;

	log_string_plus( "Saving help.are...", LOG_NORMAL, LEVEL_MASTER );

	rename( "help.are", "help.are.bak" );
	FCLOSE( fpReserve );
	if ( ( fpout = fopen( "help.are", "w" ) ) == NULL )
	{
	   bug( "hset save: fopen", 0 );
	   perror( "help.are" );
	   fpReserve = fopen( NULL_FILE, "r" );
	   return;
	}
    
	fprintf( fpout, "#HELPS\n\n" );
	for ( pHelp = first_help; pHelp; pHelp = pHelp->next )
	    fprintf( fpout, "%d %s~\n%s~\n\n",
		pHelp->level, pHelp->keyword, help_fix(pHelp->text) );

	fprintf( fpout, "0 $~\n\n\n#$\n" );
	FCLOSE( fpout );
	fpReserve = fopen( NULL_FILE, "r" );
	send_to_char( "Saved.\n\r", ch );
	return;
    }
    if ( str_cmp( arg1, "remove" ) )
	argument = one_argument( argument, arg2 );

    if ( (pHelp = get_help(ch, argument)) == NULL )
    {
	send_to_char( "Cannot find help on that subject.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "remove" ) )
    {
	UNLINK( pHelp, first_help, last_help, next, prev );
	STRFREE( pHelp->text );
	STRFREE( pHelp->keyword );
	DISPOSE( pHelp );
	send_to_char( "Removed.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "level" ) )
    {
	pHelp->level = atoi( arg2 );
	send_to_char( "Done.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "keyword" ) )
    {
	STRFREE( pHelp->keyword );
	pHelp->keyword = STRALLOC( strupper(arg2) );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    do_hset( ch, "" );
}

/*
 * Show help topics in a level range				-Thoric
 * Idea suggested by Gorog
 * prefix keyword indexing added by Fireblade
 */
void do_hlist( CHAR_DATA *ch, char *argument )
{
    int min, max, minlimit, maxlimit, cnt;
    char arg[MAX_INPUT_LENGTH];
    HELP_DATA *help;
    bool minfound, maxfound;
    char *idx;

    maxlimit = get_trust(ch);
    minlimit = maxlimit >= LEVEL_MASTER ? -1 : 0;
    
    min = minlimit;
    max  = maxlimit;
    
    idx = NULL;
    minfound = FALSE;
    maxfound = FALSE;
    
    for ( argument = one_argument(argument, arg); arg[0] != '\0';
	  argument = one_argument(argument, arg))
    {
	if( !isdigit(arg[0]) )
	{
 	    if ( idx )
	    {
		set_char_color(AT_GREEN, ch);
		ch_printf(ch, "You may only use a single keyword to index the list.\n\r");
		return;
	    }
	    idx = STRALLOC(arg);
    	}
	else
	{
	    if ( !minfound )
	    {
		min = URANGE(minlimit, atoi(arg), maxlimit);
		minfound = TRUE;
	    }
	    else
	    if ( !maxfound )
	    {
		max = URANGE(minlimit, atoi(arg), maxlimit);
		maxfound = TRUE;
	    }
	    else
	    {
		set_char_color(AT_GREEN, ch);
		ch_printf(ch, "You may only use two level limits.\n\r");
		return;
	    }
	}
    }
    
    if ( min > max )
    {
	int temp = min;

	min = max;
	max = temp;
    }
    
    set_pager_color( AT_GREEN, ch );
    pager_printf( ch, "Help Topics in level range %d to %d:\n\r\n\r", min, max );
    for ( cnt = 0, help = first_help; help; help = help->next )
	if ( help->level >= min && help->level <= max
	&&  (!idx || nifty_is_name_prefix(idx, help->keyword)) )
	{
	    pager_printf( ch, "  %3d %s\n\r", help->level, help->keyword );
	    ++cnt;
	}
    if ( cnt )
	pager_printf( ch, "\n\r%d pages found.\n\r", cnt );
    else
	send_to_char( "None found.\n\r", ch );

    if ( idx )
    	STRFREE(idx);
    
    return;
}


/* 
 * New do_who with WHO REQUEST, clan, race and homepage support.  -Thoric
 *
 * Latest version of do_who eliminates redundant code by using linked lists.
 * Shows imms separately, indicates guest and retired immortals.
 * Narn, Oct/96
 *
 * Who group by Altrag, Feb 28/97
 */
struct whogr_s
{
  struct whogr_s *next;
  struct whogr_s *follower;
  struct whogr_s *l_follow;
  DESCRIPTOR_DATA *d;
  int indent;
} *first_whogr, *last_whogr;

struct whogr_s *find_whogr(DESCRIPTOR_DATA *d, struct whogr_s *first)
{
  struct whogr_s *whogr, *whogr_t;
  
  for (whogr = first; whogr; whogr = whogr->next)
    if (whogr->d == d)
      return whogr;
    else if (whogr->follower && (whogr_t = find_whogr(d, whogr->follower)))
      return whogr_t;
  return NULL;
}

void indent_whogr(CHAR_DATA *looker, struct whogr_s *whogr, int ilev)
{
  for ( ; whogr; whogr = whogr->next )
  {
    if (whogr->follower)
    {
      int nlev = ilev;
      CHAR_DATA *wch =
          (whogr->d->original ? whogr->d->original : whogr->d->character);
      
      if (can_see(looker, wch) && !IS_IMMORTAL(wch))
        nlev += 3;
      indent_whogr(looker, whogr->follower, nlev);
    }
    whogr->indent = ilev;
  }
}

/* This a great big mess to backwards-structure the ->leader character
   fields */
void create_whogr(CHAR_DATA *looker)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *wch;
  struct whogr_s *whogr, *whogr_t;
  int dc = 0, wc = 0;
  
  while ((whogr = first_whogr) != NULL)
  {
    first_whogr = whogr->next;
    DISPOSE(whogr);
  }
  first_whogr = last_whogr = NULL;
  /* Link in the ones without leaders first */
  for (d = last_descriptor; d; d = d->prev)
  {
    if (d->connected != CON_PLAYING && d->connected != CON_EDITING)
      continue;
    ++dc;
    wch = (d->original ? d->original : d->character);
    if (!wch->leader || wch->leader == wch || !wch->leader->desc ||
         IS_NPC(wch->leader) || IS_IMMORTAL(wch) || IS_IMMORTAL(wch->leader))
    {
      CREATE(whogr, struct whogr_s, 1);
      if (!last_whogr)
        first_whogr = last_whogr = whogr;
      else
      {
        last_whogr->next = whogr;
        last_whogr = whogr;
      }
      whogr->next = NULL;
      whogr->follower = whogr->l_follow = NULL;
      whogr->d = d;
      whogr->indent = 0;
      ++wc;
    }
  }
  /* Now for those who have leaders.. */
  while (wc < dc)
    for (d = last_descriptor; d; d = d->prev)
    {
      if (d->connected != CON_PLAYING && d->connected != CON_EDITING)
        continue;
      if (find_whogr(d, first_whogr))
        continue;
      wch = (d->original ? d->original : d->character);
      if (wch->leader && wch->leader != wch && wch->leader->desc &&
         !IS_NPC(wch->leader) && !IS_IMMORTAL(wch) &&
         !IS_IMMORTAL(wch->leader) &&
         (whogr_t = find_whogr(wch->leader->desc, first_whogr)))
      {
        CREATE(whogr, struct whogr_s, 1);
        if (!whogr_t->l_follow)
          whogr_t->follower = whogr_t->l_follow = whogr;
        else
        {
          whogr_t->l_follow->next = whogr;
          whogr_t->l_follow = whogr;
        }
        whogr->next = NULL;
        whogr->follower = whogr->l_follow = NULL;
        whogr->d = d;
        whogr->indent = 0;
        ++wc;
      }
    }
  /* Set up indentation levels */
  indent_whogr(looker, first_whogr, 0);
  
  /* And now to linear link them.. */
  for (whogr_t = NULL, whogr = first_whogr; whogr; )
    if (whogr->l_follow)
    {
      whogr->l_follow->next = whogr;
      whogr->l_follow = NULL;
      if (whogr_t)
        whogr_t->next = whogr = whogr->follower;
      else
        first_whogr = whogr = whogr->follower;
    }
    else
    {
      whogr_t = whogr;
      whogr = whogr->next;
    }
}

void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char faction_name[MAX_INPUT_LENGTH];
    char council_name[MAX_INPUT_LENGTH];
    char invis_str[MAX_INPUT_LENGTH];
    char char_name[MAX_INPUT_LENGTH];
    char *extra_title;
    char class_text[MAX_INPUT_LENGTH];
    struct whogr_s *whogr, *whogr_p;
    DESCRIPTOR_DATA *d;
    int iClass, iRace;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_RACE];
    bool fClassRestrict;
    bool fRaceRestrict;
    bool fImmortalOnly;
    bool fLeader;
    bool fPkill;
    bool fShowHomepage;
    bool fFactionMatch; /* SB who clan (order),who guild, and who council */
    bool fCouncilMatch;
    bool fDeityMatch;
    bool fGroup;
    FACTION_DATA *pFaction = NULL;
    COUNCIL_DATA *pCouncil = NULL;
    DEITY_DATA *pDeity = NULL;

    WHO_DATA *cur_who = NULL;
    WHO_DATA *next_who = NULL;
    WHO_DATA *first_mortal = NULL;
    WHO_DATA *first_imm = NULL;
    WHO_DATA *first_deadly  = NULL;
    WHO_DATA *first_grouped = NULL;
    WHO_DATA *first_groupwho = NULL;


    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    fClassRestrict = FALSE;
    fRaceRestrict  = FALSE;
    fImmortalOnly  = FALSE;
    fPkill         = FALSE;
    fShowHomepage  = FALSE;
    fFactionMatch  = FALSE; /* SB who clan (order), who guild, who council */
    fCouncilMatch  = FALSE;
    fDeityMatch    = FALSE;
    fGroup	   = FALSE; /* Alty who group */
    fLeader	   = FALSE;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	rgfClass[iClass] = FALSE;
    for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	rgfRace[iRace] = FALSE;

    /*
     * The who command must have at least one argument because we often
     * have up to 500 players on. Too much spam if a player accidentally
     * types "who" with no arguments.           --Gorog
     */
    if ( sysdata.reqwho_arg == TRUE && ch != NULL && argument[0] == '\0' )
    {
        send_to_pager_color(
        "\n\r&GYou must specify at least one argument.\n\rUse 'who 1' to view the entire who list.\n\r", ch);
        return;
    }
	
    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    switch ( ++nNumber )
	    {
	    case 1: iLevelLower = atoi( arg ); break;
	    case 2: iLevelUpper = atoi( arg ); break;
	    default:
		send_to_char( "Only two level numbers allowed.\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( strlen(arg) < 3 )
	    {
		send_to_char( "Arguments must be longer than that.\n\r", ch );
		return;
	    }

	    /*
	     * Look for classes to turn on.
	     */
            if ( !str_cmp( arg, "deadly" ) || !str_cmp( arg, "pkill" ) )
              fPkill = TRUE;
            else
	    if ( !str_cmp( arg, "imm" ) || !str_cmp( arg, "gods" ) )
		fImmortalOnly = TRUE;
	    else
	    if ( !str_cmp( arg, "leader" ) )
	    	fLeader = TRUE;
	    else
	    if ( !str_cmp( arg, "www" ) )
		fShowHomepage = TRUE;
	    else
	    if ( !str_cmp( arg, "group" ) && ch )
		fGroup = TRUE;
            else		 /* SB who clan (order), guild, council */
             if  ( ( pFaction = get_faction(arg) ) )
	   	fFactionMatch = TRUE;
            else
             if ( ( pCouncil = get_council (arg) ) )
                fCouncilMatch = TRUE;
	    else
	     if ( ( pDeity = get_deity (arg) ) )
		fDeityMatch = TRUE;
	    else
	    {
		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
		    if ( !str_cmp( arg, class_table[iClass]->who_name ) )
		    {
			rgfClass[iClass] = TRUE;
			break;
		    }
		}
		if ( iClass != MAX_CLASS )
		  fClassRestrict = TRUE;

		for ( iRace = 0; iRace < MAX_RACE; iRace++ )
		{
		    if ( !str_cmp( arg, race_table[iRace]->race_name ) )
		    {
			rgfRace[iRace] = TRUE;
			break;
		    }
		}
		if ( iRace != MAX_RACE )
		  fRaceRestrict = TRUE;

		if ( iClass == MAX_CLASS && iRace == MAX_RACE 
 		 && fFactionMatch == FALSE 
                 && fCouncilMatch == FALSE
		 && fDeityMatch == FALSE )
		{
		 send_to_char( "That's not a class, race, order, guild,"
			" council or deity.\n\r", ch );
		    return;
		}
	    }
	}
    }

    /*
     * Now find matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    set_pager_color( AT_GREEN, ch );

    /*
     * Start from the last and proceed to the first to get it in the proper order. -Orion
     */
    if ( fGroup )
    {
	create_whogr( ch );
	whogr = first_whogr;
	d = whogr->d;
    }
    else
    {
	whogr = NULL;
	d = last_descriptor;
    }
    whogr_p = NULL;
    for ( ; d; whogr_p = whogr, whogr = (fGroup ? whogr->next : NULL),
    			d = (fGroup ? (whogr ? whogr->d : NULL) : d->prev))
    {
	CHAR_DATA *wch;
	char const *class;

	if ( (d->connected != CON_PLAYING && d->connected != CON_EDITING) ||
	      !can_see( ch, d->character ) || d->original)
	    continue;
	wch   = d->original ? d->original : d->character;
	if ( wch->level < iLevelLower || wch->level > iLevelUpper ||
	     ( fPkill && !CAN_PKILL( wch ) ) ||
	     ( fImmortalOnly && wch->level < LEVEL_IMMORTAL ) ||
	     ( fClassRestrict && !rgfClass[wch->class] ) ||
	     ( fRaceRestrict && !rgfRace[wch->race] ) ||
	     ( fFactionMatch && ( pFaction != wch->pcdata->faction )) ||
	     ( fCouncilMatch && ( pCouncil != wch->pcdata->council )) ||
	     ( fDeityMatch && ( pDeity != wch->pcdata->deity )) )
	    continue;
	if ( fLeader && !(wch->pcdata->council && 
	     ((wch->pcdata->council->head2 &&
	     !str_cmp(wch->pcdata->council->head2, wch->name)) ||
	     (wch->pcdata->council->head &&
	     !str_cmp(wch->pcdata->council->head, wch->name)))) &&
	     !(wch->pcdata->faction && ((wch->pcdata->faction->deity &&
	     !str_cmp(wch->pcdata->faction->deity,wch->name) )
	     || (wch->pcdata->faction->leader
	     && !str_cmp(wch->pcdata->faction->leader, wch->name ) )
	     || (wch->pcdata->faction->number1
	     && !str_cmp(wch->pcdata->faction->number1, wch->name ) )
	     || (wch->pcdata->faction->number2 
	     && !str_cmp(wch->pcdata->faction->number2, wch->name )))))
	    continue;

	if ( fGroup && !wch->leader &&
	     !IS_SET(wch->pcdata->flags, PCFLAG_GROUPWHO) &&
	     (!whogr_p || !whogr_p->indent))
	    continue;

	nMatch++;

	if ( fShowHomepage
	&&   wch->pcdata->homepage
	&&   wch->pcdata->homepage[0] != '\0' )
	{
	    sprintf( char_name, "%s (%s)", wch->name, show_tilde( wch->pcdata->homepage ) );
	}
	else
	  strcpy( char_name, wch->name );

	sprintf( class_text, "%s%2d %s", NOT_AUTHED(wch) ? "N" : " ", wch->level, class_table[wch->class]->who_name );
	class = class_text;
	switch ( wch->level )
	{
	default: break;
	case MAX_LEVEL		: class = "Supreme Entity";	break;
	case LEVEL_HIGH_ADMIN	: class = "High Admin";		break;
	case LEVEL_MASTER	: class = "Master";		break;
	case LEVEL_WORKER	: class = "Worker";		break;
	case LEVEL_SLAVE	: class = "Slave";		break;
	}

        if ( !str_cmp( wch->name, sysdata.guild_overseer ) )
          extra_title = " [Overseer of Guilds]";
        else if ( !str_cmp( wch->name, sysdata.guild_advisor ) )
          extra_title = " [Advisor to Guilds]";
        else
          extra_title = "";
 
        if ( IS_RETIRED( wch ) )
          class = "Retired"; 
        else if ( IS_GUEST( wch ) )
          class = "Guest";
	else if ( wch->pcdata->faction
             &&  !str_cmp( wch->name, wch->pcdata->faction->leader )
             &&   wch->pcdata->faction->leadrank[0] != '\0' )
                class = wch->pcdata->faction->leadrank;
        else if ( wch->pcdata->faction
	     &&  !str_cmp( wch->name, wch->pcdata->faction->number1 )
             &&   wch->pcdata->faction->onerank[0] != '\0' )
                class = wch->pcdata->faction->onerank;
	else if ( wch->pcdata->faction
             &&  !str_cmp( wch->name, wch->pcdata->faction->number2 )
             &&   wch->pcdata->faction->tworank[0] != '\0' )
                class = wch->pcdata->faction->tworank;
  	else if ( wch->pcdata->rank && wch->pcdata->rank[0] != '\0' )
	  class = wch->pcdata->rank;

	if ( wch->pcdata->faction )
	{
          FACTION_DATA *pfaction = wch->pcdata->faction;
	  if ( pfaction->faction_type == FACTION_GUILD )
	    strcpy( faction_name, " <" );
	  else if ( pfaction->faction_type == FACTION_ORDER )
	    strcpy( faction_name, " [" );
	  else
	    strcpy( faction_name, " (" );

          if ( pfaction->faction_type == FACTION_ORDER )
          {
	    if ( !str_cmp( wch->name, pfaction->deity ) )
	      strcat( faction_name, "Deity, Order of " );
            else if ( !str_cmp( wch->name, pfaction->leader ) )
	      strcat( faction_name, "Leader, Order of " );
	    else if ( !str_cmp( wch->name, pfaction->number1 ) )
	      strcat( faction_name, "Number One, Order of " );
	    else if ( !str_cmp( wch->name, pfaction->number2 ) )
	      strcat( faction_name, "Number Two, Order of " );
	    else
	      strcat( faction_name, "Order of " );
          }
          else if ( pfaction->faction_type == FACTION_GUILD )
	  {
	    if ( !str_cmp( wch->name, pfaction->leader ) )
              strcat( faction_name, "Leader, " );
            else if ( !str_cmp( wch->name, pfaction->number1 ) )
              strcat( faction_name, "First, " );
            else if ( !str_cmp( wch->name, pfaction->number2 ) )
              strcat( faction_name, "Second, " );
	  }
	  else
          {
	    if ( !str_cmp( wch->name, pfaction->deity ) )
	      strcat( faction_name, "Deity of " );
	    else if ( !str_cmp( wch->name, pfaction->leader ) )
	      strcat( faction_name, "Leader of " );
	    else if ( !str_cmp( wch->name, pfaction->number1 ) )
	      strcat( faction_name, "Number One of " );
	    else if ( !str_cmp( wch->name, pfaction->number2 ) )
	      strcat( faction_name, "Number Two of " );
          } 
	  strcat( faction_name, pfaction->name );

	  if ( pfaction->faction_type == FACTION_GUILD )
	    strcat( faction_name, ">" );
	  else if ( pfaction->faction_type == FACTION_ORDER )
	    strcat( faction_name, "]" );
	  else
	    strcat( faction_name, ")" );
	}
	else
	  faction_name[0] = '\0';

	if ( wch->pcdata->council )
	{
          strcpy( council_name, " [" );
          if (  wch->pcdata->council->head2 == NULL )
          {
             if (!str_cmp (wch->name, wch->pcdata->council->head))
                strcat (council_name, "Head of ");
          }
          else
          {
             if (!str_cmp (wch->name, wch->pcdata->council->head)
                 || !str_cmp ( wch->name, wch->pcdata->council->head2) )
                strcat (council_name, "Co-Head of ");
          }
          strcat( council_name, wch->pcdata->council_name );
          strcat( council_name, "]" );
	}
	else
          council_name[0] = '\0';

	if ( xIS_SET(wch->act, PLR_WIZINVIS) )
	  sprintf( invis_str, "(%d) ", wch->pcdata->wizinvis );
	else
	  invis_str[0] = '\0';
	sprintf( buf, "%*s%-15s %s%s%s%s%s%s%s%s%s.%s%s%s\n\r",
	  (fGroup ? whogr->indent : 0), "",
	  class,
	  invis_str,
          (wch->desc && wch->desc->connected) ? "[WRITING] " : "",
	  IS_QUESTING( wch ) ? "[QUESTING] " : "",
	  xIS_SET(wch->act, PLR_AFK) ? "[AFK] " : "",
	  xIS_SET(wch->act, PLR_ATTACKER) ? "(ATTACKER) " : "",
	  xIS_SET(wch->act, PLR_KILLER) ? "(KILLER) " : "",
	  xIS_SET(wch->act, PLR_THIEF)  ? "(THIEF) "  : "",
	  char_name,
	  wch->pcdata->title,
          extra_title,
	  faction_name,
	  council_name );

          /*  
           * This is where the old code would display the found player to the ch.
           * What we do instead is put the found data into a linked list
           */ 

          /* First make the structure. */
          CREATE( cur_who, WHO_DATA, 1 );
          cur_who->text = str_dup( buf );
          if ( IS_IMMORTAL( wch ) )
            cur_who->type = WT_IMM;
          else if ( fGroup )
            if ( wch->leader || (whogr_p && whogr_p->indent) )
              cur_who->type = WT_GROUPED;
            else
              cur_who->type = WT_GROUPWHO;
          else if ( CAN_PKILL( wch ) ) 
            cur_who->type = WT_DEADLY;
          else
            cur_who->type = WT_MORTAL;

          /* Then put it into the appropriate list. */
          switch ( cur_who->type )
          {
            case WT_MORTAL:
              cur_who->next = first_mortal;
              first_mortal = cur_who;
              break;
            case WT_DEADLY:
              cur_who->next = first_deadly;
              first_deadly = cur_who;
              break;
            case WT_GROUPED:
              cur_who->next = first_grouped;
              first_grouped = cur_who;
              break;
            case WT_GROUPWHO:
              cur_who->next = first_groupwho;
              first_groupwho = cur_who;
              break;
            case WT_IMM:
              cur_who->next = first_imm;
              first_imm = cur_who;
              break;
          }

    }


    /* Ok, now we have three separate linked lists and what remains is to 
     * display the information and clean up.
     */
    /*
     * Two extras now for grouped and groupwho (wanting group). -- Alty
     */

    if ( first_mortal )
    {
	set_pager_color( AT_DBLUE,	ch );
	send_to_pager( "\n\r[  ",	ch );
	set_pager_color( AT_BLUE,	ch );
	send_to_pager( "Mortal  ",	ch );
	set_pager_color( AT_DBLUE,	ch );
	send_to_pager( "]\n\r",		ch );
    }    

    for ( cur_who = first_mortal; cur_who; cur_who = next_who )
    {
	set_pager_color( AT_BLUE,	ch );
	send_to_pager( cur_who->text,	ch );
	next_who = cur_who->next;
	DISPOSE( cur_who->text );
	DISPOSE( cur_who ); 
    } 

    if ( first_deadly )
    {
	set_pager_color( AT_BLOOD,	ch );
	send_to_pager( "\n\r[  ",	ch );
	set_pager_color( AT_RED,	ch );
	send_to_pager( "Deadly  ",	ch );
	set_pager_color( AT_BLOOD,	ch );
	send_to_pager( "]\n\r",		ch );
    }

    for ( cur_who = first_deadly; cur_who; cur_who = next_who )
    {
	set_pager_color( AT_RED,	ch );
	send_to_pager( cur_who->text,	ch );
	next_who = cur_who->next;
	DISPOSE( cur_who->text );
	DISPOSE( cur_who ); 
    }
    
    if (first_grouped)
    {
	set_pager_color( AT_DGREEN,	ch );
	send_to_pager( "\n\r[ ",	ch );
	set_pager_color( AT_GREEN,	ch );
	send_to_pager( "Grouped! ",	ch );
	set_pager_color( AT_DGREEN,	ch );
	send_to_pager( "]\n\r",		ch );
    }

    for ( cur_who = first_grouped; cur_who; cur_who = next_who )
    {
	set_pager_color( AT_GREEN,	ch );
	send_to_pager( cur_who->text,	ch );
	next_who = cur_who->next;
	DISPOSE( cur_who->text );
	DISPOSE( cur_who ); 
    }

    if (first_groupwho)
    {
	set_pager_color( AT_PURPLE,	ch );
	send_to_pager( "\n\r[ ",	ch );
	set_pager_color( AT_PINK,	ch );
	send_to_pager( "Wanting! ",	ch );
	set_pager_color( AT_PURPLE,	ch );
	send_to_pager( "]\n\r",		ch );
    }

    for ( cur_who = first_groupwho; cur_who; cur_who = next_who )
    {
	set_pager_color( AT_PINK, 	ch );
	send_to_pager( cur_who->text,	ch );
	next_who = cur_who->next;
	DISPOSE( cur_who->text );
	DISPOSE( cur_who ); 
    }

    if ( first_imm )
    {
	set_pager_color( AT_ORANGE,	ch );
	send_to_pager( "\n\r[ ",	ch );
	set_pager_color( AT_YELLOW,	ch );
	send_to_pager( "Immortal ",	ch );
	set_pager_color( AT_ORANGE,	ch );
	send_to_pager( "]\n\r",		ch );
    }

    for ( cur_who = first_imm; cur_who; cur_who = next_who )
    {
	set_pager_color( AT_YELLOW,	ch );
	send_to_pager( cur_who->text,	ch );
	next_who = cur_who->next;
	DISPOSE( cur_who->text );
	DISPOSE( cur_who ); 
    } 

    set_pager_color( AT_CYAN,	ch );
    send_to_pager( "\n\r[ ",	ch );
    set_pager_color( AT_LBLUE,	ch );
    pager_printf( ch, "%s Player%s ", num_punct( nMatch ), nMatch == 1 ? "" : "s" );
    set_pager_color( AT_CYAN,	ch );
    send_to_pager( "]\n\r",	ch );
    set_pager_color( AT_GREEN,	ch ); 
    return;
}


void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->first_carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
	    &&   can_see_obj( ch, obj2 )
	    &&   obj1->item_type == obj2->item_type
	    && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( !obj2 )
	{
	    send_to_char( "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

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
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_WEAPON:
	    value1 = obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( !msg )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( AT_PLAIN, msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0]!='\0' 
    &&   (victim=get_char_world(ch, arg)) && !IS_NPC(victim)
    &&   IS_SET(victim->pcdata->flags, PCFLAG_DND)
    &&   get_trust(ch) < get_trust(victim) )
    {
         act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
         return;
   }

    set_pager_color( AT_PERSON, ch );
    if ( arg[0] == '\0' )
    {
        pager_printf( ch, "\n\rPlayers near you in %s:\n\r", ch->in_room->area->name );
	found = FALSE;
	for ( d = first_descriptor; d; d = d->next )
	    if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room
	    &&   victim->in_room->area == ch->in_room->area
	    &&   can_see( ch, victim )
            && ( get_trust(ch) >= get_trust(victim) 
            ||   !IS_SET(victim->pcdata->flags, PCFLAG_DND) ) 
               ) /* if victim has the DND flag ch must outrank them */ 

	    {
		found = TRUE;
		pager_printf_color( ch, "&P%-13s  ", victim->name );
		if ( IS_IMMORTAL( victim ) )
		  send_to_pager_color( "&P(&WImmortal&P)\t", ch );
		else if ( CAN_PKILL( victim ) && IS_CLANNED( victim ) )
		  pager_printf_color( ch, "%-18s\t", victim->pcdata->faction->badge );
		else if ( CAN_PKILL( victim ) )
		  send_to_pager_color( "(&wUnclanned&P)\t", ch );
		else
		  send_to_pager( "\t\t\t", ch );
		pager_printf_color( ch, "&P%s\n\r", victim->in_room->name );
	    }
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = first_char; victim; victim = victim->next )
	    if ( victim->in_room
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		pager_printf( ch, "%-28s %s\n\r",
		    PERS(victim, ch), victim->in_room->name );
		break;
	    }
	if ( !found )
	    act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "You decide you're pretty sure you could take yourself in a fight.\n\r", ch );
	return;
    }
    diff = victim->level - ch->level;

	 if ( diff <= -10 ) msg = "You are far more experienced than $N.";
    else if ( diff <=  -5 ) msg = "$N is not nearly as experienced as you.";
    else if ( diff <=  -2 ) msg = "You are more experienced than $N.";
    else if ( diff <=   1 ) msg = "You are just about as experienced as $N.";
    else if ( diff <=   4 ) msg = "You are not nearly as experienced as $N.";
    else if ( diff <=   9 ) msg = "$N is far more experienced than you!";
    else                    msg = "$N would make a great teacher for you!";
    act( AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR );

    diff = (int) (victim->max_hit - ch->max_hit) / 6;

	 if ( diff <= -200) msg = "$N looks like a feather!";
    else if ( diff <= -150) msg = "You could kill $N with your hands tied!";
    else if ( diff <= -100) msg = "Hey! Where'd $N go?";
    else if ( diff <=  -50) msg = "$N is a wimp.";
    else if ( diff <=    0) msg = "$N looks weaker than you.";
    else if ( diff <=   50) msg = "$N looks about as strong as you.";
    else if ( diff <=  100) msg = "It would take a bit of luck...";
    else if ( diff <=  150) msg = "It would take a lot of luck, and equipment!";
    else if ( diff <=  200) msg = "Why don't you dig a grave for yourself first?";
    else                    msg = "$N is built like a TANK!";
    act( AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR );

    return;
}



/*
 * Place any skill types you don't want them to be able to practice
 * normally in this list.  Separate each with a space.
 * (Uses an is_name check). -- Altrag
 */
#define CANT_PRAC "Tongue"

void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int	col;
	sh_int	lasttype, cnt;

	col = cnt = 0;
	lasttype = SKILL_SPELL;
	set_pager_color( AT_MAGIC, ch );
	for ( sn = 0; sn < top_sn; sn++ )
	{
	    if ( !skill_table[sn]->name )
		break;

	    if ( strcmp(skill_table[sn]->name, "reserved") == 0
	    && ( IS_IMMORTAL(ch) || CAN_CAST(ch) ) )
	    {
		if ( col % 3 != 0 )
		    send_to_pager( "\n\r", ch );

		set_pager_color( AT_MAGIC,			ch );
		send_to_pager_color( "                               _.:[",	ch );
		set_pager_color( AT_SCORE,			ch );
		send_to_pager_color( " Magicks ",		ch );
		set_pager_color( AT_MAGIC,			ch );
		send_to_pager_color( "]:._\n\r",		ch );

		col = 0;
	    }
	    if ( skill_table[sn]->type != lasttype )
	    {
		int count, type_length;
		char space_buf[MAX_STRING_LENGTH];

		if ( !cnt )
		    send_to_pager( "                                   (none)\n\r", ch );
		else if ( col % 3 != 0 )
		    send_to_pager( "\n\r", ch );

		space_buf[0] = '\0';

		type_length = ((79 - (strlen(skill_tname[skill_table[sn]->type]) + 11))/2);

		for( count = 0; count < type_length; count++ )
		{
		    strcat( space_buf, " " );
		}

		set_pager_color( AT_MAGIC,			ch );
		pager_printf_color( ch, "%s_.:[", space_buf );
		set_pager_color( AT_SCORE,			ch );
		pager_printf_color( ch, " %ss ", !str_cmp( skill_tname[skill_table[sn]->type], "skill" ) ? "Abilitie" : skill_tname[skill_table[sn]->type] );
		set_pager_color( AT_MAGIC,			ch );
		send_to_pager_color( "]:._\n\r",		ch );

		col = cnt = 0;
	    }
	    lasttype = skill_table[sn]->type;

	    if ( ch->level < skill_table[sn]->skill_level[ch->class]
	    || (!IS_IMMORTAL(ch) && skill_table[sn]->skill_level[ch->class] == 0) )
 	        continue;

	    if ( ch->pcdata->learned[sn] <= 0
	    &&   SPELL_FLAG(skill_table[sn], SF_SECRETSKILL) )
		continue;

	    ++cnt;
	    set_pager_color( AT_MAGIC, ch );
	    pager_printf( ch, "%20.20s", skill_table[sn]->name );
	    if ( ch->pcdata->learned[sn] >= skill_table[sn]->skill_adept[ch->class] )
		set_pager_color( AT_WHITE, ch );
	    else if ( ch->pcdata->learned[sn] > 0 )
		set_pager_color( AT_SCORE, ch );
	    else
		set_pager_color( AT_MAGIC, ch );
	    pager_printf( ch, " %3d%% ", ch->pcdata->learned[sn] );
	    if ( ++col % 3 == 0 )
		send_to_pager( "\n\r", ch );
	}

	if ( col % 3 != 0 )
	    send_to_pager( "\n\r", ch );
        set_pager_color( AT_MAGIC, ch );
	pager_printf( ch, "You have %s practice sessions left.\n\r",
	    ch->practice > 0 ? num_punct( ch->practice ) : "no" );
    }
    else
    {
	CHAR_DATA *mob;
	int adept;
	bool can_prac = TRUE;

	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
	    if ( IS_NPC(mob) && xIS_SET(mob->act, ACT_PRACTICE) )
		break;

	if ( !mob )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    act( AT_TELL, "$n tells you 'You must earn some more practice sessions.'",
		mob, NULL, ch, TO_VICT );
	    return;
	}

	sn = skill_lookup( argument );

	if ( can_prac 
          && ( ( sn == -1 )
       	       || ( !IS_NPC(ch)
	            &&  ch->level < skill_table[sn]->skill_level[ch->class] 
/* OUT FOR THIS PORT -SHADDAI
                    &&  ch->level < skill_table[sn]->race_level[ch->race]  
*/
                  ) 
             ) 
        )
	{
	    act( AT_TELL, "$n tells you 'You're not ready to learn that yet...'",
		mob, NULL, ch, TO_VICT );
	    return;
	}

	if ( is_name( skill_tname[skill_table[sn]->type], CANT_PRAC ) )
	{
	    act( AT_TELL, "$n tells you 'I do not know how to teach that.'",  
		  mob, NULL, ch, TO_VICT );
	    return;
	}

	/*
	 * Skill requires a special teacher
	 */
	if ( skill_table[sn]->teachers && skill_table[sn]->teachers[0] != '\0' )
	{
	    sprintf( buf, "%d", mob->pIndexData->vnum );
	    if ( !is_name( buf, skill_table[sn]->teachers ) )
	    {
		act( AT_TELL, "$n tells you, 'I know not know how to teach that.'",
		    mob, NULL, ch, TO_VICT );
		return;
	    }
	}

/*
 * Guild checks - right now, cant practice guild skills - done on 
 * induct/outcast
 *	
	if ( !IS_NPC(ch) && !IS_GUILDED(ch)
	&&    skill_table[sn]->guild != CLASS_NONE)
	{
	    act( AT_TELL, "$n tells you 'Only guild members can use that..'"
		mob, NULL, ch, TO_VICT );
	    return;
	}

	if ( !IS_NPC(ch) && skill_table[sn]->guild != CLASS_NONE 
	     && ch->pcdata->faction->class != skill_table[sn]->guild )
	{
	    act( AT_TELL, "$n tells you 'That can not be used by your guild.'"
		mob, NULL, ch, TO_VICT );
	    return;
	}
 */

	/*
	 * Disabled for now
	if ( mob->level < skill_table[sn]->skill_level[ch->class]
	||   mob->level < skill_table[sn]->skill_level[mob->class] )
	{
	    act( AT_TELL, "$n tells you 'You must seek another to teach you that...'",
		mob, NULL, ch, TO_VICT );
	    return;
	}
	 */

	adept = class_table[ch->class]->skill_adept * 0.2;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "$n tells you, 'I've taught you everything I can about %s.'",
		skill_table[sn]->name );
	    act( AT_TELL, buf, mob, NULL, ch, TO_VICT );
	    act( AT_TELL, "$n tells you, 'You'll have to practice it on your own now...'",
		mob, NULL, ch, TO_VICT );
	}
	else
	{
	    ch->practice--;
	    ch->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
	    act( AT_ACTION, "You practice $T.",
		    ch, NULL, skill_table[sn]->name, TO_CHAR );
	    act( AT_ACTION, "$n practices $T.",
		    ch, NULL, skill_table[sn]->name, TO_ROOM );
	    if ( ch->pcdata->learned[sn] >= adept )
	    {
		ch->pcdata->learned[sn] = adept;
		act( AT_TELL,
		 "$n tells you. 'You'll have to practice it on your own now...'",
		 mob, NULL, ch, TO_VICT );
	    }
	}
    }
    return;
}


void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    set_char_color( AT_YELLOW, ch );
    one_argument( argument, arg );
    if ( !str_cmp( arg, "max" ) )
    {
      if ( IS_PKILL( ch ) )
	wimpy = (int) ch->max_hit / 2.25;
      else
	wimpy = (int) ch->max_hit / 1.2;
    }
    else    
    if ( arg[0] == '\0' )
      wimpy = (int) ch->max_hit / 5;
    else
      wimpy = atoi( arg );

    if ( wimpy < 0 ) {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }
    if ( IS_PKILL( ch ) && wimpy > (int) ch->max_hit / 2.25 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }
    else if ( wimpy > (int) ch->max_hit / 1.2 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }
    ch->wimpy	= wimpy;
    ch_printf( ch, "Wimpy set to %d hit points.\n\r", wimpy );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char log_buf[MAX_STRING_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
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
    while ( isspace(*argument) )
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
	send_to_char( "Syntax: password <new> <again>.\n\r", ch );
	send_to_char( "Syntax: password <new> <again>.\n\r", ch );
	return;
    }

/*
    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }
*/

/* This should stop all the mistyped password problems --Shaddai */
    if ( strcmp( arg1, arg2 ))
    {
	send_to_char("Passwords don't match try again.\n\r", ch );
	return;
    }
    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    DISPOSE( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    if ( xIS_SET( sysdata.save_flags, SV_PASSCHG ) )
	save_char_obj( ch );
    if ( ch->desc && ch->desc->host[0]  != '\0' )
       sprintf(log_buf, "%s changing password from site %s\n", ch->name, 
       		ch->desc->host );
    else
       sprintf(log_buf, "%s changing thier password with no descriptor!", ch->name);
    log_string( log_buf );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
    int iHash;
    int col = 0;
    SOCIALTYPE *social;

    set_pager_color( AT_PLAIN, ch );
    for ( iHash = 0; iHash < 27; iHash++ )
	for ( social = social_index[iHash]; social; social = social->next )
	{
	    pager_printf( ch, "%-12s", social->name );
	    if ( ++col % 6 == 0 )
		send_to_pager( "\n\r", ch );
	}

    if ( col % 6 != 0 )
	send_to_pager( "\n\r", ch );
    return;
}


void do_commands( CHAR_DATA *ch, char *argument )
{
    int col;
    bool found;
    int hash;
    CMDTYPE *command;

    col = 0;
    set_pager_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' )
    {
	for ( hash = 0; hash < 126; hash++ )
	    for ( command = command_hash[hash]; command; command = command->next )
		if ( command->level <  LEVEL_HERO
		&&   command->level <= get_trust( ch )
		&&   strncmp( "do_mp", command->code, 5 ) )
		{
		    pager_printf( ch, "%-12s", command->name );
		    if ( ++col % 6 == 0 )
			send_to_pager( "\n\r", ch );
		}
	if ( col % 6 != 0 )
	    send_to_pager( "\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( hash = 0; hash < 126; hash++ )
	    for ( command = command_hash[hash]; command; command = command->next )
		if ( command->level <  LEVEL_HERO
		&&   command->level <= get_trust( ch )
		&&  !str_prefix(argument, command->name)
		&&   strncmp( "do_mp", command->code, 5 ) )
		{
		    pager_printf( ch, "%-12s", command->name );
		    found = TRUE;
		    if ( ++col % 6 == 0 )
			send_to_pager( "\n\r", ch );
		}

	if ( col % 6 != 0 )
	    send_to_pager( "\n\r", ch );
	if ( !found )
	    ch_printf( ch, "No command found under %s.\n\r", argument);
    }
    return;
}

void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    one_argument( argument, arg );

    if ( IS_NPC( ch ) )
	return;

    if ( arg[0] == '\0' )
    {
        if ( !IS_NPC(ch) && xIS_SET(ch->act, PLR_SILENCE) )
        {
	    set_char_color( AT_GREEN, ch );
            send_to_char( "You are silenced.\n\r", ch );
            return;
        }
    
	/* Channels everyone sees regardless of affiliation --Blodkai */
        send_to_char_color( "\n\r &gPublic channels  (severe penalties for abuse)&G:\n\r  ", ch );
	ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_RACETALK )?
						" &G+RACETALK" :
						" &g-racetalk" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_CHAT )    ?
				                " &G+CHAT" 	          :
						" &g-chat" );
        if ( get_trust( ch ) > 2 && !NOT_AUTHED( ch ) )
          ch_printf_color( ch, "%s", !IS_SET( ch->deaf, CHANNEL_AUCTION ) ?
				                " &G+AUCTION" 	          :
						" &g-auction" );
	ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_TRAFFIC ) ?
						" &G+TRAFFIC"		  :
						" &g-traffic" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_QUEST )   ?
				                " &G+QUEST" 	          :
						" &g-quest" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_WARTALK ) ?
				                " &G+WARTALK" 	          :
						" &g-wartalk" );
        if ( IS_HERO( ch ) )
          ch_printf_color( ch, "%s", !IS_SET( ch->deaf, CHANNEL_AVTALK )  ?
				                " &G+AVATAR"	          :
						" &g-avatar" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_MUSIC )   ?
				                " &G+MUSIC" 	          :
						" &g-music" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_ASK )     ?
					        " &G+ASK"                 :
						" &g-ask" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_SHOUT )   ?
						" &G+SHOUT"	          :
						" &g-shout" );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_YELL )    ?
				                " &G+YELL"	          :
						" &g-yell" );
  
	/* For organization channels (orders, clans, guilds, councils) */
        send_to_char_color( "\n\r &gPrivate channels (severe penalties for abuse)&G:\n\r ", ch );
        ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_TELLS )   ?
                                                " &G+TELLS"               :
                                                " &g-tells" );
        ch_printf_color( ch, "%s", !IS_SET( ch->deaf, CHANNEL_WHISPER )   ?
						" &G+WHISPER"		  :
						" &g-whisper" );
	if ( IS_ORDERED( ch ) )
	{
	    send_to_char_color( !IS_SET( ch->deaf, CHANNEL_ORDER ) ?
		" &G+ORDER"	:	" &g-order", ch );
	}
	else if ( IS_GUILDED( ch ) )
	{
	    send_to_char_color( !IS_SET( ch->deaf, CHANNEL_GUILD ) ?
		" &G+GUILD"	:	" &g-guild", ch );
	}
	else if ( IS_CLANNED( ch ) )
	{
	    send_to_char_color( !IS_SET( ch->deaf, CHANNEL_CLAN )  ?
		" &G+CLAN"	:	" &g-clan", ch );
        }
        if ( IS_IMMORTAL(ch) || ( ch->pcdata->council
	&&   !str_cmp( ch->pcdata->council->name, "Newbie Council" ) ) )
          ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_NEWBIE) ?
    				 	        " &G+NEWBIE"	     	  :
						" &g-newbie" );
        if ( !IS_NPC( ch ) && ch->pcdata->council )
          ch_printf_color( ch, "%s",   !IS_SET( ch->deaf, CHANNEL_COUNCIL)?
				                " &G+COUNCIL"	     	  :
						" &g-council" );

	/* Immortal channels */
        if ( IS_IMMORTAL( ch ) )
        {
            send_to_char_color( "\n\r &gImmortal Channels&G:\n\r  ", ch );
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_IMMTALK )    ?
		" &G+IMMTALK"	:	" &g-immtalk", ch );
/*          send_to_char_color( !IS_SET( ch->deaf, CHANNEL_PRAY )       ?
		" &G+PRAY"	:	" &g-pray", ch ); */
            if ( get_trust( ch ) >= sysdata.muse_level )
              send_to_char_color( !IS_SET( ch->deaf, CHANNEL_HIGHGOD )  ?
		" &G+MUSE"	:	" &g-muse", ch );
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_MONITOR )    ?
		" &G+MONITOR"	:	" &g-monitor", ch );
	    send_to_char_color( !IS_SET( ch->deaf, CHANNEL_AUTH )	?
	       " &G+AUTH"	:	" &g-auth", ch );	
        }
        if ( get_trust( ch ) >= sysdata.log_level )
        {
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_LOG ) 	?
		" &G+LOG"	:	" &g-log", ch);
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_BUILD)       ?
		" &G+BUILD"	:	" &g-build", ch );
            send_to_char_color( !IS_SET( ch->deaf, CHANNEL_COMM ) 	?
		" &G+COMM"	:	" &g-comm", ch );
            send_to_char_color( !IS_SET (ch->deaf, CHANNEL_WARN)
                        ? " &G+WARN" : " &g-warn", ch);
            if ( get_trust( ch ) >= sysdata.think_level )
              send_to_char_color( !IS_SET( ch->deaf, CHANNEL_HIGH ) 	?
		" &G+HIGH"	:	" &g-high", ch );
        }
        send_to_char( "\n\r", ch );   
    }
    else
    {
	bool fClear;
	bool ClearAll;
	int bit;

        bit=0;
        ClearAll = FALSE;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Channels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
	else if ( !str_cmp( arg+1, "traffic"  ) ) bit = CHANNEL_TRAFFIC;
	else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
	else if ( !str_cmp( arg+1, "clan"     ) ) bit = CHANNEL_CLAN;
	else if ( !str_cmp( arg+1, "council"  ) ) bit = CHANNEL_COUNCIL;
        else if ( !str_cmp( arg+1, "guild"    ) ) bit = CHANNEL_GUILD;  
	else if ( !str_cmp( arg+1, "quest"    ) ) bit = CHANNEL_QUEST;
	else if ( !str_cmp( arg+1, "tells"    ) ) bit = CHANNEL_TELLS;
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "log"      ) ) bit = CHANNEL_LOG;
	else if ( !str_cmp( arg+1, "build"    ) ) bit = CHANNEL_BUILD;
	else if ( !str_cmp( arg+1, "high"     ) ) bit = CHANNEL_HIGH;
	else if ( !str_cmp( arg+1, "pray"     ) ) bit = CHANNEL_PRAY;
	else if ( !str_cmp( arg+1, "avatar"   ) ) bit = CHANNEL_AVTALK;
	else if ( !str_cmp( arg+1, "monitor"  ) ) bit = CHANNEL_MONITOR;
	else if ( !str_cmp( arg+1, "auth"     ) ) bit = CHANNEL_AUTH;
	else if ( !str_cmp( arg+1, "newbie"   ) ) bit = CHANNEL_NEWBIE;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "muse"     ) ) bit = CHANNEL_HIGHGOD;
	else if ( !str_cmp( arg+1, "ask"      ) ) bit = CHANNEL_ASK;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else if ( !str_cmp( arg+1, "comm"     ) ) bit = CHANNEL_COMM;
        else if ( !str_cmp (arg+1, "warn"     ) ) bit = CHANNEL_WARN;
	else if ( !str_cmp( arg+1, "order"    ) ) bit = CHANNEL_ORDER;
        else if ( !str_cmp( arg+1, "wartalk"  ) ) bit = CHANNEL_WARTALK;
	else if ( !str_cmp( arg+1, "whisper"  ) ) bit = CHANNEL_WHISPER;
	else if ( !str_cmp( arg+1, "racetalk" ) ) bit = CHANNEL_RACETALK;
	else if ( !str_cmp( arg+1, "all"      ) ) ClearAll = TRUE;
	else
	{
	    send_to_char( "Set or clear which channel?\n\r", ch );
	    return;
	}

	if (( fClear ) && ( ClearAll ))
	{
	    REMOVE_BIT (ch->deaf, CHANNEL_RACETALK);
            REMOVE_BIT (ch->deaf, CHANNEL_AUCTION);
            REMOVE_BIT (ch->deaf, CHANNEL_CHAT);
            REMOVE_BIT (ch->deaf, CHANNEL_QUEST);
            REMOVE_BIT (ch->deaf, CHANNEL_WARTALK);
            REMOVE_BIT (ch->deaf, CHANNEL_PRAY);
	    REMOVE_BIT (ch->deaf, CHANNEL_TRAFFIC);
            REMOVE_BIT (ch->deaf, CHANNEL_MUSIC);
            REMOVE_BIT (ch->deaf, CHANNEL_ASK);
            REMOVE_BIT (ch->deaf, CHANNEL_SHOUT);
            REMOVE_BIT (ch->deaf, CHANNEL_YELL);

            if (ch->level >= LEVEL_IMMORTAL)
              REMOVE_BIT (ch->deaf, CHANNEL_AVTALK);
 
	    /*
	    if (ch->level >= sysdata.log_level )
	      REMOVE_BIT (ch->deaf, CHANNEL_COMM);
	    */

        } else if ((!fClear) && (ClearAll))
        {
	    SET_BIT (ch->deaf, CHANNEL_RACETALK);
            SET_BIT (ch->deaf, CHANNEL_AUCTION);
	    SET_BIT (ch->deaf, CHANNEL_TRAFFIC);
            SET_BIT (ch->deaf, CHANNEL_CHAT);
            SET_BIT (ch->deaf, CHANNEL_QUEST);
            SET_BIT (ch->deaf, CHANNEL_PRAY);
            SET_BIT (ch->deaf, CHANNEL_MUSIC);
            SET_BIT (ch->deaf, CHANNEL_ASK);
            SET_BIT (ch->deaf, CHANNEL_SHOUT);
	    SET_BIT (ch->deaf, CHANNEL_WARTALK);
            SET_BIT (ch->deaf, CHANNEL_YELL);
          
            if (ch->level >= LEVEL_IMMORTAL)
              SET_BIT (ch->deaf, CHANNEL_AVTALK);

	    /*
	    if (ch->level >= sysdata.log_level)
	      SET_BIT (ch->deaf, CHANNEL_COMM);
	    */

         } else if (fClear)
         {
	    REMOVE_BIT (ch->deaf, bit);
         } else
         {
	    SET_BIT    (ch->deaf, bit);
         }

	  send_to_char( "Ok.\n\r", ch );
    }

    return;
}


/*
 * display WIZLIST file						-Thoric
 */
void do_wizlist( CHAR_DATA *ch, char *argument )
{
    set_pager_color( AT_IMMORT, ch );
    show_file( ch, WIZLIST_FILE );
}

/*
 * Contributed by Grodyn.
 * Display completely overhauled, 2/97 -- Blodkai
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
 
    if ( IS_NPC(ch) )
        return;
 
    one_argument( argument, arg );
 
    set_char_color( AT_GREEN, ch );
 
    if ( arg[0] == '\0' )
    {
      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\rConfigurations ", ch );
      set_char_color( AT_GREEN, ch );
      send_to_char( "(use 'config +/- <keyword>' to toggle, see 'help config')\n\r\n\r", ch );
      set_char_color( AT_DGREEN, ch );
      send_to_char( "Display:   ", ch );
      set_char_color( AT_GREY, ch );
      ch_printf( ch, "%-12s   %-12s   %-12s   %-12s\n\r           %-12s   %-12s   %-12s   %-12s\n\r           %-12s",
        IS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) 	? "[+] PAGER"
                                                    	: "[-] pager",
        IS_SET( ch->pcdata->flags, PCFLAG_GAG )     	? "[+] GAG"
                                                    	: "[-] gag",
        xIS_SET(ch->act, PLR_BRIEF )                	? "[+] BRIEF"
                                                    	: "[-] brief",
        xIS_SET(ch->act, PLR_COMBINE )              	? "[+] COMBINE"
                                                    	: "[-] combine",
        xIS_SET(ch->act, PLR_BLANK )                	? "[+] BLANK"
                                                    	: "[-] blank",
        xIS_SET(ch->act, PLR_PROMPT )               	? "[+] PROMPT"
                                                    	: "[-] prompt",
        xIS_SET(ch->act, PLR_ANSI )                 	? "[+] ANSI"
                                                    	: "[-] ansi",
        IS_SET(ch->pcdata->flags, PCFLAG_SHOWSLOTS )    ? "[+] SLOTS"
                                                        : "[-] slots" );
      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rAuto:      ", ch );
      set_char_color( AT_GREY, ch );
      ch_printf( ch, "%-12s   %-12s   %-12s   %-12s\n\r           %-14s %-12s",
        xIS_SET( ch->act, PLR_AUTOSAC  )             	? "[+] AUTOSAC"
                                                    	: "[-] autosac",
        xIS_SET( ch->act, PLR_AUTOGOLD )             	? "[+] AUTOGOLD"
                                                    	: "[-] autogold",
        xIS_SET( ch->act, PLR_AUTOLOOT )             	? "[+] AUTOLOOT"
                                                    	: "[-] autoloot",
        xIS_SET( ch->act, PLR_AUTOEXIT )             	? "[+] AUTOEXIT"
                                                    	: "[-] autoexit",
        xIS_SET( ch->act, PLR_AUTOGLANCE )             	? "[+] AUTOGLANCE"
                                                    	: "[-] autoglance",
        xIS_SET( ch->act, PLR_AUTONEWS )             	? "[+] AUTONEWS"
                                                    	: "[-] autonews" );
 
      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rSafeties:  ", ch );
      set_char_color( AT_GREY, ch );
      ch_printf( ch, "%-12s   %-12s",
        IS_SET( ch->pcdata->flags, PCFLAG_NORECALL ) 	? "[+] NORECALL"
                                                     	: "[-] norecall",
        IS_SET( ch->pcdata->flags, PCFLAG_NOSUMMON ) 	? "[+] NOSUMMON"
                                                     	: "[-] nosummon" );
 
      if ( !IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
        ch_printf( ch, "   %-12s   %-12s",
           xIS_SET(ch->act, PLR_SHOVEDRAG )             ? "[+] DRAG"
                                                        : "[-] drag",
           xIS_SET(ch->act, PLR_NICE )               	? "[+] NICE"
                                                     	: "[-] nice" );
 
      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rMisc:      ", ch );
      set_char_color( AT_GREY, ch );
      ch_printf( ch, "%-12s   %-12s   %-12s",
	   xIS_SET(ch->act, PLR_TELNET_GA )		? "[+] TELNETGA"
                                                        : "[-] telnetga",
           IS_SET( ch->pcdata->flags, PCFLAG_GROUPWHO ) ? "[+] GROUPWHO"
                                                        : "[-] groupwho",
           IS_SET( ch->pcdata->flags, PCFLAG_NOINTRO )  ? "[+] NOINTRO"
                                                        : "[-] nointro" );
 
      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rSettings:  ", ch );
      set_char_color( AT_GREY, ch );
      ch_printf_color( ch, "Pager Length (%d)    Wimpy (&W%d&w)",
							ch->pcdata->pagerlen,
						        ch->wimpy );
 
      if ( IS_IMMORTAL( ch ) )
      {
	set_char_color( AT_DGREEN, ch );
        send_to_char( "\n\r\n\rImmortal toggles:  ", ch );
        set_char_color( AT_GREY, ch );
        ch_printf( ch, "Roomvnum [%c]",
	  xIS_SET(ch->act, PLR_ROOMVNUM ) 		? '+'
							: ' ' );
      }
 
      set_char_color( AT_DGREEN, ch );
      send_to_char( "\n\r\n\rSentences imposed on you (if any):", ch );
      set_char_color( AT_YELLOW, ch );
      ch_printf( ch, "\n\r%s%s%s%s%s%s",
          xIS_SET(ch->act, PLR_SILENCE )  ?
            " For your abuse of channels, you are currently silenced.\n\r" : "",
          xIS_SET(ch->act, PLR_NO_EMOTE ) ?
            " The gods have removed your emotes.\n\r"                      : "", 
          xIS_SET(ch->act, PLR_NO_TELL )  ?
            " You are not permitted to send 'tells' to others.\n\r"        : "", 
          xIS_SET(ch->act, PLR_LITTERBUG )?
            " A convicted litterbug.  You cannot drop anything.\n\r"       : "", 
          xIS_SET(ch->act, PLR_THIEF )    ?
            " A proven thief, you will be hunted by the authorities.\n\r"  : "", 
          xIS_SET(ch->act, PLR_KILLER )   ?
            " For the crime of murder you are sentenced to death...\n\r"   : "" );
    }
    else
    {
	bool fSet;
	int bit = 0;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

	     if ( !str_prefix( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_prefix( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_prefix( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_prefix( arg+1, "autogold" ) ) bit = PLR_AUTOGOLD;
	else if ( !str_prefix( arg+1, "autoglance" ) ) bit = PLR_AUTOGLANCE;
	else if ( !str_prefix( arg+1, "autonews" ) ) bit = PLR_AUTONEWS;
	else if ( !str_prefix( arg+1, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_prefix( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_prefix( arg+1, "combine"  ) ) bit = PLR_COMBINE;
	else if ( !str_prefix( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_prefix( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else if ( !str_prefix( arg+1, "ansi"     ) ) bit = PLR_ANSI;
/*	else if ( !str_prefix( arg+1, "flee"     ) ) bit = PLR_FLEE; */
	else if ( !str_prefix( arg+1, "nice"     ) ) bit = PLR_NICE;
	else if ( !str_prefix( arg+1, "drag"     ) ) bit = PLR_SHOVEDRAG;
	else if ( IS_IMMORTAL( ch )
	     &&   !str_prefix( arg+1, "vnum"     ) ) bit = PLR_ROOMVNUM;

	if (bit)
        {
  	  if ( (bit == PLR_FLEE || bit == PLR_NICE || bit == PLR_SHOVEDRAG) 
	  &&  IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
          {
	    send_to_char( "Pkill characters can not config that option.\n\r", ch );
	    return;
          }
          
	  if ( fSet )
	    xSET_BIT   (ch->act, bit);
	  else
	    xREMOVE_BIT(ch->act, bit);
	  send_to_char( "Ok.\n\r", ch );
          return;
        }
        else
        {
	       if ( !str_prefix( arg+1, "norecall" ) ) bit = PCFLAG_NORECALL;
	  else if ( !str_prefix( arg+1, "nointro"  ) ) bit = PCFLAG_NOINTRO;
	  else if ( !str_prefix( arg+1, "nosummon" ) ) bit = PCFLAG_NOSUMMON;
          else if ( !str_prefix( arg+1, "gag"      ) ) bit = PCFLAG_GAG; 
          else if ( !str_prefix( arg+1, "slots"    ) ) bit = PCFLAG_SHOWSLOTS; 
          else if ( !str_prefix( arg+1, "pager"    ) ) bit = PCFLAG_PAGERON;
          else if ( !str_prefix( arg+1, "groupwho" ) ) bit = PCFLAG_GROUPWHO;
	  else if ( !str_prefix( arg+1, "@hgflag_" ) ) bit = PCFLAG_HIGHGAG;
          else
	  {
	    send_to_char( "Config which option?\n\r", ch );
	    return;
    	  }

          if ( fSet )
	    SET_BIT    (ch->pcdata->flags, bit);
	  else
	    REMOVE_BIT (ch->pcdata->flags, bit);

	  send_to_char( "Ok.\n\r", ch );
          return;
        }
    }

    return;
}


void do_credits( CHAR_DATA *ch, char *argument )
{
  do_help( ch, "credits" );
}


extern int top_area;

/*
 * New do_areas, written by Fireblade, last modified - 4/27/97
 *
 *   Syntax: area            ->      lists areas in alphanumeric order
 *           area <a>        ->      lists areas with soft max less than
 *                                                    parameter a
 *           area <a> <b>    ->      lists areas with soft max bewteen
 *                                                    numbers a and b
 *           area old        ->      list areas in order loaded
 *
 */
void do_areas( CHAR_DATA *ch, char *argument )
{ 
    /*
     * upper_bound must be initialized at (MAX_LEVEL + 1) to ensure proper
     * default display. -Orion
     */
    int lower_bound	= 0;
    int upper_bound	= MAX_LEVEL + 1;

    /*
     * Default print strings, used in area display. -Orion
     */
    char *header_string1	=
	"|    Authors    |              Areas              | Soft Ranges | Hard Ranges |\n\r";
    char *header_string2	=
	"+---------------+---------------------------------+-------------+-------------+\n\r";
    char *print_string		=
	"| %-13s | %-31s |   %-3.2d-%3.2d   |   %-3.2d-%3.2d   |\n\r";

    /*
     * Default print string used with areas 'info' command. -Orion
     */
    char *info_string		=
	"\n\r  Area Name   : %s\n\r--=--=--=--=--=--=--=--=--=--=--\n\r  Author      : %s\n\r  Derivatives : %s\n\r  Hard Range  : %3d-%-3d\n\r  Soft Range  : %3d-%-3d\n\r--=--=--=--=--=--=--=--=--=--=--\n\r";

    /*
     * Argument variable used for data comparison. -Orion
     */
    char arg[MAX_STRING_LENGTH];

    /*
     * Temporary AREA_DATA variable for area display. -Orion
     */
    AREA_DATA *pArea;

    argument = one_argument(argument,arg);
        
    if ( arg && arg[0] != '\0' )
    {
	if ( !is_number( arg ) )
	{
	    if ( !strcmp( arg, "info" ) )
	    {
		AREA_DATA *temp = NULL;

		if ( !argument || argument[0] == '\0' )
		{
		    send_to_char( "\n\rSyntax: areas info <area name>\n\r", ch );
		    return;
		}

		for ( temp = first_area; temp != NULL; temp = temp->next )
		{
		    if ( !str_cmp( argument, temp->name ) )
			break;
		}

		if ( temp != NULL )
		{
		    pager_printf_color( ch, info_string, temp->name, temp->author, (!temp->derivatives || temp->derivatives[0] == '\0') ? "None" : temp->derivatives, temp->low_hard_range, temp->hi_hard_range, temp->low_soft_range, temp->hi_soft_range );
		}
		else
		{
		    send_to_char( "\n\rSyntax: areas info <area name>\n\r", ch );
		    return;
		}
		return;
	    }

	    if ( !str_cmp( arg, "old" ) )
	    {
		set_pager_color( AT_PLAIN, ch );

		send_to_pager( "\n\r", ch );
		send_to_pager( header_string2, ch );
		send_to_pager( header_string1, ch );
		send_to_pager( header_string2, ch );

		for( pArea = first_area; pArea;pArea = pArea->next )
		{
		    pager_printf( ch, print_string,
			pArea->author,
			pArea->name,
			pArea->low_soft_range,
			pArea->hi_soft_range,
			pArea->low_hard_range,
			pArea->hi_hard_range );
		}

		send_to_pager( header_string2, ch );
		send_to_pager( "For information on an area type: areas info <area name>\n\r", ch );

		return;
	    }
	    else
	    {
		send_to_char( "Area may only be followed by numbers, or 'old'.\n\r", ch );
		return;
	    }
	}

	upper_bound = atoi(arg);
	lower_bound = upper_bound;

	argument = one_argument( argument, arg );

	if ( arg && arg[0] != '\0' )
	{
	    if( !is_number( arg ) )
	    {
		send_to_char( "Area may only be followed by numbers.\n\r", ch );
		return;
	    }

	    upper_bound = atoi( arg );

	    argument = one_argument( argument, arg );

	    if ( arg && arg[0] != '\0' )
	    {
		send_to_char( "Only two level numbers allowed.\n\r", ch );
		return;
	    }
	}
    }
                         
    if ( lower_bound > upper_bound )
    {
	int swap	= lower_bound;
	lower_bound	= upper_bound;
	upper_bound	= swap;
    }
 
    set_pager_color( AT_PLAIN, ch );

    send_to_pager( "\n\r", ch );
    send_to_pager( header_string2, ch );
    send_to_pager( header_string1, ch );
    send_to_pager( header_string2, ch );
                                
    for ( pArea = first_area_name; pArea; pArea = pArea->next_sort_name )
    {
	if ( pArea->hi_soft_range  >= lower_bound &&
	     pArea->low_soft_range <= upper_bound )
	{
	    pager_printf( ch, print_string,
		pArea->author,
		pArea->name,
		pArea->low_soft_range,
		pArea->hi_soft_range,
		pArea->low_hard_range,
		pArea->hi_hard_range );
	}
    }

    send_to_pager( header_string2, ch );
    send_to_pager( "For information on an area type: areas info <area name>\n\r", ch );

    return;
}

void do_afk( CHAR_DATA *ch, char *argument )
{
     if ( IS_NPC(ch) )
     return;
     
     if xIS_SET(ch->act, PLR_AFK)
     {
    	xREMOVE_BIT(ch->act, PLR_AFK);
	send_to_char( "You are no longer afk.\n\r", ch );
/*	act(AT_GREY,"$n is no longer afk.", ch, NULL, NULL, TO_ROOM);*/
	act(AT_GREY,"$n is no longer afk.", ch, NULL, NULL, TO_CANSEE);
     }
     else
     {
	xSET_BIT(ch->act, PLR_AFK);
	send_to_char( "You are now afk.\n\r", ch );
/*	act(AT_GREY,"$n is now afk.", ch, NULL, NULL, TO_ROOM);*/
	act(AT_GREY,"$n is now afk.", ch, NULL, NULL, TO_CANSEE);
	return;
     }
         
}

void do_slist( CHAR_DATA *ch, char *argument )
{
   int sn, i, lFound;
   char skn[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH]; 
   int lowlev, hilev;
   sh_int lasttype = SKILL_SPELL;

   if ( IS_NPC(ch) )
     return;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   lowlev=1;
   hilev=(LEVEL_IMMORTAL - 1);

   if (arg1[0]!='\0')
      lowlev=atoi(arg1);

   if ((lowlev<1) || (lowlev>LEVEL_IMMORTAL))
      lowlev=1;

   if (arg2[0]!='\0')
      hilev=atoi(arg2);

   if ((hilev<0) || (hilev>=LEVEL_IMMORTAL))
      hilev=LEVEL_HERO;

   if(lowlev>hilev)
      lowlev=hilev;

   set_pager_color( AT_MAGIC, ch );
   send_to_pager("SPELL & SKILL LIST\n\r",ch);
   send_to_pager("------------------\n\r",ch);

   for (i=lowlev; i <= hilev; i++)
   {
	lFound= 0;
	sprintf(skn,"Spell");   
	for ( sn = 0; sn < top_sn; sn++ )
	{
	    if ( !skill_table[sn]->name )
		break;

	    if ( skill_table[sn]->type != lasttype )
	    {
		lasttype = skill_table[sn]->type;
		strcpy( skn, skill_tname[lasttype] );
	    }

	    if ( ch->pcdata->learned[sn] <= 0
	    &&   SPELL_FLAG(skill_table[sn], SF_SECRETSKILL) )
		continue;
	
	    if(i==skill_table[sn]->skill_level[ch->class]  )
	    {
		if( !lFound )
		{
		    lFound=1;
		    pager_printf( ch, "Level %d\n\r", i );
		}
                switch (skill_table[sn]->minimum_position)
                {
                    case POS_DEAD:
                            sprintf(buf, "any");
                            break;
                    case POS_MORTAL:
                            sprintf(buf, "mortally wounded");
                            break;
                    case POS_INCAP:
                            sprintf(buf, "incapacitated");
                            break;
                    case POS_STUNNED:
                            sprintf(buf, "stunned");
                            break;
                    case POS_SLEEPING:
                            sprintf(buf, "sleeping");
                            break;
                    case POS_RESTING:
                            sprintf(buf, "resting");
                            break;
                    case POS_STANDING:
                            sprintf(buf, "standing");
                            break;
                    case POS_FIGHTING:
                            sprintf(buf, "fighting");
                            break;
                    case POS_EVASIVE:
                            sprintf(buf, "(evasive)");   /* Fighting style support -haus */
                            break;
                    case POS_DEFENSIVE:
                            sprintf(buf, "(defensive)");
                            break;
                    case POS_AGGRESSIVE:
                            sprintf(buf, "(aggressive)");
                            break;
                    case POS_BERSERK:
                            sprintf(buf, "(berserk)");
                            break;
                    case POS_MOUNTED:
                            sprintf(buf, "mounted");
                            break;
                    case POS_SITTING:
                            sprintf(buf, "sitting");
                            break;
		    default:
			    sprintf( buf, " " );
			    break;
                }

		pager_printf(ch, "%7s: %20.20s \t Current: %-3d Max: %-3d  MinPos: %s \n\r",
                        skn, skill_table[sn]->name, 
                        ch->pcdata->learned[sn],
                        skill_table[sn]->skill_adept[ch->class],
			buf );
	    }
	}
   }
   return;
}

void do_whois( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  bool isFactioned = FALSE;

  buf[0] = '\0';

  if(IS_NPC(ch))
    return;

  if(argument[0] == '\0')
  {
    send_to_pager("You must input the name of an online character.\n\r", ch);
    return;
  }

  strcat(buf, "0.");
  strcat(buf, argument);
  if( ( ( victim = get_char_world(ch, buf) ) == NULL ))
  {
    send_to_pager("No such character online.\n\r", ch);
    return;
  }

  if(IS_NPC(victim))
  {
    send_to_pager("That's not a player!\n\r", ch);
    return;
  }

  set_pager_color( AT_GREY, ch );
  pager_printf(ch, "\n\r'%s%s.'\n\r %s is a %s level %d %s %s, %d years of age.\n\r",
	victim->name,
	victim->pcdata->title,
        victim->sex == SEX_MALE ? "He" :
        victim->sex == SEX_FEMALE ? "She" : "It",
	victim->sex == SEX_MALE ? "male" : 
	victim->sex == SEX_FEMALE ? "female" : "neutral",
	victim->level, 
	capitalize(race_table[victim->race]->race_name),
	class_table[victim->class]->who_name,
	get_age(victim) );

  pager_printf(ch, " %s is a %sdeadly player",
	victim->sex == SEX_MALE ? "He" :
	victim->sex == SEX_FEMALE ? "She" : "It",
	IS_SET(victim->pcdata->flags, PCFLAG_DEADLY) ? "" : "non-");

  if ( IS_ORDERED( victim ) )
  {
    isFactioned = TRUE;
    send_to_pager( ", and belongs to the Order ", ch );
  }
  else if ( IS_GUILDED( victim ) )
  {
    isFactioned = TRUE;
    send_to_pager( ", and belongs to the ", ch );
  }
  else if ( IS_CLANNED( victim ) )
  {
    isFactioned = TRUE;
    send_to_pager( ", and belongs to Clan ", ch );
  }

  if ( isFactioned == TRUE )
  {
	send_to_pager( victim->pcdata->faction->name, ch );
  }
  send_to_pager( ".\n\r", ch );

  if(victim->pcdata->council)
    pager_printf(ch, " %s holds a seat on:  %s\n\r",
        victim->sex == SEX_MALE ? "He" :
        victim->sex == SEX_FEMALE ? "She" : "It",
	victim->pcdata->council->name );
    
  if(victim->pcdata->deity)
    pager_printf(ch, " %s has found succor in the deity %s.\n\r",
        victim->sex == SEX_MALE ? "He" :
        victim->sex == SEX_FEMALE ? "She" : "It",
	victim->pcdata->deity->name);
  
  if(victim->pcdata->homepage && victim->pcdata->homepage[0] != '\0')
    pager_printf(ch, " %s homepage can be found at %s\n\r", 
        victim->sex == SEX_MALE ? "His" :
        victim->sex == SEX_FEMALE ? "Her" : "Its",
	show_tilde( victim->pcdata->homepage ) );

  if(victim->pcdata->bio && victim->pcdata->bio[0] != '\0')
    pager_printf(ch, " %s's personal bio:\n\r%s",
	victim->name,
	victim->pcdata->bio);
  else
    pager_printf(ch, " %s has yet to create a bio.\n\r",
	victim->name );

  if(IS_IMMORTAL(ch))
  {  
    send_to_pager("-------------------\n\r", ch);
    send_to_pager("Info for immortals:\n\r", ch);

    if ( victim->pcdata->authed_by && victim->pcdata->authed_by[0] != '\0' )
	pager_printf(ch, "%s was authorized by %s.\n\r",
		victim->name, victim->pcdata->authed_by );

    pager_printf(ch, "%s has killed %d mobiles, and been killed by a mobile %d times.\n\r",
		victim->name, victim->pcdata->mkills, victim->pcdata->mdeaths );
    if ( victim->pcdata->pkills || victim->pcdata->pdeaths )
	pager_printf(ch, "%s has killed %d players, and been killed by a player %d times.\n\r",
		victim->name, victim->pcdata->pkills, victim->pcdata->pdeaths );
    if ( victim->pcdata->illegal_pk )
	pager_printf(ch, "%s has committed %d illegal player kills.\n\r",
		victim->name, victim->pcdata->illegal_pk );

    pager_printf(ch, "%s is %shelled at the moment.\n\r",
	victim->name, 
	(victim->pcdata->release_date == 0) ? "not " : "");

    if (victim->pcdata->nuisance )
    {
      pager_printf_color( ch, "&RNuisance   &cStage: (&R%d&c/%d)  Power:  &w%d  &cTime:  &w%s.\n\r", victim->pcdata->nuisance->flags,
                  MAX_NUISANCE_STAGE, victim->pcdata->nuisance->power,
		              ctime(&victim->pcdata->nuisance->time));
    }
    if(victim->pcdata->release_date != 0)
      pager_printf(ch, "%s was helled by %s, and will be released on %24.24s.\n\r",
	victim->sex == SEX_MALE ? "He" :
	victim->sex == SEX_FEMALE ? "She" : "It",
        victim->pcdata->helled_by,
	ctime(&victim->pcdata->release_date));

    if(get_trust(victim) < get_trust(ch))
    {
      sprintf(buf2, "list %s", buf);
      do_comment(ch, buf2);
    }

    if(xIS_SET(victim->act, PLR_SILENCE) || xIS_SET(victim->act, PLR_NO_EMOTE) 
    || xIS_SET(victim->act, PLR_NO_TELL) || xIS_SET(victim->act, PLR_THIEF) 
    || xIS_SET(victim->act, PLR_KILLER) )
    {
      sprintf(buf2, "This player has the following flags set:");
      if(xIS_SET(victim->act, PLR_SILENCE)) 
        strcat(buf2, " silence");
      if(xIS_SET(victim->act, PLR_NO_EMOTE)) 
        strcat(buf2, " noemote");
      if(xIS_SET(victim->act, PLR_NO_TELL) )
        strcat(buf2, " notell");
      if(xIS_SET(victim->act, PLR_THIEF) )
        strcat(buf2, " thief");
      if(xIS_SET(victim->act, PLR_KILLER) )
        strcat(buf2, " killer");
      strcat(buf2, ".\n\r");
      send_to_pager(buf2, ch);
    }
  if ( victim->desc && victim->desc->host[0]!='\0' )   /* added by Gorog */
     {
     sprintf (buf2, "%s's IP info: %s ", victim->name, victim->desc->host);
     if (get_trust(ch) >= LEVEL_WORKER)
        strcat (buf2, victim->desc->user);
     strcat (buf2, "\n\r");
     send_to_pager(buf2, ch);
     }
  }
}

void do_pager( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  
  if ( IS_NPC(ch) )
    return;
  set_char_color( AT_NOTE, ch );
  argument = one_argument(argument, arg);
  if ( !*arg )
  {
    if ( IS_SET(ch->pcdata->flags, PCFLAG_PAGERON) )
    {
        send_to_char( "Pager disabled.\n\r", ch );
        do_config(ch, "-pager");
    } else {
        ch_printf( ch, "Pager is now enabled at %d lines.\n\r", ch->pcdata->pagerlen );
        do_config(ch, "+pager");
    }
    return;
  }
  if ( !is_number(arg) )
  {
    send_to_char( "Set page pausing to how many lines?\n\r", ch );
    return;
  }
  ch->pcdata->pagerlen = atoi(arg);
  if ( ch->pcdata->pagerlen < 5 )
    ch->pcdata->pagerlen = 5;
  ch_printf( ch, "Page pausing set to %d lines.\n\r", ch->pcdata->pagerlen );
  return;
}

/*
 * The ignore command allows players to ignore up to MAX_IGN
 * other players. Players may ignore other characters whether
 * they are online or not. This is to prevent people from
 * spamming someone and then logging off quickly to evade
 * being ignored.
 * Syntax:
 *	ignore		-	lists players currently ignored
 *	ignore none	-	sets it so no players are ignored
 *	ignore <player>	-	start ignoring player if not already
 *				ignored otherwise stop ignoring player
 *	ignore reply	-	start ignoring last player to send a
 *				tell to ch, to deal with invis spammers
 * Last Modified: June 26, 1997
 * - Fireblade
 */
void do_ignore(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	IGNORE_DATA *temp, *next;
	char fname[1024];
	struct stat fst;
	CHAR_DATA *victim;
	
	if(IS_NPC(ch))
		return;
	
	argument = one_argument(argument, arg);
	
	sprintf(fname, "%s%c/%s", PLAYER_DIR,
		tolower(arg[0]), capitalize(arg));
	
	victim = NULL;
	
	/* If no arguements, then list players currently ignored */
	if(arg[0] == '\0')
	{
		set_char_color(AT_DIVIDER, ch);
		ch_printf(ch, "\n\r----------------------------------------\n\r");
		set_char_color(AT_DGREEN, ch);
		ch_printf(ch, "You are currently ignoring:\n\r");
		set_char_color(AT_DIVIDER, ch);
		ch_printf(ch, "----------------------------------------\n\r");
		set_char_color(AT_IGNORE, ch);
		
		if(!ch->pcdata->first_ignored)
		{
			ch_printf(ch, "\t    no one\n\r");
			return;
		}
		
		for(temp = ch->pcdata->first_ignored; temp;
				temp = temp->next)
		{
			ch_printf(ch,"\t  - %s\n\r",temp->name);
		}
		
		return;
	}
	/* Clear players ignored if given arg "none" */
	else if(!strcmp(arg, "none"))
	{
		for(temp = ch->pcdata->first_ignored; temp; temp = next)
		{
			next = temp->next;
			UNLINK(temp, ch->pcdata->first_ignored,
					ch->pcdata->last_ignored,
					next, prev);
			STRFREE(temp->name);
			DISPOSE(temp);
		}
		
		set_char_color(AT_IGNORE, ch);
		ch_printf(ch, "You now ignore no one.\n\r");
		
		return;
	}
	/* Prevent someone from ignoring themself... */
	else if(!strcmp(arg, "self") || nifty_is_name(arg, ch->name))
	{
		set_char_color(AT_IGNORE, ch);
		ch_printf(ch, "Did you type something?\n\r");
		return;
	}
	else
	{
		int i;
		
		/* get the name of the char who last sent tell to ch */		
		if(!strcmp(arg, "reply"))
		{
			if(!ch->reply)
			{
				set_char_color(AT_IGNORE, ch);
				ch_printf(ch, "They're not here.\n\r");
				return;
			}
			else
			{
				strcpy(arg, ch->reply->name);
			}
		}
		
		/* Loop through the linked list of ignored players */
		/* 	keep track of how many are being ignored     */
		for(temp = ch->pcdata->first_ignored, i = 0; temp;
				temp = temp->next, i++)
		{
			/* If the argument matches a name in list remove it */
			if(!strcmp(temp->name, capitalize(arg)))
			{
				UNLINK(temp, ch->pcdata->first_ignored,
					ch->pcdata->last_ignored,
					next, prev);
				set_char_color(AT_IGNORE, ch);
				ch_printf(ch,"You no longer ignore %s.\n\r",
					temp->name);
				STRFREE(temp->name);
				DISPOSE(temp);
				return;
			}
		}
		
		/* if there wasn't a match check to see if the name   */
		/* is valid. This if-statement may seem like overkill */
		/* but it is intended to prevent people from doing the*/
		/* spam and log thing while still allowing ya to      */
		/* ignore new chars without pfiles yet...             */
		if( stat(fname, &fst) == -1 &&
			(!(victim = get_char_world(ch, arg)) ||
			IS_NPC(victim) ||
			strcmp(capitalize(arg),victim->name) != 0))
		{
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch,"No player exists by that"
				" name.\n\r");
			return;
		}
		
		if(victim)
		{
			strcpy(capitalize(arg),victim->name);
		}
		
		if (!check_parse_name( capitalize(arg), TRUE ))
		{
		    set_char_color( AT_IGNORE, ch );
		    send_to_char( "No player exists by that name.\n\r", ch );
		    return;
		}

		/* If its valid and the list size limit has not been */
		/* reached create a node and at it to the list	     */
		if(i < MAX_IGN)
		{
			IGNORE_DATA *new;
			CREATE(new, IGNORE_DATA, 1);
			new->name = STRALLOC(capitalize(arg));
			new->next = NULL;
			new->prev = NULL;
			LINK(new, ch->pcdata->first_ignored,
				ch->pcdata->last_ignored, next, prev);
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch,"You now ignore %s.\n\r", new->name);
			return;
		}
		else
		{
			set_char_color(AT_IGNORE, ch);
			ch_printf(ch,"You may only ignore %d players.\n\r",
				MAX_IGN);
			return;
		}
	}
}

/*
 * This function simply checks to see if ch is ignoring ign_ch.
 * Last Modified: October 10, 1997
 * - Fireblade
 */
bool is_ignoring(CHAR_DATA *ch, CHAR_DATA *ign_ch)
{
	IGNORE_DATA *temp;
	
	if(IS_NPC(ch) || IS_NPC(ign_ch))
		return FALSE;
	
	for(temp = ch->pcdata->first_ignored; temp; temp = temp->next)
	{
		if(nifty_is_name(temp->name, ign_ch->name))
			return TRUE;
	}
	
	return FALSE;
}

/*
 * Codebase Version Information - Orion Elder
 */
void do_version( CHAR_DATA *ch, char *argument )
{
    set_pager_color( CODEBASE_VERSION_CBAR, ch );
    pager_printf_color( ch, "\n\r%s\n\r",
	strlen( CODEBASE_VERSION_BAR ) > 0 ? CODEBASE_VERSION_BAR : "--==<0>==--" );
    set_pager_color( CODEBASE_VERSION_COLOR, ch );

    pager_printf_color( ch, "\n\r  %s Codebase\n\r    Version: %s.%s\n\r",
	CODEBASE_VERSION_TITLE, CODEBASE_VERSION_MAJOR, CODEBASE_VERSION_MINOR );

    if ( IS_IMMORTAL( ch ) )
    {
	pager_printf_color( ch, "    Compiled on %s at %s\n\r", __DATE__, __TIME__ );
    }

    set_pager_color( CODEBASE_VERSION_CBAR, ch );
    pager_printf_color( ch, "\n\r%s\n\r",
	strlen( CODEBASE_VERSION_BAR ) > 0 ? CODEBASE_VERSION_BAR : "--==<0>==--" );
    set_pager_color( CODEBASE_VERSION_COLOR, ch );

    pager_printf_color( ch, "\n\r\t%s\n\r",
	strlen( CODEBASE_VERSION_INFO ) > 0 ? CODEBASE_VERSION_INFO : "\n\r\tN//A\n\r" );

    set_pager_color( CODEBASE_VERSION_CBAR, ch );
    pager_printf_color( ch, "\n\r%s\n\r",
	strlen( CODEBASE_VERSION_BAR ) > 0 ? CODEBASE_VERSION_BAR : "--==<0>==--" );
    set_pager_color( CODEBASE_VERSION_COLOR, ch );

    return;
}
