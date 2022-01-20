#include <stdio.h>
#include <stdlib.h>

void usage(const char *progname)
{
	fprintf(stderr, "Usage %s lattice_file \n", progname);
	exit(1);
}

int main(int argc, char * argv[])
{

	FILE *fp = NULL;
	const char * lattice_file_name = NULL;
	if(argc != 2){
		usage(argv[0]);
	}

	lattice_file_name = argv[1];

	parse_lattice(lattice_file_name);
	print_flat_lattice(stdout, "test:");
	
	return 0;
	
	fp = fopen(lattice_file_name, "rt");
	if(!fp){
		fprintf(stderr, "failed to  open file %s\n",
			lattice_file_name);
		perror("failed to open file ");
		exit(2);
	}
	
#ifdef DEBUG
	fprintf(stderr, "parsing file %s\n", lattice_file_name);
#endif
	yyparse(fp);
	return 0;
}
