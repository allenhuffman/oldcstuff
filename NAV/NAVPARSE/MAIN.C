#include <stdio.h>
#include <stdlib.h>

int level = 0;

int
main( int argc, char **argv )
{
    char    buffer[80];
    int     i;
    FILE    *fp;

    printf( "*** Begin processing.\n" );
    
    fp = fopen( "test.txt", "r" );
    if ( fp == NULL ) {
        printf( "Cannot open test.txt.\n" );
        return EXIT_FAILURE;
    }
    
    while( item( fp, buffer ) );

    printf( "*** Done processing.\n" );
    
    return EXIT_SUCCESS;
}
