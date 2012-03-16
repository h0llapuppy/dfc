/*
 * dfc.c
 *
 * (C) 2012 - Hahling Robin <robin.hahling@gw-computing.net>
 *
 * Displays free disk space in an elegant manner.
 */
#define _POSIX_C_SOURCE 2

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <mntent.h>

#include <sys/param.h>
#include <sys/statvfs.h>

#include "dfc.h"

/* set flags for options */
int hflag, Hflag, vflag;

int
main(int argc, char *argv[])
{
	FILE *mtab;
	struct mntent *entbuf;
	struct statvfs vfsbuf;
	struct fsmntinfo *fmi;
	struct list queue;
	int ch;

	while ((ch = getopt(argc, argv, "hHvV:")) != -1) {
		switch (ch) {
		case 'h':
			hflag = 1;
			break;
		case 'H':
			Hflag = 1;
			break;
		case 'v':
			vflag = 1;
			break;
		case '?':
		default:
			usage(EXIT_FAILURE);
			/* NOTREACHED */
		}
	}

	/* initializes the queue */
	init_queue(&queue);

	/* init fsmntinfo */
	if ((fmi = malloc(sizeof(struct fsmntinfo))) == NULL) {
		(void)fputs("Error while allocating memory to fmi", stderr);
		return EXIT_FAILURE;
		/* NOTREACHED */
	}
	*fmi = fmi_init();

	/* open mtab file */
	if ((mtab = fopen("/etc/mtab", "r")) == NULL) {
		perror("Error while opening mtab file ");
		return EXIT_FAILURE;
		/* NOTREACHED */
	}

	/* loop to get infos from all the mounted fs */
	while ((entbuf = getmntent(mtab))) {
		/* get infos from statvfs */
		if (statvfs(entbuf->mnt_dir, &vfsbuf) == -1) {
			(void)fprintf(stderr, "Error while trying statvfs on %s\n",
					entbuf->mnt_dir);
			perror("with this error code ");
			return EXIT_FAILURE;
			/* NOTREACHED */
		} else {
			/* infos from getmntent */
			fmi->dir = entbuf->mnt_dir;
			fmi->fsname = entbuf->mnt_fsname;
			fmi->type = entbuf->mnt_type;

			/* infos from statvfs */
			fmi->bsize = vfsbuf.f_bsize;
			fmi->blocks = vfsbuf.f_blocks;
			fmi->bfree = vfsbuf.f_bfree;

			/* pointer to the next element */
			fmi->next = NULL;

			/* enqueue the element into the queue */
			enqueue(&queue, *fmi);
		}
	}
	/* TODO remove when testing is done */
	disp(queue);

	/* we need to close the mtab file now */
	if (fclose(mtab) == EOF)
		perror("Could not close mtab file ");

	return EXIT_SUCCESS;
	/* NOTREACHED */
}

/*
 * Display usage.
 * param: status --> status code (EXIT_SUCCESS, EXIT_FAILURE, ...)
 */
static void
usage(int status)
{
	if (status != 0)
		(void)fputs("Try dfc -h for more informations\n", stderr);
	else
		(void)fputs("Usage: dfc [OPTIONS(S)]\n"
		"Available options:\n"
		"	-h	print size in human readable format\n"
		"	-H	print size in human readable format but using powers of 1000, not 1024\n"
		"	-v	print program version\n",
		stdout);

	exit(status);
	/* NOTREACHED */
}

/*
 * Initializes a queue structure
 * @lst: queue pointer
 */
void
init_queue(struct list *lst)
{
	lst->head = NULL;
	lst->tail = NULL;
}

/*
 * Checks if a queue is empty
 * @lst: queue
 * Returns:
 *	--> 1 (true) if the queue is empty
 *	--> 0 if not
 */
int
is_empty(struct list lst)
{
	if (lst.head == NULL)
		return 1;
		/* NOTREACHED */
	else
		return 0;
		/* NOTREACHED */
}

/*
 * Enqueues an element into a queue
 * @lst: queue pointer
 * @elt: element
 * Returns:
 *	--> -1 on error
 *	-->  0 on sucess
 */
int
enqueue(struct list *lst, struct fsmntinfo fmi)
{
	struct fsmntinfo *new_fmi = malloc(sizeof(struct fsmntinfo));

	if (new_fmi == NULL) {
		(void)fputs("Error while allocating memory to fmi", stderr);
		return -1;
		/* NOTREACHED */
	}

	/* initialize the new element to be inserted in the queue */
	*new_fmi = fmi;

	if (is_empty((*lst)))
		lst->head = new_fmi;
	else
		lst->tail->next = new_fmi;

	lst->tail = new_fmi;

	return 0;
	/* NOTREACHED */
}

/* Dequeues the first element of a queue
 * @lst: queue pointer
 * Returns:
 *	--> the element dequeued
 */
struct fsmntinfo
dequeue(struct list *lst)
{
	struct fsmntinfo *p = NULL;
	struct fsmntinfo fmi;

	if (is_empty((*lst))) {
		fmi = fmi_init();
		return fmi;
		/* NOTREACHED */
	}

	/* in case there is only one element in the queue */
	if (lst->head == lst->tail) {
		lst->head->next = NULL;
		lst->tail = NULL;
	}

	p = lst->head;
	lst->head = p->next;
	fmi = (*p);

	return fmi;
	/* NOTREACHED */
}

/*
 * Inits an fsmntinfo to some defaults values
 * Returns:
 *	--> fsmntinfo that has been initialized
 */
struct fsmntinfo
fmi_init(void)
{
	struct fsmntinfo fmi;

	fmi.dir		= "unknown";
	fmi.fsname	= "unknown";
	fmi.type	= "unknown";
	fmi.bsize	= 0;
	fmi.blocks	= 0;
	fmi.bfree	= 0;
	fmi.next	= NULL;

	return fmi;
	/* NOTREACHED */
}

/* TODO remove this when testing is done */
void
disp(struct list lst)
{
	struct fsmntinfo *p = NULL;

	p = lst.head;

	while (p != NULL) {
		(void)printf("dir: %s\nfsname: %s\ntype: %s\nbsize: %ld\n"
				"blocks: %ld\nbfree: %ld\n",
			p->dir,
			p->fsname,
			p->type,
			p->bsize,
			p->blocks,
			p->bfree);
		(void)puts("--------------------------------------------");
		p = p->next;
	}
}