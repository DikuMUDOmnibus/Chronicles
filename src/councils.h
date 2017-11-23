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
 *   Hans-Henrik Stærfeldt, Tom Madsen, and Katja Nyboe.                   *
 ***************************************************************************
 * - Councils module header                                                *
 ***************************************************************************/

typedef struct	council_data		COUNCIL_DATA;

/*
 * Council function definitions.
 */
COUNCIL_DATA *	get_council	args( ( char *name ) );
void		load_councils	args( ( void ) );
void		save_council	args( ( COUNCIL_DATA *council ) );

#define MAX_CPD	2	/* Maximum council power level difference */

struct  council_data
{
    COUNCIL_DATA * next;	/* next council in list			*/
    COUNCIL_DATA * prev;	/* previous council in list		*/
    char *	filename;	/* Council filename			*/
    char *	name;		/* Council name				*/
    char *	description;	/* A brief description of the council	*/
    char *	head;		/* Council head				*/
    char *	head2;		/* Council co-head			*/
    char *	powers;		/* Council powers			*/
    sh_int	members;	/* Number of council members		*/
    int		board;		/* Vnum of council board		*/
    int		meeting;	/* Vnum of council's meeting room	*/
};

extern		COUNCIL_DATA	*	first_council;
extern		COUNCIL_DATA	*	last_council;
