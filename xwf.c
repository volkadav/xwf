/*
 *	xwf -- simple command-line xml well-formedness checker
 *
 *	[ adapted from perl code seen elsewhere, this is written
 *	  in pure C for speed and memory usage reasons (e.g. exec'd
 *	  in a find(1) command over a large number of files) ]
 *
 *	M. Jackson, Jan 2005.  Public domain.
 * 
 */

/* standard includes (io, EXIT_*, etc.) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* for stat(2) */
#include <sys/types.h>
#include <sys/stat.h>
/* for getopt(3) [and support of stat(2)] */
#include <unistd.h>
/* for all xml functions */
#include <expat.h>

int main(int argc, char** argv) {
	/* OPTIONS */
	int verbose = 0;	/* whether to print text to stdout or not {0 = no, 1 = yes} */
				/* if !verbose, return code is only output (handy for scripts) */
	int move = 0;		/* whether to move files containing invalid XML to filename.INVALID */
	int filearg = 1;	/* which idx of argv is the filename */
	int optchar; 		/* for getopt(3) */
	
	/* FILE-RELATED */
	FILE* infile; 		/* xml file being processed */
	size_t filesize;	/* size of infile in bytes */
	struct stat stat_info; 	/* return struct from stat(2) call (for filesize) */
	char* curpath;		/* holds a copy of argv[filearg] */
	char* newpath;		/* will hold the new path/name of the file if it is invalid */

	/* CONTENT-RELATED */
	void* buffer;		/* buffer for xml file contents */
	XML_Parser parser;	/* the expat XML parser instance */

	/* command-line processing */

	while((optchar = getopt(argc, argv,"mvh")) != -1) {
		switch(optchar) {
			case 'm':
				move = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
			case '?':
			default:
				printf("usage: %s -h | [-mv] filename\n",argv[0]);
				printf("\t-h: this help message\n");
				printf("\t-m: moves file to file.INVALID if file is invalid XML\n");
				printf("\t-v: verbose reporting of errors, otherwise no stdout output\n");
				printf("\t    and EXIT_SUCCESS -> file OK / EXIT_FAILURE -> file not OK\n");
				printf("\t    (i.e. check $? in your shell script or whatever\n");
				return EXIT_FAILURE;
		}
	} /* to my understanding of getopt(3), after this, the filename will be at the end of argv no matter
	     where it was to begin with as getopt permutes argv such that non-options are at the end */
	filearg = argc - 1; /* sets argv array idx for file to last element of the array */
	
	/* file processing */
	
	infile = fopen(argv[filearg],"r");
	if (infile == NULL) {
		fprintf(stderr,"Can't open '%s' for reading, exiting.\n",argv[filearg]);
		return EXIT_FAILURE;
	}
	
	if (stat(argv[filearg],&stat_info) == -1) {
		fprintf(stderr,"Can't stat '%s', exiting.\n",argv[filearg]);
		return EXIT_FAILURE;
	}
	filesize = (size_t)stat_info.st_size;
	
	buffer = malloc(filesize);
	if (buffer == NULL) {
		fprintf(stderr,"Can't allocate %d bytes of memory to read '%s', exiting.\n",
				(int)filesize,
				argv[filearg]);
		return EXIT_FAILURE;
	}

	fread(buffer,1,(int)filesize,infile); /* "1" is size of data element, in this case one byte */

	if(ferror(infile)) {
		fprintf(stderr, "Something wierd happened reading from '%s', exiting.\n",argv[filearg]);
		free(buffer);
		return EXIT_FAILURE;
	}
	
	if(fclose(infile)) {
		fprintf(stderr,"Can't close '%s' (!?), exiting.\n", argv[filearg]);
		free(buffer);
		return EXIT_FAILURE;
	}
	
	/* XML processing */

	parser = XML_ParserCreate(NULL); /* NULL means don't override document's char encoding */
	
	if (XML_Parse(parser,(char*)buffer,(int)filesize,1) == XML_STATUS_ERROR) { 
		/* "1" above is the parameter 'isFinal', i.e. no more doc content */	
		if (verbose == 1) {
			printf("'%s' is NOT well-formed XML.\n",argv[filearg]);
			printf("%s on line %d.\n",
					XML_ErrorString(XML_GetErrorCode(parser)),
					XML_GetCurrentLineNumber(parser));
		}

		if (move == 1) {
			curpath = (char *)malloc(strlen(argv[filearg]) + 1); /* +1 for terminal \0 */
			
			if (curpath == NULL) {
				fprintf(stderr,"Can't allocate %d bytes of memory for file processing.\n",
						strlen(argv[filearg]));
				XML_ParserFree(parser);
				free(buffer);
				return EXIT_FAILURE;
			}
			
			curpath = strcpy(curpath,argv[filearg]);
			newpath = strcat(argv[filearg],".INVALID");
			if (verbose == 1) {
				printf("moving '%s' to '%s'.\n",curpath,newpath);
			}
			(void)rename(curpath,newpath); /* cast to void because if this is erroring out, there's not
							  much we can do about it so the rval is kind of moot */
			free(curpath);
		} 

		XML_ParserFree(parser);
		free(buffer);
		return EXIT_FAILURE;
	}

	if (verbose == 1) {
		printf("'%s' is well-formed XML.\n",argv[filearg]);
	}
	
	XML_ParserFree(parser);
	free(buffer);
	return EXIT_SUCCESS;
}
