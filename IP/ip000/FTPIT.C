/* 
 * ftpit.c
 *
 * Reads a config file of sites/files to upload then generates an output
 * session file for ftp to use based on profile matching criteria.
 *
 * File format:

ftpsite     username password destdir mode sourcename destname    profile
-------------------------------------------------------------------------
redneck.com bubba    truck    .       a    INDEX.HTM  index.html  trucks
redneck.com bubba    truck    images  b    TRUCK.JPG  truck.jpg   trucks
redneck.com bubba    truck    sounds  b    FORD.WAV   ford.wav    trucks
redneck.com bubba    truck    .       a    TRUCK.HTM  truck.html  trucks
fiddles.com bubba    heehaw   .       a    INDEX2.HTM index2.html music
fiddles.com bubba    heehaw   pics    b    FIDDLE.JPG fiddle.jpg  music

 * For this file, if ftpit were called with a profile of "trucks", it
 * would perform the following actions:
 *  1. log onto redneck.com
 *  2. login as "bubba" with a password of "truck"
 *  3. go into "ascii" mode
 *  4. put local file "INDEX.HTM" to remote file "./index.html"
 *  5. go into "binary" mode
 *  6. put local file "TRUCK.JPG" to remote file "images/truck.jpg"
 *  7. put local file "FORD.WAV" to remote file "sounds/ford.wav"
 *  8. go into "ascii" mode
 *  9. put local file "TRUCK.HTM" to remote file "./truck.html"
 * 10. disconnect
 *
 * If a profile name is to contain multiple sites, it's best to group all
 * entries for the same site next to each other.  For optimal efficienty,
 * group all ASCII entries and all BINARY entries for each site together,
 * as well.
 *
 * If called with the profile "music", only the last two entries in the file
 * would be transferred.  Note that a "profile" can contain multiple target
 * keywords, such as "trucksmusic" or, for readability, "trucks/music". At
 * this time, this routine ONLY checks for a "whitespace" between each field
 * (such as spaces, tabs, ENTER) so any seperator is allowed (ie, you could
 * do "trucks.music" or "trucks-music-all" or whatever.).
 */

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define SITESIZE    80
#define NAMESIZE    40
#define PSWDSIZE    40
#define DIRSIZE     80
#define MODESIZE    8
#define SRCSIZE     40
#define DSTSIZE     40
#define PROFSIZE    40

typedef struct {
    char    site[SITESIZE];
    char    username[NAMESIZE];
    char    password[PSWDSIZE];
    char    directory[DIRSIZE];
    char    mode[MODESIZE];
    char    srcname[SRCSIZE];
    char    dstname[DSTSIZE];
    char    profile[PROFSIZE];
} FTP;

int     skip_stuff( FILE * );
int     ftp_it( char *, char *, char *, char *, char );

int ftp_it( char *infile, char *outfile, char *profile, char *ip, char doit )
{
    FILE    *infp, *outfp;
    char    buffer[128];
    int     i;
    int     line;
    FTP     ftp,prevftp;
    char    newsite, newuser, newmode;
    char    connected;
    char    tempdir = FALSE;
    char    tmpname[8];
    char    path[80];

    infp = fopen( infile, "r" );
    if ( infp == NULL ) {
        printf( "IP:  Unable to open list file '%s'.\n", infile );
        return EXIT_FAILURE;
    }
    outfp = fopen( outfile, "w" );
    if ( outfp == NULL ) {
        printf( "IP:  Unable to create session file '%s'.\n", outfile );
        fclose( infp );
        return EXIT_FAILURE;
    }
    
    connected = FALSE;
    line = 1;

    /* read from file until there is no more to read */    
    while( !feof( infp ) ) {

        /* copy current to previous */
        memcpy( &prevftp, &ftp, sizeof(ftp) );

        /* skip over blanks or comments */
        i = skip_stuff( infp );
        if ( i == -1 ) break;
        ungetc( (char)i, infp );

        /* if here, we found something we'll assume is a proper entry */
        i = fscanf( infp, "%s %s %s %s %s %s %s %s",
                ftp.site, ftp.username, ftp.password, ftp.directory,
                ftp.mode, ftp.srcname, ftp.dstname, ftp.profile );
        if ( i != 8 ) {
            printf( "IP:  Invalid entry line %d.\n", line );
        }
        /* see if this entry fits the profile we are using */
        if ( profile[0] != '\0' ) { /* are we profile matching? */
            if ( strstr( ftp.profile, profile ) == NULL ) continue;
        }
        
        /* reset flags, assuming everything is same as last time */
        newsite = newuser = newmode = FALSE;

        /* if this file is going to a different site, */
        if ( strcmp( ftp.site, prevftp.site ) != 0 ) { /* new site? */
            newsite = TRUE;
            newuser = TRUE;
            newmode = TRUE;
        } else { /* no, same site, but maybe... */
            if ( strcmp( ftp.username, prevftp.username ) != 0 ) {
                fprintf( outfp, "user\n" );
                newuser = TRUE;
	    }
            if ( strcmp( ftp.mode, prevftp.mode ) != 0 ) newmode = TRUE;
        }
        /* if we are going to a new site, or we aren't connected yet, */
        if ( newsite || !connected ) {
            /* disconnected if we are connected */
            if ( connected ) {
                fprintf( outfp, "disconnect\n" );
            }
            /* then open the new site */
            fprintf( outfp, "o %s\n", ftp.site );
            connected = TRUE;
            newuser = TRUE;
        }
        /* do we have to enter username and password? */
        if ( newuser ) {
            fprintf( outfp, "%s\n", ftp.username );
            fprintf( outfp, "%s\n", ftp.password );
            fprintf( outfp, "glob\n" );
            newmode = TRUE;
        }
        /* do we have to specify transer mode? */
        if ( newmode ) {
            if ( ftp.mode[0] == 'b' ) {
                fprintf( outfp, "binary\n" );
            } else {
                fprintf( outfp, "ascii\n" );
            }
        }
        /* check for filtering (only if in auto-mode) */
        if ( doit && ftp.mode[0] == 'f' ) { /* yep, filter this file! */

            if ( !tempdir ) { /* have we made a temp dir yet? */
                system( "md IPTEMP" );
                tempdir = TRUE;
            }
            {
                FILE    *infp2, *outfp2;
                
                /* open original file for reading */
                infp2 = fopen( ftp.srcname, "r" );
                if ( infp2 == NULL ) {
                    printf( "IP:  Unable to filter '%s'.\n", ftp.srcname );
                    continue;
                }
                /* build a temporary file to copy the new version to */
                tmpnam( tmpname );
                strcpy( path, "IPTEMP\\" );
                strcat( path, tmpname );

                /* open the new temporary file */
                outfp2 = fopen( path, "w" );
                if ( outfp2 == NULL ) {
                    printf( "IP:  Unable to create '%s'.\n", tmpname );
                    fclose( infp2 );
                    continue;
                }
                /* process the file */
                ip_filt( infp2, outfp2, ip );

                fclose( infp2 );
                fclose( outfp2 );
            }
        } else {
            /* otherwise, just use the original file (untouched) */
            strcpy( path, ftp.srcname );
        }
        fprintf( outfp, "put %s %s/%s\n", path, ftp.directory, ftp.dstname );

        line++;
    }
    fprintf( outfp, "disconnect\n" );
    fprintf( outfp, "quit\n" );

    fclose( infp );
    fclose( outfp );
    
    if ( doit ) {
        strcpy( path, "ftp -s:" );
        strcat( path, outfile );

        i = system( path );
        if ( i != 0 ) {
            printf( "IP:  Unable to launch FTP program.\n" );
        }

        /* remove session file */
        strcpy( path, "del " );
        strcat( path, outfile );
        system( path );
    }
    if ( tempdir ) { /* were we using a temporary directory? */
        system( "deltree /Y IPTEMP" );
    }

    return EXIT_SUCCESS;
}

