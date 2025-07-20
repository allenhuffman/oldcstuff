#include <stdio.h>          /* printf(), etc. */
#include <stdlib.h>         /* EXIT_SUCCESS, etc. */
#include <string.h>         /* strstr(), etc. */

void ip_filt( FILE *infp, FILE *outfp, char *ip )
{
    int     i;
    char    buffer[80];
    char    *ptr;

    while( !feof( infp ) ) {
        ptr = fgets( buffer, 80, infp );

        if ( ptr == NULL ) break;

        ptr = strstr( buffer, "*ip*" );
        if ( ptr == NULL ) {
            fprintf( outfp, "%s", buffer );
            continue;
        }	
        /* if here, we found an "*ip*" so we replace it :) */
        *ptr = '\0';
        fprintf( outfp, "%s%s%s", buffer, ip, ptr+4 );

    }

    return EXIT_SUCCESS;
}
