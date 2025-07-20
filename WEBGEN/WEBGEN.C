#define NODEBUG
/*---------------------------------------------------------------------------|
| WebGen v0.00 by Allen Huffman (allenh@pobox.com)                           |
| Copyright (C) 1997 by Sub-Etha Software                                    |
|----------------------------------------------------------------------------|
| Syntax: WebGen -? {basename}                                               |
| Usage : Blah blah blah...                                                  |
| Opts  : -? or /? = display this message.                                   |
|----------------------------------------------------------------------------|
|       Blah blah blah.                                                      |
|----------------------------------------------------------------------------|
| Ed #    Date      What Happened                                       Who  |
|  --   --------    ------------------------------------------------    ---  |
|  01   97/08/07    Created                                             ach  |
|---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
 
/*----------------------------------|
| Prototypes/function declarations. |
|----------------------------------*/
void usage(void);
int process_template( char *filename );
int just_do_it( char *mapfile );
int skip_stuff( FILE *fp );

/*
 * Some #defines
 */
#define MAXLINES    50
#define LINELEN     128
#define PATHLEN     80
#define MAXLEN      200
#define NAMELEN     20

/*
 * Some nasty global variables.
 */

char    template[MAXLINES][LINELEN];    /* template file */

char    *keyword[] = {
    "IMAGE",
    "FACING",
    "DESCRIPTION",
    "MOVEL",
    "MOVER",
    "FORWARD",
    "BACK",
    "TURNLEFT",
    "TURNRIGHT",
    "TURNAROUND"
};
#define IMAGE 0
#define FACING 1
#define DESCRIPTION 2
#define MOVEL 3
#define MOVER 4
#define FORWARD 5
#define BACK 6
#define TURNLEFT 7
#define TURNRIGHT 8
#define TURNAROUND 9
#define KEYWORDS 10

int     keyline[KEYWORDS];

int main( int argc, char *argv[] )
{
    char    basename[NAMELEN];
    char    temp[PATHLEN+LINELEN+1];
    int     i;
    FILE    *fp;
    int     templatelines;
    
    if ( argc == 1 ) { /* no options? */
        usage();
        return EXIT_SUCCESS;
    }

    for ( i=1 ; i<argc ; i++ ) {
        if ( argv[i][0] == '-' || argv[i][0] == '/' ) { /* option found! */
            switch( toupper(argv[i][1]) ) {
                case '?': /* request for help? */
                case 'H':
                    usage();
                    return EXIT_SUCCESS;
                default:
                    fprintf( stderr, "\nUnrecognized option: %s", argv[i] );
                    usage();
                    return EXIT_SUCCESS;
            }
        } else { /* not an option, must be a file name */
            strcpy( basename, argv[i] );
        }
    }
    if ( basename[0] == '\0' ) {
        fprintf( stderr, "Error - no basename!\n" );
        return EXIT_FAILURE;
    }
    /* init some stuff */
    for ( i = 0 ; i < KEYWORDS ; i++ ) keyline[i] = -1;

    strcpy( temp, basename );
    strcat( temp, ".tem" );
    
    templatelines = process_template( temp );
    if ( templatelines == -1 ) {
        fputs( "Error opening template file.\n", stderr );
        return EXIT_FAILURE;
    }
    /* if here, we know what lines contain what keywords of the template */

    strcpy( temp, basename );
    strcat( temp, ".map" );

    i = just_do_it( temp );
    
    return EXIT_SUCCESS;
}

void usage(void)
{
    fputs( "\nWebGen v0.00 by Allen Huffman (allenh@pobox.com)\n"
     	   "Copyright (C) 1997 by Sub-Etha Software.\n", stderr );
    fputs( "\n", stderr );
    fputs( "Syntax: WebGen {-opts} [templatefile]\n"
           "Usage : HTML adventure generator.\n", stderr );
    fputs( "Opts  : -? or /? = display this message.\n"
#ifndef NODEBUG
           "*** DEBUG VERSION ***\n"
#endif
	, stderr );
}

int process_template( char *filename )
{
    FILE    *fp;
    int     i,j;
    char    *ptr;
    
    fp = fopen( filename, "r" );
    if ( fp == NULL ) return -1;

    i = 0;
    while( !feof( fp ) ) {
        ptr = fgets( template[i], LINELEN, fp );
        if ( ptr == NULL ) break;   /* exit if error */

        /* process keywords */
        for ( j=0 ; j<KEYWORDS ; j++ ) {
            ptr = strstr( template[i], keyword[j] );
            if ( ptr == NULL ) continue; /* ignore if none found */

            if ( keyline[j] != -1 ) {
                fputs( "Duplicated keyword - ignoring.\n", stderr );
            } else {
                keyline[j] = i; /* remember which line has this keyword */
            }
        }
        i++;

        if ( i >= MAXLINES ) {
            fputs( "Template file too big - ignoring.\n", stderr );
            break;
        }
    };
    return i;
}

int just_do_it( char *mapfile )
{
    FILE    *infp, *outfp;
    int     i,j;
    char    buffer[MAXLEN];
    int     north, south, west, east;
    int     line = 1;
    
    /* attempt to open the map file */
    infp = fopen( mapfile, "r" );
    if ( infp == NULL ) {
        fprintf( stderr, "Unable to open map file '%s'...\n", mapfile );
        return EXIT_FAILURE;
    }

    /* if here, the map file has been opened */

    while( !feof( infp ) ) { /* loop until no more lines in map file */
        /* process the files */
        do {
            i = skip_stuff( infp );
            if ( i == -1 ) break;
            
            if ( isdigit(i) !=0 ) { /* found a number */
                ungetc( i, infp );
                i = fscanf( infp, "%d,%d,%d,%d", &north, &south, &west, &east );

                if ( i != 4 ) {
                    fprintf( stderr, "Error in .map file on line %d.\n", line );
                    fclose( infp );
                    return EXIT_FAILURE;
                }
                /* if here, I have valid directions */

                /* scan to see if there are any overrides... */
        
                do {    
                    i = skip_stuff( infp );
                    if ( i == -1 ) break;
                    
                    if ( isdigit( i ) != 0 ) { /* found next number? */
                        ungetc( i, infp );
                        /* we can output the file now... */
                        printf( "Writing .htm file...\n" );
                        break;
                    } else {
                        /* otherwise we found something that might be a keyword */
                        ungetc( i, infp );
                        fgets( buffer, MAXLEN, infp );
                        printf( "Override? --> %s", buffer );
                        continue;
                    }
                } while( 1 ); /* scanning for overrides */
            } /* end of "found a number" */
        } while( 1 ); /* end of process the files */

    } /* end of map file */

    printf( "%d lines processed.\n", line );
    
    fclose( infp ); /* close the map file */

    return EXIT_SUCCESS;
}

int skip_stuff( FILE *fp )
{
    int     ch;
    char    buffer[MAXLEN];
    
    while( !feof( fp ) ) {
         ch = getc( fp );
         if ( ferror( fp ) != 0 ) {
             fprintf( stderr, "Error trying to skip_stuff...\n" );
             return -1;
         }
        if ( isspace(ch) != 0 ) continue; /* skip whitespace */

        if ( ch == '*' ) { /* comment! */
            fgets( buffer, MAXLEN, fp );
            fprintf( stdout, "Comment: %s", buffer );
            continue;
        }

        /* if here, we got something so let's return it... */
        return( ch );
    }
    /* if here, end of file! */
    return -1;
}
