/*
	Configuration for the treat runtime. Recompile and reinstall
	to apply.
*/

/* Default input/output separators */

char* insep=" ";
char* outsep="\t";

/* The directory where the temporary FIFOs should be kept */

static const char* template="/tmp/treat.XXXXXX";

/* The prefixes of the names of the input/output FIFOs */

static const char* inprefix="in.";
static const char* outprefix="out.";

/* The default command, where no filter is specified */

static const char* defaultcomm="cat";

/* The shell used for executing the filters given as arguments */

static const char* shellpath="/bin/sh";
static const char* shellname="sh";
static const char* shellflag="-c";

/*
	The command used for pasting the output from the FIFOs together.
	The first %s is replaced by outsep, the second %s is replaced
	by a list of all output FIFOs joined with " ".
*/

static const char* pastefmt="paste -d '%s' %s";

/*
	The structure of a command:
	The first %s represents the input FIFO, the second %s represents
	the command passed as a command line argument, and the third
	%s represents the output FIFO. Note that the shell has to be
	capable of commands beginning with redirections.
*/

static const char* commfmt="<%s %s >%s";
