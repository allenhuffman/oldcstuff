#define VER "0.00"
/*---------------------------------------------------------------------------|
| HTMList v0.00 by Allen Huffman (subetha@pobox.com)                         |
| Copyright (C) 1998 by Sub-Etha Software                                    |
|----------------------------------------------------------------------------|
| Syntax: HTMLIST -? {start_dir}                                             |
| Usage : Blah blah blah...                                                  |
| Opts  : -? or /? = display this message.                                   |
|         -r = recusive / process subdirectories                             |
|         -d = include directories as links                                  |
|----------------------------------------------------------------------------|
|       Blah blah blah.                                                      |
|----------------------------------------------------------------------------|
| Ed #    Date      What Happened                                       Who  |
|  --   --------    ------------------------------------------------    ---  |
|  01   98/08/09    Begin programming.                                  ach  |
|---------------------------------------------------------------------------*/

#include <stdio.h>          /* printf(), etc. */
#include <stdlib.h>         /* EXIT_SUCCESS, etc. */
#include <string.h>         /* strstr(), etc. */

#include <dir.h>            /* findfirst(), findnext() */

#define TRUE 1
#define FALSE 0

/*
 * Some filename defaults
 */

#define HTMLFILE  "index.htm"
  
/*----------------------------------|
| Prototypes/function declarations. |
|----------------------------------*/
void    usage( void );

/*
 * Some defines...
 */

#define PATHMAX 80

/*
 * Begin processing...
 */

int main( int argc, char **argv )
{
    int     i, j, k;            /* misc. counters */
    FILE    *dirfp, *filefp;    /* file stream thingies */
    FILE    *outfp;
    char    *ptr;
    
    char    recursive   = TRUE;
    char    directories = TRUE;
    char    verbose     = FALSE;

    char    buffer[PATHMAX];
    char    cmd[PATHMAX];       /* buffer for building system() calls */
    char    *tempdir;           /* ptr to TEMP directory path list */
    
    if ( argc <=1 ) {
        usage();
        return EXIT_SUCCESS;
    }
    printf( "*** HTMList Active.\n" );

    /* scan command line options */

    for ( i=1 ; i<argc ; i++ ) {
        if ( argv[i][0] == '-' || argv[i][0] == '/' ) { /* option found! */
            switch( toupper(argv[i][1]) ) {
                case '?': /* request for help? */
                case 'H':
                    usage();
                    return EXIT_SUCCESS;
                case 'D': /* include directories as links? */
                    directories = TRUE;
                    break;
                case 'R': /* recursive? */
                    recursive = TRUE;
                    break;
                case 'V': /* specifying verbose output */
                    verbose = TRUE;
                    break;
                default:
                    printf( "\nHTMList:  Unrecognized option: %s\n", argv[i] );
                    usage();
                    return EXIT_FAILURE;
            }
        }
    } /* end of for/next loop for argc for argument search */

    tempdir = getenv( "TEMP" );
    if ( tempdir==NULL ) {
        tempdir = getenv( "TMP" );
        if ( tempdir==NULL ) {
            tempdir = ""; /* else we use ROOT of current drive */
        }
    }
    if ( verbose ) {
        printf( "* Using TEMP dir of: %s\n", tempdir );
    }
    
    /* generate a text file listing the directories we wish to process */
    strcpy( cmd, "dir /ad /b " );
    
    /* first, see if we are doing things recursively */
    if ( recursive ) {
        strcat( cmd, "/s " );
    }
    strcat( cmd, ">" );    /* redirect output */
    /* strcat( cmd, tempdir ); */
    strcat( cmd, "\\htmldirs.txt" );
    system( cmd );
        
    dirfp = fopen( "\\htmldirs.txt", "r" );    
    if ( dirfp==NULL ) {
        fprintf( stderr, "* Error opening \\htmldirs.txt\n" );
        return EXIT_FAILURE;
    }
    
    while( !feof(dirfp) ) {
        /* get a directory path */
        ptr = fgets( buffer, PATHMAX, dirfp );
        if ( ptr==NULL ) break;

        buffer[strlen(buffer)-1] = '\0';
        /* change into it */
        if ( verbose ) fprintf( stderr, "Changing into %s\n", buffer );

        chdir( buffer ); /* change into this directory */

        /* create list of files */
        system( "dir /a-d /b >\\htmlfile.txt" );

        /* open the list of files file */
        filefp = fopen( "\\htmlfile.txt", "r" );
        if ( filefp==NULL ) {
            fprintf( stderr, "* Unable to open \\htmlfile.txt\n" );
            return EXIT_FAILURE;
        }

        outfp = fopen( "index.htm", "w" );

        fprintf( outfp,
            "<HTML><HEAD><TITLE>Al's Place FTP Photo Archives</TITLE></HEAD>\n"
            "<UL>\n" );
        
        while( !feof(filefp) ) {
            ptr = fgets( buffer, PATHMAX, filefp ); /* read file name */
            if ( ptr==NULL ) break;

            buffer[strlen(buffer)-1] = '\0'; /* axe \n */
printf( "buffer->%s\n", buffer );
            if ( strcmp( "INDEX.HTM", buffer)==0 ) continue;
            
            fprintf( outfp, "<LI><A HREF=%s>%s</A></LI>\n", buffer, buffer );
        }
        fprintf( outfp,
            "</UL>\n"
            "</HTML>\n" );
        
        fclose( outfp );
        fclose( filefp );

        system( "del \\htmlfile.txt" );
    }

    fclose( dirfp );

    system( "del \\htmldirs.txt" );
    
    printf( "*** HTMList Done.\n" );
    
    return EXIT_SUCCESS;
}

/*
 * Usage screen
 */

void usage( void )
{
    printf( "\nHTMList v"VER" by Allen Huffman (subetha@pobox.com)\n"
            "Copyright (C) 1998 by Sub-Etha Software (www.pobox.com/~subetha/programs.html)\n"
            "\n"
            "Syntax: HTMList [-opts] {starting directory}\n" );
    printf( "Usage : HTML file list generator.\n"
            "Opts  : -? or -h = display this message.\n" );
    printf( "        -d = include directories as links.\n"
            "        -r = recursively process subdirectories.\n"
            "        -v = verbose output.\n" );
}
