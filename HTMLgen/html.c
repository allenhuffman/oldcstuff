/*---------------------------------------------------------------------------|
| htmlgen v0.02 by Allen Huffman (allenh@pobox.com)                          |
| Copyright (C) 1997 by Sub-Etha Software                                    |
|----------------------------------------------------------------------------|
| Syntax: htmlgen [-opts] {template name} [{template name} {...}]            |
| Usage : HTML pre-processor.  Generates .html files from template files.    |
| Opts  : -? or /? = display this message.                                   |
|       * -dNAME   = define (or NAME=VALUE) a #define value                  |
|         -o={DIR} = output .html files to specified directory.              |
|       * -z       = read template from stdin (or use no options).           |
|                                                                            |
| * Not currently implemented.                                               |
|----------------------------------------------------------------------------|
|       HTML file pre-processor.  Allows the use of "#include", "#define",   |
| "#ifdef", "#ifndef", "#else", and "#endif" inside html 'template' files to |
| create the actual .html files.                                             |
|----------------------------------------------------------------------------|
| Ed #    Date      What Happened                                       Who  |
|  --   --------    ------------------------------------------------    ---  |
|  01   97/08/08    Rewritten from scratch.                             ach  |
|  02   97/09/02    Added #redefine, #undefine, /dLABEL, etc.           ach  |
|---------------------------------------------------------------------------*/

#include <stdio.h>	/* printf(), etc. */
#include <stdlib.h> 	/* EXIT_SUCCESS, etc. */
#include <string.h>	/* strcat(), strcpy(), etc. */
#include <ctype.h>	/* no idea... */
#include <dir.h>	/* findfirst(), findnext() */
 
/*----------------------------------|
| Prototypes/function declarations. |
|----------------------------------*/
void usage(void);
void add_define( char *, char );
int remove_define( char * );
int check_define( char * );
void show_define( void );
void just_do_it( FILE *, FILE * );
int parse_file( FILE *, FILE *, char );
void macro_sub( char * );
void parse_keyword( FILE *, FILE *, char *, int, char * );

/*
 * Some #defines
 */

#define TRUE 1
#define FALSE 0

#define PATHMAX 80      /* max length of filename/path string */
#define NAMEMAX 30	/* max length of #define NAME string */
#define VALUEMAX 80     /* max length of #define name VALUE string */
#define DEFMAX 30	/* max number of #defines */
#define BUFSIZE 500     /* buffer size for reading from template files */

/*
 * Some globals
 */

char	defname[DEFMAX][NAMEMAX];
char	defvalue[DEFMAX][VALUEMAX];

char    *keyword[] = {
    "#include ",
    "#define ",
    "#ifdef ",
    "#ifndef ",
    "#else",
    "#endif",
    "#undefine ",
    "#redefine "
};
#define INCLUDE 0
#define DEFINE  1
#define IFDEF   2
#define IFNDEF  3
#define ELSE    4
#define ENDIF   5
#define UNDEFINE 6
#define REDEFINE 7
#define KEYWORDS 8

int     keylen[KEYWORDS];

int     iflevel = 0;    /* for testing... */

/*
 * Begin processing...
 */

int main( int argc, char *argv[] )
{
    int     i, j, k;
    char    outpath[PATHMAX];   /* where .html files will be generated */
    char    infile[PATHMAX];
    char    outfile[PATHMAX];
    struct  ffblk dir;

    outpath[0] = '\0';
    iflevel = 0;  /* reset #if level, too! */

    if ( argc <= 1 ) { /* no options? */
        usage();
        return EXIT_SUCCESS;
    }

    fputs( "\n*** HTMLgen Active.\n", stderr );

    /* build keyword length array */
    for ( i=0 ; i<KEYWORDS ; i++ ) {
        keylen[i] = strlen( keyword[i] );
    }
    /* zap out defines */
    for ( i=0; i<DEFMAX ; i++ ) defname[i][0] = '\0';
    
    /* scan command line options */
    
    for ( i=1 ; i<argc ; i++ ) {
        if ( argv[i][0] == '-' || argv[i][0] == '/' ) { /* option found! */
            switch( toupper(argv[i][1]) ) {
                case '?': /* request for help? */
                case 'H':
                    usage();
                    return EXIT_SUCCESS;
                case 'D':
                    add_define( &argv[i][2], FALSE );
                    break;
                case 'O': /* specifying output directory? */
                    if ( argv[i][2] == '=' ) {
                        strcpy( outpath, &argv[i][3] );
                        strcat( outpath, "\\" ); /* for DOS */
                    } else {
			fputs( "\nSpecify output path as: -o=OUTPUTDIR\n", stderr );
                        return EXIT_FAILURE;
                    }
                    i++; /* possible problem here? */
                    break;
                case 'Z':
                    fputs( "-z not implemented yet.\n", stderr );
                    return EXIT_FAILURE;
                default:
                    fputs( "\nUnrecognized option: ", stderr );
                    fputs( argv[i], stderr );
                    fputs( "\n", stderr );
                    usage();
                    return EXIT_FAILURE;
            }
        }
    } /* end of for/next for argc for argument search */
    
    /* now process filenames */
    for ( i=1 ; i<argc ; i++ ) {
        if ( argv[i][0] != '-' || argv[i][0] != '/' ) { /* option found! */

            /* if here, we are ready to process files */
            j = findfirst( argv[i], &dir, 0 );
            while( j == 0 ) { /* while no error, */
                strcpy( infile, dir.ff_name ); /* copy over name */

                strcpy( outfile, outpath );
                /* remove extension, if any */
                strcat( outfile, infile );
                for ( k=strlen(outfile) ; k>=0 ; k-- ) {
                    if ( outfile[k] == '.' ) {
                        outfile[k] = '\0';
                        break;
                    }
                }
                strcat( outfile, ".html" ); /* add desired extension */

                just_do_it( infile, outfile );

                j = findnext( &dir );
            }
        }
    } /* end of for/next for argc for filename search */

    if ( iflevel > 0 ) {
        fputs( "*** WARNING:  More #if's than #endif's.\n", stderr );
    }
    
    fputs( "*** HTMLgen Done.\n", stderr );
    
    return EXIT_SUCCESS;
}

/*
 * Usage screen
 */

void usage(void)
{
    fputs(  "\nhtmlgen v0.02 by Allen Huffman (allenh@pobox.com)\n"
	    "Copyright (C) 1997 by Sub-Etha Software\n"
	    "\n"
            "Syntax: htmlgen [-opts] {template name} [{template name} {...}]\n"
            , stderr );
    fputs(  "Usage : HTML pre-processor.  Generates .html files from template files.\n"
            "Opts  : -? or /? = display this message.\n"
            "        -dNAME   = define (or NAME=VALUE) a #define value.\n"
            "        -o={DIR} = output .html files to specified directory.\n", stderr );
    fputs(  "      * -z       = read template from stdin (or use no options).\n"
            "\n"
            "* Not currently implemented.\n"
	, stderr );
}

/*
 * Add define
 */

void add_define( char *string, char override )
{
    int     i;
    char    *ptr, *nameptr, *valueptr;

    /* loop until we find NUL, or SPACE/EQUAL delimeter */
    ptr = string;
    while( *ptr != ' ' && *ptr != '=' && *ptr != '\0' ) ptr++;
    if ( *ptr == '\0' ) {
        valueptr = ptr;
    } else {
	*ptr = '\0';
        valueptr = ptr+1;
    }
    nameptr = string;
    
    i = check_define( nameptr );
    if ( i != -1 ) { /* if it's already defined... */
        if ( override == FALSE ) {
            fputs( "*** Already defined: ", stderr );
            fputs( nameptr, stderr );
            fputs( "\n", stderr );
            return;
        } /* otherwise, we can override it... */
            fputs( "** Redefining: ", stderr );
            fputs( nameptr, stderr );
            fputs( "\n", stderr );
    } else { /* else, if it's not already defined, try to add it... */
        for ( i=0 ; i<DEFMAX ; i++ ) {
            if ( defname[i][0] == '\0' ) { /* found a slot! */
                break;
            }
        }
        if ( i == DEFMAX ) { /* no slot :( */
            fputs( "*** No more #defines allowed: ", stderr );
            fputs( string, stderr );
            return;
        }
    }
    strcpy( defname[i], nameptr );
    strcpy( defvalue[i], valueptr );
}

void show_define( void )
{
    int     i;

    for ( i=0 ; i<=DEFMAX ; i++ ) {
        if ( defname[i][0] == '\0' ) continue; /* skip empty entries */
        printf( "%d. %s=%s\n", i, defname[i], defvalue[i] );
        i++;
    }
}

int check_define( char *string )
{
    int     i;

    for( i=0 ; i<=DEFMAX ; i++ ) {
        if ( defname[i][0] == '\0' ) continue; /* skip empty entries */
        if ( strcmp( string, defname[i] ) == 0 ) {
            return i;
        }
    }
    return -1;
}

int remove_define( char *string )
{
    int     i;

    i = check_define( string );
    if ( i != -1 ) defname[i][0] = '\0';

    return i;
}

void test_define( void )
{
    add_define( "ONE one", FALSE );
    add_define( "TWO", FALSE );
    add_define( "THREE three four five six", FALSE );
    add_define( "TWO newtwo", FALSE );
    show_define();
    add_define( "TWO newtwo_for_real", TRUE );
    add_define( "THIS=THAT", FALSE );
    add_define( "THEOTHER=SPAM", TRUE );
    show_define();
    if ( check_define( "ONE" ) != -1 ) fputs( "One is defined\n",stdout );
    if ( check_define( "SPAM" ) != -1 ) fputs( "Spam is defined\n",stdout );
}

int just_do_it( char *infile, char *outfile )
{
    FILE    *infp, *outfp;
    int     i;

    infp = fopen( infile, "r" );
    if ( infp == NULL ) {
        fprintf( stderr, "*** Unable to open: %s\n", infile );
        return errno;
    }

    outfp = fopen( outfile, "w" );
    if ( outfp == NULL ) {
        fprintf( stderr, "*** Unable to create: %s\n", outfile );
        fclose( infp );
        return errno;
    }
    fputs( "<!-- These files were created using the HTML Preprocessor -->\n", outfp );
    fputs( "<!-- For details, write to:  subetha@pobox.com -->\n", outfp );
   
    printf( "Processing %s into %s...\n", infile, outfile );

    i = parse_file( infp, outfp, FALSE );

    fclose( infp );
    fclose( outfp );

    return i;
}

int parse_file( FILE *infp, FILE *outfp, char skipmode )
{
    char    *buffer;
    char    *ptr;
    int     i;

    buffer = malloc( BUFSIZE );
    if ( buffer == NULL ) {
        fputs( "*** Unable to allocate input buffer.\n", stderr );
        return errno;
    }    

    while( !feof(infp) ) {
        ptr = fgets( buffer, BUFSIZE, infp );
        if ( ptr == NULL ) break;

        buffer[strlen(buffer)-1] = '\0'; /* ? */

        /* check for macro substitution */
        macro_sub( buffer );

        /* search for keywords */
        for ( i=0 ; i<KEYWORDS ; i++ ) {
            if ( strncmp( buffer, keyword[i], keylen[i] ) == 0 ) {
                /* keyword matched */
                parse_keyword( infp, outfp, buffer, i, &skipmode );
                break;
            }
        }
	/*
         * if we found an "#endif", we are done parsing this section of
	 * the file (probably) and we should break out of this.
	 */
	if ( i == ENDIF ) break; /* KLUDGE */

        if ( i == KEYWORDS ) { /* no keyword, so display line */
            if ( skipmode == FALSE ) {
                fputs( buffer, outfp );
                fputs( "\n", outfp );
            }
        }
    }
    free( buffer );

    return EXIT_SUCCESS;
}

void macro_sub( char *buffer )
{
    char    *ptr;
    char    macro[NAMEMAX], temp[BUFSIZE]; /* adjust this later */
    int     i;

    for ( i=0 ; i<=DEFMAX ; i++ ) {
        if ( defname[i][0] == '\0' ) continue; /* skip empty entries */
        strcpy( macro, "%" );
        strcat( macro, defname[i] );
        strcat( macro, "%" );

        do {
            ptr = strstr( buffer, macro );
            if ( ptr != NULL ) { /* found one! */

                strcpy( temp, ptr+strlen(macro) ); /* copy right side */
                *ptr = '\0'; /* NULL terminate where macro was */
                strcat( buffer, defvalue[i] ); /* copy value into buffer */
                strcat( buffer, temp ); /* copy right side back in */
            }
        } while( ptr != NULL ); /* loop until all are found */
    }
}

void parse_keyword( FILE *infp, FILE *outfp, char *buffer, int i, char *skipmode )
{
    FILE    *newfp;
    char    *ptr;

    switch( i ) {
        case INCLUDE:
            newfp = fopen( buffer+keylen[i], "r" );
            if ( newfp == NULL ) {
                fprintf( stderr, "*** Cannot include: %s\n", buffer+keylen[i] );
                break;
            }
            parse_file( newfp, outfp, FALSE );
            fclose( newfp );
            break;

        case DEFINE:
            add_define( buffer+keylen[i], FALSE );
            break;

        case IFDEF:
            iflevel++;
            if ( check_define( buffer+keylen[i] ) != -1 ) {
                parse_file( infp, outfp, FALSE );
            } else {
                parse_file( infp, outfp, TRUE );
            }
            break;

        case IFNDEF:
            iflevel++;
            if ( check_define( buffer+keylen[i] ) != -1 ) {
                parse_file( infp, outfp, TRUE );
            } else {
                parse_file( infp, outfp, FALSE );
            }
            break;

        case ELSE:
            if ( *skipmode == TRUE ) {
                *skipmode = FALSE;
            } else {
                *skipmode = TRUE;
            }
            break;

        case ENDIF:
            iflevel--;
            if ( iflevel < 0 ) {
                fputs( "*** WARNING:  More #endif's than #if's.\n", stderr );
            }
            break;

        case REDEFINE:
            add_define( buffer+keylen[i], TRUE );
            break;

        case UNDEFINE:
            remove_define( buffer+keylen[i] );
            break;
            
        default:
            fputs( "*** If you ever see this, tell the author!\n", stderr );
            break;
    }
}

