/**************************************************************************

libtbm

Copyright 2012 Samsung Electronics co., Ltd. All Rights Reserved.

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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "tbm_bufmgr_int.h"

/* LCOV_EXCL_START */
tbm_bufmgr_backend tbm_backend_alloc(void)
{
	tbm_bufmgr_backend bufmgr_backend;

	bufmgr_backend = calloc(1, sizeof(struct _tbm_bufmgr_backend));
	if (!bufmgr_backend) {
		TBM_LOG_E("error: fail to allocate the bufmgr_backend\n");
		return NULL;
	}

	return bufmgr_backend;
}

void tbm_backend_free(tbm_bufmgr_backend backend)
{
	if (!backend)
		return;

	free(backend);
	backend = NULL;
}

int tbm_backend_init(tbm_bufmgr bufmgr, tbm_bufmgr_backend backend)
{
	if (!bufmgr) {
		TBM_LOG_E("error: fail to init tbm backend... bufmgr is null\n");
		return 0;
	}

	if (!backend) {
		TBM_LOG_E("error: fail to init tbm backend... backend is null\n");
		return 0;
	}

	bufmgr->backend = backend;
	bufmgr->capabilities = TBM_BUFMGR_CAPABILITY_NONE;

	if (bufmgr->backend->bo_import && bufmgr->backend->bo_export)
		bufmgr->capabilities |= TBM_BUFMGR_CAPABILITY_SHARE_KEY;

	if (bufmgr->backend->bo_import_fd && bufmgr->backend->bo_export_fd)
		bufmgr->capabilities |= TBM_BUFMGR_CAPABILITY_SHARE_FD;

	return 1;
}

void *tbm_backend_get_bufmgr_priv(tbm_bo bo)
{
	tbm_bufmgr_backend backend = bo->bufmgr->backend;

	return backend->priv;
}

void *tbm_backend_get_priv_from_bufmgr(tbm_bufmgr bufmgr)
{
	tbm_bufmgr_backend backend = bufmgr->backend;

	return backend->priv;
}

void tbm_backend_set_bo_priv(tbm_bo bo, void *bo_priv)
{
	bo->priv = bo_priv;
}

void *tbm_backend_get_bo_priv(tbm_bo bo)
{
	return bo->priv;
}

int tbm_backend_is_display_server(void)
{
	const char *value;

	value = (const char*)getenv("TBM_DISPLAY_SERVER");
	if (!value)
		return 0;

	return 1;
}
/* LCOV_EXCL_STOP */
