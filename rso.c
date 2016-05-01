#include <stdio.h>
#include <stdint.h> 	// for uint8_t and uint16_t
#include <string.h>		// for strcmp();
#include <stdlib.h>		// for memset();

#include <sndfile.h>	// The libsndfile header; can be obtained by 'sudo apt-get install libsndfile-dev' or from http://www.mega-nerd.com/libsndfile/

#define MAX_DIVISIONS 32
#define DUMP_WIDTH 16

/* Should compile on any GCC compatible compiler.
 * For license see 'LICENSE'.
 *
 */

uint16_t cvt8to16(uint8_t dataFirst, uint8_t dataSecond)
{
	uint16_t dataBoth = 0x0000;
	
	dataBoth = dataFirst;
	dataBoth = dataBoth << 8;
	dataBoth |= dataSecond;
	
	return dataBoth;
}

struct rsoHeaderData
{
	int compression;
	uint16_t length;
	uint16_t samplerate;
};

int readRsoFile(char *loc, uint8_t *samples, struct rsoHeaderData *header)
{
	// Opens the file...
	
	printf("Opening file...   ");
	
	FILE *inFile;
	inFile = fopen(loc, "r");
	
	if (! inFile)
	{
		printf("error.\n");
		exit(1);
	}
	else
	{
		printf("done.\n");
	}
	
	// Extracts the header data and samples...
	
	// Some temporary variables
	
	int length1, length2;
	
	int smplrate1, smplrate2;
	
	uint8_t padding1, padding2;		// Just used to check the padding...
	
	//
	
	printf("Analysing file...   ");
	
	int x;
	int loop = 0;
	while ((x = getc(inFile)) != EOF)
	{
		if (loop == 0)
		{
			if (x != 1) {
				printf("error.\n'> Magic byte at offset 0 should be 01.\n");
				exit(2);
			}
		}
		
		if (loop == 1)
		{
			if (x == 0 || x == 1)
			{
				header -> compression =  x;
			}
			else
			{
				printf("error.\n'> Compression flag at offset 1 should be 00 or 01.\n");
				exit(2);
			}
		}
		
		if (loop == 2)
		{
			length1 = x; 	
		}
		
		if (loop == 3)
		{
			length2 = x;
			
			header -> length = cvt8to16(length1, length2);

		}
		
		if (loop == 4)
		{
			smplrate1 = x; 	
		}
		
		if (loop == 5)
		{
			smplrate2 = x;
			
			header -> samplerate = cvt8to16(smplrate1, smplrate2);

		}
		
		if (loop == 6)
		{
			padding1 = x; 	
		}
		
		if (loop == 7)
		{
			padding2 = x; 
			
			if ( (int) cvt8to16(padding1, padding2) != 0)
			{
				printf("error.\n'> Padding at offset 7 & 8 should be 00 00.\n");
				exit(2);
			}
			
			printf("done.\n");
			printf("Reading sample data...   ");
		}
		
		if (loop >= 8 && loop < 65535)
		{
			samples[loop-8] = (uint8_t) x;

		}
		
		loop++;
	}
	printf("done.\n");
	
	printf("Closing file...   ");
	fclose(inFile);		// Closes the file...
	printf("done.\n");
	
	
}

int main(int argc, char *argv[])
{
	if (! strcmp(argv[1], "-h") || ! strcmp(argv[1], "--help")  || argc < 3)
	{
		
		printf("\
 Syntax: 				\n\
   ./rso <option> <file>		\n\
					\n\
 Options: 				\n\
   -h		= display help.		\n\
   -a		= Analyse an RSO file (display the header data).		\n\
   -d		= Dump the sample data to the screen as hexadecimal.		\n\
	\n\n");

		exit(0);
	}
	

	// And, most importantly,
	uint8_t samples[65534];
	
	/* 65534 is the maximum number of samples in an RSO file,
	 * since the fiels occupies 2 bytes in the header.
	 * However, a potential workaround for this could exist
	 * by putting another header at the end of the sample data,
	 * thus allowing for another set of 65534 samples.
	 * this is why samples is a pointer to an array of 65534-byte arrays.
	 */
	
	memset(samples, 0, 65534);
	
	struct rsoHeaderData *rsoHeader = malloc(sizeof(struct rsoHeaderData));
	
	
	// Now for interpreting the cmdline arguments...
	
	if (! strcmp(argv[1], "-a") )
	{
		readRsoFile(argv[2], &samples[0], rsoHeader);
		
		if ( rsoHeader -> compression == 1)
		{	
			printf("Compression   = Yes\n");
		} else {
			printf("Compression   = No\n");
		}
		printf("Sample count  = %d\n", (int) rsoHeader -> length);		 // "Number of samples  = %04X\n" prints it in HEX.
		printf("Samplerate    = %dHz", (int) rsoHeader -> samplerate);
	}
	
	if (! strcmp(argv[1], "-d"))
	{
		readRsoFile(argv[2], &samples[0], rsoHeader);
		
		printf("Dumping chunk 1 as hex:");
		
		for (int i = 0; i < (rsoHeader->length - 8); i++)
		{
			if (i % DUMP_WIDTH == 0)  printf("\n   ");
			
			printf("%02X ", (int) samples[i]);
		}
	}

	exit(0);
}