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
	tbm_bufmgr bufmgr = NULL;
	unsigned int capabilities = TBM_BUFMGR_CAPABILITY_NONE;

    if (tbm_sync_support)
		return TBM_SYNC_ERROR_NONE;

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

	return TBM_SYNC_ERROR_NONE;
}

tbm_sync_timeline_h
tbm_sync_timeline_create(tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	tbm_sync_timeline_h timeline = NULL;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	/* TODO: sync_timeline_create */


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

	/* TODO: sync_timeline_destroy */


done:
	_tbm_sync_mutex_unlock();

    return ret;
}

tbm_sync_timeline_h
tbm_sync_timeline_import(tbm_fd fd, tbm_sync_error_e *error)
{
	tbm_sync_error_e ret = TBM_SYNC_ERROR_NONE;
	tbm_sync_timeline_h timeline = NULL;

	_tbm_sync_mutex_lock();

	/* check the tbm_sync capability */
	ret = _tbm_sync_check_capability();;
	if (ret != TBM_SYNC_ERROR_NONE)
		goto done;

	/* TODO: sync_timeline_import */

done:
	if (error)
		*error = ret;

	_tbm_sync_mutex_unlock();

	return timeline;
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

	/* TODO: sync_timeline_increase_count */

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

	/* TODO: sync_fence_create */

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

	/* TODO: sync_fence_destroy */

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

	/* TODO: sync_fence_wait */

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

	/* TODO: tbm_sync_fence_merge */

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

