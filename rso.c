#include <stdio.h>
#include <stdint.h> 	// for uint8_t and uint16_t
#include <string.h>		// for strcmp();
#include <stdlib.h>		// for memset();

#include <sndfile.h>	// The libsndfile header; can be obtained by 'sudo apt-get install libsndfile-dev' or from http://www.mega-nerd.com/libsndfile/

#define MAX_DIVISIONS 32
#define DUMP_WIDTH 16

/* Should compile on any GCC compatible compiler with libsndfile installed;
 * use 'gcc rso.c -L /usr/lib/i386-linux-gnu/ -lsndfile -o rso'
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

struct rso_header_data
{
	int compression;
	uint16_t length;
	uint16_t samplerate;
};

int readRsoFile(char *loc, uint8_t *samples, struct rso_header_data *header)
{
	// Opens the file...
	
	printf("Opening file...\t");
	
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
	
	printf("Analysing file...\t");
	
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
			printf("Reading sample data...\t");
		}
		
		if (loop >= 8 && loop < 65535)
		{
			samples[loop-8] = (uint8_t) x;

		}
		
		loop++;
	}
	printf("done.\n");
	
	printf("Closing file...\t");
	fclose(inFile);		// Closes the file...
	printf("done.\n");
	
	
}

int main(int argc, char *argv[])
{
	if (! strcmp(argv[1], "-h") || ! strcmp(argv[1], "--help")  || argc < 3)
	{
		
		printf("\
 Syntax: 				\n\
   ./rso <option> <file> [outfile]	\n\
					\n\
 Options: 				\n\
   -h		= display help.		\n\
   -a		= Analyse an RSO file (display the header data).		\n\
   -d		= Dump the sample data to the screen as hexadecimal.		\n\
   -w		= Convers an RSO file to <outfile> in wav format.		\n\
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
	
	struct rso_header_data *rso_header = malloc(sizeof(struct rso_header_data));
	
	
	// Now for interpreting the cmdline arguments...
	
	if (! strcmp(argv[1], "-a") )
	{
		readRsoFile(argv[2], &samples[0], rso_header);
		
		if ( rso_header -> compression == 1)
		{	
			printf("Compression   = Yes\n");
		} else {
			printf("Compression   = No\n");
		}
		printf("Sample count  = %d\n", (int) rso_header -> length);		 // "Number of samples  = %04X\n" prints it in HEX.
		printf("Samplerate    = %dHz", (int) rso_header -> samplerate);
	}
	
	if (! strcmp(argv[1], "-d"))
	{
		readRsoFile(argv[2], &samples[0], rso_header);
		
		printf("Dumping chunk 1 as hex:");
		
		for (int i = 0; i < (rso_header->length - 8); i++)
		{
			if (i % DUMP_WIDTH == 0)  printf("\n   ");
			
			printf("%02X ", (int) samples[i]);
		}
	}
	
	if (! strcmp(argv[1], "-w") )	// They want to convert to WAV
	{
		readRsoFile(argv[2], &samples[0], rso_header);		// Reads our rso file into the array of 8-bit ints and the header_data struct
		
		struct SF_INFO *wav_header = malloc(sizeof(SF_INFO));
		
		wav_header -> frames 		= rso_header -> length;
		wav_header -> samplerate	= rso_header -> samplerate;
		wav_header -> channels 		= 1;
		wav_header -> format 		= SF_FORMAT_WAV | SF_FORMAT_PCM_U8;		// i.e. WAV format, unsigned 8-bit PCM, since the samples are uint8_t.
		wav_header -> sections		= 1;  // Not too sure what these two are meant to be...
		wav_header -> seekable 		= 0;  //
		
		printf("Opening WAV file...\t");
		SNDFILE* wav_file;
		wav_file = sf_open(argv[3], SFM_WRITE, wav_header);
		printf("done.\n");
		
		printf("Writing sample data to file...\t");
		
		short samples16[65534]; 	// Will be the equivalent of samples[], but with short instead of uint8_t
		
		for (int i = 0; i < (rso_header->length); i++)
		{
			short tmp = (short)(samples[i]-128);		// Converts the sample to signed
			
			samples16[i] = tmp << 8 /*(samples[i] << 8)*/;
			//printf("%d ", (int) tmp);
		}
		
		//for (int i = 0; i < (rso_header->length); i++)
		//{
			
		//}
		
		sf_write_short(wav_file, samples16, (rso_header->length));
		
		printf("done.\n");
		
		printf("Closing WAV file...\t");
		
		sf_close(wav_file);
		
		printf("done.\n");
	}
	

	exit(0);
}