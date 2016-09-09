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

/* HELPER FUNCTIONS */

#include <stdint.h>
#include "tbm_type.h"
#include "tbm_surface.h"
#include "tbm_bufmgr_int.h"

static int TBM_SURFACE_ERROR = 0;
static int TBM_SURFACE_VALID_ERROR = 0;
struct _tbm_surface ut_ret_surface;
static int ut_ret_width = 200;
static int ut_ret_height = 100;
static int ut_ret_format = 10;

static int
ut_tbm_surface_internal_query_supported_formats(uint32_t **formats, uint32_t *num)
{
	if (TBM_SURFACE_ERROR) {
		return 0;
	}

	return 1;
}

static tbm_surface_h
ut_tbm_surface_internal_create_with_flags(int width, int height,
					   int format, int flags)
{
	if (TBM_SURFACE_ERROR) {
		return NULL;
	}

	return &ut_ret_surface;
}

static int
ut_tbm_surface_is_valid(tbm_surface_h surface)
{
	if (TBM_SURFACE_VALID_ERROR) {
		return 0;
	}

	return 1;
}

static void
ut_tbm_surface_internal_destroy(tbm_surface_h surface)
{
}

static int
ut_tbm_surface_internal_get_info(tbm_surface_h surface, int opt,
				  tbm_surface_info_s *info, int map)
{
	if (TBM_SURFACE_ERROR) {
		return 0;
	}

	return 1;
}

static void
ut_tbm_surface_internal_unmap(tbm_surface_h surface)
{
}

static unsigned int
ut_tbm_surface_internal_get_width(tbm_surface_h surface)
{
	return ut_ret_width;
}

unsigned int
ut_tbm_surface_internal_get_height(tbm_surface_h surface)
{
	return ut_ret_height;
}

static tbm_format
ut_tbm_surface_internal_get_format(tbm_surface_h surface)
{
	return ut_ret_format;
}

#define pthread_mutex_lock ut_pthread_mutex_lock
#define pthread_mutex_unlock ut_pthread_mutex_unlock
#define pthread_mutex_init ut_pthread_mutex_init
#define calloc ut_calloc
#define free ut_free
#define tbm_surface_internal_query_supported_formats \
	ut_tbm_surface_internal_query_supported_formats
#define tbm_surface_internal_create_with_flags \
	ut_tbm_surface_internal_create_with_flags
#define tbm_surface_internal_is_valid ut_tbm_surface_is_valid
#define tbm_surface_internal_destroy ut_tbm_surface_internal_destroy
#define tbm_surface_internal_get_info ut_tbm_surface_internal_get_info
#define tbm_surface_internal_unmap ut_tbm_surface_internal_unmap
#define tbm_surface_internal_get_width ut_tbm_surface_internal_get_width
#define tbm_surface_internal_get_height ut_tbm_surface_internal_get_height
#define tbm_surface_internal_get_format ut_tbm_surface_internal_get_format

#include "tbm_surface.c"

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
	TBM_SURFACE_ERROR = 0;
	TBM_SURFACE_VALID_ERROR = 0;
}

/* tbm_surface_get_format */

TEST(tbm_surface_get_format, work_flow_success_2)
{
	tbm_format format = 0;
	struct _tbm_surface surface;

	_init_test();

	format = tbm_surface_get_format(&surface);

	ASSERT_EQ(format, ut_ret_format);
}

TEST(tbm_surface_get_format, work_flow_success_1)
{
	tbm_format format = 0;
	tbm_format expected_format = 0;
	struct _tbm_surface surface;

	_init_test();

	TBM_SURFACE_VALID_ERROR = 1;

	format = tbm_surface_get_format(&surface);

	ASSERT_EQ(format, expected_format);
}

/* tbm_surface_get_height */

TEST(tbm_surface_get_height, work_flow_success_1)
{
	int height = 0;
	struct _tbm_surface surface;

	_init_test();

	height = tbm_surface_get_height(&surface);

	ASSERT_EQ(height, ut_ret_height);
}

TEST(tbm_surface_get_height, null_ptr_fail_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_PARAMETER;

	_init_test();

	TBM_SURFACE_VALID_ERROR = 1;

	error = tbm_surface_get_height(NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_get_width() */

TEST(tbm_surface_get_width, work_flow_success_1)
{
	int width = 0;
	struct _tbm_surface surface;

	_init_test();

	width = tbm_surface_get_width(&surface);

	ASSERT_EQ(width, ut_ret_width);
}

TEST(tbm_surface_get_width, null_ptr_fail_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_PARAMETER;

	_init_test();

	TBM_SURFACE_VALID_ERROR = 1;

	error = tbm_surface_get_width(NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_get_info() */

TEST(tbm_surface_get_info, work_flow_success_2)
{
	int error = TBM_SURFACE_ERROR_INVALID_OPERATION;
	int expected_error = TBM_SURFACE_ERROR_NONE;
	struct _tbm_surface surface;
	tbm_surface_info_s info;

	_init_test();

	error = tbm_surface_get_info(&surface, &info);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_get_info, work_flow_success_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_OPERATION;
	struct _tbm_surface surface;
	tbm_surface_info_s info;

	_init_test();

	TBM_SURFACE_ERROR = 1;

	error = tbm_surface_get_info(&surface, &info);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_get_info, null_ptr_fail_2)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_PARAMETER;
	struct _tbm_surface surface;

	_init_test();

	error = tbm_surface_get_info(&surface, NULL);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_get_info, null_ptr_fail_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_PARAMETER;
	tbm_surface_info_s info;

	_init_test();

	TBM_SURFACE_VALID_ERROR = 1;

	error = tbm_surface_get_info(NULL, &info);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_unmap() */

TEST(tbm_surface_unmap, work_flow_success_1)
{
	int error = TBM_SURFACE_ERROR_INVALID_PARAMETER;
	int expected_error = TBM_SURFACE_ERROR_NONE;
	struct _tbm_surface surface;

	_init_test();

	error = tbm_surface_unmap(&surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_unmap, null_ptr_fail_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_PARAMETER;

	_init_test();

	TBM_SURFACE_VALID_ERROR = 1;

	error = tbm_surface_unmap(NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_map() */

TEST(tbm_surface_map, work_flow_success_2)
{
	int error = TBM_SURFACE_ERROR_INVALID_OPERATION;
	int expected_error = TBM_SURFACE_ERROR_NONE;
	struct _tbm_surface surface;
	tbm_surface_info_s info;

	_init_test();

	error = tbm_surface_map(&surface, 1, &info);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_map, work_flow_success_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_OPERATION;
	struct _tbm_surface surface;
	tbm_surface_info_s info;

	_init_test();

	TBM_SURFACE_ERROR = 1;

	error = tbm_surface_map(&surface, 1, &info);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_map, null_ptr_fail_2)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_PARAMETER;
	struct _tbm_surface surface;

	_init_test();

	error = tbm_surface_map(&surface, 1, NULL);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_map, null_ptr_fail_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_PARAMETER;
	tbm_surface_info_s info;

	_init_test();

	TBM_SURFACE_VALID_ERROR = 1;

	error = tbm_surface_map(NULL, 1, &info);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_destroy() */

TEST(tbm_surface_destroy, work_flow_success_2)
{
	int error = TBM_SURFACE_ERROR_INVALID_PARAMETER;
	int expected_error = TBM_SURFACE_ERROR_NONE;
	struct _tbm_surface surface;

	_init_test();

	error = tbm_surface_destroy(&surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_destroy, work_flow_success_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_PARAMETER;
	struct _tbm_surface surface;

	_init_test();

	TBM_SURFACE_VALID_ERROR = 1;

	error = tbm_surface_destroy(&surface);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_create() */

TEST(tbm_surface_create, work_flow_success_4)
{
	tbm_surface_h actual = (tbm_surface_h) 1;
	tbm_surface_h not_expected = NULL;
	int width = 10;
	int height = 10;
	tbm_format format = 1;

	_init_test();

	actual = tbm_surface_create(width, height, format);

	ASSERT_TRUE(actual != not_expected);
}

TEST(tbm_surface_create, work_flow_success_3)
{
	tbm_surface_h actual = (tbm_surface_h) 1;
	tbm_surface_h expected = NULL;
	int width = 10;
	int height = 10;
	tbm_format format = 1;

	_init_test();

	TBM_SURFACE_ERROR = 1;

	actual = tbm_surface_create(width, height, format);

	ASSERT_TRUE(actual == expected);
}

TEST(tbm_surface_create, work_flow_success_2)
{
	tbm_surface_h actual = (tbm_surface_h) 1;
	tbm_surface_h expected = NULL;
	int width = 10;
	int height = -10;
	tbm_format format = 1;

	_init_test();

	actual = tbm_surface_create(width, height, format);

	ASSERT_TRUE(actual == expected);
}

TEST(tbm_surface_create, work_flow_success_1)
{
	tbm_surface_h actual = (tbm_surface_h) 1;
	tbm_surface_h expected = NULL;
	int width = -10;
	int height = 10;
	tbm_format format = 1;

	_init_test();

	actual = tbm_surface_create(width, height, format);

	ASSERT_TRUE(actual == expected);
}

/* tbm_surface_query_formats() */

TEST(tbm_surface_query_formats, work_flow_success_2)
{
	int error = TBM_SURFACE_ERROR_INVALID_OPERATION;
	int expected_error = TBM_SURFACE_ERROR_NONE;
	uint32_t *formats, num;

	_init_test();

	error = tbm_surface_query_formats(&formats, &num);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_query_formats, work_flow_success_1)
{
	int error = TBM_SURFACE_ERROR_NONE;
	int expected_error = TBM_SURFACE_ERROR_INVALID_OPERATION;
	uint32_t *formats, num;

	_init_test();

	TBM_SURFACE_ERROR = 1;

	error = tbm_surface_query_formats(&formats, &num);

	ASSERT_EQ(error, expected_error);
}
