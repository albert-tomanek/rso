/*
 *  The header to a simple (and primitive)
 *  C library for reading and writing 
 *  Lego Mindstorms NXT sound files in
 *  the RSO file format.
 *  
 *  Code is pretty much self-explanatory,
 *  see 'README' for details.
 *  
 *  Licensed under the GNU General Public License v3;
 *  see 'LICENSE'.
 */

#ifndef RSO_IO
  #define RSO_IO
  #define RSO_IO_VERSION 1
  
  struct rso_header_data
  {
	int compression;
	uint16_t length;
	uint16_t samplerate;
  };
  
  void readRsoFile (char *loc, uint8_t *samples, struct rso_header_data *header);
  void writeRsoFile(char *loc, uint8_t *samples, struct rso_header_data *header);
  
  void readRsoHeader(char *loc, struct rso_header_data *header);

#endif