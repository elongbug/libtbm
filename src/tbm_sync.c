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

#include "tbm_bufmgr.h"
#include "tbm_bufmgr_int.h"
#include "tbm_sync.h"

#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#define SYNC_IOC_MAGIC               '>'
#define SYNC_IOC_WAIT                _IOW(SYNC_IOC_MAGIC, 0, __s32)
#define SYNC_IOC_MERGE               _IOWR(SYNC_IOC_MAGIC, 1, struct sync_merge_data)
#define SW_SYNC_IOC_MAGIC            'W'
#define SW_SYNC_IOC_CREATE_FENCE     _IOWR(SW_SYNC_IOC_MAGIC, 0,\
                                         struct sw_sync_create_fence_data)
#define SW_SYNC_IOC_INC              _IOW(SW_SYNC_IOC_MAGIC, 1, __u32)

#define SYNC_DEVICE_PATH             "/dev/sw_sync"

struct _tbm_sync_timeline {
	int fd;
};

struct _tbm_sync_fence {
	int fd;
};

static pthread_mutex_t tbm_sync_lock;
static int tbm_sync_support = 0;

static bool
_tbm_sync_mutex_init(void)
{
	static bool tbm_sync_mutex_init = false;

	if (tbm_sync_mutex_init)
		return true;

	if (pthread_mutex_init(&tbm_sync_lock, NULL)) {
		TBM_LOG_E("fail: tbm_sync mutex init\n");
		return false;
	}

	tbm_sync_mutex_init = true;

	return true;
}

static void
_tbm_sync_mutex_lock(void)
{
	if (!_tbm_sync_mutex_init())
		return;

	pthread_mutex_lock(&tbm_sync_lock);
}

static void
_tbm_sync_mutex_unlock(void)
{
	pthread_mutex_unlock(&tbm_sync_lock);
}

static tbm_sync_error_e
_tbm_sync_check_capability(void)
{
#ifdef NOT_IMPELMENT_YET
	tbm_bufmgr bufmgr = NULL;
	unsigned int capabilities = TBM_BUFMGR_CAPABILITY_NONE;
#endif
	struct stat st_buf;

    if (tbm_sync_support)
		return TBM_SYNC_ERROR_NONE;

#ifdef NOT_IMPELMENT_YET
	/* check the bufmgr */
	bufmgr = _tbm_bufmgr_get_bufmgr();
	if (!bufmgr) {
		return TBM_SYNC_ERROR_INVALID_OPERATION;
	}

	/* check the tbm_sync capability */
	capabilities = tbm_bufmgr_get_capability(bufmgr);

	if ((capabilities&TBM_BUFMGR_CAPABILITY_TBM_SYNC) != TBM_BUFMGR_CAPABILITY_TBM_SYNC) {
		//TODO: check the sw_sync device node... to verify the timeline sync
		tbm_sync_support = 1;

		return TBM_SYNC_ERROR_INVALID_OPERATION;
	}
#endif

	if (stat(SYNC_DEVICE_PATH, &st_buf) == 0) {
		tbm_sync_support = 1;
	} else {
		TBM_LOG_E("TBM_SYNC not supported\n");
		return TBM_SYNC_ERROR_INVALID_OPERATION;
	}

	return TBM_SYNC_ERROR_NONE;
}

tbm_sync_timeline_h
tbm_sync_timeline_create(tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	tbm_sync_timeline_h timeline = NULL;
	int fd;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	fd = open(SYNC_DEVICE_PATH, O_RDWR);
	if (fd < 0) {
		ret = TBM_SYNC_ERROR_INVALID_OPERATION;
		TBM_LOG_E("%s:%d(%s)\n", "TBM_SYNC open failed", errno, strerror(errno));
	} else {
		struct _tbm_sync_timeline *timeline_handle =
			calloc(1, sizeof(struct _tbm_sync_timeline));

		if (timeline_handle == NULL) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s\n", "TBM_SYNC calloc failed");
			close(fd);
		} else {
			timeline = timeline_handle;
		}
	}

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return timeline;
}

tbm_sync_error_e
tbm_sync_timeline_destroy(tbm_sync_timeline_h timeline)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (timeline) {
		struct _tbm_sync_timeline *timeline_handle = timeline;

		if (timeline_handle->fd != -1)
			close(timeline_handle->fd);
		free(timeline);
	} else {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	}

done:
	_tbm_sync_mutex_unlock();

    return ret;
}

tbm_sync_timeline_h
tbm_sync_timeline_import(int fd, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	tbm_sync_timeline_h timeline = NULL;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (fd < 0) {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	} else {
		struct _tbm_sync_timeline *timeline_handle =
			calloc(1, sizeof(struct _tbm_sync_timeline));

		if (timeline_handle == NULL) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s\n", "TBM_SYNC calloc failed");
		} else {
			timeline_handle->fd = fd;
			timeline = timeline_handle;
		}
	}

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return timeline;
}

int
tbm_sync_timeline_export(tbm_sync_timeline_h timeline, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	int fd = -1;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (timeline) {
		struct _tbm_sync_timeline *timeline_handle = timeline;
		fd = dup(timeline_handle->fd);
		if (fd == -1) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s:%d(%s)\n", "TBM_SYNC timeline dup failed",
					  errno, strerror(errno));
		}
	} else {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	}

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return fd;
}

tbm_sync_error_e
tbm_sync_timeline_increase_count(tbm_sync_timeline_h timeline, unsigned int interval)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (timeline) {
		struct _tbm_sync_timeline *timeline_handle = timeline;
		__u32 arg = interval;

		if (ioctl(timeline_handle->fd, SW_SYNC_IOC_INC, &arg) == -1) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s:%d(%s)\n", "TBM_SYNC timeline inc failed", errno, strerror(errno));
		}
	} else {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	}

done:
	_tbm_sync_mutex_unlock();

	return ret;
}

unsigned int
tbm_sync_timeline_get_cur_count(tbm_sync_timeline_h timeline, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	unsigned int cur_count = 0;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	/* TODO: sync_timeline_get_cur_count */

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return cur_count;
}

tbm_sync_fence_h
tbm_sync_fence_create(tbm_sync_timeline_h timeline, const char *name, unsigned int count_val, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	tbm_sync_fence_h fence = NULL;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (timeline) {
		struct _tbm_sync_timeline *timeline_handle = timeline;
		struct sw_sync_create_fence_data {
			__u32 value;
			char *name;
			__s32 fence;
		} data;

		data.value = count_val;
		strncpy(data.name, name ? name : "", 32);
		data.name[31] = '\0';

		if (ioctl(timeline_handle->fd, SW_SYNC_IOC_CREATE_FENCE, &data) == -1) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s:%d(%s)\n", "TBM_SYNC create fence failed",
					  errno, strerror(errno));
		} else {
			struct _tbm_sync_fence *fence_handle =
				calloc(1, sizeof(struct _tbm_sync_fence));

			if (fence_handle == NULL) {
				ret = TBM_SYNC_ERROR_INVALID_OPERATION;
				TBM_LOG_E("%s\n", "TBM_SYNC calloc failed");
				close(data.fence);
			} else {
				fence_handle->fd = data.fence;
				fence = fence_handle;
			}
		}
	} else {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	}

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return fence;
}

tbm_sync_error_e
tbm_sync_fence_destroy(tbm_sync_fence_h fence)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (fence) {
		struct _tbm_sync_fence *fence_handle = fence;

		if (fence_handle->fd != -1)
			close(fence_handle->fd);
		free(fence);
	}

done:
	_tbm_sync_mutex_unlock();

	return ret;
}

tbm_sync_error_e
tbm_sync_fence_wait(tbm_sync_fence_h fence, int timeout)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (fence) {
		struct _tbm_sync_fence *fence_handle = fence;
		__s32 to = timeout;

		if (ioctl(fence_handle->fd, SYNC_IOC_WAIT, &to) == -1) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s:%d(%s)\n", "TBM_SYNC fence wait failed", errno, strerror(errno));
		}
	} else {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	}

done:
	_tbm_sync_mutex_unlock();

	return ret;
}

tbm_sync_fence_h
tbm_sync_fence_merge(tbm_sync_fence_h fence1, tbm_sync_fence_h fence2, const char *name, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	tbm_sync_fence_h fence = NULL;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (fence1 && fence2) {
		struct _tbm_sync_fence *fence_handle1 = fence1;
		struct _tbm_sync_fence *fence_handle2 = fence2;

		struct sync_merge_data {
			__s32 fd2;
			char name[32];
			__s32 fence;
		} data;

		data.fd2 = fence_handle2->fd;
		strncpy(data.name, name ? name : "", 32);
		data.name[31] = '\0';

		if (ioctl(fence_handle1->fd, SYNC_IOC_MERGE, &data) == -1) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s:%d(%s)\n", "TBM_SYNC fence merge failed",
					  errno, strerror(errno));
		} else {
			struct _tbm_sync_fence *fence_handle =
				calloc(1, sizeof(struct _tbm_sync_fence));

			if (fence_handle == NULL) {
				ret = TBM_SYNC_ERROR_INVALID_OPERATION;
				TBM_LOG_E("%s\n", "TBM_SYNC calloc failed");
				close(data.fence);
			} else {
				fence_handle->fd = data.fence;
			}
		}
	} else {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	}

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return fence;
}

unsigned int
tbm_sync_fence_get_count_val(tbm_sync_fence_h fence, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	unsigned int count_val = 0;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	/* TODO: sync_fence_get_count_val */

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return count_val;
}

tbm_sync_fence_h
tbm_sync_fence_import(int fd, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	tbm_sync_fence_h fence = NULL;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (fd < 0) {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	} else {
		struct _tbm_sync_fence *fence_handle =
			calloc(1, sizeof(struct _tbm_sync_fence));

		if (fence_handle == NULL) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s\n", "TBM_SYNC calloc failed");
		} else {
			fence_handle->fd = fd;
			fence = fence_handle;
		}
	}

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return fence;
}

int
tbm_sync_fence_export(tbm_sync_fence_h fence, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	int fd = -1;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	if (fence) {
		struct _tbm_sync_fence *fence_handle = fence;
		fd = dup(fence_handle->fd);
		if (fd == -1) {
			ret = TBM_SYNC_ERROR_INVALID_OPERATION;
			TBM_LOG_E("%s:%d(%s)\n", "TBM_SYNC fence dup failed",
					  errno, strerror(errno));
		}
	} else {
		ret = TBM_SYNC_ERROR_INVALID_PARAMETER;
	}

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return fd;
}
