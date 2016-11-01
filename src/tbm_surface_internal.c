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

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "tbm_bufmgr.h"
#include "tbm_bufmgr_int.h"
#include "tbm_surface_internal.h"
#include "list.h"
#include <png.h>

static tbm_bufmgr g_surface_bufmgr;
static pthread_mutex_t tbm_surface_lock;
void _tbm_surface_mutex_unlock(void);

#define C(b, m)              (((b) >> (m)) & 0xFF)
#define B(c, s)              ((((unsigned int)(c)) & 0xff) << (s))
#define FOURCC(a, b, c, d)     (B(d, 24) | B(c, 16) | B(b, 8) | B(a, 0))
#define FOURCC_STR(id)      C(id, 0), C(id, 8), C(id, 16), C(id, 24)
#define FOURCC_ID(str)      FOURCC(((char*)str)[0], ((char*)str)[1], ((char*)str)[2], ((char*)str)[3])

/* check condition */
#define TBM_SURFACE_RETURN_IF_FAIL(cond) {\
	if (!(cond)) {\
		TBM_LOG_E("'%s' failed.\n", #cond);\
		_tbm_surface_mutex_unlock();\
		return;\
	} \
}

#define TBM_SURFACE_RETURN_VAL_IF_FAIL(cond, val) {\
	if (!(cond)) {\
		TBM_LOG_E("'%s' failed.\n", #cond);\
		_tbm_surface_mutex_unlock();\
		return val;\
	} \
}

/* LCOV_EXCL_START */

static double
_tbm_surface_internal_get_time(void)
{
	struct timespec tp;
	unsigned int time;

	clock_gettime(CLOCK_MONOTONIC, &tp);
	time = (tp.tv_sec * 1000000L) + (tp.tv_nsec / 1000);

	return time / 1000.0;
}

static void
_tbm_surface_internal_debug_data_delete(tbm_surface_debug_data *debug_data)
{
	LIST_DEL(&debug_data->item_link);

	if (debug_data->key) free(debug_data->key);
	if (debug_data->value) free(debug_data->value);
	free(debug_data);
}

char *
_tbm_surface_internal_format_to_str(tbm_format format)
{
	switch (format) {
	case TBM_FORMAT_C8:
		return "TBM_FORMAT_C8";
	case TBM_FORMAT_RGB332:
		return "TBM_FORMAT_RGB332";
	case TBM_FORMAT_BGR233:
		return "TBM_FORMAT_BGR233";
	case TBM_FORMAT_XRGB4444:
		return "TBM_FORMAT_XRGB4444";
	case TBM_FORMAT_XBGR4444:
		return "TBM_FORMAT_XBGR4444";
	case TBM_FORMAT_RGBX4444:
		return "TBM_FORMAT_RGBX4444";
	case TBM_FORMAT_BGRX4444:
		return "TBM_FORMAT_BGRX4444";
	case TBM_FORMAT_ARGB4444:
		return "TBM_FORMAT_ARGB4444";
	case TBM_FORMAT_ABGR4444:
		return "TBM_FORMAT_ABGR4444";
	case TBM_FORMAT_RGBA4444:
		return "TBM_FORMAT_RGBA4444";
	case TBM_FORMAT_BGRA4444:
		return "TBM_FORMAT_BGRA4444";
	case TBM_FORMAT_XRGB1555:
		return "TBM_FORMAT_XRGB1555";
	case TBM_FORMAT_XBGR1555:
		return "TBM_FORMAT_XBGR1555";
	case TBM_FORMAT_RGBX5551:
		return "TBM_FORMAT_RGBX5551";
	case TBM_FORMAT_BGRX5551:
		return "TBM_FORMAT_BGRX5551";
	case TBM_FORMAT_ARGB1555:
		return "TBM_FORMAT_ARGB1555";
	case TBM_FORMAT_ABGR1555:
		return "TBM_FORMAT_ABGR1555";
	case TBM_FORMAT_RGBA5551:
		return "TBM_FORMAT_RGBA5551";
	case TBM_FORMAT_BGRA5551:
		return "TBM_FORMAT_BGRA5551";
	case TBM_FORMAT_RGB565:
		return "TBM_FORMAT_RGB565";
	case TBM_FORMAT_BGR565:
		return "TBM_FORMAT_BGR565";
	case TBM_FORMAT_RGB888:
		return "TBM_FORMAT_RGB888";
	case TBM_FORMAT_BGR888:
		return "TBM_FORMAT_BGR888";
	case TBM_FORMAT_XRGB8888:
		return "TBM_FORMAT_XRGB8888";
	case TBM_FORMAT_XBGR8888:
		return "TBM_FORMAT_XBGR8888";
	case TBM_FORMAT_RGBX8888:
		return "TBM_FORMAT_RGBX8888";
	case TBM_FORMAT_BGRX8888:
		return "TBM_FORMAT_BGRX8888";
	case TBM_FORMAT_ARGB8888:
		return "TBM_FORMAT_ARGB8888";
	case TBM_FORMAT_ABGR8888:
		return "TBM_FORMAT_ABGR8888";
	case TBM_FORMAT_RGBA8888:
		return "TBM_FORMAT_RGBA8888";
	case TBM_FORMAT_BGRA8888:
		return "TBM_FORMAT_BGRA8888";
	case TBM_FORMAT_XRGB2101010:
		return "TBM_FORMAT_XRGB2101010";
	case TBM_FORMAT_XBGR2101010:
		return "TBM_FORMAT_XBGR2101010";
	case TBM_FORMAT_RGBX1010102:
		return "TBM_FORMAT_RGBX1010102";
	case TBM_FORMAT_BGRX1010102:
		return "TBM_FORMAT_BGRX1010102";
	case TBM_FORMAT_ARGB2101010:
		return "TBM_FORMAT_ARGB2101010";
	case TBM_FORMAT_ABGR2101010:
		return "TBM_FORMAT_ABGR2101010";
	case TBM_FORMAT_RGBA1010102:
		return "TBM_FORMAT_RGBA1010102";
	case TBM_FORMAT_BGRA1010102:
		return "TBM_FORMAT_BGRA1010102";
	case TBM_FORMAT_YUYV:
		return "TBM_FORMAT_YUYV";
	case TBM_FORMAT_YVYU:
		return "TBM_FORMAT_YVYU";
	case TBM_FORMAT_UYVY:
		return "TBM_FORMAT_UYVY";
	case TBM_FORMAT_VYUY:
		return "TBM_FORMAT_VYUY";
	case TBM_FORMAT_AYUV:
		return "TBM_FORMAT_AYUV";
	case TBM_FORMAT_NV12:
		return "TBM_FORMAT_NV12";
	case TBM_FORMAT_NV21:
		return "TBM_FORMAT_NV21";
	case TBM_FORMAT_NV16:
		return "TBM_FORMAT_NV16";
	case TBM_FORMAT_NV61:
		return "TBM_FORMAT_NV61";
	case TBM_FORMAT_YUV410:
		return "TBM_FORMAT_YUV410";
	case TBM_FORMAT_YVU410:
		return "TBM_FORMAT_YVU410";
	case TBM_FORMAT_YUV411:
		return "TBM_FORMAT_YUV411";
	case TBM_FORMAT_YVU411:
		return "TBM_FORMAT_YVU411";
	case TBM_FORMAT_YUV420:
		return "TBM_FORMAT_YUV420";
	case TBM_FORMAT_YVU420:
		return "TBM_FORMAT_YVU420";
	case TBM_FORMAT_YUV422:
		return "TBM_FORMAT_YUV422";
	case TBM_FORMAT_YVU422:
		return "TBM_FORMAT_YVU422";
	case TBM_FORMAT_YUV444:
		return "TBM_FORMAT_YUV444";
	case TBM_FORMAT_YVU444:
		return "TBM_FORMAT_YVU444";
	case TBM_FORMAT_NV12MT:
		return "TBM_FORMAT_NV12MT";
	default:
		return "unknwon";
	}
}
/* LCOV_EXCL_STOP */

static bool
_tbm_surface_mutex_init(void)
{
	static bool tbm_surface_mutex_init = false;

	if (tbm_surface_mutex_init)
		return true;

	if (pthread_mutex_init(&tbm_surface_lock, NULL)) {
		TBM_LOG_E("fail: tbm_surface mutex init\n");
		return false;
	}

	tbm_surface_mutex_init = true;

	return true;
}

void
_tbm_surface_mutex_lock(void)
{
	if (!_tbm_surface_mutex_init())
		return;

	pthread_mutex_lock(&tbm_surface_lock);
}

void
_tbm_surface_mutex_unlock(void)
{
	pthread_mutex_unlock(&tbm_surface_lock);
}

static void
_init_surface_bufmgr(void)
{
	g_surface_bufmgr = tbm_bufmgr_init(-1);
}

static void
_deinit_surface_bufmgr(void)
{
	if (!g_surface_bufmgr)
		return;

	tbm_bufmgr_deinit(g_surface_bufmgr);
	g_surface_bufmgr = NULL;
}

static int
_tbm_surface_internal_query_plane_data(tbm_surface_h surface,
				       int plane_idx, uint32_t *size, uint32_t *offset, uint32_t *pitch, int *bo_idx)
{
	TBM_RETURN_VAL_IF_FAIL(surface, 0);
	TBM_RETURN_VAL_IF_FAIL(plane_idx > -1, 0);

	struct _tbm_surface *surf = (struct _tbm_surface *)surface;
	struct _tbm_bufmgr *mgr = surf->bufmgr;
	int ret = 0;

	TBM_RETURN_VAL_IF_FAIL(mgr != NULL, 0);
	TBM_RETURN_VAL_IF_FAIL(surf->info.width > 0, 0);
	TBM_RETURN_VAL_IF_FAIL(surf->info.height > 0, 0);
	TBM_RETURN_VAL_IF_FAIL(surf->info.format > 0, 0);

	if (!mgr->backend->surface_get_plane_data)
		return 0;

	ret = mgr->backend->surface_get_plane_data(surf->info.width,
			surf->info.height, surf->info.format, plane_idx, size, offset, pitch, bo_idx);
	if (!ret)
		return 0;

	return 1;
}

static void
_tbm_surface_internal_destroy(tbm_surface_h surface)
{
	int i;
	tbm_bufmgr bufmgr = surface->bufmgr;
	tbm_user_data *old_data = NULL, *tmp = NULL;
	tbm_surface_debug_data *debug_old_data = NULL, *debug_tmp = NULL;

	/* destory the user_data_list */
	if (!LIST_IS_EMPTY(&surface->user_data_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(old_data, tmp, &surface->user_data_list, item_link) {
			TBM_DBG("free user_data\n");
			user_data_delete(old_data);
		}
	}

	for (i = 0; i < surface->num_bos; i++) {
		surface->bos[i]->surface = NULL;

		tbm_bo_unref(surface->bos[i]);
		surface->bos[i] = NULL;
	}

	if (!LIST_IS_EMPTY(&surface->debug_data_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(debug_old_data, debug_tmp, &surface->debug_data_list, item_link)
			_tbm_surface_internal_debug_data_delete(debug_old_data);
	}

	LIST_DEL(&surface->item_link);

	free(surface);
	surface = NULL;

	if (LIST_IS_EMPTY(&bufmgr->surf_list)) {
		LIST_DELINIT(&bufmgr->surf_list);

		if (!LIST_IS_EMPTY(&bufmgr->debug_key_list)) {
			LIST_FOR_EACH_ENTRY_SAFE(debug_old_data, debug_tmp, &bufmgr->debug_key_list, item_link) {
				_tbm_surface_internal_debug_data_delete(debug_old_data);
			}
		}

		_deinit_surface_bufmgr();
	}
}

int
tbm_surface_internal_is_valid(tbm_surface_h surface)
{
	tbm_surface_h old_data = NULL;

	if (surface == NULL || g_surface_bufmgr == NULL) {
		TBM_TRACE("error: tbm_surface(%p)\n", surface);
		return 0;
	}

	if (!LIST_IS_EMPTY(&g_surface_bufmgr->surf_list)) {
		LIST_FOR_EACH_ENTRY(old_data, &g_surface_bufmgr->surf_list, item_link) {
			if (old_data == surface) {
				TBM_TRACE("tbm_surface(%p)\n", surface);
				return 1;
			}
		}
	}
	TBM_TRACE("error: tbm_surface(%p)\n", surface);
	return 0;
}

int
tbm_surface_internal_query_supported_formats(uint32_t **formats,
		uint32_t *num)
{
	struct _tbm_bufmgr *mgr;
	int ret = 0;

	_tbm_surface_mutex_lock();

	if (!g_surface_bufmgr) {
		_init_surface_bufmgr();
		LIST_INITHEAD(&g_surface_bufmgr->surf_list);
	}

	mgr = g_surface_bufmgr;

	if (!mgr->backend->surface_supported_format) {
		TBM_TRACE("error: tbm_bufmgr(%p)\n", g_surface_bufmgr);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	ret = mgr->backend->surface_supported_format(formats, num);

	TBM_TRACE("tbm_bufmgr(%p) format num(%d)\n", g_surface_bufmgr, *num);

	_tbm_surface_mutex_unlock();

	return ret;
}

int
tbm_surface_internal_get_num_planes(tbm_format format)
{
	int num_planes = 0;

	switch (format) {
	case TBM_FORMAT_C8:
	case TBM_FORMAT_RGB332:
	case TBM_FORMAT_BGR233:
	case TBM_FORMAT_XRGB4444:
	case TBM_FORMAT_XBGR4444:
	case TBM_FORMAT_RGBX4444:
	case TBM_FORMAT_BGRX4444:
	case TBM_FORMAT_ARGB4444:
	case TBM_FORMAT_ABGR4444:
	case TBM_FORMAT_RGBA4444:
	case TBM_FORMAT_BGRA4444:
	case TBM_FORMAT_XRGB1555:
	case TBM_FORMAT_XBGR1555:
	case TBM_FORMAT_RGBX5551:
	case TBM_FORMAT_BGRX5551:
	case TBM_FORMAT_ARGB1555:
	case TBM_FORMAT_ABGR1555:
	case TBM_FORMAT_RGBA5551:
	case TBM_FORMAT_BGRA5551:
	case TBM_FORMAT_RGB565:
	case TBM_FORMAT_BGR565:
	case TBM_FORMAT_RGB888:
	case TBM_FORMAT_BGR888:
	case TBM_FORMAT_XRGB8888:
	case TBM_FORMAT_XBGR8888:
	case TBM_FORMAT_RGBX8888:
	case TBM_FORMAT_BGRX8888:
	case TBM_FORMAT_ARGB8888:
	case TBM_FORMAT_ABGR8888:
	case TBM_FORMAT_RGBA8888:
	case TBM_FORMAT_BGRA8888:
	case TBM_FORMAT_XRGB2101010:
	case TBM_FORMAT_XBGR2101010:
	case TBM_FORMAT_RGBX1010102:
	case TBM_FORMAT_BGRX1010102:
	case TBM_FORMAT_ARGB2101010:
	case TBM_FORMAT_ABGR2101010:
	case TBM_FORMAT_RGBA1010102:
	case TBM_FORMAT_BGRA1010102:
	case TBM_FORMAT_YUYV:
	case TBM_FORMAT_YVYU:
	case TBM_FORMAT_UYVY:
	case TBM_FORMAT_VYUY:
	case TBM_FORMAT_AYUV:
		num_planes = 1;
		break;
	case TBM_FORMAT_NV12:
	case TBM_FORMAT_NV12MT:
	case TBM_FORMAT_NV21:
	case TBM_FORMAT_NV16:
	case TBM_FORMAT_NV61:
		num_planes = 2;
		break;
	case TBM_FORMAT_YUV410:
	case TBM_FORMAT_YVU410:
	case TBM_FORMAT_YUV411:
	case TBM_FORMAT_YVU411:
	case TBM_FORMAT_YUV420:
	case TBM_FORMAT_YVU420:
	case TBM_FORMAT_YUV422:
	case TBM_FORMAT_YVU422:
	case TBM_FORMAT_YUV444:
	case TBM_FORMAT_YVU444:
		num_planes = 3;
		break;

	default:
		break;
	}

	TBM_TRACE("tbm_format(%s) num_planes(%d)\n", _tbm_surface_internal_format_to_str(format), num_planes);

	return num_planes;
}

int
tbm_surface_internal_get_bpp(tbm_format format)
{

	int bpp = 0;

	switch (format) {
	case TBM_FORMAT_C8:
	case TBM_FORMAT_RGB332:
	case TBM_FORMAT_BGR233:
		bpp = 8;
		break;
	case TBM_FORMAT_XRGB4444:
	case TBM_FORMAT_XBGR4444:
	case TBM_FORMAT_RGBX4444:
	case TBM_FORMAT_BGRX4444:
	case TBM_FORMAT_ARGB4444:
	case TBM_FORMAT_ABGR4444:
	case TBM_FORMAT_RGBA4444:
	case TBM_FORMAT_BGRA4444:
	case TBM_FORMAT_XRGB1555:
	case TBM_FORMAT_XBGR1555:
	case TBM_FORMAT_RGBX5551:
	case TBM_FORMAT_BGRX5551:
	case TBM_FORMAT_ARGB1555:
	case TBM_FORMAT_ABGR1555:
	case TBM_FORMAT_RGBA5551:
	case TBM_FORMAT_BGRA5551:
	case TBM_FORMAT_RGB565:
	case TBM_FORMAT_BGR565:
		bpp = 16;
		break;
	case TBM_FORMAT_RGB888:
	case TBM_FORMAT_BGR888:
		bpp = 24;
		break;
	case TBM_FORMAT_XRGB8888:
	case TBM_FORMAT_XBGR8888:
	case TBM_FORMAT_RGBX8888:
	case TBM_FORMAT_BGRX8888:
	case TBM_FORMAT_ARGB8888:
	case TBM_FORMAT_ABGR8888:
	case TBM_FORMAT_RGBA8888:
	case TBM_FORMAT_BGRA8888:
	case TBM_FORMAT_XRGB2101010:
	case TBM_FORMAT_XBGR2101010:
	case TBM_FORMAT_RGBX1010102:
	case TBM_FORMAT_BGRX1010102:
	case TBM_FORMAT_ARGB2101010:
	case TBM_FORMAT_ABGR2101010:
	case TBM_FORMAT_RGBA1010102:
	case TBM_FORMAT_BGRA1010102:
	case TBM_FORMAT_YUYV:
	case TBM_FORMAT_YVYU:
	case TBM_FORMAT_UYVY:
	case TBM_FORMAT_VYUY:
	case TBM_FORMAT_AYUV:
		bpp = 32;
		break;
	case TBM_FORMAT_NV12:
	case TBM_FORMAT_NV12MT:
	case TBM_FORMAT_NV21:
		bpp = 12;
		break;
	case TBM_FORMAT_NV16:
	case TBM_FORMAT_NV61:
		bpp = 16;
		break;
	case TBM_FORMAT_YUV410:
	case TBM_FORMAT_YVU410:
		bpp = 9;
		break;
	case TBM_FORMAT_YUV411:
	case TBM_FORMAT_YVU411:
	case TBM_FORMAT_YUV420:
	case TBM_FORMAT_YVU420:
		bpp = 12;
		break;
	case TBM_FORMAT_YUV422:
	case TBM_FORMAT_YVU422:
		bpp = 16;
		break;
	case TBM_FORMAT_YUV444:
	case TBM_FORMAT_YVU444:
		bpp = 24;
		break;
	default:
		break;
	}

	TBM_TRACE("tbm_format(%s) bpp(%d)\n", _tbm_surface_internal_format_to_str(format), bpp);

	return bpp;
}

tbm_surface_h
tbm_surface_internal_create_with_flags(int width, int height,
				       int format, int flags)
{
	TBM_RETURN_VAL_IF_FAIL(width > 0, NULL);
	TBM_RETURN_VAL_IF_FAIL(height > 0, NULL);

	struct _tbm_bufmgr *mgr;
	struct _tbm_surface *surf = NULL;
	uint32_t size = 0;
	uint32_t offset = 0;
	uint32_t stride = 0;
	uint32_t bo_size = 0;
	int bo_idx;
	int i, j;

	_tbm_surface_mutex_lock();

	if (!g_surface_bufmgr) {
		_init_surface_bufmgr();
		LIST_INITHEAD(&g_surface_bufmgr->surf_list);
	}

	mgr = g_surface_bufmgr;
	if (!TBM_BUFMGR_IS_VALID(mgr)) {
		TBM_TRACE("error: width(%d) height(%d) format(%s) flags(%d)\n",
				width, height, _tbm_surface_internal_format_to_str(format), flags);
		_tbm_surface_mutex_unlock();
		return NULL;
	}
	surf = calloc(1, sizeof(struct _tbm_surface));
	if (!surf) {
		TBM_TRACE("error: width(%d) height(%d) format(%s) flags(%d)\n",
				width, height, _tbm_surface_internal_format_to_str(format), flags);
		_tbm_surface_mutex_unlock();
		return NULL;
	}

	surf->bufmgr = mgr;
	surf->info.width = width;
	surf->info.height = height;
	surf->info.format = format;
	surf->info.bpp = tbm_surface_internal_get_bpp(format);
	surf->info.num_planes = tbm_surface_internal_get_num_planes(format);
	surf->refcnt = 1;

	/* get size, stride and offset bo_idx */
	for (i = 0; i < surf->info.num_planes; i++) {
		_tbm_surface_internal_query_plane_data(surf, i, &size, &offset, &stride,
						       &bo_idx);
		surf->info.planes[i].size = size;
		surf->info.planes[i].offset = offset;
		surf->info.planes[i].stride = stride;
		surf->planes_bo_idx[i] = bo_idx;
	}

	surf->num_bos = 1;

	for (i = 0; i < surf->info.num_planes; i++) {
		surf->info.size += surf->info.planes[i].size;

		if (surf->num_bos < surf->planes_bo_idx[i] + 1)
			surf->num_bos = surf->planes_bo_idx[i] + 1;
	}

	surf->flags = flags;

	for (i = 0; i < surf->num_bos; i++) {
		bo_size = 0;
		for (j = 0; j < surf->info.num_planes; j++) {
			if (surf->planes_bo_idx[j] == i)
				bo_size += surf->info.planes[j].size;
		}

		if (mgr->backend->surface_bo_alloc) {
			/* LCOV_EXCL_START */
			tbm_bo bo = NULL;
			void *bo_priv = NULL;

			bo = calloc(1, sizeof(struct _tbm_bo));
			if (!bo) {
				TBM_LOG_E("fail to alloc bo struct\n");
				goto alloc_fail;
			}

			bo->bufmgr = surf->bufmgr;

			pthread_mutex_lock(&surf->bufmgr->lock);

			bo_priv = mgr->backend->surface_bo_alloc(bo, width, height, format, flags, i);
			if (!bo_priv) {
				TBM_LOG_E("fail to alloc bo priv\n");
				free(bo);
				pthread_mutex_unlock(&surf->bufmgr->lock);
				goto alloc_fail;
			}

			bo->ref_cnt = 1;
			bo->flags = flags;
			bo->priv = bo_priv;

			LIST_INITHEAD(&bo->user_data_list);

			LIST_ADD(&bo->item_link, &surf->bufmgr->bo_list);

			pthread_mutex_unlock(&surf->bufmgr->lock);

			surf->bos[i] = bo;
			/* LCOV_EXCL_STOP */
		} else {
			surf->bos[i] = tbm_bo_alloc(mgr, bo_size, flags);
		}

		if (!surf->bos[i]) {
			TBM_LOG_E("fail to alloc bo idx:%d\n", i);
			goto alloc_fail;
		}

		_tbm_bo_set_surface(surf->bos[i], surf);

	}

	TBM_TRACE("width(%d) height(%d) format(%s) flags(%d) tbm_surface(%p)\n", width, height,
			_tbm_surface_internal_format_to_str(format), flags, surf);

	LIST_INITHEAD(&surf->user_data_list);
	LIST_INITHEAD(&surf->debug_data_list);

	LIST_ADD(&surf->item_link, &mgr->surf_list);

	_tbm_surface_mutex_unlock();

	return surf;

alloc_fail:

	TBM_TRACE("error: width(%d) height(%d) format(%s) flags(%d)\n",
			width, height, _tbm_surface_internal_format_to_str(format), flags);

	for (j = 0; j < i; j++) {
		if (surf->bos[j])
			tbm_bo_unref(surf->bos[j]);
	}

	free(surf);
	surf = NULL;

	if (LIST_IS_EMPTY(&mgr->surf_list)) {
		LIST_DELINIT(&mgr->surf_list);
		_deinit_surface_bufmgr();
	}

	_tbm_surface_mutex_unlock();
	return NULL;
}

tbm_surface_h
tbm_surface_internal_create_with_bos(tbm_surface_info_s *info,
				     tbm_bo *bos, int num)
{
	TBM_RETURN_VAL_IF_FAIL(bos, NULL);
	TBM_RETURN_VAL_IF_FAIL(info, NULL);
	TBM_RETURN_VAL_IF_FAIL(num == 1 || info->num_planes == num, NULL);

	struct _tbm_bufmgr *mgr;
	struct _tbm_surface *surf = NULL;
	int i;

	_tbm_surface_mutex_lock();

	if (!g_surface_bufmgr) {
		_init_surface_bufmgr();
		LIST_INITHEAD(&g_surface_bufmgr->surf_list);
	}

	mgr = g_surface_bufmgr;
	if (!TBM_BUFMGR_IS_VALID(mgr)) {
		TBM_TRACE("error: width(%d) height(%d) format(%s) bo_num(%d)\n",
				info->width, info->height, _tbm_surface_internal_format_to_str(info->format), num);
		_tbm_surface_mutex_unlock();
		return NULL;
	}

	surf = calloc(1, sizeof(struct _tbm_surface));
	if (!surf) {
		TBM_TRACE("error: width(%d) height(%d) format(%s) bo_num(%d)\n",
				info->width, info->height, _tbm_surface_internal_format_to_str(info->format), num);
		_tbm_surface_mutex_unlock();
		return NULL;
	}

	surf->bufmgr = mgr;
	surf->info.width = info->width;
	surf->info.height = info->height;
	surf->info.format = info->format;
	surf->info.bpp = info->bpp;
	surf->info.num_planes = info->num_planes;
	surf->refcnt = 1;

	/* get size, stride and offset */
	for (i = 0; i < info->num_planes; i++) {
		surf->info.planes[i].offset = info->planes[i].offset;
		surf->info.planes[i].stride = info->planes[i].stride;

		if (info->planes[i].size > 0)
			surf->info.planes[i].size = info->planes[i].size;
		else
			surf->info.planes[i].size += surf->info.planes[i].stride * info->height;

		if (num == 1)
			surf->planes_bo_idx[i] = 0;
		else
			surf->planes_bo_idx[i] = i;
	}

	if (info->size > 0) {
		surf->info.size = info->size;
	} else {
		surf->info.size = 0;
		for (i = 0; i < info->num_planes; i++)
			surf->info.size += surf->info.planes[i].size;
	}

	surf->flags = TBM_BO_DEFAULT;

	/* create only one bo */
	surf->num_bos = num;
	for (i = 0; i < num; i++) {
		if (bos[i] == NULL)
			goto bail1;

		surf->bos[i] = tbm_bo_ref(bos[i]);
		_tbm_bo_set_surface(bos[i], surf);
	}

	TBM_TRACE("tbm_surface(%p) width(%d) height(%d) format(%s) bo_num(%d)\n", surf,
			info->width, info->height, _tbm_surface_internal_format_to_str(info->format), num);

	LIST_INITHEAD(&surf->user_data_list);
	LIST_INITHEAD(&surf->debug_data_list);

	LIST_ADD(&surf->item_link, &mgr->surf_list);

	_tbm_surface_mutex_unlock();

	return surf;
bail1:
	TBM_TRACE("error: width(%d) height(%d) format(%s) bo_num(%d)\n",
				info->width, info->height, _tbm_surface_internal_format_to_str(info->format), num);
	for (i = 0; i < num; i++) {
		if (surf->bos[i]) {
			tbm_bo_unref(surf->bos[i]);
			surf->bos[i] = NULL;
		}
	}

	free(surf);
	surf = NULL;

	if (LIST_IS_EMPTY(&g_surface_bufmgr->surf_list)) {
		LIST_DELINIT(&g_surface_bufmgr->surf_list);
		_deinit_surface_bufmgr();
	}

	_tbm_surface_mutex_unlock();

	return NULL;
}

void
tbm_surface_internal_destroy(tbm_surface_h surface)
{
	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_IF_FAIL(tbm_surface_internal_is_valid(surface));

	surface->refcnt--;

	if (surface->refcnt > 0) {
		TBM_TRACE("reduce a refcnt(%d) of tbm_surface(%p)\n", surface->refcnt, surface);
		_tbm_surface_mutex_unlock();
		return;
	}

	TBM_TRACE("destroy tbm_surface(%p) refcnt(%d)\n", surface, surface->refcnt);

	if (surface->refcnt == 0)
		_tbm_surface_internal_destroy(surface);

	_tbm_surface_mutex_unlock();
}

void
tbm_surface_internal_ref(tbm_surface_h surface)
{
	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_IF_FAIL(tbm_surface_internal_is_valid(surface));

	surface->refcnt++;

	TBM_TRACE("tbm_surface(%p) refcnt(%d)\n", surface, surface->refcnt);

	_tbm_surface_mutex_unlock();
}

void
tbm_surface_internal_unref(tbm_surface_h surface)
{
	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_IF_FAIL(tbm_surface_internal_is_valid(surface));

	surface->refcnt--;

	if (surface->refcnt > 0) {
		TBM_TRACE("reduce a refcnt(%d) of tbm_surface(%p)\n", surface->refcnt, surface);
		_tbm_surface_mutex_unlock();
		return;
	}

	TBM_TRACE("destroy tbm_surface(%p) refcnt(%d)\n", surface, surface->refcnt);

	if (surface->refcnt == 0)
		_tbm_surface_internal_destroy(surface);

	_tbm_surface_mutex_unlock();
}

int
tbm_surface_internal_get_num_bos(tbm_surface_h surface)
{
	struct _tbm_surface *surf;
	int num;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	surf = (struct _tbm_surface *)surface;
	num = surf->num_bos;

	TBM_TRACE("tbm_surface(%p) num_bos(%d)\n", surface, num);

	_tbm_surface_mutex_unlock();

	return num;
}

tbm_bo
tbm_surface_internal_get_bo(tbm_surface_h surface, int bo_idx)
{
	struct _tbm_surface *surf;
	tbm_bo bo;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), NULL);
	TBM_SURFACE_RETURN_VAL_IF_FAIL(bo_idx > -1, NULL);

	surf = (struct _tbm_surface *)surface;
	bo = surf->bos[bo_idx];

	TBM_TRACE("tbm_surface(%p) bo_idx(%d) tbm_bo(%p)\n", surface, bo_idx, bo);

	_tbm_surface_mutex_unlock();

	return bo;
}

int
tbm_surface_internal_get_size(tbm_surface_h surface)
{
	struct _tbm_surface *surf;
	unsigned int size;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	surf = (struct _tbm_surface *)surface;
	size = surf->info.size;

	TBM_TRACE("tbm_surface(%p) size(%d)\n", surface, size);

	_tbm_surface_mutex_unlock();

	return size;
}

int
tbm_surface_internal_get_plane_data(tbm_surface_h surface, int plane_idx,
				    uint32_t *size, uint32_t *offset, uint32_t *pitch)
{
	struct _tbm_surface *surf;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);
	TBM_SURFACE_RETURN_VAL_IF_FAIL(plane_idx > -1, 0);

	surf = (struct _tbm_surface *)surface;

	if (plane_idx >= surf->info.num_planes) {
		TBM_TRACE("error: tbm_surface(%p) plane_idx(%d)\n", surface, plane_idx);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	if (size)
		*size = surf->info.planes[plane_idx].size;

	if (offset)
		*offset = surf->info.planes[plane_idx].offset;

	if (pitch)
		*pitch = surf->info.planes[plane_idx].stride;

	TBM_TRACE("tbm_surface(%p) plane_idx(%d) size(%d) offset(%d) pitch(%d)\n", surface, plane_idx,
				surf->info.planes[plane_idx].size, surf->info.planes[plane_idx].offset,
				surf->info.planes[plane_idx].stride);

	_tbm_surface_mutex_unlock();

	return 1;
}

int
tbm_surface_internal_get_info(tbm_surface_h surface, int opt,
			      tbm_surface_info_s *info, int map)
{
	struct _tbm_surface *surf;
	tbm_bo_handle bo_handles[4];
	int i, j;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	memset(bo_handles, 0, sizeof(tbm_bo_handle) * 4);

	surf = (struct _tbm_surface *)surface;

	memset(info, 0x00, sizeof(tbm_surface_info_s));
	info->width = surf->info.width;
	info->height = surf->info.height;
	info->format = surf->info.format;
	info->bpp = surf->info.bpp;
	info->size = surf->info.size;
	info->num_planes = surf->info.num_planes;

	if (map == 1) {
		for (i = 0; i < surf->num_bos; i++) {
			bo_handles[i] = tbm_bo_map(surf->bos[i], TBM_DEVICE_CPU, opt);
			if (bo_handles[i].ptr == NULL) {
				for (j = 0; j < i; j++)
					tbm_bo_unmap(surf->bos[j]);

				TBM_TRACE("error: tbm_surface(%p) opt(%d) map(%d)\n", surface, opt, map);
				_tbm_surface_mutex_unlock();
				return 0;
			}
		}
	} else {
		for (i = 0; i < surf->num_bos; i++)
			bo_handles[i] = tbm_bo_get_handle(surf->bos[i], TBM_DEVICE_CPU);
	}

	for (i = 0; i < surf->info.num_planes; i++) {
		info->planes[i].size = surf->info.planes[i].size;
		info->planes[i].offset = surf->info.planes[i].offset;
		info->planes[i].stride = surf->info.planes[i].stride;

		if (bo_handles[surf->planes_bo_idx[i]].ptr)
			info->planes[i].ptr = bo_handles[surf->planes_bo_idx[i]].ptr +
					      surf->info.planes[i].offset;
	}

	TBM_TRACE("tbm_surface(%p) opt(%d) map(%d)\n", surface, opt, map);

	_tbm_surface_mutex_unlock();

	return 1;
}

void
tbm_surface_internal_unmap(tbm_surface_h surface)
{
	struct _tbm_surface *surf;
	int i;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_IF_FAIL(tbm_surface_internal_is_valid(surface));

	surf = (struct _tbm_surface *)surface;

	for (i = 0; i < surf->num_bos; i++)
		tbm_bo_unmap(surf->bos[i]);

	TBM_TRACE("tbm_surface(%p)\n", surface);

	_tbm_surface_mutex_unlock();
}

unsigned int
tbm_surface_internal_get_width(tbm_surface_h surface)
{
	struct _tbm_surface *surf;
	unsigned int width;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	surf = (struct _tbm_surface *)surface;
	width = surf->info.width;

	TBM_TRACE("tbm_surface(%p) width(%d)\n", surface, width);

	_tbm_surface_mutex_unlock();

	return width;
}

unsigned int
tbm_surface_internal_get_height(tbm_surface_h surface)
{
	struct _tbm_surface *surf;
	unsigned int height;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	surf = (struct _tbm_surface *)surface;
	height = surf->info.height;

	TBM_TRACE("tbm_surface(%p) height(%d)\n", surface, height);

	_tbm_surface_mutex_unlock();

	return height;

}

tbm_format
tbm_surface_internal_get_format(tbm_surface_h surface)
{
	struct _tbm_surface *surf;
	tbm_format format;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	surf = (struct _tbm_surface *)surface;
	format = surf->info.format;

	TBM_TRACE("tbm_surface(%p) format(%s)\n", surface, _tbm_surface_internal_format_to_str(format));

	_tbm_surface_mutex_unlock();

	return format;
}

int
tbm_surface_internal_get_plane_bo_idx(tbm_surface_h surface, int plane_idx)
{
	struct _tbm_surface *surf;
	int bo_idx;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);
	TBM_SURFACE_RETURN_VAL_IF_FAIL(plane_idx > -1, 0);

	surf = (struct _tbm_surface *)surface;
	bo_idx = surf->planes_bo_idx[plane_idx];

	TBM_TRACE("tbm_surface(%p) plane_idx(%d) bo_idx(%d)\n", surface, plane_idx, bo_idx);

	_tbm_surface_mutex_unlock();

	return bo_idx;
}

int
tbm_surface_internal_add_user_data(tbm_surface_h surface, unsigned long key,
				   tbm_data_free data_free_func)
{
	tbm_user_data *data;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	/* check if the data according to the key exist if so, return false. */
	data = user_data_lookup(&surface->user_data_list, key);
	if (data) {
		TBM_TRACE("warning: user data already exist tbm_surface(%p) key(%lu)\n", surface, key);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	data = user_data_create(key, data_free_func);
	if (!data) {
		TBM_TRACE("error: tbm_surface(%p) key(%lu)\n", surface, key);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	TBM_TRACE("tbm_surface(%p) key(%lu) data(%p)\n", surface, key, data);

	LIST_ADD(&data->item_link, &surface->user_data_list);

	_tbm_surface_mutex_unlock();

	return 1;
}

int
tbm_surface_internal_set_user_data(tbm_surface_h surface, unsigned long key,
				   void *data)
{
	tbm_user_data *old_data;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	if (LIST_IS_EMPTY(&surface->user_data_list)) {
		TBM_TRACE("error: tbm_surface(%p) key(%lu)\n", surface, key);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	old_data = user_data_lookup(&surface->user_data_list, key);
	if (!old_data) {
		TBM_TRACE("error: tbm_surface(%p) key(%lu)\n", surface, key);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	if (old_data->data && old_data->free_func)
		old_data->free_func(old_data->data);

	old_data->data = data;

	TBM_TRACE("tbm_surface(%p) key(%lu) data(%p)\n", surface, key, old_data->data);

	_tbm_surface_mutex_unlock();

	return 1;
}

int
tbm_surface_internal_get_user_data(tbm_surface_h surface, unsigned long key,
				   void **data)
{
	tbm_user_data *old_data;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	if (!data || LIST_IS_EMPTY(&surface->user_data_list)) {
		TBM_TRACE("error: tbm_surface(%p) key(%lu)\n", surface, key);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	old_data = user_data_lookup(&surface->user_data_list, key);
	if (!old_data) {
		TBM_TRACE("error: tbm_surface(%p) key(%lu)\n", surface, key);
		*data = NULL;
		_tbm_surface_mutex_unlock();
		return 0;
	}

	*data = old_data->data;

	TBM_TRACE("tbm_surface(%p) key(%lu) data(%p)\n", surface, key, old_data->data);

	_tbm_surface_mutex_unlock();

	return 1;
}

int
tbm_surface_internal_delete_user_data(tbm_surface_h surface,
				      unsigned long key)
{
	tbm_user_data *old_data = (void *)0;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);

	if (LIST_IS_EMPTY(&surface->user_data_list)) {
		TBM_TRACE("error: tbm_surface(%p) key(%lu)\n", surface, key);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	old_data = user_data_lookup(&surface->user_data_list, key);
	if (!old_data) {
		TBM_TRACE("error: tbm_surface(%p) key(%lu)\n", surface, key);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	TBM_TRACE("tbm_surface(%p) key(%lu) data(%p)\n", surface, key, old_data->data);

	user_data_delete(old_data);

	_tbm_surface_mutex_unlock();

	return 1;
}

/* LCOV_EXCL_START */
unsigned int
_tbm_surface_internal_get_debug_pid(tbm_surface_h surface)
{
	TBM_RETURN_VAL_IF_FAIL(surface, 0);

	return surface->debug_pid;
}

void
tbm_surface_internal_set_debug_pid(tbm_surface_h surface, unsigned int pid)
{
	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_IF_FAIL(tbm_surface_internal_is_valid(surface));

	surface->debug_pid = pid;

	_tbm_surface_mutex_unlock();
}

static tbm_surface_debug_data *
_tbm_surface_internal_debug_data_create(char *key, char *value)
{
	tbm_surface_debug_data *debug_data = NULL;

	debug_data = calloc(1, sizeof(tbm_surface_debug_data));
	if (!debug_data)
		return NULL;

	if (key) debug_data->key = strdup(key);
	if (value) debug_data->value = strdup(value);

	return debug_data;
}

int
tbm_surface_internal_set_debug_data(tbm_surface_h surface, char *key, char *value)
{
	tbm_surface_debug_data *debug_data = NULL;
	tbm_surface_debug_data *old_data = NULL, *tmp = NULL;
	tbm_bufmgr bufmgr = NULL;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(tbm_surface_internal_is_valid(surface), 0);
	TBM_SURFACE_RETURN_VAL_IF_FAIL(key, 0);

	bufmgr = surface->bufmgr;

	TBM_SURFACE_RETURN_VAL_IF_FAIL(bufmgr, 0);

	if (!LIST_IS_EMPTY(&surface->debug_data_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(old_data, tmp, &surface->debug_data_list, item_link) {
			if (!strcmp(old_data->key, key)) {
				if (value)
					old_data->value = strdup(value);
				else
					old_data->value = NULL;
			}
		}
	}

	debug_data = _tbm_surface_internal_debug_data_create(key, value);
	if (!debug_data) {
		TBM_TRACE("error: tbm_surface(%p) key(%s) value(%s)\n", surface, key, value);
		_tbm_surface_mutex_unlock();
		return 0;
	}

	TBM_TRACE("tbm_surface(%p) key(%s) value(%s)\n", surface, key, value);

	LIST_ADD(&debug_data->item_link, &surface->debug_data_list);

	if (!LIST_IS_EMPTY(&bufmgr->debug_key_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(old_data, tmp, &bufmgr->debug_key_list, item_link) {
			if (!strcmp(old_data->key, key)) {
				_tbm_surface_mutex_unlock();
				return 1;
			}
		}
	}

	debug_data = _tbm_surface_internal_debug_data_create(key, NULL);
	LIST_ADD(&debug_data->item_link, &bufmgr->debug_key_list);

	_tbm_surface_mutex_unlock();

	return 1;
}

char *
_tbm_surface_internal_get_debug_data(tbm_surface_h surface, char *key)
{
	tbm_surface_debug_data *old_data = NULL, *tmp = NULL;

	_tbm_surface_mutex_lock();

	TBM_SURFACE_RETURN_VAL_IF_FAIL(surface, NULL);

	if (!LIST_IS_EMPTY(&surface->debug_data_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(old_data, tmp, &surface->debug_data_list, item_link) {
			if (!strcmp(old_data->key, key)) {
				_tbm_surface_mutex_unlock();
				return old_data->value;
			}
		}
	}

	_tbm_surface_mutex_unlock();

	return NULL;
}

typedef struct _tbm_surface_dump_info tbm_surface_dump_info;
typedef struct _tbm_surface_dump_buf_info tbm_surface_dump_buf_info;

struct _tbm_surface_dump_buf_info {
	int index;
	tbm_bo bo;
	int size;
	int dirty;
	int dirty_shm;
	int shm_stride;
	int shm_h;
	char name[1024];

	tbm_surface_info_s info;

	struct list_head link;
};

struct _tbm_surface_dump_info {
	char *path;  // copy???
	int dump_max;
	int count;
	struct list_head *link;
	struct list_head surface_list; /* link of surface */
};

static tbm_surface_dump_info *g_dump_info = NULL;
static const char *dump_postfix[2] = {"png", "yuv"};

static void
_tbm_surface_internal_dump_file_raw(const char *file, void *data1, int size1, void *data2,
		int size2, void *data3, int size3)
{
	unsigned int *blocks;
	FILE *fp = fopen(file, "w+");
	TBM_RETURN_IF_FAIL(fp != NULL);

	blocks = (unsigned int *)data1;
	fwrite(blocks, 1, size1, fp);

	if (size2 > 0) {
		blocks = (unsigned int *)data2;
		fwrite(blocks, 1, size2, fp);
	}

	if (size3 > 0) {
		blocks = (unsigned int *)data3;
		fwrite(blocks, 1, size3, fp);
	}

	fclose(fp);
}

static void
_tbm_surface_internal_dump_file_png(const char *file, const void *data, int width,
		int height)
{
	FILE *fp = fopen(file, "wb");
	TBM_RETURN_IF_FAIL(fp != NULL);
	int depth = 8;

	png_structp pPngStruct =
		png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!pPngStruct) {
		fclose(fp);
		return;
	}

	png_infop pPngInfo = png_create_info_struct(pPngStruct);
	if (!pPngInfo) {
		png_destroy_write_struct(&pPngStruct, NULL);
		fclose(fp);
		return;
	}

	png_init_io(pPngStruct, fp);
	png_set_IHDR(pPngStruct,
			pPngInfo,
			width,
			height,
			depth,
			PNG_COLOR_TYPE_RGBA,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_set_bgr(pPngStruct);
	png_write_info(pPngStruct, pPngInfo);

	const int pixel_size = 4;	// RGBA
	png_bytep *row_pointers =
			png_malloc(pPngStruct, height * sizeof(png_byte *));

	unsigned int *blocks = (unsigned int *)data;
	int y = 0;
	int x = 0;

	for (; y < height; ++y) {
		png_bytep row =
			png_malloc(pPngStruct, sizeof(png_byte) * width * pixel_size);
		row_pointers[y] = (png_bytep)row;
		for (x = 0; x < width; ++x) {
			unsigned int curBlock = blocks[y * width + x];
			row[x * pixel_size] = (curBlock & 0xFF);
			row[1 + x * pixel_size] = (curBlock >> 8) & 0xFF;
			row[2 + x * pixel_size] = (curBlock >> 16) & 0xFF;
			row[3 + x * pixel_size] = (curBlock >> 24) & 0xFF;
		}
	}

	png_write_image(pPngStruct, row_pointers);
	png_write_end(pPngStruct, pPngInfo);

	for (y = 0; y < height; y++)
		png_free(pPngStruct, row_pointers[y]);
	png_free(pPngStruct, row_pointers);

	png_destroy_write_struct(&pPngStruct, &pPngInfo);

	fclose(fp);
}

void
tbm_surface_internal_dump_start(char *path, int w, int h, int count)
{
	TBM_RETURN_IF_FAIL(path != NULL);
	TBM_RETURN_IF_FAIL(w > 0);
	TBM_RETURN_IF_FAIL(h > 0);
	TBM_RETURN_IF_FAIL(count > 0);

	tbm_surface_dump_buf_info *buf_info = NULL;
	tbm_surface_dump_buf_info *tmp;
	tbm_bo bo = NULL;
	int i;
	int buffer_size;
	tbm_surface_h tbm_surface;
	tbm_surface_info_s info;
	tbm_surface_error_e err;

	/* check running */
	if (g_dump_info) {
		TBM_LOG_W("waring already running the tbm_surface_internal_dump.\n");
		return;
	}

	g_dump_info = calloc(1, sizeof(struct _tbm_surface_dump_info));
	TBM_RETURN_IF_FAIL(g_dump_info);

	LIST_INITHEAD(&g_dump_info->surface_list);
	g_dump_info->count = 0;
	g_dump_info->dump_max = count;

	/* get buffer size */
	tbm_surface = tbm_surface_create(w, h, TBM_FORMAT_ARGB8888);
	if (tbm_surface == NULL) {
		TBM_LOG_E("tbm_surface_create fail\n");
		free(g_dump_info);
		g_dump_info = NULL;
		return;
	}
	err = tbm_surface_map(tbm_surface, TBM_SURF_OPTION_READ, &info);
	if (err != TBM_SURFACE_ERROR_NONE) {
		TBM_LOG_E("tbm_surface_map fail\n");
		tbm_surface_destroy(tbm_surface);
		free(g_dump_info);
		g_dump_info = NULL;
		return;
	}
	buffer_size = info.planes[0].stride * h;
	tbm_surface_unmap(tbm_surface);
	tbm_surface_destroy(tbm_surface);

	/* create dump lists */
	for (i = 0; i < count; i++)	{
		buf_info = calloc(1, sizeof(tbm_surface_dump_buf_info));
		TBM_GOTO_VAL_IF_FAIL(buf_info, fail);
		bo = tbm_bo_alloc(g_surface_bufmgr, buffer_size, TBM_BO_DEFAULT);
		if (bo == NULL) {
			free(buf_info);
			goto fail;
		}

		buf_info->index = i;
		buf_info->bo = bo;
		buf_info->size = buffer_size;

		LIST_ADDTAIL(&buf_info->link, &g_dump_info->surface_list);
	}

	g_dump_info->path = path;
	g_dump_info->link = &g_dump_info->surface_list;

	TBM_LOG_I("Dump Start.. path:%s, count:%d\n", g_dump_info->path, count);

	return;
fail:
	/* free resources */
	if (!LIST_IS_EMPTY(&g_dump_info->surface_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(buf_info, tmp, &g_dump_info->surface_list, link) {
			tbm_bo_unref(buf_info->bo);
			free(buf_info);
		}
	}

	TBM_LOG_E("Dump Start fail.. path:%s\n", g_dump_info->path);

	free(g_dump_info);
	g_dump_info = NULL;

	return;
}

void
tbm_surface_internal_dump_end(void)
{
	tbm_surface_dump_buf_info *buf_info = NULL, *tmp = NULL;
	tbm_bo_handle bo_handle;

	if (!g_dump_info)
		return;

	/* make files */
	if (!LIST_IS_EMPTY(&g_dump_info->surface_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(buf_info, tmp, &g_dump_info->surface_list, link) {
			char file[2048];

			if (buf_info->dirty) {
				void *ptr1 = NULL;
				void *ptr2 = NULL;

				bo_handle = tbm_bo_map(buf_info->bo, TBM_DEVICE_CPU, TBM_OPTION_READ);
				if (bo_handle.ptr == NULL)
					continue;

				snprintf(file, sizeof(file), "%s/%s", g_dump_info->path, buf_info->name);
				TBM_LOG_I("Dump File.. %s generated.\n", file);

				switch (buf_info->info.format) {
				case TBM_FORMAT_ARGB8888:
				case TBM_FORMAT_XRGB8888:
					_tbm_surface_internal_dump_file_png(file, bo_handle.ptr,
									buf_info->info.planes[0].stride >> 2, buf_info->info.height);
					break;
				case TBM_FORMAT_YVU420:
				case TBM_FORMAT_YUV420:
					ptr1 = bo_handle.ptr + buf_info->info.planes[0].stride * buf_info->info.height;
					ptr2 = ptr1 + buf_info->info.planes[1].stride * (buf_info->info.height >> 1);
					_tbm_surface_internal_dump_file_raw(file, bo_handle.ptr,
									buf_info->info.planes[0].stride * buf_info->info.height,
									ptr1,
									buf_info->info.planes[1].stride * (buf_info->info.height >> 1),
									ptr2,
									buf_info->info.planes[2].stride * (buf_info->info.height >> 1));
					break;
				case TBM_FORMAT_NV12:
				case TBM_FORMAT_NV21:
					ptr1 = bo_handle.ptr + buf_info->info.planes[0].stride * buf_info->info.height;
					_tbm_surface_internal_dump_file_raw(file, bo_handle.ptr,
									buf_info->info.planes[0].stride * buf_info->info.height,
									ptr1,
									buf_info->info.planes[1].stride * (buf_info->info.height >> 1),
									NULL, 0);
					break;
				case TBM_FORMAT_YUYV:
				case TBM_FORMAT_UYVY:
					_tbm_surface_internal_dump_file_raw(file, bo_handle.ptr,
									buf_info->info.planes[0].stride * buf_info->info.height,
									NULL, 0, NULL, 0);
					break;
				default:
					TBM_LOG_E("can't dump %c%c%c%c buffer", FOURCC_STR(buf_info->info.format));
					tbm_bo_unmap(buf_info->bo);
					return;
				}

				tbm_bo_unmap(buf_info->bo);
			} else if (buf_info->dirty_shm) {
				bo_handle = tbm_bo_map(buf_info->bo, TBM_DEVICE_CPU, TBM_OPTION_READ);
				if (bo_handle.ptr == NULL)
					continue;

				snprintf(file, sizeof(file), "%s/%s", g_dump_info->path, buf_info->name);
				TBM_LOG_I("Dump File.. %s generated.\n", file);

				_tbm_surface_internal_dump_file_png(file, bo_handle.ptr,
								buf_info->shm_stride >> 2, buf_info->shm_h);

				tbm_bo_unmap(buf_info->bo);
			}
		}
	}

	/* free resources */
	if (!LIST_IS_EMPTY(&g_dump_info->surface_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(buf_info, tmp, &g_dump_info->surface_list, link) {
			tbm_bo_unref(buf_info->bo);
			free(buf_info);
		}
	}

	free(g_dump_info);
	g_dump_info = NULL;

	TBM_LOG_I("Dump End..\n");
}

void
tbm_surface_internal_dump_buffer(tbm_surface_h surface, const char *type)
{
	TBM_RETURN_IF_FAIL(surface != NULL);
	TBM_RETURN_IF_FAIL(type != NULL);

	tbm_surface_dump_buf_info *buf_info;
	tbm_surface_info_s info;
	struct list_head *next_link;
	tbm_bo_handle bo_handle;
	int ret;
	const char *postfix;

	if (!g_dump_info)
		return;

	next_link = g_dump_info->link->next;
	TBM_RETURN_IF_FAIL(next_link != NULL);

	if (next_link == &g_dump_info->surface_list) {
		next_link = next_link->next;
		TBM_RETURN_IF_FAIL(next_link != NULL);
	}

	buf_info = LIST_ENTRY(tbm_surface_dump_buf_info, next_link, link);
	TBM_RETURN_IF_FAIL(buf_info != NULL);

	ret = tbm_surface_map(surface, TBM_SURF_OPTION_READ|TBM_SURF_OPTION_WRITE, &info);
	TBM_RETURN_IF_FAIL(ret == TBM_SURFACE_ERROR_NONE);

	if (info.size > buf_info->size) {
		TBM_LOG_W("Dump skip. surface over created buffer size(%d, %d)\n", info.size, buf_info->size);
		tbm_surface_unmap(surface);
		return;
	}

	if (info.format == TBM_FORMAT_ARGB8888 || info.format == TBM_FORMAT_XRGB8888)
		postfix = dump_postfix[0];
	else
		postfix = dump_postfix[1];

	/* make the file information */
	memcpy(&buf_info->info, &info, sizeof(tbm_surface_info_s));

	/* dump */
	bo_handle = tbm_bo_map(buf_info->bo, TBM_DEVICE_CPU, TBM_OPTION_WRITE);
	TBM_RETURN_IF_FAIL(bo_handle.ptr != NULL);
	memset(bo_handle.ptr, 0x00, buf_info->size);

	switch (info.format) {
	case TBM_FORMAT_ARGB8888:
	case TBM_FORMAT_XRGB8888:
		snprintf(buf_info->name, sizeof(buf_info->name), "%10.3f_%03d_%p-%s.%s",
				 _tbm_surface_internal_get_time(),
				 g_dump_info->count++, surface, type, postfix);
		memcpy(bo_handle.ptr, info.planes[0].ptr, info.size);
		break;
	case TBM_FORMAT_YVU420:
	case TBM_FORMAT_YUV420:
		snprintf(buf_info->name, sizeof(buf_info->name), "%10.3f_%03d-%s_%dx%d_%c%c%c%c.%s",
				 _tbm_surface_internal_get_time(),
				 g_dump_info->count++, type, info.planes[0].stride, info.height, FOURCC_STR(info.format), postfix);
		memcpy(bo_handle.ptr, info.planes[0].ptr, info.planes[0].stride * info.height);
		bo_handle.ptr += info.planes[0].stride * info.height;
		memcpy(bo_handle.ptr, info.planes[1].ptr, info.planes[1].stride * (info.height >> 1));
		bo_handle.ptr += info.planes[1].stride * (info.height >> 1);
		memcpy(bo_handle.ptr, info.planes[2].ptr, info.planes[2].stride * (info.height >> 1));
		break;
	case TBM_FORMAT_NV12:
	case TBM_FORMAT_NV21:
		snprintf(buf_info->name, sizeof(buf_info->name), "%10.3f_%03d-%s_%dx%d_%c%c%c%c.%s",
				 _tbm_surface_internal_get_time(),
				 g_dump_info->count++, type, info.planes[0].stride, info.height, FOURCC_STR(info.format), postfix);
		memcpy(bo_handle.ptr, info.planes[0].ptr, info.planes[0].stride * info.height);
		bo_handle.ptr += info.planes[0].stride * info.height;
		memcpy(bo_handle.ptr, info.planes[1].ptr, info.planes[1].stride * (info.height >> 1));
		break;
	case TBM_FORMAT_YUYV:
	case TBM_FORMAT_UYVY:
		snprintf(buf_info->name, sizeof(buf_info->name), "%10.3f_%03d-%s_%dx%d_%c%c%c%c.%s",
				 _tbm_surface_internal_get_time(),
				 g_dump_info->count++, type, info.planes[0].stride, info.height, FOURCC_STR(info.format), postfix);
		memcpy(bo_handle.ptr, info.planes[0].ptr, info.planes[0].stride * info.height);
		break;
	default:
		TBM_LOG_E("can't copy %c%c%c%c buffer", FOURCC_STR(info.format));
		tbm_bo_unmap(buf_info->bo);
		return;
	}

	tbm_bo_unmap(buf_info->bo);

	tbm_surface_unmap(surface);

	buf_info->dirty = 1;
	buf_info->dirty_shm = 0;

	if (g_dump_info->count == 1000)
		g_dump_info->count = 0;

	g_dump_info->link = next_link;

	TBM_LOG_I("Dump %s \n", buf_info->name);
}

void tbm_surface_internal_dump_shm_buffer(void *ptr, int w, int h, int  stride, const char *type)
{
	TBM_RETURN_IF_FAIL(ptr != NULL);
	TBM_RETURN_IF_FAIL(w > 0);
	TBM_RETURN_IF_FAIL(h > 0);
	TBM_RETURN_IF_FAIL(stride > 0);
	TBM_RETURN_IF_FAIL(type != NULL);

	tbm_surface_dump_buf_info *buf_info;
	struct list_head *next_link;
	tbm_bo_handle bo_handle;

	if (!g_dump_info)
		return;

	next_link = g_dump_info->link->next;
	TBM_RETURN_IF_FAIL(next_link != NULL);

	if (next_link == &g_dump_info->surface_list) {
		next_link = next_link->next;
		TBM_RETURN_IF_FAIL(next_link != NULL);
	}

	buf_info = LIST_ENTRY(tbm_surface_dump_buf_info, next_link, link);
	TBM_RETURN_IF_FAIL(buf_info != NULL);

	if (stride * h > buf_info->size) {
		TBM_LOG_W("Dump skip. shm buffer over created buffer size(%d, %d)\n", stride * h, buf_info->size);
		return;
	}

	/* dump */
	bo_handle = tbm_bo_map(buf_info->bo, TBM_DEVICE_CPU, TBM_OPTION_WRITE);
	TBM_RETURN_IF_FAIL(bo_handle.ptr != NULL);
	memset(bo_handle.ptr, 0x00, buf_info->size);
	memset(&buf_info->info, 0x00, sizeof(tbm_surface_info_s));

	snprintf(buf_info->name, sizeof(buf_info->name), "%10.3f_%03d-%s.%s",
			 _tbm_surface_internal_get_time(),
			 g_dump_info->count++, type, dump_postfix[0]);
	memcpy(bo_handle.ptr, ptr, stride * h);

	tbm_bo_unmap(buf_info->bo);

	buf_info->dirty = 0;
	buf_info->dirty_shm = 1;
	buf_info->shm_stride = stride;
	buf_info->shm_h = h;

	if (g_dump_info->count == 1000)
		g_dump_info->count = 0;

	g_dump_info->link = next_link;

	TBM_LOG_I("Dump %s \n", buf_info->name);
}
/*LCOV_EXCL_STOP*/
