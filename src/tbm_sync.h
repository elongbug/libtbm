#ifndef _TBM_SYNC_H_
#define _TBM_SYNC_H_

#include <tbm_bufmgr.h>

/**
 * @brief Create timeline object.
 * @return file descriptor for the created timeline on success, -1 otherwise
 * @remarks close the fd when you no longer need it
 */
tbm_fd tbm_sync_timeline_create(void);

/**
 * @brief Increase the current value of the timeline.
 * @param[in] timeline timeline object
 * @param[in] count amount of increment
 * @return 1 on success, 0 otherwise
 */
int tbm_sync_timeline_inc(tbm_fd timeline, unsigned int count);

/**
 * @brief Create fence object.
 * @param[in] timeline timeline object on which the fence is created
 * @param[in] name fence name (only first 31 characters will be used)
 * @param[in] value timeline point value for the fence
 * @return file descriptor for the created fence on success, -1 otherwise
 * @remarks close the fd when you no longer need it
 */
tbm_fd tbm_sync_fence_create(tbm_fd timeline, const char *name, unsigned int value);

/**
 * @brief Wait for the given fence to be signaled
 * @param[in] fence fence object
 * @param[in] timeout timeout in milliseconds
 * @return 1 on success, 0 on failure, -1 on timeout expire
 * @remarks negative timeout means infinite, timeout 0 returns immediately
 */
int tbm_sync_fence_wait(tbm_fd fence, int timeout);

/**
 * @brief Merge two fences into one
 * @param[in] name name of the new fence
 * @param[in] fence1 fence to be merged
 * @param[in] fence2 fence to be merged
 * @return file descriptor for the new fence on success, -1 otherwise
 * @remarks close the fd when you no longer need it
 */
tbm_fd tbm_sync_fence_merge(const char *name, tbm_fd fence1, tbm_fd fence2);

#endif /* _TBM_SYNC_H */
