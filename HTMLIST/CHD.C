#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
    chdir( "\\" );

    system( "dir" );

    chdir( "C:\\ALS" );

    system( "dir" );

    return EXIT_SUCCESS;
}
