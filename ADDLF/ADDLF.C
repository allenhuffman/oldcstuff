#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
    int ch;
    
    while ( !feof(stdin) ) {
        ch = getch();
        putch( ch );
        if ( ch==0xd ) {
            fputs( "\l", stdout );
        }
    }

    return EXIT_SUCCESS;
}
