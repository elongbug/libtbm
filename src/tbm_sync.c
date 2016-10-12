/**************************************************************************

libtbm

Copyright 2012 - 2016 Samsung Electronics co., Ltd. All Rights Reserved.

Contact: SooChan Lim <sc1.lim@samsung.com>,
		 Changyeon Lee <cyeon.lee@samsung.com>,
		 Boram Park <boram1288.park@samsung.com>,
		 Sangjin Lee <lsj119@samsung.com>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sub license, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice (including the
next paragraph) shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************************************************************/

#include "config.h"

#include "tbm_bufmgr_int.h"
#include "tbm_sync.h"

#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>

/* IOCTLs for timeline file object. */
#define TIMELINE_IOC_MAGIC			'W'
#define TIMELINE_IOC_CREATE_FENCE	_IOWR(TIMELINE_IOC_MAGIC, 0, struct create_fence_data)
#define TIMELINE_IOC_INC			_IOW(TIMELINE_IOC_MAGIC, 1, __u32)

/* IOCTLs for fence file object. */
#define FENCE_IOC_MAGIC		'>'
#define FENCE_IOC_WAIT		_IOW(FENCE_IOC_MAGIC, 0, __s32)
#define FENCE_IOC_MERGE		_IOWR(FENCE_IOC_MAGIC, 1, struct sync_merge_data)

/* Path to the sync device file. */
#define SYNC_DEVICE_PATH	"/dev/sw_sync"

/* Argument data structure for the timeline.create_fence ioctl. */
struct create_fence_data {
	__u32	value;		/* Pt value on the timeline for the fence (IN) */
	char	name[32];	/* Name of the fence object (IN) */
	__s32	fence;		/* File descriptor for the created fence (OUT) */
};

/* Argument data structure for the fence.merge ioctl. */
struct sync_merge_data {
	__s32	fd2;		/* fence to merged with the fence (IN) */
	char	name[32];	/* Name of the new fence object (IN) */
	__s32	fence;		/* File descriptor for the new fence (OUT) */
};

#define ERRNO_BUF_SIZE	256

static inline void
_log_errno()
{
	/* calling strerror_r() might overwrite errno, so save it. */
	int		errnum = errno;
	char	buf[ERRNO_BUF_SIZE];

	if (strerror_r(errnum, buf, ERRNO_BUF_SIZE) == 0) {
		TBM_LOG_E("errno : %d(%s)\n", errnum, buf);
		return;
	} else {
		TBM_LOG_E("errno : %d()\n", errnum);
		return;
	}
}

static inline void
_copy_string(char *dst, const char *src, size_t size)
{
	if (size == 0)
		return;

	if (src == NULL || size == 1) {
		*dst = '\0';
		return;
	}

	while (size-- != 1) {
		if ((*dst++ = *src++) == '\0')
			return;
	}

	*dst = '\0';
}

tbm_fd
tbm_sync_timeline_create(void)
{
	tbm_fd timeline = open(SYNC_DEVICE_PATH, O_RDWR | O_CLOEXEC);

	if (timeline == -1)
		_log_errno();

	return timeline;
}

int
tbm_sync_timeline_inc(tbm_fd timeline, unsigned int count)
{
	__u32 arg = count;

	if (ioctl(timeline, TIMELINE_IOC_INC, &arg) == -1) {
		_log_errno();
		return 0;
	}

	return 1;
}

tbm_fd
tbm_sync_fence_create(tbm_fd timeline, const char *name, unsigned int value)
{
	struct create_fence_data data = { .value = value };

	_copy_string(data.name, name, 32);

	if (ioctl(timeline, TIMELINE_IOC_CREATE_FENCE, &data) == -1) {
		_log_errno();
		return -1;
	}

	return data.fence;
}

int
tbm_sync_fence_wait(tbm_fd fence, int timeout)
{
	__s32	arg = timeout;
	int		ret;

	/* Retry while ioctl() ended up with interrupt. */
	do {
		ret = ioctl(fence, FENCE_IOC_WAIT, &arg);
	} while (ret == -1 && errno == EINTR);

	/* ioctl() was successful. */
	if (ret == 0)
		return 1;

	/* ioctl() ended up with timeout. */
	if (errno == ETIME)
		return -1;

	/* ioctl() failed for some reason. */
	_log_errno();
	return 0;
}

tbm_fd
tbm_sync_fence_merge(const char *name, tbm_fd fence1, tbm_fd fence2)
{
	struct sync_merge_data data = { .fd2 = fence2 };

	_copy_string(data.name, name, 32);

	if (ioctl(fence1, FENCE_IOC_MERGE, &data) == -1) {
		_log_errno();
		return -1;
	}

	return data.fence;
}
