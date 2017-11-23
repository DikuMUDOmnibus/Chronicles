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
 * - Auction module header                                                 *
 ***************************************************************************
 * The following code was written by Erwin Andreasen for the automated     *
 * auction command.                                                        *
 *                                                                         *
 * Completely cleaned up by Thoric                                         *
 ***************************************************************************/

typedef struct	auction_data		AUCTION_DATA;

/* how many items to track.... prevent repeat auctions */
#define AUCTION_MEM 3
#define PULSE_AUCTION                            (9 * PULSE_PER_SECOND)

void    talk_auction    args( ( char *argument ) );
void    auction_update  args( ( void ) );

struct  auction_data
{
    OBJ_DATA *	item;		/* a pointer to the item			*/
    CHAR_DATA *	seller;		/* a pointer to the seller - which may NOT quit	*/
    CHAR_DATA *	buyer;		/* a pointer to the buyer - which may NOT quit	*/
    int		bet;		/* last bet - or 0 if noone has bet anything	*/
    sh_int	going;		/* 1,2, and sold to the man in the pointy hat	*/
    sh_int	pulse;		/* how many pulses (.25s) until another call	*/
    int		starting;
    OBJ_INDEX_DATA * history[AUCTION_MEM];	/* store auction history	*/
    sh_int	hist_timer;	/* clear out history buffer if auction is idle	*/
};

extern		AUCTION_DATA	*	auction;
