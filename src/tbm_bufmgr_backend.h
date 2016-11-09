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

/*
 * This file is referenced by the xf86Module.h in xorg server.
 */

#ifndef _TBM_BUFMGR_BACKEND_H_
#define _TBM_BUFMGR_BACKEND_H_

#include <tbm_bufmgr.h>
#include <pthread.h>

/**
 * \file tbm_bufmgr_backend.h
 * \brief backend header for Tizen Buffer Manager
 *   This header is for the implementation of the TBM backend module.
 */

#define ABI_MINOR_MASK		0x0000FFFF
#define ABI_MAJOR_MASK		0xFFFF0000
#define GET_ABI_MINOR(v)	((v) & ABI_MINOR_MASK)
#define GET_ABI_MAJOR(v)	(((v) & ABI_MAJOR_MASK) >> 16)

/*
 * ABI versions.  Each version has a major and minor revision.  Modules
 * using lower minor revisions must work with servers of a higher minor
 * revision.  There is no compatibility between different major revisions.
 * Whenever the ABI_ANSIC_VERSION is changed, the others must also be
 * changed.  The minor revision mask is 0x0000FFFF and the major revision
 * mask is 0xFFFF0000.
 */
#define SET_ABI_VERSION(maj, min) \
		((((maj) << 16) & ABI_MAJOR_MASK) | ((min) & ABI_MINOR_MASK))

#define TBM_ABI_VERSION	SET_ABI_VERSION(1, 1) /**< current abi vertion  */

typedef struct _tbm_bufmgr_backend *tbm_bufmgr_backend;

/**
 * @brief TBM backend functions
 *  the set of function pointers for the backend module of TBM.
 */
struct _tbm_bufmgr_backend {
	int flags;

	void *priv;	/**< bufmgr private */

	/**
	* @brief deinitialize the bufmgr private.
	* @param[in] priv : the private of the bufmgr
	*/
	void (*bufmgr_deinit)(void *priv);

	/**
	* @brief get the size of a bo.
	* @param[in] bo : the buffer object
	* @return size if this function succeeds, otherwise 0.
	*/
	int (*bo_size)(tbm_bo bo);

	/**
	* @brief allocate the buffer object
	* @param[in] bo : the buffer object
	* @param[in] size : the size of buffer object
	* @param[in] flags : the flags of memory type
	* @return pointer of the bo private.
	*/
	void *(*bo_alloc)(tbm_bo bo, int size, int flags);

	/**
	* @brief free the buffer object.
	* @param[in] bo : the buffer object
	*/
	void (*bo_free)(tbm_bo bo);

	/**
	* @brief import the buffer object associated with the key.
	* @remarks If the backend doesn't support a buffer sharing by tbm key,
			fucntion pointer must be set to NULL.
	* @param[in] bo : the buffer object
	* @param[in] key : the key associated with the buffer object
	* @return pointer of the bo private.
	*/
	void *(*bo_import)(tbm_bo bo, unsigned int key);

	/**
	* @brief export the buffer object
	* @remarks If the backend doesn't support a buffer sharing by tbm key,
			fucntion pointer must be set to NULL.
	* @param[in] bo : the buffer object
	* @return key associated with the buffer object
	*/
	unsigned int (*bo_export)(tbm_bo bo);

	/**
	* @brief get the tbm_bo_handle according to the device type.
	* @param[in] bo : the buffer object
	* @param[in] device : the device type to get a handle
	* @return the handle of the buffer object
	*/
	tbm_bo_handle(*bo_get_handle)(tbm_bo bo, int device);

	/**
	* @brief map the buffer object according to the device type and the option.
	* @param[in] bo : the buffer object
	* @param[in] device : the device type to get a handle
	* @param[in] opt : the option to access the buffer object
	* @return the handle of the buffer object
	*/
	tbm_bo_handle(*bo_map)(tbm_bo bo, int device, int opt);

	/**
	* @brief unmap the buffer object.
	* @param[in] bo : the buffer object
	* @return 1 if this function succeeds, otherwise 0.
	*/
	int (*bo_unmap)(tbm_bo bo);

	/**
	* @brief unlock the buffer object.
	* @param[in] bo : the buffer object
	* @return 1 if this function succeeds, otherwise 0.
	* @remark This function pointer could be null. (default: use the tizen global lock)
	*/
	int (*bo_unlock)(tbm_bo bo);

	/**
	* @brief lock the buffer object with a device and an opt.
	* @param[in] bo : the buffer object
	* @param[in] device : the device type to get a handle
	* @param[in] opt : the option to access the buffer object
	* @return 1 if this function succeeds, otherwise 0.
	* @remark This function pointer could be null. (default: use the tizen global lock)
	*/
	int (*bo_lock)(tbm_bo bo, int device, int opt);

	/**
	* @brief query the formats list and the num to be supported by backend.
	* @param[out] **formats : format array list. this array has to be allocated by backend funtion
	* @param[out] *num : the number of the formats to be supported by backend
	* @return 1 if this function succeeds, otherwise 0.
	*/
	int (*surface_supported_format)(uint32_t **formats, uint32_t *num);

	/**
	* @brief get the plane data of the surface.
	* @param[in] width : the width of the surface
	* @param[in] height : the height of the surface
	* @param[in] format : the format of the surface
	* @param[in] plane_idx : the format of the surface
	* @param[out] size : the size of the plane
	* @param[out] offset : the offset of the plane
	* @param[out] pitch : the pitch of the plane
	* @param[out] bo_idx : the bo index of the plane
	* @return 1 if this function succeeds, otherwise 0.
	*/
	int (*surface_get_plane_data)(int width, int height,
				      tbm_format format, int plane_idx, uint32_t *size, uint32_t *offset,
				      uint32_t *pitch, int *bo_idx);

	/**
	* @brief import the buffer object associated with the prime fd.
	* @remarks tbm_fd must be free by user.
	* @remarks If the backend doesn't support a buffer sharing by tbm fd,
			fucntion pointer must be set to NULL.
	* @param[in] bo : the buffer object
	* @param[in] fd : the prime fd associated with the buffer object
	* @return pointer of the bo private.
	*/
	void *(*bo_import_fd)(tbm_bo bo, tbm_fd fd);

	/**
	* @brief export the buffer object
	* @remarks tbm_fd must be free by user.
	* @remarks If the backend doesn't support a buffer sharing by tbm fd,
			fucntion pointer must be set to NULL.
	* @param[in] bo : the buffer object
	* @return tbm_fd associated with the buffer object
	*/
	tbm_fd (*bo_export_fd)(tbm_bo bo);

	/**
	* @brief get the tbm flags of memory type
	* @param[in] bo : the buffer object
	* @see #TBM_BO_FLAGS
	* @return tbm flags of memory type is this function succeeds, otherwise 0.
	*/
	int (*bo_get_flags)(tbm_bo bo);

	/**
	* @brief get(bind) the native display
	* @param[in] bufmgr : the buffer object
	* @param[in] NativeDisplay : the native display
	* @return 1 if this function succeeds, otherwise 0.
	*/
	int (*bufmgr_bind_native_display)(tbm_bufmgr bufmgr, void *NativeDisplay);

	/**
	* @brief allocate the buffer object for tbm surface
	* @param[in] bo : the buffer object
	* @param[in] width : the width of surface
	* @param[in] height : the height of surface
	* @param[in] format : the format of surface
	* @param[in] flags : the flags of memory type
	* @param[in] bo_idx : the index of bo in surface
	* @return pointer of the bo private.
	*/
	void * (*surface_bo_alloc)(tbm_bo bo, int width, int height, int format, int flags, int bo_idx);

	/* Padding for future extension */
	void (*reserved1)(void);
	void (*reserved2)(void);
	void (*reserved3)(void);
	void (*reserved4)(void);
	void (*reserved5)(void);
	void (*reserved6)(void);
};

/**
 * @brief tbm module information
 *  data type for the module information
 */
typedef struct {
	const char *modname;		   /**< name of module, e.g. "foo" */
	const char *vendor;			   /**< vendor specific string */
	unsigned long abiversion;	   /**< ABI version */
} TBMModuleVersionInfo;

typedef int (*ModuleInitProc) (tbm_bufmgr, int);

#define MODULEINITPPROTO(func) int func(tbm_bufmgr, int) /**< prototype for init symbol of bakcend module */

/**
 * @brief tbm module data
 *  data type for the entry point of the backend module
 */
typedef struct {
	TBMModuleVersionInfo *vers;	/**< tbm module informtaion */
	ModuleInitProc init;		/**< init function of a backend module */
} TBMModuleData;

tbm_bufmgr_backend tbm_backend_alloc(void);
void tbm_backend_free(tbm_bufmgr_backend backend);
int tbm_backend_init(tbm_bufmgr bufmgr, tbm_bufmgr_backend backend);

void *tbm_backend_get_bufmgr_priv(tbm_bo bo);
void *tbm_backend_get_priv_from_bufmgr(tbm_bufmgr bufmgr);
void *tbm_backend_get_bo_priv(tbm_bo bo);
int tbm_backend_is_display_server(void);

#endif							/* _TBM_BUFMGR_BACKEND_H_ */
