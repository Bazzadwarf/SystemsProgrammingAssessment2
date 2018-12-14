//
// File-system system calls.


#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

// Retrieve an argument to the system call that is an FD
//
// n   = The parameter number (0 = The first parameter, 1 = second parameter, etc).
// pfd = Returns the fd number (set to 0 if not required)
// pf  = Returns the pointer to the File structure (set to 0 if not required).

static int argfd(int n, int *pfd, File **pf)
{
	int fd;
	File *f;

	if (argint(n, &fd) < 0)
	{
		return -1;
	}
	if (fd < 0 || fd >= NOFILE || (f = myProcess()->OpenFile[fd]) == 0)
	{
		return -1;
	}
	if (pfd)
	{
		*pfd = fd;
	}
	if (pf)
	{
		*pf = f;
	}
	return 0;
}

// Allocate a file descriptor for the given file and
// store it in the process table for the current process.

static int fdalloc(File *f)
{
	int fd;
	Process *curproc = myProcess();

	for (fd = 0; fd < NOFILE; fd++) 
	{
		if (curproc->OpenFile[fd] == 0) 
		{
			curproc->OpenFile[fd] = f;
			return fd;
		}
	}
	return -1;
}

// Duplicate a file descriptor

int sys_dup(void)
{
	File *f;
	int fd;

	if (argfd(0, 0, &f) < 0)
	{
		return -1;
	}
	if ((fd = fdalloc(f)) < 0)
	{
		return -1;
	}
	fileDup(f);
	return fd;
}

// Read from file.

int sys_read(void)
{
	File *f;
	int n;
	char *p;

	if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
	{
		return -1;
	}
	return fileRead(f, p, n);
}

// Write to file.  Only implemented for pipes and console at present. 

int sys_write(void)
{
	File *f;
	int n;
	char *p;
	if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
	{
		return -1;
	}
	return fileWrite(f, p, n);
}

// Close file.

int sys_close(void)
{
	int fd;
	File *f;

	if (argfd(0, &fd, &f) < 0)
	{
		return -1;
	}
	myProcess()->OpenFile[fd] = 0;
	fileClose(f);
	return 0;
}

// Return file stats.  Not fully implemented.

int sys_fstat(void)
{
	File *f;
	Stat *st;

	if (argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
	{
		return -1;
	}
	return fileStat(f, st);
}

// Open a file. 

int sys_open(void)
{
	char *path; // Path of the file we wish to open.
	int fd;		// The file descriptor number
	int omode;	// The mode of the file
	File * f;	// A file structure to hold our file.

	// Get the parameter from the funciton
	if (argstr(0, &path) < 0 || argint(1, &omode) < 0)
	{
		return -1;	// Throw an error if we can't get the variables
	}
	
	// Get the current process
	Process *curproc = myProcess();

	// At the moment, only file reading is supported
	// Open the file
	f = fsFat12Open(curproc->Cwd, path, 0);

	// Check to see if we found the file specified in the path
	if (f == 0)
	{
		return -1;	// Throw an error if we can't find the file
	}

	// Allocate a File Descriptor number to our file
	fd = fdalloc(f);

	// Check to see if fdalloc executed successfully
	if (fd < 0)
	{
		// fdalloc failed
		fileClose(f);	// Close file
		return -1;		// Throw error
	}

	// Uses bitwise '&', checks the significan bits of the two numbers

	// Check to see if the file is Readable
	// Is readable if omode is not equal to O_WRONLY, otherwise it is not readable
	f->Readable = !(omode & O_WRONLY);

	// Check to see if the file is writeable
	// Is writable depends on the the bit shift of omode and O_WRONLY are true,
	// or if the bit shift of omode and O_RDWR are true
	f->Writable = (omode & O_WRONLY) || (omode & O_RDWR);

	// Return the File Descriptor number
	return fd;
}

// Execute a program

int sys_exec(void)
{
	char *path, *argv[MAXARG];
	int i;
	uint32_t uargv, uarg;
	char adjustedPath[200];
	
	if (argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0) 
	{
		return -1;
	}
	memset(argv, 0, sizeof(argv));
	for (i = 0;; i++) 
	{
		if (i >= NELEM(argv))
		{
			return -1;
		}
		if (fetchint(uargv + 4 * i, (int*)&uarg) < 0)
		{
			return -1;
		}
		if (uarg == 0) 
		{
			argv[i] = 0;
			break;
		}
		if (fetchstr(uarg, &argv[i]) < 0)
		{
			return -1;
		}
	}
	int pathLen = strlen(path);
	safestrcpy(adjustedPath, path, 200);
	if (path[pathLen - 4] != '.')
	{
		safestrcpy(adjustedPath + pathLen, ".exe", 200 - pathLen);
		adjustedPath[pathLen + 4] = 0;
	}
	return exec(adjustedPath, argv);
}

int sys_pipe(void)
{
	int *fd;
	File *rf, *wf;
	int fd0, fd1;

	if (argptr(0, (void*)&fd, 2 * sizeof(fd[0])) < 0)
	{
		return -1;
	}
	if (pipealloc(&rf, &wf) < 0)
	{
		return -1;
	}
	fd0 = -1;
	if ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) 
	{
		if (fd0 >= 0)
		{
			myProcess()->OpenFile[fd0] = 0;
		}
		fileClose(rf);
		fileClose(wf);
		return -1;
	}
	fd[0] = fd0;
	fd[1] = fd1;
	return 0;
}

int sys_chdir(void)
{
	// Variables for the system call
	char *directory;
	File *f;
	int dirlen, cwdlen;

	// Get the directory passed in to the system call
	if (argstr(0, &directory) < 0)
	{
		return -1;
	}

	// Get the directory length
	dirlen = strlen(directory);

	// Check to see if we have a directory passed in
	if (dirlen <= 0)
	{
		return -1;
	}

	// Get a pointer to the curent process
	Process *currProc = myProcess();

	char * returnToRoot;
	returnToRoot = "-r";

	if(directory == returnToRoot)
	{
		safestrcpy(currProc->Cwd, "/", 200);
		return 0;
	}

	// Validate the directory by attempting to open the directory
	f = fsFat12Open(currProc->Cwd, directory, 1);

	// If the directory doesn't exist, return -1
	if (f == 0)
	{
		return -1;
	}

	// Close the file we have successfully opened
	fsFat12Close(f);

	// Append a '/' to our directory
	if(directory[dirlen] != '/' && directory[dirlen] != '\\')
	{
		directory[dirlen] = '/';
	}

	// Get the current working directory length
	cwdlen = strlen(currProc->Cwd);

	// Set out current working directory
	safestrcpy(currProc->Cwd + cwdlen, directory, 200 - cwdlen);

	// Check to see if our directory has copied corerectly
	if (strlen(currProc->Cwd) <= 0)
	{
		return -1;
	}

	// Return 0, the system call has executed correctly
	return 0;
}

int sys_getcwd(void)
{
	// System call variables
	char *directory;
	int bufferSize;

	// Get the variables passed in to our system called
	if(argstr(0,&directory) < 0 || argint(1, &bufferSize) < 0)
	{
		return -1;
	}	
	
	// Get our current process and our processess current working directory
	Process *currProc = myProcess();
	safestrcpy(directory, currProc->Cwd, bufferSize);

	// Check our string has copied correctly
	if(strlen(directory) <= 0)
	{
		return -1;
	}
	return 0;
}

int sys_opendir(void)
{	
	char *path; // Path of the file we wish to open.
	int fd;		// The file descriptor number
	File * f;	// A file structure to hold our file.
	
	// Get the parameter from the funciton
	if (argstr(0, &path) < 0)
	{
		return 0;	// Throw an error if we can't get the variables
	}

	// Get the current process
	Process *curproc = myProcess();

	// At the moment, only file reading is supported
	// Open the file
	f = fsFat12Open(curproc->Cwd, path, 0);

	// If the directory doesn't exist, return -1
	if (f == 0)
	{
		return 0;
	}

	// Allocate a File Descriptor number to our file
	fd = fdalloc(f);

		// Check to see if fdalloc executed successfully
	if (fd < 0)
	{
		// fdalloc failed
		fileClose(f);	// Close file
		return -1;		// Throw error
	}

	//here is some code
	return fd;
}

int sys_readdir(void)
{
	return 0;
}

int sys_closedir(void)
{
	return 0;
}