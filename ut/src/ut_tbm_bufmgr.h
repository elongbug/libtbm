#ifndef _TBM_BUFMGR_H_
#define _TBM_BUFMGR_H_

#include <tbm_type.h>
#include <stdint.h>

/* tbm error base : this error base is same as TIZEN_ERROR_TBM in tizen_error.h */
#ifndef TBM_ERROR_BASE
#define TBM_ERROR_BASE			-0x02830000
#endif

typedef struct _tbm_bufmgr *tbm_bufmgr;

typedef struct _tbm_bo *tbm_bo;

typedef uint32_t tbm_key;

typedef int32_t tbm_fd;

/* TBM_DEVICE_TYPE */

#define TBM_DEVICE_DEFAULT   0
#define TBM_DEVICE_CPU       1
#define TBM_DEVICE_2D        2
#define TBM_DEVICE_3D        3
#define TBM_DEVICE_MM        4

/* TBM_OPTION */

#define TBM_OPTION_READ      (1 << 0)
#define TBM_OPTION_WRITE     (1 << 1)
#define TBM_OPTION_VENDOR    (0xffff0000)

/* stub for union tbm_bo_handle */
class tbm_bo_handle {
public:
	void *ptr;
	int32_t s32;
	uint32_t u32;
	int64_t s64;
	uint64_t u64;
	tbm_bo_handle() {}
	tbm_bo_handle(const uint64_t v)
	{
		if (v == 0)
			ptr = NULL;
		else
			ptr = &u64;

		s32 = v;
		u32 = v;
		s64 = v;
		u64 = v;
	}
};

enum TBM_BO_FLAGS {
	TBM_BO_DEFAULT = 0,			   /**< default memory: it depends on the backend         */
	TBM_BO_SCANOUT = (1 << 0),	   /**< scanout memory                                    */
	TBM_BO_NONCACHABLE = (1 << 1), /**< non-cachable memory                               */
	TBM_BO_WC = (1 << 2),		   /**< write-combine memory                              */
	TBM_BO_VENDOR = (0xffff0000), /**< vendor specific memory: it depends on the backend */
};

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

enum TBM_BUFMGR_CAPABILITY {
	TBM_BUFMGR_CAPABILITY_NONE = 0,					/**< Not Support capability*/
	TBM_BUFMGR_CAPABILITY_SHARE_KEY = (1 << 0),		/**< Support sharing buffer by tbm key */
	TBM_BUFMGR_CAPABILITY_SHARE_FD = (1 << 1),		/**< Support sharing buffer by tbm fd */
};

#ifdef __cplusplus
extern "C" {
#endif

/* Functions for buffer manager */

tbm_bufmgr tbm_bufmgr_init(int fd);

void tbm_bufmgr_deinit(tbm_bufmgr bufmgr);

/* Functions for bo */

tbm_bo tbm_bo_alloc(tbm_bufmgr bufmgr, int size, int flags);

tbm_bo tbm_bo_ref(tbm_bo bo);

void tbm_bo_unref(tbm_bo bo);

tbm_bo_handle tbm_bo_map(tbm_bo bo, int device, int opt);

int tbm_bo_unmap(tbm_bo bo);

tbm_bo_handle tbm_bo_get_handle(tbm_bo bo, int device);

tbm_key tbm_bo_export(tbm_bo bo);

tbm_fd tbm_bo_export_fd(tbm_bo bo);

tbm_bo tbm_bo_import(tbm_bufmgr bufmgr, tbm_key key);

tbm_bo tbm_bo_import_fd(tbm_bufmgr bufmgr, tbm_fd fd);

int tbm_bo_size(tbm_bo bo);

int tbm_bo_locked(tbm_bo bo);

int tbm_bo_swap(tbm_bo bo1, tbm_bo bo2);

typedef void (*tbm_data_free) (void *user_data);

int tbm_bo_add_user_data(tbm_bo bo, unsigned long key,
			 tbm_data_free data_free_func);

int tbm_bo_delete_user_data(tbm_bo bo, unsigned long key);

int tbm_bo_set_user_data(tbm_bo bo, unsigned long key, void *data);

int tbm_bo_get_user_data(tbm_bo bo, unsigned long key, void **data);

tbm_error_e tbm_get_last_error(void);

unsigned int tbm_bufmgr_get_capability(tbm_bufmgr bufmgr);

int tbm_bo_get_flags(tbm_bo bo);

void tbm_bufmgr_debug_show(tbm_bufmgr bufmgr);

void tbm_bufmgr_debug_trace(tbm_bufmgr bufmgr, int onoff);

int tbm_bufmgr_bind_native_display(tbm_bufmgr bufmgr, void *NativeDisplay);

#ifdef __cplusplus
}
#endif
#endif							/* _TBM_BUFMGR_H_ */
