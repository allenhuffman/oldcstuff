/*---------------------------------------------------------------------------|
| FTPem v0.00 by Allen Huffman (allenh@pobox.com)                            |
| Copyright (C) 1998 by Sub-Etha Software                                    |
|----------------------------------------------------------------------------|
| Syntax: ftpem *.html ftp.server.com username password                      |
| Usage : Batch transfers files to an ftp site one file at a time.           |
| Opts  : -? or /? = display this message.                                   |
|----------------------------------------------------------------------------|
|       Blah blah blah.                                                      |
|----------------------------------------------------------------------------|
| Ed #    Date      What Happened                                       Who  |
|  --   --------    ------------------------------------------------    ---  |
|  01   98/02/08    Created                                             ach  |
|---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
    FILE    *infp, *outfp;
    int     i;
    char    buffer[80];

    /* check for proper amount of command line arguments */
    
    if ( argc != 5 ) {
        printf( "\nUsage:  ftpem *.html ftp.site.com username password\n"
            );
        return EXIT_FAILURE;
    }

    /* first build a raw file list by taking the DIR command output */
    
    strcpy( buffer, "dir /b " );
    strcat( buffer, argv[1] );
    strcat( buffer, ">ftpem.tmp" );

    i = system( buffer );
    if ( i != 0 ) {
        printf( "%s: Unable to create file list: %s\n", argv[0], buffer );
        return EXIT_FAILURE;
    }

    /* open this file for input so we can process it */
    
    infp = fopen( "ftpem.tmp", "r" );

    if ( infp == NULL ) {
        printf( "%s: Unable to open 'ftpem.tmp'.\n", argv[0] );
        return EXIT_FAILURE;
    }

    /* create the ftp session file */
    
    outfp = fopen( "ftpem.ftp", "w" );

    if ( outfp == NULL ) {
        printf( "%s: Unable to create 'ftpem.ftp'.\n", argv[0] );
        return EXIT_FAILURE;
    }

    /* start building the ftp session file */

    fprintf( outfp, "glob\n"
                    "hash\n"
           );

    /* loop until all files are taken care of */
           
    while( !feof( infp ) ) {
        if ( fgets( buffer, 80, infp ) == NULL ) break;

        fprintf( outfp, "o %s\n"                            /* server */
                        "%s\n"                              /* username */
                        "%s\n"                              /* password */
                        "bin\n"                             /* binary mode */
                        "put %s\n"                          /* put filename */
                        "disconnect\n",                     /* disconnect */
                        argv[2], argv[3], argv[4], buffer
               );

    }
    fprintf( outfp,  "quit\n" );                            /* quit ftp */

    /* close the files */
    
    fclose( outfp );
    fclose( infp );

    /* delete the temporary file */
    
    i = system( "del ftpem.tmp" );

    if ( i != 0 ) {
        printf( "%s: Unable to delete temp file ftpem.tmp'\n", argv[0] );
    }

    /* now run ftp using this session file */
    
    i = system( "ftp -s:ftpem.ftp" );

    if ( i != 0 ) {
        printf( "%s: Unable to launch ftp program.\n", argv[0] );
        return EXIT_FAILURE;
    }

    /* delete the session file */
    
    i = system( "del ftpem.ftp" );

    if ( i != 0 ) {
        printf( "%s: Unable to delete ftp session file ftpem.ftp'\n", argv[0] );
    }
    
    return EXIT_SUCCESS;
}
