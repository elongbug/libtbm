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

typedef struct _tbm_surface_queue tbm_surface_queue_s;
typedef struct _tbm_surface tbm_surface_s;

#include "pthread_stubs.h"
#include "stdlib_stubs.h"

#define pthread_mutex_lock ut_pthread_mutex_lock
#define pthread_mutex_unlock ut_pthread_mutex_unlock
#define pthread_mutex_init ut_pthread_mutex_init
#define pthread_cond_signal ut_pthread_cond_signal
#define pthread_cond_wait ut_pthread_cond_wait
#define calloc ut_calloc
#define free ut_free

#include "tbm_surface_queue.c"

/* HELPER FUNCTIONS */

static void ut_tbm_surface_queue_notify_cb(tbm_surface_queue_h surface_queue,
										   void *data) {}

static tbm_surface_h ut_tbm_surface_alloc_cb(tbm_surface_queue_h surface_queue,
											 void *data) {}

static void ut_tbm_surface_free_cb(tbm_surface_queue_h surface_queue,
								   void *data, tbm_surface_h surface) {}

static void ut_enqueue(tbm_surface_queue_h queue, queue_node *node) {}

static queue_node *ut_dequeue(tbm_surface_queue_h queue)
{
	return NULL;
}

static void ut_release(tbm_surface_queue_h queue, queue_node *node) {}

static queue_node *ut_acquire(tbm_surface_queue_h queue)
{
	return NULL;
}

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

/* tbm_surface_queue_sequence_create */

TEST(tbm_surface_queue_sequence_create, work_flow_success_6)
{
	tbm_surface_queue_h surface_queue = NULL;
	int queue_size = 1000;
	int width = 200;
	int height = 100;
	int format = 50;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_sequence_create(queue_size, width, height,
											 format, flags);

	ASSERT_NE(surface_queue, NULL);
	tbm_surface_queue_s copy_surface_queue = *surface_queue;
	tbm_queue_default data = *((tbm_queue_default *) surface_queue->impl_data);
	free(surface_queue->impl_data);
	free(surface_queue);
	ASSERT_EQ(queue_size, data.queue_size);
	ASSERT_EQ(flags, data.flags);
	ASSERT_EQ(queue_size, copy_surface_queue.queue_size);
	ASSERT_EQ(width, copy_surface_queue.width);
	ASSERT_EQ(height, copy_surface_queue.height);
	ASSERT_EQ(format, copy_surface_queue.format);
}

TEST(tbm_surface_queue_sequence_create, work_flow_success_5)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 1000;
	int width = 200;
	int height = 100;
	int format = 50;
	int flags = 10;

	_init_test();

	CALLOC_ERROR = 1;

	surface_queue = tbm_surface_queue_sequence_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

TEST(tbm_surface_queue_sequence_create, work_flow_success_4)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 1000;
	int width = 200;
	int height = 100;
	int format = 0;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_sequence_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

TEST(tbm_surface_queue_sequence_create, work_flow_success_3)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 1000;
	int width = 200;
	int height = 0;
	int format = 50;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_sequence_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

TEST(tbm_surface_queue_sequence_create, work_flow_success_2)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 1000;
	int width = 0;
	int height = 100;
	int format = 50;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_sequence_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

TEST(tbm_surface_queue_sequence_create, work_flow_success_1)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 0;
	int width = 200;
	int height = 100;
	int format = 50;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_sequence_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

/* tbm_surface_queue_create() */

TEST(tbm_surface_queue_create, work_flow_success_6)
{
	tbm_surface_queue_h surface_queue = NULL;
	int queue_size = 1000;
	int width = 200;
	int height = 100;
	int format = 50;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_create(queue_size, width, height,
											 format, flags);

	ASSERT_NE(surface_queue, NULL);
	tbm_surface_queue_s copy_surface_queue = *surface_queue;
	tbm_queue_default data = *((tbm_queue_default *) surface_queue->impl_data);
	free(surface_queue->impl_data);
	free(surface_queue);
	ASSERT_EQ(queue_size, data.queue_size);
	ASSERT_EQ(flags, data.flags);
	ASSERT_EQ(queue_size, copy_surface_queue.queue_size);
	ASSERT_EQ(width, copy_surface_queue.width);
	ASSERT_EQ(height, copy_surface_queue.height);
	ASSERT_EQ(format, copy_surface_queue.format);
}

TEST(tbm_surface_queue_create, work_flow_success_5)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 1000;
	int width = 200;
	int height = 100;
	int format = 50;
	int flags = 10;

	_init_test();

	CALLOC_ERROR = 1;

	surface_queue = tbm_surface_queue_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

TEST(tbm_surface_queue_create, work_flow_success_4)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 1000;
	int width = 200;
	int height = 100;
	int format = 0;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

TEST(tbm_surface_queue_create, work_flow_success_3)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 1000;
	int width = 200;
	int height = 0;
	int format = 50;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

TEST(tbm_surface_queue_create, work_flow_success_2)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 1000;
	int width = 0;
	int height = 100;
	int format = 50;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

TEST(tbm_surface_queue_create, work_flow_success_1)
{
	tbm_surface_queue_h surface_queue = NULL;
	tbm_surface_queue_h expected_surface_queue = NULL;
	int queue_size = 0;
	int width = 200;
	int height = 100;
	int format = 50;
	int flags = 10;

	_init_test();

	surface_queue = tbm_surface_queue_create(queue_size, width, height,
											 format, flags);

	ASSERT_EQ(surface_queue, expected_surface_queue);
}

/* tbm_surface_queue_flush() */

TEST(tbm_surface_queue_flush, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface_queue;
	LIST_INITHEAD(&surface_queue.list);
	LIST_INITHEAD(&surface_queue.reset_noti);
	surface_queue.impl = NULL;

	_init_test();

	error = tbm_surface_queue_flush(&surface_queue);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_flush, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_flush(NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_reset() */

TEST(tbm_surface_queue_reset, work_flow_success_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface_queue;
	int width, height, format;

	_init_test();

	width = 200;
	height = 100;
	format = 10;
	surface_queue.width = 1;
	surface_queue.height = 1;
	surface_queue.format = 1;
	LIST_INITHEAD(&surface_queue.list);
	LIST_INITHEAD(&surface_queue.reset_noti);
	surface_queue.impl = NULL;

	error = tbm_surface_queue_reset(&surface_queue, width, height, format);

	ASSERT_EQ(error, expected_error);
	ASSERT_EQ(width, surface_queue.width);
	ASSERT_EQ(height, surface_queue.height);
	ASSERT_EQ(format, surface_queue.format);
}

TEST(tbm_surface_queue_reset, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface_queue;
	int width, height, format;

	_init_test();

	width = 200;
	height = 100;
	format = 10;
	surface_queue.width = width;
	surface_queue.height = height;
	surface_queue.format = format;

	error = tbm_surface_queue_reset(&surface_queue, width, height, format);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_reset, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_reset(NULL, 0, 0, 0);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_destroy() */

TEST(tbm_surface_queue_destroy, work_flow_success_1)
{
	tbm_surface_queue_h surface_queue = calloc(1, sizeof(*surface_queue));
	queue_notify *destory_item1;
	queue_notify *destory_item2;
	queue_notify *acquirable_item1;
	queue_notify *acquirable_item2;
	queue_notify *dequeuable_item1;
	queue_notify *dequeuable_item2;
	queue_notify *reset_item1;
	queue_notify *reset_item2;

	_init_test();

	surface_queue->impl = NULL;
	destory_item1 = calloc(1, sizeof(*destory_item1));
	destory_item2 = calloc(1, sizeof(*destory_item2));
	acquirable_item1 = calloc(1, sizeof(*acquirable_item1));
	acquirable_item2 = calloc(1, sizeof(*acquirable_item2));
	dequeuable_item1 = calloc(1, sizeof(*dequeuable_item1));
	dequeuable_item2 = calloc(1, sizeof(*dequeuable_item2));
	reset_item1 = calloc(1, sizeof(*reset_item1));
	reset_item2 = calloc(1, sizeof(*reset_item2));
	destory_item1->cb = ut_tbm_surface_queue_notify_cb;
	destory_item2->cb = ut_tbm_surface_queue_notify_cb;
	LIST_INITHEAD(&surface_queue->destory_noti);
	LIST_INITHEAD(&surface_queue->list);
	LIST_INITHEAD(&surface_queue->acquirable_noti);
	LIST_INITHEAD(&surface_queue->dequeuable_noti);
	LIST_INITHEAD(&surface_queue->reset_noti);
	LIST_ADD(&destory_item1->link, &surface_queue->destory_noti);
	LIST_ADD(&destory_item2->link, &surface_queue->destory_noti);
	LIST_ADD(&dequeuable_item1->link, &surface_queue->dequeuable_noti);
	LIST_ADD(&dequeuable_item2->link, &surface_queue->dequeuable_noti);
	LIST_ADD(&acquirable_item1->link, &surface_queue->acquirable_noti);
	LIST_ADD(&acquirable_item2->link, &surface_queue->acquirable_noti);
	LIST_ADD(&reset_item1->link, &surface_queue->reset_noti);
	LIST_ADD(&reset_item2->link, &surface_queue->reset_noti);

	tbm_surface_queue_destroy(surface_queue);

	ASSERT_GE(free_call_count, 9);
}

/* tbm_surface_queue_can_acquire() */

TEST(tbm_surface_queue_can_acquire, work_flow_success_3)
{
	int actual = 0;
	int expected = 1;
	int wait = 0;
	tbm_surface_queue_s surface_queue;
	queue_node node;

	_init_test();

	_queue_init(&surface_queue.dirty_queue);
	_queue_node_push_back(&surface_queue.dirty_queue, &node);

	actual = tbm_surface_queue_can_acquire(&surface_queue, wait);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_queue_can_acquire, work_flow_success_2)
{
	int actual = 1;
	int expected = 0;
	int wait = 0;
	tbm_surface_queue_s surface_queue;

	_init_test();

	_queue_init(&surface_queue.dirty_queue);

	actual = tbm_surface_queue_can_acquire(&surface_queue, wait);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_queue_can_acquire, work_flow_success_1)
{
	int actual = 1;
	int expected = 0;
	int wait = 2;
	tbm_surface_queue_s surface_queue;

	_init_test();

	_queue_init(&surface_queue.dirty_queue);
	LIST_INITHEAD(&surface_queue.list);

	actual = tbm_surface_queue_can_acquire(&surface_queue, wait);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_queue_can_acquire, null_ptr_fail_1)
{
	int actual = 1;
	int expected = 0;

	_init_test();

	actual = tbm_surface_queue_can_acquire(NULL, 0);

	ASSERT_EQ(actual, expected);
}

/* tbm_surface_queue_acquire() */

TEST(tbm_surface_queue_acquire, work_flow_success_4)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_s expected_surface;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s *surface;
	queue_node node;

	_init_test();

	surface_queue.impl = NULL;
	_queue_init(&surface_queue.dirty_queue);
	node.surface = &expected_surface;
	_queue_node_push_back(&surface_queue.dirty_queue, &node);

	error = tbm_surface_queue_acquire(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
	ASSERT_TRUE(surface == &expected_surface);
}

TEST(tbm_surface_queue_acquire, work_flow_success_3)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s *surface;
	queue_node node;

	_init_test();

	surface_queue.impl = NULL;
	_queue_init(&surface_queue.dirty_queue);
	node.surface = NULL;
	_queue_node_push_back(&surface_queue.dirty_queue, &node);

	error = tbm_surface_queue_acquire(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_acquire, work_flow_success_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_EMPTY;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s *surface;

	_init_test();

	surface_queue.impl = NULL;
	_queue_init(&surface_queue.dirty_queue);

	error = tbm_surface_queue_acquire(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_acquire, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_EMPTY;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s *surface;
	tbm_surface_queue_interface impl;

	_init_test();

	surface_queue.impl = &impl;
	impl.acquire = ut_acquire;

	error = tbm_surface_queue_acquire(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_acquire, null_ptr_fail_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;

	_init_test();

	error = tbm_surface_queue_acquire(&surface_queue, NULL);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_acquire, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_s *surface;

	_init_test();

	error = tbm_surface_queue_acquire(NULL, &surface);

	ASSERT_EQ(error, expected_error);

}

/* tbm_surface_queue_release() */

TEST(tbm_surface_queue_release, work_flow_success_5)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;
	queue_node node;

	_init_test();

	_queue_init(&surface_queue.free_queue);
	_queue_init(&surface_queue.dirty_queue);
	LIST_INITHEAD(&surface_queue.list);
	node.surface = &surface;
	LIST_ADD(&node.link, &surface_queue.list);
	surface_queue.impl = NULL;
	LIST_INITHEAD(&surface_queue.dequeuable_noti);

	error = tbm_surface_queue_release(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_release, work_flow_success_4)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;
	queue_node node;
	tbm_surface_queue_interface impl;

	_init_test();

	_queue_init(&surface_queue.free_queue);
	_queue_init(&surface_queue.dirty_queue);
	LIST_INITHEAD(&surface_queue.list);
	node.surface = &surface;
	LIST_ADD(&node.link, &surface_queue.list);
	surface_queue.impl = &impl;
	impl.release = &ut_release;

	error = tbm_surface_queue_release(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_release, work_flow_success_3)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;
	queue_node node;

	_init_test();

	_queue_init(&surface_queue.free_queue);
	_queue_init(&surface_queue.dirty_queue);
	LIST_INITHEAD(&surface_queue.list);
	node.surface = &surface;
	LIST_ADD(&node.item_link, &surface_queue.dirty_queue.head);

	error = tbm_surface_queue_release(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_release, work_flow_success_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;
	queue_node node;

	_init_test();

	_queue_init(&surface_queue.free_queue);
	_queue_init(&surface_queue.dirty_queue);
	LIST_INITHEAD(&surface_queue.list);
	node.surface = &surface;
	LIST_ADD(&node.item_link, &surface_queue.free_queue.head);

	error = tbm_surface_queue_release(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_release, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;

	_init_test();

	_queue_init(&surface_queue.free_queue);
	_queue_init(&surface_queue.dirty_queue);
	LIST_INITHEAD(&surface_queue.list);

	error = tbm_surface_queue_release(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_release, null_ptr_fail_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;

	_init_test();

	error = tbm_surface_queue_release(&surface_queue, NULL);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_release, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_s surface;

	_init_test();

	error = tbm_surface_queue_release(NULL, &surface);

	ASSERT_EQ(error, expected_error);

}

/* tbm_surface_queue_can_dequeue() */

TEST(tbm_surface_queue_can_dequeue, work_flow_success_3)
{
	int actual = 0;
	int expected = 1;
	int wait = 0;
	tbm_surface_queue_s surface_queue;
	queue_node node;

	_init_test();

	LIST_INITHEAD(&surface_queue.free_queue.head);
	surface_queue.free_queue.count = 0;
	surface_queue.impl = NULL;
	LIST_INITHEAD(&surface_queue.free_queue.head);
	LIST_ADD(&node.item_link, &surface_queue.free_queue.head);

	actual = tbm_surface_queue_can_dequeue(&surface_queue, wait);

	ASSERT_EQ(actual, expected);
}


TEST(tbm_surface_queue_can_dequeue, work_flow_success_2)
{
	int actual = 1;
	int expected = 0;
	int wait = 0;
	tbm_surface_queue_s surface_queue;

	_init_test();

	LIST_INITHEAD(&surface_queue.free_queue.head);
	surface_queue.free_queue.count = 0;
	surface_queue.impl = NULL;

	actual = tbm_surface_queue_can_dequeue(&surface_queue, wait);

	ASSERT_EQ(actual, expected);
}


TEST(tbm_surface_queue_can_dequeue, work_flow_success_1)
{
	int actual = 0;
	int expected = 1;
	int wait = 2;
	tbm_surface_queue_s surface_queue;

	_init_test();

	LIST_INITHEAD(&surface_queue.free_queue.head);
	surface_queue.free_queue.count = 0;
	surface_queue.impl = NULL;

	actual = tbm_surface_queue_can_dequeue(&surface_queue, wait);

	ASSERT_EQ(actual, expected);
}

TEST(tbm_surface_queue_can_dequeue, null_ptr_fail_1)
{
	int actual = 1;
	int expected = 0;

	_init_test();

	actual = tbm_surface_queue_can_dequeue(NULL, 0);

	ASSERT_EQ(actual, expected);
}

/* tbm_surface_queue_dequeue() */

TEST(tbm_surface_queue_dequeue, work_flow_success_3)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s *surface;
	tbm_surface_s expected_surface;
	queue_node node;

	_init_test();

	surface_queue.impl = NULL;
	node.surface = &expected_surface;
	LIST_INITHEAD(&surface_queue.free_queue.head);
	LIST_ADD(&node.item_link, &surface_queue.free_queue.head);

	error = tbm_surface_queue_dequeue(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
	ASSERT_TRUE(surface == &expected_surface);
}

TEST(tbm_surface_queue_dequeue, work_flow_success_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s *surface;
	queue_node node;

	_init_test();

	surface_queue.impl = NULL;
	node.surface = NULL;
	LIST_INITHEAD(&surface_queue.free_queue.head);
	LIST_ADD(&node.item_link, &surface_queue.free_queue.head);

	error = tbm_surface_queue_dequeue(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_dequeue, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_EMPTY;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s *surface;
	tbm_surface_queue_interface impl;

	_init_test();

	surface_queue.impl = &impl;
	impl.dequeue = ut_dequeue;

	error = tbm_surface_queue_dequeue(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_dequeue, null_ptr_fail_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;

	_init_test();

	error = tbm_surface_queue_dequeue(&surface_queue, NULL);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_dequeue, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_s *surface;

	_init_test();

	error = tbm_surface_queue_dequeue(NULL, &surface);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_enqueue() */

TEST(tbm_surface_queue_enqueue, work_flow_success_5)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;
	queue_node node;

	_init_test();

	node.surface = &surface;
	LIST_INITHEAD(&surface_queue.free_queue.head);
	LIST_INITHEAD(&surface_queue.dirty_queue.head);
	LIST_INITHEAD(&surface_queue.list);
	LIST_ADD(&node.link, &surface_queue.list);
	tbm_surface_queue_interface impl;
	surface_queue.impl = &impl;
	impl.enqueue = ut_enqueue;
	surface_queue.dirty_queue.count = 0;
	LIST_INITHEAD(&surface_queue.dirty_queue.head);
	LIST_INITHEAD(&surface_queue.acquirable_noti);

	error = tbm_surface_queue_enqueue(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_enqueue, work_flow_success_4)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;
	queue_node node;

	_init_test();

	node.surface = &surface;
	LIST_INITHEAD(&surface_queue.free_queue.head);
	LIST_INITHEAD(&surface_queue.dirty_queue.head);
	LIST_INITHEAD(&surface_queue.list);
	LIST_ADD(&node.link, &surface_queue.list);
	surface_queue.impl = NULL;
	surface_queue.dirty_queue.count = 0;
	LIST_INITHEAD(&surface_queue.dirty_queue.head);
	LIST_INITHEAD(&surface_queue.acquirable_noti);

	error = tbm_surface_queue_enqueue(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_enqueue, work_flow_success_3)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;
	queue_node node;

	_init_test();

	LIST_INITHEAD(&surface_queue.free_queue.head);
	LIST_INITHEAD(&surface_queue.dirty_queue.head);
	LIST_INITHEAD(&surface_queue.list);
	LIST_ADD(&node.item_link, &surface_queue.dirty_queue.head);

	error = tbm_surface_queue_enqueue(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_enqueue, work_flow_success_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;
	queue_node node;

	_init_test();

	LIST_INITHEAD(&surface_queue.free_queue.head);
	LIST_INITHEAD(&surface_queue.dirty_queue.head);
	LIST_INITHEAD(&surface_queue.list);
	LIST_ADD(&node.item_link, &surface_queue.free_queue.head);

	error = tbm_surface_queue_enqueue(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_enqueue, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;
	tbm_surface_s surface;

	_init_test();

	LIST_INITHEAD(&surface_queue.free_queue.head);
	LIST_INITHEAD(&surface_queue.dirty_queue.head);
	LIST_INITHEAD(&surface_queue.list);

	error = tbm_surface_queue_enqueue(&surface_queue, &surface);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_enqueue, null_ptr_fail_2)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	tbm_surface_queue_s surface_queue;

	_init_test();

	error = tbm_surface_queue_enqueue(&surface_queue, NULL);

	ASSERT_EQ(error, expected_error);
}

TEST(tbm_surface_queue_enqueue, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_s surface;

	_init_test();

	error = tbm_surface_queue_enqueue(NULL, &surface);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_remove_reset_cb() */

TEST(tbm_surface_queue_remove_reset_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	queue_notify *item;
	int data;
	queue_notify *find_item;
	queue_notify *tmp;

	_init_test();

	item = (queue_notify *)calloc(1, sizeof(queue_notify));
	FREE_TESTED_PTR = item;

	LIST_INITHEAD(&item->link);
	item->cb = ut_tbm_surface_queue_notify_cb;
	item->data = &data;

	LIST_INITHEAD(&surface.reset_noti);
	LIST_ADDTAIL(&item->link, &surface.reset_noti);

	find_item = NULL;

	error = tbm_surface_queue_remove_reset_cb(&surface,
											 ut_tbm_surface_queue_notify_cb,
											 &data);

	ASSERT_EQ(error, expected_error);
	if (!LIST_IS_EMPTY(&surface.reset_noti)) {
		LIST_FOR_EACH_ENTRY_SAFE(find_item, tmp, &surface.reset_noti, link) {
			if (find_item->data == &data)
				break;
		}
	}
	ASSERT_TRUE(find_item == NULL);
	ASSERT_EQ(free_called_for_tested_ptr, 1);
}

TEST(tbm_surface_queue_remove_reset_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_remove_reset_cb(NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_add_reset_cb() */

TEST(tbm_surface_queue_add_reset_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	int data;
	queue_notify *item;
	queue_notify *tmp;

	_init_test();

	LIST_INITHEAD(&surface.reset_noti);

	error = tbm_surface_queue_add_reset_cb(&surface,
											 ut_tbm_surface_queue_notify_cb,
											 &data);

	ASSERT_EQ(error, expected_error);
	if (!LIST_IS_EMPTY(&surface.reset_noti)) {
		LIST_FOR_EACH_ENTRY_SAFE(item, tmp, &surface.reset_noti, link) {
			if (item->data == &data && item->cb == ut_tbm_surface_queue_notify_cb)
				break;
		}
	}
	ASSERT_TRUE(item != NULL);
	free(item);
}

TEST(tbm_surface_queue_add_reset_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_add_reset_cb(NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_get_size() */

TEST(tbm_surface_queue_get_size, work_flow_success_1)
{
	int size = 10;
	int expected_size = 1000;
	tbm_surface_queue_s surface;

	_init_test();

	surface.queue_size = expected_size;

	size = tbm_surface_queue_get_size(&surface);

	ASSERT_EQ(size, expected_size);
}

TEST(tbm_surface_queue_get_size, null_ptr_fail_1)
{
	int size = 10;
	int expected_size = 0;

	_init_test();

	size = tbm_surface_queue_get_size(NULL);

	ASSERT_EQ(size, expected_size);
}

/* tbm_surface_queue_get_format() */

TEST(tbm_surface_queue_get_format, work_flow_success_1)
{
	int format = 10;
	int expected_format = 20;
	tbm_surface_queue_s surface;

	_init_test();

	surface.format = expected_format;

	format = tbm_surface_queue_get_format(&surface);

	ASSERT_EQ(format, expected_format);
}

TEST(tbm_surface_queue_get_format, null_ptr_fail_1)
{
	int format = 10;
	int expected_format = 0;

	_init_test();

	format = tbm_surface_queue_get_format(NULL);

	ASSERT_EQ(format, expected_format);
}

/* tbm_surface_queue_get_height() */

TEST(tbm_surface_queue_get_height, work_flow_success_1)
{
	int height = 0;
	int expected_height = 50;
	tbm_surface_queue_s surface;

	_init_test();

	surface.height = expected_height;

	height = tbm_surface_queue_get_height(&surface);

	ASSERT_EQ(height, expected_height);
}

TEST(tbm_surface_queue_get_height, null_ptr_fail_1)
{
	int height = 10;
	int expected_height = 0;

	_init_test();

	height = tbm_surface_queue_get_height(NULL);

	ASSERT_EQ(height, expected_height);
}

/* tbm_surface_queue_get_width() */

TEST(tbm_surface_queue_get_width, work_flow_success_1)
{
	int width = 0;
	int expected_width = 50;
	tbm_surface_queue_s surface;

	_init_test();

	surface.width = expected_width;

	width = tbm_surface_queue_get_width(&surface);

	ASSERT_EQ(width, expected_width);
}

TEST(tbm_surface_queue_get_width, null_ptr_fail_1)
{
	int width = 10;
	int expected_width = 0;

	_init_test();

	width = tbm_surface_queue_get_width(NULL);

	ASSERT_EQ(width, expected_width);
}

/* tbm_surface_queue_set_alloc_cb() */

TEST(tbm_surface_queue_set_alloc_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	int data;

	_init_test();

	error = tbm_surface_queue_set_alloc_cb(&surface, ut_tbm_surface_alloc_cb,
										   ut_tbm_surface_free_cb, &data);

	ASSERT_EQ(error, expected_error);
	ASSERT_TRUE(surface.alloc_cb == ut_tbm_surface_alloc_cb);
	ASSERT_TRUE(surface.free_cb == ut_tbm_surface_free_cb);
	ASSERT_TRUE(surface.alloc_cb_data == &data);
}

TEST(tbm_surface_queue_set_alloc_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_set_alloc_cb(NULL, NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_remove_acquirable_cb() */

TEST(tbm_surface_queue_remove_acquirable_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	int data;
	queue_notify *item;
	queue_notify *find_item;
	queue_notify *tmp;

	_init_test();

	item = (queue_notify *)calloc(1, sizeof(queue_notify));
	FREE_TESTED_PTR = item;

	LIST_INITHEAD(&item->link);
	item->cb = ut_tbm_surface_queue_notify_cb;
	item->data = &data;

	LIST_INITHEAD(&surface.acquirable_noti);
	LIST_ADDTAIL(&item->link, &surface.acquirable_noti);

	find_item = NULL;

	error = tbm_surface_queue_remove_acquirable_cb(&surface,
											 ut_tbm_surface_queue_notify_cb,
											 &data);

	ASSERT_EQ(error, expected_error);
	if (!LIST_IS_EMPTY(&surface.acquirable_noti)) {
		LIST_FOR_EACH_ENTRY_SAFE(find_item, tmp, &surface.acquirable_noti, link) {
			if (find_item->data == &data)
				break;
		}
	}
	ASSERT_TRUE(find_item == NULL);
	ASSERT_EQ(free_called_for_tested_ptr, 1);
}

TEST(tbm_surface_queue_remove_acquirable_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_remove_acquirable_cb(NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_add_acquirable_cb() */

TEST(tbm_surface_queue_add_acquirable_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	int data;
	queue_notify *item;
	queue_notify *tmp;
	tbm_surface_queue_notify_cb cb;
	tbm_surface_queue_notify_cb expected_cb;

	_init_test();

	LIST_INITHEAD(&surface.acquirable_noti);

	error = tbm_surface_queue_add_acquirable_cb(&surface,
											 ut_tbm_surface_queue_notify_cb,
											 &data);

	ASSERT_EQ(error, expected_error);
	if (!LIST_IS_EMPTY(&surface.acquirable_noti)) {
		LIST_FOR_EACH_ENTRY_SAFE(item, tmp, &surface.acquirable_noti, link) {
			if (item->data == &data)
				break;
		}
	}
	ASSERT_TRUE(item != NULL);
	cb = item->cb;
	expected_cb = ut_tbm_surface_queue_notify_cb;
	free(item);
	ASSERT_TRUE(cb == expected_cb);
}

TEST(tbm_surface_queue_add_acquirable_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_add_acquirable_cb(NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_remove_dequeuable_cb() */

TEST(tbm_surface_queue_remove_dequeuable_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	int data;
	queue_notify *item;
	queue_notify *find_item;
	queue_notify *tmp;

	_init_test();

	item = (queue_notify *)calloc(1, sizeof(queue_notify));
	FREE_TESTED_PTR = item;

	LIST_INITHEAD(&item->link);
	item->cb = ut_tbm_surface_queue_notify_cb;
	item->data = &data;

	LIST_INITHEAD(&surface.dequeuable_noti);
	LIST_ADDTAIL(&item->link, &surface.dequeuable_noti);

	find_item = NULL;

	error = tbm_surface_queue_remove_dequeuable_cb(&surface,
											 ut_tbm_surface_queue_notify_cb,
											 &data);

	ASSERT_EQ(error, expected_error);
	if (!LIST_IS_EMPTY(&surface.dequeuable_noti)) {
		LIST_FOR_EACH_ENTRY_SAFE(find_item, tmp, &surface.dequeuable_noti, link) {
			if (find_item->data == &data)
				break;
		}
	}
	ASSERT_TRUE(find_item == NULL);
	ASSERT_EQ(free_called_for_tested_ptr, 1);
}

TEST(tbm_surface_queue_remove_dequeuable_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_remove_dequeuable_cb(NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_add_dequeuable_cb() */

TEST(tbm_surface_queue_add_dequeuable_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	int data;
	queue_notify *item;
	queue_notify *tmp;

	_init_test();

	LIST_INITHEAD(&surface.dequeuable_noti);

	error = tbm_surface_queue_add_dequeuable_cb(&surface,
											 ut_tbm_surface_queue_notify_cb,
											 &data);

	ASSERT_EQ(error, expected_error);
	if (!LIST_IS_EMPTY(&surface.dequeuable_noti)) {
		LIST_FOR_EACH_ENTRY_SAFE(item, tmp, &surface.dequeuable_noti, link) {
			if (item->data == &data)
				break;
		}
	}
	ASSERT_TRUE(item != NULL);
	tbm_surface_queue_notify_cb cb = item->cb;
	tbm_surface_queue_notify_cb expected_cb = ut_tbm_surface_queue_notify_cb;
	free(item);
	ASSERT_TRUE(cb == expected_cb);
}

TEST(tbm_surface_queue_add_dequeuable_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_add_dequeuable_cb(NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_remove_destroy_cb() */

TEST(tbm_surface_queue_remove_destroy_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	int data;
	queue_notify *item;
	queue_notify *find_item;
	queue_notify *tmp;

	_init_test();

	item = (queue_notify *)calloc(1, sizeof(queue_notify));
	FREE_TESTED_PTR = item;

	LIST_INITHEAD(&item->link);
	item->cb = ut_tbm_surface_queue_notify_cb;
	item->data = &data;

	LIST_INITHEAD(&surface.destory_noti);
	LIST_ADDTAIL(&item->link, &surface.destory_noti);

	find_item = NULL;

	error = tbm_surface_queue_remove_destroy_cb(&surface,
											 ut_tbm_surface_queue_notify_cb,
											 &data);

	ASSERT_EQ(error, expected_error);
	if (!LIST_IS_EMPTY(&surface.destory_noti)) {
		LIST_FOR_EACH_ENTRY_SAFE(find_item, tmp, &surface.destory_noti, link) {
			if (find_item->data == &data)
				break;
		}
	}
	ASSERT_TRUE(find_item == NULL);
	ASSERT_EQ(free_called_for_tested_ptr, 1);
}

TEST(tbm_surface_queue_remove_destroy_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_remove_destroy_cb(NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}

/* tbm_surface_queue_add_destroy_cb() */

TEST(tbm_surface_queue_add_destroy_cb, work_flow_success_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_s surface;
	int data;
	queue_notify *item;
	queue_notify *tmp;
	tbm_surface_queue_notify_cb cb;
	tbm_surface_queue_notify_cb expected_cb;

	_init_test();

	LIST_INITHEAD(&surface.destory_noti);

	error = tbm_surface_queue_add_destroy_cb(&surface,
											 ut_tbm_surface_queue_notify_cb,
											 &data);

	ASSERT_EQ(error, expected_error);
	if (!LIST_IS_EMPTY(&surface.destory_noti)) {
		LIST_FOR_EACH_ENTRY_SAFE(item, tmp, &surface.destory_noti, link) {
			if (item->data == &data)
				break;
		}
	}
	ASSERT_TRUE(item != NULL);
	cb = item->cb;
	expected_cb = ut_tbm_surface_queue_notify_cb;
	free(item);
	ASSERT_EQ(cb, expected_cb);
}

TEST(tbm_surface_queue_add_destroy_cb, null_ptr_fail_1)
{
	tbm_surface_queue_error_e error = TBM_SURFACE_QUEUE_ERROR_NONE;
	tbm_surface_queue_error_e expected_error =
			TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;

	_init_test();

	error = tbm_surface_queue_add_destroy_cb(NULL, NULL, NULL);

	ASSERT_EQ(error, expected_error);
}
