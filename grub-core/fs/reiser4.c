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


/* Return the label of the device DEVICE in LABEL.  The label is
   returned in a grub_malloc'ed buffer and should be freed by the
   caller.  */
static grub_err_t
grub_reiser4_label (grub_device_t device, char **label)
{
  *label = grub_malloc (REISERFS_MAX_LABEL_LENGTH);
  if (*label)
    {
      grub_disk_read (device->disk,
                      REISER4_MASTER_OFFSET / GRUB_DISK_SECTOR_SIZE,
                      offsetof(), REISERFS_MAX_LABEL_LENGTH,
                      *label);
    }
  return grub_errno;
}


static grub_err_t grub_reiser4_uuid (grub_device_t device, char **uuid)
{
  struct grub_reiser4_data *data;
  grub_disk_t disk = device->disk;
  grub_dl_ref (my_mod);
  data = grub_reiserfs_mount (disk);
  if (data)
      *uuid = grub_xasprintf ("%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
			     grub_be_to_cpu16 (data->sb.uuid[0]),
			     grub_be_to_cpu16 (data->sb.uuid[1]),
			     grub_be_to_cpu16 (data->sb.uuid[2]),
			     grub_be_to_cpu16 (data->sb.uuid[3]),
			     grub_be_to_cpu16 (data->sb.uuid[4]),
			     grub_be_to_cpu16 (data->sb.uuid[5]),
			     grub_be_to_cpu16 (data->sb.uuid[6]),
			     grub_be_to_cpu16 (data->sb.uuid[7]));
  else
    *uuid = NULL;
  grub_dl_unref (my_mod);
  grub_free (data);
  return grub_errno;
}




GRUB_MOD_INIT(reiser4)
{
  grub_fs_register (&grub_reiser4_fs);
  my_mod = mod;
}

GRUB_MOD_FINI(reiser4)
{
  grub_fs_unregister (&grub_reiser4_fs);
}
