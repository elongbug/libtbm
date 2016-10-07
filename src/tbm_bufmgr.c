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

#include "tbm_bufmgr.h"
#include "tbm_bufmgr_int.h"
#include "tbm_bufmgr_backend.h"
#include "list.h"

#ifdef DEBUG
int bDebug;
#endif

#ifdef TRACE
int bTrace;
#endif

#ifdef HAVE_DLOG
int bDlog;
#endif

tbm_bufmgr gBufMgr;
int b_dump_queue;

static pthread_mutex_t gLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t tbm_bufmgr_lock = PTHREAD_MUTEX_INITIALIZER;
static __thread tbm_error_e tbm_last_error = TBM_ERROR_NONE;

static void _tbm_bufmgr_mutex_unlock(void);

//#define TBM_BUFMGR_INIT_TIME

#define PREFIX_LIB    "libtbm_"
#define SUFFIX_LIB    ".so"
#define DEFAULT_LIB   PREFIX_LIB"default"SUFFIX_LIB

/* values to indicate unspecified fields in XF86ModReqInfo. */
#define MAJOR_UNSPEC        0xFF
#define MINOR_UNSPEC        0xFF
#define PATCH_UNSPEC        0xFFFF
#define ABI_VERS_UNSPEC   0xFFFFFFFF

#define MODULE_VERSION_NUMERIC(maj, min, patch) \
			((((maj) & 0xFF) << 24) | (((min) & 0xFF) << 16) | (patch & 0xFFFF))
#define GET_MODULE_MAJOR_VERSION(vers)    (((vers) >> 24) & 0xFF)
#define GET_MODULE_MINOR_VERSION(vers)    (((vers) >> 16) & 0xFF)
#define GET_MODULE_PATCHLEVEL(vers)    ((vers) & 0xFFFF)

/* check condition */
#define TBM_BUFMGR_RETURN_IF_FAIL(cond) {\
	if (!(cond)) {\
		TBM_LOG_E("'%s' failed.\n", #cond);\
		_tbm_bufmgr_mutex_unlock();\
		return;\
	} \
}

#define TBM_BUFMGR_RETURN_VAL_IF_FAIL(cond, val) {\
	if (!(cond)) {\
		TBM_LOG_E("'%s' failed.\n", #cond);\
		_tbm_bufmgr_mutex_unlock();\
		return val;\
	} \
}

enum {
	LOCK_TRY_ONCE,
	LOCK_TRY_ALWAYS,
	LOCK_TRY_NEVER
};

static void
_tbm_set_last_result(tbm_error_e err)
{
	tbm_last_error = err;
}

static bool
_tbm_bufmgr_mutex_init(void)
{
	static bool tbm_bufmgr_mutex_init = false;

	if (tbm_bufmgr_mutex_init)
		return true;

	if (pthread_mutex_init(&tbm_bufmgr_lock, NULL)) {
		TBM_LOG_E("fail: tbm_bufmgr mutex init\n");
		return false;
	}

	tbm_bufmgr_mutex_init = true;

	return true;
}

static void
_tbm_bufmgr_mutex_lock(void)
{
	if (!_tbm_bufmgr_mutex_init())
		return;

	pthread_mutex_lock(&tbm_bufmgr_lock);
}

static void
_tbm_bufmgr_mutex_unlock(void)
{
	pthread_mutex_unlock(&tbm_bufmgr_lock);
}

char * tbm_flag_to_str(int f)
{
	static char str[255];
	int c = 0;
	if (f == TBM_BO_DEFAULT)
		 snprintf(str, 255, "DEFAULT\n");
	else {
		if (f & TBM_BO_SCANOUT)
			c = snprintf(&str[c], 255, "SCANOUT,");
		if (f & TBM_BO_NONCACHABLE)
			c = snprintf(&str[c], 255, "NONCACHABLE,");
		if (f & TBM_BO_WC)
			c = snprintf(&str[c], 255, "WC");
	}
	return str;
}

/* LCOV_EXCL_START */
static int last_chk_bo_cnt = 0;
static void
_tbm_util_check_bo_cnt(tbm_bufmgr bufmgr)
{
	if (bufmgr->bo_cnt >= 500 && ((bufmgr->bo_cnt % 20) == 0)) {
		if (bufmgr->bo_cnt > last_chk_bo_cnt) {
			TBM_DEBUG("============TBM BO CNT DEBUG: bo_cnt=%d\n", bufmgr->bo_cnt);
			tbm_bufmgr_debug_show(bufmgr);
			last_chk_bo_cnt = bufmgr->bo_cnt;
		}
	}
}

static int
_tbm_util_get_max_surface_size(int * w, int * h)
{
	int count = 0;
	tbm_surface_h surface = NULL, tmp = NULL;
	tbm_surface_info_s info;

	*w = 0;
	*h = 0;

	if (gBufMgr == NULL)
		return count;

	if (!LIST_IS_EMPTY(&gBufMgr->surf_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(surface, tmp, &gBufMgr->surf_list, item_link) {
			if (tbm_surface_get_info(surface, &info) == TBM_SURFACE_ERROR_NONE) {
				count++;
				if (*w < info.width) *w = info.width;
				if (*h < info.height) *h = info.height;
			}
		}
	}

	return count;
}

static void
_tbm_util_get_appname_brief(char *brief)
{
	char delim[] = "/";
	char *token = NULL;
	char temp[255] = {0,};
	char *saveptr = NULL;

	token = strtok_r(brief, delim, &saveptr);

	while (token != NULL) {
		memset(temp, 0x00, 255 * sizeof(char));
		strncpy(temp, token, 254 * sizeof(char));
		token = strtok_r(NULL, delim, &saveptr);
	}

	snprintf(brief, sizeof(temp), "%s", temp);
}

static void
_tbm_util_get_appname_from_pid(long pid, char *str)
{
	FILE *fp;
	int len;
	long app_pid = pid;
	char fn_cmdline[255] = {0,};
	char cmdline[255] = {0,};

	snprintf(fn_cmdline, sizeof(fn_cmdline), "/proc/%ld/cmdline", app_pid);

	fp = fopen(fn_cmdline, "r");
	if (fp == 0) {
		fprintf(stderr, "cannot file open /proc/%ld/cmdline", app_pid);
		return;
	}

	if (!fgets(cmdline, 255, fp)) {
		fprintf(stderr, "fail to get appname for pid(%ld)\n", app_pid);
		fclose(fp);
		return;
	}
	fclose(fp);

	len = strlen(cmdline);
	if (len < 1)
		memset(cmdline, 0x00, 255);
	else
		cmdline[len] = 0;

	snprintf(str, sizeof(cmdline), "%s", cmdline);
}
/* LCOV_EXCL_STOP */

tbm_user_data
*user_data_lookup(struct list_head *user_data_list, unsigned long key)
{
	tbm_user_data *user_data = NULL;
	tbm_user_data *old_data = NULL, *tmp = NULL;

	if (!LIST_IS_EMPTY(user_data_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(old_data, tmp, user_data_list, item_link) {
			if (old_data->key == key) {
				user_data = old_data;
				return user_data;
			}
		}
	}

	return user_data;
}

tbm_user_data
*user_data_create(unsigned long key, tbm_data_free data_free_func)
{
	tbm_user_data *user_data = NULL;

	user_data = calloc(1, sizeof(tbm_user_data));
	if (!user_data)
		return NULL;

	user_data->key = key;
	user_data->free_func = data_free_func;
	user_data->data = (void *)0;

	return user_data;
}

void
user_data_delete(tbm_user_data *user_data)
{
	if (user_data->data && user_data->free_func)
		user_data->free_func(user_data->data);

	LIST_DEL(&user_data->item_link);

	free(user_data);
}

static int
_bo_lock(tbm_bo bo, int device, int opt)
{
	tbm_bufmgr bufmgr = bo->bufmgr;
	int ret = 0;

	if (bufmgr->backend->bo_lock)
		ret = bufmgr->backend->bo_lock(bo, device, opt);
	else
		ret = 1;

	return ret;
}

static void
_bo_unlock(tbm_bo bo)
{
	tbm_bufmgr bufmgr = bo->bufmgr;

	if (bufmgr->backend->bo_unlock)
		bufmgr->backend->bo_unlock(bo);
}

static int
_tbm_bo_lock(tbm_bo bo, int device, int opt)
{
	tbm_bufmgr bufmgr = NULL;
	int old;
	int ret = 0;

	if (!bo)
		return 0;

	bufmgr = bo->bufmgr;

	/* do not try to lock the bo */
	if (bufmgr->lock_type == LOCK_TRY_NEVER)
		return 1;

	if (bo->lock_cnt < 0) {
		TBM_LOG_E("error bo:%p LOCK_CNT=%d\n",
			bo, bo->lock_cnt);
	}

	old = bo->lock_cnt;
	if (bufmgr->lock_type == LOCK_TRY_ONCE) {
		if (bo->lock_cnt == 0) {
			_tbm_bufmgr_mutex_unlock();
			ret = _bo_lock(bo, device, opt);
			_tbm_bufmgr_mutex_lock();
			if (ret)
				bo->lock_cnt++;
		} else
			ret = 1;
	} else if (bufmgr->lock_type == LOCK_TRY_ALWAYS) {
		_tbm_bufmgr_mutex_unlock();
		ret = _bo_lock(bo, device, opt);
		_tbm_bufmgr_mutex_lock();
		if (ret)
			bo->lock_cnt++;
	} else {
		TBM_LOG_E("error bo:%p lock_type is wrong.\n",
			bo);
	}

	TBM_DBG_LOCK(">> LOCK bo:%p(%d->%d)\n",
		 bo, old, bo->lock_cnt);

	return ret;
}

static void
_tbm_bo_unlock(tbm_bo bo)
{
	tbm_bufmgr bufmgr = NULL;

	int old;

	if (!bo)
		return;

	bufmgr = bo->bufmgr;

	/* do not try to unlock the bo */
	if (bufmgr->lock_type == LOCK_TRY_NEVER)
		return;

	old = bo->lock_cnt;
	if (bufmgr->lock_type == LOCK_TRY_ONCE) {
		if (bo->lock_cnt > 0) {
			bo->lock_cnt--;
			if (bo->lock_cnt == 0)
				_bo_unlock(bo);
		}
	} else if (bufmgr->lock_type == LOCK_TRY_ALWAYS) {
		if (bo->lock_cnt > 0) {
			bo->lock_cnt--;
			_bo_unlock(bo);
		}
	} else {
		TBM_LOG_E("error bo:%p lock_type is wrong.\n",
			bo);
	}

	if (bo->lock_cnt < 0)
		bo->lock_cnt = 0;

	TBM_DBG_LOCK(">> UNLOCK bo:%p(%d->%d)\n",
		 bo, old, bo->lock_cnt);
}

static int
_tbm_bo_is_valid(tbm_bo bo)
{
	tbm_bo old_data = NULL, tmp = NULL;

	if (bo == NULL)
		return 0;

	if (gBufMgr == NULL) {
		TBM_LOG_E("error tbm_bufmgr was deinited\n");
		return 0;
	}

	if (!LIST_IS_EMPTY(&gBufMgr->bo_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(old_data, tmp, &gBufMgr->bo_list, item_link) {
			if (old_data == bo)
				return 1;
		}

	}
	return 0;
}

/* LCOV_EXCL_START */
static int
_check_version(TBMModuleVersionInfo *data)
{
	int abimaj, abimin;
	int vermaj, vermin;

	abimaj = GET_ABI_MAJOR(data->abiversion);
	abimin = GET_ABI_MINOR(data->abiversion);

	TBM_DBG("TBM module %s: vendor=\"%s\" ABI=%d,%d\n",
	    data->modname ? data->modname : "UNKNOWN!",
	    data->vendor ? data->vendor : "UNKNOWN!", abimaj, abimin);

	vermaj = GET_ABI_MAJOR(TBM_ABI_VERSION);
	vermin = GET_ABI_MINOR(TBM_ABI_VERSION);

	TBM_DBG("TBM ABI version %d.%d\n",
	    vermaj, vermin);

	if (abimaj != vermaj) {
		TBM_LOG_E("TBM module ABI major ver(%d) doesn't match the TBM's ver(%d)\n",
			abimaj, vermaj);
		return 0;
	} else if (abimin > vermin) {
		TBM_LOG_E("TBM module ABI minor ver(%d) is newer than the TBM's ver(%d)\n",
			abimin, vermin);
		return 0;
	}
	return 1;
}

static int
_tbm_bufmgr_load_module(tbm_bufmgr bufmgr, int fd, const char *file)
{
	char path[PATH_MAX] = { 0, };
	TBMModuleData *initdata = NULL;
	void *module_data;

	snprintf(path, sizeof(path), BUFMGR_MODULE_DIR "/%s", file);

	module_data = dlopen(path, RTLD_LAZY);
	if (!module_data) {
		TBM_LOG_E("failed to load module: %s(%s)\n",
			dlerror(), file);
		return 0;
	}

	initdata = dlsym(module_data, "tbmModuleData");
	if (initdata) {
		ModuleInitProc init;
		TBMModuleVersionInfo *vers;

		vers = initdata->vers;
		init = initdata->init;

		if (vers) {
			if (!_check_version(vers)) {
				dlclose(module_data);
				return 0;
			}
		} else {
			TBM_LOG_E("Error: module does not supply version information.\n");

			dlclose(module_data);
			return 0;
		}

		if (init) {
			if (!init(bufmgr, fd)) {
				TBM_LOG_E("Fail to init module(%s)\n",
					file);
				dlclose(module_data);
				return 0;
			}

			if (!bufmgr->backend || !bufmgr->backend->priv) {
				TBM_LOG_E("Error: module(%s) wrong operation. Check backend or backend's priv.\n",
					file);
				dlclose(module_data);
				return 0;
			}
		} else {
			TBM_LOG_E("Error: module does not supply init symbol.\n");
			dlclose(module_data);
			return 0;
		}
	} else {
		TBM_LOG_E("Error: module does not have data object.\n");
		dlclose(module_data);
		return 0;
	}

	bufmgr->module_data = module_data;

	TBM_DBG("Success to load module(%s)\n",
	    file);

	return 1;
}

static int
_tbm_load_module(tbm_bufmgr bufmgr, int fd)
{
	struct dirent **namelist;
	const char *p = NULL;
	int n;
	int ret = 0;

	/* load bufmgr priv from default lib */
	ret = _tbm_bufmgr_load_module(bufmgr, fd, DEFAULT_LIB);

	/* load bufmgr priv from configured path */
	if (!ret) {
		n = scandir(BUFMGR_MODULE_DIR, &namelist, 0, alphasort);
		if (n < 0) {
			TBM_LOG_E("no files : %s\n",
				BUFMGR_MODULE_DIR);
		} else {
			while (n--) {
				if (!ret && strstr(namelist[n]->d_name, PREFIX_LIB)) {
					p = strstr(namelist[n]->d_name, SUFFIX_LIB);
					if (p && !strcmp(p, SUFFIX_LIB))
						ret = _tbm_bufmgr_load_module(bufmgr, fd, namelist[n]->d_name);
				}
				free(namelist[n]);
			}
			free(namelist);
		}
	}

	return ret;
}
/* LCOV_EXCL_STOP */

tbm_bufmgr
tbm_bufmgr_init(int fd)
{
	char *env;

#ifdef TBM_BUFMGR_INIT_TIME
	struct timeval start_tv, end_tv;
#endif

	pthread_mutex_lock(&gLock);

#ifdef TBM_BUFMGR_INIT_TIME
	/* get the start tv */
	gettimeofday(&start_tv, NULL);
#endif

	/* LCOV_EXCL_START */
#ifdef HAVE_DLOG
	env = getenv("TBM_DLOG");
	if (env) {
		bDlog = atoi(env);
		TBM_LOG_D("TBM_DLOG=%s\n", env);
	} else {
		bDlog = 1;
	}
#endif

#ifdef DEBUG
	env = getenv("TBM_DEBUG");
	if (env) {
		bDebug = atoi(env);
		TBM_LOG_D("TBM_DEBUG=%s\n", env);
	} else {
		bDebug = 0;
	}
#endif

#ifdef TRACE
	env = getenv("TBM_TRACE");
	if (env) {
		bTrace = atoi(env);
		TBM_LOG_D("TBM_TRACE=%s\n", env);
	} else {
		bTrace = 0;
	}
#endif
	/* LCOV_EXCL_STOP */

	/* initialize buffer manager */
	if (gBufMgr) {
		gBufMgr->ref_count++;
		TBM_TRACE("reuse  tbm_bufmgr(%p) ref_count(%d) fd(%d)\n", gBufMgr, gBufMgr->ref_count, gBufMgr->fd);
		pthread_mutex_unlock(&gLock);
		return gBufMgr;
	}

	TBM_DBG("bufmgr init\n");

	/* allocate bufmgr */
	gBufMgr = calloc(1, sizeof(struct _tbm_bufmgr));
	if (!gBufMgr) {
		_tbm_set_last_result(TBM_BO_ERROR_HEAP_ALLOC_FAILED);
		TBM_TRACE("error: fail to alloc bufmgr fd(%d)\n", fd);
		pthread_mutex_unlock(&gLock);
		return NULL;
	}

	gBufMgr->fd = fd;

	/* load bufmgr priv from env */
	if (!_tbm_load_module(gBufMgr, gBufMgr->fd)) {
		/* LCOV_EXCL_START */
		_tbm_set_last_result(TBM_BO_ERROR_LOAD_MODULE_FAILED);
		TBM_LOG_E("error : Fail to load bufmgr backend\n");
		free(gBufMgr);
		gBufMgr = NULL;
		pthread_mutex_unlock(&gLock);
		return NULL;
		/* LCOV_EXCL_STOP */
	}

	/* log for tbm backend_flag */
	TBM_DBG("backend flag:%x:", gBufMgr->backend->flags);
	TBM_DBG("\n");

	gBufMgr->ref_count = 1;

	TBM_DBG("create tizen bufmgr:%p ref_count:%d\n",
	    gBufMgr, gBufMgr->ref_count);

	/* setup the lock_type */
	env = getenv("BUFMGR_LOCK_TYPE");
	if (env && !strcmp(env, "always"))
		gBufMgr->lock_type = LOCK_TRY_ALWAYS;
	else if (env && !strcmp(env, "none"))
		gBufMgr->lock_type = LOCK_TRY_NEVER;
	else if (env && !strcmp(env, "once"))
		gBufMgr->lock_type = LOCK_TRY_ONCE;
	else
		gBufMgr->lock_type = LOCK_TRY_ALWAYS;

	TBM_DBG("BUFMGR_LOCK_TYPE=%s\n",
	    env ? env : "default:once");

	TBM_TRACE("create tbm_bufmgr(%p) ref_count(%d) fd(%d)\n", gBufMgr, gBufMgr->ref_count, fd);

	/* intialize bo_list */
	LIST_INITHEAD(&gBufMgr->bo_list);

	/* intialize surf_list */
	LIST_INITHEAD(&gBufMgr->surf_list);

	/* intialize surf_list */
	LIST_INITHEAD(&gBufMgr->surf_queue_list);

	/* intialize debug_key_list */
	LIST_INITHEAD(&gBufMgr->debug_key_list);

#ifdef TBM_BUFMGR_INIT_TIME
	/* get the end tv */
 	gettimeofday(&end_tv, NULL);
	TBM_LOG_I("tbm_bufmgr_init time: %ld ms", ((end_tv.tv_sec * 1000 + end_tv.tv_usec / 1000) - (start_tv.tv_sec * 1000 + start_tv.tv_usec / 1000)));
#endif

	pthread_mutex_unlock(&gLock);

	return gBufMgr;
}

void
tbm_bufmgr_deinit(tbm_bufmgr bufmgr)
{
	TBM_RETURN_IF_FAIL(TBM_BUFMGR_IS_VALID(bufmgr));

	tbm_bo bo = NULL;
	tbm_bo tmp = NULL;

	tbm_surface_h surf = NULL;
	tbm_surface_h tmp_surf = NULL;

	pthread_mutex_lock(&gLock);

	if (!gBufMgr) {
		TBM_LOG_E("gBufmgr already destroy: bufmgr:%p\n", bufmgr);
		pthread_mutex_unlock(&gLock);
		return;
	}

	bufmgr->ref_count--;
	if (bufmgr->ref_count > 0) {
		TBM_TRACE("reduce a ref_count(%d) of tbm_bufmgr(%p)\n", bufmgr->ref_count, bufmgr);
		pthread_mutex_unlock(&gLock);
		return;
	}

	/* destroy bo_list */
	if (!LIST_IS_EMPTY(&bufmgr->bo_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(bo, tmp, &bufmgr->bo_list, item_link) {
			TBM_LOG_E("Un-freed bo(%p, ref:%d)\n",
				bo, bo->ref_cnt);
			bo->ref_cnt = 1;
			tbm_bo_unref(bo);
		}
	}

	/* destroy surf_list */
	if (!LIST_IS_EMPTY(&bufmgr->surf_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(surf, tmp_surf, &bufmgr->surf_list, item_link) {
			TBM_LOG_E("Un-freed surf(%p, ref:%d)\n",
				surf, surf->refcnt);
			tbm_surface_destroy(surf);
		}
	}

	/* destroy bufmgr priv */
	bufmgr->backend->bufmgr_deinit(bufmgr->backend->priv);
	bufmgr->backend->priv = NULL;
	tbm_backend_free(bufmgr->backend);
	bufmgr->backend = NULL;

	TBM_TRACE("destroy tbm_bufmgr(%p)\n", bufmgr);

	dlclose(bufmgr->module_data);

	if (bufmgr->fd > 0)
		close(bufmgr->fd);

	free(bufmgr);
	bufmgr = NULL;
	gBufMgr = NULL;

	pthread_mutex_unlock(&gLock);
}

int
tbm_bo_size(tbm_bo bo)
{
	tbm_bufmgr bufmgr = NULL;
	int size;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	bufmgr = gBufMgr;

	size = bufmgr->backend->bo_size(bo);

	TBM_TRACE("bo(%p) size(%d)\n", bo, size);

	_tbm_bufmgr_mutex_unlock();

	return size;
}

tbm_bo
tbm_bo_ref(tbm_bo bo)
{
	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), NULL);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), NULL);

	bo->ref_cnt++;

	TBM_TRACE("bo(%p) ref_cnt(%d)\n", bo, bo->ref_cnt);

	_tbm_bufmgr_mutex_unlock();

	return bo;
}

void
tbm_bo_unref(tbm_bo bo)
{
	tbm_bufmgr bufmgr = NULL;
	tbm_user_data *old_data = NULL, *tmp = NULL;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_IF_FAIL(gBufMgr);
	TBM_BUFMGR_RETURN_IF_FAIL(_tbm_bo_is_valid(bo));

	bufmgr = gBufMgr;

	TBM_TRACE("bo(%p) ref_cnt(%d)\n", bo, bo->ref_cnt - 1);

	if (bo->ref_cnt <= 0) {
		_tbm_bufmgr_mutex_unlock();
		return;
	}

	bo->ref_cnt--;
	if (bo->ref_cnt == 0) {
		/* destory the user_data_list */
		if (!LIST_IS_EMPTY(&bo->user_data_list)) {
			LIST_FOR_EACH_ENTRY_SAFE(old_data, tmp, &bo->user_data_list, item_link) {
				TBM_DBG("free user_data\n");
				user_data_delete(old_data);
			}
		}

		if (bo->lock_cnt > 0) {
			TBM_LOG_E("error lock_cnt:%d\n",
				bo->lock_cnt);
			_bo_unlock(bo);
		}

		/* call the bo_free */
		bufmgr->backend->bo_free(bo);
		bo->priv = NULL;

		LIST_DEL(&bo->item_link);
		free(bo);

		bufmgr->bo_cnt--;
	}

	_tbm_bufmgr_mutex_unlock();
}

tbm_bo
tbm_bo_alloc(tbm_bufmgr bufmgr, int size, int flags)
{
	tbm_bo bo = NULL;
	void *bo_priv = NULL;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(bufmgr), NULL);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(bufmgr == gBufMgr, NULL);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(size > 0, NULL);

	bo = calloc(1, sizeof(struct _tbm_bo));
	if (!bo) {
		TBM_TRACE("error: fail to create of tbm_bo size(%d) flag(%s)\n", size, tbm_flag_to_str(flags));
		_tbm_set_last_result(TBM_BO_ERROR_HEAP_ALLOC_FAILED);
		_tbm_bufmgr_mutex_unlock();
		return NULL;
	}

	_tbm_util_check_bo_cnt(bufmgr);
	bufmgr->bo_cnt++;

	bo->bufmgr = bufmgr;

	bo_priv = bufmgr->backend->bo_alloc(bo, size, flags);
	if (!bo_priv) {
		TBM_TRACE("error: fail to create of tbm_bo size(%d) flag(%s)\n", size, tbm_flag_to_str(flags));
		_tbm_set_last_result(TBM_BO_ERROR_BO_ALLOC_FAILED);
		free(bo);
		_tbm_bufmgr_mutex_unlock();
		return NULL;
	}

	bo->ref_cnt = 1;
	bo->flags = flags;
	bo->priv = bo_priv;

	TBM_TRACE("bo(%p) size(%d) refcnt(%d), flag(%s)\n", bo, size, bo->ref_cnt, tbm_flag_to_str(bo->flags));

	LIST_INITHEAD(&bo->user_data_list);

	LIST_ADD(&bo->item_link, &bufmgr->bo_list);

	_tbm_bufmgr_mutex_unlock();

	return bo;
}

tbm_bo
tbm_bo_import(tbm_bufmgr bufmgr, unsigned int key)
{
	tbm_bo bo = NULL;
	tbm_bo bo2 = NULL;
	tbm_bo tmp = NULL;
	void *bo_priv = NULL;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(bufmgr), NULL);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(bufmgr == gBufMgr, NULL);

	if (!bufmgr->backend->bo_import) {
		_tbm_bufmgr_mutex_unlock();
		return NULL;
	}

	_tbm_util_check_bo_cnt(bufmgr);

	bo = calloc(1, sizeof(struct _tbm_bo));
	if (!bo) {
		TBM_TRACE("error: fail to import of tbm_bo by key(%d)\n", key);
		_tbm_bufmgr_mutex_unlock();
		return NULL;
	}

	bufmgr->bo_cnt++;

	bo->bufmgr = bufmgr;

	bo_priv = bufmgr->backend->bo_import(bo, key);
	if (!bo_priv) {
		TBM_TRACE("error: fail to import of tbm_bo by key(%d)\n", key);
		_tbm_set_last_result(TBM_BO_ERROR_IMPORT_FAILED);
		free(bo);
		_tbm_bufmgr_mutex_unlock();
		return NULL;
	}

	if (!LIST_IS_EMPTY(&bufmgr->bo_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(bo2, tmp, &bufmgr->bo_list, item_link) {
			if (bo2->priv == bo_priv) {
				TBM_TRACE("find bo(%p) ref(%d) key(%d) flag(%s) in list\n",
							bo2, bo2->ref_cnt, key, tbm_flag_to_str(bo2->flags));
				bo2->ref_cnt++;
				free(bo);
				_tbm_bufmgr_mutex_unlock();
				return bo2;
			}
		}
	}

	bo->ref_cnt = 1;
	bo->priv = bo_priv;

	if (bufmgr->backend->bo_get_flags)
		bo->flags = bufmgr->backend->bo_get_flags(bo);
	else
		bo->flags = TBM_BO_DEFAULT;

	TBM_TRACE("import new bo(%p) ref(%d) key(%d) flag(%s) in list\n",
			  bo, bo->ref_cnt, key, tbm_flag_to_str(bo->flags));

	LIST_INITHEAD(&bo->user_data_list);

	LIST_ADD(&bo->item_link, &bufmgr->bo_list);

	_tbm_bufmgr_mutex_unlock();

	return bo;
}

tbm_bo
tbm_bo_import_fd(tbm_bufmgr bufmgr, tbm_fd fd)
{
	tbm_bo bo = NULL;
	tbm_bo bo2 = NULL;
	tbm_bo tmp = NULL;
	void *bo_priv = NULL;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(bufmgr), NULL);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(bufmgr == gBufMgr, NULL);

	if (!bufmgr->backend->bo_import_fd) {
		_tbm_bufmgr_mutex_unlock();
		return NULL;
	}

	_tbm_util_check_bo_cnt(bufmgr);

	bo = calloc(1, sizeof(struct _tbm_bo));
	if (!bo) {
		TBM_TRACE("error: fail to import tbm_bo by tbm_fd(%d)\n", fd);
		_tbm_bufmgr_mutex_unlock();
		return NULL;
	}

	bufmgr->bo_cnt++;

	bo->bufmgr = bufmgr;

	bo_priv = bufmgr->backend->bo_import_fd(bo, fd);
	if (!bo_priv) {
		TBM_TRACE("error: fail to import tbm_bo by tbm_fd(%d)\n", fd);
		_tbm_set_last_result(TBM_BO_ERROR_IMPORT_FD_FAILED);
		free(bo);
		_tbm_bufmgr_mutex_unlock();
		return NULL;
	}

	if (!LIST_IS_EMPTY(&bufmgr->bo_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(bo2, tmp, &bufmgr->bo_list, item_link) {
			if (bo2->priv == bo_priv) {
				TBM_TRACE("find bo(%p) ref(%d) fd(%d) flag(%s) in list\n",
							bo2, bo2->ref_cnt, fd, tbm_flag_to_str(bo2->flags));
				bo2->ref_cnt++;
				free(bo);
				_tbm_bufmgr_mutex_unlock();
				return bo2;
			}
		}
	}

	bo->ref_cnt = 1;
	bo->priv = bo_priv;

	if (bufmgr->backend->bo_get_flags)
		bo->flags = bufmgr->backend->bo_get_flags(bo);
	else
		bo->flags = TBM_BO_DEFAULT;

	TBM_TRACE("import bo(%p) ref(%d) fd(%d) flag(%s)in list\n",
				bo, bo->ref_cnt, fd, tbm_flag_to_str(bo->flags));

	LIST_INITHEAD(&bo->user_data_list);

	LIST_ADD(&bo->item_link, &bufmgr->bo_list);

	_tbm_bufmgr_mutex_unlock();

	return bo;
}

tbm_key
tbm_bo_export(tbm_bo bo)
{
	tbm_bufmgr bufmgr = NULL;
	tbm_key ret;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	bufmgr = gBufMgr;

	if (!bufmgr->backend->bo_export) {
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	ret = bufmgr->backend->bo_export(bo);
	if (!ret) {
		_tbm_set_last_result(TBM_BO_ERROR_EXPORT_FAILED);
		TBM_TRACE("error: bo(%p) tbm_key(%d)\n", bo, ret);
		_tbm_bufmgr_mutex_unlock();
		return ret;
	}

	TBM_TRACE("bo(%p) tbm_key(%d)\n", bo, ret);

	_tbm_bufmgr_mutex_unlock();

	return ret;
}

tbm_fd
tbm_bo_export_fd(tbm_bo bo)
{
	tbm_bufmgr bufmgr = NULL;
	int ret;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), -1);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), -1);

	bufmgr = gBufMgr;

	if (!bufmgr->backend->bo_export_fd) {
		_tbm_bufmgr_mutex_unlock();
		return -1;
	}

	ret = bufmgr->backend->bo_export_fd(bo);
	if (ret < 0) {
		_tbm_set_last_result(TBM_BO_ERROR_EXPORT_FD_FAILED);
		TBM_TRACE("error: bo(%p) tbm_fd(%d)\n", bo, ret);
		_tbm_bufmgr_mutex_unlock();
		return ret;
	}

	TBM_TRACE("bo(%p) tbm_fd(%d)\n", bo, ret);

	_tbm_bufmgr_mutex_unlock();

	return ret;
}

tbm_bo_handle
tbm_bo_get_handle(tbm_bo bo, int device)
{
	tbm_bufmgr bufmgr = NULL;
	tbm_bo_handle bo_handle;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), (tbm_bo_handle) NULL);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), (tbm_bo_handle) NULL);

	bufmgr = gBufMgr;

	bo_handle = bufmgr->backend->bo_get_handle(bo, device);
	if (bo_handle.ptr == NULL) {
		_tbm_set_last_result(TBM_BO_ERROR_GET_HANDLE_FAILED);
		TBM_TRACE("error: bo(%p) bo_handle(%p)\n", bo, bo_handle.ptr);
		_tbm_bufmgr_mutex_unlock();
		return (tbm_bo_handle) NULL;
	}

	TBM_TRACE("bo(%p) bo_handle(%p)\n", bo, bo_handle.ptr);

	_tbm_bufmgr_mutex_unlock();

	return bo_handle;
}

tbm_bo_handle
tbm_bo_map(tbm_bo bo, int device, int opt)
{
	tbm_bufmgr bufmgr = NULL;
	tbm_bo_handle bo_handle;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), (tbm_bo_handle) NULL);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), (tbm_bo_handle) NULL);

	bufmgr = gBufMgr;

	if (!_tbm_bo_lock(bo, device, opt)) {
		_tbm_set_last_result(TBM_BO_ERROR_LOCK_FAILED);
		TBM_TRACE("error: fail to lock bo:%p)\n", bo);
		_tbm_bufmgr_mutex_unlock();
		return (tbm_bo_handle) NULL;
	}

	bo_handle = bufmgr->backend->bo_map(bo, device, opt);
	if (bo_handle.ptr == NULL) {
		_tbm_set_last_result(TBM_BO_ERROR_MAP_FAILED);
		TBM_TRACE("error: fail to map bo:%p\n", bo);
		_tbm_bo_unlock(bo);
		_tbm_bufmgr_mutex_unlock();
		return (tbm_bo_handle) NULL;
	}

	/* increase the map_count */
	bo->map_cnt++;

	TBM_TRACE("bo(%p) map_cnt(%d)\n", bo, bo->map_cnt);

	_tbm_bufmgr_mutex_unlock();

	return bo_handle;
}

int
tbm_bo_unmap(tbm_bo bo)
{
	tbm_bufmgr bufmgr = NULL;
	int ret;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	bufmgr = gBufMgr;

	ret = bufmgr->backend->bo_unmap(bo);
	if (!ret) {
		TBM_TRACE("error: bo(%p) map_cnt(%d)\n", bo, bo->map_cnt);
		_tbm_set_last_result(TBM_BO_ERROR_UNMAP_FAILED);
		_tbm_bufmgr_mutex_unlock();
		return ret;
	}

	/* decrease the map_count */
	bo->map_cnt--;

	TBM_TRACE("bo(%p) map_cnt(%d)\n", bo, bo->map_cnt);

	_tbm_bo_unlock(bo);

	_tbm_bufmgr_mutex_unlock();

	return ret;
}

int
tbm_bo_swap(tbm_bo bo1, tbm_bo bo2)
{
	tbm_bufmgr bufmgr = NULL;
	void *temp;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo1), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo2), 0);

	bufmgr = gBufMgr;

	TBM_TRACE("before: bo1(%p) bo2(%p)\n", bo1, bo2);

	if (bufmgr->backend->bo_size(bo1) != bufmgr->backend->bo_size(bo2)) {
		_tbm_set_last_result(TBM_BO_ERROR_SWAP_FAILED);
		TBM_TRACE("error: bo1(%p) bo2(%p)\n", bo1, bo2);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	TBM_TRACE("after: bo1(%p) bo2(%p)\n", bo1, bo2);

	temp = bo1->priv;
	bo1->priv = bo2->priv;
	bo2->priv = temp;

	_tbm_bufmgr_mutex_unlock();

	return 1;
}

int
tbm_bo_locked(tbm_bo bo)
{
	tbm_bufmgr bufmgr = NULL;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	bufmgr = gBufMgr;

	if (bufmgr->lock_type == LOCK_TRY_NEVER) {
		TBM_TRACE("bo(%p) lock_cnt(%d)\n", bo, bo->lock_cnt);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	if (bo->lock_cnt > 0) {
		TBM_TRACE("error: bo(%p) lock_cnt(%d)\n", bo, bo->lock_cnt);
		_tbm_bufmgr_mutex_unlock();
		return 1;
	}

	TBM_TRACE("bo(%p) lock_cnt(%d)\n", bo, bo->lock_cnt);
	_tbm_bufmgr_mutex_unlock();

	return 0;
}

int
tbm_bo_add_user_data(tbm_bo bo, unsigned long key,
		     tbm_data_free data_free_func)
{
	tbm_user_data *data;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	/* check if the data according to the key exist if so, return false. */
	data = user_data_lookup(&bo->user_data_list, key);
	if (data) {
		TBM_TRACE("warning: user data already exist key(%ld)\n", key);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	data = user_data_create(key, data_free_func);
	if (!data) {
		TBM_TRACE("error: bo(%p) key(%lu)\n", bo, key);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	TBM_TRACE("bo(%p) key(%lu) data(%p)\n", bo, key, data->data);

	LIST_ADD(&data->item_link, &bo->user_data_list);

	_tbm_bufmgr_mutex_unlock();

	return 1;
}

int
tbm_bo_set_user_data(tbm_bo bo, unsigned long key, void *data)
{
	tbm_user_data *old_data;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	if (LIST_IS_EMPTY(&bo->user_data_list)) {
		TBM_TRACE("error: bo(%p) key(%lu)\n", bo, key);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	old_data = user_data_lookup(&bo->user_data_list, key);
	if (!old_data) {
		TBM_TRACE("error: bo(%p) key(%lu)\n", bo, key);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	if (old_data->data && old_data->free_func)
		old_data->free_func(old_data->data);

	old_data->data = data;

	TBM_TRACE("bo(%p) key(%lu) data(%p)\n", bo, key, old_data->data);

	_tbm_bufmgr_mutex_unlock();

	return 1;
}

int
tbm_bo_get_user_data(tbm_bo bo, unsigned long key, void **data)
{
	tbm_user_data *old_data;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	if (!data || LIST_IS_EMPTY(&bo->user_data_list)) {
		TBM_TRACE("error: bo(%p) key(%lu)\n", bo, key);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	old_data = user_data_lookup(&bo->user_data_list, key);
	if (!old_data) {
		TBM_TRACE("error: bo(%p) key(%lu)\n", bo, key);
		*data = NULL;
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	*data = old_data->data;

	TBM_TRACE("bo(%p) key(%lu) data(%p)\n", bo, key, old_data->data);

	_tbm_bufmgr_mutex_unlock();

	return 1;
}

int
tbm_bo_delete_user_data(tbm_bo bo, unsigned long key)
{
	tbm_user_data *old_data = (void *)0;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	if (LIST_IS_EMPTY(&bo->user_data_list)) {
		TBM_TRACE("error: bo(%p) key(%lu)\n", bo, key);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	old_data = user_data_lookup(&bo->user_data_list, key);
	if (!old_data) {
		TBM_TRACE("error: bo(%p) key(%lu)\n", bo, key);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	TBM_TRACE("bo(%p) key(%lu) data(%p)\n", bo, key, old_data->data);

	user_data_delete(old_data);

	_tbm_bufmgr_mutex_unlock();

	return 1;
}

unsigned int
tbm_bufmgr_get_capability(tbm_bufmgr bufmgr)
{
	int capabilities = TBM_BUFMGR_CAPABILITY_NONE;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(bufmgr), TBM_BUFMGR_CAPABILITY_NONE);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(bufmgr == gBufMgr, TBM_BUFMGR_CAPABILITY_NONE);

	TBM_TRACE("tbm_bufmgr(%p) capability(%d)\n", bufmgr, bufmgr->capabilities);

	capabilities = bufmgr->capabilities;

	_tbm_bufmgr_mutex_unlock();

	return capabilities;
}

int
tbm_bo_get_flags(tbm_bo bo)
{
	int flags;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	flags = bo->flags;

	TBM_TRACE("bo(%p)\n", bo);

	_tbm_bufmgr_mutex_unlock();

	return flags;
}

/* LCOV_EXCL_START */
tbm_error_e
tbm_get_last_error(void)
{
	return tbm_last_error;
}

void
tbm_bufmgr_debug_show(tbm_bufmgr bufmgr)
{
	tbm_bo bo = NULL, tmp_bo = NULL;
	int bo_cnt = 0;
	tbm_surface_h surf = NULL, tmp_surf = NULL;
	int surf_cnt = 0;
	int i;
	char app_name[255] = {0,};
	unsigned int pid = 0;
	char title[255] = {0,};
	char data[255] = {0,};
	tbm_surface_debug_data *debug_old_data = NULL, *debug_tmp = NULL;

	pthread_mutex_lock(&gLock);

	if (!TBM_BUFMGR_IS_VALID(bufmgr) || (bufmgr != gBufMgr)) {
		TBM_LOG_E("invalid bufmgr\n");
		pthread_mutex_unlock(&gLock);
		return;
	}

	TBM_DEBUG("\n");
	_tbm_util_get_appname_from_pid(getpid(), app_name);
	_tbm_util_get_appname_brief(app_name);
	TBM_DEBUG("============TBM DEBUG: %s(%d)===========================\n",
		  app_name, getpid());
	memset(app_name, 0x0, 255 * sizeof(char));
	snprintf(title, 255, "%s", "no  surface     refcnt  width  height  bpp  size    n_b  n_p  flags  format    app_name       ");
	if (!LIST_IS_EMPTY(&bufmgr->debug_key_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(debug_old_data, debug_tmp, &bufmgr->debug_key_list, item_link) {
			strncat(title, "  ", 3);
			strncat(title, debug_old_data->key, strlen(debug_old_data->key) + 1);
		}
	}

	TBM_DEBUG("[tbm_surface information]\n");
	TBM_DEBUG("%s\n", title);
	/* show the tbm_surface information in surf_list */
	if (!LIST_IS_EMPTY(&bufmgr->surf_list)) {
		char *value = NULL;

		LIST_FOR_EACH_ENTRY_SAFE(surf, tmp_surf, &bufmgr->surf_list, item_link) {
			pid = _tbm_surface_internal_get_debug_pid(surf);
			if (!pid) {
				/* if pid is null, set the self_pid */
				pid = getpid();
			}

			_tbm_util_get_appname_from_pid(pid, app_name);
			_tbm_util_get_appname_brief(app_name);

			snprintf(data, 255, "%-2d  %-9p    %-4d  %-5d  %-6d  %-3d  %-6d   %-2d   %-2d    %-3d  %-8s  %-15s",
				  ++surf_cnt,
				  surf,
				  surf->refcnt,
				  surf->info.width,
				  surf->info.height,
				  surf->info.bpp,
				  surf->info.size / 1024,
				  surf->num_bos,
				  surf->num_planes,
				  surf->flags,
				  _tbm_surface_internal_format_to_str(surf->info.format) + 11,
				  app_name);

			if (!LIST_IS_EMPTY(&bufmgr->debug_key_list)) {
				LIST_FOR_EACH_ENTRY_SAFE(debug_old_data, debug_tmp, &bufmgr->debug_key_list, item_link) {
					strncat(data, "  ", 3);

					value = _tbm_surface_internal_get_debug_data(surf, debug_old_data->key);
					if (value)
						strncat(data, value, strlen(value) + 1);
					else
						strncat(data, "none", 5);
				}
			}
			TBM_DEBUG("%s\n", data);

			for (i = 0; i < surf->num_bos; i++) {
				TBM_DEBUG(" bo:%-12p  %-26d%-10d\n",
					  surf->bos[i],
					  surf->bos[i]->ref_cnt,
					  tbm_bo_size(surf->bos[i]) / 1024);
			}

			memset(app_name, 0x0, 255 * sizeof(char));
		}
	} else {
		TBM_DEBUG("no tbm_surfaces.\n");
	}
	TBM_DEBUG("\n");

	TBM_DEBUG("[tbm_bo information]\n");
	TBM_DEBUG("no  bo          refcnt  size    lock_cnt  map_cnt  flags  surface\n");

	/* show the tbm_bo information in bo_list */
	if (!LIST_IS_EMPTY(&bufmgr->bo_list)) {
		LIST_FOR_EACH_ENTRY_SAFE(bo, tmp_bo, &bufmgr->bo_list, item_link) {
			TBM_DEBUG("%-4d%-11p   %-4d  %-6d     %-5d     %-4d    %-3d  %-11p\n",
				  ++bo_cnt,
				  bo,
				  bo->ref_cnt,
				  tbm_bo_size(bo) / 1024,
				  bo->lock_cnt,
				  bo->map_cnt,
				  bo->flags,
				  bo->surface);
		}
	} else {
		TBM_DEBUG("no tbm_bos.\n");
	}
	TBM_DEBUG("\n");

	TBM_DEBUG("===============================================================\n");

	pthread_mutex_unlock(&gLock);
}

void
tbm_bufmgr_debug_trace(tbm_bufmgr bufmgr, int onoff)
{
	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_IF_FAIL(TBM_BUFMGR_IS_VALID(bufmgr));
	TBM_BUFMGR_RETURN_IF_FAIL(bufmgr == gBufMgr);

#ifdef TRACE
	TBM_LOG_D("bufmgr=%p onoff=%d\n", bufmgr, onoff);
	bTrace = onoff;
#endif

	_tbm_bufmgr_mutex_unlock();
}

int
tbm_bufmgr_debug_queue_dump(char *path, int count, int onoff)
{
	int w = 0, h = 0;

	TBM_RETURN_VAL_IF_FAIL(path != NULL, 0);
	TBM_LOG_D("path=%s count=%d onoff=%d\n", path, count, onoff);

	pthread_mutex_lock(&gLock);

	if (onoff == 1) {
		if (_tbm_util_get_max_surface_size(&w, &h) == 0) {
			TBM_LOG_I("No tbm_surface.\n");
			pthread_mutex_unlock(&gLock);
			return 0;
		}

		tbm_surface_internal_dump_start(path, w, h, count);
		b_dump_queue = 1;
	} else if (onoff == 0) {
		tbm_surface_internal_dump_end();
		b_dump_queue = 0;
	} else {
		pthread_mutex_unlock(&gLock);
		return 0;
	}

	pthread_mutex_unlock(&gLock);
	return 1;
}

int
tbm_bufmgr_debug_dump_all(char *path)
{
	int w = 0, h = 0, count = 0;
	tbm_surface_h surface = NULL, tmp = NULL;

	TBM_RETURN_VAL_IF_FAIL(path != NULL, 0);
	TBM_LOG_D("path=%s\n", path);

	pthread_mutex_lock(&gLock);

	count = _tbm_util_get_max_surface_size(&w, &h);
	if (count == 0) {
		TBM_LOG_I("No tbm_surface.\n");
		pthread_mutex_unlock(&gLock);
		return 1;
	}

	tbm_surface_internal_dump_start(path, w, h, count);

	LIST_FOR_EACH_ENTRY_SAFE(surface, tmp, &gBufMgr->surf_list, item_link) {
		tbm_surface_internal_dump_buffer(surface, "dump_all");
	}

	tbm_surface_internal_dump_end();

	pthread_mutex_unlock(&gLock);

	return 1;
}

/* internal function */
tbm_bufmgr
_tbm_bufmgr_get_bufmgr(void)
{
	return gBufMgr;
}

int
_tbm_bo_set_surface(tbm_bo bo, tbm_surface_h surface)
{
	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);
	TBM_BUFMGR_RETURN_VAL_IF_FAIL(_tbm_bo_is_valid(bo), 0);

	bo->surface = surface;

	_tbm_bufmgr_mutex_unlock();

	return 1;
}

int
tbm_bufmgr_bind_native_display(tbm_bufmgr bufmgr, void *NativeDisplay)
{
	int ret;

	_tbm_bufmgr_mutex_lock();

	TBM_BUFMGR_RETURN_VAL_IF_FAIL(TBM_BUFMGR_IS_VALID(gBufMgr), 0);

	if (!bufmgr->backend->bufmgr_bind_native_display) {
		TBM_TRACE("error: tbm_bufmgr(%p) NativeDisplay(%p)\n", bufmgr, NativeDisplay);
		_tbm_bufmgr_mutex_unlock();
		return 1;
	}

	ret = bufmgr->backend->bufmgr_bind_native_display(bufmgr, NativeDisplay);
	if (!ret) {
		TBM_TRACE("error: tbm_bufmgr(%p) NativeDisplay(%p)\n", bufmgr, NativeDisplay);
		_tbm_bufmgr_mutex_unlock();
		return 0;
	}

	TBM_TRACE("tbm_bufmgr(%p) NativeDisplay(%p)\n", bufmgr, NativeDisplay);

	_tbm_bufmgr_mutex_unlock();

	return 1;
}
/* LCOV_EXCL_STOP */
