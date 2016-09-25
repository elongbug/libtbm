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
#include "tbm_bufmgr.h"
#include "tbm_bufmgr_int.h"
#include "tbm_surface_internal.h"

int
tbm_surface_query_formats(uint32_t **formats, uint32_t *num)
{
	TBM_TRACE("\n");

	if (!tbm_surface_internal_query_supported_formats(formats, num))
		return TBM_SURFACE_ERROR_INVALID_OPERATION;

	return TBM_SURFACE_ERROR_NONE;
}

tbm_surface_h
tbm_surface_create(int width, int height, tbm_format format)
{
	TBM_TRACE("width(%d) height(%d)\n", width, height);

	if (!(width > 0) || !(height > 0)) {
#ifdef HAVE_CAPI_0_1_1
		set_last_result(TBM_SURFACE_ERROR_INVALID_PARAMETER);
#endif
		return NULL;
	}

	struct _tbm_surface *surf = NULL;

	surf = tbm_surface_internal_create_with_flags(width, height, format,
						      TBM_BO_DEFAULT);
	if (!surf) {
#ifdef HAVE_CAPI_0_1_1
		set_last_result(TBM_SURFACE_ERROR_INVALID_OPERATION);
#endif
		return NULL;
	}
#ifdef HAVE_CAPI_0_1_1
	set_last_result(TBM_SURFACE_ERROR_NONE);
#endif
	return surf;
}

int
tbm_surface_destroy(tbm_surface_h surface)
{
	TBM_TRACE("tbm_surface(%p)\n", surface);

	TBM_RETURN_VAL_IF_FAIL(surface, TBM_SURFACE_ERROR_INVALID_PARAMETER);

	tbm_surface_internal_destroy(surface);

	return TBM_SURFACE_ERROR_NONE;
}

int
tbm_surface_map(tbm_surface_h surface, int opt, tbm_surface_info_s *info)
{
	TBM_TRACE("tbm_surface(%p)\n", surface);

	TBM_RETURN_VAL_IF_FAIL(surface, TBM_SURFACE_ERROR_INVALID_PARAMETER);
	TBM_RETURN_VAL_IF_FAIL(info != NULL, TBM_SURFACE_ERROR_INVALID_PARAMETER);

	int ret = 0;

	ret = tbm_surface_internal_get_info(surface, opt, info, 1);
	if (ret == 0)
		return TBM_SURFACE_ERROR_INVALID_OPERATION;

	return TBM_SURFACE_ERROR_NONE;
}

int
tbm_surface_unmap(tbm_surface_h surface)
{
	TBM_TRACE("tbm_surface(%p)\n", surface);

	TBM_RETURN_VAL_IF_FAIL(surface, TBM_SURFACE_ERROR_INVALID_PARAMETER);

	tbm_surface_internal_unmap(surface);

	return TBM_SURFACE_ERROR_NONE;
}

int
tbm_surface_get_info(tbm_surface_h surface, tbm_surface_info_s *info)
{
	TBM_TRACE("tbm_surface(%p)\n", surface);

	TBM_RETURN_VAL_IF_FAIL(surface, TBM_SURFACE_ERROR_INVALID_PARAMETER);
	TBM_RETURN_VAL_IF_FAIL(info != NULL, TBM_SURFACE_ERROR_INVALID_PARAMETER);

	int ret = 0;

	ret = tbm_surface_internal_get_info(surface, 0, info, 0);
	if (ret == 0)
		return TBM_SURFACE_ERROR_INVALID_OPERATION;

	return TBM_SURFACE_ERROR_NONE;
}

int
tbm_surface_get_width(tbm_surface_h surface)
{
	TBM_TRACE("tbm_surface(%p)\n", surface);

	TBM_RETURN_VAL_IF_FAIL(surface, TBM_SURFACE_ERROR_INVALID_PARAMETER);

	return tbm_surface_internal_get_width(surface);
}

int
tbm_surface_get_height(tbm_surface_h surface)
{
	TBM_TRACE("tbm_surface(%p)\n", surface);

	TBM_RETURN_VAL_IF_FAIL(surface, TBM_SURFACE_ERROR_INVALID_PARAMETER);

	return tbm_surface_internal_get_height(surface);
}

tbm_format
tbm_surface_get_format(tbm_surface_h surface)
{
	TBM_TRACE("tbm_surface(%p)\n", surface);

	if (!surface) {
#ifdef HAVE_CAPI_0_1_1
		set_last_result(TBM_SURFACE_ERROR_INVALID_PARAMETER);
#endif
		return 0;
	}
#ifdef HAVE_CAPI_0_1_1
	set_last_result(TBM_SURFACE_ERROR_NONE);
#endif
	return tbm_surface_internal_get_format(surface);
}
