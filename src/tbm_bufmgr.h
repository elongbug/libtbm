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

#ifndef _TBM_BUFMGR_H_
#define _TBM_BUFMGR_H_

#include <tbm_type.h>
#include <stdint.h>

/* tbm error base : this error base is same as TIZEN_ERROR_TBM in tizen_error.h */
#ifndef TBM_ERROR_BASE
#define TBM_ERROR_BASE			-0x02830000
#endif

/**
 * \file tbm_bufmgr.h
 * \brief Tizen Buffer Manager
 */

/**
 * @brief Definition for the tizen buffer manager
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct _tbm_bufmgr *tbm_bufmgr;

/**
 * @brief Definition for the tizen buffer object
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct _tbm_bo *tbm_bo;
/**
 * @brief Definition for the key associated with the buffer object
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef uint32_t tbm_key;
/**
 * @brief Definition for the file descripter of the system buffer manager
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef int32_t tbm_fd;

/* TBM_DEVICE_TYPE */

/**
 * @brief Definition for the device type to get the default handle
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_DEVICE_DEFAULT   0
/**
 * @brief Definition for the device type to get the virtual memory
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_DEVICE_CPU       1
/**
 * @brief Definition for the device type to get the 2D memory handle
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_DEVICE_2D        2
/**
 * @brief Definition for the device type to get the 3D memory handle
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_DEVICE_3D        3
/**
 * @brief Definition for the device type to get the multimedia handle
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_DEVICE_MM        4

/* TBM_OPTION */

/**
 * @brief Definition for the access option to read
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_OPTION_READ      (1 << 0)
/**
 * @brief Definition for the access option to write
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_OPTION_WRITE     (1 << 1)
/**
 * @brief Definition for the vendor specific option that depends on the backend
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_OPTION_VENDOR    (0xffff0000)

/**
 * @brief tbm_bo_handle abstraction of the memory handle by TBM_DEVICE_TYPE
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef union _tbm_bo_handle {
	void *ptr;
	int32_t s32;
	uint32_t u32;
	int64_t s64;
	uint64_t u64;
} tbm_bo_handle;

/**
 * @brief Enumeration of bo memory type
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
enum TBM_BO_FLAGS {
	TBM_BO_DEFAULT = 0,			   /**< default memory: it depends on the backend         */
	TBM_BO_SCANOUT = (1 << 0),	   /**< scanout memory                                    */
	TBM_BO_NONCACHABLE = (1 << 1), /**< non-cachable memory                               */
	TBM_BO_WC = (1 << 2),		   /**< write-combine memory                              */
	TBM_BO_VENDOR = (0xffff0000), /**< vendor specific memory: it depends on the backend */
};

/**
 * @brief Enumeration for tbm error type.
 * @since_tizen 2.4
 */
typedef enum {
	TBM_ERROR_NONE = 0,						/**< Successful */
	TBM_BO_ERROR_GET_FD_FAILED = TBM_ERROR_BASE | 0x0101,	  /**< failed to get fd failed */
	TBM_BO_ERROR_HEAP_ALLOC_FAILED = TBM_ERROR_BASE | 0x0102, /**< failed to allocate the heap memory */
	TBM_BO_ERROR_LOAD_MODULE_FAILED = TBM_ERROR_BASE | 0x0103,/**< failed to load module*/
	TBM_BO_ERROR_THREAD_INIT_FAILED = TBM_ERROR_BASE | 0x0104,/**< failed to initialize the pthread */
	TBM_BO_ERROR_BO_ALLOC_FAILED = TBM_ERROR_BASE | 0x0105,	  /**< failed to allocate tbm_bo */
	TBM_BO_ERROR_INIT_STATE_FAILED = TBM_ERROR_BASE | 0x0106, /**< failed to initialize the state of tbm_bo */
	TBM_BO_ERROR_IMPORT_FAILED = TBM_ERROR_BASE | 0x0107,	  /**< failed to import the handle of tbm_bo */
	TBM_BO_ERROR_IMPORT_FD_FAILED = TBM_ERROR_BASE | 0x0108,  /**< failed to import fd of tbm_bo */
	TBM_BO_ERROR_EXPORT_FAILED = TBM_ERROR_BASE | 0x0109,	  /**< failed to export the handle of the tbm_bo */
	TBM_BO_ERROR_EXPORT_FD_FAILED = TBM_ERROR_BASE | 0x01010, /**< failed to export fd of tbm_bo */
	TBM_BO_ERROR_GET_HANDLE_FAILED = TBM_ERROR_BASE | 0x0111, /**< failed to get the tbm_bo_handle */
	TBM_BO_ERROR_LOCK_FAILED = TBM_ERROR_BASE | 0x0112,		  /**< failed to lock the tbm_bo */
	TBM_BO_ERROR_MAP_FAILED = TBM_ERROR_BASE | 0x0113,		  /**< failed to map the tbm_bo to get the tbm_bo_handle */
	TBM_BO_ERROR_UNMAP_FAILED = TBM_ERROR_BASE | 0x0114,	  /**< failed to unmap the tbm_bo */
	TBM_BO_ERROR_SWAP_FAILED = TBM_ERROR_BASE | 0x0115,		  /**< failed to swap the tbm_bos */
	TBM_BO_ERROR_DUP_FD_FAILED = TBM_ERROR_BASE | 0x0116,	  /**< failed to duplicate fd */
} tbm_error_e;

/**
 * @brief Enumeration of tbm buffer manager capability.
 * @since_tizen 2.4
 */
enum TBM_BUFMGR_CAPABILITY {
	TBM_BUFMGR_CAPABILITY_NONE = 0,					/**< Not Support capability*/
	TBM_BUFMGR_CAPABILITY_SHARE_KEY = (1 << 0),		/**< Support sharing buffer by tbm key */
	TBM_BUFMGR_CAPABILITY_SHARE_FD = (1 << 1),		/**< Support sharing buffer by tbm fd */
	TBM_BUFMGR_CAPABILITY_TBM_SYNC = (1 << 2),		/**< Support tbm sync */
};

#ifdef __cplusplus
extern "C" {
#endif

/* Functions for buffer manager */

/**
 * @brief Initializes the buffer manager.
 * @details If fd is lower than zero, fd is get drm fd in tbm_bufmgr_init function\n
 * The user can decide the lock type and cache flush type with the environment variables, which are BUFMGR_LOCK_TYPE and BUFMGR_MAP_CACHE.\n
 * \n
 * BUFMGR_LOCK default is once\n
 * once : The previous bo which is locked is unlock when the new bo is trying to be locked\n
 * always : The new bo is locked until the previous bo which is locked is unlocked\n
 * never : Every bo is never locked.\n
 * \n
 * BUFMGR_MAP_CACHE default is true\n
 * true : use map cache flushing\n
 * false : to use map cache flushing
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] fd : file descripter of the system buffer manager
 * @return a buffer manager
 * @retval #tbm_bufmgr
 * @see tbm_bufmgr_deinit();
 * @par Example
   @code
   #include <tbm_bufmgr.h>
   int bufmgr_fd;

   setenv("BUFMGR_LOCK_TYPE", "once", 1);
   setenv("BUFMGR_MAP_CACHE", "true", 1);

   tbm_bufmgr bufmgr;
   bufmgr = tbm_bufmgr_init (bufmgr_fd);

   ....

   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
tbm_bufmgr tbm_bufmgr_init(int fd);

/**
 * @brief Deinitializes the buffer manager.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bufmgr : the buffer manager
 * @see tbm_bufmgr_init()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_error_e error;
   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   if (!bufmgr)
   {
      error = tbm_get_last_error ();
      ...
   }

   ....

   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
void tbm_bufmgr_deinit(tbm_bufmgr bufmgr);

/* Functions for bo */

/**
 * @brief Allocates the buffer object.
 * @details This function create tbm_bo and set reference count to 1.\n
 * The user can craete tbm_bo with memory type flag #TBM_BO_FLAGS\n\n
 * #TBM_BO_DEFAULT indecates default memory: it depends on the backend\n
 * #TBM_BO_SCANOUT indecates scanout memory\n
 * #TBM_BO_NONCACHABLE indecates non-cachable memory\n
 * #TBM_BO_WC indecates write-combine memory\n
 * #TBM_BO_VENDOR indecates vendor specific memory: it depends on the tbm backend
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bufmgr : the buffer manager
 * @param[in] size : the size of buffer object
 * @param[in] flags : the flags of memory type
 * @return a buffer object
 * @retval #tbm_bo
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo;
   tbm_error_e error;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
   if (!bo)
   {
      error = tbm_get_last_error ();
      ...
   }

   ....

   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
tbm_bo tbm_bo_alloc(tbm_bufmgr bufmgr, int size, int flags);

/**
 * @brief Increases the reference count of bo.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @return a buffer object
 * @retval #tbm_bo
 * @see tbm_bo_unref()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);

   ...

   bo = tbm_bo_ref (bo);

   ....

   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
tbm_bo tbm_bo_ref(tbm_bo bo);

/**
 * @brief Decreases the reference count of bo
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @see tbm_bo_ref()
 * @see tbm_bo_alloc()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
void tbm_bo_unref(tbm_bo bo);

/**
 * @brief Maps the buffer object according to the device type and the option.
 * @details Cache flushing and Locking is executed, while tbm_bo is mapping in the proper condition according to the device type and the access option.\n
 * If the cache flush type of bufmgr set true, the map cache flushing is executed
 * If the lock type of bufmgr set once, the previous bo which is locked is unlock when the new bo is trying to be locked.\n
 * If the lock type of bufmgr set always, the new bo is locked until the previous bo which is locked is unlocked.\n
 * If the lock type of bufmgr set never, Every bo is never locked.\n\n
 * #TBM_DEVICE_DEFAULT indecates the default handle.\n
 * #TBM_DEVICE_2D indecates the 2D memory handle.\n
 * #TBM_DEVICE_3D indecates the 3D memory handle.\n
 * #TBM_DEVICE_CPU indecates the virtual memory handle.\n
 * #TBM_DEVICE_MM indecates the multimedia handle.\n\n
 * #TBM_OPTION_READ indecates the accss option to read.\n
 * #TBM_OPTION_WRITE indecates the access option to write.\n
 * #TBM_OPTION_VENDOR indecates the vendor specific option that depends on the backend.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @param[in] device : the device type to get a handle
 * @param[in] opt : the option to access the buffer object
 * @return the handle of the buffer object
 * @exception #TBM_ERROR_NONE            Success
 * @exception #TBM_ERROR_BO_LOCK_FAILED  tbm_bo lock failed
 * @exception #TBM_ERROR_BO_MAP_FAILED   tbm_bo map failed
 * @retval #tbm_bo
 * @see tbm_bo_unmap()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo;
   tbm_bo_handle handle;
   tbm_error_e error;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);

   ...

   handle = tbm_bo_map (bo, TBM_DEVICE_2D, TBM_OPTION_READ|TBM_OPTION_WRITE);
   if (handle.ptr == NULL)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   tbm_bo_unmap (bo);
   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
tbm_bo_handle tbm_bo_map(tbm_bo bo, int device, int opt);

/**
 * @brief Unmaps the buffer object.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @return 1 if this function succeeds, otherwise 0.
 * @see tbm_bo_map()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo
   tbm_bo_handle handle;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);

   ...

   handle = tbm_bo_map (bo, TBM_DEVICE_2D, TBM_OPTION_READ|TBM_OPTION_WRITE);

   ...

   tbm_bo_unmap (bo);
   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
int tbm_bo_unmap(tbm_bo bo);

/**
 * @brief Gets the tbm_bo_handle according to the device type.
 * @details The tbm_bo_handle can be get without the map of the tbm_bo.\n
 * In this case, TBM does not guarantee the lock and the cache flush of the tbm_bo.\n\n
 * #TBM_DEVICE_DEFAULT indecates the default handle.\n
 * #TBM_DEVICE_2D indecates the 2D memory handle.\n
 * #TBM_DEVICE_3D indecates the 3D memory handle.\n
 * #TBM_DEVICE_CPU indecates the virtual memory handle.\n
 * #TBM_DEVICE_MM indecates the multimedia handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @param[in] device : the device type to get a handle
 * @return the handle of the buffer object
 * @retval #tbm_bo_handle
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo;
   tbm_bo_handle handle;
   tbm_error_e error;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);

   ...

   handle = tbm_bo_get_handle (bo, TBM_DEVICE_2D);
   if (handle.ptr == NULL)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
  */
tbm_bo_handle tbm_bo_get_handle(tbm_bo bo, int device);

/**
 * @brief Exports the buffer object by key.
 * @details The tbm_bo can be exported to the anther process with the unique key associated with the the tbm_bo.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @return key associated with the buffer object
 * @retval #tbm_key
 * @see tbm_bo_import()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo;
   tbm_key key;
   tbm_error_e error;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
   key = tbm_bo_export (bo);
   if (key == 0)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
  */
tbm_key tbm_bo_export(tbm_bo bo);

/**
 * @brief Exports the buffer object by fd.
 * @details The tbm_bo can be exported to the anther process with the unique fd associated with the the tbm_bo.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release the fd using close().
 * @param[in] bo : the buffer object
 * @return fd associated with the buffer object
 * @retval #tbm_fd
 * @see tbm_bo_import_fd()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_fd bo_fd;
   tbm_bufmgr bufmgr;
   tbm_bo;
   tbm_error_e error;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
   bo_fd = tbm_bo_export (bo);
   if (bo_fd == 0)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
  */
tbm_fd tbm_bo_export_fd(tbm_bo bo);

/**
 * @brief Imports the buffer object associated with the key.
 * @details The reference count of the tbm_bo is 1.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bufmgr : the buffer manager
 * @param[in] key : the key associated with the buffer object
 * @return a buffer object
 * @retval #tbm_bo
 * @see tbm_bo_export()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   int bo_key;
   tbm_bufmgr bufmgr;
   tbm_bo;
   tbm_error_e error;

   ...

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_import (bufmgr, key);
   if (bo == NULL)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
  */
tbm_bo tbm_bo_import(tbm_bufmgr bufmgr, tbm_key key);

/**
 * @brief Imports the buffer object associated with the fd.
 * @details The reference count of the tbm_bo is 1.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release the fd using close().
 * @param[in] bufmgr : the buffer manager
 * @param[in] fd : the fd associated with the buffer object
 * @return a buffer object
 * @retval #tbm_bo
 * @see tbm_bo_export_fd()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_fd bo_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo;
   tbm_error_e error;

   ...

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_import_fd (bo_fd);
   if (bo == 0)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
  */
tbm_bo tbm_bo_import_fd(tbm_bufmgr bufmgr, tbm_fd fd);

/**
 * @brief Gets the size of a bo.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @return 1 if this function succeeds, otherwise 0.
 * @see tbm_bo_alloc()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo;
   int size;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
   size = tbm_bo_size (bo);

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
  */
int tbm_bo_size(tbm_bo bo);

/**
 * @brief Gets the state where the buffer object is locked.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @return 1 if this bo is locked, otherwise 0.
 * @see tbm_bo_map()
 * @see tbm_bo_unmap()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);

   ...

   if (tbm_bo_locked (bo))
   {

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
*/
int tbm_bo_locked(tbm_bo bo);

/**
 * @brief Swaps the buffer object.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo1 : the buffer object
 * @param[in] bo2 : the buffer object
 * @return 1 if this function succeeds, otherwise 0.
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo1;
   tbm_bo bo2;
   int ret;
   tbm_error_e error;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo1 = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
   bo2 = tbm_bo_alloc (bufmgr, 256 * 256, TBM_BO_DEFAULT);

   ...

   ret = tbm_bo_swap (bo1, bo2);
   if (ret == 0)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   tbm_bo_unref (bo1);
   tbm_bo_unref (bo2);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
int tbm_bo_swap(tbm_bo bo1, tbm_bo bo2);

/**
 * @brief Called when the user data is deleted in buffer object.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] user_data User_data to be passed to callback function
 * @pre The callback must be registered using tbm_bo_add_user_data().\n
 * tbm_bo_delete_user_data() must be called to invoke this callback.
 * @see tbm_bo_add_user_data()
 * @see tbm_bo_delete_user_data()
 */
typedef void (*tbm_data_free) (void *user_data);

/**
 * @brief Adds a user_data to the buffer object.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @param[in] key : the key associated with the user_data
 * @param[in] data_free_func : the function pointer to free the user_data
 * @return 1 if this function succeeds, otherwise 0.
 * @post tbm_data_free() will be called under certain conditions, after calling tbm_bo_delete_user_data().
 * @see tbm_data_free()
 * @see tbm_bo_set_user_data()
 * @see tbm_bo_get_user_data()
 * @see tbm_bo_delete_user_data()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   void example_data_free (void *user_data)
   {
       char *data = (char*) user_data;
       free(data);
   }

   int main()
   {
       int bufmgr_fd;
       tbm_bufmgr bufmgr;
       tbm_bo bo;
       char *user_data;
       char *get_data;
       int ret;

       bufmgr = tbm_bufmgr_init (bufmgr_fd);
       bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
       user_data = (char*) malloc (sizeof(char) * 128);

       ...

       tbm_bo_add_user_data (bo, 1, example_data_free);
       tbm_bo_set_user_data (bo, 1, user_data);

       ...

       ret = tbm_bo_get_user_data (bo, 1, &get_data);
       tbm_bo_delete_user_data (bo, 1);

       ...

       tbm_bo_unref (bo);
       tbm_bufmgr_deinit (bufmgr);
   }
   @endcode
 */

int tbm_bo_add_user_data(tbm_bo bo, unsigned long key,
			 tbm_data_free data_free_func);

/**
 * @brief Deletes the user_data in the buffer object.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @param[in] key : the key associated with the user_date
 * @return 1 if this function succeeds, otherwise 0.
 * @see tbm_bo_add_user_data()
 * @see tbm_bo_get_user_data()
 * @see tbm_bo_delete_user_data()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   void example_data_free (void *user_data)
   {
       char *data = (char*) user_data;
       free(data);
   }

   int main()
   {
       int bufmgr_fd;
       tbm_bufmgr bufmgr;
       tbm_bo bo;
       char *user_data;
       char *get_data;
       int ret;

       bufmgr = tbm_bufmgr_init (bufmgr_fd);
       bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
       user_data = (char*) malloc (sizeof(char) * 128);

       ...

       tbm_bo_add_user_data (bo, 1, example_data_free);
       tbm_bo_set_user_data (bo, 1, user_data);

       ...

       ret = tbm_bo_get_user_data (bo, 1, &get_data);
       tbm_bo_delete_user_data (bo, 1);

       ...

       tbm_bo_unref (bo);
       tbm_bufmgr_deinit (bufmgr);
   }
   @endcode
 */
int tbm_bo_delete_user_data(tbm_bo bo, unsigned long key);

/**
 * @brief Sets a user_date to the buffer object.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @param[in] key : the key associated with the user_date
 * @param[in] data : a pointer of the user_data
 * @return 1 if this function succeeds, otherwise 0.
 * @see tbm_bo_add_user_data()
 * @see tbm_bo_set_user_data()
 * @see tbm_bo_delete_user_data()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   void example_data_free (void *user_data)
   {
       char *data = (char*) user_data;
       free(data);
   }

   int main()
   {
       int bufmgr_fd;
       tbm_bufmgr bufmgr;
       tbm_bo bo;
       char *user_data;
       char *get_data;
       int ret;

       bufmgr = tbm_bufmgr_init (bufmgr_fd);
       bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
       user_data = (char*) malloc (sizeof(char) * 128);

       ...

       tbm_bo_add_user_data (bo, 1, example_data_free);
       tbm_bo_set_user_data (bo, 1, user_data);

       ...

       ret = tbm_bo_get_user_data (bo, 1, &get_data);
       tbm_bo_delete_user_data (bo, 1);

       ...

       tbm_bo_unref (bo);
       tbm_bufmgr_deinit (bufmgr);
   }
   @endcode
 */
int tbm_bo_set_user_data(tbm_bo bo, unsigned long key, void *data);

/**
 * @brief Gets a user_data from the buffer object with the key.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bo : the buffer object
 * @param[in] key : the key associated with the user_date
 * @param[out] data : to get the user data
 * @return 1 if this function succeeds, otherwise 0.
 * @see tbm_bo_add_user_data()
 * @see tbm_bo_set_user_data()
 * @see tbm_bo_get_user_data()
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   void example_data_free (void *user_data)
   {
       char *data = (char*) user_data;
       free(data);
   }

   int main()
   {
       int bufmgr_fd;
       tbm_bufmgr bufmgr;
       tbm_bo bo;
       char *user_data;
       char *get_data;
       int ret;

       bufmgr = tbm_bufmgr_init (bufmgr_fd);
       bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
       user_data = (char*) malloc (sizeof(char) * 128);

       ...

       tbm_bo_add_user_data (bo, 1, example_data_free);
       tbm_bo_set_user_data (bo, 1, user_data);

       ...

       ret = tbm_bo_get_user_data (bo, 1, &get_data);
       tbm_bo_delete_user_data (bo, 1);

       ...

       tbm_bo_unref (bo);
       tbm_bufmgr_deinit (bufmgr);
   }
   @endcode
 */
int tbm_bo_get_user_data(tbm_bo bo, unsigned long key, void **data);

/**
 * @brief Gets the latest tbm_error.
 * @since_tizen 2.4
 * @return the latest tbm_error
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo bo;
   tbm_bo_handle handle;
   tbm_error_e error;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
   if (!bo)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   handle = tbm_bo_map (bo, TBM_DEVICE_2D, TBM_OPTION_READ|TBM_OPTION_WRITE);
   if (handle.ptr == NULL)
   {
      error = tbm_get_last_error ();
      ...
   }

   ...

   tbm_bo_unmap (bo);
   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
tbm_error_e tbm_get_last_error(void);

/**
 * @brief Gets the tbm buffer capability.
 * @since_tizen 2.4
 * @param[in] bufmgr : the buffer manager
 * @return the tbm bufmgr capability
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   unsigned int capability;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);

   capability = tbm_bufmgr_get_capability (bufmgr);

   tbm_bufmgr_deinit (bufmgr);
   @endcode
 */
unsigned int tbm_bufmgr_get_capability(tbm_bufmgr bufmgr);

/**
 * @brief Gets the tbm bo flags.
 * @since_tizen 2.4
 * @param[in] bo : the buffer object
 * @return the tbm bo flags
 * @see TBM_BO_FLAGS
 * @par Example
   @code
   #include <tbm_bufmgr.h>

   int bufmgr_fd;
   tbm_bufmgr bufmgr;
   tbm_bo;
   int flags;

   bufmgr = tbm_bufmgr_init (bufmgr_fd);
   bo = tbm_bo_alloc (bufmgr, 128 * 128, TBM_BO_DEFAULT);
   flags = tbm_bo_get_flags (bo);

   ...

   tbm_bo_unref (bo);
   tbm_bufmgr_deinit (bufmgr);

   @endcode
 */
int tbm_bo_get_flags(tbm_bo bo);

/**
 * @brief Print out the information of tbm_bos.
 * @since_tizen 3.0
 * @param[in] bufmgr : the buffer manager
 */
void tbm_bufmgr_debug_show(tbm_bufmgr bufmgr);

/**
 * @brief Print out the trace of tbm_bos.
 * @since_tizen 3.0
 * @param[in] bufmgr : the buffer manager
 * @param[in] onoff : 1 is on, and 0 is off
 */
void tbm_bufmgr_debug_trace(tbm_bufmgr bufmgr, int onoff);

/**
 * @brief Dump all tbm surfaces
 * @param[in] path : the given dump path
 * @return 1 if this function succeeds, otherwise 0.
 */
int tbm_bufmgr_debug_dump_all(char *path);

/**
 * @brief Start the dump debugging for queue.
 * @since_tizen 3.0
 * @param[in] path : the given dump path
 * @param[in] count : the dump count number
 * @param[in] onoff : 1 is on, and 0 is off, if onoff==0 path and count are ignored
 * @return 1 if this function succeeds, otherwise 0.
 */
int tbm_bufmgr_debug_queue_dump(char *path, int count, int onoff);

int tbm_bufmgr_bind_native_display(tbm_bufmgr bufmgr, void *NativeDisplay);

#ifdef __cplusplus
}
#endif
#endif							/* _TBM_BUFMGR_H_ */
