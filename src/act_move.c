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
 * - Player movement module                                                *
 ***************************************************************************/

#include <ctype.h>
#include <string.h>
#include "mud.h"



const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6, 5, 7, 4
};

char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down",
    "northeast", "northwest", "southeast", "southwest", "somewhere"
};

const	int	trap_door	[]		=
{
    TRAP_N, TRAP_E, TRAP_S, TRAP_W, TRAP_U, TRAP_D,
    TRAP_NE, TRAP_NW, TRAP_SE, TRAP_SW
};


const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4, 9, 8, 7, 6, 10
};


/*
 * Local functions.
 */
OBJ_DATA	*has_key	args( ( CHAR_DATA *ch, int key ) );


char *	const		sect_names[SECT_MAX][2] =
{
    { "In a room","inside"	},	{ "In a city",	"cities"	},
    { "In a field","fields"	},	{ "In a forest","forests"	},
    { "hill",	"hills"		},	{ "On a mountain","mountains"	},
    { "In the water","waters"	},	{ "In rough water","waters"	},
    { "Underwater", "underwaters" },	{ "In the air",	"air"		},
    { "In a desert","deserts"	},	{ "Somewhere",	"unknown"	},
    { "ocean floor", "ocean floor" },	{ "underground", "underground"	}
};


const	int		sent_total[SECT_MAX]		=
{
    3, 5, 4, 4, 1, 1, 1, 1, 1, 2, 2, 25, 1, 1
};

char *	const		room_sents[SECT_MAX][25]	=
{
    {
	"rough hewn walls of granite with the occasional spider crawling around",
	"signs of a recent battle from the bloodstains on the floor",
	"a damp musty odour not unlike rotting vegetation"
    },

    {
	"the occasional stray digging through some garbage",
	"merchants trying to lure customers to their tents",
	"some street people putting on an interesting display of talent",
	"an argument between a customer and a merchant about the price of an item",
	"several shady figures talking down a dark alleyway"
    },

    {
	"sparce patches of brush and shrubs",
	"a small cluster of trees far off in the distance",
	"grassy fields as far as the eye can see",
	"a wide variety of weeds and wildflowers"
    },

    {
	"tall, dark evergreens prevent you from seeing very far",
	"many huge oak trees that look several hundred years old",
	"a solitary lonely weeping willow",
	"a patch of bright white birch trees slender and tall"
    },

    {
	"rolling hills lightly speckled with violet wildflowers"
    },

    {
	"the rocky mountain pass offers many hiding places"
    },

    {
	"the water is smooth as glass"
    },

    {
	"rough waves splash about angrily"
    },

    {
	"a small school of fish"
    },

    {
	"the land far below",
	"a misty haze of clouds"
    },

    {
	"sand as far as the eye can see",
	"an oasis far in the distance"
    },

    {
	"nothing unusual",	"nothing unusual",	"nothing unusual",
	"nothing unusual",	"nothing unusual",	"nothing unusual",
	"nothing unusual",	"nothing unusual",	"nothing unusual",
	"nothing unusual",	"nothing unusual",	"nothing unusual",
	"nothing unusual",	"nothing unusual",	"nothing unusual",
	"nothing unusual",	"nothing unusual",	"nothing unusual",
	"nothing unusual",	"nothing unusual",	"nothing unusual",
	"nothing unusual",	"nothing unusual",	"nothing unusual",
	"nothing unusual",
    },

    { 
        "rocks and coral which litter the ocean floor."
    },

    {
	"a lengthy tunnel of rock."
    }

};

char *grab_word( char *argument, char *arg_first )
{
    char cEnd;
    sh_int count;

    count = 0;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' || ++count >= 255 )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first++ = *argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

char *wordwrap( char *txt, sh_int wrap )
{
    static char buf[MAX_STRING_LENGTH];
    char *bufp;

    buf[0] = '\0';
    bufp = buf;
    if ( txt != NULL )
    {
        char line[MAX_STRING_LENGTH];
        char temp[MAX_STRING_LENGTH];
        char *ptr, *p;
        int ln, x;

	++bufp;
        line[0] = '\0';
        ptr = txt;
        while ( *ptr )
        {
	  ptr = grab_word( ptr, temp );
          ln = strlen( line );  x = strlen( temp );
          if ( (ln + x + 1) < wrap )
          {
	    if ( ln>0 && line[ln-1] == '.' ) 
              strcat( line, "  " );
	    else
              strcat( line, " " );
            strcat( line, temp );
            p = strchr( line, '\n' );
            if ( !p )
              p = strchr( line, '\r' );
            if ( p )
            {
                strcat( buf, line );
                line[0] = '\0';
            }
          }
          else
          {
            strcat( line, "\r\n" );
            strcat( buf, line );
            strcpy( line, temp );
          }
        }
        if ( line[0] != '\0' )
            strcat( buf, line );
    }
    return bufp;
}

char *rev_exit( sh_int vdir )
{
    switch( vdir )
    {
	default: return "somewhere";
	case 0:  return "the south";
	case 1:  return "the west";
	case 2:  return "the north";
	case 3:  return "the east";
	case 4:  return "below";
	case 5:  return "above";
	case 6:  return "the southwest";
	case 7:  return "the southeast";
	case 8:  return "the northwest";
	case 9:  return "the northeast";
    }

    return "<??\?>";
}

/*
 * Function to get the equivelant exit of DIR 0-MAXDIR out of linked list.
 * Made to allow old-style diku-merc exit functions to work.	-Thoric
 */
EXIT_DATA *get_exit( ROOM_INDEX_DATA *room, sh_int dir )
{
    EXIT_DATA *xit;

    if ( !room )
    {
	bug( "Get_exit: NULL room", 0 );
	return NULL;
    }

    for (xit = room->first_exit; xit; xit = xit->next )
       if ( xit->vdir == dir )
         return xit;
    return NULL;
}

/*
 * Function to get an exit, leading the the specified room
 */
EXIT_DATA *get_exit_to( ROOM_INDEX_DATA *room, sh_int dir, int vnum )
{
    EXIT_DATA *xit;

    if ( !room )
    {
	bug( "Get_exit: NULL room", 0 );
	return NULL;
    }

    for (xit = room->first_exit; xit; xit = xit->next )
       if ( xit->vdir == dir && xit->vnum == vnum )
         return xit;
    return NULL;
}

/*
 * Function to get the nth exit of a room			-Thoric
 */
EXIT_DATA *get_exit_num( ROOM_INDEX_DATA *room, sh_int count )
{
    EXIT_DATA *xit;
    int cnt;

    if ( !room )
    {
	bug( "Get_exit: NULL room", 0 );
	return NULL;
    }

    for (cnt = 0, xit = room->first_exit; xit; xit = xit->next )
       if ( ++cnt == count )
         return xit;
    return NULL;
}


/*
 * Modify movement due to encumbrance				-Thoric
 */
sh_int encumbrance( CHAR_DATA *ch, sh_int move )
{
    int cur, max;

    max = can_carry_w(ch);
    cur = ch->carry_weight;
    if ( cur >= max )
      return move * 4;
    else
    if ( cur >= max * 0.95 )
      return move * 3.5;
    else
    if ( cur >= max * 0.90 )
      return move * 3;
    else
    if ( cur >= max * 0.85 )
      return move * 2.5;
    else
    if ( cur >= max * 0.80 )
      return move * 2;
    else
    if ( cur >= max * 0.75 )
      return move * 1.5;
    else
      return move;
}


/*
 * Check to see if a character can fall down, checks for looping   -Thoric
 */
bool will_fall( CHAR_DATA *ch, int fall )
{
    if ( IS_SET( ch->in_room->room_flags, ROOM_NOFLOOR )
    &&   CAN_GO(ch, DIR_DOWN)
    && (!IS_AFFECTED( ch, AFF_FLYING )
    || ( ch->mount && !IS_AFFECTED( ch->mount, AFF_FLYING ) ) ) )
    {
	if ( fall > 80 )
	{
	   bug( "Falling (in a loop?) more than 80 rooms: vnum %d", ch->in_room->vnum );
	   char_from_room( ch );
	   char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	   fall = 0;
	   return TRUE;
	}
	set_char_color( AT_FALLING, ch );
	send_to_char( "You're falling down...\n\r", ch );
	move_char( ch, get_exit(ch->in_room, DIR_DOWN), ++fall );
	return TRUE;
    }
    return FALSE;
}

ch_ret move_char( CHAR_DATA *ch, EXIT_DATA *pexit, int fall )
{
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    OBJ_DATA *boat;
    char buf[MAX_STRING_LENGTH];
    char *txt;
    char *dtxt;
    ch_ret retcode;
    sh_int door;
    bool drunk = FALSE;
    bool nuisance = FALSE;
    bool brief = FALSE;

    if ( !IS_NPC( ch ) )
    {
	if ( IS_DRUNK( ch, 2 ) && ch->position != POS_SHOVE && ch->position != POS_DRAG )
	    drunk = TRUE;

	/*
	 * Nuisance flag, makes them walk in random directions 50% of the time. -Shaddai
	 */
	if ( ch->pcdata->nuisance && ch->pcdata->nuisance->flags > 8 &&
	     ch->position != POS_SHOVE && ch->position != POS_DRAG && 
	     number_percent() > ( ch->pcdata->nuisance->flags*ch->pcdata->nuisance->power ) )
	     nuisance = TRUE;
    }

    if ( !fall && ( nuisance || drunk ) )
    {
	door = number_door();
	pexit = get_exit( ch->in_room, door );
    }

    retcode = rNONE;
    txt = NULL;

    if ( IS_NPC( ch ) && xIS_SET( ch->act, ACT_MOUNTED ) )
	return retcode;

    in_room = ch->in_room;
    from_room = in_room;

    if ( !pexit || ( to_room = pexit->to_room ) == NULL )
    {
	if ( drunk && ch->position != POS_MOUNTED &&
	     ch->in_room->sector_type != SECT_WATER_SWIM &&
	     ch->in_room->sector_type != SECT_WATER_NOSWIM &&
	     ch->in_room->sector_type != SECT_UNDERWATER &&
	     ch->in_room->sector_type != SECT_OCEANFLOOR )
	{
	    switch ( number_bits( 4 ) )
	    {
		default:
		    act( AT_ACTION, "You drunkenly stumble into some obstacle.", ch, NULL, NULL, TO_CHAR );
		    act( AT_ACTION, "$n drunkenly stumbles into a nearby obstacle.", ch, NULL, NULL, TO_ROOM );
		    break;
		case 3:
		    act( AT_ACTION, "In your drunken stupor you trip over your own feet and tumble to the ground.", ch, NULL, NULL, TO_CHAR );
		    act( AT_ACTION, "$n stumbles drunkenly, trips and tumbles to the ground.", ch, NULL, NULL, TO_ROOM );
		    ch->position = POS_RESTING;
		    break;
		case 4:
		    act( AT_SOCIAL, "You utter a string of slurred obscenities.", ch, NULL, NULL, TO_CHAR) ;
		    act( AT_ACTION, "Something blurry and immovable has intercepted you as you stagger along.", ch, NULL, NULL, TO_CHAR );
		    act( AT_HURT, "Oh geez... THAT really hurt.  Everything slowly goes dark and numb...", ch, NULL, NULL, TO_CHAR );
		    act( AT_ACTION, "$n drunkenly staggers into something.", ch, NULL, NULL, TO_ROOM );
		    act( AT_SOCIAL, "$n utters a string of slurred obscenities: @*&^%@*&!", ch, NULL, NULL, TO_ROOM );
		    act( AT_ACTION, "$n topples to the ground with a thud.", ch, NULL, NULL, TO_ROOM );
		    ch->position = POS_INCAP;
		    break;
	    }
	}
	else if ( nuisance )
	    act( AT_ACTION, "You stare around trying to remember where you where going.", ch, NULL, NULL, TO_CHAR );
	else if ( drunk )
	    act( AT_ACTION, "You stare around trying to make sense of things through your drunken stupor.", ch, NULL, NULL, TO_CHAR );
	else
	    send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return rNONE;
    }

    door = pexit->vdir;

    /*
     * Exit is only a "window", there is no way to travel in that direction
     * unless it's a door with a window in it -Thoric
     */
    if ( IS_SET( pexit->exit_info, EX_WINDOW ) && !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return rNONE;
    }

    if ( IS_SET( pexit->exit_info, EX_PORTAL ) && IS_NPC( ch ) )
    {
	act( AT_PLAIN, "Mobs can't use portals.", ch, NULL, NULL, TO_CHAR );
	return rNONE;
    }

    if ( IS_SET( pexit->exit_info, EX_NOMOB ) && IS_NPC( ch ) )
    {
	act( AT_PLAIN, "Mobs can't enter there.", ch, NULL, NULL, TO_CHAR );
	return rNONE;
    }

    if ( IS_SET( pexit->exit_info, EX_CLOSED ) && (!IS_AFFECTED( ch, AFF_PASS_DOOR ) ||
	 IS_SET( pexit->exit_info, EX_NOPASSDOOR )) )
    {
	if ( !IS_SET( pexit->exit_info, EX_SECRET ) && !IS_SET( pexit->exit_info, EX_DIG ) )
	{
	    if ( drunk )
	    {
		act( AT_PLAIN, "$n runs into the $d in $s drunken state.", ch, NULL, pexit->keyword, TO_ROOM );
		act( AT_PLAIN, "You run into the $d in your drunken state.", ch, NULL, pexit->keyword, TO_CHAR );
	    }
	    else
		act( AT_PLAIN, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else
	{
	    if ( drunk )
		send_to_char( "You stagger around in your drunken state.\n\r", ch );
	    else
		send_to_char( "Alas, you cannot go that way.\n\r", ch );
	}

	return rNONE;
    }

    if ( !fall && IS_AFFECTED( ch, AFF_CHARM ) && ch->master && in_room == ch->master->in_room )
    {
	send_to_char( "What... and leave your beloved master?\n\r", ch );
	return rNONE;
    }

    if ( room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return rNONE;
    }

    if ( room_is_dnd(ch, to_room) )
    {
	send_to_char( "That room is \"do not disturb\" right now.\n\r", ch );
	return rNONE;
    }

    if ( !IS_IMMORTAL( ch ) && !IS_NPC( ch ) && ch->in_room->area != to_room->area )
    {
	if ( ch->level < to_room->area->low_hard_range )
	{
	    set_char_color( AT_TELL, ch );
	    switch( to_room->area->low_hard_range - ch->level )
	    {
		case 1:
		    send_to_char( "A voice in your mind says, 'You are nearly ready to go that way...'", ch );
		    break;
		case 2:
		    send_to_char( "A voice in your mind says, 'Soon you shall be ready to travel down this path... soon.'", ch );
		    break;
		case 3:
		    send_to_char( "A voice in your mind says, 'You are not ready to go down that path... yet.'.\n\r", ch);
		    break;
		default:
		    send_to_char( "A voice in your mind says, 'You are not ready to go down that path.'.\n\r", ch);
	    }
	    return rNONE;
	}
	else if ( ch->level > to_room->area->hi_hard_range )
	{
	    set_char_color( AT_TELL, ch );
	    send_to_char( "A voice in your mind says, 'There is nothing more for you down that path.'", ch );
	    return rNONE;
	}          
    }

    if ( !fall && !IS_NPC(ch) )
    {
	int move;

	/*
	 * Prevent deadlies from entering an antipkill-flagged area from a non-flagged
	 * area, but allow them to move around if already inside one. - Blodkai
	 */
        if ( IS_SET( to_room->area->flags, AFLAG_ANTIPKILL ) &&
	    !IS_SET( ch->in_room->area->flags, AFLAG_ANTIPKILL ) &&
	     (IS_PKILL(ch) && !IS_IMMORTAL (ch)) )
	{
	    set_char_color( AT_MAGIC, ch );
	    send_to_char( "\n\rA godly force forbids deadly characters from entering that area...\n\r", ch);
	    return rNONE;
        }

	if ( in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR ||
	     IS_SET( pexit->exit_info, EX_FLY ) )
	{
	    if ( ch->mount && !IS_AFFECTED( ch->mount, AFF_FLYING ) )
	    {
		send_to_char( "Your mount can't fly.\n\r", ch );
		return rNONE;
	    }
	    if ( !ch->mount && !IS_AFFECTED( ch, AFF_FLYING ) )
	    {
		send_to_char( "You'd need to fly to go there.\n\r", ch );
		return rNONE;
	    }
	}

	if ( in_room->sector_type == SECT_WATER_NOSWIM ||
	     to_room->sector_type == SECT_WATER_NOSWIM )
	{
	    if ( (ch->mount && !IS_FLOATING(ch->mount)) || !IS_FLOATING(ch) )
	    {
		/*
		 * Look for a boat.
		 * We can use the boat obj for a more detailed description.
		 */
		if ( (boat=get_objtype(ch, ITEM_BOAT)) != NULL )
		{
		    if ( drunk )
			txt = "paddles unevenly";
		    else
			txt = "paddles";
		}
		else
		{
		    if ( ch->mount )
			send_to_char( "Your mount would drown!\n\r", ch );
		    else
			send_to_char( "You'd need a boat to go there.\n\r", ch );
		    return rNONE;
		}
	    }
	}

	if ( IS_SET( pexit->exit_info, EX_CLIMB ) )
	{
	    bool found;

	    found = FALSE;
	    if ( ch->mount && IS_AFFECTED( ch->mount, AFF_FLYING ) )
		found = TRUE;
	    else if ( IS_AFFECTED( ch, AFF_FLYING ) )
		found = TRUE;

	    if ( !found && !ch->mount )
	    {
		if ( (!IS_NPC( ch ) && number_percent( ) > LEARNED( ch, gsn_climb )) ||
		     drunk || ch->mental_state < -90 )
		{
		    send_to_char( "You start to climb... but lose your grip and fall!\n\r", ch);
		    learn_from_failure( ch, gsn_climb );
		    if ( pexit->vdir == DIR_DOWN )
		    {
			retcode = move_char( ch, pexit, 1 );
			return retcode;
		    }
		    set_char_color( AT_HURT, ch );
		    send_to_char( "OUCH! You hit the ground!\n\r", ch );
		    WAIT_STATE( ch, 20 );
		    retcode = damage( ch, ch, (pexit->vdir == DIR_UP ? 10 : 5), TYPE_UNDEFINED );
		    return retcode;
		}
		found = TRUE;
		learn_from_success( ch, gsn_climb );
		WAIT_STATE( ch, skill_table[gsn_climb]->beats );
		txt = "climbs";
	    }

	    if ( !found )
	    {
		send_to_char( "You can't climb.\n\r", ch );
		return rNONE;
	    }
	}

	if ( ch->mount )
	{
	    bool retVal = FALSE;

	    switch (ch->mount->position)
	    {
		case POS_DEAD:
		    send_to_char( "Your mount is dead!\n\r", ch );
		    retVal = TRUE;
		    break;
		case POS_MORTAL:
		case POS_INCAP:
		    send_to_char( "Your mount is hurt far too badly to move.\n\r", ch );
		    retVal = TRUE;
		    break;
		case POS_STUNNED:
		    send_to_char( "Your mount is too stunned to do that.\n\r", ch );
		    retVal = TRUE;
		    break;
		case POS_SLEEPING:
		    send_to_char( "Your mount is sleeping.\n\r", ch );
		    retVal = TRUE;
		    break;
		case POS_RESTING:
		    send_to_char( "Your mount is resting.\n\r", ch);
		    retVal = TRUE;
		    break;
		case POS_SITTING:
		    send_to_char( "Your mount is sitting down.\n\r", ch);
		    retVal = TRUE;
		    break;
		default:
		    break;
	    }

	    if ( retVal == TRUE )
		return rNONE;

	    if ( !IS_FLOATING( ch->mount ) )
		move = movement_loss[UMIN( SECT_MAX-1, in_room->sector_type )];
	    else
		move = 1;
	    if ( ch->mount->move < move )
	    {
		send_to_char( "Your mount is too exhausted.\n\r", ch );
		return rNONE;
	    }
	}
	else
	{
	    if ( !IS_FLOATING( ch ) )
		move = encumbrance( ch, movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)] );
	    else
		move = 1;

	    if ( ch->move < move )
	    {
		send_to_char( "You are too exhausted.\n\r", ch );
		return rNONE;
	    }
	}

	WAIT_STATE( ch, move );
	if ( ch->mount )
	    ch->mount->move -= move;
	else
	    ch->move -= move;
    }

    /*
     * Check if player can fit in the room
     */
    if ( to_room->tunnel > 0 )
    {
	CHAR_DATA *ctmp;
	int count = ch->mount ? 1 : 0;
	
	for ( ctmp = to_room->first_person; ctmp; ctmp = ctmp->next_in_room )
	{
	    if ( ++count >= to_room->tunnel )
	    {
		if ( ch->mount && count == to_room->tunnel )
		    send_to_char( "There is no room for both you and your mount there.\n\r", ch );
		else
		    send_to_char( "There is no room for you there.\n\r", ch );
		return rNONE;
	    }
	}
    }

    /*
     * Check for traps on exit - later
     */
    if ( IS_NPC( ch ) || !xIS_SET( ch->act, PLR_WIZINVIS ) )
    {
	if ( fall )
	    txt = "falls";
	else if ( !txt )
	{
	    if ( ch->mount )
	    {
		if ( IS_AFFECTED( ch->mount, AFF_FLOATING ) )
		    txt = "floats";
		else if ( IS_AFFECTED( ch->mount, AFF_FLYING ) )
		    txt = "flies";
		else
		    txt = "rides";
	    }
	    else
	    {
		if ( IS_AFFECTED( ch, AFF_FLOATING ) )
		{
		    if ( drunk )
			txt = "floats unsteadily";
		    else
			txt = "floats";
		}
		else if ( IS_AFFECTED( ch, AFF_FLYING ) )
		{
		    if ( drunk )
			txt = "flies shakily";
		    else
			txt = "flies";
		}
		else if ( ch->position == POS_SHOVE )
		    txt = "is shoved";
		else if ( ch->position == POS_DRAG )
		    txt = "is dragged";
		else
		{
		    if ( drunk )
			txt = "stumbles drunkenly";
		    else
			txt = "leaves";
		}
	    }
	}

	/*
	 * Print some messages about leaving the room. -Orion
	 */
	if ( !IS_AFFECTED( ch, AFF_SNEAK ) )
	{
	    if ( ch->mount )
	    {
		sprintf( buf, "$n %s %s upon $N.", txt, dir_name[door] );
		act( AT_ACTION, buf, ch, NULL, ch->mount, TO_NOTVICT );
	    }
	    else
	    {
		sprintf( buf, "$n %s $T.", txt );
		act( AT_ACTION, buf, ch, NULL, dir_name[door], TO_ROOM );
	    }
	}
	else
	{
	    CHAR_DATA *temp_char, *temp_next;
	    char sneak_buf1[MAX_STRING_LENGTH], sneak_buf2[MAX_STRING_LENGTH];
	    bool hasMount = ch->mount ? TRUE : FALSE;

	    if ( hasMount )
	    {
		sprintf( sneak_buf1, "$n attempts to sneak away upon %s.%c", IS_NPC(ch->mount) ? ch->mount->short_descr : ch->mount->name, '\0' );
		sprintf( sneak_buf2, "$n %s %s upon %s.%c", txt, dir_name[door], IS_NPC(ch->mount) ? ch->short_descr : ch->name, '\0' );
	    }
	    else
	    {
		sprintf( sneak_buf1, "$n attempts to sneak away.%c", '\0' );
		sprintf( sneak_buf2, "$n %s %s.%c", txt, dir_name[door], '\0' );
	    }

	    for ( temp_char = ch->in_room->first_person; temp_char; temp_char = temp_next )
	    {
		temp_next = temp_char->next_in_room;

		if ( IS_AFFECTED( temp_char, AFF_DETECT_SNEAK ) )
		{
		    act( AT_ACTION, sneak_buf1, ch, NULL, temp_char, TO_VICT );
		    act( AT_ACTION, sneak_buf2, ch, NULL, temp_char, TO_VICT );
		}
	    }
	}
    }

    rprog_leave_trigger( ch );

    if( char_died(ch) )
	return global_retcode;

    char_from_room( ch );
    char_to_room( ch, to_room );
    if ( ch->mount )
    {
	rprog_leave_trigger( ch->mount );

	/*
	 * Mount bug fix test. -Orion
	 */
	if( char_died( ch->mount ) )
	    return global_retcode;

	if( ch->mount )
	{
	    char_from_room( ch->mount );
	    char_to_room( ch->mount, to_room );
	}
    }

    if ( IS_NPC( ch ) || !xIS_SET( ch->act, PLR_WIZINVIS ) )
    {
	if ( fall )
	    txt = "falls";
	else if ( ch->mount )
	{
	    if ( IS_AFFECTED( ch->mount, AFF_FLOATING ) )
		txt = "floats in";
	    else if ( IS_AFFECTED( ch->mount, AFF_FLYING ) )
		txt = "flies in";
	    else
		txt = "rides in";
	}
	else
	{
	    if ( IS_AFFECTED( ch, AFF_FLOATING ) )
	    {
		if ( drunk )
		    txt = "floats in unsteadily";
		else
		    txt = "floats in";
	    }
	    else if ( IS_AFFECTED( ch, AFF_FLYING ) )
	    {
		if ( drunk )
		    txt = "flies in shakily";
		else
		    txt = "flies in";
	    }
	    else if ( ch->position == POS_SHOVE )
		txt = "is shoved in";
	    else if ( ch->position == POS_DRAG )
		txt = "is dragged in";
	    else
	    {
		if ( drunk )
		    txt = "stumbles drunkenly in";
		else
		    txt = "arrives";
	    }
	}

	dtxt = rev_exit(door);

	/*
	 * Print some entering messages. -Orion
	 */
	if ( !IS_AFFECTED( ch, AFF_SNEAK ) )
	{
	    if ( ch->mount )
	    {
		sprintf( buf, "$n %s from %s upon $N.", txt, dtxt );
		act( AT_ACTION, buf, ch, NULL, ch->mount, TO_ROOM );
	    }
	    else
	    {
		sprintf( buf, "$n %s from %s.", txt, dtxt );
		act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
	    }
	}
	else
	{
	    CHAR_DATA *temp_char, *temp_next;
	    char sneak_buf1[MAX_STRING_LENGTH], sneak_buf2[MAX_STRING_LENGTH];
	    bool hasMount = ch->mount ? TRUE : FALSE;

	    if ( hasMount )
	    {
		sprintf( sneak_buf1, "$n attempts to sneak closer upon %s.%c", IS_NPC(ch->mount) ? ch->mount->short_descr : ch->mount->name, '\0' );
		sprintf( sneak_buf2, "$n %s from %s upon %s.%c", txt, dtxt, IS_NPC(ch->mount) ? ch->short_descr : ch->name, '\0' );
	    }
	    else
	    {
		sprintf( sneak_buf1, "$n attempts to sneak closer.%c", '\0' );
		sprintf( sneak_buf2, "$n %s from %s.%c", txt, dtxt, '\0' );
	    }

	    for ( temp_char = ch->in_room->first_person; temp_char; temp_char = temp_next )
	    {
		temp_next = temp_char->next_in_room;

		if ( IS_AFFECTED( temp_char, AFF_DETECT_SNEAK ) )
		{
		    act( AT_ACTION, sneak_buf1, ch, NULL, temp_char, TO_VICT );
		    act( AT_ACTION, sneak_buf2, ch, NULL, temp_char, TO_VICT );
		}
	    }
	}
    }

    if ( !IS_IMMORTAL( ch ) && !IS_NPC( ch ) && ch->in_room->area != to_room->area )
    {
	set_char_color( AT_MAGIC, ch );
	if ( ch->level < to_room->area->low_soft_range )
	{
	    send_to_char( "You feel uncomfortable being in this strange land...\n\r", ch );
	}
	else if ( ch->level > to_room->area->hi_soft_range )
	{
	    send_to_char( "You feel there is not much to gain visiting this place...\n\r", ch );
	}
    }

    /*
     * Make sure everyone sees the room description of death traps. -Orion
     */
    if ( IS_SET( ch->in_room->room_flags, ROOM_DEATH ) && !IS_IMMORTAL( ch ) )
    {
	if ( xIS_SET( ch->act, PLR_BRIEF ) )
	    brief = TRUE;
	xREMOVE_BIT( ch->act, PLR_BRIEF );
    }

    do_look( ch, "auto" );

    if ( brief ) 
 	xSET_BIT( ch->act, PLR_BRIEF );

    /*
     * Put good-old EQ-munching death traps back in!		-Thoric
     */
    if ( IS_SET( ch->in_room->room_flags, ROOM_DEATH ) && !IS_IMMORTAL( ch ) )
    {
	act( AT_DEAD, "$n falls prey to a terrible death!", ch, NULL, NULL, TO_ROOM );
	set_char_color( AT_DEAD, ch );
	send_to_char( "Oopsie... you're dead!\n\r", ch );
	sprintf(buf, "%s hit a DEATH TRAP in room %d!", ch->name, ch->in_room->vnum );
	log_string( buf );
	to_channel( buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
	extract_char( ch, FALSE );

	return rCHAR_DIED;
    }

    /*
     * BIG ugly looping problem here when the character is mptransed back to the starting 
     * room. To avoid this, check how many chars are in the room at the start and stop
     * processing followers after doing the right number of them. -Narn
    */
    if ( !fall )
    {
	CHAR_DATA *fch;
	CHAR_DATA *nextinroom;
	int chars = 0, count = 0;

	for ( fch = from_room->first_person; fch; fch = fch->next_in_room )
	    chars++;

	for ( fch = from_room->first_person; fch && ( count < chars ); fch = nextinroom )
	{
	    nextinroom = fch->next_in_room;
	    count++;
	    if ( fch != ch && fch->master == ch && fch->position == POS_STANDING )
	    {
		act( AT_ACTION, "You follow $N.", fch, NULL, ch, TO_CHAR );
		move_char( fch, pexit, 0 );
	    }
	}
    }

    if ( ch->in_room->first_content )
	retcode = check_room_for_traps( ch, TRAP_ENTER_ROOM );

    if ( retcode != rNONE )
	return retcode;

    if ( char_died( ch ) )
	return retcode;

    /*
     * Really should check mounts in this chunk of code. -Orion
     */
    mprog_entry_trigger( ch );
    if ( char_died( ch ) )
	return retcode;

    rprog_enter_trigger( ch );
    if ( char_died( ch ) )
	return retcode;

    mprog_greet_trigger( ch );
    if ( char_died( ch ) )
	return retcode;

    oprog_greet_trigger( ch );
    if ( char_died( ch ) )
	return retcode;

    if ( !will_fall( ch, fall ) && fall > 0 )
    {
	if ( !IS_AFFECTED( ch, AFF_FLOATING ) || (ch->mount && !IS_AFFECTED( ch->mount, AFF_FLOATING )) )
	{
	    set_char_color( AT_HURT, ch );
	    send_to_char( "OUCH! You hit the ground!\n\r", ch );
	    WAIT_STATE( ch, 20 );
	    retcode = damage( ch, ch, 20 * fall, TYPE_UNDEFINED );
	}
	else
	{
	    set_char_color( AT_MAGIC, ch );
	    send_to_char( "You lightly float down to the ground.\n\r", ch );
	}
    }
    return retcode;
}


void do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_NORTH), 0 );
    return;
}


void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_EAST), 0 );
    return;
}


void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_SOUTH), 0 );
    return;
}


void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_WEST), 0 );
    return;
}


void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_UP), 0 );
    return;
}


void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_DOWN), 0 );
    return;
}

void do_northeast( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_NORTHEAST), 0 );
    return;
}

void do_northwest( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_NORTHWEST), 0 );
    return;
}

void do_southeast( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_SOUTHEAST), 0 );
    return;
}

void do_southwest( CHAR_DATA *ch, char *argument )
{
    move_char( ch, get_exit(ch->in_room, DIR_SOUTHWEST), 0 );
    return;
}



EXIT_DATA *find_door( CHAR_DATA *ch, char *arg, bool quiet )
{
    EXIT_DATA *pexit;
    int door;

    if (arg == NULL || !str_cmp(arg,""))
      return NULL;

    pexit = NULL;
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
    else
    {
	for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	{
	    if ( (quiet || IS_SET(pexit->exit_info, EX_ISDOOR))
	    &&    pexit->keyword
	    &&    nifty_is_name( arg, pexit->keyword ) )
		return pexit;
	}
	if ( !quiet )
	  act( AT_PLAIN, "You see no $T here.", ch, NULL, arg, TO_CHAR );
	return NULL;
    }

    if ( (pexit = get_exit( ch->in_room, door )) == NULL )
    {
	if ( !quiet)
	  act( AT_PLAIN, "You see no $T here.", ch, NULL, arg, TO_CHAR );
	return NULL;
    }

    if ( quiet )
	return pexit;

    if ( IS_SET(pexit->exit_info, EX_SECRET) )
    {
	act( AT_PLAIN, "You see no $T here.", ch, NULL, arg, TO_CHAR );
	return NULL;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return NULL;
    }

    return pexit;
}


void set_bexit_flag( EXIT_DATA *pexit, int flag )
{
    EXIT_DATA *pexit_rev;

    SET_BIT(pexit->exit_info, flag);
    if ( (pexit_rev = pexit->rexit) != NULL
    &&   pexit_rev != pexit )
	SET_BIT( pexit_rev->exit_info, flag );
}

void remove_bexit_flag( EXIT_DATA *pexit, int flag )
{
    EXIT_DATA *pexit_rev;

    REMOVE_BIT(pexit->exit_info, flag);
    if ( (pexit_rev = pexit->rexit) != NULL
    &&   pexit_rev != pexit )
	REMOVE_BIT( pexit_rev->exit_info, flag );
}

void toggle_bexit_flag( EXIT_DATA *pexit, int flag )
{
    EXIT_DATA *pexit_rev;

    TOGGLE_BIT(pexit->exit_info, flag);
    if ( (pexit_rev = pexit->rexit) != NULL
    &&   pexit_rev != pexit )
	TOGGLE_BIT( pexit_rev->exit_info, flag );
}

void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    EXIT_DATA *pexit;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
    {
	/* 'open door' */
	EXIT_DATA *pexit_rev;

        if (  IS_SET(pexit->exit_info, EX_SECRET)
        &&    pexit->keyword && !nifty_is_name( arg, pexit->keyword ) )
            { ch_printf( ch, "You see no %s here.\n\r", arg ); return;   }
	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED)
	     &&IS_SET(pexit->exit_info, EX_BOLTED))
	    { send_to_char( "The bolts locked.\n\r",	   ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_BOLTED) )
	    { send_to_char( "It's bolted.\n\r",		   ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	if ( !IS_SET(pexit->exit_info, EX_SECRET)
	||   (pexit->keyword && nifty_is_name( arg, pexit->keyword )) )
	{
	    act( AT_ACTION, "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	    act( AT_ACTION, "You open the $d.", ch, NULL, pexit->keyword, TO_CHAR );
	    if ( (pexit_rev = pexit->rexit) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
	    {
		CHAR_DATA *rch;

		for ( rch = pexit->to_room->first_person; rch; rch = rch->next_in_room )
		    act( AT_ACTION, "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	    remove_bexit_flag( pexit, EX_CLOSED );
	    if ( (door=pexit->vdir) >= 0 && door < 10 )
		check_room_for_traps( ch, trap_door[door]);
            return;
	}
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER )
	{ 
          ch_printf( ch, "%s is not a container.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	{ 
          ch_printf( ch, "%s is already open.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	{ 
          ch_printf( ch, "%s cannot be opened or closed.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	{ 
          ch_printf( ch, "%s is locked.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	act( AT_ACTION, "You open $p.", ch, obj, NULL, TO_CHAR );
	act( AT_ACTION, "$n opens $p.", ch, obj, NULL, TO_ROOM );
	check_for_trap( ch, obj, TRAP_OPEN );
	return;
    }

    ch_printf( ch, "You see no %s here.\n\r", arg );
    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    EXIT_DATA *pexit;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
    {
	/* 'close door' */
	EXIT_DATA *pexit_rev;

	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	act( AT_ACTION, "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	act( AT_ACTION, "You close the $d.", ch, NULL, pexit->keyword, TO_CHAR );

	/* close the other side */
	if ( ( pexit_rev = pexit->rexit ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = pexit->to_room->first_person; rch; rch = rch->next_in_room )
		act( AT_ACTION, "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
	set_bexit_flag( pexit, EX_CLOSED );
	if ( (door=pexit->vdir) >= 0 && door < 10 )
	  check_room_for_traps( ch, trap_door[door]);
        return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	{ 
          ch_printf( ch, "%s is not a container.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	{ 
          ch_printf( ch, "%s is already closed.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
        { 
          ch_printf( ch, "%s cannot be opened or closed.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 

	SET_BIT(obj->value[1], CONT_CLOSED);
	act( AT_ACTION, "You close $p.", ch, obj, NULL, TO_CHAR );
	act( AT_ACTION, "$n closes $p.", ch, obj, NULL, TO_ROOM );
	check_for_trap( ch, obj, TRAP_CLOSE );
	return;
    }

    ch_printf( ch, "You see no %s here.\n\r", arg );
    return;
}


/*
 * Keyring support added by Thoric
 * Idea suggested by Onyx <MtRicmer@worldnet.att.net> of Eldarion
 *
 * New: returns pointer to key/NULL instead of TRUE/FALSE
 *
 * If you want a feature like having immortals always have a key... you'll
 * need to code in a generic key, and make sure extract_obj doesn't extract it
 */
OBJ_DATA *has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj, *obj2;

    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key || (obj->item_type == ITEM_KEY && obj->value[0] == key) )
	    return obj;
	else
	if ( obj->item_type == ITEM_KEYRING )
	    for ( obj2 = obj->first_content; obj2; obj2 = obj2->next_content )
		if ( obj2->pIndexData->vnum == key || obj2->value[0] == key )
		    return obj2;
    }

    return NULL;
}


void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj, *key;
    EXIT_DATA *pexit;
    int count;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
    {
	/* 'lock door' */

	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( (key=has_key(ch, pexit->key)) == NULL )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	if ( !IS_SET(pexit->exit_info, EX_SECRET)
	||   (pexit->keyword && nifty_is_name( arg, pexit->keyword )) )
	{
	    send_to_char( "*Click*\n\r", ch );
	    count = key->count;
	    key->count = 1;
	    act( AT_ACTION, "$n locks the $d with $p.", ch, key, pexit->keyword, TO_ROOM );
	    key->count = count;
	    set_bexit_flag( pexit, EX_LOCKED );
            return;
        }
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( (key=has_key(ch, obj->value[2])) == NULL )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	count = key->count;
	key->count = 1;
	act( AT_ACTION, "$n locks $p with $P.", ch, obj, key, TO_ROOM );
	key->count = count;
	return;
    }

    ch_printf( ch, "You see no %s here.\n\r", arg );
    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj, *key;
    EXIT_DATA *pexit;
    int count;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
    {
	/* 'unlock door' */

	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( (key=has_key(ch, pexit->key)) == NULL )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	if ( !IS_SET(pexit->exit_info, EX_SECRET)
	||   (pexit->keyword && nifty_is_name( arg, pexit->keyword )) )
	{
	    send_to_char( "*Click*\n\r", ch );
	    count = key->count;
	    key->count = 1;
	    act( AT_ACTION, "$n unlocks the $d with $p.", ch, key, pexit->keyword, TO_ROOM );
	    key->count = count;
	    if ( IS_SET(pexit->exit_info, EX_EATKEY) )
	    {
		separate_obj(key);
		extract_obj(key);
	    }
	    remove_bexit_flag( pexit, EX_LOCKED );
            return;   
	}
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( (key=has_key(ch, obj->value[2])) == NULL )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	count = key->count;
	key->count = 1;
	act( AT_ACTION, "$n unlocks $p with $P.", ch, obj, key, TO_ROOM );
	key->count = count;
	if ( IS_SET(obj->value[1], CONT_EATKEY) )
	{
	    separate_obj(key);
	    extract_obj(key);
	}
	return;
    }

    ch_printf( ch, "You see no %s here.\n\r", arg );
    return;
}

void do_bashdoor( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *gch;
	EXIT_DATA *pexit;
	char       arg [ MAX_INPUT_LENGTH ];

	if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_bashdoor]->skill_level[ch->class] )
	{
	    send_to_char( "You're not enough of a warrior to bash doors!\n\r", ch );
	    return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
	    send_to_char( "Bash what?\n\r", ch );
	    return;
	}

	if ( ch->fighting )
	{
	    send_to_char( "You can't break off your fight.\n\r", ch );
	    return;
	}

	if ( ( pexit = find_door( ch, arg, FALSE ) ) != NULL )
	{
	    ROOM_INDEX_DATA *to_room;
	    EXIT_DATA       *pexit_rev;
	    int              chance;
	    char	    *keyword;

	    if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    {
		send_to_char( "Calm down.  It is already open.\n\r", ch );
		return;
	    }

	    WAIT_STATE( ch, skill_table[gsn_bashdoor]->beats );

	    if ( IS_SET( pexit->exit_info, EX_SECRET ) )
		keyword = "wall";
	    else
		keyword = pexit->keyword;
	    if ( !IS_NPC(ch) )
		chance = LEARNED(ch, gsn_bashdoor) / 2;
	    else
		chance = 90;
	    if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
		chance /= 3;

	    if ( !IS_SET( pexit->exit_info, EX_BASHPROOF )
	    &&   ch->move >= 15
	    &&   number_percent( ) < ( chance + 4 * ( get_curr_str( ch ) - 19 ) ) )
	    {
		REMOVE_BIT( pexit->exit_info, EX_CLOSED );
		if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
		REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		SET_BIT( pexit->exit_info, EX_BASHED );

		act(AT_SKILL, "Crash!  You bashed open the $d!", ch, NULL, keyword, TO_CHAR );
		act(AT_SKILL, "$n bashes open the $d!",          ch, NULL, keyword, TO_ROOM );
		learn_from_success(ch, gsn_bashdoor);

		if ( (to_room = pexit->to_room) != NULL
		&&   (pexit_rev = pexit->rexit) != NULL
		&&    pexit_rev->to_room	== ch->in_room )
		{
			CHAR_DATA *rch;

			REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
			if ( IS_SET( pexit_rev->exit_info, EX_LOCKED ) )
			  REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
			SET_BIT( pexit_rev->exit_info, EX_BASHED );

			for ( rch = to_room->first_person; rch; rch = rch->next_in_room )
			{
			    act(AT_SKILL, "The $d crashes open!",
				rch, NULL, pexit_rev->keyword, TO_CHAR );
			}
		}
		damage( ch, ch, ( ch->max_hit / 20 ), gsn_bashdoor );

	    }
	    else
	    {
		act(AT_SKILL, "WHAAAAM!!!  You bash against the $d, but it doesn't budge.",
			ch, NULL, keyword, TO_CHAR );
		act(AT_SKILL, "WHAAAAM!!!  $n bashes against the $d, but it holds strong.",
			ch, NULL, keyword, TO_ROOM );
		damage( ch, ch, ( ch->max_hit / 20 ) + 10, gsn_bashdoor );
		learn_from_failure(ch, gsn_bashdoor);
	    }
	}    
	else
	{
	    act(AT_SKILL, "WHAAAAM!!!  You bash against the wall, but it doesn't budge.",
		ch, NULL, NULL, TO_CHAR );
	    act(AT_SKILL, "WHAAAAM!!!  $n bashes against the wall, but it holds strong.",
		ch, NULL, NULL, TO_ROOM );
	    damage( ch, ch, ( ch->max_hit / 20 ) + 10, gsn_bashdoor );
	    learn_from_failure(ch, gsn_bashdoor);
	}
	if ( !char_died( ch ) )
	   for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
	   {
		 if ( IS_AWAKE( gch )
		 && !gch->fighting
		 && ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
		 && ( ch->level - gch->level <= 4 )
		 && number_bits( 2 ) == 0 )
		 multi_hit( gch, ch, TYPE_UNDEFINED );
	   }

        return;
}


void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	    { send_to_char( "You can't seem to wake up!\n\r", ch ); return; }

	send_to_char( "You wake and climb quickly to your feet.\n\r", ch );
	act( AT_ACTION, "$n arises from $s slumber.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_RESTING:
        send_to_char( "You gather yourself and stand up.\n\r", ch );
	act( AT_ACTION, "$n rises from $s rest.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_STANDING;
        break;

    case POS_SITTING:
	send_to_char( "You move quickly to your feet.\n\r", ch );
	act( AT_ACTION, "$n rises up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
    case POS_EVASIVE:
    case POS_DEFENSIVE:
    case POS_AGGRESSIVE:
    case POS_BERSERK:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}


void do_sit( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
            { send_to_char( "You can't seem to wake up!\n\r", ch ); return; }

	send_to_char( "You wake and sit up.\n\r", ch );
	act( AT_ACTION, "$n wakes and sits up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SITTING;
	break;

    case POS_RESTING:
        send_to_char( "You stop resting and sit up.\n\r", ch );
	act( AT_ACTION, "$n stops resting and sits up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SITTING;
	break;

    case POS_STANDING:
        send_to_char( "You sit down.\n\r", ch );
	act( AT_ACTION, "$n sits down.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_SITTING;
        break;
    case POS_SITTING:
	send_to_char( "You are already sitting.\n\r", ch );
	return;

    case POS_FIGHTING:
    case POS_EVASIVE:
    case POS_DEFENSIVE:
    case POS_AGGRESSIVE:
    case POS_BERSERK:
	send_to_char( "You are busy fighting!\n\r", ch );
	return;
    case POS_MOUNTED:
        send_to_char( "You are already sitting - on your mount.\n\r", ch );
        return;
    }

    return;
}


void do_rest( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
            { send_to_char( "You can't seem to wake up!\n\r", ch ); return; }

	send_to_char( "You rouse from your slumber.\n\r", ch );
	act( AT_ACTION, "$n rouses from $s slumber.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_RESTING;
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	return;

    case POS_STANDING:
        send_to_char( "You sprawl out haphazardly.\n\r", ch );
	act( AT_ACTION, "$n sprawls out haphazardly.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_RESTING;
        break;

    case POS_SITTING:
        send_to_char( "You lie back and sprawl out to rest.\n\r", ch );
	act( AT_ACTION, "$n lies back and sprawls out to rest.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;

    case POS_FIGHTING:
    case POS_EVASIVE:
    case POS_DEFENSIVE:
    case POS_AGGRESSIVE:
    case POS_BERSERK:
	send_to_char( "You are busy fighting!\n\r", ch );
	return;
    case POS_MOUNTED:
        send_to_char( "You'd better dismount first.\n\r", ch );
        return;
    }

    rprog_rest_trigger( ch );
    return;
}


void do_sleep( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	return;

    case POS_RESTING:
	if ( ch->mental_state > 30 && (number_percent()+10) < ch->mental_state )
	{
	    send_to_char( "You just can't seem to calm yourself down enough to sleep.\n\r", ch );
	    act( AT_ACTION, "$n closes $s eyes for a few moments, but just can't seem to go to sleep.", ch, NULL, NULL, TO_ROOM );
	    return;
	}
        send_to_char( "You close your eyes and drift into slumber.\n\r", ch );
	act( AT_ACTION, "$n closes $s eyes and drifts into a deep slumber.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_SLEEPING;
        break;

    case POS_SITTING:
	if ( ch->mental_state > 30 && (number_percent()+5) < ch->mental_state )
	{
	    send_to_char( "You just can't seem to calm yourself down enough to sleep.\n\r", ch );
	    act( AT_ACTION, "$n closes $s eyes for a few moments, but just can't seem to go to sleep.", ch, NULL, NULL, TO_ROOM );
	    return;
	}
        send_to_char( "You slump over and fall dead asleep.\n\r", ch );
	act( AT_ACTION, "$n nods off and slowly slumps over, dead asleep.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_SLEEPING;
        break;

    case POS_STANDING: 
	if ( ch->mental_state > 30 && number_percent() < ch->mental_state )
	{
	    send_to_char( "You just can't seem to calm yourself down enough to sleep.\n\r", ch );
	    act( AT_ACTION, "$n closes $s eyes for a few moments, but just can't seem to go to sleep.", ch, NULL, NULL, TO_ROOM );
	    return;
	}
        send_to_char( "You collapse into a deep sleep.\n\r", ch );
	act( AT_ACTION, "$n collapses into a deep sleep.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_SLEEPING;
        break;

    case POS_FIGHTING:
    case POS_EVASIVE:
    case POS_DEFENSIVE:
    case POS_AGGRESSIVE:
    case POS_BERSERK:
	send_to_char( "You are busy fighting!\n\r", ch );
	return;
    case POS_MOUNTED:
        send_to_char( "You really should dismount first.\n\r", ch );
        return;
    }

    rprog_sleep_trigger( ch );
    return;
}


void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
    { act( AT_PLAIN, "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) || victim->position < POS_SLEEPING )
    { act( AT_PLAIN, "You can't seem to wake $M!",  ch, NULL, victim, TO_CHAR );  return; }

    act( AT_ACTION, "You wake $M.", ch, NULL, victim, TO_CHAR );
    victim->position = POS_STANDING;
    act( AT_ACTION, "$n wakes you.", ch, NULL, victim, TO_VICT );
    return;
}


/*
 * teleport a character to another room
 */
void teleportch( CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool show )
{
    char buf[MAX_STRING_LENGTH];

    if ( room_is_private( room ) )
      return;
    act( AT_ACTION, "$n disappears suddenly!", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, room );
    act( AT_ACTION, "$n arrives suddenly!", ch, NULL, NULL, TO_ROOM );
    if ( show )
      do_look( ch, "auto" );
    if ( IS_SET( ch->in_room->room_flags, ROOM_DEATH ) && !IS_IMMORTAL( ch ) )
    {
       act( AT_DEAD, "$n falls prey to a terrible death!", ch, NULL, NULL, TO_ROOM );
       set_char_color( AT_DEAD, ch );
       send_to_char( "Oopsie... you're dead!\n\r", ch );
       sprintf(buf, "%s hit a DEATH TRAP in room %d!",
		     ch->name, ch->in_room->vnum );
       log_string( buf );
       to_channel( buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
       extract_char( ch, FALSE );
    }
}

void teleport( CHAR_DATA *ch, sh_int room, EXT_BV *flags )
{
    CHAR_DATA *nch, *nch_next;
    ROOM_INDEX_DATA *start = ch->in_room, *dest;
    bool show;

    dest = get_room_index( room );
    if ( !dest )
    {
	bug( "teleport: bad room vnum %d", room );
	return;
    }

    if ( xIS_SET( *flags, TELE_SHOWDESC ) )
	show = TRUE;
    else
	show = FALSE;
    if ( !xIS_SET( *flags, TELE_TRANSALL ) )
    {
	teleportch( ch, dest, show );
	return;
    }

    /* teleport everybody in the room */
    for ( nch = start->first_person; nch; nch = nch_next )
    {
	nch_next = nch->next_in_room;
	teleportch( nch, dest, show );
    }

    /* teleport the objects on the ground too */
    if ( xIS_SET( *flags, TELE_TRANSALLPLUS ) )
    {
	OBJ_DATA *obj, *obj_next;

	for ( obj = start->first_content; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    obj_from_room(obj);
	    obj_to_room(obj, dest);
	}
    }
}

/*
 * "Climb" in a certain direction.				-Thoric
 */
void do_climb( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA *pexit;
    bool found;

    found = FALSE;
    if ( argument[0] == '\0' )
    {
	for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    if ( IS_SET( pexit->exit_info, EX_xCLIMB ) )
	    {
		move_char( ch, pexit, 0 );
		return;
	    }
	send_to_char( "You cannot climb here.\n\r", ch );
	return;
    }

    if ( (pexit = find_door( ch, argument, TRUE )) != NULL
    &&   IS_SET( pexit->exit_info, EX_xCLIMB ))
    {
	move_char( ch, pexit, 0 );
	return;
    }
    send_to_char( "You cannot climb there.\n\r", ch );
    return;
}

/*
 * "enter" something (moves through an exit)			-Thoric
 */
void do_enter( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA *pexit;
    bool found;

    found = FALSE;
    if ( argument[0] == '\0' )
    {
	for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    if ( IS_SET( pexit->exit_info, EX_xENTER ) )
	    {
		move_char( ch, pexit, 0 );
		return;
	    }
	if ( ch->in_room->sector_type != SECT_INSIDE && IS_OUTSIDE(ch) )
	    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
		if ( pexit->to_room && (pexit->to_room->sector_type == SECT_INSIDE
		||  IS_SET(pexit->to_room->room_flags, ROOM_INDOORS)) )
		{
		    move_char( ch, pexit, 0 );
		    return;
		}
	send_to_char( "You cannot find an entrance here.\n\r", ch );
	return;
    }

    if ( (pexit = find_door( ch, argument, TRUE )) != NULL
    &&   IS_SET( pexit->exit_info, EX_xENTER ))
    {
	move_char( ch, pexit, 0 );
	return;
    }
    send_to_char( "You cannot enter that.\n\r", ch );
    return;
}

/*
 * Leave through an exit.					-Thoric
 */
void do_leave( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA *pexit;
    bool found;

    found = FALSE;
    if ( argument[0] == '\0' )
    {
	for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    if ( IS_SET( pexit->exit_info, EX_xLEAVE ) )
	    {
		move_char( ch, pexit, 0 );
		return;
	    }
	if ( ch->in_room->sector_type == SECT_INSIDE || !IS_OUTSIDE(ch) )
	    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
		if ( pexit->to_room && pexit->to_room->sector_type != SECT_INSIDE
		&&  !IS_SET(pexit->to_room->room_flags, ROOM_INDOORS) )
		{
		    move_char( ch, pexit, 0 );
		    return;
		}
	send_to_char( "You cannot find an exit here.\n\r", ch );
	return;
    }

    if ( (pexit = find_door( ch, argument, TRUE )) != NULL
    &&   IS_SET( pexit->exit_info, EX_xLEAVE ))
    {
	move_char( ch, pexit, 0 );
	return;
    }
    send_to_char( "You cannot leave that way.\n\r", ch );
    return;
}

/*
 * Check to see if an exit in the room is pulling (or pushing) players around.
 * Some types may cause damage.					-Thoric
 *
 * People kept requesting currents (like SillyMUD has), so I went all out
 * and added the ability for an exit to have a "pull" or a "push" force
 * and to handle different types much beyond a simple water current.
 *
 * This check is called by violence_update().  I'm not sure if this is the
 * best way to do it, or if it should be handled by a special queue.
 *
 * Future additions to this code may include equipment being blown away in
 * the wind (mostly headwear), and people being hit by flying objects
 *
 * TODO:
 *	handle more pulltypes
 *	give "entrance" messages for players and objects
 *	proper handling of player resistance to push/pulling
 */
ch_ret pullcheck( CHAR_DATA *ch, int pulse )
{
    ROOM_INDEX_DATA *room;
    EXIT_DATA *xtmp, *xit = NULL;
    OBJ_DATA *obj, *obj_next;
    bool move = FALSE, moveobj = TRUE, showroom = TRUE;
    int pullfact, pull;
    int resistance;
    char *tochar = NULL, *toroom = NULL, *objmsg = NULL;
    char *destrm = NULL, *destob = NULL, *dtxt = "somewhere";

    if ( (room=ch->in_room) == NULL )
    {
	bug( "pullcheck: %s not in a room?!?", ch->name );
	return rNONE;
    }

    /* Find the exit with the strongest force (if any) */
    for ( xtmp = room->first_exit; xtmp; xtmp = xtmp->next )
	if ( xtmp->pull && xtmp->to_room && (!xit || abs(xtmp->pull) > abs(xit->pull)) )
	    xit = xtmp;

    if ( !xit )
	return rNONE;

    pull = xit->pull;

    /* strength also determines frequency */
    pullfact = URANGE(1, 20-(abs(pull)/5), 20);

    /* strongest pull not ready yet... check for one that is */
    if ( (pulse % pullfact) != 0 )
    {
	for ( xit = room->first_exit; xit; xit = xit->next )
	    if ( xit->pull && xit->to_room )
	    {
		pull = xit->pull;
		pullfact = URANGE(1, 20-(abs(pull)/5), 20);
		if ( (pulse % pullfact) != 0 )
		    break;
	    }

	if ( !xit )
	    return rNONE;
    }

    /* negative pull = push... get the reverse exit if any */
    if ( pull < 0 )
	if ( (xit=get_exit(room, rev_dir[xit->vdir])) == NULL )
	    return rNONE;

    dtxt = rev_exit(xit->vdir);

    /*
     * First determine if the player should be moved or not
     * Check various flags, spells, the players position and strength vs.
     * the pull, etc... any kind of checks you like.
     */
    switch(xit->pulltype)
    {
	case PULL_CURRENT:
	case PULL_WHIRLPOOL:
	    switch(room->sector_type)
	    {
		/* allow whirlpool to be in any sector type */
		default:
		    if ( xit->pulltype == PULL_CURRENT )
			break;
		case SECT_WATER_SWIM:
		case SECT_WATER_NOSWIM:
		    if ( (ch->mount && !IS_FLOATING(ch->mount))
		    ||  (!ch->mount && !IS_FLOATING(ch)) )
			move = TRUE;
		    break;

		case SECT_UNDERWATER:
		case SECT_OCEANFLOOR:
		    move = TRUE;
		    break;
	    }
	    break;
	case PULL_GEYSER:
	case PULL_WAVE:
	    move = TRUE;
	    break;

	case PULL_WIND:
	case PULL_STORM:
	    /* if not flying... check weight, position & strength */
	    move = TRUE;
	    break;

	case PULL_COLDWIND:
	    /* if not flying... check weight, position & strength */
	    /* also check for damage due to bitter cold */
	    move = TRUE;
	    break;

	case PULL_HOTAIR:
	    /* if not flying... check weight, position & strength */
	    /* also check for damage due to heat */
	    move = TRUE;
	    break;

	/* light breeze -- very limited moving power */
	case PULL_BREEZE:
	    move = FALSE;
	    break;

	/*
	 * exits with these pulltypes should also be blocked from movement
	 * ie: a secret locked pickproof door with the name "_sinkhole_", etc
	 */
	case PULL_EARTHQUAKE:
	case PULL_SINKHOLE:
	case PULL_QUICKSAND:
	case PULL_LANDSLIDE:
	case PULL_SLIP:
	case PULL_LAVA:
	    if ( (ch->mount && !IS_FLOATING(ch->mount))
	    ||  (!ch->mount && !IS_FLOATING(ch)) )
		move = TRUE;
	    break;

	/* as if player moved in that direction him/herself */
	case PULL_UNDEFINED:
	    return move_char(ch, xit, 0);

	/* all other cases ALWAYS move */
	default:
	    move = TRUE;
	    break;
    }

    /* assign some nice text messages */
    switch(xit->pulltype)
    {
	case PULL_MYSTERIOUS:
	    /* no messages to anyone */
	    showroom = FALSE;
	    break;
	case PULL_WHIRLPOOL:
	case PULL_VACUUM:
	    tochar = "You are sucked $T!";
	    toroom = "$n is sucked $T!";
	    destrm = "$n is sucked in from $T!";
	    objmsg = "$p is sucked $T.";
	    destob = "$p is sucked in from $T!";
	    break;
	case PULL_CURRENT:
	case PULL_LAVA:
	    tochar = "You drift $T.";
	    toroom = "$n drifts $T.";
	    destrm = "$n drifts in from $T.";
	    objmsg = "$p drifts $T.";
	    destob = "$p drifts in from $T.";
	    break;
	case PULL_BREEZE:
	    tochar = "You drift $T.";
	    toroom = "$n drifts $T.";
	    destrm = "$n drifts in from $T.";
	    objmsg = "$p drifts $T in the breeze.";
	    destob = "$p drifts in from $T.";
	    break;
	case PULL_GEYSER:
	case PULL_WAVE:
	    tochar = "You are pushed $T!";
	    toroom = "$n is pushed $T!";
	    destrm = "$n is pushed in from $T!";
	    destob = "$p floats in from $T.";
	    break;
	case PULL_EARTHQUAKE:
	    tochar = "The earth opens up and you fall $T!";
	    toroom = "The earth opens up and $n falls $T!";
	    destrm = "$n falls from $T!";
	    objmsg = "$p falls $T.";
	    destob = "$p falls from $T.";
	    break;
	case PULL_SINKHOLE:
	    tochar = "The ground suddenly gives way and you fall $T!";
	    toroom = "The ground suddenly gives way beneath $n!";
	    destrm = "$n falls from $T!";
	    objmsg = "$p falls $T.";
	    destob = "$p falls from $T.";
	    break;
	case PULL_QUICKSAND:
	    tochar = "You begin to sink $T into the quicksand!";
	    toroom = "$n begins to sink $T into the quicksand!";
	    destrm = "$n sinks in from $T.";
	    objmsg = "$p begins to sink $T into the quicksand.";
	    destob = "$p sinks in from $T.";
	    break;
	case PULL_LANDSLIDE:
	    tochar = "The ground starts to slide $T, taking you with it!";
	    toroom = "The ground starts to slide $T, taking $n with it!";
	    destrm = "$n slides in from $T.";
	    objmsg = "$p slides $T.";
	    destob = "$p slides in from $T.";
	    break;
	case PULL_SLIP:
	    tochar = "You lose your footing!";
	    toroom = "$n loses $s footing!";
	    destrm = "$n slides in from $T.";
	    objmsg = "$p slides $T.";
	    destob = "$p slides in from $T.";
	    break;
	case PULL_VORTEX:
	    tochar = "You are sucked into a swirling vortex of colors!";
	    toroom = "$n is sucked into a swirling vortex of colors!";
	    toroom = "$n appears from a swirling vortex of colors!";
	    objmsg = "$p is sucked into a swirling vortex of colors!";
	    objmsg = "$p appears from a swirling vortex of colors!";
	    break;
	case PULL_HOTAIR:
	    tochar = "A blast of hot air blows you $T!";
	    toroom = "$n is blown $T by a blast of hot air!";
	    destrm = "$n is blown in from $T by a blast of hot air!";
	    objmsg = "$p is blown $T.";
	    destob = "$p is blown in from $T.";
	    break;
	case PULL_COLDWIND:
	    tochar = "A bitter cold wind forces you $T!";
	    toroom = "$n is forced $T by a bitter cold wind!";
	    destrm = "$n is forced in from $T by a bitter cold wind!";
	    objmsg = "$p is blown $T.";
	    destob = "$p is blown in from $T.";
	    break;
	case PULL_WIND:
	    tochar = "A strong wind pushes you $T!";
	    toroom = "$n is blown $T by a strong wind!";
	    destrm = "$n is blown in from $T by a strong wind!";
	    objmsg = "$p is blown $T.";
	    destob = "$p is blown in from $T.";
	    break;
	case PULL_STORM:
	    tochar = "The raging storm drives you $T!";
	    toroom = "$n is driven $T by the raging storm!";
	    destrm = "$n is driven in from $T by a raging storm!";
	    objmsg = "$p is blown $T.";
	    destob = "$p is blown in from $T.";
	    break;
	default:
	    if ( pull > 0 )
	    {
		tochar = "You are pulled $T!";
		toroom = "$n is pulled $T.";
		destrm = "$n is pulled in from $T.";
		objmsg = "$p is pulled $T.";
		objmsg = "$p is pulled in from $T.";
	    }
	    else
	    {
		tochar = "You are pushed $T!";
		toroom = "$n is pushed $T.";
		destrm = "$n is pushed in from $T.";
		objmsg = "$p is pushed $T.";
		objmsg = "$p is pushed in from $T.";
	    }
	    break;
    }


    /* Do the moving */
    if ( move )
    {
	/* display an appropriate exit message */
	if ( tochar )
	{
	    act(AT_PLAIN, tochar, ch, NULL, dir_name[xit->vdir], TO_CHAR );
	    send_to_char("\n\r", ch);
	}
	if ( toroom )
	    act(AT_PLAIN, toroom, ch, NULL, dir_name[xit->vdir], TO_ROOM );

	/* display an appropriate entrance message */
	if ( destrm && xit->to_room->first_person )
	{
	    act(AT_PLAIN, destrm, xit->to_room->first_person, NULL, dtxt, TO_CHAR );
	    act(AT_PLAIN, destrm, xit->to_room->first_person, NULL, dtxt, TO_ROOM );
	}


	/* move the char */
	if ( xit->pulltype == PULL_SLIP )
	    return move_char(ch, xit, 1);
	char_from_room(ch);
	char_to_room(ch, xit->to_room);

	if ( showroom )
	    do_look(ch, "auto");

	/* move the mount too */
	if ( ch->mount )
	{
	    char_from_room(ch->mount);
	    char_to_room(ch->mount, xit->to_room);
	    if ( showroom )
		do_look(ch->mount, "auto");
	}
    }

    /* move objects in the room */
    if ( moveobj )
    {
	for ( obj = room->first_content; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( IS_OBJ_STAT(obj, ITEM_BURIED) || !CAN_WEAR(obj, ITEM_TAKE) )
		continue;

	    resistance = get_obj_weight(obj);
	    if ( IS_OBJ_STAT(obj, ITEM_METAL) )
		resistance = (resistance * 6) / 5;
	    switch(obj->item_type)
	    {
		case ITEM_SCROLL:
		case ITEM_NOTE:
		case ITEM_TRASH:
		    resistance >>= 2;	break;
		case ITEM_SCRAPS:
		case ITEM_CONTAINER:
		    resistance >>= 1;	break;
		case ITEM_PEN:
		case ITEM_WAND:
		    resistance = (resistance * 5) / 6;
		    break;

		case ITEM_CORPSE_PC:
		case ITEM_CORPSE_NPC:
		case ITEM_FOUNTAIN:
		    resistance <<= 2;	break;
	    }

	    /* is the pull greater than the resistance of the object? */
	    if ( (abs(pull) * 10) > resistance )
	    {
		if ( objmsg && room->first_person )
		{
		    act(AT_PLAIN, objmsg, room->first_person, obj, dir_name[xit->vdir], TO_CHAR);
		    act(AT_PLAIN, objmsg, room->first_person, obj, dir_name[xit->vdir], TO_ROOM);
		}
		if ( destob && xit->to_room->first_person )
		{
		    act(AT_PLAIN, destob, xit->to_room->first_person, obj, dtxt, TO_CHAR);
		    act(AT_PLAIN, destob, xit->to_room->first_person, obj, dtxt, TO_ROOM);
		}
		obj_from_room(obj);
		obj_to_room(obj, xit->to_room);
	    }
	}
    }

    return rNONE;
}

/*
 * This function bolts a door. Written by Blackmane
 */

void do_bolt (CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  EXIT_DATA *pexit;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    {
      send_to_char ("Bolt what?\n\r", ch);
      return;
    }

  if ((pexit = find_door (ch, arg, TRUE)) != NULL)
  {

      	if (!IS_SET (pexit->exit_info, EX_ISDOOR))
	{
   	  send_to_char ("You can't do that.\n\r", ch);
   	  return;
 	}
        if (!IS_SET (pexit->exit_info, EX_CLOSED))
 	{
   	  send_to_char ("It's not closed.\n\r", ch);
   	  return;
 	}
        if (!IS_SET (pexit->exit_info, EX_ISBOLT))
 	{
   	  send_to_char ("You don't see a bolt.\n\r", ch);
   	  return;
 	}
 	if (IS_SET (pexit->exit_info, EX_BOLTED))
 	{
   	  send_to_char ("It's already bolted.\n\r", ch);
   	  return;
 	}

        if (!IS_SET (pexit->exit_info, EX_SECRET)
   	|| (pexit->keyword && nifty_is_name (arg, pexit->keyword)))
 	{
   	  send_to_char ("*Clunk*\n\r", ch);
   	  act (AT_ACTION,"$n bolts the $d.", ch, NULL, pexit->keyword, TO_ROOM);
   	  set_bexit_flag (pexit, EX_BOLTED);
   	  return;
 	}
    }

    ch_printf (ch, "You see no %s here.\n\r", arg);
    return;
}



/*
 * This function unbolts a door.  Written by Blackmane
 */

void do_unbolt (CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  EXIT_DATA *pexit;

  one_argument (argument, arg);

  if (arg[0] == '\0')
  {
      send_to_char ("Unbolt what?\n\r", ch);
      return;
  }

  if ((pexit = find_door (ch, arg, TRUE)) != NULL)
  {

      	if (!IS_SET (pexit->exit_info, EX_ISDOOR))
  	{
   	  send_to_char ("You can't do that.\n\r", ch);
   	  return;
 	}
      	if (!IS_SET (pexit->exit_info, EX_CLOSED))
 	{
   	  send_to_char ("It's not closed.\n\r", ch);
   	  return;
 	}
      	if (!IS_SET (pexit->exit_info, EX_ISBOLT))
 	{
   	  send_to_char ("You don't see a bolt.\n\r", ch);
   	  return;
 	}
      	if (!IS_SET (pexit->exit_info, EX_BOLTED))
 	{
   	  send_to_char ("It's already unbolted.\n\r", ch);
   	  return;
 	}

      	if (!IS_SET (pexit->exit_info, EX_SECRET)
   	|| (pexit->keyword && nifty_is_name (arg, pexit->keyword)))
 	{
   	  send_to_char ("*Clunk*\n\r", ch);
   	  act (AT_ACTION,"$n unbolts the $d.",ch,NULL,pexit->keyword,TO_ROOM);
   	  remove_bexit_flag (pexit, EX_BOLTED);
   	  return;
 	}
    }

    ch_printf (ch, "You see no %s here.\n\r", arg);
    return;
}
