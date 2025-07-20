/*
 * Al's Magic Number Guessing Game
 *
 * Hacked together on 2/13/98
 *
 */

#include <stdio.h>
#include <stdlib.h>

/* prototypes */

char    yesno( void );

int main( int argc, char **argv )
{
    int     i, j, k;
    int     magic;
    int     ch;

    printf( "\n"
            "Welcome to Al's Magic Number Guessing Game!\n"
            "\n"
            "Think of a number between 1 and 100.  Try to remember the number\n"
            "you have thought of, otherwise this is likely to be a less than\n"
            "impressive trick.\n"
            "\n"
            );
    do {
        printf( "Have you thought of a number? " );
        fflush( stdout );
        ch = yesno();

        if ( ch=='Y' ) break;

        printf( "\n"
                "Well, I'm not letting you out of this program until you HAVE\n"
                "thought of a number between 1 and 100.  So there.\n"
                "\n"
                );
    } while( 1 );

    printf( "\n"
            "I will show you a series of numbers.  In each series, try to find\n"
            "the number you have thought of.  LOOK CAREFULLY because I will ask\n"
            "you after each list to tell me if you see YOUR secret thought of\n"
            "number in the list.\n"
            "\n"
            );
    printf( "If you answer incorrectly, basically, I'll be wrong and you will\n"
            "be less than impressed with this program.  So, PAY ATTENTION and\n"
            "don't tell me your number is in the list unless it really IS in\n"
            "the list.\n"
            "\n"
            );

    printf( "Do you understand? " );
    fflush( stdout );
    ch = yesno();

    if ( ch=='Y' ) {
        printf( "\n"
                "Good.  I knew you would.  Now, let's begin...\n"
                );
    } else {
        printf( "\n"
                "Well, it's obvious to me that you probably don't possess\n"
                "the required intelligence to even be USING a computer, let\n"
                "alone trying out this program.  Regardless, let's just try\n"
                "it anyway, and see how you do.\n"
                 );
    }
    
    i = 1;
    magic = 0;

    do {
        k = 0;
        
        printf( "\n" );
        for ( j=0 ; j<100 ; j++ ) {
            if ( j&i ) {
                printf( "%2d ", j );

                k++;
                if ( k > 13 ) {
                    printf( "\n" );
                    k = 0;
                }
            }

        }
        printf( "\n\nIs your number in the above list? (Y/N/Q) " );

        ch = yesno();
        if ( ch=='Y' ) magic = magic + i;
        
        i = i<<1;

    } while( i < 128 );
    
    printf( "\nAre you sure that %d wasn't in that last listing? ", magic );

    ch = yesno();

    if ( ch=='Y' ) {
        printf( "\n"
                "I'm suprised you answered that question.  That's the big\n"
                "magic trick, you see.  I asked you if your number was there\n"
                "and how else would I have known what number to ask unless\n"
                "I had already figured it out?  At least pretend to be amazed\n"
                "at this.\n"
                "\n"
                );
    } else {
        printf( "\n"
                "What do you mean 'Nope'?  No you aren't sure that it was in\n"
                "this list?  If that's what you mean, it seems obvious to me\n"
                "that you haven't even bothered to begin to even try to\n"
                "understand the point of this program.\n"
                "\n"
                );
        printf( "Oh well.\n"
                "\n"
                );
    }
    printf( "Thanks for playing Al's Magic Number Guessing Game!\n" );
    
    return EXIT_SUCCESS;
}

char      yesno()
{
    int     ch;

    do {
        ch = toupper( getch() );
    } while( ch != 'Y' && ch != 'N' );

    if ( ch=='Y' ) {
        printf( "Yep.\n" );
    } else {
        printf( "Nope.\n" );
    }

    return ch;
}
