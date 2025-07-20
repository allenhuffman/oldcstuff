#define VER "0.00"
/*---------------------------------------------------------------------------|
| WebGen v0.01 by Allen Huffman (subetha@pobox.com)                          |
| Copyright (C) 1998 by Sub-Etha Software                                    |
| (www.pobox.com/~subetha/programs.html)                                     |
|----------------------------------------------------------------------------|
| Syntax: HTMLlist -? {basename}                                             |
| Usage : Blah blah blah...                                                  |
| Opts  : -? or -h = display this message.                                   |
|----------------------------------------------------------------------------|
|       Blah blah blah.                                                      |
|----------------------------------------------------------------------------|
| Ed #    Date      What Happened                                       Who  |
|  --   --------    ------------------------------------------------    ---  |
|  01   98/09/08    Created                                             ach  |
|---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 128
#define MAXLINES 142

/* prototypes */
int count_leading_dots( char * );

/* globals */


int main( int argc, char **argv )
{
    FILE    *infp;
    FILE    *outfp;
    char    buffer[MAXLINES][BUFSIZE];
    int     dots[MAXLINES];
    int     i;
    int     lines;
    char    *ptr;
    int     more_cat, more_top, more_sub;
    
    /* first, we read the source and figure it all out */
    
    fprintf( stderr, "*** Opening file.\n" );

    infp = fopen( "nav.txt", "r" );
    if ( infp==NULL ) {
        fprintf( stderr, "*** ERROR opening file.\n" );
        return EXIT_FAILURE;
    }
    
    lines = 0;
    while( !feof( infp ) && lines<MAXLINES ) {
        ptr = fgets( buffer[lines], BUFSIZE, infp );  /* read a line */
        if ( ptr==NULL ) break;
        lines++;
    }
    fclose( infp );

    fprintf( stderr, "*** %d lines read.\n", lines );

    /* process the file */

    for ( i=0; i<lines ; i++ ) {
        /* count leading dots */
        dots[i] = count_leading_dots( buffer[i] );

/*        printf( "* Line %d has %d dots.\n", i, dots[i] ); */
    }

    outfp = stdout;
    fprintf( outfp, "<HTML>\n"
                    "<HEAD><TITLE>NAV generated file</TITLE></HEAD>\n"
                    "<BODY>\n" );

    fprintf( outfp, "<PRE>\n" );

    for ( i=0; i<lines ; i++ ) {
        switch( dots[i] ) {
            case 0: /* category */
                more_cat = find_more( &dots, i, lines );
                if ( dots[i+1]>0 ) { /* with a topic */
                    printf( "[-]" );
                } else {
                    printf( " |-" );
                }
                printf( "---%s", buffer[i] );
                break;
            case 1: /* topic */
                more_top = find_more( &dots, i, lines );
                if ( more_cat>0 ) {
                    printf( " | " );
                } else {
                    printf( "   " );
                }
                if ( dots[i+1]>1 ) { /* with a subtopic */
                    printf( "[+]" );
                } else {
                    printf( " |-" );
                }
                printf( "---%s", buffer[i] );
                break;
            case 2: /* subtopic */
                more_sub = find_more( &dots, i, lines );
                if ( more_cat>0 ) {
                    printf( " | " );
                } else {
                    printf( "   " );
                }
                if ( more_top>0 ) {
                    printf( " | " );
                } else {
                    printf( "   " );
                }
                if ( dots[i+1]>2 ) { /* with items */
                    printf( "[+]" );
                } else {
                    printf( " |-" );
                }
                printf( "%s", buffer[i] );
                break;
            default:
                break;
        }
    }
    
    fprintf( outfp, "</PRE>\n" );
                    
    fprintf( outfp, "</HTML>\n" );
    
    return EXIT_SUCCESS;
}

int     find_more( int dots[], int from, int maxdots )
{
    int     i;

    for ( i=from+1; i<maxdots ; i++ ) {
        if ( dots[i]==dots[from] ) {
            return i;
        }
    }
    return 0;
}

int     count_leading_dots( char *buffer )
{
    int     i, dots;

    dots = 0;
    for( i=0 ; i<strlen(buffer) ; i++ ) {

        if ( buffer[i]=='.' ) { /* dot found */
            dots++;
            continue;
        }
        if ( buffer[i]!=' ' ) break; /* something else, so quit */
    }
    return dots;
}
