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

#include "stdlib_stubs.h"

int CALLOC_ERROR = 0;
int FREE_CALLED = 0;
void *FREE_PTR = NULL;
void *FREE_TESTED_PTR = 0;
int free_called_for_tested_ptr = 0;
int free_call_count = 0;
int GETENV_ERROR = 0;

void *ut_calloc(size_t nmemb, size_t size)
{
	if (CALLOC_ERROR) {
		return NULL;
	}

	return calloc(nmemb, size);
}

void ut_free(void *ptr)
{
	if (FREE_TESTED_PTR == ptr) {
		free_called_for_tested_ptr = 1;
	}
	FREE_PTR = ptr;
	FREE_CALLED = 1;
	free(ptr);
	free_call_count++;
}

char *ut_getenv(const char *name)
{
	if (GETENV_ERROR) {
		return NULL;
	}

	return "getenv";
}
