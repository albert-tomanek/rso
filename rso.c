#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define HEADER_SIZE 8 //bytes

uint16_t cvt8to16(uint8_t dataFirst, uint8_t dataSecond)
{
	uint16_t dataBoth = 0x0000;
	
	dataBoth = dataFirst;
	dataBoth = dataBoth << 8;
	dataBoth |= dataSecond;
	
	return dataBoth;
}

int main(int argc, char *argv[])
{
	if (! strcmp(argv[1], "-h") || ! strcmp(argv[1], "--help")  || argc < 3)
	{
		
		printf("\
 Syntax: 						\n\
   ./rso <option> <file>		\n\
								\n\
 Options: 						\n\
   -h		= display help.		\n\
   -a		= Analyse an RSO file (display the header data).	\n\
	\n\n");

		return 1;
	}
	
	printf("Opening file...   ");
	
	FILE *inFile;
	inFile = fopen(argv[2], "r");
	
	if (! inFile)
	{
		printf("error.\n");
		return 1;
	}
	else
	{
		printf("done.\n");
	}
	
	uint8_t  compression;
	
	int length1, length2;
	uint16_t length;
	
	int smplrate1, smplrate2;
	uint16_t smplrate;
	
	uint8_t padding1, padding2;		// Just used to check the padding...
	
	printf("Analysing file...   ");
	
	int x;
	int loop = 0;
	while ((x = getc(inFile)) != EOF)
	{
		if (loop == 0)
		{
			if (x != 1) {
				printf("error.\nMagic byte at offset 0 should be 01.\n");
				return 2;
			}
		}
		
		if (loop == 1)
		{
			if (x == 0 || x == 1)
			{
				compression = (uint8_t) x;
			}
			else
			{
				printf("error.\nCompression flag at offset 1 should be 00 or 01.\n");
				return 2;
			}
		}
		
		if (loop == 2)
		{
			length1 = x; 	
		}
		
		if (loop == 3)
		{
			length2 = x;
			
			length = cvt8to16(length1, length2);

		}
		
		if (loop == 4)
		{
			smplrate1 = x; 	
		}
		
		if (loop == 5)
		{
			smplrate2 = x;
			
			smplrate = cvt8to16(smplrate1, smplrate2);

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
				printf("error.\nPadding at offset 7 & 8 should be 00 00.\n");
				return 2;
			}
		}
		
		loop++;
	}
	
	fclose(inFile);
	
	printf("done.\n");
	
	printf("\n");
	
	if (! strcmp(argv[1], "-a") )
	{
		if ( (int) compression == 1)
		{	
			printf("Compression   = Yes\n");
		} else {
			printf("Compression   = No\n");
		}
		printf("Sample count  = %d\n", (int) length);		 // "Number of samples  = %04X\n" prints it in HEX.
		printf("Samplerate    = %dHz", (int) smplrate);
	}

}