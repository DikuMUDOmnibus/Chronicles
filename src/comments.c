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
 * - Comments (notes) module by Haus                                       *
 ***************************************************************************/

#include <ctype.h>
#include <string.h>
#include <time.h>
#include "mud.h"


void note_attach(CHAR_DATA *ch);

void comment_remove( CHAR_DATA *ch, CHAR_DATA *victim, NOTE_DATA *pnote )
{
    if ( !victim->comments )
    {
      bug( "comment remove: null board", 0 );
      return;
    }

    if ( !pnote )
    {
      bug( "comment remove: null pnote", 0 );
      return;
    }
    
    /*
     * Remove comment from linked list.
     */
    if ( !pnote->prev )
      victim->comments	= pnote->next;
    else
      pnote->prev->next = pnote->next;

    STRFREE( pnote->text    );
    STRFREE( pnote->subject );
    STRFREE( pnote->to_list );
    STRFREE( pnote->date    );
    STRFREE( pnote->sender  );
    DISPOSE( pnote );

    /*
     * Rewrite entire list.
     */
     save_char_obj(victim);

    return;
}

void do_comment( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    NOTE_DATA  *pnote;
    CHAR_DATA  *victim;
    int vnum;
    int anum;

    if ( IS_NPC(ch) )
    {
	send_to_char("Mobs can't use the comment command.\n\r", ch);
	return;
    }

    if ( !ch->desc )
    {
	bug( "do_comment: no descriptor", 0 );
	return;
    }

    /* Put in to prevent crashing when someone issues a comment command
       from within the editor. -Narn */
    if ( ch->desc->connected == CON_EDITING )
    {
	send_to_char("You can't use the comment command from within the editor.\n\r", ch);
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_WRITING_NOTE:
	  if ( !ch->pnote )
	  {
	     bug( "do_comment: note got lost?", 0 );
	     send_to_char( "Your note got lost!\n\r", ch );
	     stop_editing(ch);
	     return;
	  }
	  if ( ch->dest_buf != ch->pnote )
	     bug( "do_comment: sub_writing_note: ch->dest_buf != ch->pnote", 0 );
	  STRFREE( ch->pnote->text );
	  ch->pnote->text = copy_buffer( ch );
	  stop_editing( ch );	   
	  return;
    }

    set_char_color( AT_NOTE, ch );
    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( !str_cmp( arg, "about" ) )
    {

        victim = get_char_world(ch, argument);
	if (!victim)
	{
	   send_to_char("They're not logged on!\n\r", ch);   /* maybe fix this? */
	   return;
	}

    if ( IS_NPC(victim) )
    {
	send_to_char("No comments about mobs\n\r", ch);
	return;
    }


    }


    if ( !str_cmp( arg, "list" ) )
    {
        victim = get_char_world(ch, argument);
	if (!victim)
	{
	   send_to_char("They're not logged on!\n\r", ch);   /* maybe fix this? */
	   return;
	}

        if ( IS_NPC(victim) )
        {
	    send_to_char("No comments about mobs\n\r", ch);
	    return;
        }

	if ( get_trust(victim) >= get_trust( ch ) )
	{
	  send_to_char( "You're not of the right caliber to do this...\n\r", ch );
	  return;
	}

	if ( !victim->comments )
	{
	  send_to_char( "There are no relevant comments.\n\r", ch );
	  return;
	}

	vnum = 0;
	for ( pnote = victim->comments; pnote; pnote = pnote->next )
	{
	  vnum++;
	  sprintf( buf, "%2d) %-10s [%s] %s\n\r",
	     vnum,
	     pnote->sender,
             pnote->date, 
	     pnote->subject );
/* Brittany added date to comment list and whois with above change */
	  send_to_char( buf, ch );
	}

	/* act( AT_ACTION, "$n glances over the notes.", ch, NULL, NULL, TO_ROOM ); */
	return;
    }

    if ( !str_cmp( arg, "read" ) )
    {
	bool fAll;

        argument = one_argument( argument, arg1 );
        victim = get_char_world(ch, arg1);
	if (!victim)
	{
	   send_to_char("They're not logged on!\n\r", ch);   /* maybe fix this? */
	   return;
	}

        if ( IS_NPC(victim) )
        {
	    send_to_char("No comments about mobs\n\r", ch);
	    return;
        }

	if ( get_trust(victim) >= get_trust( ch ) )
	{
	  send_to_char( "You're not of the right caliber to do this...\n\r", ch );
	  return;
	}

	if ( !victim->comments )
	{
	  send_to_char( "There are no relevant comments.\n\r", ch );
	  return;
	}



	if ( !str_cmp( argument, "all" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else
	if ( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Note read which number?\n\r", ch );
	    return;
	}

	vnum = 0;
	for ( pnote = victim->comments; pnote; pnote = pnote->next )
	{
	    vnum++;
	    if ( vnum == anum || fAll )
	    {
		sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
		    vnum,
		    pnote->sender,
		    pnote->subject,
		    pnote->date,
		    pnote->to_list
		    );
		send_to_char( buf, ch );
		send_to_char( pnote->text, ch );
		/* act( AT_ACTION, "$n reads a note.", ch, NULL, NULL, TO_ROOM ); */
		return;
	    }
	}

	send_to_char( "No such comment.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "write" ) )
    {
	note_attach( ch );
	ch->substate = SUB_WRITING_NOTE;
	ch->dest_buf = ch->pnote;
	start_editing( ch, ch->pnote->text );
	return;
    }

    if ( !str_cmp( arg, "subject" ) )
    {
	note_attach( ch );
	STRFREE( ch->pnote->subject );
	ch->pnote->subject = STRALLOC( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "to" ) )
    {
	note_attach( ch );
	STRFREE( ch->pnote->to_list );
	ch->pnote->to_list = STRALLOC( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "clear" ) )
    {
	if ( ch->pnote )
	{
	    STRFREE( ch->pnote->text );
	    STRFREE( ch->pnote->subject );
	    STRFREE( ch->pnote->to_list );
	    STRFREE( ch->pnote->date );
	    STRFREE( ch->pnote->sender );
	    DISPOSE( ch->pnote );
	}
	ch->pnote = NULL;

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "show" ) )
    {
	if ( !ch->pnote )
	{
	    send_to_char( "You have no comment in progress.\n\r", ch );
	    return;
	}

	sprintf( buf, "%s: %s\n\rTo: %s\n\r",
	    ch->pnote->sender,
	    ch->pnote->subject,
	    ch->pnote->to_list
	    );
	send_to_char( buf, ch );
	send_to_char( ch->pnote->text, ch );
	return;
    }

    if ( !str_cmp( arg, "post" ) )
    {
	char *strtime;

	if ( !ch->pnote )
	{
	    send_to_char( "You have no comment in progress.\n\r", ch );
	    return;
	}

        argument = one_argument(argument, arg1);
        victim = get_char_world(ch, arg1);
	if (!victim)
	{
	   send_to_char("They're not logged on!\n\r", ch);   /* maybe fix this? */
	   return;
	}

        if ( IS_NPC(victim) )
        {
	    send_to_char("No comments about mobs\n\r", ch);
	    return;
        }

	if (  get_trust(victim) > get_trust( ch ) )
	{
	  send_to_char( "You're not of the right caliber to do this...\n\r", ch );
	  return;
	}

	/* act( AT_ACTION, "$n posts a note.", ch, NULL, NULL, TO_ROOM ); */

	strtime				= ctime( &current_time );
	strtime[strlen(strtime)-1]	= '\0';
	ch->pnote->date			= STRALLOC( strtime );

	pnote		= ch->pnote;
	ch->pnote	= NULL;


	/* LIFO to make life easier */
	pnote->next = victim->comments;
        if (victim->comments)
	   victim->comments->prev = pnote;
        pnote->prev = NULL;
        victim->comments = pnote;

        save_char_obj(victim);


#ifdef NOTDEFD
	FCLOSE( fpReserve );
	sprintf( notefile, "%s/%s", BOARD_DIR, board->note_file );
	if ( ( fp = fopen( notefile, "a" ) ) == NULL )
	{
	    perror( notefile );
	}
	else
	{
	    fprintf( fp, "Sender  %s~\nDate    %s~\nTo      %s~\nSubject %s~\nText\n%s~\n\n",
		pnote->sender,
		pnote->date,
		pnote->to_list,
		pnote->subject,
		pnote->text
		);
	    FCLOSE( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );
#endif

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "remove" ) )
    {
        argument = one_argument(argument, arg1);
        victim = get_char_world(ch, arg1);
	if (!victim)
	{
	   send_to_char("They're not logged on!\n\r", ch);   /* maybe fix this? */
	   return;
	}

        if ( IS_NPC(victim) )
        {
	    send_to_char("No comments about mobs\n\r", ch);
	    return;
        }

	if (  (get_trust(victim) >= get_trust( ch ) )
           || ( get_trust( ch ) < 58                ) )   /* switch to some LEVEL_ thingie */
	{
	  send_to_char( "You're not of the right caliber to do this...\n\r", ch );
	  return;
	}

	/*argument = one_argument(argument, arg); */
	if ( !is_number( argument ) )
	{
	    send_to_char( "Comment remove which number?\n\r", ch );
	    return;
	}

	anum = atoi( argument );
	vnum = 0;
	for ( pnote = victim->comments; pnote; pnote = pnote->next )
	{
	    vnum++;
	    if ( ( 58 <= get_trust( ch ) )    /* switch to some LEVEL_ thingie */
	    &&   ( vnum == anum ) )
	    {
		comment_remove( ch, victim, pnote );
		send_to_char( "Ok.\n\r", ch );
		/* act( AT_ACTION, "$n removes a note.", ch, NULL, NULL, TO_ROOM ); */
		return;
	    }
	}

	send_to_char( "No such comment.\n\r", ch );
	return;
    }

    send_to_char( "Huh?  Type 'help comment' for usage (i hope!).\n\r", ch );
    return;
}


void fwrite_comments( CHAR_DATA *ch, FILE *fp )
{
NOTE_DATA *pnote;

   if(!ch->comments)
      return;

   for(pnote=ch->comments;pnote;pnote=pnote->next)
   {
       fprintf( fp,"#COMMENT\n" );
       fprintf( fp,"sender	%s~\n",pnote->sender);
       fprintf( fp,"date  	%s~\n",pnote->date);
       fprintf( fp,"to     	%s~\n",pnote->to_list);
       fprintf( fp,"subject	%s~\n",pnote->subject);
       fprintf( fp,"text\n%s~\n",pnote->text);
   }
   return;
}

void fread_comment( CHAR_DATA *ch, FILE *fp )
{
    NOTE_DATA *pnote;

    for ( ; ; )
    {
	char letter;

	do
	{
	    letter = getc( fp );
	    if ( feof(fp) )
	    {
		FCLOSE( fp );
		return;
	    }
	}
	while ( isspace(letter) );
	ungetc( letter, fp );

	CREATE( pnote, NOTE_DATA, 1 );

	if ( str_cmp( fread_word( fp ), "sender" ) )
	    break;
	pnote->sender	= fread_string( fp );

	if ( str_cmp( fread_word( fp ), "date" ) )
	    break;
	pnote->date	= fread_string( fp );

	if ( str_cmp( fread_word( fp ), "to" ) )
	    break;
	pnote->to_list	= fread_string( fp );

	if ( str_cmp( fread_word( fp ), "subject" ) )
	    break;
	pnote->subject	= fread_string( fp );

	if ( str_cmp( fread_word( fp ), "text" ) )
	    break;
	pnote->text	= fread_string( fp );

	pnote->next		= ch->comments;
	pnote->prev		= NULL;
	ch->comments		= pnote;
	return;
    }

    bug( "fread_comment: bad key word. strap in!", 0 );
    /* exit( 1 ); */
}

