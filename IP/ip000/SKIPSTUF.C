#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 80

int skip_stuff( FILE *fp )
{
    int     ch;
    char    buffer[MAXLEN];
    
    while( !feof( fp ) ) {
         ch = getc( fp );
         if ( ferror( fp ) != 0 ) {
             printf( stderr, "IP:  Error trying to skip_stuff.\n" );
             return -1;
         }
        if ( isspace(ch) != 0 ) continue; /* skip whitespace */

        if ( ch == '*' ) { /* comment! */
            fgets( buffer, MAXLEN, fp );
/*            fprintf( stdout, "Comment: %s", buffer ); */
            continue;
        }

        /* if here, we got something so let's return it... */
        return( ch );
    }
    /* if here, end of file! */
    return -1;
}
