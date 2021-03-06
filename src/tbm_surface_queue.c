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

#include "config.h"

#include "tbm_bufmgr_int.h"
#include "list.h"

#define FREE_QUEUE	1
#define DIRTY_QUEUE	2
#define NODE_LIST	4

#define TBM_QUEUE_DEBUG 0

#ifdef TRACE
#define TBM_QUEUE_TRACE(fmt, ...)  { if (bTrace&0x1) fprintf(stderr, "[TBM:TRACE(%d)(%s:%d)] " fmt, getpid(), __func__, __LINE__, ##__VA_ARGS__); }
#else
#define TBM_QUEUE_TRACE(fmt, ...)
#endif /* TRACE */

#if TBM_QUEUE_DEBUG
#define TBM_LOCK() TBM_LOG_D("[LOCK] %s:%d surface:%p\n", __func__, __LINE__, surface_queue)
#define TBM_UNLOCK() TBM_LOG_D("[UNLOCK] %s:%d surface:%p\n", __func__, __LINE__, surface_queue)
#else
#define TBM_LOCK()
#define TBM_UNLOCK()
#endif

static tbm_bufmgr g_surf_queue_bufmgr;
static pthread_mutex_t tbm_surf_queue_lock;
void _tbm_surface_queue_mutex_unlock(void);

/* check condition */
#define TBM_SURF_QUEUE_RETURN_IF_FAIL(cond) {\
	if (!(cond)) {\
		TBM_LOG_E("'%s' failed.\n", #cond);\
		_tbm_surf_queue_mutex_unlock();\
		return;\
	} \
}

#define TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(cond, val) {\
	if (!(cond)) {\
		TBM_LOG_E("'%s' failed.\n", #cond);\
		_tbm_surf_queue_mutex_unlock();\
		return val;\
	} \
}

typedef enum _queue_node_type {
	QUEUE_NODE_TYPE_NONE,
	QUEUE_NODE_TYPE_DEQUEUE,
	QUEUE_NODE_TYPE_ENQUEUE,
	QUEUE_NODE_TYPE_ACQUIRE,
	QUEUE_NODE_TYPE_RELEASE
} Queue_Node_Type;

typedef struct {
	struct list_head head;
	int count;
} queue;

typedef struct {
	tbm_surface_h surface;

	struct list_head item_link;
	struct list_head link;

	Queue_Node_Type type;

	unsigned int priv_flags;	/*for each queue*/
} queue_node;

typedef struct {
	struct list_head link;

	tbm_surface_queue_notify_cb cb;
	void *data;
} queue_notify;

typedef struct {
	struct list_head link;

	tbm_surface_queue_trace_cb cb;
	void *data;
} queue_trace;

typedef struct _tbm_surface_queue_interface {
	void (*init)(tbm_surface_queue_h queue);
	void (*reset)(tbm_surface_queue_h queue);
	void (*destroy)(tbm_surface_queue_h queue);
	void (*need_attach)(tbm_surface_queue_h queue);

	void (*enqueue)(tbm_surface_queue_h queue, queue_node *node);
	void (*release)(tbm_surface_queue_h queue, queue_node *node);
	queue_node *(*dequeue)(tbm_surface_queue_h queue);
	queue_node *(*acquire)(tbm_surface_queue_h queue);
	void (*need_detach)(tbm_surface_queue_h queue, queue_node *node);
} tbm_surface_queue_interface;

struct _tbm_surface_queue {
	int width;
	int height;
	int format;
	int queue_size;
	int num_attached;

	queue free_queue;
	queue dirty_queue;
	struct list_head list;

	struct list_head destory_noti;
	struct list_head dequeuable_noti;
	struct list_head dequeue_noti;
	struct list_head can_dequeue_noti;
	struct list_head acquirable_noti;
	struct list_head reset_noti;
	struct list_head trace_noti;

	pthread_mutex_t lock;
	pthread_cond_t free_cond;
	pthread_cond_t dirty_cond;

	const tbm_surface_queue_interface *impl;
	void *impl_data;

	//For external buffer allocation
	tbm_surface_alloc_cb alloc_cb;
	tbm_surface_free_cb free_cb;
	void *alloc_cb_data;

	struct list_head item_link; /* link of surface queue */
};

/* LCOV_EXCL_START */

static bool
_tbm_surf_queue_mutex_init(void)
{
	static bool tbm_surf_queue_mutex_init = false;

	if (tbm_surf_queue_mutex_init)
		return true;

	if (pthread_mutex_init(&tbm_surf_queue_lock, NULL)) {
		TBM_LOG_E("fail: pthread_mutex_init\n");
		return false;
	}

	tbm_surf_queue_mutex_init = true;

	return true;
}

static void
_tbm_surf_queue_mutex_lock(void)
{
	if (!_tbm_surf_queue_mutex_init()) {
		TBM_LOG_E("fail: _tbm_surf_queue_mutex_init\n");
		return;
	}

	pthread_mutex_lock(&tbm_surf_queue_lock);
}

static void
_tbm_surf_queue_mutex_unlock(void)
{
	pthread_mutex_unlock(&tbm_surf_queue_lock);
}

static void
_init_tbm_surf_queue_bufmgr(void)
{
	g_surf_queue_bufmgr = tbm_bufmgr_init(-1);
}

static void
_deinit_tbm_surf_queue_bufmgr(void)
{
	if (!g_surf_queue_bufmgr)
		return;

	tbm_bufmgr_deinit(g_surf_queue_bufmgr);
	g_surf_queue_bufmgr = NULL;
}

static int
_tbm_surface_queue_is_valid(tbm_surface_queue_h surface_queue)
{
	tbm_surface_queue_h old_data = NULL;

	if (surface_queue == NULL) {
		TBM_LOG_E("error: surface_queue is NULL.\n");
		return 0;
	}

	if (g_surf_queue_bufmgr == NULL) {
		TBM_LOG_E("error: g_surf_queue_bufmgr is NULL.\n");
		return 0;
	}

	if (LIST_IS_EMPTY(&g_surf_queue_bufmgr->surf_queue_list)) {
		TBM_LOG_E("error: surf_queue_list is empty\n");
		return 0;
	}

	LIST_FOR_EACH_ENTRY(old_data, &g_surf_queue_bufmgr->surf_queue_list,
				item_link) {
		if (old_data == surface_queue) {
			TBM_TRACE("tbm_surface_queue(%p)\n", surface_queue);
			return 1;
		}
	}

	TBM_LOG_E("error: Invalid tbm_surface_queue(%p)\n", surface_queue);

	return 0;
}

static queue_node *
_queue_node_create(void)
{
	queue_node *node = (queue_node *) calloc(1, sizeof(queue_node));

	TBM_RETURN_VAL_IF_FAIL(node != NULL, NULL);

	return node;
}

static void
_queue_node_delete(queue_node *node)
{
	LIST_DEL(&node->item_link);
	LIST_DEL(&node->link);
	free(node);
}

static int
_queue_is_empty(queue *queue)
{
	if (LIST_IS_EMPTY(&queue->head))
		return 1;

	return 0;
}

static void
_queue_node_push_back(queue *queue, queue_node *node)
{
	LIST_ADDTAIL(&node->item_link, &queue->head);
	queue->count++;
}

static void
_queue_node_push_front(queue *queue, queue_node *node)
{
	LIST_ADD(&node->item_link, &queue->head);
	queue->count++;
}

static queue_node *
_queue_node_pop_front(queue *queue)
{
	queue_node *node;

	node = LIST_ENTRY(queue_node, queue->head.next, item_link);

	LIST_DEL(&node->item_link);
	queue->count--;

	return node;
}

static queue_node *
_queue_node_pop(queue *queue, queue_node *node)
{
	LIST_DEL(&node->item_link);
	queue->count--;

	return node;
}

static queue_node *
_queue_get_node(tbm_surface_queue_h surface_queue, int type,
		tbm_surface_h surface, int *out_type)
{
	queue_node *node = NULL;

	if (type == 0)
		type = FREE_QUEUE | DIRTY_QUEUE | NODE_LIST;
	if (out_type)
		*out_type = 0;

	if (type & FREE_QUEUE) {
		LIST_FOR_EACH_ENTRY(node, &surface_queue->free_queue.head,
					 item_link) {
			if (node->surface == surface) {
				if (out_type)
					*out_type = FREE_QUEUE;

				return node;
			}
		}
	}

	if (type & DIRTY_QUEUE) {
		LIST_FOR_EACH_ENTRY(node, &surface_queue->dirty_queue.head,
					 item_link) {
			if (node->surface == surface) {
				if (out_type)
					*out_type = DIRTY_QUEUE;

				return node;
			}
		}
	}

	if (type & NODE_LIST) {
		LIST_FOR_EACH_ENTRY(node, &surface_queue->list, link) {
			if (node->surface == surface) {
				if (out_type)
					*out_type = NODE_LIST;

				return node;
			}
		}
	}

	TBM_LOG_E("fail to get the queue_node.\n");

	return NULL;
}

static void
_queue_delete_node(tbm_surface_queue_h surface_queue, queue_node *node)
{
	if (node->surface) {
		if (surface_queue->free_cb) {
			surface_queue->free_cb(surface_queue,
					surface_queue->alloc_cb_data,
					node->surface);
		}

		tbm_surface_destroy(node->surface);
	}

	_queue_node_delete(node);
}

static void
_queue_init(queue *queue)
{
	LIST_INITHEAD(&queue->head);

	queue->count = 0;
}

static void
_notify_add(struct list_head *list, tbm_surface_queue_notify_cb cb,
	    void *data)
{
	TBM_RETURN_IF_FAIL(cb != NULL);

	queue_notify *item = (queue_notify *)calloc(1, sizeof(queue_notify));

	TBM_RETURN_IF_FAIL(item != NULL);

	LIST_INITHEAD(&item->link);
	item->cb = cb;
	item->data = data;

	LIST_ADDTAIL(&item->link, list);
}

static void
_notify_remove(struct list_head *list,
	       tbm_surface_queue_notify_cb cb, void *data)
{
	queue_notify *item = NULL, *tmp;

	LIST_FOR_EACH_ENTRY_SAFE(item, tmp, list, link) {
		if (item->cb == cb && item->data == data) {
			LIST_DEL(&item->link);
			free(item);
			return;
		}
	}

	TBM_LOG_E("Cannot find notifiy\n");
}

static void
_notify_remove_all(struct list_head *list)
{
	queue_notify *item = NULL, *tmp;

	LIST_FOR_EACH_ENTRY_SAFE(item, tmp, list, link) {
		LIST_DEL(&item->link);
		free(item);
	}
}

static void
_notify_emit(tbm_surface_queue_h surface_queue,
	     struct list_head *list)
{
	queue_notify *item = NULL, *tmp;;

	/*
		The item->cb is the outside function of the libtbm.
		The tbm user may/can remove the item of the list,
		so we have to use the LIST_FOR_EACH_ENTRY_SAFE.
	*/
	LIST_FOR_EACH_ENTRY_SAFE(item, tmp, list, link)
		item->cb(surface_queue, item->data);
}

static void
_trace_add(struct list_head *list, tbm_surface_queue_trace_cb cb,
	    void *data)
{
	TBM_RETURN_IF_FAIL(cb != NULL);

	queue_trace *item = (queue_trace *)calloc(1, sizeof(queue_trace));

	TBM_RETURN_IF_FAIL(item != NULL);

	LIST_INITHEAD(&item->link);
	item->cb = cb;
	item->data = data;

	LIST_ADDTAIL(&item->link, list);
}

static void
_trace_remove(struct list_head *list,
	       tbm_surface_queue_trace_cb cb, void *data)
{
	queue_trace *item = NULL, *tmp;

	LIST_FOR_EACH_ENTRY_SAFE(item, tmp, list, link) {
		if (item->cb == cb && item->data == data) {
			LIST_DEL(&item->link);
			free(item);
			return;
		}
	}

	TBM_LOG_E("Cannot find notifiy\n");
}

static void
_trace_remove_all(struct list_head *list)
{
	queue_trace *item = NULL, *tmp;

	LIST_FOR_EACH_ENTRY_SAFE(item, tmp, list, link) {
		LIST_DEL(&item->link);
		free(item);
	}
}

static void
_trace_emit(tbm_surface_queue_h surface_queue,
	     struct list_head *list, tbm_surface_h surface, tbm_surface_queue_trace trace)
{
	queue_trace *item = NULL, *tmp;;

	/*
		The item->cb is the outside function of the libtbm.
		The tbm user may/can remove the item of the list,
		so we have to use the LIST_FOR_EACH_ENTRY_SAFE.
	*/
	LIST_FOR_EACH_ENTRY_SAFE(item, tmp, list, link)
		item->cb(surface_queue, surface, trace, item->data);
}

static int
_tbm_surface_queue_get_node_count(tbm_surface_queue_h surface_queue, Queue_Node_Type type)
{
	queue_node *node = NULL;
	int count = 0;

	LIST_FOR_EACH_ENTRY(node, &surface_queue->list, link) {
		if (node->type == type)
			count++;
	}

	return count;
}

static void
_tbm_surface_queue_attach(tbm_surface_queue_h surface_queue,
			  tbm_surface_h surface)
{
	queue_node *node;

	node = _queue_node_create();
	TBM_RETURN_IF_FAIL(node != NULL);

	tbm_surface_internal_ref(surface);
	node->surface = surface;

	LIST_ADDTAIL(&node->link, &surface_queue->list);
	surface_queue->num_attached++;
	_queue_node_push_back(&surface_queue->free_queue, node);
}

static void
_tbm_surface_queue_detach(tbm_surface_queue_h surface_queue,
			  tbm_surface_h surface)
{
	queue_node *node;
	int queue_type;

	node = _queue_get_node(surface_queue, 0, surface, &queue_type);
	if (node) {
		_queue_delete_node(surface_queue, node);
		surface_queue->num_attached--;
	}
}

static void
_tbm_surface_queue_enqueue(tbm_surface_queue_h surface_queue,
			   queue_node *node, int push_back)
{
	if (push_back)
		_queue_node_push_back(&surface_queue->dirty_queue, node);
	else
		_queue_node_push_front(&surface_queue->dirty_queue, node);
}

static queue_node *
_tbm_surface_queue_dequeue(tbm_surface_queue_h surface_queue)
{
	queue_node *node;

	if (_queue_is_empty(&surface_queue->free_queue)) {
		if (surface_queue->impl && surface_queue->impl->need_attach)
			surface_queue->impl->need_attach(surface_queue);

		if (_queue_is_empty(&surface_queue->free_queue)) {
			TBM_LOG_E("surface_queue->free_queue is empty.\n");
			return NULL;
		}
	}

	node = _queue_node_pop_front(&surface_queue->free_queue);

	return node;
}

static queue_node *
_tbm_surface_queue_acquire(tbm_surface_queue_h surface_queue)
{
	queue_node *node;

	if (_queue_is_empty(&surface_queue->dirty_queue))
		return NULL;

	node = _queue_node_pop_front(&surface_queue->dirty_queue);

	return node;
}

static void
_tbm_surface_queue_release(tbm_surface_queue_h surface_queue,
			   queue_node *node, int push_back)
{
	if (push_back)
		_queue_node_push_back(&surface_queue->free_queue, node);
	else
		_queue_node_push_front(&surface_queue->free_queue, node);
}

static void
_tbm_surface_queue_init(tbm_surface_queue_h surface_queue,
			int queue_size,
			int width, int height, int format,
			const tbm_surface_queue_interface *impl, void *data)
{
	TBM_RETURN_IF_FAIL(surface_queue != NULL);
	TBM_RETURN_IF_FAIL(impl != NULL);

	if (!g_surf_queue_bufmgr)
		_init_tbm_surf_queue_bufmgr();

	pthread_mutex_init(&surface_queue->lock, NULL);
	pthread_cond_init(&surface_queue->free_cond, NULL);
	pthread_cond_init(&surface_queue->dirty_cond, NULL);

	surface_queue->queue_size = queue_size;
	surface_queue->width = width;
	surface_queue->height = height;
	surface_queue->format = format;
	surface_queue->impl = impl;
	surface_queue->impl_data = data;

	_queue_init(&surface_queue->free_queue);
	_queue_init(&surface_queue->dirty_queue);
	LIST_INITHEAD(&surface_queue->list);

	LIST_INITHEAD(&surface_queue->destory_noti);
	LIST_INITHEAD(&surface_queue->dequeuable_noti);
	LIST_INITHEAD(&surface_queue->dequeue_noti);
	LIST_INITHEAD(&surface_queue->can_dequeue_noti);
	LIST_INITHEAD(&surface_queue->acquirable_noti);
	LIST_INITHEAD(&surface_queue->reset_noti);
	LIST_INITHEAD(&surface_queue->trace_noti);

	if (surface_queue->impl && surface_queue->impl->init)
		surface_queue->impl->init(surface_queue);

	LIST_ADD(&surface_queue->item_link, &g_surf_queue_bufmgr->surf_queue_list);
}

tbm_surface_queue_error_e
tbm_surface_queue_add_destroy_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb destroy_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_add(&surface_queue->destory_noti, destroy_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_remove_destroy_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb destroy_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_remove(&surface_queue->destory_noti, destroy_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_add_dequeuable_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb dequeuable_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_add(&surface_queue->dequeuable_noti, dequeuable_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_remove_dequeuable_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb dequeuable_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_remove(&surface_queue->dequeuable_noti, dequeuable_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_add_dequeue_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb dequeue_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_add(&surface_queue->dequeue_noti, dequeue_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_remove_dequeue_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb dequeue_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_remove(&surface_queue->dequeue_noti, dequeue_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_add_can_dequeue_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb can_dequeue_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_add(&surface_queue->can_dequeue_noti, can_dequeue_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_remove_can_dequeue_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb can_dequeue_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_remove(&surface_queue->can_dequeue_noti, can_dequeue_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_add_acquirable_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb acquirable_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_add(&surface_queue->acquirable_noti, acquirable_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_remove_acquirable_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb acquirable_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_remove(&surface_queue->acquirable_noti, acquirable_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_add_trace_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_trace_cb trace_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_trace_add(&surface_queue->trace_noti, trace_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_remove_trace_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_trace_cb trace_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_trace_remove(&surface_queue->trace_noti, trace_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_set_alloc_cb(
	tbm_surface_queue_h surface_queue,
	tbm_surface_alloc_cb alloc_cb,
	tbm_surface_free_cb free_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	surface_queue->alloc_cb = alloc_cb;
	surface_queue->free_cb = free_cb;
	surface_queue->alloc_cb_data = data;

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

int
tbm_surface_queue_get_width(tbm_surface_queue_h surface_queue)
{
	int width;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue), 0);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	width = surface_queue->width;

	_tbm_surf_queue_mutex_unlock();

	return width;
}

int
tbm_surface_queue_get_height(tbm_surface_queue_h surface_queue)
{
	int height;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue), 0);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	height = surface_queue->height;

	_tbm_surf_queue_mutex_unlock();

	return height;
}

int
tbm_surface_queue_get_format(tbm_surface_queue_h surface_queue)
{
	int format;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue), 0);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	format = surface_queue->format;

	_tbm_surf_queue_mutex_unlock();

	return format;
}

int
tbm_surface_queue_get_size(tbm_surface_queue_h surface_queue)
{
	int queue_size;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue), 0);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	queue_size = surface_queue->queue_size;

	_tbm_surf_queue_mutex_unlock();

	return queue_size;
}

tbm_surface_queue_error_e
tbm_surface_queue_add_reset_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb reset_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_add(&surface_queue->reset_noti, reset_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_remove_reset_cb(
	tbm_surface_queue_h surface_queue, tbm_surface_queue_notify_cb reset_cb,
	void *data)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	_notify_remove(&surface_queue->reset_noti, reset_cb, data);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_enqueue(tbm_surface_queue_h
			  surface_queue, tbm_surface_h surface)
{
	queue_node *node;
	int queue_type;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);
	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(surface != NULL,
			       TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE);

	if (b_dump_queue)
		tbm_surface_internal_dump_buffer(surface, "enqueue");

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p) tbm_surface(%p)\n", surface_queue, surface);

	node = _queue_get_node(surface_queue, 0, surface, &queue_type);
	if (node == NULL || queue_type != NODE_LIST) {
		TBM_LOG_E("tbm_surface_queue_enqueue::Surface is existed in free_queue or dirty_queue node:%p, type:%d\n",
			node, queue_type);
		pthread_mutex_unlock(&surface_queue->lock);

		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_ALREADY_EXIST;
	}

	if (surface_queue->impl && surface_queue->impl->enqueue)
		surface_queue->impl->enqueue(surface_queue, node);
	else
		_tbm_surface_queue_enqueue(surface_queue, node, 1);

	if (_queue_is_empty(&surface_queue->dirty_queue)) {
		TBM_LOG_E("enqueue surface but queue is empty node:%p\n", node);
		pthread_mutex_unlock(&surface_queue->lock);

		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_UNKNOWN_SURFACE;
	}

	node->type = QUEUE_NODE_TYPE_ENQUEUE;

	pthread_mutex_unlock(&surface_queue->lock);
	pthread_cond_signal(&surface_queue->dirty_cond);

	_tbm_surf_queue_mutex_unlock();

	_trace_emit(surface_queue, &surface_queue->trace_noti, surface, TBM_SURFACE_QUEUE_TRACE_ENQUEUE);

	_notify_emit(surface_queue, &surface_queue->acquirable_noti);

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_dequeue(tbm_surface_queue_h
			  surface_queue, tbm_surface_h *surface)
{
	queue_node *node;

	_tbm_surf_queue_mutex_lock();

	*surface = NULL;

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);
	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(surface != NULL,
			       TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE);

	pthread_mutex_lock(&surface_queue->lock);

	if (surface_queue->impl && surface_queue->impl->dequeue)
		node = surface_queue->impl->dequeue(surface_queue);
	else
		node = _tbm_surface_queue_dequeue(surface_queue);

	if (node == NULL || node->surface == NULL) {
		TBM_LOG_E("_queue_node_pop_front failed\n");
		pthread_mutex_unlock(&surface_queue->lock);

		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	}

	node->type = QUEUE_NODE_TYPE_DEQUEUE;
	*surface = node->surface;

	TBM_QUEUE_TRACE("tbm_surface_queue(%p) tbm_surface(%p)\n", surface_queue, *surface);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	_trace_emit(surface_queue, &surface_queue->trace_noti, *surface, TBM_SURFACE_QUEUE_TRACE_DEQUEUE);

	_notify_emit(surface_queue, &surface_queue->dequeue_noti);

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

int
tbm_surface_queue_can_dequeue(tbm_surface_queue_h surface_queue, int wait)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue), 0);

	_tbm_surf_queue_mutex_unlock();

	_notify_emit(surface_queue, &surface_queue->can_dequeue_noti);

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue), 0);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	if (_queue_is_empty(&surface_queue->free_queue)) {
		if (surface_queue->impl && surface_queue->impl->need_attach)
			surface_queue->impl->need_attach(surface_queue);

		if (!_tbm_surface_queue_is_valid(surface_queue)) {
			TBM_LOG_E("surface_queue:%p is invalid", surface_queue);
			_tbm_surf_queue_mutex_unlock();
			return 0;
		}
	}

	if (!_queue_is_empty(&surface_queue->free_queue)) {
		pthread_mutex_unlock(&surface_queue->lock);
		_tbm_surf_queue_mutex_unlock();
		return 1;
	}

	if (wait && _tbm_surface_queue_get_node_count(surface_queue,
						QUEUE_NODE_TYPE_ACQUIRE)) {
		_tbm_surf_queue_mutex_unlock();
		pthread_cond_wait(&surface_queue->free_cond, &surface_queue->lock);
		_tbm_surf_queue_mutex_lock();

		if (!_tbm_surface_queue_is_valid(surface_queue)) {
			TBM_LOG_E("surface_queue:%p is invalid", surface_queue);
			pthread_mutex_unlock(&surface_queue->lock);
			_tbm_surf_queue_mutex_unlock();
			return 0;
		}

		pthread_mutex_unlock(&surface_queue->lock);
		_tbm_surf_queue_mutex_unlock();
		return 1;
	}

	pthread_mutex_unlock(&surface_queue->lock);
	_tbm_surf_queue_mutex_unlock();
	return 0;
}

tbm_surface_queue_error_e
tbm_surface_queue_release(tbm_surface_queue_h
			  surface_queue, tbm_surface_h surface)
{
	queue_node *node;
	int queue_type;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);
	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(surface != NULL,
			       TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p) tbm_surface(%p)\n", surface_queue, surface);

	node = _queue_get_node(surface_queue, 0, surface, &queue_type);
	if (node == NULL || queue_type != NODE_LIST) {
		TBM_LOG_E("tbm_surface_queue_release::Surface is existed in free_queue or dirty_queue node:%p, type:%d\n",
			node, queue_type);
		pthread_mutex_unlock(&surface_queue->lock);

		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	}

	if (surface_queue->queue_size < surface_queue->num_attached) {
		TBM_QUEUE_TRACE("deatch tbm_surface_queue(%p) surface(%p)\n", surface_queue, node->surface);

		if (surface_queue->impl && surface_queue->impl->need_detach)
			surface_queue->impl->need_detach(surface_queue, node);
		else
			_tbm_surface_queue_detach(surface_queue, surface);

		pthread_mutex_unlock(&surface_queue->lock);

		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_NONE;
	}

	if (surface_queue->impl && surface_queue->impl->release)
		surface_queue->impl->release(surface_queue, node);
	else
		_tbm_surface_queue_release(surface_queue, node, 1);

	if (_queue_is_empty(&surface_queue->free_queue)) {
		pthread_mutex_unlock(&surface_queue->lock);

		TBM_LOG_E("surface_queue->free_queue is empty.\n");
		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE;
	}

	node->type = QUEUE_NODE_TYPE_RELEASE;

	pthread_mutex_unlock(&surface_queue->lock);
	pthread_cond_signal(&surface_queue->free_cond);

	_tbm_surf_queue_mutex_unlock();

	_trace_emit(surface_queue, &surface_queue->trace_noti, surface, TBM_SURFACE_QUEUE_TRACE_RELEASE);

	_notify_emit(surface_queue, &surface_queue->dequeuable_noti);

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_acquire(tbm_surface_queue_h
			  surface_queue, tbm_surface_h *surface)
{
	queue_node *node;

	_tbm_surf_queue_mutex_lock();

	*surface = NULL;

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);
	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(surface != NULL,
			       TBM_SURFACE_QUEUE_ERROR_INVALID_SURFACE);

	pthread_mutex_lock(&surface_queue->lock);

	if (surface_queue->impl && surface_queue->impl->acquire)
		node = surface_queue->impl->acquire(surface_queue);
	else
		node = _tbm_surface_queue_acquire(surface_queue);

	if (node == NULL || node->surface == NULL) {
		TBM_LOG_E("_queue_node_pop_front failed\n");
		pthread_mutex_unlock(&surface_queue->lock);

		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE;
	}

	node->type = QUEUE_NODE_TYPE_ACQUIRE;

	*surface = node->surface;

	TBM_QUEUE_TRACE("tbm_surface_queue(%p) tbm_surface(%p)\n", surface_queue, *surface);

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	if (b_dump_queue)
		tbm_surface_internal_dump_buffer(*surface, "acquire");

	_trace_emit(surface_queue, &surface_queue->trace_noti, *surface, TBM_SURFACE_QUEUE_TRACE_ACQUIRE);

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

int
tbm_surface_queue_can_acquire(tbm_surface_queue_h surface_queue, int wait)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue), 0);

	pthread_mutex_lock(&surface_queue->lock);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	if (!_queue_is_empty(&surface_queue->dirty_queue)) {
		pthread_mutex_unlock(&surface_queue->lock);
		_tbm_surf_queue_mutex_unlock();
		return 1;
	}

	if (wait && _tbm_surface_queue_get_node_count(surface_queue,
						QUEUE_NODE_TYPE_DEQUEUE)) {
		_tbm_surf_queue_mutex_unlock();
		pthread_cond_wait(&surface_queue->dirty_cond, &surface_queue->lock);
		_tbm_surf_queue_mutex_lock();

		if (!_tbm_surface_queue_is_valid(surface_queue)) {
			TBM_LOG_E("surface_queue:%p is invalid", surface_queue);
			pthread_mutex_unlock(&surface_queue->lock);
			_tbm_surf_queue_mutex_unlock();
			return 0;
		}

		pthread_mutex_unlock(&surface_queue->lock);
		_tbm_surf_queue_mutex_unlock();
		return 1;
	}

	pthread_mutex_unlock(&surface_queue->lock);
	_tbm_surf_queue_mutex_unlock();
	return 0;
}

void
tbm_surface_queue_destroy(tbm_surface_queue_h surface_queue)
{
	queue_node *node = NULL, *tmp;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue));

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	LIST_DEL(&surface_queue->item_link);

	LIST_FOR_EACH_ENTRY_SAFE(node, tmp, &surface_queue->list, link)
		_queue_delete_node(surface_queue, node);

	if (surface_queue->impl && surface_queue->impl->destroy)
		surface_queue->impl->destroy(surface_queue);

	_notify_emit(surface_queue, &surface_queue->destory_noti);

	_notify_remove_all(&surface_queue->destory_noti);
	_notify_remove_all(&surface_queue->dequeuable_noti);
	_notify_remove_all(&surface_queue->dequeue_noti);
	_notify_remove_all(&surface_queue->can_dequeue_noti);
	_notify_remove_all(&surface_queue->acquirable_noti);
	_notify_remove_all(&surface_queue->reset_noti);
	_trace_remove_all(&surface_queue->trace_noti);

	pthread_mutex_destroy(&surface_queue->lock);

	free(surface_queue);

	if (LIST_IS_EMPTY(&g_surf_queue_bufmgr->surf_queue_list))
		_deinit_tbm_surf_queue_bufmgr();

	_tbm_surf_queue_mutex_unlock();
}

tbm_surface_queue_error_e
tbm_surface_queue_reset(tbm_surface_queue_h
			surface_queue, int width, int height, int format)
{
	queue_node *node = NULL, *tmp;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	if (width == surface_queue->width && height == surface_queue->height &&
		format == surface_queue->format) {
		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_NONE;
	}

	pthread_mutex_lock(&surface_queue->lock);

	surface_queue->width = width;
	surface_queue->height = height;
	surface_queue->format = format;

	/* Destory surface and Push to free_queue */
	LIST_FOR_EACH_ENTRY_SAFE(node, tmp, &surface_queue->list, link)
		_queue_delete_node(surface_queue, node);

	/* Reset queue */
	_queue_init(&surface_queue->free_queue);
	_queue_init(&surface_queue->dirty_queue);
	LIST_INITHEAD(&surface_queue->list);

	surface_queue->num_attached = 0;

	if (surface_queue->impl && surface_queue->impl->reset)
		surface_queue->impl->reset(surface_queue);

	pthread_mutex_unlock(&surface_queue->lock);
	pthread_cond_signal(&surface_queue->free_cond);

	_tbm_surf_queue_mutex_unlock();

	_notify_emit(surface_queue, &surface_queue->reset_noti);

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_notify_reset(tbm_surface_queue_h surface_queue)
{
	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	_tbm_surf_queue_mutex_unlock();

	_notify_emit(surface_queue, &surface_queue->reset_noti);

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_set_size(tbm_surface_queue_h
			surface_queue, int queue_size, int flush)
{
	queue_node *node = NULL, *tmp;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
					TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);
	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(queue_size > 0,
					TBM_SURFACE_QUEUE_ERROR_INVALID_PARAMETER);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	if ((surface_queue->queue_size == queue_size) && !flush) {
		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_NONE;
	}

	pthread_mutex_lock(&surface_queue->lock);

	if (flush) {
		/* Destory surface and Push to free_queue */
		LIST_FOR_EACH_ENTRY_SAFE(node, tmp, &surface_queue->list, link)
			_queue_delete_node(surface_queue, node);

		/* Reset queue */
		_queue_init(&surface_queue->free_queue);
		_queue_init(&surface_queue->dirty_queue);
		LIST_INITHEAD(&surface_queue->list);

		surface_queue->num_attached = 0;
		surface_queue->queue_size = queue_size;

		if (surface_queue->impl && surface_queue->impl->reset)
			surface_queue->impl->reset(surface_queue);

		pthread_mutex_unlock(&surface_queue->lock);
		pthread_cond_signal(&surface_queue->free_cond);

		_tbm_surf_queue_mutex_unlock();

		_notify_emit(surface_queue, &surface_queue->reset_noti);

		return TBM_SURFACE_QUEUE_ERROR_NONE;
	} else {
		if (surface_queue->queue_size > queue_size) {
			int need_del = surface_queue->queue_size - queue_size;

			LIST_FOR_EACH_ENTRY_SAFE(node, tmp, &surface_queue->free_queue.head, item_link) {
				TBM_QUEUE_TRACE("deatch tbm_surface_queue(%p) surface(%p)\n", surface_queue, node->surface);

				if (surface_queue->impl && surface_queue->impl->need_detach)
					surface_queue->impl->need_detach(surface_queue, node);
				else
					_tbm_surface_queue_detach(surface_queue, node->surface);

				need_del--;
				if (need_del == 0)
					break;
			}
		}

		surface_queue->queue_size = queue_size;

		pthread_mutex_unlock(&surface_queue->lock);

		_tbm_surf_queue_mutex_unlock();

		return TBM_SURFACE_QUEUE_ERROR_NONE;
	}
}

tbm_surface_queue_error_e
tbm_surface_queue_flush(tbm_surface_queue_h surface_queue)
{
	queue_node *node = NULL, *tmp;

	_tbm_surf_queue_mutex_lock();

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	if (surface_queue->num_attached == 0) {
		_tbm_surf_queue_mutex_unlock();
		return TBM_SURFACE_QUEUE_ERROR_NONE;
	}

	pthread_mutex_lock(&surface_queue->lock);

	/* Destory surface and Push to free_queue */
	LIST_FOR_EACH_ENTRY_SAFE(node, tmp, &surface_queue->list, link)
		_queue_delete_node(surface_queue, node);

	/* Reset queue */
	_queue_init(&surface_queue->free_queue);
	_queue_init(&surface_queue->dirty_queue);
	LIST_INITHEAD(&surface_queue->list);

	surface_queue->num_attached = 0;

	if (surface_queue->impl && surface_queue->impl->reset)
		surface_queue->impl->reset(surface_queue);

	pthread_mutex_unlock(&surface_queue->lock);
	pthread_cond_signal(&surface_queue->free_cond);

	_tbm_surf_queue_mutex_unlock();

	_notify_emit(surface_queue, &surface_queue->reset_noti);

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

tbm_surface_queue_error_e
tbm_surface_queue_get_surfaces(tbm_surface_queue_h surface_queue,
			tbm_surface_h *surfaces, int *num)
{
	queue_node *node = NULL;

	_tbm_surf_queue_mutex_lock();

	*num = 0;

	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(_tbm_surface_queue_is_valid(surface_queue),
			       TBM_SURFACE_QUEUE_ERROR_INVALID_QUEUE);
	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(num != NULL,
			       TBM_SURFACE_QUEUE_ERROR_INVALID_PARAMETER);

	pthread_mutex_lock(&surface_queue->lock);

	LIST_FOR_EACH_ENTRY(node, &surface_queue->list, link) {
		if (surfaces)
			surfaces[*num] = node->surface;

		*num = *num + 1;
	}

	pthread_mutex_unlock(&surface_queue->lock);

	_tbm_surf_queue_mutex_unlock();

	return TBM_SURFACE_QUEUE_ERROR_NONE;
}

typedef struct {
	int flags;
} tbm_queue_default;

static void
__tbm_queue_default_destroy(tbm_surface_queue_h surface_queue)
{
	free(surface_queue->impl_data);
}

static void
__tbm_queue_default_need_attach(tbm_surface_queue_h surface_queue)
{
	tbm_queue_default *data = (tbm_queue_default *)surface_queue->impl_data;
	tbm_surface_h surface;

	if (surface_queue->queue_size == surface_queue->num_attached)
		return;

	if (surface_queue->alloc_cb) {
		pthread_mutex_unlock(&surface_queue->lock);
		_tbm_surf_queue_mutex_unlock();
		surface = surface_queue->alloc_cb(surface_queue, surface_queue->alloc_cb_data);
		_tbm_surf_queue_mutex_lock();
		pthread_mutex_lock(&surface_queue->lock);

		/* silent return */
		if (!surface)
			return;

		tbm_surface_internal_ref(surface);
	} else {
		surface = tbm_surface_internal_create_with_flags(surface_queue->width,
				surface_queue->height,
				surface_queue->format,
				data->flags);
		TBM_RETURN_IF_FAIL(surface != NULL);
	}

	_tbm_surface_queue_attach(surface_queue, surface);
	tbm_surface_internal_unref(surface);
}

static const tbm_surface_queue_interface tbm_queue_default_impl = {
	NULL,				/*__tbm_queue_default_init*/
	NULL,				/*__tbm_queue_default_reset*/
	__tbm_queue_default_destroy,
	__tbm_queue_default_need_attach,
	NULL,				/*__tbm_queue_default_enqueue*/
	NULL,				/*__tbm_queue_default_release*/
	NULL,				/*__tbm_queue_default_dequeue*/
	NULL,				/*__tbm_queue_default_acquire*/
	NULL,				/*__tbm_queue_default_need_detach*/
};

tbm_surface_queue_h
tbm_surface_queue_create(int queue_size, int width,
			 int height, int format, int flags)
{
	TBM_RETURN_VAL_IF_FAIL(queue_size > 0, NULL);
	TBM_RETURN_VAL_IF_FAIL(width > 0, NULL);
	TBM_RETURN_VAL_IF_FAIL(height > 0, NULL);
	TBM_RETURN_VAL_IF_FAIL(format > 0, NULL);

	_tbm_surf_queue_mutex_lock();

	tbm_surface_queue_h surface_queue = (tbm_surface_queue_h) calloc(1,
					    sizeof(struct _tbm_surface_queue));
	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(surface_queue != NULL, NULL);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	tbm_queue_default *data = (tbm_queue_default *) calloc(1,
				  sizeof(tbm_queue_default));
	if (data == NULL) {
		TBM_LOG_E("cannot allocate the tbm_queue_default.\n");
		free(surface_queue);
		_tbm_surf_queue_mutex_unlock();
		return NULL;
	}

	data->flags = flags;
	_tbm_surface_queue_init(surface_queue,
				queue_size,
				width, height, format,
				&tbm_queue_default_impl, data);

	_tbm_surf_queue_mutex_unlock();

	return surface_queue;
}

typedef struct {
	int flags;
	queue dequeue_list;
} tbm_queue_sequence;

static void
__tbm_queue_sequence_init(tbm_surface_queue_h surface_queue)
{
	tbm_queue_sequence *data = (tbm_queue_sequence *)surface_queue->impl_data;

	_queue_init(&data->dequeue_list);
}

static void
__tbm_queue_sequence_reset(tbm_surface_queue_h surface_queue)
{
	tbm_queue_sequence *data = (tbm_queue_sequence *)surface_queue->impl_data;

	_queue_init(&data->dequeue_list);
}

static void
__tbm_queue_sequence_destroy(tbm_surface_queue_h surface_queue)
{
	free(surface_queue->impl_data);
}

static void
__tbm_queue_sequence_need_attach(tbm_surface_queue_h surface_queue)
{
	tbm_queue_sequence *data = (tbm_queue_sequence *)surface_queue->impl_data;
	tbm_surface_h surface;

	if (surface_queue->queue_size == surface_queue->num_attached)
		return;

	if (surface_queue->alloc_cb) {
		pthread_mutex_unlock(&surface_queue->lock);
		_tbm_surf_queue_mutex_unlock();
		surface = surface_queue->alloc_cb(surface_queue, surface_queue->alloc_cb_data);
		_tbm_surf_queue_mutex_lock();
		pthread_mutex_lock(&surface_queue->lock);

		/* silent return */
		if (!surface)
			return;

		tbm_surface_internal_ref(surface);
	} else {
		surface = tbm_surface_internal_create_with_flags(surface_queue->width,
				surface_queue->height,
				surface_queue->format,
				data->flags);
		TBM_RETURN_IF_FAIL(surface != NULL);
	}

	_tbm_surface_queue_attach(surface_queue, surface);
	tbm_surface_internal_unref(surface);
}

static void
__tbm_queue_sequence_enqueue(tbm_surface_queue_h surface_queue,
			     queue_node *node)
{
	tbm_queue_sequence *data = (tbm_queue_sequence *)surface_queue->impl_data;
	queue_node *next = NULL, *tmp;

	node->priv_flags = 0;

	LIST_FOR_EACH_ENTRY_SAFE(next, tmp, &data->dequeue_list.head, item_link) {
		if (next->priv_flags)
			break;
		_queue_node_pop(&data->dequeue_list, next);
		_tbm_surface_queue_enqueue(surface_queue, next, 1);
	}
}

static queue_node *
__tbm_queue_sequence_dequeue(tbm_surface_queue_h
			     surface_queue)
{
	tbm_queue_sequence *data = (tbm_queue_sequence *)surface_queue->impl_data;
	queue_node *node;

	node = _tbm_surface_queue_dequeue(surface_queue);
	if (node) {
		_queue_node_push_back(&data->dequeue_list, node);
		node->priv_flags = 1;
	}

	return node;
}

static const tbm_surface_queue_interface tbm_queue_sequence_impl = {
	__tbm_queue_sequence_init,
	__tbm_queue_sequence_reset,
	__tbm_queue_sequence_destroy,
	__tbm_queue_sequence_need_attach,
	__tbm_queue_sequence_enqueue,
	NULL,					/*__tbm_queue_sequence_release*/
	__tbm_queue_sequence_dequeue,
	NULL,					/*__tbm_queue_sequence_acquire*/
	NULL,					/*__tbm_queue_sequence_need_dettach*/
};

tbm_surface_queue_h
tbm_surface_queue_sequence_create(int queue_size, int width,
				  int height, int format, int flags)
{
	TBM_RETURN_VAL_IF_FAIL(queue_size > 0, NULL);
	TBM_RETURN_VAL_IF_FAIL(width > 0, NULL);
	TBM_RETURN_VAL_IF_FAIL(height > 0, NULL);
	TBM_RETURN_VAL_IF_FAIL(format > 0, NULL);

	_tbm_surf_queue_mutex_lock();

	tbm_surface_queue_h surface_queue = (tbm_surface_queue_h) calloc(1,
					    sizeof(struct _tbm_surface_queue));
	TBM_SURF_QUEUE_RETURN_VAL_IF_FAIL(surface_queue != NULL, NULL);

	TBM_QUEUE_TRACE("tbm_surface_queue(%p)\n", surface_queue);

	tbm_queue_sequence *data = (tbm_queue_sequence *) calloc(1,
				   sizeof(tbm_queue_sequence));
	if (data == NULL) {
		TBM_LOG_E("cannot allocate the tbm_queue_sequence.\n");
		free(surface_queue);
		_tbm_surf_queue_mutex_unlock();
		return NULL;
	}

	data->flags = flags;
	_tbm_surface_queue_init(surface_queue,
				queue_size,
				width, height, format,
				&tbm_queue_sequence_impl, data);

	_tbm_surf_queue_mutex_unlock();

	return surface_queue;
}
/* LCOV_EXCL_STOP */
