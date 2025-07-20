#include <stdio.h>
#include <stdlib.h>

#define BUFLEN 128

int main( int argc, char **argv )
{
    FILE    *infp, *outfp;
    int     maxlines,lines;
    int     file;
    char    outfile[13];
    char    buffer[BUFLEN];
    
    if ( argc != 3 ) {
        fputs( "Usage:  fsplit {filename} {# of lines}\n", stderr );
        return EXIT_FAILURE;
    }

    infp = fopen( argv[1], "r" );
    if ( infp == NULL ) {
        fputs( "Cannot open file.\n", stderr );
        return EXIT_FAILURE;
    }
    maxlines = atoi( argv[2] );
    
    file = 1;
    
    while ( !feof( infp ) ) { /* while more data to read */

        sprintf( outfile, "file%d.txt", file );
        
        outfp = fopen( outfile, "w" );
        if ( outfp == NULL ) {
            fclose( infp );
            fputs( "Cannot create output file.\n", stderr );
            return EXIT_SUCCESS;
        }
        lines = maxlines;
        while ( !feof( infp ) && lines != 0 ) {
            fgets( buffer, BUFLEN, infp );
            fputs( buffer, outfp );
            lines--;
        }
        fclose( outfp );
        file++;
    }
    
    fclose( infp );

    return EXIT_SUCCESS;
}
