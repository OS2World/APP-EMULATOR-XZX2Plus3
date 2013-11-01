/* XZX tape format to Spectrum +3/2A converter V0.1
 *
 * This file will convert the given XZX tape file to Spectrum +3/2A format.
 * The new file can then be transfered directly to a +3/2A disc, using e.g
 * 22DISK, and be loaded as normal.
 * If the file doesn't contain the header 'ZXF1', the user will be asked
 * if it should be converted anyway, if so it's possible to specify a which
 * filetype it should be, eg. BASIC or CODE.
 *
 * NOTE: The old file will be lost forever!! 
 * 
 * This is totally Freeware made by:
 *	Thomas A. Kjaer 
 *	email: takjaer@daimi.aau.dk
 *
 * Any bugs and improvements could be reported to me, and then if I have the
 * time for it, I'll try to fix it.
 *
 * Made with GCC under Linux 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/stat.h>

void print_header(unsigned char *);
unsigned int make_header(unsigned char *, unsigned char *);
int make_tapeheader(unsigned char, unsigned char *, unsigned char *); 

unsigned char tapeheader[17];
unsigned char plus3header[128];
char XZXID[4] = "ZXF1";
char tempfile[] = "tmpxzxconv.$$$";

main(int argc, char *argv[])
{
	FILE *fp, *tp;		/* filepointers fp = source, tp = temp */
	char *filename;		/* filename for source */
	char checkid[4];	/* for file ID check */
	unsigned int i, source_len;
	unsigned char c;
	unsigned char *tempbuffer;	/* temporary for raw tape data */
	
	if (argc !=2) {
		fprintf(stderr, "\nxzx2plus - xzx tape file to Spectrum +3 converter\n");
		fprintf(stderr, "Usage: xzx2plus file\n");
		fprintf(stderr, "\nFreeware 1994 by Thomas A.Kjaer takjaer@daimi.aau.dk\n\n");
		exit(1);
	};
	
	filename=malloc(strlen(argv[1]));
	strcpy(filename, argv[1]);
	printf("\nConverting file: %s\n", filename);
	
/* open source file and read important informations from the header.
 * But first check for ZXF1 identification.
 */
 
 	if ((fp = fopen(filename, "r")) == NULL) {
 		fprintf(stderr, "%s: can't open %s\n\n", argv[0], argv[1]);
 		close(fp);
 		exit(1);
 	}
	
	fscanf(fp, "%4s", checkid);
	if (strncmp(XZXID, checkid, 4) != 0) {
		fprintf(stderr, "%s is not a XZX file\n", filename);
		fprintf(stderr, "What type should it be ?:\n");
		fprintf(stderr, "\t0=BASIC\n\t1=Number Array\n");
		fprintf(stderr, "\t2=Chr. Array\n\t3=CODE\n");
		fprintf(stderr, "Default is (3) CODE\n");	
		scanf("%d", &i);
		if (i < 0 || i > 3) fprintf(stderr, "Invalid type, using type %d\n", i = 3);
		/* make new tapeheader, type i of size (fp) */ 
		if (make_tapeheader(i, tapeheader, filename) == -1) {
			close(fp);
			exit(1);
		}
		print_header(tapeheader);
		source_len = make_header(tapeheader, plus3header);
		if ((fp = freopen(filename, "r", fp)) == NULL) {
			fprintf(stderr, "Couldn't reopen file %s\n", filename);
			close(fp);
			exit(1);
		}
	}
/* Filecheck O.K. now read the tapeheader */
	else {
		if ((fread(tapeheader, sizeof(char), 17, fp)) != 17) {
			fprintf(stderr, "error: unexpected EOF\n");
			close(fp);
			exit(1);
		}		
		print_header(tapeheader);
		source_len = make_header(tapeheader, plus3header);
	};
	
/* open temporary file and write header */
	if ((tp = fopen(tempfile, "w")) == NULL) {
		fprintf(stderr, "error: couldn't write temporary file\n");
		close(tp);
		remove(tempfile);
		close(fp);
		exit(1);
	}
	for (i = 0; i < 128; fputc(plus3header[i++], tp));
/* append raw data from source file */
	if ((tempbuffer = malloc(source_len)) == NULL) {
		fprintf(stderr, "error: memory allocation error\n");
		close(tp);
		remove(tempfile);
		close(fp);
		exit(1);
	}
	fprintf(stderr, "reading %d bytes\n", source_len);
	if ((fread(tempbuffer, sizeof(char), source_len, fp)) != source_len) {
		fprintf(stderr, "error: unexpected EOF reading data\n");
		close(tp);
		remove(tempfile);
		close(fp);
		exit(1);
	}
	if ((fwrite(tempbuffer, sizeof(char), source_len, tp)) != source_len) {
		fprintf(stderr, "error: error in writing file\n");
		close(tp);
		remove(tempfile);
		close(fp);
		exit(1);
	}		
	remove(filename);
	rename(tempfile, filename);
	close(tp);
	remove(tempfile);
	close(fp);
	exit(0);
}

/* just prints some informations from the tape header */
void print_header(unsigned char *header)
{
	char filetype;
	unsigned int parm11_12, parm13_14, parm15_16;
	int n;
	
	for (n = 1; n < 11; fputc(header[n++], stderr));
	fputc('\n', stderr);
	filetype = header[0];
	parm11_12 = 256*header[12]+header[11];
	parm13_14 = 256*header[14]+header[13];
	parm15_16 = 256*header[16]+header[15];
	switch(filetype) {
	case 0: fprintf(stderr, "BASIC\n");
		fprintf(stderr, "Length: %d\n", parm11_12);
		fprintf(stderr, "AUTO-Line: %d\n", parm13_14);
		fprintf(stderr, "BAS-Length: %d\n", parm15_16);
		break;
	case 3: fprintf(stderr, "CODE\n");
		fprintf(stderr, "Length: %d\n", parm11_12);
		fprintf(stderr, "Start: %d\n", parm13_14);
		break;
	};
	fprintf(stderr, "\n");
}

/* Makes the +3DOS 128 bytes header from the information in header 
 * returns the length of the file as found in the header
 */
unsigned int make_header(unsigned char *header, unsigned char *p3header)
{
	unsigned char p3prefix[10] = {'P','L','U','S','3','D','O','S',0x1a,0x01};
	unsigned int totallength;
	unsigned int chksum;
	unsigned int n;
		
	totallength = 128+header[11]+256*header[12];
	printf("PLUS3DOS length = %d\n", totallength);
	memcpy(plus3header, p3prefix, 10);
	plus3header[15] = header[0];	/* filetype */
	memcpy(&plus3header[16], &header[11], 6); /* other parametres */
	plus3header[11] = totallength % 256;
	plus3header[12] = totallength / 256;
	/* now make the checksum byte */
	chksum = 0;
	for (n = 0; n < 127; chksum = chksum + plus3header[n++]);
	printf("PLUS3DOS checksum byte = %d\n", chksum % 256);
	plus3header[127] = chksum % 256;
	
	return (totallength-128);
}

/* Makes a new header from datas given by the user 
 * returns -1 on error
 */
int make_tapeheader(unsigned char filetype, unsigned char *header, unsigned char *file)
{
	struct stat *stbuf;
	unsigned int parm11_12, parm13_14, parm15_16;
	
	fprintf(stderr, "Making header for file: %s with ", file);
	if (stat(file, stbuf) == -1) {
		fprintf(stderr, "make_tapeheader can't access %s\n", file);
		return (-1);
	}
	parm11_12 = stbuf->st_size;
	fprintf(stderr, "length = %d\n", parm11_12);
	
	/* Set filetype and length of block */
	header[0] = filetype;
	header[11] = parm11_12 % 256; 
	header[12] = parm11_12 / 256;
	switch(filetype) {
	case	0: fprintf(stderr, "BASIC file:\n");
		   fprintf(stderr, "AUTO-Line: ");
		   scanf("%d", &parm13_14);
		   if (parm13_14 < 0 || parm13_14 >32768) parm13_14 = 32768;
		   fprintf(stderr, "BAS-Length: ");
		   scanf("%d", &parm15_16);
		   break;
	case	3: fprintf(stderr, "CODE file:\n");
		   fprintf(stderr, "Start address: ");
		   scanf("%d", &parm13_14);
		   parm15_16 = 0;
		   break;
	}
	header[13] = parm13_14 % 256; header[14] = parm13_14 / 256;
	header[15] = parm15_16 % 256; header[16] = parm15_16 / 256;
	return (0);	
}