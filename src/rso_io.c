/*
 *  A simple (and primitive) C library for reading
 *  and writing Lego Mindstorms NXT sound files in
 *  the RSO file format.
 *  
 *  Code is pretty much self-explanatory,
 *  see 'README' for details.
 *  
 *  Licensed under the GNU General Public License v3;
 *  see 'LICENSE'.
 */

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "rso_io.h"

// Used to convert two 8-bit values to a 16-bit value (i.e. the header data in the rso file)
uint16_t cvt8to16(uint8_t dataFirst, uint8_t dataSecond)
{
	uint16_t dataBoth = 0x0000;
	
	dataBoth = dataFirst;
	dataBoth = dataBoth << 8;
	dataBoth |= dataSecond;
	
	return dataBoth;
}
void cvt16to8(uint16_t dataAll, uint8_t arrayData[2])		// Don't ask me how this works, but it does it's job.
{
	arrayData[0] = (dataAll >> 8) & 0x00FF;
	arrayData[1] = dataAll & 0x00FF;
}

void readRsoFile(char *loc, uint8_t *samples, struct rso_header_data *header)
{
	assert(header != NULL);
	
	// Opens the file...
	
	printf("Opening RSO file...\t");
	
	FILE *inFile;
	inFile = fopen(loc, "r");
	
	if (! inFile)
	{
		printf("error.\n");
		fclose(inFile); return;
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
	
	printf("Analysing RSO file...\t");
	
	int x;
	int loop = 0;
	while ((x = getc(inFile)) != EOF)
	{
		if (loop == 0)
		{
			if (x != 1) {
				printf("error.\n'> Magic byte at offset 0 should be 01.\n");
				fclose(inFile); return;
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
				fclose(inFile); return;
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
				fclose(inFile); return;
			}
			
			printf("done.\n");
			printf("Reading sample data...\t");
		}
		
		if (loop >= 8 && loop < 65535)
		{
			samples[loop-8] = (uint8_t) x;

		}
		
		loop++;
	}
	printf("done.\n");
	
	printf("Closing RSO file...\t");
	fclose(inFile);		// Closes the file...
	printf("done.\n");
	
	
}


void writeRsoFile(char *loc, uint8_t *samples, struct rso_header_data *header)
{
	assert(header != NULL);
	
	// Opens the file...
	
	printf("Opening RSO file...\t");
	
	FILE *outFile;
	outFile = fopen(loc, "w");
	
	if (! outFile)
	{
		printf("error.\n");
		fclose(outFile); return;
	}
	else
	{
		printf("done.\n");
	}
	
	uint8_t samplerate8[2];
	cvt16to8(header->samplerate, samplerate8);
	
	uint8_t length8[2];
	cvt16to8(header->length, length8);
	
	
	//
	
	printf("Writing data...  \t");
	
	int loop = 0;
	while (loop < (header->length + 8))
	{
		if (loop == 0)
		{
			fputc(0x01, outFile); 	// The magic byte at offset 0
		}
		
		if (loop == 1)
		{
			fputc(0x00, outFile);	// The compression flag at offset 1; we can't do compression yet...
		}
		
		if (loop == 2)
		{
			fputc(length8[0], outFile); 	// The first byte of length
		}
		
		if (loop == 3)
		{
			fputc(length8[1], outFile);		// The second byte of length

		}
		
		if (loop == 4)
		{
			fputc(samplerate8[0], outFile); 	// The first byte of samplerate
		}
		
		if (loop == 5)
		{
			fputc(samplerate8[1], outFile); 	// The second byte of samplerate
		}
		
		if (loop == 6)
		{
			fputc(0x00, outFile);		// padding / loop flag
		}
		
		if (loop == 7)
		{
			fputc(0x00, outFile);		// padding / loop flag
		}
		
		if (loop >= 8 && loop < 65535)
		{
			fputc(samples[loop-8], outFile);
		}
		
		loop++;
	}
	printf("done.\n");
	
	printf("Closing RSO file...\t");
	fclose(outFile);		// Closes the file...
	printf("done.\n");
	
}

void readRsoHeader(char *loc, struct rso_header_data *header)
{
	assert(header != NULL);
	
	// Opens the file...
	
	printf("Opening RSO file...\t");
	
	FILE *inFile;
	inFile = fopen(loc, "r");
	
	if (! inFile)
	{
		printf("error.\n");
		fclose(inFile); return;
	}
	else
	{
		printf("done.\n");
	}
	
	// Extracts the header data and samples...
	
	// Some temporary variables
	
	uint8_t length1, length2;
	
	uint8_t smplrate1, smplrate2;
	
	uint8_t padding1, padding2;		// Just used to check the padding...
	
	//
	
	printf("Analysing RSO file...\t");
	
	int x;
	int loop = 0;
	while ((x = getc(inFile)) != EOF)
	{
		if (loop == 0)
		{
			if (x != 1) {
				printf("error.\n'> Magic byte at offset 0 should be 01.\n");
				fclose(inFile); return;
			}
		}
		
		else if (loop == 1)
		{
			if (x == 0 || x == 1)
			{
				header -> compression =  x;
			}
			else
			{
				printf("error.\n'> Compression flag at offset 1 should be 00 or 01.\n");
				fclose(inFile); return;
			}
		}
		
		else if (loop == 2)
		{
			length1 = x; 	
		}
		
		else if (loop == 3)
		{
			length2 = x;
			
			header -> length = cvt8to16(length1, length2);

		}
		
		else if (loop == 4)
		{
			smplrate1 = x; 	
		}
		
		else if (loop == 5)
		{
			smplrate2 = x;
			
			header -> samplerate = cvt8to16(smplrate1, smplrate2);

		}
		
		else if (loop == 6)
		{
			padding1 = x; 	
		}
		
		else if (loop == 7)
		{
			padding2 = x; 
			
			if ( cvt8to16(padding1, padding2) != 0)
			{
				printf("error.\n'> Padding at offset 7 & 8 should be 00 00.\n");
				fclose(inFile); return;
			}
		}
		
		else
		{
			break;
		}
		
		loop++;
	}
	printf("done.\n");
	
	printf("Closing RSO file...\t");
	fclose(inFile);		// Closes the file...
	printf("done.\n");
	
	
}