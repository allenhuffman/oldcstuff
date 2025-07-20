#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int
mode( FILE *infp, char *token )
{
    if ( token[0] == 'b' ) {
        printf( "bin\n" );
    } else {
        printf( "ascii\n" );
    }

    get_next( infp, token );

    if ( strstr( token, "[" ) != 0 ) {

        get_next( infp, token );
        
        filelist( infp, token );

        return FALSE;
    }
    
    while( file( infp, token ) );
    
    return FALSE;
}
