#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	// Get the directory passed in
	char * directory;
	directory = argv[0];
	printf("\nMoving into Directory Program \n");
	printf("argv[0]: %s (Size %d)", directory, strlen(directory));
	printf("\n \n");

	// Get the current working directory
	char currentWorkingDirectory[200];
	if (getcwd(currentWorkingDirectory, 200) == -1)
	{
		// Throw an error if we are unable to obtain the current working directory
		printf("Unable to obtain current working directory \n \n");
		exit();
	}
	printf("Current working directory obtained: ");
	printf(currentWorkingDirectory);
	printf("\n \n");

	// Check to see if we are trying to read the root directory
	if (strlen(currentWorkingDirectory) == 1 && strlen(directory) == 0)
	{
		// Throw an error if we are trying to read the root directory because it is special
		printf("Currently trying to read the root directory \nABORT ABORT \n \n");
		exit();
	}

	int fd = opendir(directory);

	printf("File Directory Number: %d \n \n", fd);

	if (fd == 0)
	{
		printf("Unable to obtain a file number \n ABORT ABORT \n \n");
		exit();
	}

	printf("File number obtained, time to read some files bro \n \n");

	// END OF PROGRAM
	exit();
}