// Lets generate Windows 95-era licensing keys!
// https://medium.com/@dgurney/so-you-want-to-generate-license-keys-for-old-microsoft-products-a355c8bf5408

// 5/28/20 - Yay! generating Windows CD keys works! I've ironed out most of the possible bugs,
//           except for one where the program can not generate zeros in the keys, due to the way
//           I was converting my digit arrays into full integers, long story short, 0 * 1000 is
//           still zero, so there are no zeros appearing in the generated license keys. Going
//           to work on OEM keys tomorrow, I'm sleepy.

// 6/3/20 - Yay again! I've finished OEM keys and made all site generations their own function
//          so it could be called multiple times in the case the user wants more than one
//          generated. Ironed out all bugs, I'll start implementing command-line arguments
//          and bringing it all together in the main() function. It's been nice.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

const char *VERSION = "v. 1.0.2.";


typedef int _bool;
enum { TRUE = 0, FALSE = 1};


int random_int(int minimum, int maximum)
{
    return (int)(rand() % ((maximum - minimum) + 1)) + minimum;
}

/////////////////////////////////////////////////////////////////
// CD FUNCTIONS
/////////////////////////////////////////////////////////////////

int cdFirstSite(_bool isVerbose)
{
    _bool passesFirstTest = FALSE;      // Hasn't passed the test since we haven't given it yet.

    // The first site can not be any of these numbers.
    const int firstSiteLookUp[] =
    {
        333, 444, 555, 666, 777, 888, 999
    };

    int firstSite[3];
    int compiledFirstSite = 0;

    while(passesFirstTest == FALSE)
    {
        firstSite[0] = random_int(1, 9);
        firstSite[1] = random_int(1, 9);
        firstSite[2] = random_int(1, 9);

        compiledFirstSite = (firstSite[0] * 100) + (firstSite[1] * 10) + firstSite[2];
        if(isVerbose == TRUE)
            printf("compiled first site is \"%d\"\n", compiledFirstSite);

        int i, lookIndex;
        for(i = 0; i < 7; i++)
        {
            lookIndex = firstSiteLookUp[i];
            if(compiledFirstSite == lookIndex)
            {
                if(isVerbose == TRUE)
                    fprintf(stderr, "compiled site \"%d\" failed test. retrying...\n", compiledFirstSite);
                break;
            }
        }

        if(compiledFirstSite != lookIndex)
            passesFirstTest = TRUE;

    }

    if(isVerbose == TRUE)
        printf("compiled site \"%d\" passes first test!\n", compiledFirstSite);

    return compiledFirstSite;
}

long int cdSecondSite(_bool isVerbose)
{

    long int secondSite[7];
    long int compiledSecondSite = 0;

    int sumOfDigits = 0;

    _bool passesSecondTest = FALSE;

    while(passesSecondTest == FALSE)
    {
        int i;

        // Last site is 7 digits long, so we'll do our assigning in a loop.
        for(i = 0; i <= 5; i++)
        {
            secondSite[i] = random_int(1, 9);
        }

        secondSite[6] = random_int(1, 7);       // Windows checks for the last digit. Must not be 0 or >=8

        // Same for our compiling.
        // Trying to fix a bug that I believe is in the compiling of the
        // array, let's try the repetitive and dirty way I did in the
        // first site.

        compiledSecondSite = (secondSite[0] * 1000000) +
                             (secondSite[1] * 100000 ) +
                             (secondSite[2] * 10000  ) +
                             (secondSite[3] * 1000   ) +
                             (secondSite[4] * 100    ) +
                             (secondSite[5] * 10     ) +
                              secondSite[6];

        if(isVerbose == TRUE)
            printf("compiled second site is \"%ld\"\n", compiledSecondSite);

        // And now let's sum up the digits
        // The digits in question must not be divisible by 7,
        // or else we run the thing over again.
        int digit = 0;
        long int placeHolder = compiledSecondSite;

        while(placeHolder > 0)
        {
            digit = (placeHolder % 10);
            sumOfDigits += digit;
            placeHolder /= 10;
        }

        if(sumOfDigits % 7 == 0)
            passesSecondTest = TRUE;
        else
        {
            if(isVerbose == TRUE)
                fprintf(stderr, "compiled site \"%ld\" failed test. retrying...\n", compiledSecondSite);
        }

    }

    if(isVerbose == TRUE)
        printf("compiled site \"%ld\" passes second test!\n", compiledSecondSite);

    return compiledSecondSite;
}

/////////////////////////////////////////////////////////////////
// OEM FUNCTIONS
/////////////////////////////////////////////////////////////////

// todo, prototype in main() already, sooooo

unsigned int oemFirstSite(_bool isVerbose)
{
    // OEM codes have 3 randomized sites. First one begins below.
    int firstSite[5];
    unsigned int compiledFirstSite = 0;

    // First 3 digits are actually part of a date.
    firstSite[0] = random_int(1, 3);
    firstSite[1] = random_int(1, 6);                // 111 to 366, the exact day.
    firstSite[2] = random_int(1, 6);

    // The last two digits are a date, ranging from 95 to 03.
    // having a zero in our keys will confuse the compiling method
    // I have so only 95 to 99 will be supported. This also makes the
    // switch() statement shorter.

    int choice = random_int(1, 5);
    switch(choice)
    {
        case 1: firstSite[3] = 9; firstSite[4] = 5; break;
        case 2: firstSite[3] = 9; firstSite[4] = 6; break;
        case 3: firstSite[3] = 9; firstSite[4] = 7; break;
        case 4: firstSite[3] = 9; firstSite[4] = 8; break;
        case 5: firstSite[3] = 9; firstSite[4] = 9; break;
    }

    // Compiling the first site.
    compiledFirstSite = (firstSite[0] * 10000) +
                        (firstSite[1] * 1000 ) +
                        (firstSite[2] * 100  ) +
                        (firstSite[3] * 10   ) +
                         firstSite[4];

    // There is no after-generation checking here, so it goes as is.
    if(isVerbose == TRUE)
        printf("compiled first site is \"%d\"\n", compiledFirstSite);

    return compiledFirstSite;
}

long int oemThirdSite(_bool isVerbose)
{
    // This is the same as the second site of the CD key,
    // except the first digit needs to be a zero.

    // What we are going to do is generate the rest of
    // the key and then add the zero after the fact.
    // We only have to worry about the 6 digits being
    // divisible by 7 since zero doesn't affect a sum.

    int thirdSite[6];
    long int compiledThirdSite = 0;

    int sumOfDigits = 0;
    _bool passesThirdTest = FALSE;

    while(passesThirdTest == FALSE)
    {
        int i;

        // Same as last time. Too long, loop time.
        for(i = 0; i < 6; i++)
        {
            thirdSite[i] = random_int(1, 9);
        }

        compiledThirdSite = (thirdSite[0] * 100000) +
                            (thirdSite[1] * 10000 ) +
                            (thirdSite[2] * 1000  ) +
                            (thirdSite[3] * 100   ) +
                            (thirdSite[4] * 10    ) +
                             thirdSite[5];

        if(isVerbose == TRUE)
            printf("compiled third site is \"%ld\"\n", compiledThirdSite);

        int digit = 0;
        long int placeholder = compiledThirdSite;

        while(placeholder > 0)
        {
            digit = (placeholder % 10);
            sumOfDigits += digit;
            placeholder /= 10;
        }

        if(sumOfDigits % 7 == 0)
            passesThirdTest = TRUE;
        else
        {
            if(isVerbose == TRUE)
                fprintf(stderr, "compiled site \"%ld\" failed test. retrying...\n", compiledThirdSite);
        }

    }

    if(isVerbose == TRUE)
        printf("compiled site \"%ld\" passes test!\n", compiledThirdSite);

    return compiledThirdSite;
}

unsigned int oemFourthSite(_bool isVerbose)
{
    // The fourth site is just random numbers, as long as it is
    // five digits.

    int fourthSite[5];
    unsigned int compiledFourthSite = 0;

    int i;
    for(i = 0; i < 5; i++)
    {
        fourthSite[i] = random_int(1, 9);
    }

    compiledFourthSite = (fourthSite[0] * 10000) +
                         (fourthSite[1] * 1000 ) +
                         (fourthSite[2] * 100  ) +
                         (fourthSite[3] * 10   ) +
                          fourthSite[4];

    if(isVerbose == TRUE)
        printf("compiled fourth site is \"%d\"\n", compiledFourthSite);

    return compiledFourthSite;
}

/////////////////////////////////////////////////////////////////
// main()
/////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    srand(time(NULL));

    // And now here is the generating part.
    _bool verboseMode = FALSE;

    unsigned int firstOEM, fourthOEM;
    const char *secondOEM = "OEM";
    long int thirdOEM;

    int firstCD;
    long int secondCD;

    firstOEM    = oemFirstSite(verboseMode);
    thirdOEM    = oemThirdSite(verboseMode);
    fourthOEM   = oemFourthSite(verboseMode);

    printf("\"%d-%s-0%ld-%d\"\n", firstOEM, secondOEM, thirdOEM, fourthOEM);

    firstCD     = cdFirstSite(verboseMode);
    secondCD    = cdSecondSite(verboseMode);

    printf("\"%d-%ld\"\n", firstCD, secondCD);

    return 0;
}
