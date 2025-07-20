/*---------------------------------------------------------------------------|
| todelphi v0.02 by Allen Huffman (allenh@pobox.com)                         |
| Copyright (C) 1998 by Sub-Etha Software                                    |
|----------------------------------------------------------------------------|
| Syntax: todelphi username password file {file} {...}                       |
| Usage : Batch transfers files to people.delphi.com one file at a time.     |
| Opts  : -? or /? = display this message.                                   |
|----------------------------------------------------------------------------|
|       Blah blah blah.                                                      |
|----------------------------------------------------------------------------|
| Ed #    Date      What Happened                                       Who  |
|  --   --------    ------------------------------------------------    ---  |
|  00   98/04/29    Created                                             ach  |
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
    char    disconnect;

    disconnect = 0;

    /* check for proper amount of command line arguments */
    
    if ( argc < 4 ) {
        usage();
        return EXIT_FAILURE;
    }

    /* create the ftp session file */
    
    outfp = fopen( "todelphi.ftp", "w" );

    if ( outfp == NULL ) {
        printf( "%s: Unable to create 'todelphi.ftp'.\n", argv[0] );
        return EXIT_FAILURE;
    }

    /* start building the ftp session file */

    fprintf( outfp, "glob\n"
                    "hash\n"
           );


    /* if we don't need to disconnect after each and every file
     * transfer, we can just do this stuff once.
     */
    if ( !disconnect ) {
        fprintf( outfp, "o people.delphi.com\n"     /* server */
                        "%s\n"                      /* username */
                        "%s\n"                      /* password */
                        "bin\n"                     /* binary mode */
                        "cd web\n",                 /* the right dir */
                        argv[1], argv[2] );
    }

    /* loop through command line file list */
    
    for ( i=3 ; i<argc ; i++ ) {

        /* first build a raw file list by taking the DIR command output */
    
        strcpy( buffer, "dir /b " );
        strcat( buffer, argv[i] );
        strcat( buffer, ">todelphi.tmp" );

        j = system( buffer );
        if ( j != 0 ) {
            printf( "%s: Unable to create file list: %s\n", argv[0], buffer );
            return EXIT_FAILURE;
        }

        /* open this file for input so we can process it */
    
        infp = fopen( "todelphi.tmp", "r" );

        if ( infp == NULL ) {
            printf( "%s: Unable to open 'todelphi.tmp'.\n", argv[0] );
            return EXIT_FAILURE;
        }

        while( !feof( infp ) ) {
            if ( fgets( buffer, 80, infp ) == NULL ) break;

            /* if we have to disconnect after each transfer, then
             * we have to do this each time.
             */
            if ( disconnect ) {
            
                fprintf( outfp, "o people.delphi.com\n"     /* server */
                                "%s\n"                      /* username */
                                "%s\n"                      /* password */
                                "bin\n"                     /* binary mode */
                                "cd web\n",                 /* the right dir */
                                argv[1], argv[2] );
            }

            fprintf( outfp, "put %s", buffer );             /* put filename */

            /* if we disconnect each time, let us do it */
            if ( disconnect ) {
                fprintf( outfp, "disconnect\n" );          /* disconnect */
            };
        }
        /* delete the session file */
    
        j = system( "del todelphi.tmp" );
        if ( j != 0 ) {
            printf( "%s: Unable to delete ftp temp file 'todelphi.tmp'\n", argv[0] );
        }
    }

    /* else we just disconnect once here */
    if ( !disconnect ) {
        fprintf( outfp, "disconnect\n" );          /* disconnect */
    }

    fprintf( outfp,  "quit\n" );                            /* quit ftp */

    /* close the files */
    
    fclose( outfp );
    fclose( infp );

#ifdef NODEBUG
    /* now run ftp using this session file */
    
    i = system( "ftp -s:todelphi.ftp" );
    if ( i != 0 ) {
        printf( "%s: Unable to launch ftp program.\n", argv[0] );
        return EXIT_FAILURE;
    }
#else
        printf( "\ntodelphi DEBUG: fork --> ftp -s:todelphi.ftp\n" );
#endif

#ifdef NODEBUG
    /* delete the session file */

    i = system( "del todelphi.ftp" );
    if ( i != 0 ) {
        printf( "%s: Unable to delete ftp session file todelphi.ftp'\n", argv[0] );
    }
#endif
    
    return EXIT_SUCCESS;
}

void    usage()
{
    printf( "\n"
            "Syntax: todelphi username password file {file} {...}\n"
            "Usage : Batch transfers files to people.delphi.com one file at a time.\n"
            "Opts  : -? or /? = display this message.\n"
          );
#ifndef NODEBUG
    printf( "\n"
            "*** DEBUG VERSION ***\n"
          );
#endif
}
