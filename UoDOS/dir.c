#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void main(int argc, char *argv[])
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
		exec("/usrbin/sh.exe", argv);
	}
	printf("Current working directory obtained: ");
	printf(currentWorkingDirectory);
	printf("\n \n");

	// Check to see if we are trying to read the root directory
	if (strlen(currentWorkingDirectory) == 1 && strlen(directory) == 0)
	{
		// Throw an error if we are trying to read the root directory because it is special
		printf("Currently trying to read the root directory \nABORT ABORT \n \n");
		exec("/usrbin/sh.exe", argv);
	}

	int fd = opendir(directory);

	printf("File Directory Number: %d \n \n", fd);

	if (fd == 0)
	{
		printf("Unable to obtain a file number \n ABORT ABORT \n \n");
		exec("/usrbin/sh.exe", argv);
	}

	printf("File number obtained, time to read some files bro \n \n");

	struct _DirectoryEntry * directoryEntry = 0;

	//int i = 6;
	bool reading = true;
	while(reading)
	{
		// Get a directory entry
		readdir(fd, directoryEntry);

		// Print the file name stored in the directory entry
		uint8_t filename[8] = { 0 };
		memmove(&filename, directoryEntry->Filename + 3, 7);
		printf("%s", (char*)&filename);

		uint8_t * extension;
		memmove(&extension, directoryEntry->Ext + 3, 3);
		printf("%s", (char*)&extension);
		printf("\n");

		// Check to see if we are at the end of our directory entries
		if(filename[0] == 0)
		{
			// Stop reading files
			reading = false;
		}	
	}	

	printf("it worked");
	printf("\n \n");

	// While we can read directory entries
		//Read directory entries
		// Check we haven't ran our of files to read

	// Close our directory once we have read all of the files to be read in the directory
	if(closedir(fd) == -1)
	{
		printf("There was an error closing the file\n");
		exec("/usrbin/sh.exe", argv);
	}
	printf("File closed\n");
	printf("Finished\n");
	
	// END OF PROGRAM
	exec("/usrbin/sh.exe", argv);
}