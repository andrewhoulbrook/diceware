/* Implementation of a diceware passphrase generator in C.    *
 *  Ref: http://world.std.com/~reinhold/diceware.wordlist.asc */

#include <stdio.h>
#include <string.h>
#include <sodium.h>

// Set limit for size of passphrases to be generated
#define MAX_WORDS 100

// Function prototypes
void dieRoll(int *);
void verifyWordlist(char *, char *);

// Filename for Diceware wordlists and their SHA256 hashes
char wordlists[3][23] = {"diceware.wordlist.txt", "beale.wordlist.txt", "eff.large.wordlist.txt"};
char wordlistHashes[3][65] = {"74e3dcef458181446498d5c161a5650d01abb7e1cae3a8d8a175c9c751c1ab4e", "81fe9603fe49b43dccf2bb401dc5c76e90148757e075d5dbbdc4e7d5294e0c70", "addd35536511597a02fa0a9ff1e5284677b8883b83e986e43f15a3db996b903e"};

// Banner 
char *banner1 = "*********************************";
char *banner2 = "* Diceware Passphrase Generator *";
char *banner3 = "*                               *";
char *banner4 = "*********************************";

// Simulate the roll of a die. 
// Uses libsodium's randombytes_uniform to generate uniform random numbers
void dieRoll(int *iRoll)
{
    // Generate random int between 1 and 6
    *iRoll = randombytes_uniform(6) + 1;

    return;
}

// Check if local copy of wordlist exists and verify its integrity (SHA-256 hash)
void verifyWordlist(char *filename, char* fileHash)
{
    // Check if local copy of wordlist exists
    FILE *file = fopen(filename, "rb");

    if (file == 0)
    {
        printf("\nERROR: wordlist %s cannot be read.\n", filename);
        exit(1);
    }
    else
    {
        // Declare buffer for file reading and variable for handling length of buffer          
        unsigned char *buffer;
        unsigned char out[crypto_hash_sha256_BYTES];
        unsigned long length;
        size_t read_length;

        char hex[(crypto_hash_sha256_BYTES *2) + 1];

        // Determine size of wordlist file
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocate guarded memory space for the buffer 
        buffer = sodium_malloc(length + 1);

        if (buffer == NULL)
        {
            printf("\nERROR: Could not allocate memory.\n");
            exit(1);
        }

        // Read file into buffer, check if file has been read
        if (length)
        {
            read_length = fread(buffer, 1, length, file);
        
            if (read_length != length)
            {
                printf("\nERROR: error reading wordlist %s\n", filename);
                exit(1);
            }
        }

        // Close file and null-terminate the buffer string
        fclose(file);
        buffer[length] = '\0';

        // Generate a SHA256 hash for the wordlist
        crypto_hash_sha256(out, buffer, length);

        // Convert the hash to hexadecimal
        sodium_bin2hex(hex, sizeof hex, out, crypto_hash_sha256_BYTES);
        
        // Free buffer memory
        sodium_free(buffer); 

        // Verify the hash for the wordlist
        if (strcmp(hex, fileHash) != 0)
        {
            printf("\nERROR: hash for wordlist %s does not match.\n", filename);
            exit(1);
        }
        
        return;
    }
}

/**********
 *  Main  *
 **********/

int main()
{
    // Clear user's terminal
    system("clear");

    // First call to initiate libsodium.
    if (sodium_init() == 1)
    {
        printf("\nERROR: libsodium could not be initiated.\n");
        exit(1);
    }

    // Print banner to terminal
    printf("\n\n%s\n", banner1);
    printf("%s\n", banner2);
    printf("%s\n", banner3);
    printf("%s\n\n", banner4);

    int iWordlist = 1;      // default to original Diceware wordlist
 
    // Show user Diceware wordlist options, user selects a wordlist to use
    printf("Choose a wordlist:\n\n1. %s\n2. %s\n3. %s\n\n", wordlists[0], wordlists[1], wordlists[2]);
    if (scanf("%d", &iWordlist) != 1)
    { 
        printf("WARNING: Please enter a valid option.");
    }
    else
    {
        // If user types erroroneous input, reset to default wordlist
        if (iWordlist <= 0 || iWordlist > 3)
        {
            iWordlist = 1;
        }
    }
    
    // Check if local copy of wordlist exists, verify its integrity
    verifyWordlist(wordlists[iWordlist - 1], wordlistHashes[iWordlist - 1]);

    int iPassphraseLen = 6;      // default setting (six word passphrase)

    // Get user-defined size of passphrase length (in words)
    printf("Enter the length of Diceware passphrase (in words):\nType 0 to use default (six words)\n\n");
    if (scanf("%d", &iPassphraseLen) != 1)
    { 
        printf("WARNING: Please enter a valid option.");
    }
    else
    {
        // If user types erroroneous input, reset to default wordlist
        if (iPassphraseLen <= 0 || iPassphraseLen > MAX_WORDS)
        {
            iPassphraseLen = 1;
        }
    }
    
    // Allocate guarded memory for passphrase and variables used to construct the passphrase
    // Also implicitly calls sodium_mlock to prevent memory regions being swapped to disk
    int *ptrRandomNumber = sodium_malloc(sizeof *ptrRandomNumber);
    int *ptrRoll = sodium_malloc(sizeof *ptrRoll);
    char *sPassphrase = sodium_malloc(((sizeof sPassphrase) * 6 * iPassphraseLen) + 1);

    if (ptrRandomNumber == NULL || ptrRoll == NULL || sPassphrase == NULL)
    {
        printf("\nERROR: Could not allocate memory.\n");
        exit(1);
    }

    // Initialise passphrase with NULL 
    sPassphrase[0] = '\0';

    // Declare loop variables
    int x;
    int y;

    // Declare file variable for reading from wordlist
    FILE *fWordlist;

    // Generate five random numbers to build each word in a Diceware passphrase
    for (y = 1; y <= iPassphraseLen; y++)
    {
        // Open the wordlist file stream 
        fWordlist = fopen(wordlists[iWordlist - 1], "r");

        // Generate five random numbers (five rolls of the die)
        // Create a five digit index into the Diceware wordlist
        for (x = 0; x < 5; x++)
        {
            dieRoll(ptrRoll);      // Roll a die

            switch (x)
            {
                case 0: *ptrRandomNumber = 10000 * *ptrRoll; break;
                case 1: *ptrRandomNumber += 1000 * *ptrRoll; break;
                case 2: *ptrRandomNumber += 100 * *ptrRoll; break;
                case 3: *ptrRandomNumber += 10 * *ptrRoll; break;
                case 4: *ptrRandomNumber += *ptrRoll; break;
            }
        }

        // Declare variable to hold Diceware indcies and corresponding words
        int dicewareIndex;
        char dicewareWord[6];

        // Find Diceware word from wordlist matching five digit random number generated
        while ( !feof(fWordlist) )
        {
            // Read line from the wordlist file
            fscanf(fWordlist, "%d\t%s", &dicewareIndex, dicewareWord);

            // Determine if match is found to a Diceware word
            if (dicewareIndex == *ptrRandomNumber)
            {
                // Concat matching Diceware word to passphrase
                if (y > 1)
                {
                    strcat(sPassphrase, "-");
                }
                
                strcat(sPassphrase, dicewareWord);
            
                break;
            }
        }

        // Close wordlist
        fclose(fWordlist);
    }

    // Print generate random passphrase to terminal
    printf("\nPassphrase: %s\n\n", sPassphrase);

    // Free memory. Implicitly calls sodium_munlock and zeros memory region prior to deallocation 
    sodium_free(sPassphrase);
    sodium_free(ptrRandomNumber);
    sodium_free(ptrRoll);

    // Print clear screen warning and exit program
    printf("\nWarning: remember to clear terminal and keep passphrase confidential.\n\n")
    exit(0);

}