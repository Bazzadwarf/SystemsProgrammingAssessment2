#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void main(int argc, char *argv[])
{
	// Get the directory passed in
	char * directory;
	directory = argv[0];
	
	bool extraInfo = false;
	if(strchr(directory, '-') != 0)
	{
		// Create our new directory
		char * newDirectory = "";

		// Get the length of the string we want to split
		int directoryLength = strlen(directory);

		// String split loop
		for(int i = 0; i < directoryLength; i++)
		{
			// Check to see if we have reached the first space in our directory stsring
			if(directory[i] == '-' && directory[i + 1] == 'l')
			{
				// Copy our new directory into our directory
				directory = "";
				directory = newDirectory;
				
				extraInfo = true;

				// Set loop to false
				i = directoryLength;
			}
			else
			{
				char c = directory[i];
				strcpy(newDirectory + i, &c);
			}
		}
	}

	// Get the current working directory
	char currentWorkingDirectory[200];
	if (getcwd(currentWorkingDirectory, 200) == -1)
	{
		// Throw an error if we are unable to obtain the current working directory
		printf("-1: Unable to obtain current working directory \n \n");
		exec("/usrbin/sh.exe", argv);
	}

	// Check to see if we are trying to read the root directory
	if (strlen(currentWorkingDirectory) == 1 && strlen(directory) == 0)
	{
		// Throw an error if we are trying to read the root directory because it is special
		printf("Currently trying to read the root directory\n \n");
		exec("/usrbin/sh.exe", argv);
	}

	int fd = opendir(directory);
	if (fd == 0)
	{
		printf("0: Unable to obtain a file\n \n");
		exec("/usrbin/sh.exe", argv);
	}

	struct _DirectoryEntry * directoryEntry = 0;

	bool reading = true;
	while(reading)
	{
		// Get a directory entry
		readdir(fd, directoryEntry);

		// Get the file name
		uint8_t filename[8] = { 0 };
		memmove(&filename, directoryEntry->Filename + 3, 7);

		// Check to see if we are at the end of our directory entries
		if(filename[0] == 0)
		{
			// Stop reading files
			reading = false;
			printf("\n");
			break;
		}	

		// Print the file name stored in the directory entry
		printf("\n%s", (char*)&filename);

		// Print the file extension
		uint8_t * extension;
		memmove(&extension, directoryEntry->Ext + 3, 3);
		printf("%s    ", (char*)&extension);

		if(extraInfo)
		{
//-----------------------------------------------------------------------
			// // Print creation date and time
			// Get the full date
			uint16_t Date;
			memmove(&Date, &directoryEntry->DateCreated + 3, sizeof(uint16_t));

			// Get the day
			uint16_t Day;
			memmove(&Day, &Date, sizeof(uint16_t));
			Day = Day & 0x1F;

			// Get the month
			uint16_t Month;
			memmove(&Month, &Date, sizeof(uint16_t));
			Month = Month & 0x1E0;
			Month = Month >> 5;

			// Get the year
			uint16_t Year;
			memmove(&Year, &Date, sizeof(uint16_t));
			Year = Year & 0xFE00;
			Year = Year >> 9;
			Year += 1980;

			// Print the date
			printf("\nCreated: %d-%d-%d ", Day, Month, Year);

			// Get the time
			uint16_t Time;
			memmove(&Time, &directoryEntry->TimeCreated + 3, sizeof(uint16_t));

			// Get the day
			uint16_t Seconds;
			memmove(&Seconds, &Time, sizeof(uint16_t));
			Seconds = Seconds & 0x1F;
			Seconds *= 2;

			// Get the minuets
			uint16_t Mins;
			memmove(&Mins, &Time, sizeof(uint16_t));
			Mins = Mins & 0x7E0;
			Mins = Mins >> 5;
			
			// Get the Hours
			uint16_t Hours;
			memmove(&Hours, &Time, sizeof(uint16_t));
			Hours = Hours & 0xF800;
			Hours = Hours >> 11;
			
			// Print the time
			printf("%d:%d:%d", Hours, Mins, Seconds);

//-----------------------------------------------------------------------

			// // Print last modified date and time
			
			// Get the full date
			//uint16_t Date;
			memmove(&Date, &directoryEntry->LastModDate + 3, sizeof(uint16_t));

			// Get the day
			//uint16_t Day;
			memmove(&Day, &Date, sizeof(uint16_t));
			Day = Day & 0x1F;

			// Get the month
			//uint16_t Month;
			memmove(&Month, &Date, sizeof(uint16_t));
			Month = Month & 0x1E0;
			Month = Month >> 5;

			// Get the year
			//uint16_t Year;
			memmove(&Year, &Date, sizeof(uint16_t));
			Year = Year & 0xFE00;
			Year = Year >> 9;
			Year += 1980;

			// Print the date
			printf("\nModified: %d-%d-%d", Day, Month, Year);

			// Get the time
			//uint16_t Time;
			memmove(&Time, &directoryEntry->LastModTime + 3, sizeof(uint16_t));

			// Get the day
			//uint16_t Seconds;
			memmove(&Seconds, &Time, sizeof(uint16_t));
			Seconds = Seconds & 0x1F;
			Seconds *= 2;

			// Get the minuets
			//uint16_t Mins;
			memmove(&Mins, &Time, sizeof(uint16_t));
			Mins = Mins & 0x7E0;
			Mins = Mins >> 5;
			
			// Get the Hours
			//uint16_t Hours;
			memmove(&Hours, &Time, sizeof(uint16_t));
			Hours = Hours & 0xF800;
			Hours = Hours >> 11;
			
			// Print the time
			printf(" %d:%d:%d", Hours, Mins, Seconds);

//-----------------------------------------------------------------------

			// Print size of the file
			int FileSize;
			memmove(&FileSize, &directoryEntry->FileSize, sizeof(uint16_t));
			printf("\nFile Size: %d", FileSize);
			
//-----------------------------------------------------------------------

			// Print the attributes of the file
			
			uint8_t attributes;
			memmove(&attributes, &directoryEntry->Attrib + 3, sizeof(uint8_t));

			//-----------------------------------------------------------------------

			// Print Read Only
			uint8_t attribute;
			memmove(&attribute, &attributes, sizeof(uint8_t));
			attribute = attribute & 0x01;

			printf("\nRead only: ");
			if(attribute == 1)
			{
				printf("True");
			}
			else
			{
				printf("False");
			}

			//-----------------------------------------------------------------------

			// Print Hidden
			//uint8_t hidden;
			memmove(&attribute, &attributes, sizeof(uint8_t));
			attribute = attribute & 0x02;
			attribute = attribute >> 1;

			printf("\nHidden: ");
			if(attribute == 1)
			{
				printf("True");
			}
			else
			{
				printf("False");
			}

			//-----------------------------------------------------------------------

			// Print System
			//uint8_t sysfile;
			memmove(&attribute, &attributes, sizeof(uint8_t));
			attribute = attribute & 0x04;
			attribute = attribute >> 2;

			printf("\nSystem File: ");
			if(attribute == 1)
			{
				printf("True");
			}
			else
			{
				printf("False");
			}

			//-----------------------------------------------------------------------

			// Print Volume Label
			//uint8_t volumelabel;
			memmove(&attribute, &attributes, sizeof(uint8_t));
			attribute = attribute & 0x08;
			attribute = attribute >> 3;

			printf("\nVolume Label: ");
			if(attribute == 1)
			{
				printf("Exists");
			}
			else
			{
				printf("Empty");
			}

			//-----------------------------------------------------------------------
			
			// Print subdirectory
			//uint8_t subdirectory;
			memmove(&attribute, &attributes, sizeof(uint8_t));
			attribute = attribute & 0x10;
			attribute = attribute >> 4;

			printf("\nType of file: ");
			if(attribute == 1)
			{
				printf("Subdirectory");
			}
			else
			{
				printf("File");
			}

			//-----------------------------------------------------------------------
			
			// Print Archived
			//uint8_t archived;
			memmove(&attribute, &attributes, sizeof(uint8_t));
			attribute = attribute & 0x00;
			attribute = attribute >> 5;

			printf("\nArchived: ");
			if(attribute == 1)
			{
				printf("True");
			}
			else
			{
				printf("False");
			}

			//-----------------------------------------------------------------------

			printf("\n");
		}
	}	

	// Close our directory once we have read all of the files to be read in the directory
	if(closedir(fd) == -1)
	{
		printf("There was an error closing the file\n");
		exec("/usrbin/sh.exe", argv);
	}

	// END OF PROGRAM
	exec("/usrbin/sh.exe", argv);
}