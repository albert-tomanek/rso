#include <stdio.h>
#include <stdint.h> 	// for uint8_t and uint16_t
#include <string.h>		// for strcmp(); and strlen();
#include <stdlib.h>		// for memset();

#include <sndfile.h>	// The libsndfile header; can be obtained by 'sudo apt-get install libsndfile-dev' or from http://www.mega-nerd.com/libsndfile/

#include "rso_io.h"		// The header file with functions for handling RSO files..

#define DUMP_WIDTH 16

/* Should compile on any GCC compatible compiler with libsndfile installed;
 * use 'gcc rso.c -L /usr/lib/i386-linux-gnu/ -lsndfile -o rso'
 * For license see 'LICENSE'.
 *
 */

enum filetype {
	WAV_FILE,
	RSO_FILE
	};

char* get_extention(char *filename)
{
	int filename_len = strlen(filename);
	//static char lastfourchars[5] = {'\0','\0','\0','\0','\0'};
	char *lastfourchars = malloc(sizeof(char) * 4);
	
	strncpy(&lastfourchars[0], &filename[filename_len - 4], 4);
	
	printf("%s\n", lastfourchars);
	
	return lastfourchars;
	//return WAV_FILE;
}

///////////

void rso_to_wav(char *infile, char *outfile);
void wav_to_rso(char *infile, char *outfile);

int main(int argc, char *argv[])
{
	
	if (argc < 2)
	{
		printf("\
 Syntax: 								\n\
   ./rso <option> <file> [outfile]		\n\
										\n\
 Options: 								\n\
   -a		= Analyse an RSO file (display the header data).		\n\
   -d		= Dump RSO sample data to the screen as hexadecimal.	\n\
   -w		= Converts an RSO file to <outfile> in WAV format.		\n\
   -r		= Converts a WAV file to <outfile> in RSO format.		\n\
	\n\n");
		
		return 0;
	}
	
	// Now for interpreting the cmdline arguments...
	
	if (! strcmp(argv[1], "-a") )
	{
		// And, most importantly,
		uint8_t samples[65534];
		
		/* 65534 is the maximum number of samples in an RSO file,
		 * since the field occupies 2 bytes in the header.
		 * However, a potential workaround for this could exist
		 * by putting another header at the end of the sample data,
		 * thus allowing for another set of 65534 samples.
		 */
		
		memset(samples, 0, 65534);
		
		struct rso_header_data *rso_header = malloc(sizeof(struct rso_header_data));
			
		readRsoFile(argv[2], &samples[0], rso_header);
		
		printf("Compression   = ");
		if ( rso_header -> compression == 1)
		{	
			printf("Yes\n");
		} else {
			printf("No\n");
		}
		printf("Sample count  = %d\n", (int) rso_header -> length);		 // "Number of samples  = %04X\n" prints it in HEX.
		printf("Samplerate    = %dHz", (int) rso_header -> samplerate);
	}
	
	else if (! strcmp(argv[1], "-d"))
	{
		// And, most importantly,
		uint8_t samples[65534];
		
		/* 65534 is the maximum number of samples in an RSO file,
		 * since the field occupies 2 bytes in the header.
		 * However, a potential workaround for this could exist
		 * by putting another header at the end of the sample data,
		 * thus allowing for another set of 65534 samples.
		 */
		
		memset(samples, 0, 65534);
		
		struct rso_header_data *rso_header = malloc(sizeof(struct rso_header_data));
		
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
		rso_to_wav(argv[2], argv[3]);
	}
	
	else if (! strcmp(argv[1], "-r") )	// They want to convert from WAV to RSO.
	{
		wav_to_rso(argv[2], argv[3]);
	}
	
	
	return 0;
}


void rso_to_wav(char *infile, char *outfile)
{

	// And, most importantly,
	uint8_t samples[65534];
	
	/* 65534 is the maximum number of samples in an RSO file,
	 * since the field occupies 2 bytes in the header.
	 * However, a potential workaround for this could exist
	 * by putting another header at the end of the sample data,
	 * thus allowing for another set of 65534 samples.
	 */
	
	memset(samples, 0, 65534);
	
	struct rso_header_data *rso_header = malloc(sizeof(struct rso_header_data));
		
	readRsoFile(infile, &samples[0], rso_header);		// Reads our rso file into the array of 8-bit ints and the header_data struct
	
	struct SF_INFO *wav_header = malloc(sizeof(SF_INFO));
		
	wav_header -> frames 		= rso_header -> length;
	wav_header -> samplerate	= rso_header -> samplerate;
	wav_header -> channels 		= 1;
	wav_header -> format 		= SF_FORMAT_WAV | SF_FORMAT_PCM_U8;		// i.e. WAV format, unsigned 8-bit PCM, since the samples are uint8_t.
	wav_header -> sections		= 1;  // Not too sure what these two are meant to be...
	wav_header -> seekable 		= 0;  //
		
	printf("Opening WAV file...\t");
	SNDFILE* wav_file;
	wav_file = sf_open(outfile, SFM_WRITE, wav_header);
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

void wav_to_rso(char *infile, char *outfile)
{
	// And, most importantly,
	uint8_t samples[65534];
	
	/* 65534 is the maximum number of samples in an RSO file,
	 * since the field occupies 2 bytes in the header.
	 * However, a potential workaround for this could exist
	 * by putting another header at the end of the sample data,
	 * thus allowing for another set of 65534 samples.
	 */
	
	memset(samples, 0, 65534);

	struct SF_INFO *wav_header = malloc(sizeof(SF_INFO));	// Allocates us memory for the wav header struct...
	
	memset(wav_header, 0, sizeof(SF_INFO)); 	// And frees it of any junk that came in it.
	
	printf("Opening WAV file...\t");
	SNDFILE* wav_file;
	wav_file = sf_open(infile, SFM_READ, wav_header);
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
	
	writeRsoFile(outfile, &samples[0], rso_header);

}