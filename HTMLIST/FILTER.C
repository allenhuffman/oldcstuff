#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
    char    buffer[100];

    printf( "<HTML><HEAD><TITLE>Al's FTP Photo Archives</TITLE></HEAD>\n"
            "<UL>\n" );
    
    while ( !feof(stdin) ) {
        fgets( buffer, 100, stdin );

        printf( "<LI><A HREF=%s>%s</A></LI>\n", buffer, buffer );
    }

    printf( "</UL>\n"
            "</HTML>\n" );
    
    return EXIT_SUCCESS;
}
