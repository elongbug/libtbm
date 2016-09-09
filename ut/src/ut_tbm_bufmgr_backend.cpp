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

#include "pthread_stubs.h"
#include "stdlib_stubs.h"

typedef struct _tbm_bufmgr_backend tbm_bufmgr_backend_s;
typedef struct _tbm_bufmgr tbm_bufmgr_s;
typedef struct _tbm_bo tbm_bo_s;

#define pthread_mutex_lock ut_pthread_mutex_lock
#define pthread_mutex_unlock ut_pthread_mutex_unlock
#define pthread_mutex_init ut_pthread_mutex_init
#define calloc ut_calloc
#define free ut_free
#define getenv ut_getenv

#include "tbm_bufmgr_backend.c"

static void _init_test()
{
	PTHREAD_MUTEX_INIT_ERROR = 0;
	CALLOC_ERROR = 0;
	FREE_CALLED = 0;
	FREE_PTR = NULL;
	FREE_TESTED_PTR = NULL;
	free_called_for_tested_ptr = 0;
	free_call_count = 0;
	GETENV_ERROR = 0;
}

/* tbm_backend_is_display_server() */

TEST(tbm_backend_is_display_server, work_flow_success_2)
{
	int error = 0;
	int expected_error = 1;

	_init_test();

	error = tbm_backend_is_display_server();

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_backend_is_display_server, work_flow_success_1)
{
	int error = 1;
	int expected_error = 0;

	_init_test();

	GETENV_ERROR = 1;

	error = tbm_backend_is_display_server();

	ASSERT_EQ(error, expected_error);
}

/* tbm_backend_get_bo_priv() */

TEST(tbm_backend_get_bo_priv, work_flow_success_1)
{
	int priv;
	void *actual_priv = NULL;
	void *expected_priv = &priv;
	tbm_bo_s bo;

	_init_test();

	bo.priv = &priv;

	actual_priv = tbm_backend_get_bo_priv(&bo);

	ASSERT_TRUE(actual_priv == expected_priv);
}

/* tbm_backend_set_bo_priv() */

TEST(tbm_backend_set_bo_priv, work_flow_success_1)
{
	int priv;
	tbm_bo_s bo;

	_init_test();

	tbm_backend_set_bo_priv(&bo, &priv);

	ASSERT_TRUE(bo.priv == &priv);
}

/* tbm_backend_get_priv_from_bufmgr() */

TEST(tbm_backend_get_priv_from_bufmgr, work_flow_success_1)
{
	int priv;
	void *actual_priv = NULL;
	void *expected_priv = &priv;
	tbm_bufmgr_s bufmgr;
	tbm_bufmgr_backend_s backend;

	_init_test();

	bufmgr.backend = &backend;
	backend.priv = &priv;

	actual_priv = tbm_backend_get_priv_from_bufmgr(&bufmgr);

	ASSERT_TRUE(actual_priv == expected_priv);
}

/* tbm_backend_get_bufmgr_priv() */

TEST(tbm_backend_get_bufmgr_priv, work_flow_success_1)
{
	int priv;
	void *actual_priv = NULL;
	void *expected_priv = &priv;
	tbm_bo_s bo;
	tbm_bufmgr_s bufmgr;
	tbm_bufmgr_backend_s backend;

	_init_test();

	bo.bufmgr = &bufmgr;
	bufmgr.backend = &backend;
	backend.priv = &priv;

	actual_priv = tbm_backend_get_bufmgr_priv(&bo);

	ASSERT_TRUE(actual_priv == expected_priv);
}

/* tbm_backend_init() */

TEST(tbm_backend_init, work_flow_success_1)
{
	int error = 0;
	int expected_error = 1;
	tbm_bufmgr_s bufmgr;
	tbm_bufmgr_backend_s backend;

	_init_test();

	error = tbm_backend_init(&bufmgr, &backend);

	ASSERT_EQ(error, expected_error);
	ASSERT_TRUE(bufmgr.backend == &backend);
}

TEST(tbm_backend_init, null_ptr_fail_2)
{
	int error = 1;
	int expected_error = 0;
	tbm_bufmgr_s bufmgr;

	_init_test();

	error = tbm_backend_init(&bufmgr, NULL);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_backend_init, null_ptr_fail_1)
{
	int error = 1;
	int expected_error = 0;
	tbm_bufmgr_backend_s backend;

	_init_test();

	error = tbm_backend_init(NULL, &backend);

	ASSERT_EQ(error, expected_error);
}

/* tbm_backend_free() */

TEST(tbm_backend_free, work_flow_success_1)
{
	tbm_bufmgr_backend backend = calloc(1, sizeof(*backend));

	_init_test();

	FREE_TESTED_PTR = backend;

	tbm_backend_free(backend);

	ASSERT_EQ(free_called_for_tested_ptr, 1);
}

/* tbm_backend_alloc() */

TEST(tbm_backend_alloc, work_flow_success_2)
{
	tbm_bufmgr_backend backend = NULL;
	tbm_bufmgr_backend not_expected_backend = NULL;

	_init_test();

	backend = tbm_backend_alloc();

	ASSERT_TRUE(backend != not_expected_backend);
	free(backend);
}

TEST(tbm_backend_alloc, work_flow_success_1)
{
	tbm_bufmgr_backend backend = (tbm_bufmgr_backend) 1;
	tbm_bufmgr_backend expected_backend = NULL;

	_init_test();

	CALLOC_ERROR = 1;

	backend = tbm_backend_alloc();

	ASSERT_TRUE(backend == expected_backend);
}
