/*
 * 02/17/98 - Got rid of carraige return at the end of ascii time string.
 */

#include <stdio.h>          /* printf(), etc. */
#include <stdlib.h>         /* EXIT_SUCCESS, etc. */
#include <string.h>         /* strstr(), etc. */
#include <time.h>           /* for time functions */

#define LINEMAX 128         /* input file line length limit */

void ip_filt( FILE *infp, FILE *outfp, char *ip )
{
    int     i;
    char    buffer[LINEMAX];
    char    *ptr;

    struct  tm *timeptr;
    time_t  timer;
    char    *asctimeptr;

    /* get some time stuff */
    time( &timer );
    timeptr = localtime( &timer );
    asctimeptr = asctime( timeptr );

    /* asctime seems to return with a carraige return on the end of
     * the string, so let's get rid of that...
     */
    asctimeptr[strlen(asctimeptr)-1] = '\0';
     
    while( !feof( infp ) ) {
        ptr = fgets( buffer, LINEMAX, infp );

        if ( ptr == NULL ) break;

        ptr = strstr( buffer, "*ip*" );
        if ( ptr != NULL ) {
            /* if here, we found an "*ip*" so we replace it :) */
            *ptr = '\0';
            fprintf( outfp, "%s%s%s", buffer, ip, ptr+4 );
            continue;
        }

        ptr = strstr( buffer, "*time*" );
        if ( ptr != NULL ) {
            /* if here, we found a "*time*" so we replace it, too :) */
            *ptr = '\0';
            fprintf( outfp, "%s%s%s", buffer, asctimeptr, ptr+6 );
            continue;
        }

        fprintf( outfp, "%s", buffer );

    }

    return EXIT_SUCCESS;
}
