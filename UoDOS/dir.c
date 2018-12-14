#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	printf("List Directory program started \n");
	
	char * directory;
	directory = argv[0];
	
	// if(strlen(directory) <= 0)
	// {
	// 	printf("No directory supplied, will search in the current working \n");
	// 	exit();
	// }

	printf(directory);
    printf("\n");
    
	// Try and open our directory
    int result = opendir(directory);
	if(result == 0)
	{
		printf("Problem finding a directory entry");
		exit();
	}

	printf("%d\n", result);

	// struct _DirectoryEntry * directoryEntry;
	// if(readdir(result, directoryEntry) == -1)
	// {
	// 	printf("It fucked up\n");
	// 	exit();
	// }
	// else
	// {
	// 	printf("It worked");
	// }

	// if(closedir() == -1)
	// {
	// 	return;
	// }
	//chdir(directory);
	printf("List Directory program finished \n");
    
	exit();
}