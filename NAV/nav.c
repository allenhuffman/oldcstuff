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
int find_text_start( char * );

/* globals */


int main( int argc, char **argv )
{
    FILE    *infp;
    FILE    *outfp;
    char    buffer[MAXLINES][BUFSIZE];
    int     dots[MAXLINES], start[MAXLINES];
    int     i, j;
    int     lines;
    char    *ptr;
    int     more[5];
    
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
        start[i] = find_text_start( buffer[i] );
    }

    outfp = stdout;
    fprintf( outfp, "<HTML>\n"
                    "<HEAD><TITLE>NAV generated file</TITLE></HEAD>\n"
                    "<BODY>\n" );

    fprintf( outfp, "<PRE>\n" );

    /* reset */
    for ( i=0 ; i<5 ; i++ ) more[i]=0;

    printf( " +---START\n" );
    
    /* scan through lines */
    for ( i=0 ; i<lines ; i++ ) {
        more[dots[i]] = find_more( &dots, i, lines );

/* */
        j=0;
        while( dots[i]>j ) {
            if ( more[j]>0 ) {
                printf( " | " );
            } else {
                printf( "   " );
            }
            j++;
	}
        printf( " | \n" );
/* */
        
        j=0;
        while( dots[i]>j ) {
            if ( more[j]>0 ) {
                printf( " | " );
            } else {
                printf( "   " );
            }
            j++;
	}
	
        if ( dots[i+1]>dots[i] ) { /* sub category below us? */
            printf( "[-]" );
        } else {
            printf( " +-" );
        }

        if ( dots[i+1]>dots[i] ) {
            printf( "-+-" );
        } else {
            printf( "---" );
        }
        printf( "%s", buffer[i]+start[i] );
    }
    printf( " |\n +---END\n" );
    
    fprintf( outfp, "</PRE>\n" );
                    
    fprintf( outfp, "</HTML>\n" );
    
    return EXIT_SUCCESS;
}

int     find_more( int dots[], int from, int lines )
{
    int     i;

    i = from;
    while( i<lines ) {
        if ( dots[i+1]==dots[from] ) {
            return 1;
        }
        if ( dots[i+1]<dots[from] ) return -1;
        i++;
    }
    return -1;
}

int     find_text_start( char *buffer )
{
    int     i;

    i = 0;
    while( buffer[i]==' ' || buffer[i]=='.' && buffer[i]!='\0' ) i++;

    return i;
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
