/*---------------------------------------------------------------------------|
| geoput v0.02 by Allen Huffman (allenh@pobox.com)                           |
| Copyright (C) 1998 by Sub-Etha Software                                    |
|----------------------------------------------------------------------------|
| Syntax: geoput username password file {file} {...}                         |
| Usage : Batch transfers files to GeoCities one file at a time.             |
| Opts  : -? or /? = display this message.                                   |
|----------------------------------------------------------------------------|
|       Blah blah blah.                                                      |
|----------------------------------------------------------------------------|
| Ed #    Date      What Happened                                       Who  |
|  --   --------    ------------------------------------------------    ---  |
|  00   98/02/08    Created                                             ach  |
|  01   98/02/17    Modified                                            ach  |
|  02   98/02/25    Fixed                                               ach  |
|---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Prototypes
 */
void    usage( void );

int     main( int argc, char **argv )
{
    FILE    *infp, *outfp;
    int     i, j;
    char    buffer[80];

    /* check for proper amount of command line arguments */
    
    if ( argc < 4 ) {
        usage();
        return EXIT_FAILURE;
    }

    /* create the ftp session file */
    
    outfp = fopen( "geoput.ftp", "w" );

    if ( outfp == NULL ) {
        printf( "%s: Unable to create 'geoput.ftp'.\n", argv[0] );
        return EXIT_FAILURE;
    }

    /* start building the ftp session file */

    fprintf( outfp, "glob\n"
                    "hash\n"
           );

    /* loop through command line file list */
    
    for ( i=3 ; i<argc ; i++ ) {

        /* first build a raw file list by taking the DIR command output */
    
        strcpy( buffer, "dir /b " );
        strcat( buffer, argv[i] );
        strcat( buffer, ">geoput.tmp" );

        j = system( buffer );
        if ( j != 0 ) {
            printf( "%s: Unable to create file list: %s\n", argv[0], buffer );
            return EXIT_FAILURE;
        }

        /* open this file for input so we can process it */
    
        infp = fopen( "geoput.tmp", "r" );

        if ( infp == NULL ) {
            printf( "%s: Unable to open 'geoput.tmp'.\n", argv[0] );
            return EXIT_FAILURE;
        }

        while( !feof( infp ) ) {
            if ( fgets( buffer, 80, infp ) == NULL ) break;

            fprintf( outfp, "o ftp.geocities.com\n"         /* server */
                            "%s\n"                          /* username */
                            "%s\n"                          /* password */
                            "bin\n"                         /* binary mode */
                            "put %s"                        /* put filename */
                            "disconnect\n",                 /* disconnect */
                            argv[1], argv[2], buffer
                   );
        }
        /* delete the session file */
    
        j = system( "del geoput.tmp" );
        if ( j != 0 ) {
            printf( "%s: Unable to delete ftp temp file 'geoput.tmp'\n", argv[0] );
        }
    }
    fprintf( outfp,  "quit\n" );                            /* quit ftp */

    /* close the files */
    
    fclose( outfp );
    fclose( infp );

#ifdef NODEBUG
    /* now run ftp using this session file */
    
    i = system( "ftp -s:geoput.ftp" );
    if ( i != 0 ) {
        printf( "%s: Unable to launch ftp program.\n", argv[0] );
        return EXIT_FAILURE;
    }
#else
        printf( "\ngeoput DEBUG: fork --> ftp -s:geoput.ftp\n" );
#endif

#ifdef NODEBUG
    /* delete the session file */

    i = system( "del geoput.ftp" );
    if ( i != 0 ) {
        printf( "%s: Unable to delete ftp session file geoput.ftp'\n", argv[0] );
    }
#endif
    
    return EXIT_SUCCESS;
}

void    usage()
{
    printf( "\n"
            "Syntax: geoput username password file {file} {...}\n"
            "Usage : Batch transfers files to GeoCities one file at a time.\n"
            "Opts  : -? or /? = display this message.\n"
          );
#ifndef NODEBUG
    printf( "\n"
            "*** DEBUG VERSION ***\n"
          );
#endif
}
