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

#ifndef _STUBS_H
#define _STUBS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "tbm_bufmgr_tgl.h"

#define IOCTL_FD_ERROR_NONE 1
#define IOCTL_FD_ERROR 2
#define IOCTL_FD_GET 3

FILE g_file;
FILE g_error_file;
int CALLOC_ERROR;
int free_called;
int DLOPEN_ERROR;

int ut_fclose(FILE *stream)
{
	return 0;
}

FILE *ut_fopen(const char *filename, const char *mode)
{
	if (!filename || !mode)
		return NULL;

	if (!strcmp(filename, "/proc/111/cmdline"))
		return NULL;

	if (!strcmp(filename, "/proc/222/cmdline"))
		return &g_error_file;

	return &g_file;
}

char *ut_fgets(char *str, int num, FILE *stream)
{
	if (!str || num < 1 || !stream)
		return NULL;

	if (stream == &g_error_file)
		return NULL;

	strncpy(str, "application", 255);

	return "application";
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return 0;
}

int dup2(int old_handle, int new_handle)
{
	if (old_handle == new_handle)
		return -1;

	return 0;
}

int dup(int handle)
{
	if (1 == handle)
		return -1;

	return 0;
}

int ioctl(int fd, unsigned long int request, ...)
{
	if (IOCTL_FD_ERROR == fd)
		return 1;

	if (IOCTL_FD_GET == fd) {
		va_list argList;
		va_start(argList, request);
		struct tgl_user_data *arg = va_arg(argList, struct tgl_user_data *);
		arg->data1 = 1;
		arg->locked = 1;
		va_end(argList);
	}

	return 0;
}

void *ut_calloc(size_t nmemb, size_t size)
{
	if (CALLOC_ERROR)
		return NULL;

	return calloc(nmemb, size);
}

void ut_free(void *ptr)
{
	free_called = 1;
	free(ptr);
}

#endif /* _STUBS_H */
