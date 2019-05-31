/*
 *  apfsprogs/mkapfs/mkapfs.h
 *
 * Copyright (C) 2019 Ernesto A. Fernández <ernesto.mnd.fernandez@gmail.com>
 */

#ifndef _MKAPFS_H
#define _MKAPFS_H

#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <apfs/raw.h>

/* Filesystem parameters */
struct parameters {
	unsigned long	blocksize;	/* Block size */
	u64		block_count;	/* Number of blocks in the container */
	char		*label;		/* Volume label */
	char		*main_uuid;	/* Container UUID in standard format */
	char		*vol_uuid;	/* Volume UUID in standard format */
	bool		case_sensitive;	/* Is the filesystem case-sensitive? */
};

/* String to identify the program and its version */
#define MKFS_ID_STRING	"mkapfs for linux, version 0.1"

/* Hardcoded transaction ids */
#define MKFS_XID	1

/* Hardcoded object ids */
#define	SPACEMAN_OID	APFS_OID_RESERVED_COUNT
#define REAPER_OID	(SPACEMAN_OID + 1)
#define FIRST_VOL_OID	(REAPER_OID + 1)

/*
 * Constants describing the checkpoint areas; these are hardcoded for now, but
 * should actually change with the container size.
 */
#define CPOINT_DESC_BASE	(APFS_NX_BLOCK_NUM + 1)
#define CPOINT_DESC_BLOCKS	64
#define CPOINT_DATA_BASE	(CPOINT_DESC_BASE + CPOINT_DESC_BLOCKS)
#define CPOINT_DATA_BLOCKS	5904

/* Hardcoded block numbers */
#define CPOINT_MAP_BNO		CPOINT_DESC_BASE
#define CPOINT_SB_BNO		(CPOINT_DESC_BASE + 1)
#define REAPER_BNO		CPOINT_DATA_BASE
#define MAIN_OMAP_BNO		20000
#define MAIN_OMAP_ROOT_BNO	20001
#define FIRST_VOL_BNO		20002

/* Declarations for global variables */
extern struct parameters *param;	/* Filesystem parameters */
extern int fd;				/* File descriptor for the device */

extern __attribute__((noreturn)) void system_error(void);

/**
 * get_zeroed_block - Map and zero a filesystem block
 * @bno: block number
 *
 * Returns a pointer to the mapped block; the caller must unmap it after use.
 */
static inline void *get_zeroed_block(u64 bno)
{
	void *block;

	block = mmap(NULL, param->blocksize, PROT_READ | PROT_WRITE,
		     MAP_SHARED, fd, bno * param->blocksize);
	if (block == MAP_FAILED)
		system_error();
	memset(block, 0, param->blocksize);
	return block;
}

/**
 * get_timestamp - Get the current time in nanoseconds
 *
 * Calls clock_gettime(), so may not work with old versions of glibc.
 */
static inline u64 get_timestamp(void)
{
	struct timespec time;

	if (clock_gettime(CLOCK_REALTIME, &time))
		system_error();

	return (u64)time.tv_sec * NSEC_PER_SEC + time.tv_nsec;
}

#endif	/* _MKAPFS_H */