/*	$NetBSD: hello.c,v 1.1 2015/05/13 07:07:36 pgoyette Exp $	*/

/*-
 * Copyright (c) 2015 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: hello.c,v 1.1 2015/05/13 07:07:36 pgoyette Exp $");

#include <sys/param.h>
#include <sys/module.h>
#include <sys/namei.h>
#include <sys/mount.h>
#include <sys/vfs_syscalls.h>
#include <sys/uio.h>
#include <fs/tmpfs/tmpfs_args.h>

static int
mount_get_vfsops(const char *fstype, enum uio_seg type_seg,
    struct vfsops **vfsops)
{
	char fstypename[sizeof(((struct statvfs *)NULL)->f_fstypename)];
	int error;

	if (type_seg == UIO_USERSPACE) {
		/* Copy file-system type from userspace.  */
		error = copyinstr(fstype, fstypename, sizeof(fstypename), NULL);
	} else {
		error = copystr(fstype, fstypename, sizeof(fstypename), NULL);
		KASSERT(error == 0);
	}

	if (error) {
		/*
		 * Historically, filesystem types were identified by numbers.
		 * If we get an integer for the filesystem type instead of a
		 * string, we check to see if it matches one of the historic
		 * filesystem types.
		 */
		u_long fsindex = (u_long)fstype;
		if (fsindex >= nmountcompatnames ||
		    mountcompatnames[fsindex] == NULL)
			return ENODEV;
		strlcpy(fstypename, mountcompatnames[fsindex],
		    sizeof(fstypename));
	}

	/* Accept `ufs' as an alias for `ffs', for compatibility. */
	if (strcmp(fstypename, "ufs") == 0)
		fstypename[0] = 'f';

	if ((*vfsops = vfs_getopsbyname(fstypename)) != NULL)
		return 0;

	/* If we can autoload a vfs module, try again */
	(void)module_autoload(fstypename, MODULE_CLASS_VFS);

	if ((*vfsops = vfs_getopsbyname(fstypename)) != NULL)
		return 0;

	return ENODEV;
}

static void 
mount(const char *fstype, const char *path, int mntflags)
{
        int error;
        struct vnode *vp;
	struct vfsops *vfsops = NULL;
	struct tmpfs_args tmpfs_args;
	size_t data_len = sizeof(tmpfs_args);

	error = namei_simple_kernel(path, NSM_FOLLOW_TRYEMULROOT, &vp);
	if (error != 0){
		vp = NULL;
		printf("%d", error);
	}
	error = mount_get_vfsops(fstype, 1, &vfsops);
	if (error != 0)
		printf("%d", error);
	
	printf("CURLWP: %p\n", curlwp);
	error = mount_domount(curlwp, &vp, vfsops, path, mntflags, &tmpfs_args, &data_len);
	printf("%d", error);
}

/*
 * Last parameter of MODULE macro is a list of names (as string; names are
 * separated by commas) of dependencies.  If module has no dependencies,
 * then NULL should be passed.
 */

MODULE(MODULE_CLASS_MISC, mount, NULL);

static int
mount_modcmd(modcmd_t cmd, void *arg __unused)
{
	switch (cmd) {
	case MODULE_CMD_INIT:
		mount(MOUNT_TMPFS, "/tmp\0", 0);
		break;

	case MODULE_CMD_FINI:
		break;

	case MODULE_CMD_STAT:
		printf("Example module status queried.\n");
		break;

	default:
		return ENOTTY;
	}

	return 0;
}
