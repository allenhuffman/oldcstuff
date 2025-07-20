#include <stdio.h>
#include <stdlib.h>

int
main( int argc, char **argv )
{
    char    buffer[80];
    int     i;
    FILE    *fp;

    fp = fopen( "test.txt", "r" );
    if ( fp == NULL ) {
        printf( "Cannot open test.txt.\n" );
        return EXIT_FAILURE;
    }
    
    do {
        i = get_next( fp, buffer );

        if ( i != EXIT_SUCCESS ) break;

        blivet( fp, buffer );

        printf( "disconnect\n" );
        
    } while( 1 );

    printf( "quit\n" );
    
    return EXIT_SUCCESS;
}
