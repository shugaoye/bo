//*****************************************************************************
//
// syscalls_cs3.c - implementation of system call stubs for Newlib.
//
// Copyright (c) 2012 Roger Ye.  All rights reserved.
// Software License Agreement
//  
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHOR SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#include <sys/stat.h>
#include <sys/unistd.h>
#include <serial_goldfish.h>

#include <errno.h>
#undef errno
extern int errno;

/*
 environ
 A pointer to a list of environment variables and their values. 
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

/*
 * init
 * Initialize serial data structure.
 */
void _init(void) {
    struct serial_device *drv;

    drv = default_serial_console();
    drv->start();

    return;
}

/*
 * write
 * Write a character to a file. `libc' subroutines will use this system routine
 * for output to all files, including stdout.
 * Returns -1 on error or number of bytes sent
 */
int _write(int file, char *ptr, int len) {
    int n;
    struct serial_device *drv;

    drv = default_serial_console();

    switch (file) {
    case STDOUT_FILENO: /*stdout*/
        for (n = 0; n < len; n++) {
        	drv->putc(*ptr++);
        }
        break;
    case STDERR_FILENO: /* stderr */
        for (n = 0; n < len; n++) {
        	drv->putc(*ptr++);
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}

/*
 * read
 * Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 * Returns -1 on error or blocks until the number of characters have been read.
 */
int _read(int file, char *ptr, int len) {
    int n, len1;
    int num = -1;
    char c;
    struct serial_device *drv;

    drv = default_serial_console();
    len1 = drv->tstc();
    /* We implement a blocking read here. */
    while (len1 <= 0) {
    	len1 = drv->tstc();
    }

    if(len1) {
		if(len1 > len) {
			/* len is the buffer size. We cannot read more than buffer size.*/
			len1 = len;
		}

		num = 0;

		switch (file) {
		case STDIN_FILENO:
			for (n = 0; n < len1; n++) {
				c = drv->getc();
				*ptr++ = c;
				num++;
			}
			break;
		default:
			errno = EBADF;
			return -1;
		}
    }
    return num;
}

/*
 * sbrk
 * Increase program data space.
 * Malloc and related functions depend on this.
 */
#define STACK_BUFFER_SIZE 65536 /* Reserved stack space in bytes. */
static char *heap_end = 0;

void * _sbrk(int incr) {

    extern char __cs3_heap_start; // Defined by the linker
    extern char __cs3_heap_end; /* Defined by the linker */
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &__cs3_heap_start;
    }
    prev_heap_end = heap_end;

	if (heap_end + STACK_BUFFER_SIZE + incr > &__cs3_heap_end) {
		/* Heap and stack collision */
		errno = ENOMEM;
		_write(STDERR_FILENO, "Error in _sbrk!\n", 16);
		return (void *)0;
	}
    heap_end += incr;

    return (void *) prev_heap_end;
}

/*
 * Exit a program without cleanup.
 *
 */
void _exit(int status) {
    _write(1, "exit", 4);
    while (1) {
        ;
    }
}

/*
 * open
 * Open a file. A minimal implementation without file system.
 * */
int _open (const char *name, int flags, int mode)
{
	errno = ENOSYS;
	return -1; /* Always fails */
}

int _close(int file) {
    return -1;
}

/*
 * execve
 * Transfer control to a new process. Minimal implementation (for a system without processes):
 */
int _execve(char *name, char **argv, char **env) {
    errno = ENOMEM;
    return -1;
}

/*
 * fork
 * Create a new process. Minimal implementation (for a system without processes):
 */

int _fork(void) {
    errno = EAGAIN;
    return -1;
}

/*
 * fstat
 * Status of an open file. For consistency with other minimal implementations in these examples,
 * all files are regarded as character special devices.
 * The `sys/stat.h' header file required is distributed in the `include' sub directory for this C library.
 */
int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 * getpid
 * Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes.
 * Minimal implementation, for a system without processes:
 */
int _getpid(void) {
    return 1;
}

/*
 * isatty
 * Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int _isatty(int file) {
    switch (file){
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

/*
 * kill
 * Send a signal. Minimal implementation:
 */
int _kill(int pid, int sig) {
    errno = EINVAL;
    return (-1);
}

/*
 * link
 * Establish a new name for an existing file. Minimal implementation:
 */

int _link(char *old, char *new) {
    errno = EMLINK;
    return -1;
}

/*
 * lseek
 * Set position in a file. Minimal implementation:
 */
int _lseek(int file, int ptr, int dir) {
    return 0;
}

/*
 * stat
 * Status of a file (by name). Minimal implementation:
 */
int _stat(const char *filepath, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 * unlink
 * Remove a file's directory entry. Minimal implementation:
 */
int _unlink(char *name) {
    errno = ENOENT;
    return -1;
}

/*
 * wait
 * Wait for a child process. Minimal implementation:
 */
int _wait(int *status) {
    errno = ECHILD;
    return -1;
}

