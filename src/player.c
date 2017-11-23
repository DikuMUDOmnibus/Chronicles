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
 * - Player settings/statistics module                                     *
 ***************************************************************************/

#include <ctype.h>
#include <string.h>
#include <time.h>
#include "mud.h"
#include "deity.h"
#include "factions.h"
#include "language.h"
#include "polymorph.h"

/*
 *  Locals
 */
char *tiny_affect_loc_name(int location);

void do_gold(CHAR_DATA * ch, char *argument)
{
   set_char_color( AT_GOLD, ch );
   ch_printf( ch,  "You have %s gold pieces.\n\r", num_punct(ch->gold) );
   return;
}


void do_worth(CHAR_DATA *ch, char *argument)
{
    char            buf[MAX_STRING_LENGTH];
    char            buf2[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
      return;

    set_pager_color(AT_SCORE, ch);
    pager_printf(ch, "\n\rWorth for %s%s.\n\r", ch->name, ch->pcdata->title);
    send_to_pager(" ----------------------------------------------------------------------------\n\r", ch);
    if (!ch->pcdata->deity)		 sprintf( buf, "N/A" );
    else if (ch->pcdata->favor > 2250)	 sprintf( buf, "loved" );
    else if (ch->pcdata->favor > 2000)	 sprintf( buf, "cherished" );
    else if (ch->pcdata->favor > 1750) 	 sprintf( buf, "honored" );
    else if (ch->pcdata->favor > 1500)	 sprintf( buf, "praised" );
    else if (ch->pcdata->favor > 1250)	 sprintf( buf, "favored" );
    else if (ch->pcdata->favor > 1000)	 sprintf( buf, "respected" );
    else if (ch->pcdata->favor > 750)	 sprintf( buf, "liked" );
    else if (ch->pcdata->favor > 250)	 sprintf( buf, "tolerated" );
    else if (ch->pcdata->favor > -250)	 sprintf( buf, "ignored" );
    else if (ch->pcdata->favor > -750)	 sprintf( buf, "shunned" );
    else if (ch->pcdata->favor > -1000)	 sprintf( buf, "disliked" );
    else if (ch->pcdata->favor > -1250)	 sprintf( buf, "dishonored" );
    else if (ch->pcdata->favor > -1500)	 sprintf( buf, "disowned" );
    else if (ch->pcdata->favor > -1750)	 sprintf( buf, "abandoned" );
    else if (ch->pcdata->favor > -2000)	 sprintf( buf, "despised" );
    else if (ch->pcdata->favor > -2250)	 sprintf( buf, "hated" );
    else				 sprintf( buf, "damned" );

    if ( ch->level < 10 )
    {
       if (ch->alignment > 900)		 sprintf(buf2, "devout");
       else if (ch->alignment > 700)	 sprintf(buf2, "noble");
       else if (ch->alignment > 350)	 sprintf(buf2, "honorable");
       else if (ch->alignment > 100)	 sprintf(buf2, "worthy");
       else if (ch->alignment > -100)	 sprintf(buf2, "neutral");
       else if (ch->alignment > -350)	 sprintf(buf2, "base");
       else if (ch->alignment > -700)	 sprintf(buf2, "evil");
       else if (ch->alignment > -900)	 sprintf(buf2, "ignoble");
       else				 sprintf(buf2, "fiendish");      
    }
    else
	sprintf(buf2, "%d", ch->alignment );
    pager_printf(ch, "|Level: %-4d |Favor: %-10s |Alignment: %-9s |Experience: %-9d|\n\r", 
                     ch->level, buf, buf2, ch->exp );
    send_to_pager(" ----------------------------------------------------------------------------\n\r", ch);
        switch (ch->style) {
        case STYLE_EVASIVE:
                sprintf(buf, "evasive");
                break;
        case STYLE_DEFENSIVE:
                sprintf(buf, "defensive");
                break;
        case STYLE_AGGRESSIVE:
                sprintf(buf, "aggressive");
                break;
        case STYLE_BERSERK:
                sprintf(buf, "berserk");
                break;
        default:
                sprintf(buf, "standard");
                break;
        }
    pager_printf(ch, "|Glory: %-4d |Weight: %-9d |Style: %-13s |Gold: %-14s |\n\r",
                 ch->pcdata->quest_curr, ch->carry_weight, buf, num_punct(ch->gold) );
    send_to_pager(" ----------------------------------------------------------------------------\n\r", ch);
    if ( ch->level < 15 && !IS_PKILL(ch) )
    	pager_printf(ch, "|            |Hitroll: -------- |Damroll: ----------- |                     |\n\r" );
    else
    	pager_printf(ch, "|            |Hitroll: %-8d |Damroll: %-11d |                     |\n\r", GET_HITROLL(ch), GET_DAMROLL(ch) );
    send_to_pager(" ----------------------------------------------------------------------------\n\r", ch);
    return;
}


void do_oldscore( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf;
    SKILLTYPE   *skill;

    set_pager_color( AT_SCORE, ch );
    pager_printf( ch,
	"You are %s%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	ch->level,
	get_age(ch),
	(get_age(ch) - 17) * 2 );

    if ( get_trust( ch ) != ch->level )
	pager_printf( ch, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );

    if (  IS_NPC(ch) && xIS_SET(ch->act, ACT_MOBINVIS) )
      pager_printf( ch, "You are mobinvis at level %d.\n\r",
            ch->mobinvis);

    if ( !IS_NPC(ch) && IS_BLOODSUCKER(ch) )
      pager_printf( ch,
	"You have %d/%d hit, %d/%d blood level, %d/%d movement, %d practices.\n\r",
	ch->hit,  ch->max_hit,
	ch->pcdata->condition[COND_BLOODTHIRST], GetMaxBlood( ch ),
	ch->move, ch->max_move,
	ch->practice );
    else
      pager_printf( ch,
	"You have %d/%d hit, %d/%d mana, %d/%d movement, %d practices.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->practice );

    pager_printf( ch,
	"You are carrying %d/%d items with weight %d/%d kg.\n\r",
	ch->carry_number, can_carry_n(ch),
	ch->carry_weight, can_carry_w(ch) );

    pager_printf( ch,
	"Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d  Cha: %d  Lck: %d.\n\r",
	get_curr_str(ch),
	get_curr_int(ch),
	get_curr_wis(ch),
	get_curr_dex(ch),
	get_curr_con(ch),
	get_curr_cha(ch),
	get_curr_lck(ch) );

    pager_printf(ch,
	     "You have scored %s exp, and have ",num_punct(ch->exp));
    pager_printf(ch,
	     "%s gold coins.\n\r", num_punct(ch->gold) );


    if ( !IS_NPC(ch) )
    pager_printf( ch,
	"You have achieved %d glory during your life, and currently have %d.\n\r",
	ch->pcdata->quest_accum, ch->pcdata->quest_curr );

    pager_printf( ch,
	"Autoexit: %s   Autoloot: %s   Autosac: %s   Autogold: %s\n\r",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOSAC) ) ? "yes" : "no",
  	(!IS_NPC(ch) && xIS_SET(ch->act, PLR_AUTOGOLD)) ? "yes" : "no" );

    pager_printf( ch, "Wimpy set to %d hit points.\n\r", ch->wimpy );

    if ( !IS_NPC(ch) ) {
       if ( ch->pcdata->condition[COND_DRUNK]   > 10 )
	   send_to_pager( "You are drunk.\n\r",   ch );
       if ( ch->pcdata->condition[COND_THIRST] ==  0 )
	   send_to_pager( "You are thirsty.\n\r", ch );
       if ( ch->pcdata->condition[COND_FULL]   ==  0 )
	   send_to_pager( "You are hungry.\n\r",  ch );
    }

    switch( ch->mental_state / 10 )
    {
        default:   send_to_pager( "You're completely messed up!\n\r", ch ); break;
        case -10:  send_to_pager( "You're barely conscious.\n\r", ch ); break;
        case  -9:  send_to_pager( "You can barely keep your eyes open.\n\r", ch ); break;
        case  -8:  send_to_pager( "You're extremely drowsy.\n\r", ch ); break;
        case  -7:  send_to_pager( "You feel very unmotivated.\n\r", ch ); break;
        case  -6:  send_to_pager( "You feel sedated.\n\r", ch ); break;
        case  -5:  send_to_pager( "You feel sleepy.\n\r", ch ); break;
        case  -4:  send_to_pager( "You feel tired.\n\r", ch ); break;
        case  -3:  send_to_pager( "You could use a rest.\n\r", ch ); break;
        case  -2:  send_to_pager( "You feel a little under the weather.\n\r", ch ); break;
        case  -1:  send_to_pager( "You feel fine.\n\r", ch ); break;
        case   0:  send_to_pager( "You feel great.\n\r", ch ); break;
        case   1:  send_to_pager( "You feel energetic.\n\r", ch ); break;
        case   2:  send_to_pager( "Your mind is racing.\n\r", ch ); break;
        case   3:  send_to_pager( "You can't think straight.\n\r", ch ); break;
        case   4:  send_to_pager( "Your mind is going 100 miles an hour.\n\r", ch ); break;
        case   5:  send_to_pager( "You're high as a kite.\n\r", ch ); break;
        case   6:  send_to_pager( "Your mind and body are slipping appart.\n\r", ch ); break;
        case   7:  send_to_pager( "Reality is slipping away.\n\r", ch ); break;
        case   8:  send_to_pager( "You have no idea what is real, and what is not.\n\r", ch ); break;
        case   9:  send_to_pager( "You feel immortal.\n\r", ch ); break;
        case  10:  send_to_pager( "You are a Supreme Entity.\n\r", ch ); break;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_pager( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_pager( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_pager( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_pager( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_pager( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_pager( "You are resting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_pager( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_pager( "You are fighting.\n\r",		ch );
	break;
    case POS_MOUNTED:
	send_to_pager( "Mounted.\n\r",			ch );
	break;
    case POS_SHOVE:
	send_to_pager( "Being shoved.\n\r",		ch );
	break;
    case POS_DRAG:
	send_to_pager( "Being dragged.\n\r",		ch );
	break;
    }

    if ( ch->level >= 25 )
	pager_printf( ch, "AC: %d.  ", GET_AC(ch) );

    send_to_pager( "You are ", ch );
	 if ( GET_AC(ch) >=  101 ) send_to_pager( "WORSE than naked!\n\r", ch );
    else if ( GET_AC(ch) >=   80 ) send_to_pager( "naked.\n\r",            ch );
    else if ( GET_AC(ch) >=   60 ) send_to_pager( "wearing clothes.\n\r",  ch );
    else if ( GET_AC(ch) >=   40 ) send_to_pager( "slightly armored.\n\r", ch );
    else if ( GET_AC(ch) >=   20 ) send_to_pager( "somewhat armored.\n\r", ch );
    else if ( GET_AC(ch) >=    0 ) send_to_pager( "armored.\n\r",          ch );
    else if ( GET_AC(ch) >= - 20 ) send_to_pager( "well armored.\n\r",     ch );
    else if ( GET_AC(ch) >= - 40 ) send_to_pager( "strongly armored.\n\r", ch );
    else if ( GET_AC(ch) >= - 60 ) send_to_pager( "heavily armored.\n\r",  ch );
    else if ( GET_AC(ch) >= - 80 ) send_to_pager( "superbly armored.\n\r", ch );
    else if ( GET_AC(ch) >= -100 ) send_to_pager( "divinely armored.\n\r", 
ch );
    else                           send_to_pager( "invincible!\n\r",       ch );

    if ( ch->level >= 15
    ||   IS_PKILL( ch ) )
	pager_printf( ch, "Hitroll: %d  Damroll: %d.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );

    if ( ch->level >= 10 )
	pager_printf( ch, "Alignment: %d.  ", ch->alignment );

    send_to_pager( "You are ", ch );
	 if ( ch->alignment >  900 ) send_to_pager( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_pager( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_pager( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_pager( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_pager( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_pager( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_pager( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_pager( "demonic.\n\r", ch );
    else                             send_to_pager( "satanic.\n\r", ch );

    if ( ch->first_affect )
    {
	send_to_pager( "You are affected by:\n\r", ch );
	for ( paf = ch->first_affect; paf; paf = paf->next )
	    if ( (skill=get_skilltype(paf->type)) != NULL )
	{
	    pager_printf( ch, "Spell: '%s'", skill->name );

	    if ( ch->level >= 20 )
		pager_printf( ch,
		    " modifies %s by %d for %d rounds",
		    affect_loc_name( paf->location ),
		    paf->modifier,
		    paf->duration );

	    send_to_pager( ".\n\r", ch );
	}
    }

    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
    {
	pager_printf( ch, "\n\rWizInvis level: %d   WizInvis is %s\n\r",
			ch->pcdata->wizinvis,
			xIS_SET(ch->act, PLR_WIZINVIS) ? "ON" : "OFF" );
	if ( ch->pcdata->r_range_lo && ch->pcdata->r_range_hi )
	  pager_printf( ch, "Room Range: %d - %d\n\r", ch->pcdata->r_range_lo,
					 	   ch->pcdata->r_range_hi	);
	if ( ch->pcdata->o_range_lo && ch->pcdata->o_range_hi )
	  pager_printf( ch, "Obj Range : %d - %d\n\r", ch->pcdata->o_range_lo,
	  					   ch->pcdata->o_range_hi	);
	if ( ch->pcdata->m_range_lo && ch->pcdata->m_range_hi )
	  pager_printf( ch, "Mob Range : %d - %d\n\r", ch->pcdata->m_range_lo,
	  					   ch->pcdata->m_range_hi	);
    }

    return;
}


/*
 * This command has been updated to clean up a lot of the inherited bad code.
 * With a little cleaning up, it has become this new function. -Orion
 * 
 * Old version by Haus
 */
void do_score( CHAR_DATA * ch, char *argument )
{
    int		iLang;
    char	*divider = 
	"============================================================================\n\r";
    char	buf[MAX_STRING_LENGTH], hr_buf[MAX_STRING_LENGTH], dr_buf[MAX_STRING_LENGTH], print_buf[MAX_STRING_LENGTH];
    AFFECT_DATA	*paf;

    buf[0]	= hr_buf[0] = dr_buf[0] = print_buf[0] = '\0';

    if ( IS_NPC( ch ) )
    {
	do_oldscore( ch, argument );
	return;
    }

    set_pager_color( AT_SCORE, ch );

    pager_printf( ch, "\n\rScore for %s%s.\n\r", ch->name, ch->pcdata->title);

    if ( get_trust( ch ) != ch->level )
	pager_printf( ch, "You are trusted at level %d.\n\r", get_trust( ch ) );


    /*
     * Print the score divider. -Orion
     */    
    send_to_pager( divider, ch );


    pager_printf(ch, "LEVEL: %-3d         Race : %-10.10s        Played: %d hours\n\r",
	ch->level, capitalize(get_race(ch)), (get_age(ch) - 17) * 2);

    pager_printf(ch, "YEARS: %-6d      Class: %-11.11s       Log In: %s\r",
		get_age(ch), capitalize(get_class(ch)), ctime(&(ch->logon)) );

    if (ch->level >= 15
    ||  IS_PKILL( ch ) )
    {
	sprintf( hr_buf, "HitRoll: %-4d", GET_HITROLL( ch ) );
	sprintf( dr_buf, "DamRoll: %-4d", GET_DAMROLL( ch ) );
    }
    else
    {
	sprintf( hr_buf, "%s", "" );
	sprintf( dr_buf, "%s", "" );
    }

    pager_printf(ch, "STR  : %2.2d(%2.2d)    %13s              Saved : %s\r",
	get_curr_str(ch), ch->perm_str, hr_buf,
	ch->save_time ? ctime(&(ch->save_time)) : "N/A\n" );

    pager_printf(ch, "INT  : %2.2d(%2.2d)    %13s              Time  : %s\r",
	get_curr_int(ch), ch->perm_int, dr_buf, ctime(&current_time) );



    if (GET_AC(ch) >= 101)
	sprintf(buf, "the rags of a beggar");
    else if (GET_AC(ch) >= 80)
	sprintf(buf, "improper for adventure");
    else if (GET_AC(ch) >= 55)
	sprintf(buf, "shabby and threadbare");
    else if (GET_AC(ch) >= 40)
	sprintf(buf, "of poor quality");
    else if (GET_AC(ch) >= 20)
	sprintf(buf, "scant protection");
    else if (GET_AC(ch) >= 10)
	sprintf(buf, "that of a knave");
    else if (GET_AC(ch) >= 0)
	sprintf(buf, "moderately crafted");
    else if (GET_AC(ch) >= -10)
	sprintf(buf, "well crafted");
    else if (GET_AC(ch) >= -20)
	sprintf(buf, "the envy of squires");
    else if (GET_AC(ch) >= -40)
	sprintf(buf, "excellently crafted");
    else if (GET_AC(ch) >= -60)
	sprintf(buf, "the envy of knights");
    else if (GET_AC(ch) >= -80)
	sprintf(buf, "the envy of barons");
    else if (GET_AC(ch) >= -100)
	sprintf(buf, "the envy of dukes");
    else if (GET_AC(ch) >= -200)
	sprintf(buf, "the envy of emperors");
    else
	sprintf(buf, "that of an avatar");



    if ( ch->level > 24 )
    {
	sprintf( print_buf, "%4.4d, %s", GET_AC( ch ), buf );
    }
    else
    {
	sprintf( print_buf, "%s", buf );
    }

    pager_printf( ch, "WIS  : %2.2d(%2.2d)      Armor: %s\n\r",
	get_curr_wis( ch ), ch->perm_wis, print_buf );



    if (ch->alignment > 900)
	sprintf(buf, "devout");
    else if (ch->alignment > 700)
	sprintf(buf, "noble");
    else if (ch->alignment > 350)
	sprintf(buf, "honorable");
    else if (ch->alignment > 100)
	sprintf(buf, "worthy");
    else if (ch->alignment > -100)
	sprintf(buf, "neutral");
    else if (ch->alignment > -350)
	sprintf(buf, "base");
    else if (ch->alignment > -700)
	sprintf(buf, "evil");
    else if (ch->alignment > -900)
	sprintf(buf, "ignoble");
    else
	sprintf(buf, "fiendish");


    if (ch->level < 10)
    {
	sprintf( print_buf, "%-17.17s", buf );
    }
    else
    {
	sprintf( print_buf, "%+4.4d, %-11.11s", ch->alignment, buf );
    }

    pager_printf( ch, "DEX  : %2.2d(%2.2d)      Align: %-17.17s Items : %5.5d (Max %5.5d)\n\r",
	get_curr_dex( ch ), ch->perm_dex, print_buf, ch->carry_number, can_carry_n( ch ) );



    switch (ch->position)
    {
	case POS_DEAD:
		sprintf(buf, "decomposing");
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
    }



    pager_printf( ch, "CON  : %2.2d(%2.2d)      Pos'n: %-17.17s Weight: %5.5d (Max %7.7d)\n\r",
	get_curr_con( ch ), ch->perm_con, buf, ch->carry_weight, can_carry_w( ch ) );


    /*
     * Fighting style support -haus
     */
    switch (ch->style)
    {
        case STYLE_EVASIVE:
                sprintf(buf, "evasive");
                break;
        case STYLE_DEFENSIVE:
                sprintf(buf, "defensive");
                break;
        case STYLE_AGGRESSIVE:
                sprintf(buf, "aggressive");
                break;
        case STYLE_BERSERK:
                sprintf(buf, "berserk");
                break;
        default:
                sprintf(buf, "standard");
                break;
    }

    pager_printf( ch, "CHA  : %2.2d(%2.2d)      Wimpy: %-5d             Style : %-10.10s\n\r",
	get_curr_cha( ch ), ch->perm_cha, ch->wimpy, buf );
 
    pager_printf( ch, "LCK  : %2.2d(%2.2d) \n\r",
	get_curr_lck( ch ), ch->perm_lck );

    pager_printf( ch, "Glory: %4.4d(%4.4d) \n\r",
	ch->pcdata->quest_curr, ch->pcdata->quest_accum );

    pager_printf( ch, "PRACT: %3.3d         Hitpoints: %-5d of %5d   Pager  : [%c]%2d    AutoExit[%c]\n\r",
	ch->practice,
	ch->hit,
	ch->max_hit,
	IS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) ? '*' : ' ',
	ch->pcdata->pagerlen,
	xIS_SET( ch->act, PLR_AUTOEXIT ) ? '*' : ' ');

    if ( IS_BLOODSUCKER( ch ) )
    {
	sprintf( print_buf, "Blood: %-5d of %5d",
	    ch->pcdata->condition[COND_BLOODTHIRST],
	    GetMaxBlood( ch ) );
    }
    else if ( CAN_CAST( ch ) )
    {
	sprintf( print_buf, " Mana: %-5d of %5d",
	    ch->mana,
	    ch->max_mana );
    }
    else
    {
	sprintf( print_buf, "%s", " " );
    }
    
    pager_printf( ch, "EXP  : %-9d       %-21.21s   MKills : %-5.5d    AutoLoot[%c]\n\r",
	ch->exp,
	print_buf,
	ch->pcdata->mkills,
	xIS_SET( ch->act, PLR_AUTOLOOT ) ? '*' : ' ' );

    pager_printf( ch, "GOLD : %-13s    Move: %-5d of %5d   Mdeaths: %-5.5d    AutoSac [%c]\n\r",
	num_punct( ch->gold ), ch->move, ch->max_move, ch->pcdata->mdeaths, xIS_SET( ch->act, PLR_AUTOSAC ) ? '*' : ' ' );

    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	send_to_pager("You are drunk.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
	send_to_pager("You are in danger of dehydrating.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] == 0)
	send_to_pager("You are starving to death.\n\r", ch);
    if ( ch->position != POS_SLEEPING )
	switch( ch->mental_state / 10 )
	{
	    default:   send_to_pager( "You're completely messed up!\n\r", ch );	break;
	    case -10:  send_to_pager( "You're barely conscious.\n\r", ch );	break;
	    case  -9:  send_to_pager( "You can barely keep your eyes open.\n\r", ch );	break;
	    case  -8:  send_to_pager( "You're extremely drowsy.\n\r", ch );	break;
	    case  -7:  send_to_pager( "You feel very unmotivated.\n\r", ch );	break;
	    case  -6:  send_to_pager( "You feel sedated.\n\r", ch );		break;
	    case  -5:  send_to_pager( "You feel sleepy.\n\r", ch );		break;
	    case  -4:  send_to_pager( "You feel tired.\n\r", ch );		break;
	    case  -3:  send_to_pager( "You could use a rest.\n\r", ch );		break;
	    case  -2:  send_to_pager( "You feel a little under the weather.\n\r", ch );	break;
	    case  -1:  send_to_pager( "You feel fine.\n\r", ch );		break;
	    case   0:  send_to_pager( "You feel great.\n\r", ch );		break;
	    case   1:  send_to_pager( "You feel energetic.\n\r", ch );	break;
	    case   2:  send_to_pager( "Your mind is racing.\n\r", ch );	break;
	    case   3:  send_to_pager( "You can't think straight.\n\r", ch );	break;
	    case   4:  send_to_pager( "Your mind is going 100 miles an hour.\n\r", ch );	break;
	    case   5:  send_to_pager( "You're high as a kite.\n\r", ch );	break;
	    case   6:  send_to_pager( "Your mind and body are slipping apart.\n\r", ch );	break;
	    case   7:  send_to_pager( "Reality is slipping away.\n\r", ch );	break;
	    case   8:  send_to_pager( "You have no idea what is real, and what is not.\n\r", ch );	break;
	    case   9:  send_to_pager( "You feel immortal.\n\r", ch );	break;
	    case  10:  send_to_pager( "You are a Supreme Entity.\n\r", ch );	break;
	}
    else
    if ( ch->mental_state >45 )
	send_to_pager( "Your sleep is filled with strange and vivid dreams.\n\r", ch );
    else
    if ( ch->mental_state >25 )
	send_to_pager( "Your sleep is uneasy.\n\r", ch );
    else
    if ( ch->mental_state <-35 )
	send_to_pager( "You are deep in a much needed sleep.\n\r", ch );
    else
    if ( ch->mental_state <-25 )
	send_to_pager( "You are in deep slumber.\n\r", ch );
    send_to_pager("Languages: ", ch );
    for ( iLang = 0; lang_array[iLang] != LANG_UNKNOWN; iLang++ )
    {
	if ( knows_language( ch, lang_array[iLang], ch )
	||  (IS_NPC(ch) && ch->speaks == 0) )
	{
	    if ( lang_array[iLang] & ch->speaking
	    ||  (IS_NPC(ch) && !ch->speaking) )
		set_pager_color( AT_RED, ch );
	    send_to_pager( lang_names[iLang], ch );
	    send_to_pager( " ", ch );
	    set_pager_color( AT_SCORE, ch );
	}
    }
    send_to_pager( "\n\r", ch );

    if ( ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' )
	pager_printf( ch, "You are bestowed with the command(s): %s.\n\r", 
		ch->pcdata->bestowments );

    if ( ch->morph && ch->morph->morph )
    {
	/*
	 * Print the score divider. -Orion
	 */
	send_to_pager( divider, ch );


      if ( IS_IMMORTAL( ch ) )
         pager_printf (ch, "Morphed as (%d) %s with a timer of %d.\n\r",
                ch->morph->morph->vnum, ch->morph->morph->short_desc, 
		ch->morph->timer
                );
      else
        pager_printf (ch, "You are morphed into a %s.\n\r",
                ch->morph->morph->short_desc );
    } 
    if ( CAN_PKILL( ch ) )
    {
	/*
	 * Print the score divider. -Orion
	 */
	send_to_pager( divider, ch );


	pager_printf(ch, "PKILL DATA:  Pkills (%3.3d)     Illegal Pkills (%3.3d)     Pdeaths (%3.3d)\n\r",
		ch->pcdata->pkills, ch->pcdata->illegal_pk, ch->pcdata->pdeaths );
    }
    if ( IS_CLANNED( ch ) )
    {
	/*
	 * Print the score divider. -Orion
	 */
	send_to_pager( divider, ch );


	pager_printf(ch, "CLAN STATS:  %-14.14s  Clan AvPkills : %-5d  Clan NonAvpkills : %-5d\n\r",
		ch->pcdata->faction->name, ch->pcdata->faction->pkills[6],
		(ch->pcdata->faction->pkills[1]+ch->pcdata->faction->pkills[2]+
		 ch->pcdata->faction->pkills[3]+ch->pcdata->faction->pkills[4]+
		 ch->pcdata->faction->pkills[5]) );
        pager_printf(ch, "                             Clan AvPdeaths: %-5d  Clan NonAvpdeaths: %-5d\n\r",
		ch->pcdata->faction->pdeaths[6],
		( ch->pcdata->faction->pdeaths[1] + ch->pcdata->faction->pdeaths[2] +
		  ch->pcdata->faction->pdeaths[3] + ch->pcdata->faction->pdeaths[4] +
		  ch->pcdata->faction->pdeaths[5] ) );
    }
    if (ch->pcdata->deity)
    {
	/*
	 * Print the score divider. -Orion
	 */
	send_to_pager( divider, ch );


	if (ch->pcdata->favor > 2250)
	  sprintf( buf, "loved" );
	else if (ch->pcdata->favor > 2000)
	  sprintf( buf, "cherished" );
	else if (ch->pcdata->favor > 1750)
	  sprintf( buf, "honored" );
	else if (ch->pcdata->favor > 1500)
	  sprintf( buf, "praised" );
	else if (ch->pcdata->favor > 1250)
	  sprintf( buf, "favored" );
	else if (ch->pcdata->favor > 1000)
	  sprintf( buf, "respected" );
	else if (ch->pcdata->favor > 750)
	  sprintf( buf, "liked" );
	else if (ch->pcdata->favor > 250)
	  sprintf( buf, "tolerated" );
	else if (ch->pcdata->favor > -250)
	  sprintf( buf, "ignored" );
	else if (ch->pcdata->favor > -750)
	  sprintf( buf, "shunned" );
	else if (ch->pcdata->favor > -1000)
	  sprintf( buf, "disliked" );
	else if (ch->pcdata->favor > -1250)
	  sprintf( buf, "dishonored" );
	else if (ch->pcdata->favor > -1500)
	  sprintf( buf, "disowned" );
	else if (ch->pcdata->favor > -1750)
	  sprintf( buf, "abandoned" );
	else if (ch->pcdata->favor > -2000)
	  sprintf( buf, "despised" );
	else if (ch->pcdata->favor > -2250)
	  sprintf( buf, "hated" );
	else
	  sprintf( buf, "damned" );
	pager_printf(ch, "Deity:  %-20s  Favor: %s\n\r", ch->pcdata->deity->name, buf );
    }
    if ( IS_ORDERED( ch ) )
    {
	/*
	 * Print the score divider. -Orion
	 */
	send_to_pager( divider, ch );

	pager_printf(ch, "Order:  %-20s  Order Mkills:  %-6d   Order MDeaths:  %-6d\n\r",
		ch->pcdata->faction->name, ch->pcdata->faction->mkills, ch->pcdata->faction->mdeaths);
    }
    if ( IS_GUILDED( ch ) )
    {
	/*
	 * Print the score divider. -Orion
	 */
	send_to_pager( divider, ch );

        pager_printf(ch, "Guild:  %-20s  Guild Mkills:  %-6d   Guild MDeaths:  %-6d\n\r",
                ch->pcdata->faction->name, ch->pcdata->faction->mkills, ch->pcdata->faction->mdeaths);
    }
    if (IS_IMMORTAL(ch))
    {
	/*
	 * Print the score divider. -Orion
	 */
	send_to_pager( divider, ch );


	pager_printf(ch, "IMMORTAL DATA: Wizinvis [%c]  Wizlevel (%d)\n\r",
		xIS_SET(ch->act, PLR_WIZINVIS) ? '*' : ' ', ch->pcdata->wizinvis );

	pager_printf(ch, "Bamfin       : %s %s\n\r", ch->name, (ch->pcdata->bamfin[0] != '\0')
		? ch->pcdata->bamfin : "appears in a swirling mist.");
	pager_printf(ch, "Bamfout      : %s %s\n\r", ch->name, (ch->pcdata->bamfout[0] != '\0')
		? ch->pcdata->bamfout : "leaves in a swirling mist.");


	/* Area Loaded info - Scryn 8/11*/
	if (ch->pcdata->area)
	{
	pager_printf(ch, "Vnums        : Room (%-5.5d - %-5.5d)  Object (%-5.5d - %-5.5d)  Mob (%-5.5d - %-5.5d)\n\r",
		ch->pcdata->area->low_r_vnum, ch->pcdata->area->hi_r_vnum,
		ch->pcdata->area->low_o_vnum, ch->pcdata->area->hi_o_vnum,
		ch->pcdata->area->low_m_vnum, ch->pcdata->area->hi_m_vnum);
	    pager_printf(ch, "Area Loaded [%c]\n\r", (IS_SET (ch->pcdata->area->status, AREA_LOADED)) ? '*' : ' ' );
	}
    }
    if (ch->first_affect)
    {
	int i;
	SKILLTYPE *sktmp;

	i = 0;


	/*
	 * Print the score divider. -Orion
	 */
	send_to_pager( divider, ch );


	send_to_pager("AFFECT DATA:                            ", ch);
	for (paf = ch->first_affect; paf; paf = paf->next)
	{
	    if ( (sktmp=get_skilltype(paf->type)) == NULL )
		continue;
	    if (ch->level < 20)
	    {
		pager_printf(ch, "[%-34.34s]    ", sktmp->name);
		if (i == 0)
		   i = 2;
		if ((++i % 3) == 0)
		   send_to_pager("\n\r", ch);
	     }
	     if (ch->level >= 20)
	     {
		if (paf->modifier == 0)
		    pager_printf(ch, "[%-24.24s;%5d rds]    ",
			sktmp->name,
			paf->duration);
		else
		if (paf->modifier > 999)
		    pager_printf(ch, "[%-15.15s; %7.7s;%5d rds]    ",
			sktmp->name,
			tiny_affect_loc_name(paf->location),
			paf->duration);
		else
		    pager_printf(ch, "[%-11.11s;%+-3.3d %7.7s;%5d rds]    ",
			sktmp->name,
			paf->modifier,
			tiny_affect_loc_name(paf->location),
			paf->duration);
		if (i == 0)
		    i = 1;
		if ((++i % 2) == 0)
		    send_to_pager("\n\r", ch);
	    }
	}
    }
    send_to_pager("\n\r", ch);
    return;
}

/*
 * Return ascii name of an affect location.
 */
char           *
tiny_affect_loc_name(int location)
{
	switch (location) {
	case APPLY_NONE:		return "NIL";
	case APPLY_STR:			return " STR  ";
	case APPLY_DEX:			return " DEX  ";
	case APPLY_INT:			return " INT  ";
	case APPLY_WIS:			return " WIS  ";
	case APPLY_CON:			return " CON  ";
	case APPLY_CHA:			return " CHA  ";
	case APPLY_LCK:			return " LCK  ";
	case APPLY_SEX:			return " SEX  ";
	case APPLY_CLASS:		return " CLASS";
	case APPLY_LEVEL:		return " LVL  ";
	case APPLY_AGE:			return " AGE  ";
	case APPLY_MANA:		return " MANA ";
	case APPLY_HIT:			return " HV   ";
	case APPLY_MOVE:		return " MOVE ";
	case APPLY_GOLD:		return " GOLD ";
	case APPLY_EXP:			return " EXP  ";
	case APPLY_AC:			return " AC   ";
	case APPLY_HITROLL:		return " HITRL";
	case APPLY_DAMROLL:		return " DAMRL";
	case APPLY_SAVING_POISON:	return "SV POI";
	case APPLY_SAVING_ROD:		return "SV ROD";
	case APPLY_SAVING_PARA:		return "SV PARA";
	case APPLY_SAVING_BREATH:	return "SV BRTH";
	case APPLY_SAVING_SPELL:	return "SV SPLL";
	case APPLY_HEIGHT:		return "HEIGHT";
	case APPLY_WEIGHT:		return "WEIGHT";
	case APPLY_AFFECT:		return "AFF BY";
	case APPLY_RESISTANT:		return "RESIST";
	case APPLY_IMMUNE:		return "IMMUNE";
	case APPLY_SUSCEPTIBLE:		return "SUSCEPT";
	case APPLY_WEAPONSPELL:		return " WEAPON";
	case APPLY_BACKSTAB:		return "BACKSTB";
	case APPLY_PICK:		return " PICK  ";
	case APPLY_TRACK:		return " TRACK ";
	case APPLY_STEAL:		return " STEAL ";
	case APPLY_SNEAK:		return " SNEAK ";
	case APPLY_HIDE:		return " HIDE  ";
	case APPLY_PALM:		return " PALM  ";
	case APPLY_DETRAP:		return " DETRAP";
	case APPLY_DODGE:		return " DODGE ";
	case APPLY_PEEK:		return " PEEK  ";
	case APPLY_SCAN:		return " SCAN  ";
	case APPLY_GOUGE:		return " GOUGE ";
	case APPLY_SEARCH:		return " SEARCH";
	case APPLY_MOUNT:		return " MOUNT ";
	case APPLY_DISARM:		return " DISARM";
	case APPLY_KICK:		return " KICK  ";
	case APPLY_PARRY:		return " PARRY ";
	case APPLY_BASH:		return " BASH  ";
	case APPLY_STUN:		return " STUN  ";
	case APPLY_PUNCH:		return " PUNCH ";
	case APPLY_CLIMB:		return " CLIMB ";
	case APPLY_GRIP:		return " GRIP  ";
	case APPLY_SCRIBE:		return " SCRIBE";
	case APPLY_BREW:		return " BREW  ";
	case APPLY_WEARSPELL:		return " WEAR  ";
	case APPLY_REMOVESPELL:		return " REMOVE";
	case APPLY_EMOTION:		return "EMOTION";
	case APPLY_MENTALSTATE:		return " MENTAL";
	case APPLY_STRIPSN:		return " DISPEL";
	case APPLY_REMOVE:		return " REMOVE";
	case APPLY_DIG:			return " DIG   ";
	case APPLY_FULL:		return " HUNGER";
	case APPLY_THIRST:		return " THIRST";
	case APPLY_DRUNK:		return " DRUNK ";
	case APPLY_BLOOD:		return " BLOOD ";
	case APPLY_COOK:		return " COOK  ";
	case APPLY_RECURRINGSPELL:	return " RECURR";
	case APPLY_CONTAGIOUS:		return "CONTGUS";
	case APPLY_ODOR:		return " ODOR  ";
	case APPLY_ROOMFLAG:		return " RMFLG ";
	case APPLY_SECTORTYPE:		return " SECTOR";
	case APPLY_ROOMLIGHT:		return " LIGHT ";
	case APPLY_TELEVNUM:		return " TELEVN";
	case APPLY_TELEDELAY:		return " TELEDY";
	};

	bug("Affect_location_name: unknown location %d.", location);
	return "(??\?)";
}

char *
get_class(CHAR_DATA *ch)
{
    if ( IS_NPC(ch) && ch->class < MAX_NPC_CLASS && ch->class >= 0)
    	return ( npc_class[ch->class] );
    else if ( !IS_NPC(ch) && ch->class < MAX_PC_CLASS && ch->class >= 0 )
        return class_table[ch->class]->who_name;
    return ("Unknown");
}


char *
get_race( CHAR_DATA *ch)
{
    if(  ch->race < MAX_PC_RACE  && ch->race >= 0)
        return (race_table[ch->race]->race_name);
    if ( ch->race < MAX_NPC_RACE && ch->race >= 0)
	return ( npc_race[ch->race] );
    return ("Unknown");
}

/*								-Thoric
 * Display your current exp, level, and surrounding level exp requirements
 */
void do_level( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int x, lowlvl, hilvl;

    if ( ch->level == 1 )
      lowlvl = 1;
    else
      lowlvl = UMAX( 2, ch->level - 5 );
    hilvl = URANGE( ch->level, ch->level + 5, MAX_LEVEL );
    set_char_color( AT_SCORE, ch );
    ch_printf( ch, "\n\rExperience required, levels %d to %d:\n\r______________________________________________\n\r\n\r", lowlvl, hilvl );
    sprintf( buf, " exp  (Current: %12s)", num_punct(ch->exp) );
    sprintf( buf2," exp  (Needed:  %12s)", num_punct( exp_level(ch, ch->level+1) - ch->exp) );
    for ( x = lowlvl; x <= hilvl; x++ )
	ch_printf( ch, " (%2d) %12s%s\n\r", x, num_punct( exp_level( ch, x ) ),
		(x == ch->level) ? buf : (x == ch->level+1) ? buf2 : " exp" );
    send_to_char( "______________________________________________\n\r", ch );
}

/* 1997, Blodkai */
void do_remains( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    bool found = FALSE;
 
    if ( IS_NPC( ch ) )
      return;
    set_char_color( AT_MAGIC, ch );
    if ( !ch->pcdata->deity ) {
      send_to_pager( "You have no deity from which to seek such assistance...\n\r", ch );
      return;
    }
    if ( ch->pcdata->favor < ch->level*2 ) {
      send_to_pager( "Your favor is insufficient for such assistance...\n\r", ch );
      return;
    }
    pager_printf( ch, "%s appears in a vision, revealing that your remains... ", ch->pcdata->deity->name );
    sprintf( buf, "the corpse of %s", ch->name );
    for ( obj = first_object; obj; obj = obj->next ) {
      if ( obj->in_room && !str_cmp( buf, obj->short_descr )
      && ( obj->pIndexData->vnum == 11 ) ) {
        found = TRUE;
        pager_printf( ch, "\n\r  - at %s will endure for %d ticks",
          obj->in_room->name,
          obj->timer );
      }
    }
    if ( !found )
      send_to_pager( " no longer exist.\n\r", ch );
    else
    {
      send_to_pager( "\n\r", ch );
      ch->pcdata->favor -= ch->level*2;
    }
    return;
}

/* Affects-at-a-glance, Blodkai */
void do_affected ( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    SKILLTYPE *skill;
 
    if ( IS_NPC(ch) )
	return;

    set_char_color( AT_SCORE, ch );

    argument = one_argument( argument, arg );
    if ( !str_cmp( arg, "by" ) )
    {
        send_to_char_color( "\n\r&BImbued with:\n\r", ch );
	ch_printf_color( ch, "&C%s\n\r",
	  !xIS_EMPTY(ch->affected_by) ? affect_bit_name( &ch->affected_by ) : "nothing" );
        if ( ch->level >= 20 )
        {
            send_to_char( "\n\r", ch );
            if ( ch->resistant > 0 )
	    {
                send_to_char_color( "&BResistances:  ", ch );
                ch_printf_color( ch, "&C%s\n\r", flag_string(ch->resistant, ris_flags) );
	    }
            if ( ch->immune > 0 )
	    {
                send_to_char_color( "&BImmunities:   ", ch);
                ch_printf_color( ch, "&C%s\n\r",  flag_string(ch->immune, ris_flags) );
	    }
            if ( ch->susceptible > 0 )
	    {
                send_to_char_color( "&BSuscepts:     ", ch );
                ch_printf_color( ch, "&C%s\n\r", flag_string(ch->susceptible, ris_flags) );
	    }
        }
	return;      
    }

    if ( !ch->first_affect )
    {
        send_to_char_color( "\n\r&CNo cantrip or skill affects you.\n\r", ch );
    }
    else
    {
	send_to_char( "\n\r", ch );
        for (paf = ch->first_affect; paf; paf = paf->next)
	    if ( (skill=get_skilltype(paf->type)) != NULL )
        {
	    set_char_color( AT_BLUE, ch );
            send_to_char( "Affected:  ", ch );
            set_char_color( AT_SCORE, ch );
            if ( ch->level >= 20
	    ||   IS_PKILL( ch ) )
            {
                if (paf->duration < 25 ) set_char_color( AT_WHITE, ch );
                if (paf->duration < 6  ) set_char_color( AT_WHITE + AT_BLINK, ch );
                ch_printf( ch, "(%5d)   ", paf->duration );
	    }
            ch_printf( ch, "%-18s\n\r", skill->name );
        }
    }
    return;
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_RED, ch );
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->first_carrying, ch, TRUE, TRUE );
    return;
}


void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;
    bool empty_slot;

    set_char_color( AT_RED, ch );
    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    set_char_color( AT_OBJECT, ch );
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	empty_slot = TRUE;

	for ( obj = ch->first_carrying; obj; obj = obj->next_content )
	{
	    if ( obj->wear_loc == iWear )
	    {
		empty_slot = FALSE;

                if( (!IS_NPC(ch)) && (ch->race>0) && (ch->race<MAX_PC_RACE))
                    send_to_char(race_table[ch->race]->where_name[iWear], ch);
                else
                    send_to_char( where_name[iWear], ch );

		if ( can_see_obj( ch, obj ) )
		{
		    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
		    send_to_char( "\n\r", ch );
		}
		else
		    send_to_char( "something.\n\r", ch );
		found = TRUE;
	    }
	}

	if ( empty_slot )
	{
	    if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_SHOWSLOTS ) )
	    {
		if ( ( ch->race >= 0 ) && ( ch->race < MAX_PC_RACE ) )
		    send_to_char(race_table[ch->race]->where_name[iWear], ch);
		else
		    send_to_char( where_name[iWear], ch );

		send_to_char( "Nothing.\n\r", ch );
	    }
	}
    }

    if ( IS_NPC( ch ) )
	send_to_char( "Nothing.\n\r", ch );
    else if ( !found && !IS_SET( ch->pcdata->flags, PCFLAG_SHOWSLOTS ) )
	send_to_char( "Nothing.\n\r", ch );

    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
	strcpy( buf, title );

    STRFREE( ch->pcdata->title );
    ch->pcdata->title = STRALLOC( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    set_char_color( AT_SCORE, ch );
    if ( ch->level < 5 )
    {
	send_to_char( "Sorry... you must be at least level 5 to set your title...\n\r", ch );
	return;
    }
    if ( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ))
    {
	set_char_color( AT_IMMORT, ch );
        send_to_char( "The Gods prohibit you from changing your title.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 50 )
	argument[50] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}


void do_homepage( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
	return;

    if ( ch->level < 5 )
    {
	send_to_char( "Sorry... you must be at least level 5 to do that.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	if ( !ch->pcdata->homepage )
	  ch->pcdata->homepage = str_dup( "" );
	ch_printf( ch, "Your homepage is: %s\n\r",
		show_tilde( ch->pcdata->homepage ) );
	return;
    }

    if ( !str_cmp( argument, "clear" ) )
    {
	if ( ch->pcdata->homepage )
	  DISPOSE(ch->pcdata->homepage);
	ch->pcdata->homepage = str_dup("");
	send_to_char( "Homepage cleared.\n\r", ch );
	return;
    }

    if ( strstr( argument, "://" ) )
	strcpy( buf, argument );
    else
	sprintf( buf, "http://%s", argument );
    if ( strlen(buf) > 70 )
	buf[70] = '\0';

    hide_tilde( buf );
    if ( ch->pcdata->homepage )
      DISPOSE(ch->pcdata->homepage);
    ch->pcdata->homepage = str_dup(buf);
    send_to_char( "Homepage set.\n\r", ch );
}



/*
 * Set your personal description				-Thoric
 */
void do_description( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Monsters are too dumb to do that!\n\r", ch );
	return;	  
    }

    if ( !ch->desc )
    {
	bug( "do_description: no descriptor", 0 );
	return;
    }

    switch( ch->substate )
    {
	default:
	   bug( "do_description: illegal substate", 0 );
	   return;

	case SUB_RESTRICTED:
	   send_to_char( "You cannot use this command from within another command.\n\r", ch );
	   return;

	case SUB_NONE:
	   ch->substate = SUB_PERSONAL_DESC;
	   ch->dest_buf = ch;
	   start_editing( ch, ch->description );
	   return;

	case SUB_PERSONAL_DESC:
	   STRFREE( ch->description );
	   ch->description = copy_buffer( ch );
	   stop_editing( ch );
	   return;	
    }
}

/* Ripped off do_description for whois bio's -- Scryn*/
void do_bio( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Mobs cannot set a bio.\n\r", ch );
	return;	  
    }
    if ( ch->level < 5 )
    {
	set_char_color( AT_SCORE, ch );
	send_to_char( "You must be at least level five to write your bio...\n\r", ch );
	return;
    }
    if ( !ch->desc )
    {
	bug( "do_bio: no descriptor", 0 );
	return;
    }

    switch( ch->substate )
    {
	default:
	   bug( "do_bio: illegal substate", 0 );
	   return;
	  	   
	case SUB_RESTRICTED:
	   send_to_char( "You cannot use this command from within another command.\n\r", ch );
	   return;

	case SUB_NONE:
	   ch->substate = SUB_PERSONAL_BIO;
	   ch->dest_buf = ch;
	   start_editing( ch, ch->pcdata->bio );
	   return;

	case SUB_PERSONAL_BIO:
	   STRFREE( ch->pcdata->bio );
	   ch->pcdata->bio = copy_buffer( ch );
	   stop_editing( ch );
	   return;	
    }
}



/*
 * New stat and statreport command coded by Morphina
 * Bug fixes by Shaddai
 */

void do_statreport( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
    {
	send_to_char("Huh?\n\r", ch );
	return;
    }

    if ( IS_BLOODSUCKER(ch) )
    {
      ch_printf( ch, "You report: %d/%d hp %d/%d blood %d/%d mv %d xp.\n\r",
 		ch->hit,  ch->max_hit, ch->pcdata->condition[COND_BLOODTHIRST],
		 GetMaxBlood( ch ), ch->move, ch->max_move, ch->exp   );
      sprintf( buf, "$n reports: %d/%d hp %d/%d blood %d/%d mv %d xp.",
 		ch->hit,  ch->max_hit, ch->pcdata->condition[COND_BLOODTHIRST],
		 GetMaxBlood( ch ), ch->move, ch->max_move, ch->exp   );
      act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
    }
    else
    {
      ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d xp.\n\r",
 		ch->hit,  ch->max_hit, ch->mana, ch->max_mana,
 		ch->move, ch->max_move, ch->exp   );
      sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d/%d mv %d xp.",
 		ch->hit,  ch->max_hit, ch->mana, ch->max_mana,
 		ch->move, ch->max_move, ch->exp   );
      act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
    }

    ch_printf( ch, "Your base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
      		ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, 
		ch->perm_con, ch->perm_cha, ch->perm_lck );
    sprintf( buf, "$n's base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.",
      		ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, 
		ch->perm_con, ch->perm_cha, ch->perm_lck );
    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );

    ch_printf( ch, "Your current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
		get_curr_str(ch), get_curr_wis(ch), get_curr_int(ch),
		get_curr_dex(ch), get_curr_con(ch), get_curr_cha(ch),
		get_curr_lck(ch) );
    sprintf( buf, "$n's current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.",
		get_curr_str(ch), get_curr_wis(ch), get_curr_int(ch),
		get_curr_dex(ch), get_curr_con(ch), get_curr_cha(ch),
		get_curr_lck(ch) );
    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );
    return;
}

void do_stat( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
    {
	send_to_char("Huh?\n\r", ch );
	return;
    }

    if ( IS_BLOODSUCKER(ch) )
      ch_printf( ch, "You report: %d/%d hp %d/%d blood %d/%d mv %d xp.\n\r",
 		ch->hit,  ch->max_hit, ch->pcdata->condition[COND_BLOODTHIRST],
		 GetMaxBlood( ch ), ch->move, ch->max_move, ch->exp   );
    else
      ch_printf( ch, "You report: %d/%d hp %d/%d mana %d/%d mv %d xp.\n\r",
 		ch->hit,  ch->max_hit, ch->mana, ch->max_mana,
 		ch->move, ch->max_move, ch->exp   );

      ch_printf( ch, "Your base stats:    %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
      		ch->perm_str, ch->perm_wis, ch->perm_int, ch->perm_dex, 
		ch->perm_con, ch->perm_cha, ch->perm_lck );

      ch_printf( ch, "Your current stats: %-2d str %-2d wis %-2d int %-2d dex %-2d con %-2d cha %-2d lck.\n\r",
		get_curr_str(ch), get_curr_wis(ch), get_curr_int(ch),
		get_curr_dex(ch), get_curr_con(ch), get_curr_cha(ch),
		get_curr_lck(ch) );
    return;
}


void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) && ch->fighting )
	return;

    if ( IS_AFFECTED(ch, AFF_POSSESS) )
    {   
       send_to_char("You can't do that in your current state of mind!\n\r", ch);
       return;
    }

    
    if ( IS_BLOODSUCKER(ch) )
      ch_printf( ch,
	"You report: %d/%d hp %d/%d blood %d/%d mv %d xp.\n\r",
	ch->hit,  ch->max_hit,
	ch->pcdata->condition[COND_BLOODTHIRST], GetMaxBlood( ch ),
	ch->move, ch->max_move,
	ch->exp   );
    else
      ch_printf( ch,
	"You report: %d/%d hp %d/%d mana %d/%d mv %d xp.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    if ( IS_BLOODSUCKER(ch) )
      sprintf( buf, "$n reports: %d/%d hp %d/%d blood %d/%d mv %d xp.\n\r",
	ch->hit,  ch->max_hit,
	ch->pcdata->condition[COND_BLOODTHIRST], GetMaxBlood( ch ),
	ch->move, ch->max_move,
	ch->exp   );
    else
      sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d/%d mv %d xp.",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    act( AT_REPORT, buf, ch, NULL, NULL, TO_ROOM );

    return;
}

void do_fprompt( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  
  set_char_color( AT_GREY, ch );

  if ( IS_NPC(ch) )
  {
    send_to_char( "NPC's can't change their prompt..\n\r", ch );
    return;
  }
  smash_tilde( argument );
  one_argument( argument, arg );
  if ( !*arg || !str_cmp( arg, "display" ) )
  {
    send_to_char( "Your current fighting prompt string:\n\r", ch );
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->fprompt, "" ) ? "(default prompt)"
				 				: ch->pcdata->fprompt );
    set_char_color( AT_GREY, ch );
    send_to_char( "Type 'help prompt' for information on changing your prompt.\n\r", ch );
    return;
  }
  send_to_char( "Replacing old prompt of:\n\r", ch );
  set_char_color( AT_WHITE, ch );
  ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->fprompt, "" ) ? "(default prompt)"
							      : ch->pcdata->fprompt );
  if (ch->pcdata->fprompt)
    STRFREE(ch->pcdata->fprompt);
  if ( strlen(argument) > 128 )
    argument[128] = '\0';

  /* Can add a list of pre-set prompts here if wanted.. perhaps
     'prompt 1' brings up a different, pre-set prompt */
  if ( !str_cmp(arg, "default") )
    ch->pcdata->fprompt = STRALLOC("");
  else
    ch->pcdata->fprompt = STRALLOC(argument);
  return;
}

void do_prompt( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  
  set_char_color( AT_GREY, ch );

  if ( IS_NPC(ch) )
  {
    send_to_char( "NPC's can't change their prompt..\n\r", ch );
    return;
  }
  smash_tilde( argument );
  one_argument( argument, arg );
  if ( !*arg || !str_cmp( arg, "display" ) )
  {
    send_to_char( "Your current prompt string:\n\r", ch );
    set_char_color( AT_WHITE, ch );
    ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->prompt, "" ) ? "(default prompt)"
				 				: ch->pcdata->prompt );
    set_char_color( AT_GREY, ch );
    send_to_char( "Type 'help prompt' for information on changing your prompt.\n\r", ch );
    return;
  }
  send_to_char( "Replacing old prompt of:\n\r", ch );
  set_char_color( AT_WHITE, ch );
  ch_printf( ch, "%s\n\r", !str_cmp( ch->pcdata->prompt, "" ) ? "(default prompt)"
							      : ch->pcdata->prompt );
  if (ch->pcdata->prompt)
    STRFREE(ch->pcdata->prompt);
  if ( strlen(argument) > 128 )
    argument[128] = '\0';

  /* Can add a list of pre-set prompts here if wanted.. perhaps
     'prompt 1' brings up a different, pre-set prompt */
  if ( !str_cmp(arg, "default") )
    ch->pcdata->prompt = STRALLOC("");
  else
    ch->pcdata->prompt = STRALLOC(argument);
  return;
}

/*
 * Get the maximum blood of a player. If mobile, return 0. -Orion
 */
int GetMaxBlood (CHAR_DATA *ch)
{
    int blood = 0;

    if ( !IS_NPC( ch ) && IS_BLOODSUCKER( ch ) )
    {
	blood = (ch->level + 10) + ch->pcdata->apply_blood;
    }

    return blood;
}
