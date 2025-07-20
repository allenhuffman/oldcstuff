#include <stdio.h>          /* printf(), etc. */
#include <stdlib.h>         /* EXIT_SUCCESS, etc. */
#include <string.h>         /* strstr(), etc. */

int get_ip( char *ip )
{
    int     i;
    char    buffer[80];
    char    *ptr;
    FILE    *infp;
    
    i = system ( "route print > showip.out" );
    if ( i != 0 ) {
        printf( "IP:  Unable for fork 'route' command.\n" );
        return EXIT_FAILURE;
    }
    
    infp = fopen( "showip.out", "r" );
    if ( infp == NULL ) {
        printf( "SHOWIP:  unable to open 'showip.out' file.\n" );
        return EXIT_FAILURE;
    }

    while( !feof( infp ) ) {
	ptr = fgets( buffer, 80, infp );
	if ( ptr == NULL ) break;

        if ( isdigit( buffer[67] ) ) { /* found it probably */
            buffer[68] = '\0';

            ptr = &buffer[67];          /* walk back to find space */
            while ( *ptr != ' ' && ptr > buffer ) ptr--;
            ptr++;
            
            strncpy( ip, ptr, 16 );
            break;
        }	
    }
    fclose( infp );

    system( "del showip.out" );

    return EXIT_SUCCESS;
}
