#define VER "0.02"
/*---------------------------------------------------------------------------|
| IP v0.02 by Allen Huffman (subetha@pobox.com)                              |
| Copyright (C) 1997 by Sub-Etha Software                                    |
|----------------------------------------------------------------------------|
| Syntax: IP -? {basename}                                                   |
| Usage : Blah blah blah...                                                  |
| Opts  : -? or /? = display this message.                                   |
|----------------------------------------------------------------------------|
|       Blah blah blah.                                                      |
|----------------------------------------------------------------------------|
| Ed #    Date      What Happened                                       Who  |
|  --   --------    ------------------------------------------------    ---  |
|  01   98/02/09    Added *time*.                                       ach  |
|  02   98/02/17    More code cleanup.                                  ach  |
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

#define SESSIONFILE "session.ftp"
#define FILESTOFTP  "filesto.ftp"

/*----------------------------------|
| Prototypes/function declarations. |
|----------------------------------*/
void    usage( void );
int     get_ip( char * );
void    ip_filt( FILE *, FILE *, char * );
int     skip_stuff( FILE * );
int     ftp_it( char *, char *, char *, char *, char );

/*
 * Some defines...
 */

#define PATHMAX 80

/*
 * Begin processing...
 */

int main( int argc, char **argv )
{
    char    ip[16];             /* holds our current IP address string */
    struct  ffblk dir;          /* holds directory entries */
    int     i, j, k;            /* misc. counters */
    FILE    *infp, *outfp;      /* input and output file streams */
    char    infile[PATHMAX];
    char    outfile[PATHMAX];
    char    outpath[PATHMAX]    = "\0";
    char    profile[40]         = "\0";
    char    ftpfile[PATHMAX]    = SESSIONFILE;
    char    filestoftp[PATHMAX] = FILESTOFTP;

    char    automode    = FALSE;
    char    verbose     = FALSE;
    char    keepfile    = FALSE;

    
    if ( argc <=1 ) {
        usage();
        return EXIT_SUCCESS;
    }
    printf( "*** IP Active.\n" );

    ip[0] = '\0';
    
    /* scan command line options */

    for ( i=1 ; i<argc ; i++ ) {
        if ( argv[i][0] == '-' || argv[i][0] == '/' ) { /* option found! */
            switch( toupper(argv[i][1]) ) {
                case '?': /* request for help? */
                case 'H':
                    usage();
                    return EXIT_SUCCESS;
                case 'A':
                    automode = TRUE;
                    break;
                case 'F':
                    keepfile = TRUE;
                    if ( argv[i][2] == '=' ) { /* "-f=filepath" */
                        strcpy( ftpfile, &argv[i][3] );
                    }
                    /* otherwise we just use defaults */
                    break;
                case 'I':
                    if ( argv[i][2] == '=' ) {
                        strcpy( filestoftp, &argv[i][3] );
                    } else {
                        printf( "\nIP:  Specify input file as: -i=FILENAME.EXT\n" );
                        return EXIT_FAILURE;
                    }
                    break;
                case 'O': /* specifying output directory? */
                    if ( argv[i][2] == '=' ) {
                        strcpy( outpath, &argv[i][3] );
                        strcat( outpath, "\\" ); /* for DOS */
                    } else {
                        printf( "\nIP:  Specify output path as: -o=OUTPUTDIR\n" );
                        return EXIT_FAILURE;
                    }
                    i++; /* possible problem here? */
                    break;
                case 'P': /* specifying a profile to match */
                    if ( argv[i][2] == '=' ) {
                        strcpy( profile, &argv[i][3] );
                    } else {
                        printf( "\nIP:  Specify profile keyword as: -p=KEYWORD\n" );
                        return EXIT_FAILURE;
                    }
                    break;
                case 'V': /* specifying verbose output */
                    verbose = TRUE;
                    break;
                case 'Z': /* specifying IP address manually */
                    if ( argv[i][2] == '=' ) {
                        strncpy( ip, &argv[i][3], 16 );
                    } else {
                        printf( "\nIP:  Specify IP as: -z=1.2.3.4\n" );
                        return EXIT_FAILURE;
                    }
                    break;
                default:
                    printf( "\nIP:  Unrecognized option: %s\n", argv[i] );
                    usage();
                    return EXIT_FAILURE;
            }
        }
    } /* end of for/next loop for argc for argument search */

    /* get our current IP address */

    if ( ip[0] == '\0' ) {
        printf( "IP:  Determining current IP address.\n" );
        i = get_ip( ip );
        if ( i != EXIT_SUCCESS ) {
            printf( "IP:  Unable to determine current IP address.\n" );
            return EXIT_FAILURE;
        }
    }

    /* now process filenames */    
    for ( i = 1 ; i < argc ; i++ ) {
        if ( argv[i][0] != '-' && argv[i][0] != '/' ) {

            /* if here, we are ready to process files */
            j = findfirst( argv[i], &dir, 0 );

/* maybe put a check in here to warn user when they haven't specified
 * a valid file name?
 */
            while( j == 0 ) { /* while no error, */

                strcpy( infile, dir.ff_name ); /* copy over name */
                strcpy( outfile, outpath ); /* prepend output directory */

                /* remove extension, if any */
                strcat( outfile, infile );
                for ( k=strlen(outfile) ; k>=0 ; k-- ) {
                    if ( outfile[k] == '.' ) {
                        outfile[k] = '\0';
                        break;
                    }
                }
                strcat( outfile, ".html" ); /* add desired extension */

                infp = fopen( infile, "r" );
                if ( infp == NULL ) {
                    printf( "IP:  Cannot open '%s'.\n", infile );

                } else {

                    outfp = fopen( outfile, "w" );

                    if ( outfp == NULL ) {
                        printf( "IP:  Cannot create '%s'.\n", outfile );


                        fclose( infp );

                    } else {

                        printf( "IP:  Processing '%s' into '%s'.\n",
                                infile, outfile );
                        ip_filt( infp, outfp, ip );
                        fclose( infp );
                        fclose( outfp );
                    }
                }
                j = findnext( &dir );
            }
        } /* end of for/next for argc for filename search */
    }

    /* now handle generating the ftp session file */
    if ( ftpfile[0]!='\0' ) { /* should we generate session file? */

        if ( profile[0] == '\0' ) {
            printf( "IP:  Processing all FTP entries.\n" );
        } else {
            printf( "IP:  Generating FTP session profile '%s'.\n", profile );
        }

        i = ftp_it( filestoftp, ftpfile, profile, ip, automode );

        /* if successfull, and we plan to keep the file... */
        if ( i == EXIT_SUCCESS && !automode ) {
            printf( "IP:  FTP session file '%s' created.\n", ftpfile );
        }
    }

    printf( "*** IP Done.\n" );
    
    return EXIT_SUCCESS;
}

/*
 * Usage screen
 */

void usage( void )
{
    printf( "\nIP v"VER" by Allen Huffman (subetha@pobox.com)\n"
            "Copyright (C) 1998 by Sub-Etha Software (www.pobox.com/~subetha/programs.html)\n"
            "\n"
            "Syntax: IP [-opts] {template name} [{template name} {...}]\n" );
    printf( "Usage : IP .html pre-processor.  Replaces IP macros with IP addr.\n"
            "Opts  : -? or -h = display this message.\n"
            "        -a       = automatic filter/ftp mode.\n"
            "        -f       = generate ftp session file '"SESSIONFILE"'.\n" );
    printf( "        -f=FILE  = generate ftp file and use this name for it.\n"
            "        -i=FILE  = specify name of ftp file list (default is '"FILESTOFTP"').\n"
            "        -o=DIR   = output filtered .html files to specified directory.\n" );
    printf( "        -p=TEXT  = profile keyword to match while processing.\n"
            "        -v       = verbose (debugging) output.\n"
            "\n"
            "NOTE  : Requires ROUTE, FTP, and DELTREE to be found in PATH.\n" );
    printf( "\n"
            "NOTE 2: -a mode may use -f, -i, and/or -p options.\n"
            "        -a by itself uses temporary filter dir and filenames,\n"
            "           which are automatically deleted upon completion.\n" );
    printf( "        -o is only used when NOT using -a mode.\n"
             );
}
