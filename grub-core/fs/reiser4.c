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

/*
  TODO:
  implement support of reiser4
*/
#include <grub/err.h>
#include <grub/file.h>
#include <grub/mm.h>
#include <grub/misc.h>
#include <grub/disk.h>
#include <grub/dl.h>
#include <grub/types.h>
#include <grub/fshelp.h>
#include <grub/reiser4.h>

GRUB_MOD_LICENSE ("GPLv3+");
static grub_dl_t my_mod;


static struct grub_reiser4_data* grub_reiser4_mount(grub_disk_t disk){
  struct grub_reiser4_data *data = 0;
  
  data = grub_malloc (sizeof (*data));
  if (!data) goto fail;
  
  grub_disk_read (disk, REISER4_MASTER_OFFSET / GRUB_DISK_SECTOR_SIZE, 0, 
		  sizeof(struct reiser4_master_sb_t), data->sb);
  if (grub_errno)
    goto fail;
  
  if (grub_memcmp(data->sb.magic,REISER4_MAGIC_STRING, sizeof(REISER4_MAGIC_STRING)-1)){
    grub_error (GRUB_ERR_BAD_FS, "not a Reiser4 filesystem");
    goto fail;
  }
  data->disk = disk;
  return data;
  
fail:
  grub_error (GRUB_ERR_BAD_FS,"Reiser4 mount error: %d",grub_errno);
  if (!!data) grub_free(data);
  return NULL;
}

/* Return the label of the device DEVICE in LABEL.  The label is
   returned in a grub_malloc'ed buffer and should be freed by the
   caller.  */
static grub_err_t
grub_reiser4_label (grub_device_t device, char **label)
{
  struct grub_reiser4_data *data;
  grub_disk_t disk = device->disk;  
  *label = NULL;
  
  grub_dl_ref (my_mod);
  data = grub_reiser4_mount (disk);
  if (data)
      *label = grub_strndup(data->sb.label, REISER4_MAX_LABEL_LENGTH);

  grub_dl_unref (my_mod);
  grub_free (data);
  return grub_errno;
}

static grub_err_t 
grub_reiser4_close (grub_file_t file)
{
  struct grub_fshelp_node *node = file->data;
  struct grub_reiser4_data *data = node->data;
  grub_free (data);
  grub_free (node);
  grub_dl_unref (my_mod);
  return GRUB_ERR_NONE;
}

static grub_err_t grub_reiser4_uuid (grub_device_t device, char **uuid)
{
  struct grub_reiser4_data *data;
  int i;
  grub_disk_t disk = device->disk;
  grub_dl_ref (my_mod);
  data = grub_reiser4_mount (disk);
  *uuid = NULL;
  
  for (i = 0; i < ARRAY_SIZE (data->sb.uuid); i++)
      if (data->sb.uuid[i])
	  break;

  if (i < ARRAY_SIZE (data->sb.uuid))
      *uuid = grub_xasprintf ("%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
			     grub_be_to_cpu16 (data->sb.uuid[0]),
			     grub_be_to_cpu16 (data->sb.uuid[1]),
			     grub_be_to_cpu16 (data->sb.uuid[2]),
			     grub_be_to_cpu16 (data->sb.uuid[3]),
			     grub_be_to_cpu16 (data->sb.uuid[4]),
			     grub_be_to_cpu16 (data->sb.uuid[5]),
			     grub_be_to_cpu16 (data->sb.uuid[6]),
			     grub_be_to_cpu16 (data->sb.uuid[7]));
  grub_dl_unref (my_mod);
  grub_free (data);
  return grub_errno;
}


static struct grub_fs grub_reiser4_fs =
  {
    .name = "reiser4",
    .dir = grub_reiser4_dir,
    .open = grub_reiser4_open,
    .read = grub_reiser4_read,
    .close = grub_reiser4_close, //done
    .label = grub_reiser4_label, //done
    .uuid = grub_reiser4_uuid,//Done
    .next = 0
  };

GRUB_MOD_INIT(reiser4)
{
  grub_fs_register (&grub_reiser4_fs);
  my_mod = mod;
}

GRUB_MOD_FINI(reiser4)
{
  grub_fs_unregister (&grub_reiser4_fs);
}
