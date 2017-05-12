#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "arg.h"
#include "config.h"

char* argv0;

static void usage(void)
{
	fprintf(stderr, "%s [-s insep] [-o outsep] [filters]\n", argv0);
	exit(1);
}

int main(int argc, char** argv)
{
	int i;
	size_t shcommlen, expectedlen, jolen, pastelen, linelen;
	ssize_t rlen;
	char* dirname, * shcomm, * pastecomm, * jonames, * s, * t, * line;
	char** ifnames, ** ofnames;
	FILE** infifos;

	shcommlen=BUFSIZ;
	linelen=BUFSIZ;

	ARGBEGIN {
	case 's':
		insep=EARGF(usage());
		break;
	case 'o':
		outsep=EARGF(usage());
		break;
	default:
		usage();
	} ARGEND;

	shcomm=calloc(shcommlen, sizeof(char));
	line=calloc(linelen, sizeof(char));
	dirname=calloc(strlen(template)+1, sizeof(char));
	ifnames=calloc(argc+1, sizeof(char*));
	ofnames=calloc(argc+1, sizeof(char*));
	infifos=calloc(argc+1, sizeof(FILE*));
	if(!dirname||!ifnames||!ofnames||!infifos||!shcomm)
	{
		fprintf(stderr, "%s: error: could not allocate memory, exiting.\n", argv0);
		exit(2);
	}

	strncpy(dirname, template, strlen(template)+1);
	dirname=mkdtemp(dirname);
	if(!dirname)
	{
		fprintf(stderr, "%s: error: could not create temporary directory, exiting.\n", argv0);
		exit(3);
	}

	/* create FIFOs in dirname */

	for(i=0; i<(argc+1); i++)
	{
		/* components: template + "/" + prefix + max length of number + null */
		ifnames[i]=calloc(strlen(template)+1+strlen(inprefix)+10+1, sizeof(char));
		ofnames[i]=calloc(strlen(template)+1+strlen(outprefix)+10+1, sizeof(char));
		sprintf(ifnames[i], "%s/%s%d", dirname, inprefix, i);
		sprintf(ofnames[i], "%s/%s%d", dirname, outprefix, i);
		if(mkfifo(ifnames[i], 0600)<0||mkfifo(ofnames[i], 0600)<0)
		{
			fprintf(stderr, "%s: error: could not create FIFO in %s, exiting.\n", argv0, dirname);
			exit(4);
		}
	}

	/* join together the output fifo names */

	for(i=0, jolen=0; i<(argc+1); i++)
		jolen+=strlen(ofnames[i])+1; /* outname + " " */

	jonames=calloc(jolen, sizeof(char));
	if(!jonames)
	{
		fprintf(stderr, "%s: error: could not allocate memory, exiting.\n", argv0);
		exit(2);
	}

	s=jonames;
	for(i=0; i<(argc+1); i++)
	{
		strncpy(s, ofnames[i], strlen(ofnames[i]));
		s+=strlen(ofnames[i]);
		if(i<argc)
			*s++=' ';
	}
	*s++='\0';

	/* create the command for pasting from the joined output fifo names */

	/* explicit is better than implicit */
	/* components: length of pastefmt without %s + outsep + jolen - jolen null + null */
	pastelen=(strlen(pastefmt)-4)+strlen(outsep)+jolen-1+1;
	pastecomm=calloc(pastelen, sizeof(char));
	snprintf(pastecomm, pastelen, pastefmt, outsep, jonames);
	printf("pastecomm: \"%s\"\n", pastecomm);

	for(i=0; i<argc+1; i++)
	{
		/* components: length of comm without %s + in + comm + out + null */
		expectedlen=(strlen(commfmt)-6)+strlen(ifnames[i])+strlen(i==argc?defaultcomm:argv[i])+
			    strlen(ofnames[i])+1;
		if(expectedlen>shcommlen)
		{
			shcommlen=expectedlen;
			shcomm=realloc(shcomm, expectedlen*sizeof(char));
			if(!shcomm)
			{
				fprintf(stderr, "%s: error: could not reallocate memory, exiting.\n", argv0);
				exit(2);
			}
		}
		snprintf(shcomm, expectedlen, commfmt, ifnames[i], i==argc?defaultcomm:argv[i], ofnames[i]);
		/* system(shcomm); */
		printf("shcomm: \"%s\"\n", shcomm);
	}

	while((rlen=getline(&line, &linelen, stdin))!=-1)
	{
		if(line[rlen-1]=='\n')
			line[--rlen]='\0';
		s=line;
		for(i=0; i<argc; i++)
		{
			t=strstr(s, insep);
			if(!t)
			{
				fwrite(s, sizeof(char), rlen-(s-line), stdout);
				s=line+rlen;
			}
			else
			{
				fwrite(s, sizeof(char), t-s, stdout);
				s=t+strlen(insep);
			}
			fputc('\n', stdout);
		}
		fwrite(s, sizeof(char), rlen-(s-line), stdout);
		fputc('\n', stdout);
		line[0]='\0';
	}

	remove(dirname);

	for(i=0; i<(argc+1); i++)
	{
		remove(ifnames[i]);
		remove(ofnames[i]);
		free(ifnames[i]);
		free(ofnames[i]);
	}
	free(line);
	free(jonames);
	free(pastecomm);
	free(shcomm);
	free(ifnames);
	free(ofnames);
	free(infifos);

	remove(dirname);
	free(dirname);

	return 0;
}
