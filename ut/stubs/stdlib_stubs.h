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

#ifndef _STDLIB_STUBS_H
#define _STDLIB_STUBS_H

#include <stdlib.h>

extern int CALLOC_ERROR;
extern int FREE_CALLED;
extern void *FREE_PTR;
extern void *FREE_TESTED_PTR;
extern int free_called_for_tested_ptr;
extern int free_call_count;
extern int GETENV_ERROR;

void *ut_calloc(size_t nmemb, size_t size);

void ut_free(void *ptr);

char *ut_getenv(const char *name);

#endif /* _STDLIB_STUBS_H */
