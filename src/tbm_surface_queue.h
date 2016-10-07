/**************************************************************************

libtbm

Copyright 2014 Samsung Electronics co., Ltd. All Rights Reserved.

Contact: SooChan Lim <sc1.lim@samsung.com>, Sangjin Lee <lsj119@samsung.com>
Boram Park <boram1288.park@samsung.com>, Changyeon Lee <cyeon.lee@samsung.com>

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

#ifndef _TBM_SURFACE_QUEUE_H_
#define _TBM_SURFACE_QUEUE_H_

#include <tbm_surface.h>

typedef enum {
	TBM_SURFACE_QUEUE_ERROR_NONE = 0,					  /**< Successful */
	TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE = -1,
	TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE = -2,
	TBM_SURFACE_QUEUE_ERROR_EMPTY = -3,
	TBM_SURFACE_QUEUE_ERROR_INVALID_PARAMETER = -4,
	TBM_SURFACE_QUEUE_ERROR_SURFACE_ALLOC_FAILED = -5,
} tbm_surface_queue_error_e;

typedef struct _tbm_surface_queue *tbm_surface_queue_h;

typedef void (*tbm_surface_queue_notify_cb)(tbm_surface_queue_h surface_queue,
		void *data);

typedef tbm_surface_h (*tbm_surface_alloc_cb)(tbm_surface_queue_h surface_queue,
		void *data);

typedef void (*tbm_surface_free_cb)(tbm_surface_queue_h surface_queue,
		void *data, tbm_surface_h surface);

#ifdef __cplusplus
extern "C" {
#endif

tbm_surface_queue_error_e tbm_surface_queue_enqueue(
		tbm_surface_queue_h surface_queue, tbm_surface_h surface);

tbm_surface_queue_error_e tbm_surface_queue_dequeue(
		tbm_surface_queue_h surface_queue, tbm_surface_h *surface);

tbm_surface_queue_error_e tbm_surface_queue_release(
		tbm_surface_queue_h surface_queue, tbm_surface_h surface);

tbm_surface_queue_error_e tbm_surface_queue_acquire(
		tbm_surface_queue_h surface_queue, tbm_surface_h *surface);

int tbm_surface_queue_can_dequeue(tbm_surface_queue_h surface_queue, int wait);

int tbm_surface_queue_can_acquire(tbm_surface_queue_h surface_queue, int wait);

void tbm_surface_queue_destroy(tbm_surface_queue_h surface_queue);

int tbm_surface_queue_get_width(tbm_surface_queue_h surface_queue);

int tbm_surface_queue_get_height(tbm_surface_queue_h surface_queue);

int tbm_surface_queue_get_format(tbm_surface_queue_h surface_queue);

int tbm_surface_queue_get_size(tbm_surface_queue_h surface_queue);

tbm_surface_queue_error_e tbm_surface_queue_reset(
		tbm_surface_queue_h surface_queue, int width, int height, int format);

tbm_surface_queue_error_e tbm_surface_queue_set_size(
		tbm_surface_queue_h surface_queue, int queue_size, int flush);

tbm_surface_queue_error_e tbm_surface_queue_flush(tbm_surface_queue_h surface_queue);

tbm_surface_queue_error_e tbm_surface_queue_add_reset_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb reset_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_remove_reset_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb reset_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_add_destroy_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb destroy_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_remove_destroy_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb destroy_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_add_dequeuable_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb dequeuable_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_remove_dequeuable_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb dequeuable_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_add_dequeue_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb dequeue_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_remove_dequeue_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb dequeue_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_add_acquirable_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb acquirable_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_remove_acquirable_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb acquirable_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_set_alloc_cb(
	tbm_surface_queue_h surface_queue,
	tbm_surface_alloc_cb alloc_cb,
	tbm_surface_free_cb free_cb,
	void *data);

tbm_surface_queue_error_e tbm_surface_queue_get_surfaces(
	tbm_surface_queue_h surface_queue,
	tbm_surface_h *surfaces, int *num);

/*The functions of queue factory*/
tbm_surface_queue_h tbm_surface_queue_create(int queue_size, int width,
		int height, int format, int flags);
tbm_surface_queue_h tbm_surface_queue_sequence_create(int queue_size, int width,
		int height, int format, int flags);

#ifdef __cplusplus
}
#endif
#endif							/* _TBM_SURFACE_H_ */
