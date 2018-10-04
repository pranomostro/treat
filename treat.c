#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "arg.h"
#include "config.h"

#define is_odigit(c) ('0' <= c && c <='7')

char* argv0;

static void usage(void);
size_t incarcerate(char* s);
void paste(size_t numfiles, char** ofnames);

static void usage(void)
{
	fprintf(stderr, "%s [-i insep] [-o outsep] [filters]\n", argv0);
	exit(1);
}

size_t incarcerate(char *s)
{
	static const char escapes[256] = {
		['"'] = '"',
		['\''] = '\'',
		['\\'] = '\\',
		['a'] = '\a',
		['b'] = '\b',
		['E'] = 033,
		['e'] = 033,
		['f'] = '\f',
		['n'] = '\n',
		['r'] = '\r',
		['t'] = '\t',
		['v'] = '\v'
	};
	size_t m, q;
	char *r, *w;

	for (r = w = s; *r;) {
		if (*r != '\\') {
			*w++ = *r++;
			continue;
		}
		r++;
		if (!*r) {
			fprintf(stderr, "null escape sequence\n");
			exit(7);
		} else if (escapes[(unsigned char)*r]) {
			*w++ = escapes[(unsigned char)*r++];
		} else if (is_odigit(*r)) {
			for (q = 0, m = 4; m && is_odigit(*r); m--, r++)
				q = q * 8 + (*r - '0');
			*w++ = q<255?q:255;
		} else if (*r == 'x' && isxdigit(r[1])) {
			r++;
			for (q = 0, m = 2; m && isxdigit(*r); m--, r++)
				if (isdigit(*r))
					q = q * 16 + (*r - '0');
				else
					q = q * 16 + (tolower(*r) - 'a' + 10);
			*w++ = q;
		} else {
			fprintf(stderr, "invalid escape sequence '\\%c'\n", *r);
			exit(7);
		}
	}
	*w = '\0';

	return w - s;
}


void paste(size_t numfiles, char** ofnames)
{
	size_t i, fieldlen;
	ssize_t rlen;
	char* field;
	FILE** outfifos;

	fieldlen=BUFSIZ;
	field=calloc(fieldlen, sizeof(char));
	outfifos=calloc(numfiles, sizeof(FILE*));

	for(i=0; i<numfiles; i++)
	{
		outfifos[i]=fopen(ofnames[i], "r");
		if(!outfifos[i])
		{
			fprintf(stderr, "%s: error: could not open FIFO %s, exiting.\n",
				argv0, ofnames[i]);
			exit(6);
		}
	}

	while(1)
	{
		for(i=0; i<numfiles; i++)
		{
			if((rlen=getline(&field, &fieldlen, outfifos[i]))<0)
				goto end;
			if(field[rlen-1]=='\n')
				field[rlen-1]='\0';

			fwrite(field, sizeof(char), rlen-1, stdout);
			if(i<numfiles-1)
				fwrite(outsep, sizeof(char), strlen(outsep), stdout);
		}
		fputc('\n', stdout);
	}

end:
	for(i=0; i<numfiles; i++)
		fclose(outfifos[i]);

	free(field);
	free(outfifos);
}

int main(int argc, char** argv)
{
	int i;
	size_t shcommlen, expectedlen, linelen;
	ssize_t rlen;
	char* dirname, * shcomm, * s, * t, * line;
	char** ifnames, ** ofnames;
	FILE** infifos;

	shcommlen=BUFSIZ;
	linelen=BUFSIZ;

	ARGBEGIN {
	case 'i':
		insep=EARGF(usage());
		incarcerate(insep);
		break;
	case 'o':
		outsep=EARGF(usage());
		incarcerate(outsep);
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
		fprintf(stderr, "%s: error: could not create temporary directory, exiting.\n",
			argv0);
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
			fprintf(stderr, "%s: error: could not create FIFO in %s, exiting.\n",
				argv0, dirname);
			exit(4);
		}
	}

	/* start the filters supplied in argv */

	for(i=0; i<argc+1; i++)
	{
		/* components: length of comm without %s + in + comm + out + null */
		expectedlen=(strlen(commfmt)-6)+strlen(ifnames[i])+
			     strlen(i==argc?defaultcomm:argv[i])+strlen(ofnames[i])+1;
		if(expectedlen>shcommlen)
		{
			shcommlen=expectedlen;
			shcomm=realloc(shcomm, expectedlen*sizeof(char));
			if(!shcomm)
			{
				fprintf(stderr, "%s: error: could not reallocate memory, exiting.\n",
					argv0);
				exit(2);
			}
		}
		snprintf(shcomm, expectedlen, commfmt, ifnames[i],
			 i==argc?defaultcomm:argv[i], ofnames[i]);

		switch(fork())
		{
		case 0:
			execl(shellpath, shellname, shellflag, shcomm, (char *) 0);
			exit(0);
			break;
		case -1:
			fprintf(stderr, "%s: error: could not fork, exiting.\n", argv0);
			exit(5);
			break;
		default:
			break;
		}
	}

	switch(fork())
	{
	case 0:
		paste(argc+1, ofnames);
		exit(0);
		break;
	case -1:
		fprintf(stderr, "%s: error: could not fork, exiting.\n", argv0);
		exit(5);
		break;
	default:
		break;
	}

	/* open the FIFOs */

	for(i=0; i<(argc+1); i++)
	{
		infifos[i]=fopen(ifnames[i], "w");
		setbuf(infifos[i], NULL); /* if this is not done, treat sometimes hangs */
		if(!infifos[i])
		{
			fprintf(stderr, "%s: error: could not open FIFO %s, exiting.\n",
				argv0, ifnames[i]);
			exit(6);
		}
	}

	/* split up the line and write it into the input FIFOs */

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
				fwrite(s, sizeof(char), rlen-(s-line), infifos[i]);
				s=line+rlen;
			}
			else
			{
				fwrite(s, sizeof(char), t-s, infifos[i]);
				s=t+strlen(insep);
			}
			fputc('\n', infifos[i]);
		}
		fwrite(s, sizeof(char), rlen-(s-line), infifos[i]);
		fputc('\n', infifos[i]);
		line[0]='\0';
	}

	for(i=0; i<(argc+1); i++)
		fclose(infifos[i]);

	/* wait for children to terminate */

	for(i=0; i<(argc+1); i++)
		wait(NULL);

	/* clean up */

	for(i=0; i<(argc+1); i++)
	{
		remove(ifnames[i]);
		remove(ofnames[i]);
		free(ifnames[i]);
		free(ofnames[i]);
	}
	free(line);
	free(shcomm);
	free(ifnames);
	free(ofnames);
	free(infifos);

	remove(dirname);
	free(dirname);

	return 0;
}
