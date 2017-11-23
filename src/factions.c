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
 * - Faction module                                                        *
 ***************************************************************************/

#include <string.h>
#include "mud.h"
#include "councils.h"
#include "factions.h"
#include "files.h"
#include "language.h"

#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];

FACTION_DATA * first_faction;
FACTION_DATA * last_faction;
COUNCIL_DATA * first_council;
COUNCIL_DATA * last_council;

char *	const			faction_flag [];

/* local routines */
void	fread_faction		args( ( FACTION_DATA *faction, FILE *fp ) );
bool	load_faction_file	args( ( char *factionfile ) );
void	write_faction_list	args( ( void ) );

void	fread_council		args( ( COUNCIL_DATA *council, FILE *fp ) );
bool	load_council_file	args( ( char *councilfile ) );
void	write_council_list	args( ( void ) );

bool	check_factionflags	args( ( CHAR_DATA *ch, FACTION_DATA *faction ) );
char *	faction_bit_name	args( ( EXT_BV *faction_flags ) );
int	get_factionflag		args( ( char *flag ) );

/*
 * Get pointer to faction structure from faction name.
 */
FACTION_DATA *get_faction( char *name )
{
    FACTION_DATA *faction;
    
    for ( faction = first_faction; faction; faction = faction->next )
       if ( !str_cmp( name, faction->name ) )
         return faction;
    return NULL;
}

COUNCIL_DATA *get_council( char *name )
{
    COUNCIL_DATA *council;
    
    for ( council = first_council; council; council = council->next )
       if ( !str_cmp( name, council->name ) )
         return council;
    return NULL;
}

void write_faction_list( )
{
    FACTION_DATA *tfact;
    FILE *fpout;
    char filename[256];

    sprintf( filename, "%s%s", FACTION_DIR, FACTION_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
	bug( "FATAL: cannot open faction.lst for writing!\n\r", 0 );
 	return;
    }	  
    for ( tfact = first_faction; tfact; tfact = tfact->next )
	fprintf( fpout, "%s\n", tfact->filename );
    fprintf( fpout, "$\n" );
    FCLOSE( fpout );
}

void write_council_list( )
{
    COUNCIL_DATA *tcouncil;
    FILE *fpout;
    char filename[256];

    sprintf( filename, "%s%s", COUNCIL_DIR, COUNCIL_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
	bug( "FATAL: cannot open council.lst for writing!\n\r", 0 );
 	return;
    }	  
    for ( tcouncil = first_council; tcouncil; tcouncil = tcouncil->next )
	fprintf( fpout, "%s\n", tcouncil->filename );
    fprintf( fpout, "$\n" );
    FCLOSE( fpout );
}

/*
 * Save a faction's data to its data file
 */
void save_faction( FACTION_DATA *faction )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];

    if ( !faction )
    {
	bug( "save_faction: null faction pointer!", 0 );
	return;
    }
        
    if ( !faction->filename || faction->filename[0] == '\0' )
    {
	sprintf( buf, "save_faction: %s has no filename", faction->name );
	bug( buf, 0 );
	return;
    }
    
    sprintf( filename, "%s%s", FACTION_DIR, faction->filename );
    
    FCLOSE( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_faction: fopen", 0 );
    	perror( filename );
    }
    else
    {
	fprintf( fp, "#FACTION\n" );
	fprintf( fp, "Name         %s~\n",	faction->name		);
	fprintf( fp, "Filename     %s~\n",	faction->filename	);
	fprintf( fp, "Motto        %s~\n",	faction->motto		);
	fprintf( fp, "Description  %s~\n",	faction->description	);
	fprintf( fp, "Deity        %s~\n",	faction->deity		);
	fprintf( fp, "Leader       %s~\n",	faction->leader		);
	fprintf( fp, "NumberOne    %s~\n",	faction->number1	);
	fprintf( fp, "NumberTwo    %s~\n",	faction->number2	);
	fprintf( fp, "Badge        %s~\n",	faction->badge		);
	fprintf( fp, "Leadrank     %s~\n",	faction->leadrank	);
	fprintf( fp, "Onerank      %s~\n",	faction->onerank	);
	fprintf( fp, "Tworank      %s~\n",	faction->tworank	);
	fprintf( fp, "PKillRangeNew   %d %d %d %d %d %d %d\n",
		faction->pkills[0], faction->pkills[1], faction->pkills[2],
		faction->pkills[3], faction->pkills[4], faction->pkills[5],
		faction->pkills[6]);
	fprintf( fp, "PDeathRangeNew  %d %d %d %d %d %d %d\n",	
		faction->pdeaths[0], faction->pdeaths[1], faction->pdeaths[2],
		faction->pdeaths[3], faction->pdeaths[4], faction->pdeaths[5],
		faction->pdeaths[6]);
	fprintf( fp, "MKills       %d\n",	faction->mkills		);
	fprintf( fp, "MDeaths      %d\n",	faction->mdeaths	);
	fprintf( fp, "IllegalPK    %d\n",	faction->illegal_pk	);
	fprintf( fp, "Score        %d\n",	faction->score		);
	fprintf( fp, "Type         %d\n",	faction->faction_type	);
	fprintf( fp, "Flags        %s\n",	print_bitvector( &faction->flags ) );
	fprintf( fp, "Favour       %d\n",	faction->favour		);
	fprintf( fp, "Strikes      %d\n",	faction->strikes	);
	fprintf( fp, "Members      %d\n",	faction->members	);
	fprintf( fp, "MemLimit     %d\n",	faction->mem_limit	);
	fprintf( fp, "Alignment    %d\n",	faction->alignment	);
	fprintf( fp, "Board        %d\n",	faction->board		);
	fprintf( fp, "ObjOne       %d\n",	faction->obj1		);
	fprintf( fp, "ObjTwo       %d\n",	faction->obj2		);
	fprintf( fp, "ObjThree     %d\n",	faction->obj3		);
        fprintf( fp, "ObjFour      %d\n",	faction->obj4		);
	fprintf( fp, "ObjFive      %d\n", 	faction->obj5		);
	fprintf( fp, "Recall       %d\n",	faction->recall		);
	fprintf( fp, "Storeroom    %d\n",	faction->storeroom	);
	fprintf( fp, "End\n\n"						);
	fprintf( fp, "#END\n"						);
    }
    FCLOSE( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
 * Save a council's data to its data file
 */
void save_council( COUNCIL_DATA *council )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];

    if ( !council )
    {
	bug( "save_council: null council pointer!", 0 );
	return;
    }
        
    if ( !council->filename || council->filename[0] == '\0' )
    {
	sprintf( buf, "save_council: %s has no filename", council->name );
	bug( buf, 0 );
	return;
    }
 
    sprintf( filename, "%s%s", COUNCIL_DIR, council->filename );
    
    FCLOSE( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_council: fopen", 0 );
    	perror( filename );
    }
    else
    {

        fprintf( fp, "#COUNCIL\n" );
        if ( council-> name ) 
           fprintf( fp, "Name         %s~\n",   council->name           );
        if ( council->filename )
           fprintf( fp, "Filename     %s~\n",   council->filename       );
        if ( council->description )
           fprintf( fp, "Description  %s~\n",   council->description    );
        if ( council->head ) 
           fprintf( fp, "Head         %s~\n",   council->head           );
        if ( council->head2 != NULL)
                fprintf (fp, "Head2        %s~\n", council->head2);
        fprintf( fp, "Members      %d\n",       council->members        );
        fprintf( fp, "Board        %d\n",       council->board          );
        fprintf( fp, "Meeting      %d\n",       council->meeting        );
        if ( council->powers )
           fprintf( fp, "Powers       %s~\n",   council->powers         );
        fprintf( fp, "End\n\n"                                          );
        fprintf( fp, "#END\n"                                           );

    }
    FCLOSE( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Read in actual faction data.
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

/*
 * Reads in PKill and PDeath still for backward compatibility but now it
 * should be written to PKillRange and PDeathRange for multiple level pkill
 * tracking support. --Shaddai
 * Added a hardcoded limit memlimit to the amount of members a faction can 
 * have set using setfaction.  --Shaddai
 */

void fread_faction( FACTION_DATA *faction, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;

    faction->mem_limit = 0;  /* Set up defaults */
    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Alignment",	faction->alignment,	fread_number( fp ) );
	    break;

	case 'B':
            KEY( "Badge",       faction->badge,		fread_string( fp ) );
	    KEY( "Board",	faction->board,		fread_number( fp ) );
	    break;

	case 'C':
	    KEY( "ObjOne",	faction->obj1,		fread_number( fp ) );
	    KEY( "ObjTwo",	faction->obj2,		fread_number( fp ) );
	    KEY( "ObjThree",faction->obj3,		fread_number( fp ) );
            KEY( "ObjFour", faction->obj4,		fread_number( fp ) );
            KEY( "ObjFive", faction->obj5,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Deity",	faction->deity,		fread_string( fp ) );
	    KEY( "Description",	faction->description,	fread_string( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!faction->name)
		  faction->name		= STRALLOC( "" );
		if (!faction->leader)
		  faction->leader	= STRALLOC( "" );
		if (!faction->description)
		  faction->description 	= STRALLOC( "" );
		if (!faction->motto)
		  faction->motto	= STRALLOC( "" );
		if (!faction->number1)
		  faction->number1	= STRALLOC( "" );
		if (!faction->number2)
		  faction->number2	= STRALLOC( "" );
		if (!faction->deity)
		  faction->deity	= STRALLOC( "" );
		if (!faction->badge)
	  	  faction->badge	= STRALLOC( "" );
		if (!faction->leadrank)
		  faction->leadrank	= STRALLOC( "" );
		if (!faction->onerank)
		  faction->onerank	= STRALLOC( "" );
		if (!faction->tworank)
		  faction->tworank	= STRALLOC( "" );
		return;
	    }
	    break;
	    
	case 'F':
	    KEY( "Favour",	faction->favour,	fread_number( fp ) );
	    KEY( "Filename",	faction->filename,	fread_string_nohash( fp ) );
	    KEY( "Flags",	faction->flags,		fread_bitvector( fp ) );

	case 'I':
	    KEY( "IllegalPK",	faction->illegal_pk,	fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "Leader",	faction->leader,	fread_string( fp ) );
	    KEY( "Leadrank",	faction->leadrank,	fread_string( fp ) );
	    break;

	case 'M':
	    KEY( "MDeaths",	faction->mdeaths,	fread_number( fp ) );
	    KEY( "Members",	faction->members,	fread_number( fp ) );
	    KEY( "MemLimit",	faction->mem_limit,	fread_number( fp ) );
	    KEY( "MKills",	faction->mkills,	fread_number( fp ) );
	    KEY( "Motto",	faction->motto,		fread_string( fp ) );
	    break;
 
	case 'N':
	    KEY( "Name",	faction->name,		fread_string( fp ) );
	    KEY( "NumberOne",	faction->number1,	fread_string( fp ) );
	    KEY( "NumberTwo",	faction->number2,	fread_string( fp ) );
	    break;

	case 'O':
	    KEY( "Onerank",	faction->onerank,	fread_string( fp ) );
	    break;

	case 'P':
	    KEY( "PDeaths",	faction->pdeaths[6],	fread_number( fp ) );
	    KEY( "PKills",	faction->pkills[6],	fread_number( fp ) );
	    /* Addition of New Ranges */
	    if ( !str_cmp ( word, "PDeathRange" ) )
	    {
		fMatch = TRUE;
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
            }
	    if ( !str_cmp ( word, "PDeathRangeNew" ) )
            {
		fMatch = TRUE;
		faction->pdeaths[0] = fread_number( fp );
		faction->pdeaths[1] = fread_number( fp );
		faction->pdeaths[2] = fread_number( fp );
		faction->pdeaths[3] = fread_number( fp );
		faction->pdeaths[4] = fread_number( fp );
		faction->pdeaths[5] = fread_number( fp );
		faction->pdeaths[6] = fread_number( fp );
	    }
	    if ( !str_cmp ( word, "PKillRangeNew" ) )
            {
		fMatch = TRUE;
		faction->pkills[0] = fread_number( fp );
		faction->pkills[1] = fread_number( fp );
		faction->pkills[2] = fread_number( fp );
		faction->pkills[3] = fread_number( fp );
		faction->pkills[4] = fread_number( fp );
		faction->pkills[5] = fread_number( fp );
		faction->pkills[6] = fread_number( fp );
	    }
	    if ( !str_cmp ( word, "PKillRange" ) )
	    {
		fMatch = TRUE;
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
		fread_number( fp );
            }
	    break;

	case 'R':
	    KEY( "Recall",	faction->recall,	fread_number( fp ) );
	    break;

	case 'S':
	    KEY( "Score",	faction->score,		fread_number( fp ) );
	    KEY( "Strikes",	faction->strikes,	fread_number( fp ) );
	    KEY( "Storeroom",	faction->storeroom,	fread_number( fp ) );
	    break;

	case 'T':
	    KEY( "Tworank",	faction->tworank,	fread_string( fp ) );
	    KEY( "Type",	faction->faction_type,	fread_number( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_faction: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

/*
 * Read in actual council data.
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

void fread_council( COUNCIL_DATA *council, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'B':
	    KEY( "Board",	council->board,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Description",	council->description,	fread_string( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!council->name)
		  council->name		= STRALLOC( "" );
		if (!council->description)
		  council->description 	= STRALLOC( "" );
		if (!council->powers)
		  council->powers	= STRALLOC( "" );
		return;
	    }
	    break;
	    
	case 'F':
	    KEY( "Filename",	council->filename,	fread_string_nohash( fp ) );
  	    break;

	case 'H':
	    KEY( "Head", 	council->head, 		fread_string( fp ) );
            KEY ("Head2", 	council->head2, 	fread_string( fp ) );
	    break;

	case 'M':
	    KEY( "Members",	council->members,	fread_number( fp ) );
	    KEY( "Meeting",   	council->meeting, 	fread_number( fp ) );
	    break;
 
	case 'N':
	    KEY( "Name",	council->name,		fread_string( fp ) );
	    break;

	case 'P':
	    KEY( "Powers",	council->powers,	fread_string( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_council: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}


/*
 * Load a faction file
 */

bool load_faction_file( char *factionfile )
{
    char filename[256];
    FACTION_DATA *faction;
    FILE *fp;
    bool found;

    CREATE( faction, FACTION_DATA, 1 );

    /* Make sure we default these to 0 --Shaddai */
    faction->pkills[0] = 0;
    faction->pkills[1] = 0;
    faction->pkills[2] = 0;
    faction->pkills[3] = 0;
    faction->pkills[4] = 0;
    faction->pkills[5] = 0;
    faction->pkills[6] = 0;
    faction->pdeaths[0]= 0;
    faction->pdeaths[1]= 0;
    faction->pdeaths[2]= 0;
    faction->pdeaths[3]= 0;
    faction->pdeaths[4]= 0;
    faction->pdeaths[5]= 0;
    faction->pdeaths[6]= 0;

    found = FALSE;
    sprintf( filename, "%s%s", FACTION_DIR, factionfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_faction_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "FACTION"	) )
	    {
	    	fread_faction( faction, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_faction_file: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	FCLOSE( fp );
    }

    if ( found )
    {
	ROOM_INDEX_DATA *storeroom;

	LINK( faction, first_faction, last_faction, next, prev );

	if ( faction->storeroom == 0
	|| (storeroom = get_room_index( faction->storeroom )) == NULL )
	{
	    log_string( "Storeroom not found" );
	    return found;
	}
	
	sprintf( filename, "%s%s.vault", FACTION_DIR, faction->filename );
	if ( ( fp = fopen( filename, "r" ) ) != NULL )
	{
	    int iNest;
	    bool found;
	    OBJ_DATA *tobj, *tobj_next;

	    log_string( "Loading faction storage room" );
	    rset_supermob(storeroom);
	    for ( iNest = 0; iNest < MAX_NEST; iNest++ )
		rgObjNest[iNest] = NULL;

	    found = TRUE;
	    for ( ; ; )
	    {
		char letter;
		char *word;

		letter = fread_letter( fp );
		if ( letter == '*' )
		{
		    fread_to_eol( fp );
		    continue;
		}

		if ( letter != '#' )
		{
		    bug( "Load_faction_vault: # not found.", 0 );
		    bug( faction->name, 0 );
		    break;
		}

		word = fread_word( fp );
		if ( !str_cmp( word, "OBJECT" ) )	/* Objects	*/
		  fread_obj  ( supermob, fp, OS_CARRY );
		else
		if ( !str_cmp( word, "END"    ) )	/* Done		*/
		  break;
		else
		{
		    bug( "Load_faction_vault: bad section.", 0 );
		    bug( faction->name, 0 );
		    break;
		}
	    }
	    FCLOSE( fp );
	    for ( tobj = supermob->first_carrying; tobj; tobj = tobj_next )
	    {
		tobj_next = tobj->next_content;
		obj_from_char( tobj );
		obj_to_room( tobj, storeroom );
	    }
	    release_supermob();
	}
	else
	    log_string( "Cannot open faction vault" );
    }
    else
      DISPOSE( faction );

    return found;
}

/*
 * Load a council file
 */

bool load_council_file( char *councilfile )
{
    char filename[256];
    COUNCIL_DATA *council;
    FILE *fp;
    bool found;

    CREATE( council, COUNCIL_DATA, 1 );

    found = FALSE;
    sprintf( filename, "%s%s", COUNCIL_DIR, councilfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_council_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "COUNCIL"	) )
	    {
	    	fread_council( council, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		bug( "Load_council_file: bad section.", 0 );
		break;
	    }
	}
	FCLOSE( fp );
    }

    if ( found )
      LINK( council, first_council, last_council, next, prev );

    else
      DISPOSE( council );

    return found;
}

/*
 * Load in all the faction files.
 */
void load_factions( )
{
    FILE *fpList;
    char *filename;
    char factionlist[256];
    char buf[MAX_STRING_LENGTH];
    
    
    first_faction	= NULL;
    last_faction	= NULL;

    log_string( "Loading factions..." );

    sprintf( factionlist, "%s%s", FACTION_DIR, FACTION_LIST );
    FCLOSE( fpReserve );
    if ( ( fpList = fopen( factionlist, "r" ) ) == NULL )
    {
	perror( factionlist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	log_string( filename );
	if ( filename[0] == '$' )
	  break;

	if ( !load_faction_file( filename ) )
	{
	  sprintf( buf, "Cannot load faction file: %s", filename );
	  bug( buf, 0 );
	}
    }
    FCLOSE( fpList );
    log_string(" Done faction " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
 * Load in all the council files.
 */
void load_councils( )
{
    FILE *fpList;
    char *filename;
    char councillist[256];
    char buf[MAX_STRING_LENGTH];
    
    
    first_council	= NULL;
    last_council	= NULL;

    log_string( "Loading councils..." );

    sprintf( councillist, "%s%s", COUNCIL_DIR, COUNCIL_LIST );
    FCLOSE( fpReserve );
    if ( ( fpList = fopen( councillist, "r" ) ) == NULL )
    {
	perror( councillist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	log_string( filename );
	if ( filename[0] == '$' )
	  break;

	if ( !load_council_file( filename ) )
	{
	  sprintf( buf, "Cannot load council file: %s", filename );
	  bug( buf, 0 );
	}
    }
    FCLOSE( fpList );
    log_string(" Done councils " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_make( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    FACTION_DATA *faction;
    int level;

    if ( IS_NPC( ch ) || !ch->pcdata->faction )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    faction = ch->pcdata->faction;
    
    if ( str_cmp( ch->name, faction->leader )
    &&   str_cmp( ch->name, faction->deity )
    &&  (faction->faction_type != FACTION_GUILD
    ||   str_cmp( ch->name, faction->number1 )) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Make what?\n\r", ch );
	return;
    }

    pObjIndex = get_obj_index( faction->obj1 );
    level = 40;

    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( faction->obj2 );
      level = 45;
    }
    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( faction->obj3 );
      level = 50;
    }
    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( faction->obj4 );
      level = 35;
    }
    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
      pObjIndex = get_obj_index( faction->obj5 );
      level = 1;
    }

    if ( !pObjIndex || !is_name( arg, pObjIndex->name ) )
    {
	send_to_char( "You don't know how to make that.\n\r", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    xSET_BIT( obj->extra_flags, ITEM_CLANOBJECT );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
      obj = obj_to_char( obj, ch );
    else
      obj = obj_to_room( obj, ch->in_room );
    act( AT_MAGIC, "$n makes $p!", ch, obj, NULL, TO_ROOM );
    act( AT_MAGIC, "You make $p!", ch, obj, NULL, TO_CHAR );
    return;
}

void do_induct( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    FACTION_DATA *faction;

    if ( IS_NPC( ch ) || !ch->pcdata->faction )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    faction = ch->pcdata->faction;
    
    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("induct", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, faction->deity   )
    ||   !str_cmp( ch->name, faction->leader  )
    ||   !str_cmp( ch->name, faction->number1 )
    ||   !str_cmp( ch->name, faction->number2 ) )
	;
    else
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Induct whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) )
    {
	send_to_char( "You can't induct such a godly presence.\n\r", ch );
	return;
    }

    if ( check_factionflags( victim, faction ) == TRUE )
    {
	ch_printf( ch, "For some reason you're biased against %s...\n\r", victim->name );
	return;
    }

    if ( victim->level < 10 )
    {
	send_to_char( "This player is not worthy of joining yet.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && victim->pcdata->faction )
    {
	if ( victim->pcdata->faction == faction )
	    ch_printf( ch, "This player already belongs to your %s!\n\r", victim->pcdata->faction->faction_type == FACTION_GUILD ? "guild" : victim->pcdata->faction->faction_type == FACTION_ORDER ? "order" : "clan" );
	else
	    ch_printf( ch, "This player already belongs to %s!\n\r", victim->pcdata->faction->faction_type == FACTION_GUILD ? "a guild" : victim->pcdata->faction->faction_type == FACTION_ORDER ? "an order" : "a clan" );
	return;
    }

    if ( faction->mem_limit && faction->members >= faction->mem_limit )
    {
    	ch_printf( ch, "Your %s is too big to induct anyone else.\n\r", faction->faction_type == FACTION_GUILD ? "guild" : faction->faction_type == FACTION_ORDER ? "order" : "clan" );
	return;
    }
    faction->members++;
    if ( faction->faction_type != FACTION_ORDER && faction->faction_type != FACTION_GUILD )
      SET_BIT(victim->speaks, LANG_CLAN);

    if ( faction->faction_type != FACTION_GUILD && faction->faction_type != FACTION_ORDER )
      SET_BIT( victim->pcdata->flags, PCFLAG_DEADLY );

    victim->pcdata->faction = faction;
    STRFREE(victim->pcdata->faction_name);
    victim->pcdata->faction_name = QUICKLINK( faction->name );
    act( AT_MAGIC, "You induct $N into $t", ch, faction->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n inducts $N into $t", ch, faction->name, victim, TO_NOTVICT );
    act( AT_MAGIC, "$n inducts you into $t", ch, faction->name, victim, TO_VICT );
    save_char_obj( victim );
    save_faction( faction );
    return;
}

void do_council_induct( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    COUNCIL_DATA *council;

    if ( IS_NPC( ch ) || !ch->pcdata->council )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    council = ch->pcdata->council;
    
  if ((council->head == NULL || str_cmp (ch->name, council->head))
      && ( council->head2 == NULL || str_cmp ( ch->name, council->head2 ))
      && str_cmp (council->name, "mortal council"))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Induct whom into your council?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim->pcdata->council )
    {
	send_to_char( "This player already belongs to a council!\n\r", ch );
	return;
    }

    council->members++;
    victim->pcdata->council = council;
    STRFREE(victim->pcdata->council_name);
    victim->pcdata->council_name = QUICKLINK( council->name );
    act( AT_MAGIC, "You induct $N into $t", ch, council->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n inducts $N into $t", ch, council->name, victim, TO_ROOM );
    act( AT_MAGIC, "$n inducts you into $t", ch, council->name, victim, TO_VICT );
    save_char_obj( victim );
    save_council( council );
    return;
}

void do_outcast( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    FACTION_DATA *faction;
    char buf[MAX_STRING_LENGTH];
    int vic_value = 0, ch_value = 0;

    if ( IS_NPC( ch ) || !ch->pcdata->faction )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    faction	= ch->pcdata->faction;

    if ( (ch->pcdata && ch->pcdata->bestowments
    &&    is_name("outcast", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, faction->deity   )
    ||   !str_cmp( ch->name, faction->leader  )
    ||   !str_cmp( ch->name, faction->number1 )
    ||   !str_cmp( ch->name, faction->number2 ) )
	;
    else
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }


    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Outcast whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	ch_printf( ch, "Kick yourself out of your own %s?\n\r", faction->faction_type == FACTION_GUILD ? "guild" : faction->faction_type == FACTION_ORDER ? "order" : "clan" );
	return;
    }
 
    if ( victim->pcdata->faction != ch->pcdata->faction )
    {
	ch_printf( ch, "They do not belong to your %s!\n\r", faction->faction_type == FACTION_GUILD ? "guild" : faction->faction_type == FACTION_ORDER ? "order" : "clan" );
	return;
    }

    if (!str_cmp (ch->name, faction->deity))
        ch_value = 4;
    else if (!str_cmp (ch->name, faction->leader))
        ch_value = 3;
    else if (!str_cmp (ch->name, faction->number1))
        ch_value = 2;
    else if (!str_cmp (ch->name, faction->number2))
        ch_value = 1;
    else
        ch_value = 0;

    if (!str_cmp (victim->name, faction->deity))
        vic_value = 4;
    else if (!str_cmp (victim->name, faction->leader))
        vic_value = 3;
    else if (!str_cmp (victim->name, faction->number1))
        vic_value = 2;
    else if (!str_cmp (victim->name, faction->number2))
        vic_value = 1;
    else
        vic_value = 0;

    /*
     * Outcast fix. -Orion
     */
    if ( ch_value <= vic_value )
    {
        ch_printf( ch, "You can't outcast your %s!\n\r", ch_value < vic_value ? "superior" : "equal" );
        return;
    }

    if ( victim->speaking & LANG_CLAN )
        victim->speaking = LANG_COMMON;
    REMOVE_BIT( victim->speaks, LANG_CLAN );
    --faction->members;
    if ( !str_cmp( victim->name, faction->number1 ) )
    {
	STRFREE( faction->number1 );
	faction->number1 = STRALLOC( "" );
    }
    if ( !str_cmp( victim->name, faction->number2 ) )
    {
	STRFREE( faction->number2 );
	faction->number2 = STRALLOC( "" );
    }
    victim->pcdata->faction = NULL;
    STRFREE(victim->pcdata->faction_name);
    victim->pcdata->faction_name = STRALLOC( "" );
    act( AT_MAGIC, "You outcast $N from $t", ch, faction->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n outcasts $N from $t", ch, faction->name, victim, TO_ROOM );
    act( AT_MAGIC, "$n outcasts you from $t", ch, faction->name, victim, TO_VICT );
    if ( faction->faction_type != FACTION_GUILD && faction->faction_type != FACTION_ORDER )
    {
	sprintf(buf, "%s has been outcast from %s!", victim->name, faction->name);
	echo_to_all(AT_MAGIC, buf, ECHOTAR_ALL);
    }
    save_char_obj( victim );	/* faction gets saved when pfile is saved */
    save_faction( faction );
    return;
}

void do_resign( CHAR_DATA *ch, char *argument )
{
    FACTION_DATA *faction;

    if ( IS_NPC(ch) || !ch->pcdata->faction )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    faction =  ch->pcdata->faction;

    /*
     * Leaders are appointed by Imms, they can be removed by Imms. -Orion
     */
    if ( !str_cmp( ch->name, faction->leader ) )
    {
	ch_printf( ch, "You can't resign from %s ... you are the leader!\n\r", faction->name );
	return;
    }

    if ( ch->speaking & LANG_CLAN )
	ch->speaking = LANG_COMMON;
    REMOVE_BIT( ch->speaks, LANG_CLAN );
    --faction->members;

    if ( !str_cmp( ch->name, faction->number1 ) )
    {
	STRFREE( faction->number1 );
	faction->number1 = STRALLOC( "" );
    }
    if ( !str_cmp( ch->name, faction->number2 ) )
    {
	STRFREE( faction->number2 );
	faction->number2 = STRALLOC( "" );
    }

    ch->pcdata->faction = NULL;
    ch->pcdata->faction = NULL;
    STRFREE(ch->pcdata->faction_name);
    ch->pcdata->faction_name = STRALLOC( "" );

    act( AT_MAGIC, "You resign from $t", ch, faction->name, NULL, TO_CHAR );
    act( AT_MAGIC, "$n resigns from $t", ch, faction->name, NULL, TO_ROOM );
    save_char_obj( ch );
    save_faction( faction );
    return;
}

void do_council_outcast( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    COUNCIL_DATA *council;

    if ( IS_NPC( ch ) || !ch->pcdata->council )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    council = ch->pcdata->council;

  if ((council->head == NULL || str_cmp (ch->name, council->head))
      && ( council->head2 == NULL || str_cmp ( ch->name, council->head2 ))
      && str_cmp (council->name, "mortal council"))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Outcast whom from your council?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Kick yourself out of your own council?\n\r", ch );
	return;
    }
 
    if ( victim->pcdata->council != ch->pcdata->council )
    {
	send_to_char( "This player does not belong to your council!\n\r", ch );
	return;
    }

    --council->members;
    victim->pcdata->council = NULL;
    STRFREE(victim->pcdata->council_name);
    victim->pcdata->council_name = STRALLOC( "" );
    act( AT_MAGIC, "You outcast $N from $t", ch, council->name, victim, TO_CHAR );
    act( AT_MAGIC, "$n outcasts $N from $t", ch, council->name, victim, TO_ROOM );
    act( AT_MAGIC, "$n outcasts you from $t", ch, council->name, victim, TO_VICT );
    save_char_obj( victim );
    save_council( council );
    return;
}

void do_setfaction( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    FACTION_DATA *faction;
    bool fMatch = FALSE;

    set_char_color( AT_PLAIN, ch );
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Usage: setfaction <faction> <field> <deity|leader|number1|number2> <player>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( " deity leader number1 number2\n\r", ch ); 
	send_to_char( " members board recall storage\n\r", ch );
	send_to_char( " align (not functional) memlimit\n\r", ch );
	send_to_char( " leadrank onerank tworank\n\r", ch );
	send_to_char( " obj1 obj2 obj3 obj4 obj5\n\r", ch );
	if ( get_trust( ch ) >= LEVEL_WORKER )
	{
	  send_to_char( " name filename motto desc\n\r", ch );
	  send_to_char( " favour strikes type class\n\r", ch );
	}
	if ( get_trust( ch ) >= LEVEL_MASTER )
	  send_to_char(" pkill1-7 pdeath1-7\n\r", ch );
	return;
    }

    faction = get_faction( arg1 );
    if ( !faction )
    {
	send_to_char( "No such faction.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "deity" ) )
    {
	STRFREE( faction->deity );
	faction->deity = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "leader" ) )
    {
	STRFREE( faction->leader );
	faction->leader = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "number1" ) )
    {
	STRFREE( faction->number1 );
	faction->number1 = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "number2" ) )
    {
	STRFREE( faction->number2 );
	faction->number2 = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "leadrank" ) )
    {
	STRFREE( faction->leadrank );
	faction->leadrank = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "onerank" ) )
    {
	STRFREE( faction->onerank );
	faction->onerank = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "tworank" ) )
    {
	STRFREE( faction->tworank );
	faction->tworank = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "badge" ) )
    {
	STRFREE( faction->badge );
	faction->badge = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "board" ) )
    {
	faction->board = atoi( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "memlimit") )
    {
    	faction->mem_limit = atoi( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "members" ) )
    {
	faction->members = atoi( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "recall" ) )
    {
	faction->recall = atoi( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "storage" ) )
    {
	faction->storeroom = atoi( argument );
	fMatch = TRUE;
    }
    else if ( !str_prefix( arg2, "obj" ) )
    {
	if ( !str_cmp( arg2, "obj1" ) )
	{
	    faction->obj1 = atoi( argument );
	    fMatch = TRUE;
	}
	else if ( !str_cmp( arg2, "obj2" ) )
	{
	    faction->obj2 = atoi( argument );
	    fMatch = TRUE;
	}
	else if ( !str_cmp( arg2, "obj3" ) )
	{
	    faction->obj3 = atoi( argument );
	    fMatch = TRUE;
	}
	else if ( !str_cmp( arg2, "obj4" ) )
	{
	    faction->obj4 = atoi( argument );
	    fMatch = TRUE;
	}
	else if ( !str_cmp( arg2, "obj5" ) )
	{
	    faction->obj5 = atoi( argument );
	    fMatch = TRUE;
	}
    }

    /*
     * Gets this down to one section, instead of tons of instances. -Orion
     */
    if ( fMatch == TRUE )
    {
	send_to_char( "Done.\n\r", ch );
	save_faction( faction );
	return;
    }

    if ( get_trust( ch ) < LEVEL_WORKER )
    {
	do_setfaction( ch, "" );
	return;
    }
    else if ( !str_cmp( arg2, "align" ) )
    {
	faction->alignment = atoi( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "type" ) )
    {
	if ( !str_cmp( argument, "order" ) )
	    faction->faction_type = FACTION_ORDER;
	else if ( !str_cmp( argument, "guild" ) )
	    faction->faction_type = FACTION_GUILD;
	else if ( !str_cmp( argument, "clan" ) )
	    faction->faction_type = FACTION_CLAN;
	else
	    faction->faction_type = FACTION_CLAN;
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "flags" ) )
    {
	char factionflag[MAX_STRING_LENGTH];

	while ( argument[0] != '\0' )
	{
	    int value;

	    argument = one_argument( argument, factionflag );

	    value = get_factionflag( factionflag );

	    if ( value < 0 || value > MAX_BITS )
	    {
		ch_printf( ch, "Unknown flag: %s\n\r", factionflag );
		return;
	    }
	    xTOGGLE_BIT( faction->flags, value );
	}
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "name" ) )
    {
	STRFREE( faction->name );
	faction->name = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "filename" ) )
    {
	DISPOSE( faction->filename );
	faction->filename = str_dup( argument );
	write_faction_list( );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "motto" ) )
    {
	STRFREE( faction->motto );
	faction->motto = STRALLOC( argument );
	fMatch = TRUE;
    }
    else if ( !str_cmp( arg2, "desc" ) )
    {
	STRFREE( faction->description );
	faction->description = STRALLOC( argument );
	fMatch = TRUE;
    }

    /*
     * Gets this down to one section, instead of tons of instances. -Orion
     */
    if ( fMatch == TRUE )
    {
	send_to_char( "Done.\n\r", ch );
	save_faction( faction );
	return;
    }

    if ( get_trust( ch ) < LEVEL_MASTER )
    {
        do_setfaction( ch, "" );
        return;
    }
    if ( !str_prefix( "pkill", arg2) )
    {
	int temp_value;
	if ( !str_cmp( arg2, "pkill1" ) )
		temp_value = 0;
	else if ( !str_cmp( arg2, "pkill2" ) )
		temp_value = 1;
	else if ( !str_cmp( arg2, "pkill3" ) )
		temp_value = 2;
	else if ( !str_cmp( arg2, "pkill4" ) )
		temp_value = 3;
	else if ( !str_cmp( arg2, "pkill5" ) )
		temp_value = 4;
	else if ( !str_cmp( arg2, "pkill6" ) )
		temp_value = 5;
	else if ( !str_cmp( arg2, "pkill7" ) )
		temp_value = 6;
	else
	{
		do_setfaction( ch, "" );
		return;
	}
	faction->pkills[temp_value] = atoi( argument );
	fMatch = TRUE;
    }
    if ( !str_prefix( "pdeath", arg2) )
    {
	int temp_value;
	if ( !str_cmp( arg2, "pdeath1" ) )
		temp_value = 0;
	else if ( !str_cmp( arg2, "pdeath2" ) )
		temp_value = 1;
	else if ( !str_cmp( arg2, "pdeath3" ) )
		temp_value = 2;
	else if ( !str_cmp( arg2, "pdeath4" ) )
		temp_value = 3;
	else if ( !str_cmp( arg2, "pdeath5" ) )
		temp_value = 4;
	else if ( !str_cmp( arg2, "pdeath6" ) )
		temp_value = 5;
	else if ( !str_cmp( arg2, "pdeath7" ) )
		temp_value = 6;
	else
	{
		do_setfaction( ch, "" );
		return;
	}
	faction->pdeaths[temp_value] = atoi( argument );
	fMatch = TRUE;
    }

    /*
     * Gets this down to one section, instead of tons of instances. -Orion
     */
    if ( fMatch == TRUE )
    {
	send_to_char( "Done.\n\r", ch );
	save_faction( faction );
	return;
    }
    else
	do_setfaction( ch, "" );

    return;
}

void do_setcouncil( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    COUNCIL_DATA *council;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Usage: setcouncil <council> <field> <deity|leader|number1|number2> <player>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( " head head2 members board meeting\n\r", ch ); 
	if ( get_trust( ch ) >= LEVEL_WORKER )
	  send_to_char( " name filename desc\n\r", ch );
        if ( get_trust( ch ) >= LEVEL_MASTER )
	  send_to_char( " powers\n\r", ch);
	return;
    }

    council = get_council( arg1 );
    if ( !council )
    {
	send_to_char( "No such council.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "head" ) )
    {
	STRFREE( council->head );
	council->head = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_council( council );
	return;
    }

  if (!str_cmp (arg2, "head2"))
    {
      if ( council->head2 != NULL )
        STRFREE (council->head2);
      if ( !str_cmp ( argument, "none" ) || !str_cmp ( argument, "clear" ) )
        council->head2 = NULL;
      else
        council->head2 = STRALLOC (argument);
      send_to_char ("Done.\n\r", ch);
      save_council (council);
      return;
    }
    if ( !str_cmp( arg2, "board" ) )
    {
	council->board = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_council( council );
	return;
    }
    if ( !str_cmp( arg2, "members" ) )
    {
	council->members = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_council( council );
	return;
    }
    if ( !str_cmp( arg2, "meeting" ) )
    {
	council->meeting = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_council( council );
	return;
    }
    if ( get_trust( ch ) < LEVEL_WORKER )
    {
	do_setcouncil( ch, "" );
	return;
    }
    if ( !str_cmp( arg2, "name" ) )
    {
	STRFREE( council->name );
	council->name = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_council( council );
	return;
    }
    if ( !str_cmp( arg2, "filename" ) )
    {

      if ( council->filename && council->filename[0] != '\0' )
                DISPOSE( council->filename );
	council->filename = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_council( council );
	write_council_list( );
	return;
    }
    if ( !str_cmp( arg2, "desc" ) )
    {
	STRFREE( council->description );
	council->description = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_council( council );
	return;
    }
    if ( get_trust( ch ) < LEVEL_MASTER )
    {
	do_setcouncil( ch, "" );
	return;
    }
    if ( !str_cmp( arg2, "powers" ) )
    {
	STRFREE( council->powers );
	council->powers = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_council( council );
	return;
    }
    
    do_setcouncil( ch, "" );
    return;
}

/*
 * Added multiple levels on pkills and pdeaths. -- Shaddai
 */

void do_showfaction( CHAR_DATA *ch, char *argument )
{   
    FACTION_DATA *faction;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if ( argument[0] == '\0' )
    {
	send_to_char( "Usage: showfaction <faction>\n\r", ch );
	return;
    }

    faction = get_faction( argument );
    if ( !faction )
    {
	send_to_char( "No such clan, guild or order.\n\r", ch );
	return;
    }

    ch_printf_color( ch, "\n\r&w%s    : &W%s\t\t&wBadge: %s\n\r&wFilename : &W%s\n\r&wMotto    : &W%s\n\r",
			faction->faction_type == FACTION_ORDER ? "Order" :
		        faction->faction_type == FACTION_GUILD ? "Guild" : "Clan ",
    			faction->name,
			faction->badge ? faction->badge : "(not set)",
    			faction->filename,
    			faction->motto );
    ch_printf_color( ch, "&wDesc     : &W%s\n\r&wDeity    : &W%s\n\r",
    			faction->description,
    			faction->deity );
    ch_printf_color( ch, "&wLeader   : &W%-19.19s\t&wRank: &W%s\n\r",
			faction->leader,
			faction->leadrank );
    ch_printf_color( ch, "&wNumber1  : &W%-19.19s\t&wRank: &W%s\n\r",
			faction->number1,
			faction->onerank );
    ch_printf_color( ch, "&wNumber2  : &W%-19.19s\t&wRank: &W%s\n\r",
			faction->number2,
			faction->tworank );
    ch_printf_color( ch, "&wPKills   : &w1-9:&W%-3d &w10-14:&W%-3d &w15-19:&W%-3d &w20-29:&W%-3d &w30-39:&W%-3d &w40-49:&W%-3d &w50:&W%-3d\n\r",  
    			faction->pkills[0], faction->pkills[1], faction->pkills[2],
    			faction->pkills[3], faction->pkills[4], faction->pkills[5],
			faction->pkills[6]);
    ch_printf_color( ch, "&wPDeaths  : &w1-9:&W%-3d &w10-14:&W%-3d &w15-19:&W%-3d &w20-29:&W%-3d &w30-39:&W%-3d &w40-49:&W%-3d &w50:&W%-3d\n\r",  
    			faction->pdeaths[0], faction->pdeaths[1], faction->pdeaths[2],
    			faction->pdeaths[3], faction->pdeaths[4], faction->pdeaths[5],
			faction->pdeaths[6] );
    ch_printf_color( ch, "&wIllegalPK: &W%-6d\n\r",
			faction->illegal_pk );
    ch_printf_color( ch, "&wMKills   : &W%-6d   &wMDeaths: &W%-6d\n\r",
    			faction->mkills,
    			faction->mdeaths );
    ch_printf_color( ch, "&wScore    : &W%-6d   &wFavor  : &W%-6d   &wStrikes: &W%d\n\r",
    			faction->score,
    			faction->favour,
    			faction->strikes );
    ch_printf_color( ch, "&wMembers  : &W%-6d  &wMemLimit: &W%-6d   &wAlign  : &W%-6d",
    			faction->members,
    			faction->mem_limit,
    			faction->alignment );
    send_to_char( "\n\r", ch );
    ch_printf_color( ch, "&wBoard    : &W%-5d    &wRecall : &W%-5d    &wStorage: &W%-5d\n\r",
			faction->board,
			faction->recall,
			faction->storeroom ); 
    ch_printf_color( ch, "&wObj1( &W%d &w)  Obj2( &W%d &w)  Obj3( &W%d &w)  Obj4( &W%d &w)  Obj5( &W%d &w)\n\r",
    			faction->obj1,
    			faction->obj2,
    			faction->obj3,
			faction->obj4,
			faction->obj5 );
    ch_printf_color( ch, "&wFlags    : &W%s\n\r", ext_flag_string( &faction->flags, faction_flag ) );
    return;
}

void do_showcouncil( CHAR_DATA *ch, char *argument )
{
    COUNCIL_DATA *council;

    set_char_color( AT_PLAIN, ch );

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if ( argument[0] == '\0' )
    {
	send_to_char( "Usage: showcouncil <council>\n\r", ch );
	return;
    }

    council = get_council( argument );
    if ( !council )
    {
	send_to_char( "No such council.\n\r", ch );
	return;
    }

    ch_printf_color( ch, "\n\r&wCouncil :  &W%s\n\r&wFilename:  &W%s\n\r",
    			council->name,
    			council->filename );
  ch_printf_color (ch, "&wHead:      &W%s\n\r", council->head );
  ch_printf_color (ch, "&wHead2:     &W%s\n\r", council->head2 );
  ch_printf_color (ch, "&wMembers:   &W%-d\n\r", council->members );
    ch_printf_color( ch, "&wBoard:     &W%-5d\n\r&wMeeting:   &W%-5d\n\r&wPowers:    &W%s\n\r",
    			council->board,
    			council->meeting,
			council->powers );
    ch_printf_color( ch, "&wDescription:\n\r&W%s\n\r", council->description );
    return;
}

void do_makefaction( CHAR_DATA *ch, char *argument )
{
    char filename[256];
    FACTION_DATA *faction;
    bool found;

    set_char_color( AT_IMMORT, ch );

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makefaction <faction name>\n\r", ch );
	return;
    }

    found = FALSE;
    sprintf( filename, "%s%s", FACTION_DIR, strlower(argument) );

    CREATE( faction, FACTION_DATA, 1 );
    LINK( faction, first_faction, last_faction, next, prev );

    faction->name		= STRALLOC( argument );
    faction->filename		= STRALLOC( "" ); /*Bug fix by baria@mud.tander.com*/
    faction->motto		= STRALLOC( "" );
    faction->description	= STRALLOC( "" );
    faction->deity		= STRALLOC( "" );
    faction->leader		= STRALLOC( "" );
    faction->number1		= STRALLOC( "" );
    faction->number2		= STRALLOC( "" );
    faction->leadrank		= STRALLOC( "" );
    faction->onerank		= STRALLOC( "" );
    faction->tworank		= STRALLOC( "" );
    faction->badge		= STRALLOC( "" );
}

void do_makecouncil( CHAR_DATA *ch, char *argument )
{
    char filename[256];
    COUNCIL_DATA *council;
    bool found;

    set_char_color( AT_IMMORT, ch );

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makecouncil <council name>\n\r", ch );
	return;
    }

    found = FALSE;
    sprintf( filename, "%s%s", COUNCIL_DIR, strlower(argument) );

    CREATE( council, COUNCIL_DATA, 1 );
    LINK( council, first_council, last_council, next, prev );
    council->name		= STRALLOC( argument );
    council->head		= STRALLOC( "" );
    council->head2 		= NULL;
    council->powers		= STRALLOC( "" );
}

/*
 * Added multiple level pkill and pdeath support. --Shaddai
 */
void do_clans( CHAR_DATA *ch, char *argument )
{
    FACTION_DATA *clan;
    int count = 0;

    if ( argument[0] == '\0' )
    {
        set_char_color( AT_BLOOD, ch );
        send_to_char( "\n\rClan          Deity         Leader           Pkills:    Avatar      Other\n\r_________________________________________________________________________\n\r\n\r", ch );
        for ( clan = first_faction; clan; clan = clan->next )
	{
            if ( clan->faction_type == FACTION_CLAN )
	    {
		set_char_color( AT_GREY, ch);
		ch_printf( ch, "%-13s %-13s %-13s", clan->name, clan->deity, clan->leader );
		set_char_color( AT_BLOOD, ch );
		ch_printf( ch, "                %5d      %5d\n\r", clan->pkills[6], (clan->pkills[2]+clan->pkills[3]+ clan->pkills[4]+clan->pkills[5]) );
		count++;
	    }
	}
        set_char_color( AT_BLOOD, ch );
        if ( !count )
          send_to_char( "There are no Clans currently formed.\n\r", ch );
        else
          send_to_char( "_________________________________________________________________________\n\r\n\rUse 'clans <clan>' for detailed information and a breakdown of victories.\n\r", ch );
        return;
    }

    clan = get_faction( argument );
    if ( !clan || clan->faction_type != FACTION_CLAN )
    {
        set_char_color( AT_BLOOD, ch );
        send_to_char( "No such clan.\n\r", ch );
        return;
    }
    set_char_color( AT_BLOOD, ch );
    ch_printf( ch, "\n\r%s, '%s'\n\r\n\r", clan->name, clan->motto );
    set_char_color( AT_GREY, ch );
    send_to_char_color( "Victories:&w\n\r", ch );
   ch_printf_color( ch, "    &w15-19...  &r%-4d\n\r    &w20-29...  &r%-4d\n\r    &w30-39...  &r%-4d\n\r    &w40-49...  &r%-4d\n\r",
	clan->pkills[2],
	clan->pkills[3],
	clan->pkills[4],
	clan->pkills[5] );
    ch_printf_color( ch, "   &wAvatar...  &r%-4d\n\r", 
	clan->pkills[6] );
    set_char_color( AT_GREY, ch );
    ch_printf( ch, "Clan Leader:  %s\n\rNumber One :  %s\n\rNumber Two :  %s\n\rClan Deity :  %s\n\r",
                        clan->leader,
                        clan->number1,
                        clan->number2,
			clan->deity );
    if ( !str_cmp( ch->name, clan->deity   )
    ||   !str_cmp( ch->name, clan->leader  )
    ||   !str_cmp( ch->name, clan->number1 )
    ||   !str_cmp( ch->name, clan->number2 ) )
	ch_printf( ch, "Members    :  %d\n\r",
			clan->members );
    ch_printf( ch, "Biases     :  %s\n\r", faction_bit_name( &clan->flags ) );
    set_char_color( AT_BLOOD, ch );
    ch_printf( ch, "\n\rDescription:  %s\n\r", clan->description );
    return;
}

void do_orders( CHAR_DATA *ch, char *argument )
{
    FACTION_DATA *order;
    int count = 0;

    if ( argument[0] == '\0' )
    {
        set_char_color( AT_DGREEN, ch );
        send_to_char( "\n\rOrder            Deity          Leader           Mkills      Mdeaths\n\r____________________________________________________________________\n\r\n\r", ch );
	set_char_color( AT_GREEN, ch );
        for ( order = first_faction; order; order = order->next )
	{
	    if ( order->faction_type == FACTION_ORDER )
	    {
		ch_printf( ch, "%-16s %-14s %-14s   %-7d       %5d\n\r", order->name, order->deity, order->leader, order->mkills, order->mdeaths );
		count++;
	    }
	}
        set_char_color( AT_DGREEN, ch );
	if ( !count )
	  send_to_char( "There are no Orders currently formed.\n\r", ch );
	else
	  send_to_char( "____________________________________________________________________\n\r\n\rUse 'orders <order>' for more detailed information.\n\r", ch );
	return;
    }

    order = get_faction( argument );
    if ( !order || order->faction_type != FACTION_ORDER )
    {
        set_char_color( AT_DGREEN, ch );
        send_to_char( "No such Order.\n\r", ch );
        return;
    }
 
    set_char_color( AT_DGREEN, ch );
    ch_printf( ch, "\n\rOrder of %s\n\r'%s'\n\r\n\r", order->name, order->motto );
    set_char_color( AT_GREEN, ch );
    ch_printf( ch, "Deity      :  %s\n\rLeader     :  %s\n\rNumber One :  %s\n\rNumber Two :  %s\n\r",
                        order->deity,
                        order->leader,
                        order->number1,
                        order->number2 );
    if ( !str_cmp( ch->name, order->deity   )
    ||   !str_cmp( ch->name, order->leader  )
    ||   !str_cmp( ch->name, order->number1 )
    ||   !str_cmp( ch->name, order->number2 ) )
        ch_printf( ch, "Members    :  %d\n\r",
			order->members );
    ch_printf( ch, "Biases     :  %s\n\r", faction_bit_name( &order->flags ) );
    set_char_color( AT_DGREEN, ch );
    ch_printf( ch, "\n\rDescription:\n\r%s\n\r", order->description );
    return;
}

void do_councils( CHAR_DATA *ch, char *argument)
{
    COUNCIL_DATA *council;

    set_char_color( AT_CYAN, ch );
    if ( !first_council )
    {
	send_to_char( "There are no councils currently formed.\n\r", ch );
	return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char_color ("\n\r&cTitle                    Head\n\r", ch);
      for (council = first_council; council; council = council->next)
      {
        if ( council->head2 != NULL )
           ch_printf_color (ch, "&w%-24s %s and %s\n\r",  council->name,
                council->head, council->head2 );
        else
           ch_printf_color (ch, "&w%-24s %-14s\n\r", council->name, council->head);
      }
      send_to_char_color( "&cUse 'councils <name of council>' for more detailed information.\n\r", ch );
      return;
    }        
    council = get_council( argument );
    if ( !council )
    {
      send_to_char_color( "&cNo such council exists...\n\r", ch );
      return;
    }
    ch_printf_color( ch, "&c\n\r%s\n\r", council->name );
  if ( council->head2 == NULL )
        ch_printf_color (ch, "&cHead:     &w%s\n\r&cMembers:  &w%d\n\r",
        council->head, council->members );
  else
        ch_printf_color (ch, "&cCo-Heads:     &w%s &cand &w%s\n\r&cMembers:  &w%d\n\r",
           council->head, council->head2, council->members );
    ch_printf_color( ch, "&cDescription:\n\r&w%s\n\r",
        council->description );
    return;
} 

void do_guilds( CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];  
    FACTION_DATA *guild;
    int count = 0;

    if ( argument[0] == '\0' )
    {
        set_char_color( AT_HUNGRY, ch );
        send_to_char( "\n\rGuild                  Leader             Mkills      Mdeaths\n\r_____________________________________________________________\n\r\n\r", ch );
	set_char_color( AT_YELLOW, ch );
        for ( guild = first_faction; guild; guild = guild->next )
        if ( guild->faction_type == FACTION_GUILD )
	{
	    ch_printf( ch, "%-20s   %-14s     %-6d       %6d\n\r", guild->name, guild->leader, guild->mkills, guild->mdeaths );
	    count++;
	}
        set_char_color( AT_HUNGRY, ch );
        if ( !count )
          send_to_char( "There are no Guilds currently formed.\n\r", ch );
        else
          send_to_char( "_____________________________________________________________\n\r\n\rFor specifics, guilds <group>. Example: 'guilds mercenaries'\n\r", ch );
	return;
    }

    sprintf( buf, "guild of %s", argument );
    guild = get_faction( buf );
    if ( !guild || guild->faction_type != FACTION_GUILD )
    {
        set_char_color( AT_HUNGRY, ch );
        send_to_char( "No such Guild.\n\r", ch );
        return;
    }
    set_char_color( AT_HUNGRY, ch );
    ch_printf( ch, "\n\r%s\n\r", guild->name );
    set_char_color( AT_YELLOW, ch );
    ch_printf( ch, "Guildmaster: %s\n\rNumber One : %s\n\rNumber Two : %s\n\rMotto      : %s\n\r",
                        guild->leader,
                        guild->number1,
                        guild->number2,
			guild->motto );
    if ( !str_cmp( ch->name, guild->deity   )
    ||   !str_cmp( ch->name, guild->leader  )
    ||   !str_cmp( ch->name, guild->number1 )
    ||   !str_cmp( ch->name, guild->number2 ) )
        ch_printf( ch, "Members    : %d\n\r",
			guild->members );
    ch_printf( ch, "Biases     : %s\n\r", faction_bit_name( &guild->flags ) );
    set_char_color( AT_HUNGRY, ch );
    ch_printf( ch, "Description:\n\r%s\n\r", guild->description );
    return;
}                                                                           

void do_victories( CHAR_DATA *ch, char *argument )
{
    char filename[256]; 

    if ( !IS_CLANNED( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    sprintf( filename, "%s%s.record", FACTION_DIR, ch->pcdata->faction->name );
    set_pager_color( AT_PURPLE, ch );
    if ( !str_cmp( ch->name, ch->pcdata->faction->leader ) && !str_cmp( argument, "clean" ) )
    {
	FILE *fp = fopen( filename, "w" );

	if ( fp )
	    FCLOSE( fp );

	send_to_pager( "\n\rVictories ledger has been cleared.\n\r", ch );
	return;
    }
    else
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    send_to_pager( "\n\rLVL  Character       LVL  Character\n\r", ch );
    show_file( ch, filename );
    return;
}


void do_shove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int exit_dir;
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    bool nogo;
    ROOM_INDEX_DATA *to_room;    
    int chance = 0;
    int race_bonus = 0;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch)
    || !IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
    {
	send_to_char("Only deadly characters can shove.\n\r", ch);
	return;
    }

    if  ( get_timer(ch, TIMER_PKILLED) > 0)
    {
	send_to_char("You can't shove a player right now.\n\r", ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Shove whom?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch);
	return;
    }

    if (victim == ch)
    {
	send_to_char("You shove yourself around, to no avail.\n\r", ch);
	return;
    }
    if ( IS_NPC(victim)
    || !IS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) )
    {
	send_to_char("You can only shove deadly characters.\n\r", ch);
	return;
    }
    
    if ( abs( ch->level - victim->level ) > 5 )
    {
	send_to_char("There is too great an experience difference for you to even bother.\n\r", ch);
	return;
    }

    if  ( get_timer(victim, TIMER_PKILLED) > 0)
    {
	send_to_char("You can't shove that player right now.\n\r", ch);
	return;
    }

    if ( (victim->position) != POS_STANDING )
    {
	act( AT_PLAIN, "$N isn't standing up.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "Shove them in which direction?\n\r", ch);
	return;
    }

    exit_dir = get_dir( arg2 );
    if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    &&  get_timer(victim, TIMER_SHOVEDRAG) <= 0)
    {
	send_to_char("That character cannot be shoved right now.\n\r", ch);
	return;
    }
    victim->position = POS_SHOVE;
    nogo = FALSE;
    if ((pexit = get_exit(ch->in_room, exit_dir)) == NULL )
      nogo = TRUE;
    else
    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    && (!IS_AFFECTED(victim, AFF_PASS_DOOR)
    ||   IS_SET(pexit->exit_info, EX_NOPASSDOOR)) )
      nogo = TRUE;
    if ( nogo )
    {
	send_to_char( "There's no exit in that direction.\n\r", ch );
        victim->position = POS_STANDING;
	return;
    }
    to_room = pexit->to_room;
    if (IS_SET(to_room->room_flags, ROOM_DEATH))
    {
      send_to_char("You cannot shove someone into a death trap.\n\r", ch);
      victim->position = POS_STANDING;
      return;
    }
    
    if (ch->in_room->area != to_room->area
    &&  !in_hard_range( victim, to_room->area ) )
    {
      send_to_char("That character cannot enter that area.\n\r", ch);
      victim->position = POS_STANDING;
      return;
    }

/* Check for class, assign percentage based on that. */
if (ch->class == CLASS_WARRIOR)
  chance = 70;
if (ch->class == CLASS_VAMPIRE)
  chance = 65;
if (ch->class == CLASS_RANGER)
  chance = 60;
if (ch->class == CLASS_DRUID)
  chance = 45;
if (ch->class == CLASS_CLERIC)
  chance = 35;
if (ch->class == CLASS_THIEF)
  chance = 30;
if (ch->class == CLASS_MAGE)
  chance = 15;

/* Add 3 points to chance for every str point above 15, subtract for 
below 15 */

chance += ((get_curr_str(ch) - 15) * 3);

chance += (ch->level - victim->level);

if (ch->race == 1)
race_bonus = -3;

if (ch->race == 2)
race_bonus = 3;

if (ch->race == 3)
race_bonus = -5;

if (ch->race == 4)
race_bonus = -7;
 
if (ch->race == 6)
race_bonus = 5;
 
if (ch->race == 7)
race_bonus = 7;
 
if (ch->race == 8)
race_bonus = 10;
 
if (ch->race == 9)
race_bonus = -2;
 
chance += race_bonus;
 
/* Debugging purposes - show percentage for testing */

/* sprintf(buf, "Shove percentage of %s = %d", ch->name, chance);
act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
*/

if (chance < number_percent( ))
{
  send_to_char("You failed.\n\r", ch);
  victim->position = POS_STANDING;
  return;
}
    act( AT_ACTION, "You shove $M.", ch, NULL, victim, TO_CHAR );
    act( AT_ACTION, "$n shoves you.", ch, NULL, victim, TO_VICT );
    move_char( victim, get_exit(ch->in_room,exit_dir), 0);
    if ( !char_died(victim) )
      victim->position = POS_STANDING;
    WAIT_STATE(ch, 12);
    /* Remove protection from shove/drag if char shoves -- Blodkai */
    if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE)   
    &&   get_timer(ch, TIMER_SHOVEDRAG) <= 0 )
      add_timer( ch, TIMER_SHOVEDRAG, 10, NULL, 0 );
}

void do_drag( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int exit_dir;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    bool nogo;
    int chance = 0;
    int race_bonus = 0;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) )
    /* || !IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )  */
    {
	send_to_char("Only characters can drag.\n\r", ch);
	return;
    }

    if  ( get_timer(ch, TIMER_PKILLED) > 0)
    {
	send_to_char("You can't drag a player right now.\n\r", ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Drag whom?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch);
	return;
    }

    if ( victim == ch )
    {
	send_to_char("You take yourself by the scruff of your neck, but go nowhere.\n\r", ch);
	return; 
    }

    if ( IS_NPC(victim)  )
         /* || !IS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) ) */
    {
	send_to_char("You can only drag characters.\n\r", ch);
	return;
    }

    if ( !xIS_SET( victim->act, PLR_SHOVEDRAG)
    &&   !IS_SET( victim->pcdata->flags, PCFLAG_DEADLY) )
    {
	send_to_char("That character doesn't seem to appreciate your attentions.\n\r", ch);
	return;
    }

    if  ( get_timer(victim, TIMER_PKILLED) > 0)
    {
	send_to_char("You can't drag that player right now.\n\r", ch);
	return;
    }

    if ( victim->fighting )
    {
        send_to_char( "You try, but can't get close enough.\n\r", ch);
        return;
    }
          
    if( !IS_SET(ch->pcdata->flags, PCFLAG_DEADLY ) && IS_SET( victim->pcdata->flags, PCFLAG_DEADLY ) ){
	send_to_char("You cannot drag a deadly character.\n\r", ch);
	return;
    }

    if ( !IS_SET(victim->pcdata->flags, PCFLAG_DEADLY) && victim->position > 3 )
    {
	send_to_char("They don't seem to need your assistance.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "Drag them in which direction?\n\r", ch);
	return;
    }

    if ( abs( ch->level - victim->level ) > 5 )
    {
	if ( IS_SET(victim->pcdata->flags, PCFLAG_DEADLY)
	&&   IS_SET(ch->pcdata->flags, PCFLAG_DEADLY) )
	{
          send_to_char("There is too great an experience difference for you to even bother.\n\r", ch);
          return;
	}
    }

    exit_dir = get_dir( arg2 );

    if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    &&   get_timer( victim, TIMER_SHOVEDRAG ) <= 0)
    {
	send_to_char("That character cannot be dragged right now.\n\r", ch);
	return;
    }

    nogo = FALSE;
    if ((pexit = get_exit(ch->in_room, exit_dir)) == NULL )
      nogo = TRUE;
    else
    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    && (!IS_AFFECTED(victim, AFF_PASS_DOOR)
    ||   IS_SET(pexit->exit_info, EX_NOPASSDOOR)) )
      nogo = TRUE;
    if ( nogo )
    {
	send_to_char( "There's no exit in that direction.\n\r", ch );
	return;
    }

    to_room = pexit->to_room;
    if (IS_SET(to_room->room_flags, ROOM_DEATH))
    {
      send_to_char("You cannot drag someone into a death trap.\n\r", ch);
      return;
    }

    if (ch->in_room->area != to_room->area
    && !in_hard_range( victim, to_room->area ) )
    {
      send_to_char("That character cannot enter that area.\n\r", ch);
      victim->position = POS_STANDING;
      return;
    }
    
/* Check for class, assign percentage based on that. */
if (ch->class == CLASS_WARRIOR)
  chance = 70;
if (ch->class == CLASS_VAMPIRE)
  chance = 65;
if (ch->class == CLASS_RANGER)
  chance = 60;
if (ch->class == CLASS_DRUID)
  chance = 45;
if (ch->class == CLASS_CLERIC)
  chance = 35;
if (ch->class == CLASS_THIEF)
  chance = 30;
if (ch->class == CLASS_MAGE)
  chance = 15;

/* Add 3 points to chance for every str point above 15, subtract for 
below 15 */

chance += ((get_curr_str(ch) - 15) * 3);

chance += (ch->level - victim->level);

if (ch->race == 1)
race_bonus = -3;

if (ch->race == 2)
race_bonus = 3;

if (ch->race == 3)
race_bonus = -5;

if (ch->race == 4)
race_bonus = -7;

if (ch->race == 6)
race_bonus = 5;

if (ch->race == 7)
race_bonus = 7;

if (ch->race == 8)
race_bonus = 10;

if (ch->race == 9)
race_bonus = -2;

chance += race_bonus;
/*
sprintf(buf, "Drag percentage of %s = %d", ch->name, chance);
act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
*/
if (chance < number_percent( ))
{
  send_to_char("You failed.\n\r", ch);
  victim->position = POS_STANDING;
  return;
}
    if ( victim->position < POS_STANDING )
    {
	sh_int temp;

	temp = victim->position;
	victim->position = POS_DRAG;
	act( AT_ACTION, "You drag $M into the next room.", ch, NULL, victim, TO_CHAR ); 
	act( AT_ACTION, "$n grabs your hair and drags you.", ch, NULL, victim, TO_VICT ); 
	move_char( victim, get_exit(ch->in_room,exit_dir), 0);
	if ( !char_died(victim) )
	  victim->position = temp;
/* Move ch to the room too.. they are doing dragging - Scryn */
	move_char( ch, get_exit(ch->in_room,exit_dir), 0);
	WAIT_STATE(ch, 12);
	return;
    }
    send_to_char("You cannot do that to someone who is standing.\n\r", ch);
    return;
}

char *  const   faction_flag [] =
{
  "antimage",		"anticleric",		"antithief",
  "antiwarrior",	"antivampire",		"antidruid",
  "antiranger",		"antiaugurer",		"antipaladin",
  "antigood",		"antineutral",		"antievil",
  ""
};

int get_factionflag( char *flag )
{
    int x;

    for ( x = 0; x < (sizeof( faction_flag ) / sizeof( faction_flag[0] )); x++ )
    {
	if ( !str_cmp( flag, faction_flag[x] ) )
	    return x;
    }
    return -1;
}

bool check_factionflags( CHAR_DATA *ch, FACTION_DATA *faction )
{
    bool value = FALSE;

    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_MAGE )    && ch->class == CLASS_MAGE )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_CLERIC )  && ch->class == CLASS_CLERIC )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_THIEF )   && ch->class == CLASS_THIEF )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_WARRIOR ) && ch->class == CLASS_WARRIOR )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_VAMPIRE ) && ch->class == CLASS_VAMPIRE )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_DRUID )   && ch->class == CLASS_DRUID )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_RANGER )  && ch->class == CLASS_RANGER )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_AUGURER ) && ch->class == CLASS_AUGURER )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_PALADIN ) && ch->class == CLASS_PALADIN )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_GOOD )    && IS_GOOD( ch ) )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_NEUTRAL ) && IS_NEUTRAL( ch ) )
	value = TRUE;
    if ( HAS_FACTION_FLAG( faction, FACTION_ANTI_EVIL )    && IS_EVIL( ch ) )
	value = TRUE;

    return value;
}

/*
 * Return ascii name of faction flags vector.
 */
char *faction_bit_name( EXT_BV *faction_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( xIS_SET(*faction_flags, FACTION_ANTI_MAGE)   )
	strcat( buf, " anti-mage"               );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_CLERIC) )
	strcat( buf, " anti-cleric"             );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_THIEF)  )
	strcat( buf, " anti-thief"              );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_WARRIOR))
	strcat( buf, " anti-warrior"            );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_VAMPIRE))
	strcat( buf, " anti-vampire"            );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_DRUID)  )
	strcat( buf, " anti-druid"              );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_RANGER) )
	strcat( buf, " anti-ranger"             );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_AUGURER))
	strcat( buf, " anti-augurer"            );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_PALADIN))
	strcat( buf, " anti-paladin"            );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_GOOD)   )
	strcat( buf, " anti-good"               );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_NEUTRAL))
	strcat( buf, " anti-neutral"            );
    if ( xIS_SET(*faction_flags, FACTION_ANTI_EVIL)   )
	strcat( buf, " anti-evil"               );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}
