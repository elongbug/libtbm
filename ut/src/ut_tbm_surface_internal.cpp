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

#include "gtest/gtest.h"

#include "tbm_bufmgr_int.h"

#include "pthread_stubs.h"
#include "stdlib_stubs.h"

/* HELPER FUNCTIONS */

#include "tbm_bufmgr.h"

static int UT_TBM_SURFACE_INTERNAL_ERROR = 0;
static struct _tbm_bufmgr ut_ret_bufmgr;
static int ut_tbm_bo_unmap_count = 0;
static int ut_tbm_data_free_called = 0;

static tbm_bufmgr ut_tbm_bufmgr_init(int fd)
{
	return &ut_ret_bufmgr;
}

static void ut_tbm_bufmgr_deinit(tbm_bufmgr bufmgr) {}

static int ut_surface_supported_format(uint32_t **formats, uint32_t *num)
{
	if (UT_TBM_SURFACE_INTERNAL_ERROR) {
		return 0;
	}

	return 1;
}

static tbm_bo_handle ut_tbm_bo_get_handle(tbm_bo bo, int device)
{
	tbm_bo_handle ut_ret_handle;
	return ut_ret_handle;
}

static int ut_tbm_bo_unmap(tbm_bo bo)
{
	ut_tbm_bo_unmap_count++;
}

static void ut_tbm_data_free(void *user_data)
{
	ut_tbm_data_free_called = 1;
}

#define pthread_mutex_lock ut_pthread_mutex_lock
#define pthread_mutex_unlock ut_pthread_mutex_unlock
#define pthread_mutex_init ut_pthread_mutex_init
#define calloc ut_calloc
#define free ut_free
#define tbm_bufmgr_init ut_tbm_bufmgr_init
#define tbm_bufmgr_deinit ut_tbm_bufmgr_deinit
#define tbm_bo_get_handle ut_tbm_bo_get_handle
#define tbm_bo_unmap ut_tbm_bo_unmap

#include "tbm_surface_internal.c"

static void _init_test()
{
	g_surface_bufmgr = NULL;
	PTHREAD_MUTEX_INIT_ERROR = 0;
	CALLOC_ERROR = 0;
	FREE_CALLED = 0;
	FREE_PTR = NULL;
	FREE_TESTED_PTR = NULL;
	free_called_for_tested_ptr = 0;
	free_call_count = 0;
	GETENV_ERROR = 0;
	UT_TBM_SURFACE_INTERNAL_ERROR = 0;
	ut_tbm_bo_unmap_count = 0;
	ut_tbm_data_free_called = 0;
}

/* tbm_surface_internal_delete_user_data() */

TEST(tbm_surface_internal_delete_user_data, work_flow_success_4)
{
	int ret = 0;
	int expected_ret = 1;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	unsigned long key = 1;
	int data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	LIST_INITHEAD(&surface.user_data_list);

	tbm_user_data *old_data = calloc(1, sizeof(*old_data));
	old_data->data = &data;
	old_data->free_func = ut_tbm_data_free;
	old_data->key = key;
	LIST_ADD(&old_data->item_link, &surface.user_data_list);
	FREE_TESTED_PTR = old_data;

	ret = tbm_surface_internal_delete_user_data(&surface, key);

	ASSERT_EQ(ret, expected_ret);
	ASSERT_EQ(free_called_for_tested_ptr, 1);
	ASSERT_EQ(ut_tbm_data_free_called, 1);
}

TEST(tbm_surface_internal_delete_user_data, work_flow_success_3)
{
	int ret = 1;
	int expected_ret = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	unsigned long key = 1;
	tbm_user_data old_data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	LIST_INITHEAD(&surface.user_data_list);

	old_data.key = key + 1;
	LIST_ADD(&old_data.item_link, &surface.user_data_list);

	ret = tbm_surface_internal_delete_user_data(&surface, key);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_delete_user_data, work_flow_success_2)
{
	int ret = 1;
	int expected_ret = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	LIST_INITHEAD(&surface.user_data_list);

	ret = tbm_surface_internal_delete_user_data(&surface, 1);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_delete_user_data, work_flow_success_1)
{
	int ret = 1;
	int expected_ret = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	ret = tbm_surface_internal_delete_user_data(&surface, 1);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_delete_user_data, null_ptr_fail_1)
{
	int ret = 1;
	int expected_ret = 0;

	_init_test();

	ret = tbm_surface_internal_delete_user_data(NULL, 1);

	ASSERT_EQ(ret, expected_ret);
}

/* tbm_surface_internal_get_user_data() */

TEST(tbm_surface_internal_get_user_data, work_flow_success_4)
{
	int ret = 0;
	int expected_ret = 1;
	unsigned long key = 1;
	void *data;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	int expected_data = 6;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	LIST_INITHEAD(&surface.user_data_list);

	tbm_user_data old_data;
	old_data.data = &expected_data;
	old_data.key = key;
	LIST_ADD(&old_data.item_link, &surface.user_data_list);

	ret = tbm_surface_internal_get_user_data(&surface, key, &data);

	ASSERT_EQ(ret, expected_ret);
	ASSERT_TRUE(data == &expected_data);
}

TEST(tbm_surface_internal_get_user_data, work_flow_success_3)
{
	int ret = 1;
	int expected_ret = 0;
	unsigned long key = 1;
	void *data;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	tbm_user_data old_data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	LIST_INITHEAD(&surface.user_data_list);

	old_data.key = key + 1;
	LIST_ADD(&old_data.item_link, &surface.user_data_list);

	ret = tbm_surface_internal_get_user_data(&surface, key, &data);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_user_data, work_flow_success_2)
{
	int ret = 1;
	int expected_ret = 0;
	unsigned long key = 1;
	void *data;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	LIST_INITHEAD(&surface.user_data_list);

	ret = tbm_surface_internal_get_user_data(&surface, key, &data);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_user_data, work_flow_success_1)
{
	int ret = 1;
	int expected_ret = 0;
	unsigned long key = 1;
	void *data;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_INITHEAD(&surface.user_data_list);

	ret = tbm_surface_internal_get_user_data(&surface, key, &data);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_user_data, null_ptr_fail_2)
{
	int ret = 1;
	int expected_ret = 0;
	unsigned long key = 1;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	LIST_INITHEAD(&surface.user_data_list);

	ret = tbm_surface_internal_get_user_data(&surface, key, NULL);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_user_data, null_ptr_fail_1)
{
	int ret = 1;
	int expected_ret = 0;
	unsigned long key = 1;
	void *data;

	_init_test();

	ret = tbm_surface_internal_get_user_data(NULL, key, &data);

	ASSERT_EQ(ret, expected_ret);
}

/* tbm_surface_internal_set_user_data() */

TEST(tbm_surface_internal_set_user_data, work_flow_success_4)
{
	int ret = 0;
	int expected_ret = 1;
	unsigned long key = 1;
	int data;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	tbm_user_data old_data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	old_data.data = &data;
	old_data.free_func = ut_tbm_data_free;
	old_data.key = key;
	LIST_INITHEAD(&surface.user_data_list);
	LIST_ADD(&old_data.item_link, &surface.user_data_list);

	ret = tbm_surface_internal_set_user_data(&surface, key, &data);

	ASSERT_EQ(ret, expected_ret);
	ASSERT_TRUE(old_data.data == &data);
	ASSERT_EQ(ut_tbm_data_free_called, 1);
}

TEST(tbm_surface_internal_set_user_data, work_flow_success_3)
{
	int ret = 0;
	int expected_ret = 1;
	unsigned long key = 1;
	int data;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	tbm_user_data old_data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	old_data.data = NULL;
	old_data.free_func = NULL;
	old_data.key = key;
	LIST_INITHEAD(&surface.user_data_list);
	LIST_ADD(&old_data.item_link, &surface.user_data_list);

	ret = tbm_surface_internal_set_user_data(&surface, key, &data);

	ASSERT_EQ(ret, expected_ret);
	ASSERT_TRUE(old_data.data == &data);
}

TEST(tbm_surface_internal_set_user_data, work_flow_success_2)
{
	int ret = 1;
	int expected_ret = 0;
	unsigned long key = 1;
	int data;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	tbm_user_data old_data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	old_data.key = key + 1;
	LIST_INITHEAD(&surface.user_data_list);
	LIST_ADD(&old_data.item_link, &surface.user_data_list);

	ret = tbm_surface_internal_set_user_data(&surface, key, &data);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_set_user_data, work_flow_success_1)
{
	int ret = 1;
	int expected_ret = 0;
	unsigned long key = 1;
	int data;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	ret = tbm_surface_internal_set_user_data(&surface, key, &data);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_set_user_data, null_ptr_fail_1)
{
	int ret = 1;
	int expected_ret = 0;
	unsigned long key = 1;
	int data;

	_init_test();

	ret = tbm_surface_internal_set_user_data(NULL, key, &data);

	ASSERT_EQ(ret, expected_ret);
}

/* tbm_surface_internal_add_user_data() */

TEST(tbm_surface_internal_add_user_data, work_flow_success_4)
{
	int ret = 0;
	int expected_ret = 1;
	unsigned long key = 1;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	tbm_user_data data;
	tbm_user_data *added_data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	data.key = key + 1;
	LIST_INITHEAD(&surface.user_data_list);
	LIST_ADD(&data.item_link, &surface.user_data_list);

	ret = tbm_surface_internal_add_user_data(&surface, key, ut_tbm_data_free);

	ASSERT_EQ(ret, expected_ret);

	added_data = user_data_lookup(&surface.user_data_list, key);

	ASSERT_TRUE(added_data != NULL);

	tbm_user_data copy_data = *added_data;
	free(added_data);

	ASSERT_EQ(copy_data.key, key);
	ASSERT_TRUE(copy_data.free_func == ut_tbm_data_free);
	ASSERT_TRUE(copy_data.data == NULL);
}

TEST(tbm_surface_internal_add_user_data, work_flow_success_3)
{
	int ret = 0;
	int expected_ret = 0;
	unsigned long key = 1;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	tbm_user_data data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	data.key = key + 1;
	LIST_INITHEAD(&surface.user_data_list);
	LIST_ADD(&data.item_link, &surface.user_data_list);
	CALLOC_ERROR = 1;

	ret = tbm_surface_internal_add_user_data(&surface, key, ut_tbm_data_free);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_add_user_data, work_flow_success_2)
{
	int ret = 0;
	int expected_ret = 0;
	unsigned long key = 1;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;
	tbm_user_data data;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	data.key = key;
	LIST_INITHEAD(&surface.user_data_list);
	LIST_ADD(&data.item_link, &surface.user_data_list);

	ret = tbm_surface_internal_add_user_data(&surface, key, ut_tbm_data_free);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_add_user_data, work_flow_success_1)
{
	int ret = 0;
	int expected_ret = 0;
	unsigned long key = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	ret = tbm_surface_internal_add_user_data(&surface, key, ut_tbm_data_free);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_add_user_data, null_ptr_fail_1)
{
	int ret = 0;
	int expected_ret = 0;
	unsigned long key = 0;

	_init_test();

	ret = tbm_surface_internal_add_user_data(NULL, key, ut_tbm_data_free);

	ASSERT_EQ(ret, expected_ret);
}

/* tbm_surface_internal_set_debug_pid() */

TEST(tbm_surface_internal_set_debug_pid, work_flow_success_1)
{
	unsigned int pid = 20;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	tbm_surface_internal_set_debug_pid(&surface, pid);

	ASSERT_EQ(pid, surface.debug_pid);
}

/* tbm_surface_internal_get_plane_bo_idx() */

TEST(tbm_surface_internal_get_plane_bo_idx, work_flow_success_3)
{
	int actual = 1;
	int plane_idx = 0;
	int expected_bo_idx = 10;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.planes_bo_idx[plane_idx] = expected_bo_idx;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	actual = tbm_surface_internal_get_plane_bo_idx(&surface, plane_idx);

	ASSERT_EQ(actual, expected_bo_idx);
}

TEST(tbm_surface_internal_get_plane_bo_idx, work_flow_success_2)
{
	int actual = 1;
	int expected = 0;
	int plane_idx = -1;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	actual = tbm_surface_internal_get_plane_bo_idx(&surface, plane_idx);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_internal_get_plane_bo_idx, work_flow_success_1)
{
	int actual = 1;
	int expected = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	actual = tbm_surface_internal_get_plane_bo_idx(&surface, 0);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_internal_get_plane_bo_idx, null_ptr_fail_1)
{
	_init_test();
	int actual = 1;
	int expected = 0;

	actual = tbm_surface_internal_get_plane_bo_idx(NULL, 0);

	ASSERT_EQ(actual, expected);
}

/* tbm_surface_internal_get_format() */

TEST(tbm_surface_internal_get_format, work_flow_success_2)
{
	unsigned int actual = 1;
	int expected_format = 768;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	surface.info.format = expected_format;

	actual = tbm_surface_internal_get_format(&surface);

	ASSERT_EQ(actual, expected_format);
}

TEST(tbm_surface_internal_get_format, work_flow_success_1)
{
	unsigned int actual = 1;
	unsigned int expected = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	actual = tbm_surface_internal_get_format(&surface);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_internal_get_format, null_ptr_fail_1)
{
	unsigned int actual = 1;
	unsigned int expected = 0;

	_init_test();

	actual = tbm_surface_internal_get_format(NULL);

	ASSERT_EQ(actual, expected);
}

/* tbm_surface_internal_get_height() */

TEST(tbm_surface_internal_get_height, work_flow_success_2)
{
	unsigned int actual = 1;
	int expected_height = 768;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	surface.info.height = expected_height;

	actual = tbm_surface_internal_get_height(&surface);

	ASSERT_EQ(actual, expected_height);
}

TEST(tbm_surface_internal_get_height, work_flow_success_1)
{
	unsigned int actual = 1;
	unsigned int expected = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	actual = tbm_surface_internal_get_height(&surface);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_internal_get_height, null_ptr_fail_1)
{
	unsigned int actual = 1;
	unsigned int expected = 0;

	_init_test();

	actual = tbm_surface_internal_get_height(NULL);

	ASSERT_EQ(actual, expected);
}

/* tbm_surface_internal_get_width() */

TEST(tbm_surface_internal_get_width, work_flow_success_2)
{
	unsigned int actual = 1;
	int expected_width = 1024;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	surface.info.width = expected_width;

	actual = tbm_surface_internal_get_width(&surface);

	ASSERT_EQ(actual, expected_width);
}

TEST(tbm_surface_internal_get_width, work_flow_success_1)
{
	unsigned int actual = 1;
	unsigned int expected = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	actual = tbm_surface_internal_get_width(&surface);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_internal_get_width, null_ptr_fail_1)
{
	unsigned int actual = 1;
	unsigned int expected = 0;

	_init_test();

	actual = tbm_surface_internal_get_width(NULL);

	ASSERT_EQ(actual, expected);
}

/* tbm_surface_internal_unmap() */

TEST(tbm_surface_internal_unmap, work_flow_success_1)
{
	struct _tbm_surface surface;
	int count = 10;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.num_bos = count;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	tbm_surface_internal_unmap(&surface);

	ASSERT_EQ(ut_tbm_bo_unmap_count, count);
}

/* tbm_surface_internal_get_info() */

TEST(tbm_surface_internal_get_info, work_flow_success_2)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_surface_info_s info;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.info.num_planes = 0;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	uint32_t expected_width = 2;
	uint32_t expected_height = 3;
	uint32_t expected_format = 4;
	uint32_t expected_bpp = 5;
	uint32_t expected_size = 6;
	surface.info.width = expected_width;
	surface.info.height = expected_height;
	surface.info.format = expected_format;
	surface.info.bpp = expected_bpp;
	surface.info.size = expected_size;
	surface.info.num_planes = 0;
	surface.num_bos = 1;

	ret = tbm_surface_internal_get_info(&surface, 1, &info, 2);

	ASSERT_EQ(ret, expected_ret);
	ASSERT_EQ(info.width, expected_width);
	ASSERT_EQ(info.height, expected_height);
	ASSERT_EQ(info.format, expected_format);
	ASSERT_EQ(info.bpp, expected_bpp);
	ASSERT_EQ(info.size, expected_size);
}

TEST(tbm_surface_internal_get_info, work_flow_success_1)
{
	int ret = 1;
	int expected_ret = 0;
	tbm_surface_info_s info;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	ret = tbm_surface_internal_get_info(&surface, 1, &info, 1);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_info, null_ptr_fail_1)
{
	int ret = 1;
	int expected_ret = 0;
	tbm_surface_info_s info;

	_init_test();

	ret = tbm_surface_internal_get_info(NULL, 1, &info, 1);

	ASSERT_EQ(ret, expected_ret);
}

/* tbm_surface_internal_get_plane_data() */

TEST(tbm_surface_internal_get_plane_data, work_flow_success_4)
{
	int ret = 0;
	int expected_ret = 1;
	int plane_idx = 0;
	uint32_t size = 0;
	uint32_t offset = 0;
	uint32_t pitch = 0;
	uint32_t expected_size = 1024;
	uint32_t expected_offset = 10;
	uint32_t expected_pitch = 20;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.info.planes[plane_idx].size = expected_size;
	surface.info.planes[plane_idx].offset = expected_offset;
	surface.info.planes[plane_idx].stride = expected_pitch;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	surface.info.num_planes = 1;

	ret = tbm_surface_internal_get_plane_data(&surface, plane_idx, &size,
											  &offset, &pitch);

	ASSERT_EQ(ret, expected_ret);
	ASSERT_EQ(size, expected_size);
	ASSERT_EQ(offset, expected_offset);
	ASSERT_EQ(pitch, expected_pitch);
}

TEST(tbm_surface_internal_get_plane_data, work_flow_success_3)
{
	int ret = 0;
	int expected_ret = 0;
	int plane_idx = 3;
	uint32_t size = 0;
	uint32_t offset = 0;
	uint32_t pitch = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);
	surface.info.num_planes = plane_idx - 1;

	ret = tbm_surface_internal_get_plane_data(&surface, plane_idx, &size,
											  &offset, &pitch);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_plane_data, work_flow_success_2)
{
	int ret = 0;
	int expected_ret = 0;
	int plane_idx = -1;
	uint32_t size = 0;
	uint32_t offset = 0;
	uint32_t pitch = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	ret = tbm_surface_internal_get_plane_data(&surface, plane_idx, &size,
											  &offset, &pitch);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_plane_data, work_flow_success_1)
{
	int ret = 0;
	int expected_ret = 0;
	uint32_t size = 0;
	uint32_t offset = 0;
	uint32_t pitch = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	ret = tbm_surface_internal_get_plane_data(&surface, 1, &size,
											  &offset, &pitch);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_plane_data, null_ptr_fail_1)
{
	int ret = 0;
	int expected_ret = 0;
	uint32_t size = 0;
	uint32_t offset = 0;
	uint32_t pitch = 0;

	_init_test();

	ret = tbm_surface_internal_get_plane_data(NULL, 1, &size, &offset, &pitch);

	ASSERT_EQ(ret, expected_ret);
}

/* tbm_surface_internal_get_size() */

TEST(tbm_surface_internal_get_size, work_flow_success_2)
{
	unsigned int actual_size = 0;
	unsigned int expected_size = 1024;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.info.size = expected_size;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	actual_size = tbm_surface_internal_get_size(&surface);

	ASSERT_EQ(actual_size, expected_size);
	ASSERT_EQ(actual_size, expected_size);
}

TEST(tbm_surface_internal_get_size, work_flow_success_1)
{
	unsigned int actual_size = 0;
	unsigned int expected_size = 0;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	actual_size = tbm_surface_internal_get_size(&surface);

	ASSERT_EQ(actual_size, expected_size);
}

TEST(tbm_surface_internal_get_size, null_ptr_fail_1)
{
	unsigned int actual_size = 0;
	unsigned int expected_size = 0;

	_init_test();

	actual_size = tbm_surface_internal_get_size(NULL);

	ASSERT_EQ(actual_size, expected_size);
}

/* tbm_surface_internal_get_bo() */

TEST(tbm_surface_internal_get_bo, work_flow_success_3)
{
	tbm_bo bo;
	int bo_idx = 0;
	struct _tbm_bo expected_bo;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.bos[bo_idx] = &expected_bo;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	bo = tbm_surface_internal_get_bo(&surface, bo_idx);

	ASSERT_TRUE(bo == &expected_bo);
}

TEST(tbm_surface_internal_get_bo, work_flow_success_2)
{
	tbm_bo bo;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);

	bo = tbm_surface_internal_get_bo(&surface, 1);

	ASSERT_TRUE(bo == NULL);
}

TEST(tbm_surface_internal_get_bo, work_flow_success_1)
{
	tbm_bo bo;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	bo = tbm_surface_internal_get_bo(&surface, -1);

	ASSERT_TRUE(bo == NULL);
}

TEST(tbm_surface_internal_get_bo, null_ptr_fail_1)
{
	tbm_bo bo;

	_init_test();

	bo = tbm_surface_internal_get_bo(NULL, 1);

	ASSERT_TRUE(bo == NULL);
}

/* tbm_surface_internal_get_num_bos() */

TEST(tbm_surface_internal_get_num_bos, work_flow_success_1)
{
	int actual_num = 0;
	int expected_num = 5;
	struct _tbm_surface surface;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.num_bos = expected_num;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	actual_num = tbm_surface_internal_get_num_bos(&surface);

	ASSERT_EQ(actual_num, expected_num);
}

/* tbm_surface_internal_unref() */

TEST(tbm_surface_internal_unref, work_flow_success_2)
{
	tbm_surface_h surface = calloc(1, sizeof(*surface));
	surface->refcnt = 1;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_INITHEAD(&surface->user_data_list);
	LIST_ADD(&surface->item_link, &bufmgr.surf_list);
	surface->num_bos = 0;
	FREE_TESTED_PTR = surface;
	surface->bufmgr = &bufmgr;

	tbm_surface_internal_unref(surface);

	ASSERT_EQ(free_called_for_tested_ptr, 1);
	ASSERT_TRUE(g_surface_bufmgr == NULL);
}

TEST(tbm_surface_internal_unref, work_flow_success_1)
{
	struct _tbm_surface surface;
	int expected_refcnt = 9;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.refcnt = expected_refcnt + 1;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	tbm_surface_internal_unref(&surface);

	ASSERT_EQ(expected_refcnt, surface.refcnt);
}

/* tbm_surface_internal_ref() */

TEST(tbm_surface_internal_ref, work_flow_success_1)
{
	struct _tbm_surface surface;
	int expected_refcnt = 10;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.refcnt = expected_refcnt - 1;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	tbm_surface_internal_ref(&surface);

	ASSERT_EQ(expected_refcnt, surface.refcnt);
}

/* tbm_surface_internal_destroy() */

TEST(tbm_surface_internal_destroy, work_flow_success_2)
{
	tbm_surface_h surface = calloc(1, sizeof(*surface));
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface->refcnt = 1;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_INITHEAD(&surface->user_data_list);
	LIST_ADD(&surface->item_link, &bufmgr.surf_list);
	surface->num_bos = 0;
	FREE_TESTED_PTR = surface;
	surface->bufmgr = &bufmgr;

	tbm_surface_internal_destroy(surface);

	ASSERT_EQ(free_called_for_tested_ptr, 1);
	ASSERT_TRUE(g_surface_bufmgr == NULL);
}

TEST(tbm_surface_internal_destroy, work_flow_success_1)
{
	struct _tbm_surface surface;
	int expected_refcnt = 9;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	surface.refcnt = expected_refcnt + 1;
	g_surface_bufmgr = &bufmgr;
	LIST_INITHEAD(&bufmgr.surf_list);
	LIST_ADD(&surface.item_link, &bufmgr.surf_list);

	tbm_surface_internal_destroy(&surface);

	ASSERT_EQ(expected_refcnt, surface.refcnt);
}

/* tbm_surface_internal_get_bpp() */

TEST(tbm_surface_internal_get_bpp, work_flow_success_59)
{
	int ret = 0;
	int expected_ret = 24;
	tbm_format format = TBM_FORMAT_YVU444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_58)
{
	int ret = 0;
	int expected_ret = 24;
	tbm_format format = TBM_FORMAT_YUV444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_57)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_YVU422;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_56)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_YUV422;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_55)
{
	int ret = 0;
	int expected_ret = 12;
	tbm_format format = TBM_FORMAT_YVU420;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_54)
{
	int ret = 0;
	int expected_ret = 12;
	tbm_format format = TBM_FORMAT_YUV420;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_53)
{
	int ret = 0;
	int expected_ret = 12;
	tbm_format format = TBM_FORMAT_YVU411;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_52)
{
	int ret = 0;
	int expected_ret = 12;
	tbm_format format = TBM_FORMAT_YUV411;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_51)
{
	int ret = 0;
	int expected_ret = 9;
	tbm_format format = TBM_FORMAT_YVU410;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_50)
{
	int ret = 0;
	int expected_ret = 9;
	tbm_format format = TBM_FORMAT_YUV410;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_49)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_NV61;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_48)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_NV16;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_47)
{
	int ret = 0;
	int expected_ret = 12;
	tbm_format format = TBM_FORMAT_NV21;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_46)
{
	int ret = 0;
	int expected_ret = 12;
	tbm_format format = TBM_FORMAT_NV12MT;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_45)
{
	int ret = 0;
	int expected_ret = 12;
	tbm_format format = TBM_FORMAT_NV12;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_44)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_AYUV;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_43)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_VYUY;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_42)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_UYVY;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_41)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_YVYU;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_40)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_YUYV;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_39)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_BGRA1010102;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_38)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_RGBA1010102;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_37)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_ABGR2101010;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_36)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_ARGB2101010;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_35)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_BGRX1010102;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_34)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_RGBX1010102;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_33)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_XBGR2101010;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_32)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_XRGB2101010;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_31)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_BGRA8888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_30)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_RGBA8888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_29)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_ABGR8888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_28)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_ARGB8888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_27)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_BGRX8888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_26)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_RGBX8888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_25)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_XBGR8888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_24)
{
	int ret = 0;
	int expected_ret = 32;
	tbm_format format = TBM_FORMAT_XRGB8888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_23)
{
	int ret = 0;
	int expected_ret = 24;
	tbm_format format = TBM_FORMAT_BGR888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_22)
{
	int ret = 0;
	int expected_ret = 24;
	tbm_format format = TBM_FORMAT_RGB888;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_21)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_BGR565;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_20)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_RGB565;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_19)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_BGRA5551;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_18)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_RGBA5551;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_17)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_ABGR1555;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_16)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_ARGB1555;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_15)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_BGRX5551;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_14)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_RGBX5551;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_13)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_XBGR1555;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_12)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_XRGB1555;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_11)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_BGRA4444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_10)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_RGBA4444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_9)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_ABGR4444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_8)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_ARGB4444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_7)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_BGRX4444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_6)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_RGBX4444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_5)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_XBGR4444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_4)
{
	int ret = 0;
	int expected_ret = 16;
	tbm_format format = TBM_FORMAT_XRGB4444;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_3)
{
	int ret = 0;
	int expected_ret = 8;
	tbm_format format = TBM_FORMAT_BGR233;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_2)
{
	int ret = 0;
	int expected_ret = 8;
	tbm_format format = TBM_FORMAT_RGB332;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_bpp, work_flow_success_1)
{
	int ret = 0;
	int expected_ret = 8;
	tbm_format format = TBM_FORMAT_C8;

	_init_test();

	ret = tbm_surface_internal_get_bpp(format);

	ASSERT_EQ(ret, expected_ret);
}

/* tbm_surface_internal_get_num_planes() */

TEST(tbm_surface_internal_get_num_planes, work_flow_success_60)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YVU444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_59)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YUV444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_58)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YVU422;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_57)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YUV422;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_56)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YVU420;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_55)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YUV420;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_54)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YVU411;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_53)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YUV411;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_52)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YVU410;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_51)
{
	int ret = 0;
	int expected_ret = 3;
	tbm_format format = TBM_FORMAT_YUV410;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_50)
{
	int ret = 0;
	int expected_ret = 2;
	tbm_format format = TBM_FORMAT_NV61;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_49)
{
	int ret = 0;
	int expected_ret = 2;
	tbm_format format = TBM_FORMAT_NV16;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_48)
{
	int ret = 0;
	int expected_ret = 2;
	tbm_format format = TBM_FORMAT_NV21;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_47)
{
	int ret = 0;
	int expected_ret = 2;
	tbm_format format = TBM_FORMAT_NV12MT;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_46)
{
	int ret = 0;
	int expected_ret = 2;
	tbm_format format = TBM_FORMAT_NV12;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_45)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_AYUV;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_44)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_VYUY;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_43)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_UYVY;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_42)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_YVYU;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_41)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_YUYV;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_40)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGRA1010102;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_39)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGBA1010102;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_38)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ABGR2101010;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_37)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ARGB2101010;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_36)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGRX1010102;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_35)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGBX1010102;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_34)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_XBGR2101010;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_33)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_XRGB2101010;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_32)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGRA8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_31)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGBA8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_30)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ABGR8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_29)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ABGR8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_28)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ARGB8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_27)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGRX8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_26)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGBX8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_25)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_XBGR8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_24)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_XRGB8888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_23)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGR888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_22)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGB888;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_21)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGR565;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_20)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGB565;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_19)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGRA5551;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_18)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGBA5551;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_17)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ABGR1555;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_16)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ARGB1555;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_15)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGRX5551;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_14)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGBX5551;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_13)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_XBGR1555;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_12)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_XRGB1555;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_11)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGRA4444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_10)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGBA4444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_9)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ABGR4444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_8)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_ARGB4444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_7)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGRX4444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_6)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGBX4444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_5)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_XBGR4444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_4)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_XRGB4444;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_3)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_BGR233;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_2)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_RGB332;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

TEST(tbm_surface_internal_get_num_planes, work_flow_success_1)
{
	int ret = 0;
	int expected_ret = 1;
	tbm_format format = TBM_FORMAT_C8;

	_init_test();

	ret = tbm_surface_internal_get_num_planes(format);

	ASSERT_EQ(ret, expected_ret);
}

/* tbm_surface_internal_query_supported_formats() */

TEST(tbm_surface_internal_query_supported_formats, work_flow_success_3)
{
	int ret = 0;
	int expecte_ret = 1;
	uint32_t *formats, num;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	ut_ret_bufmgr.backend = &backend;
	backend.surface_supported_format = ut_surface_supported_format;

	ret = tbm_surface_internal_query_supported_formats(&formats, num);

	ASSERT_EQ(ret, expecte_ret);
}

TEST(tbm_surface_internal_query_supported_formats, work_flow_success_2)
{
	int ret = 0;
	int expecte_ret = 0;
	uint32_t *formats, num;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	ut_ret_bufmgr.backend = &backend;
	backend.surface_supported_format = ut_surface_supported_format;
	UT_TBM_SURFACE_INTERNAL_ERROR = 1;

	ret = tbm_surface_internal_query_supported_formats(&formats, num);

	ASSERT_EQ(ret, expecte_ret);
}

TEST(tbm_surface_internal_query_supported_formats, work_flow_success_1)
{
	int ret = 0;
	int expecte_ret = 0;
	uint32_t *formats, num;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;

	_init_test();

	g_surface_bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	ut_ret_bufmgr.backend = &backend;
	backend.surface_supported_format = NULL;

	ret = tbm_surface_internal_query_supported_formats(&formats, num);

	ASSERT_EQ(ret, expecte_ret);
}
