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

#include "ut_tbm_bufmgr.h"

#include "pthread_stubs.h"
#include "stdlib_stubs.h"

#define pthread_mutex_lock ut_pthread_mutex_lock
#define pthread_mutex_unlock ut_pthread_mutex_unlock
#define pthread_mutex_init ut_pthread_mutex_init
#define calloc ut_calloc
#define free ut_free

#include "tbm_bufmgr.c"
#include "tbm_bufmgr_stubs.h"

static void _init_test()
{
	gBufMgr = NULL;
	PTHREAD_MUTEX_INIT_ERROR = 0;
	CALLOC_ERROR = 0;
	FREE_CALLED = 0;
	FREE_PTR = NULL;
	FREE_TESTED_PTR = NULL;
	free_called_for_tested_ptr = 0;
	free_call_count = 0;
	GETENV_ERROR = 0;
	UT_TBM_ERROR = 0;
	TBM_BO_ALLOC_ERROR = 0;
	TBM_BO_IMPORT_ERROR = 0;
	bo_ret_flags = TBM_BO_SCANOUT;
}

/* tbm_bufmgr_bind_native_display() */

TEST(tbm_bufmgr_bind_native_display, work_flow_success_3)
{
	int expected = 1;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int actual;

	_init_test();

	bufmgr.backend = &backend;
	backend.bufmgr_bind_native_display = ut_bufmgr_bind_native_display;

	actual = tbm_bufmgr_bind_native_display(&bufmgr, NULL);

	ASSERT_EQ(expected, actual);
}

TEST(tbm_bufmgr_bind_native_display, work_flow_success_2)
{
	int expected = 0;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int actual;

	_init_test();

	bufmgr.backend = &backend;
	backend.bufmgr_bind_native_display = ut_bufmgr_bind_native_display;
	UT_TBM_ERROR = 1;

	actual = tbm_bufmgr_bind_native_display(&bufmgr, NULL);

	ASSERT_EQ(expected, actual);
}

TEST(tbm_bufmgr_bind_native_display, work_flow_success_1)
{
	int expected = 1;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int actual;

	_init_test();

	bufmgr.backend = &backend;
	backend.bufmgr_bind_native_display = NULL;

	actual = tbm_bufmgr_bind_native_display(&bufmgr, NULL);

	ASSERT_EQ(expected, actual);
}

TEST(tbm_bufmgr_bind_native_display, null_ptr_fail_1)
{
	int expected = 0;
	int actual;

	_init_test();

	actual = tbm_bufmgr_bind_native_display(NULL, NULL);

	ASSERT_EQ(expected, actual);
}

/* tbm_bo_get_flags() */

TEST(tbm_bo_get_flags, work_flow_success_2)
{
	int expected_flags = 5;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual_flags;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.flags = expected_flags;

	actual_flags = tbm_bo_get_flags(&bo);

	ASSERT_EQ(actual_flags, expected_flags);
}

TEST(tbm_bo_get_flags, work_flow_success_1)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_get_flags(&bo);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_get_flags, null_ptr_fail_1)
{
	int expected = 0;
	int actual;

	_init_test();

	actual = tbm_bo_get_flags(NULL);

	ASSERT_EQ(actual, expected);
}


/* tbm_bufmgr_get_capability() */

TEST(tbm_bufmgr_get_capability, work_flow_success_3)
{
	unsigned int capability = TBM_BUFMGR_CAPABILITY_NONE;
	unsigned int expected_capability = TBM_BUFMGR_CAPABILITY_SHARE_FD;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;

	_init_test();

	bufmgr.backend = &backend;
	backend.bo_import = NULL;
	backend.bo_export = NULL;
	backend.bo_import_fd = ut_bo_import_fd;
	backend.bo_export_fd = ut_bo_export_fd;

	capability = tbm_bufmgr_get_capability(&bufmgr);

	ASSERT_EQ(capability, expected_capability);
}

TEST(tbm_bufmgr_get_capability, work_flow_success_2)
{
	unsigned int capability = TBM_BUFMGR_CAPABILITY_NONE;
	unsigned int expected_capability = TBM_BUFMGR_CAPABILITY_SHARE_KEY;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;

	_init_test();

	bufmgr.backend = &backend;
	backend.bo_import = ut_bo_import;
	backend.bo_export = ut_bo_export;
	backend.bo_import_fd = NULL;
	backend.bo_export_fd = NULL;

	capability = tbm_bufmgr_get_capability(&bufmgr);

	ASSERT_EQ(capability, expected_capability);
}

TEST(tbm_bufmgr_get_capability, work_flow_success_1)
{
	unsigned int capability = TBM_BUFMGR_CAPABILITY_SHARE_KEY;
	unsigned int expected_capability = TBM_BUFMGR_CAPABILITY_NONE;

	_init_test();

	capability = tbm_bufmgr_get_capability(NULL);

	ASSERT_EQ(capability, expected_capability);
}

/* tbm_get_last_error() */

TEST(tbm_get_last_error, work_flow_success_1)
{
	tbm_last_error = TBM_BO_ERROR_GET_FD_FAILED;
	tbm_error_e expected_error = tbm_last_error;

	_init_test();

	tbm_error_e error = tbm_get_last_error();

	ASSERT_EQ(error, expected_error);
}

/* tbm_bo_delete_user_data() */

TEST(tbm_bo_delete_user_data, work_flow_success_4)
{
	unsigned int key = 5;
	int expected = 1;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int expected_data;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	LIST_INITHEAD(&bo.user_data_list);
	tbm_user_data *user_data = calloc(1, sizeof(tbm_user_data));
	user_data->data = &expected_data;
	user_data->key = key;
	user_data->free_func = NULL;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data->item_link, &bo.user_data_list);
	FREE_TESTED_PTR = user_data;

	actual = tbm_bo_delete_user_data(&bo, key);

	ASSERT_EQ(actual, expected);
	ASSERT_EQ(free_called_for_tested_ptr, 1);
}

TEST(tbm_bo_delete_user_data, work_flow_success_3)
{
	unsigned int key = 5;
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int expected_data;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	LIST_INITHEAD(&bo.user_data_list);
	tbm_user_data user_data;
	user_data.data = &expected_data;
	user_data.key = key - 1;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data.item_link, &bo.user_data_list);

	actual = tbm_bo_delete_user_data(&bo, key);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_delete_user_data, work_flow_success_2)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	LIST_INITHEAD(&bo.user_data_list);

	actual = tbm_bo_delete_user_data(&bo, 1);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_delete_user_data, work_flow_success_1)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_delete_user_data(&bo, 1);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_delete_user_data, null_ptr_fail_1)
{
	int expected = 0;
	int actual;

	_init_test();

	actual = tbm_bo_delete_user_data(NULL, 1);

	ASSERT_EQ(actual, expected);
}

/* tbm_bo_get_user_data() */

TEST(tbm_bo_get_user_data, work_flow_success_5)
{
	unsigned int key = 5;
	void *data;
	int expected = 1;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int expected_data;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	LIST_INITHEAD(&bo.user_data_list);
	tbm_user_data user_data;
	user_data.data = &expected_data;
	user_data.key = key;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data.item_link, &bo.user_data_list);

	actual = tbm_bo_get_user_data(&bo, key, &data);

	ASSERT_EQ(actual, expected);
	ASSERT_TRUE(data == &expected_data);
}

TEST(tbm_bo_get_user_data, work_flow_success_4)
{
	unsigned int key = 5;
	void *data;
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int expected_data;
	tbm_user_data user_data;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	LIST_INITHEAD(&bo.user_data_list);
	user_data.data = &expected_data;
	user_data.key = key - 1;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data.item_link, &bo.user_data_list);

	actual = tbm_bo_get_user_data(&bo, key, &data);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_get_user_data, work_flow_success_3)
{
	void *data;
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	LIST_INITHEAD(&bo.user_data_list);

	actual = tbm_bo_get_user_data(&bo, 1, &data);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_get_user_data, work_flow_success_2)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_get_user_data(&bo, 1, NULL);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_get_user_data, work_flow_success_1)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_get_user_data(&bo, 1, NULL);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_get_user_data, null_ptr_fail_1)
{
	int expected = 0;
	int actual;

	_init_test();

	actual = tbm_bo_get_user_data(NULL, 1, NULL);

	ASSERT_EQ(actual, expected);
}

/* tbm_bo_set_user_data() */

TEST(tbm_bo_set_user_data, work_flow_success_3)
{
	unsigned int key = 5;
	int expected = 1;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int data, actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	tbm_user_data user_data;
	user_data.data = NULL;
	user_data.free_func = NULL;
	user_data.key = key;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data.item_link, &bo.user_data_list);

	actual = tbm_bo_set_user_data(&bo, key, &data);

	ASSERT_EQ(actual, expected);
	ASSERT_TRUE(user_data.data == &data);
}

TEST(tbm_bo_set_user_data, work_flow_success_2)
{
	unsigned int key = 5;
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	tbm_user_data user_data;
	user_data.key = key - 1;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data.item_link, &bo.user_data_list);

	actual = tbm_bo_set_user_data(&bo, key, NULL);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_set_user_data, work_flow_success_1)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_set_user_data(&bo, 1, NULL);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_set_user_data, null_ptr_fail_1)
{
	int expected = 0;
	int actual;

	_init_test();

	actual = tbm_bo_set_user_data(NULL, 1, NULL);

	ASSERT_EQ(actual, expected);
}

/* tbm_bo_add_user_data() */

TEST(tbm_bo_add_user_data, work_flow_success_4)
{
	tbm_user_data *data = NULL;
	unsigned long key = 5;
	int expected = 1;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	tbm_user_data user_data;
	user_data.key = key - 1;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data.item_link, &bo.user_data_list);

	actual = tbm_bo_add_user_data(&bo, key, ut_tbm_data_free);

	ASSERT_EQ(actual, expected);
	data = user_data_lookup(&bo.user_data_list, key);
	ASSERT_TRUE(data != NULL);
	tbm_user_data copy_data = *data;
	free(data);
	ASSERT_EQ(copy_data.key, key);
	ASSERT_TRUE(copy_data.free_func == ut_tbm_data_free);
	ASSERT_TRUE(copy_data.data == NULL);
}

TEST(tbm_bo_add_user_data, work_flow_success_3)
{
	unsigned long key = 5;
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	tbm_user_data user_data;
	user_data.key = key - 1;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data.item_link, &bo.user_data_list);
	CALLOC_ERROR = 1;

	actual = tbm_bo_add_user_data(&bo, key, NULL);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_add_user_data, work_flow_success_2)
{
	unsigned long key = 5;
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	tbm_user_data user_data;
	user_data.key = key;
	LIST_INITHEAD(&bo.user_data_list);
	LIST_ADD(&user_data.item_link, &bo.user_data_list);

	actual = tbm_bo_add_user_data(&bo, key, NULL);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_add_user_data, work_flow_success_1)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_add_user_data(&bo, 1, NULL);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_add_user_data, null_ptr_fail_1)
{
	int expected = 0;
	int actual;

	_init_test();

	actual = tbm_bo_add_user_data(NULL, 1, NULL);

	ASSERT_EQ(actual, expected);
}

/* tbm_bo_locked() */

TEST(tbm_bo_locked, work_flow_success_4)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.lock_type = LOCK_TRY_ONCE;
	bo.lock_cnt = 0;

	actual = tbm_bo_locked(&bo);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_locked, work_flow_success_3)
{
	int expected = 1;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.lock_type = LOCK_TRY_ONCE;
	bo.lock_cnt = 10;

	actual = tbm_bo_locked(&bo);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_locked, work_flow_success_2)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.lock_type = LOCK_TRY_NEVER;

	actual = tbm_bo_locked(&bo);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_locked, work_flow_success_1)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_locked(&bo);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_locked, null_ptr_fail_1)
{
	int expected = 0;
	int actual;

	_init_test();

	actual = tbm_bo_locked(NULL);

	ASSERT_EQ(actual, expected);
}

/* tbm_bo_swap() */

TEST(tbm_bo_swap, work_flow_success_4)
{
	int priv1 = 10;
	int priv2 = 20;
	int expected = 1;
	struct _tbm_bo bo1;
	struct _tbm_bo bo2;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr bufmgr2;
	struct _tbm_bufmgr_backend backend1;
	struct _tbm_bufmgr_backend backend2;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo1.item_link, &bufmgr.bo_list);
	LIST_ADD(&bo2.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bufmgr.backend = &backend1;
	bufmgr2.backend = &backend2;
	bo1.bufmgr = &bufmgr;
	bo2.bufmgr = &bufmgr2;
	backend1.bo_size = ut_bo_size;
	backend2.bo_size = ut_bo2_size;
	bo1.priv = &priv1;
	bo2.priv = &priv2;

	actual = tbm_bo_swap(&bo1, &bo2);

	ASSERT_EQ(actual, expected);
	ASSERT_TRUE(bo1.priv == &priv2);
	ASSERT_TRUE(bo2.priv == &priv1);
}

TEST(tbm_bo_swap, work_flow_success_3)
{
	tbm_error_e expected_last_error = TBM_BO_ERROR_SWAP_FAILED;
	int expected = 0;
	struct _tbm_bo bo1;
	struct _tbm_bo bo2;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr bufmgr2;
	struct _tbm_bufmgr_backend backend1;
	struct _tbm_bufmgr_backend backend2;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo1.item_link, &bufmgr.bo_list);
	LIST_ADD(&bo2.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bufmgr.backend = &backend1;
	bufmgr2.backend = &backend2;
	bo1.bufmgr = &bufmgr;
	bo2.bufmgr = &bufmgr2;
	backend1.bo_size = ut_bo_size;
	backend2.bo_size = ut_bo2_size;
	bo2_size = 200;

	actual = tbm_bo_swap(&bo1, &bo2);

	ASSERT_EQ(actual, expected);
	ASSERT_EQ(tbm_last_error, expected_last_error);
}

TEST(tbm_bo_swap, work_flow_success_2)
{
	int expected = 0;
	struct _tbm_bo bo1;
	struct _tbm_bo bo2;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo1.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_swap(&bo1, &bo2);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_swap, work_flow_success_1)
{
	int expected = 0;
	struct _tbm_bo bo1;
	struct _tbm_bo bo2;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo2.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_swap(&bo1, &bo2);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_swap, null_ptr_fail_2)
{
	int expected = 0;
	struct _tbm_bo bo1;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_swap(&bo1, NULL);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_swap, null_ptr_fail_1)
{
	int expected = 0;
	struct _tbm_bo bo2;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_swap(NULL, &bo2);

	ASSERT_EQ(actual, expected);
}

/* tbm_bo_unmap() */

TEST(tbm_bo_unmap_null, work_flow_success_3)
{
	int not_expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_unmap = ut_bo_unmap;
	bufmgr.lock_type = LOCK_TRY_NEVER;

	actual = tbm_bo_unmap(&bo);

	ASSERT_NE(actual, not_expected);
}

TEST(tbm_bo_unmap_null, work_flow_success_2)
{
	tbm_error_e expected_last_error = TBM_BO_ERROR_UNMAP_FAILED;
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_unmap = ut_bo_unmap;
	UT_TBM_ERROR = 1;

	actual = tbm_bo_unmap(&bo);

	ASSERT_EQ(actual, expected);
	ASSERT_EQ(tbm_last_error, expected_last_error);
}

TEST(tbm_bo_unmap_null, work_flow_success_1)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_unmap(&bo);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_unmap, null_ptr_fail_1)
{
	int expected = 0;
	int actual;

	_init_test();

	actual = tbm_bo_unmap(NULL);

	ASSERT_EQ(actual, expected);
}

/* tbm_bo_map() */

TEST(tbm_bo_map, work_flow_success_4)
{
	struct _tbm_bo bo;
	bo.map_cnt = 5;
	unsigned int expected_map_cnt = bo.map_cnt + 1;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	tbm_bo_handle handle;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bufmgr.lock_type = LOCK_TRY_NEVER;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_map = ut_bo_map;

	handle = tbm_bo_map(&bo, 1, 1);

	ASSERT_TRUE(handle.ptr != NULL);
	ASSERT_EQ(bo.map_cnt, expected_map_cnt);
}

TEST(tbm_bo_map, work_flow_success_3)
{
	tbm_error_e expected_last_error = TBM_BO_ERROR_MAP_FAILED;
	tbm_bo_handle expected_handle;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	tbm_bo_handle handle;

	_init_test();

	expected_handle.ptr = NULL;
	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bufmgr.lock_type = LOCK_TRY_NEVER;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_map = ut_bo_map;
	UT_TBM_ERROR = 1;

	handle = tbm_bo_map(&bo, 1, 1);

	ASSERT_TRUE(handle.ptr == expected_handle.ptr);
	ASSERT_EQ(tbm_last_error, expected_last_error);
}

TEST(tbm_bo_map, work_flow_success_2)
{
	tbm_error_e expected_last_error = TBM_BO_ERROR_LOCK_FAILED;
	tbm_bo_handle expected_handle;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	tbm_bo_handle handle;

	_init_test();

	expected_handle.ptr = NULL;
	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bufmgr.lock_type = LOCK_TRY_NEVER + 546;
	bo.bufmgr = &bufmgr;

	handle = tbm_bo_map(&bo, 1, 1);

	ASSERT_TRUE(handle.ptr == expected_handle.ptr);
	ASSERT_EQ(tbm_last_error, expected_last_error);
}

TEST(tbm_bo_map, work_flow_success_1)
{
	tbm_bo_handle expected_handle;
	expected_handle.ptr = NULL;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	tbm_bo_handle handle = tbm_bo_map(&bo, 1, 1);

	ASSERT_TRUE(handle.ptr == expected_handle.ptr);
}

TEST(tbm_bo_map, null_ptr_fail_1)
{
	tbm_bo_handle expected_handle;
	expected_handle.ptr = NULL;
	tbm_bo_handle handle;

	_init_test();

	handle = tbm_bo_map(NULL, 1, 1);

	ASSERT_TRUE(handle.ptr == expected_handle.ptr);
}

/* tbm_bo_get_handle() */

TEST(tbm_bo_get_handle, work_flow_success_3)
{
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	tbm_bo_handle handle;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_get_handle = ut_bo_get_handle;

	handle = tbm_bo_get_handle(&bo, 1);

	ASSERT_TRUE(handle.ptr != NULL);
}

TEST(tbm_bo_get_handle, work_flow_success_2)
{
	tbm_bo_handle expected_handle;
	expected_handle.ptr = NULL;
	tbm_error_e expected_last_result = TBM_BO_ERROR_GET_HANDLE_FAILED;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_get_handle = ut_bo_get_handle;
	UT_TBM_ERROR = 1;

	tbm_bo_handle handle = tbm_bo_get_handle(&bo, 1);

	ASSERT_TRUE(handle.ptr == expected_handle.ptr);
	ASSERT_EQ(tbm_last_error, expected_last_result);
}

TEST(tbm_bo_get_handle, work_flow_success_1)
{
	tbm_bo_handle expected_handle;
	expected_handle.ptr = NULL;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	tbm_bo_handle handle;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	handle = tbm_bo_get_handle(&bo, 1);

	ASSERT_TRUE(handle.ptr == expected_handle.ptr);
}

TEST(tbm_bo_get_handle, null_ptr_fail_1)
{
	tbm_bo_handle expected_handle;
	expected_handle.ptr = NULL;
	tbm_bo_handle handle;

	_init_test();

	handle = tbm_bo_get_handle(NULL, 1);

	ASSERT_TRUE(handle.ptr == expected_handle.ptr);
}

/* tbm_bo_export_fd() */

TEST(tbm_bo_export_fd, work_flow_success_3)
{
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int key;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_export_fd = ut_bo_export_fd;

	key = tbm_bo_export_fd(&bo);

	ASSERT_GE(key, 0);
}

TEST(tbm_bo_export_fd, work_flow_success_2)
{
	tbm_error_e expected_last_result = TBM_BO_ERROR_EXPORT_FD_FAILED;
	int expected_key = -1;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int key;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_export_fd = ut_bo_export_fd;
	UT_TBM_ERROR = 1;

	key = tbm_bo_export_fd(&bo);

	ASSERT_EQ(key, expected_key);
	ASSERT_EQ(tbm_last_error, expected_last_result);
}

TEST(tbm_bo_export_fd, work_flow_success_1)
{
	int expected_key = -1;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int key;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	key = tbm_bo_export_fd(&bo);

	ASSERT_EQ(key, expected_key);
}

TEST(tbm_bo_export_fd, null_ptr_fail_1)
{
	int expected_key = -1;
	int key;

	_init_test();

	key = tbm_bo_export_fd(NULL);

	ASSERT_EQ(key, expected_key);
}

/* tbm_bo_export() */

TEST(tbm_bo_export, work_flow_success_3)
{
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int key;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_export = ut_bo_export;

	key = tbm_bo_export(&bo);

	ASSERT_GT(key, 0);
}

TEST(tbm_bo_export, work_flow_success_2)
{
	tbm_error_e expected_last_result = TBM_BO_ERROR_EXPORT_FAILED;
	int expected_key = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int key;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.bo_export = ut_bo_export;
	UT_TBM_ERROR = 1;

	key = tbm_bo_export(&bo);

	ASSERT_EQ(key, expected_key);
	ASSERT_EQ(tbm_last_error, expected_last_result);
}

TEST(tbm_bo_export, work_flow_success_1)
{
	int expected_key = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int key;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	key = tbm_bo_export(&bo);

	ASSERT_EQ(key, expected_key);
}

TEST(tbm_bo_export, null_ptr_fail_1)
{
	int expected_key = 0;
	int key;

	_init_test();

	key = tbm_bo_export(NULL);

	ASSERT_EQ(key, expected_key);
}

/* tbm_bo_import_fd() */

TEST(tbm_bo_import_fd, work_flow_success_5)
{
	int expected_flags = bo_ret_flags;
	int expected_ref_cnt = 1;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo *actual_bo;
	struct _tbm_bo copy_bo;

	_init_test();

	bufmgr.backend = &backend;
	bufmgr.bo_cnt = 1;
	backend.bo_import_fd = ut_bo_import_fd;
	LIST_INITHEAD(&bufmgr.bo_list);
	backend.bo_get_flags = ut_bo_get_flags;

	actual_bo = tbm_bo_import_fd(&bufmgr, 1);

	ASSERT_TRUE(actual_bo != NULL);
	copy_bo = *actual_bo;
	free(actual_bo);
	ASSERT_EQ(copy_bo.ref_cnt, expected_ref_cnt);
	ASSERT_EQ(copy_bo.flags, expected_flags);
	ASSERT_TRUE(bufmgr.bo_list.next != &bufmgr.bo_list);
	ASSERT_TRUE(bufmgr.bo_list.prev != &bufmgr.bo_list);
}

TEST(tbm_bo_import_fd, work_flow_success_4)
{
	int expected_flags = TBM_BO_DEFAULT;
	int expected_ref_cnt = 1;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo *actual_bo;
	struct _tbm_bo copy_bo;

	_init_test();

	bufmgr.backend = &backend;
	bufmgr.bo_cnt = 1;
	backend.bo_import_fd = ut_bo_import_fd;
	LIST_INITHEAD(&bufmgr.bo_list);
	backend.bo_get_flags = NULL;

	actual_bo = tbm_bo_import_fd(&bufmgr, 1);

	ASSERT_TRUE(actual_bo != NULL);
	copy_bo = *actual_bo;
	free(actual_bo);
	ASSERT_EQ(copy_bo.ref_cnt, expected_ref_cnt);
	ASSERT_EQ(copy_bo.flags, expected_flags);
	ASSERT_TRUE(bufmgr.bo_list.next != &bufmgr.bo_list);
	ASSERT_TRUE(bufmgr.bo_list.prev != &bufmgr.bo_list);
}

TEST(tbm_bo_import_fd, work_flow_success_3)
{
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo expected_bo;
	int expected_ref_cnt;

	_init_test();

	bufmgr.backend = &backend;
	bufmgr.bo_cnt = 1;
	backend.bo_import_fd = ut_bo_import_fd;
	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&expected_bo.item_link, &bufmgr.bo_list);
	expected_bo.priv = ret_bo;
	expected_bo.ref_cnt = 10;
	expected_ref_cnt = expected_bo.ref_cnt + 1;

	struct _tbm_bo *actual_bo = tbm_bo_import_fd(&bufmgr, 1);

	ASSERT_TRUE(actual_bo == &expected_bo);
	ASSERT_EQ(actual_bo->ref_cnt, expected_ref_cnt);
}

TEST(tbm_bo_import_fd, work_flow_success_2)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo *actual;

	_init_test();

	bufmgr.backend = &backend;
	bufmgr.bo_cnt = 1;
	backend.bo_import_fd = ut_bo_import_fd;
	TBM_BO_IMPORT_ERROR = 1;

	actual = tbm_bo_import_fd(&bufmgr, 1);

	ASSERT_TRUE(actual == expected);
}

TEST(tbm_bo_import_fd, work_flow_success_1)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bo *actual;

	_init_test();

	CALLOC_ERROR = 1;

	actual = tbm_bo_import_fd(&bufmgr, 1);

	ASSERT_TRUE(actual == expected);
}

TEST(tbm_bo_import_fd, null_ptr_fail_1)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bo *actual;

	_init_test();

	actual = tbm_bo_import_fd(NULL, 1);

	ASSERT_TRUE(actual == expected);
}

/* tbm_bo_import() */

TEST(tbm_bo_import, work_flow_success_5)
{
	int expected_flags = bo_ret_flags;
	int expected_ref_cnt = 1;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo *actual_bo;
	struct _tbm_bo copy_bo;

	_init_test();

	bufmgr.backend = &backend;
	bufmgr.bo_cnt = 1;
	backend.bo_import = ut_bo_import;
	LIST_INITHEAD(&bufmgr.bo_list);
	backend.bo_get_flags = ut_bo_get_flags;

	actual_bo = tbm_bo_import(&bufmgr, 1);

	ASSERT_TRUE(actual_bo != NULL);
	copy_bo = *actual_bo;
	free(actual_bo);
	ASSERT_EQ(copy_bo.ref_cnt, expected_ref_cnt);
	ASSERT_EQ(copy_bo.flags, expected_flags);
	ASSERT_TRUE(bufmgr.bo_list.next != &bufmgr.bo_list);
	ASSERT_TRUE(bufmgr.bo_list.prev != &bufmgr.bo_list);
}

TEST(tbm_bo_import, work_flow_success_4)
{
	int expected_flags = TBM_BO_DEFAULT;
	int expected_ref_cnt = 1;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo *actual_bo;
	struct _tbm_bo copy_bo;

	_init_test();

	bufmgr.backend = &backend;
	bufmgr.bo_cnt = 1;
	backend.bo_import = ut_bo_import;
	LIST_INITHEAD(&bufmgr.bo_list);
	backend.bo_get_flags = NULL;

	actual_bo = tbm_bo_import(&bufmgr, 1);

	ASSERT_TRUE(actual_bo != NULL);
	copy_bo = *actual_bo;
	free(actual_bo);
	ASSERT_EQ(copy_bo.ref_cnt, expected_ref_cnt);
	ASSERT_EQ(copy_bo.flags, expected_flags);
	ASSERT_TRUE(bufmgr.bo_list.next != &bufmgr.bo_list);
	ASSERT_TRUE(bufmgr.bo_list.prev != &bufmgr.bo_list);
}

TEST(tbm_bo_import, work_flow_success_3)
{
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo expected_bo;
	int expected_ref_cnt;
	struct _tbm_bo *actual_bo;

	_init_test();

	bufmgr.backend = &backend;
	bufmgr.bo_cnt = 1;
	backend.bo_import = ut_bo_import;
	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&expected_bo.item_link, &bufmgr.bo_list);
	expected_bo.priv = ret_bo;
	expected_bo.ref_cnt = 10;
	expected_ref_cnt = expected_bo.ref_cnt + 1;

	actual_bo = tbm_bo_import(&bufmgr, 1);

	ASSERT_TRUE(actual_bo == &expected_bo);
	ASSERT_EQ(actual_bo->ref_cnt, expected_ref_cnt);
}

TEST(tbm_bo_import, work_flow_success_2)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo *actual;

	_init_test();

	bufmgr.backend = &backend;
	bufmgr.bo_cnt = 1;
	backend.bo_import = ut_bo_import;
	TBM_BO_IMPORT_ERROR = 1;

	actual = tbm_bo_import(&bufmgr, 1);

	ASSERT_TRUE(actual == expected);
}

TEST(tbm_bo_import, work_flow_success_1)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bo *actual;

	_init_test();

	CALLOC_ERROR = 1;

	actual = tbm_bo_import(&bufmgr, 1);

	ASSERT_TRUE(actual == expected);
}

TEST(tbm_bo_import, null_ptr_fail_1)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bo *actual;

	_init_test();

	actual = tbm_bo_import(NULL, 1);

	ASSERT_TRUE(actual == expected);
}

/* tbm_bo_alloc() */

TEST(tbm_bo_alloc, work_flow_success_3)
{
	int flags = 6;
	int expected_ref_cnt = 1;
	struct _tbm_bo *not_expected_bo = NULL;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo *actual_bo;
	struct _tbm_bo copy_bo;

	_init_test();

	bufmgr.backend = &backend;
	backend.bo_alloc = ut_bo_alloc;
	LIST_INITHEAD(&bufmgr.bo_list);

	actual_bo = tbm_bo_alloc(&bufmgr, 1, flags);

	ASSERT_TRUE(actual_bo != not_expected_bo);
	copy_bo = *actual_bo;
	free(actual_bo);
	ASSERT_EQ(copy_bo.flags, flags);
	ASSERT_EQ(copy_bo.ref_cnt, expected_ref_cnt);
	ASSERT_TRUE(copy_bo.priv == ret_bo);
	ASSERT_TRUE(bufmgr.bo_list.next != &bufmgr.bo_list);
	ASSERT_TRUE(bufmgr.bo_list.prev != &bufmgr.bo_list);
}

TEST(tbm_bo_alloc, work_flow_success_2)
{
	struct _tbm_bo *expected = NULL;
	tbm_error_e expected_last_error = TBM_BO_ERROR_BO_ALLOC_FAILED;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	struct _tbm_bo *actual;

	_init_test();

	bufmgr.backend = &backend;
	backend.bo_alloc = ut_bo_alloc;
	TBM_BO_ALLOC_ERROR = 1;

	actual = tbm_bo_alloc(&bufmgr, 1, 1);

	ASSERT_TRUE(actual == expected);
	ASSERT_EQ(tbm_last_error, expected_last_error);
}

TEST(tbm_bo_alloc, work_flow_success_1)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bufmgr bufmgr;
	tbm_error_e expected_last_error;
	struct _tbm_bo *actual;

	_init_test();

	CALLOC_ERROR = 1;
	expected_last_error = TBM_BO_ERROR_HEAP_ALLOC_FAILED;

	actual = tbm_bo_alloc(&bufmgr, 1, 1);

	ASSERT_TRUE(actual == expected);
	ASSERT_EQ(tbm_last_error, expected_last_error);
}

TEST(tbm_bo_alloc, null_int_fail_1)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bo *actual;

	_init_test();

	actual = tbm_bo_alloc(&bufmgr, 0, 1);

	ASSERT_TRUE(actual == expected);
}

TEST(tbm_bo_alloc, null_ptr_fail_1)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bo *actual;

	_init_test();

	actual = tbm_bo_alloc(NULL, 1, 1);

	ASSERT_TRUE(actual == expected);
}

/* tbm_bo_unref() */

TEST(tbm_bo_unref, work_flow_success_1)
{
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int expected_ref_cnt;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.ref_cnt = 10;
	expected_ref_cnt = bo.ref_cnt - 1;

	tbm_bo_unref(&bo);

	ASSERT_EQ(bo.ref_cnt, expected_ref_cnt);
}

/* tbm_bo_ref() */

TEST(tbm_bo_ref, work_flow_success_3)
{
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bo *expected;
	struct _tbm_bo *actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	bo.ref_cnt = 1;
	int expected_ref_cnt = bo.ref_cnt + 1;
	expected = &bo;

	actual = tbm_bo_ref(&bo);

	ASSERT_TRUE(actual == expected);
	ASSERT_EQ(bo.ref_cnt, expected_ref_cnt);
}

TEST(tbm_bo_ref, work_flow_success_2)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bo *actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_ref(&bo);

	ASSERT_TRUE(actual == expected);
}

TEST(tbm_bo_ref, work_flow_success_1)
{
	struct _tbm_bo *expected = NULL;
	struct _tbm_bo *actual;

	_init_test();

	actual = tbm_bo_ref(NULL);

	ASSERT_TRUE(actual == expected);
}

/* tbm_bo_size() */

TEST(tbm_bo_size, work_flow_success_3)
{
	int not_expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	struct _tbm_bufmgr_backend backend;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	LIST_ADD(&bo.item_link, &bufmgr.bo_list);
	gBufMgr = &bufmgr;
	backend.bo_size = ut_bo_size;
	bufmgr.backend = &backend;
	bo.bufmgr = &bufmgr;

	actual = tbm_bo_size(&bo);

	ASSERT_TRUE(actual != not_expected);
}

TEST(tbm_bo_size, work_flow_success_2)
{
	int expected = 0;
	struct _tbm_bo bo;
	struct _tbm_bufmgr bufmgr;
	int actual;

	_init_test();

	LIST_INITHEAD(&bufmgr.bo_list);
	gBufMgr = &bufmgr;

	actual = tbm_bo_size(&bo);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_bo_size, work_flow_success_1)
{
	int expected = 0;
	int actual ;

	_init_test();

	actual = tbm_bo_size(NULL);

	ASSERT_EQ(actual, expected);
}
