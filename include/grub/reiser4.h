/* reiser4.c - ReiserFS versions up to 4.? */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2003,2004,2005,2008,2010  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grub/err.h>
#include <grub/file.h>
#include <grub/mm.h>
#include <grub/misc.h>
#include <grub/disk.h>
#include <grub/dl.h>
#include <grub/types.h>
#include <grub/fshelp.h>


#define REISER4_MAX_LABEL_LENGTH 16
#define REISER4_LABEL_OFFSET
#define REISER4_MASTER_OFFSET 65536
#define REISER4_MAGIC_STRING "ReIsEr4"
/* Master super block structure. It is the same for all reiser4 filesystems,
   so, we can declare it here. It contains common for all format fields like
   block size etc. */
struct reiser4_master_sb {
	/* Master super block magic. */
	char magic[16];

	/* Disk format in use. */
	grub_uint16_t format;

	/* Filesyetem block size in use. */
	grub_uint16_t blksize;

	/* Filesyetm uuid in use. */
	char uuid[16];

	/* Filesystem label in use. */
	char label[REISER4_MAX_LABEL_LENGTH];
	
	/*location of diskmap*/
	grub_uint64_t diskmap;
}GRUB_PACKED; 

/* Returned when opening a file.  */
struct grub_reiser4_data
{		
  /*superblock*/
  struct reiser4_master_sb sb;
  /*grub disk device*/
  grub_disk_t disk;
};

struct grub_fshelp_node
{
  struct grub_reiser4_data *data;
  //TODO: Add necessary fields
};

static grub_err_t grub_reiser4_open (struct grub_file *file, const char *name);
static grub_err_t grub_reiser4_close (grub_file_t file);
static grub_err_t grub_reiser4_dir (grub_device_t device, const char *path,
                   int (*hook) (const char *filename, const struct grub_dirhook_info *info));
static grub_ssize_t grub_reiser4_read (grub_file_t file, char *buf, grub_size_t len);
static grub_err_t grub_reiser4_uuid (grub_device_t device, char **uuid);
static grub_err_t grub_reiser4_label (grub_device_t device, char **label);



