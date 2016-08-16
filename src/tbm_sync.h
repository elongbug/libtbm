#ifndef _TBM_SYNC_H_
#define _TBM_SYNC_H_

typedef enum {
	TBM_SYNC_ERROR_NONE = 0,				/**< Successful */
	TBM_SYNC_ERROR_INVALID_PARAMETER = -1,  /**< Invalid parameter */
	TBM_SYNC_ERROR_INVALID_OPERATION = -2,  /**< Invalid Operation */
} tbm_sync_error_e;

typedef void *tbm_sync_timeline_h;
typedef void *tbm_sync_fence_h;

tbm_sync_timeline_h tbm_sync_timeline_create(tbm_sync_error_e *error);
tbm_sync_error_e    tbm_sync_timeline_destroy(tbm_sync_timeline_h timeline);
tbm_sync_timeline_h tbm_sync_timeline_import(int fd, tbm_sync_error_e *error);
int                 tbm_sync_timeline_export(tbm_sync_timeline_h timeline, tbm_sync_error_e *error);
tbm_sync_error_e    tbm_sync_timeline_increase_count(tbm_sync_timeline_h timeline, unsigned int interval);
unsigned int        tbm_sync_timeline_get_cur_count(tbm_sync_timeline_h timeline, tbm_sync_error_e *error);

tbm_sync_fence_h    tbm_sync_fence_create(tbm_sync_timeline_h timeline, const char *name, unsigned int count_val, tbm_sync_error_e *error);
tbm_sync_error_e    tbm_sync_fence_destroy(tbm_sync_fence_h fence);
tbm_sync_error_e    tbm_sync_fence_wait(tbm_sync_fence_h fence, int timeout);
tbm_sync_fence_h    tbm_sync_fence_merge(tbm_sync_fence_h fence1, tbm_sync_fence_h fence2, const char *name, tbm_sync_error_e *error);
unsigned int        tbm_sync_fence_get_count_val(tbm_sync_fence_h fence, tbm_sync_error_e *error);
tbm_sync_fence_h    tbm_sync_fence_import(int fd, tbm_sync_error_e *error);
int                 tbm_sync_fence_export(tbm_sync_fence_h fence, tbm_sync_error_e *error);

#endif /* _TBM_SYNC_H */
