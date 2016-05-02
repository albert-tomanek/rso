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

struct rso_header_data
{
	int compression;
	uint16_t length;
	uint16_t samplerate;
};

int readRsoFile(char *loc, uint8_t *samples, struct rso_header_data *header)
{
	// Opens the file...
	
	printf("Opening RSO file...\t");
	
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
	
	printf("Analysing RSO file...\t");
	
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
	
	printf("Closing RSO file...\t");
	fclose(inFile);		// Closes the file...
	printf("done.\n");
	
	
}


int writeRsoFile(char *loc, uint8_t *samples, struct rso_header_data *header)
{
	// Opens the file...
	
	printf("Opening RSO file...\t");
	
	FILE *outFile;
	outFile = fopen(loc, "w");
	
	if (! outFile)
	{
		printf("error.\n");
		exit(1);
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

int main(int argc, char *argv[])
{

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
	
	else if (! strcmp(argv[1], "-d"))
	{
		readRsoFile(argv[2], &samples[0], rso_header);
		
		printf("Dumping chunk 1 as hex:");
		
		for (int i = 0; i < (rso_header->length - 8); i++)
		{
			if (i % DUMP_WIDTH == 0)  printf("\n   ");
			
			printf("%02X ", (int) samples[i]);
		}
	}
	
	else if (! strcmp(argv[1], "-w") )	// They want to convert from RSO to WAV
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
		
		for (int i = 0; i < (rso_header->length); i++)	// Converts the samples to 16-bit, (let's hope that short is 16-bit...)
		{
			short sample = (short)(samples[i]-128);		// Converts the sample to signed
			
			samples16[i] = sample << 8 ;
			//printf("%d ", (int) sample << 8);
		}
		
		sf_write_short(wav_file, samples16, (rso_header->length));
		
		printf("done.\n");
		
		printf("Closing WAV file...\t");
		
		sf_close(wav_file);
		
		printf("done.\n");
	}
	
	else if (! strcmp(argv[1], "-r") )	// They want to convert from WAV to RSO.
	{
		struct SF_INFO *wav_header = malloc(sizeof(SF_INFO));	// Allocates us memory for the wav header struct...
		
		memset(wav_header, 0, sizeof(SF_INFO)); 	// And frees it of any junk that came in it.
		
		printf("Opening WAV file...\t");
		SNDFILE* wav_file;
		wav_file = sf_open(argv[2], SFM_READ, wav_header);
		printf("done.\n");
		
		short samples16[65534];
		
		// Here, we check whether the wav file's parameters are compatible with the RSO format; if not, we change them.
		
		printf("Analising WAV file...\t");
		
		if (wav_header -> frames > 65534)
		{
			wav_header -> frames = 65534;
			printf("warning:\nThe given WAV file contains more than 65534 samples;\nAny further samples will be ignored.\n");
		}
		else if (wav_header -> samplerate > 65534)
		{
			wav_header -> samplerate = 65534;
		}
		else
		{
			printf("done.\n");
		}
		
		// Read the sample data
		
		sf_read_short(wav_file, samples16, wav_header->frames);
		
		
		for (int i = 0; i < (wav_header -> frames); i++)
		{													// Converts the sample data to unsigned 8-bit
			unsigned short sample = (short)(samples16[i] / 2 + 0x8000);	// i.e. half of 65534
		
			samples[i] = sample >> 8 ;
			
			printf("%d ", (int) samples[i]);
		}
		
		// Take care of the RSO header...
		
		struct rso_header_data  *rso_header = malloc( sizeof( struct rso_header_data ) );
		
		rso_header -> compression = 0;
		rso_header -> samplerate  = (uint16_t) (wav_header -> samplerate);
		rso_header -> length      = (uint16_t) (wav_header -> frames);
		
		writeRsoFile(argv[3], &samples[0], rso_header);
		
	}
	
	else
	{
		printf("\
 Syntax: 	\n\
   ./rso <option> <file> [outfile]		\n\
\n\
 Options: 	\n\
   -h		= display help.		\n\
   -a		= Analyse an RSO file (display the header data).	\n\
   -d		= Dump the sample data to the screen as hexadecimal.	\n\
   -w		= Converts an RSO file to <outfile> in WAV format.	\n\
   -r		= Converts a WAV file to <outfile> in RSO format.	\n\
	\n\n");
	}
	
	return 0;
}