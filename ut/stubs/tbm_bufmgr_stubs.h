/**************************************************************************
 *
 * Copyright 2016 Samsung Electronics co., Ltd. All Rights Reserved.
 *
 * Contact: Konstantin Drabeniuk <k.drabeniuk@samsung.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
**************************************************************************/

#ifndef TBM_BUFMGR_STUBS_H
#define TBM_BUFMGR_STUBS_H

#include <tbm_bufmgr.h>

/* HELPER FUNCTIONS */

static int bo_size = 100;
static int bo2_size = 100;
static void *ret_bo = "bo_alloc";
static int UT_TBM_ERROR = 0;
static int TBM_BO_ALLOC_ERROR = 0;
static int TBM_BO_IMPORT_ERROR = 0;
static int bo_ret_flags = TBM_BO_SCANOUT;

static int ut_bo_size(tbm_bo bo)
{
	return bo_size;
}

static int ut_bo2_size(tbm_bo bo)
{
	return bo2_size;
}

static void *ut_bo_alloc(tbm_bo bo, int size, int flags)
{
	if (TBM_BO_ALLOC_ERROR)
		return NULL;

	return ret_bo;
}

static void *ut_bo_import(tbm_bo bo, unsigned int key)
{
	if (TBM_BO_IMPORT_ERROR)
		return NULL;

	return ret_bo;
}

static int ut_bo_get_flags(tbm_bo bo)
{
	return bo_ret_flags;
}

static void *ut_bo_import_fd(tbm_bo bo, tbm_fd fd)
{
	if (TBM_BO_IMPORT_ERROR)
		return NULL;

	return ret_bo;
}

static unsigned int ut_bo_export(tbm_bo bo)
{
	if (UT_TBM_ERROR)
		return 0;

	return 1;
}

static tbm_fd ut_bo_export_fd(tbm_bo bo)
{
	if (UT_TBM_ERROR)
		return -1;

	return 1;
}

static tbm_bo_handle ut_bo_get_handle(tbm_bo bo, int device)
{
	tbm_bo_handle ret;

	if (UT_TBM_ERROR)
		ret.ptr = NULL;
	else
		ret.ptr = (void *)12;

	return ret;
}

static tbm_bo_handle ut_bo_map(tbm_bo bo, int device, int opt)
{
	tbm_bo_handle ret;

	if (UT_TBM_ERROR)
		ret.ptr = NULL;
	else
		ret.ptr = (void *)12;

	return ret;
}

static int ut_bo_unmap(tbm_bo bo)
{
	if (UT_TBM_ERROR)
		return 0;

	return 1;
}

static void ut_tbm_data_free(void *user_data) {}

static int ut_bufmgr_bind_native_display(tbm_bufmgr bufmgr, void *NativeDisplay)
{
	if (UT_TBM_ERROR)
		return 0;

	return 1;
}

#endif /* TBM_BUFMGR_STUBS_H */
