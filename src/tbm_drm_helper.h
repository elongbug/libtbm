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

#ifndef _TBM_DRM_HELPER_H_
#define _TBM_DRM_HELPER_H_

/**
 * @brief Initialize authentication server in display server.
 * @details
 * In DRM system, client sholud get authenticated fd from display server for using drm.
   Tbm provides wayland protocol and helper function for passing and authenticating
   fd from display server.
 * @param[in] wl_display wayland display
 * @param[in] fd fd of drm_master
 * @param[in] device_name name of drm device
 * @param[in] flags flags
  * @see #tbm_drm_helper_wl_auth_server_deinit()
 */
int tbm_drm_helper_wl_auth_server_init(void  *wl_display, int fd, const char *device_name, uint32_t flags);

/**
 * @brief Deinitialize authentication server in display server
 * @details
 * In DRM system, client sholud get authenticated fd from display server for using drm.
   Tbm provides wayland protocol and helper function for passing and authenticating
   fd from display server.
 * @see #tdm_helper_set_tbm_master_fd()
 * @see #tbm_drm_helper_unset_tbm_master_fd()
 */
void tbm_drm_helper_wl_auth_server_deinit(void);

/**
 * @brief Get a fd from the specific enviroment variable.
 * @details
 * This function will dup the fd of the given enviroment variable.
   The Caller SHOULD close the fd.
   In DRM system, a drm-master-fd SHOULD be shared between TDM backend and
   TBM backend in display server side by using "TDM_DRM_MASTER_FD" and "TBM_DRM_MASTER_FD".
 * @param[in] env The given enviroment variable
 * @return fd if success. Otherwise, -1.
 * @see #tdm_helper_set_tbm_master_fd()
 */
int  tbm_drm_helper_get_master_fd(void);

/**
 * @brief Set the given fd to TBM_DRM_MASTER_FD enviroment variable.
 * @details
 * In DRM system, a drm-master-fd @b SHOULD be shared between TDM backend and
   TBM backend in display server side by using "TDM_DRM_MASTER_FD"
   and "TBM_DRM_MASTER_FD".
 * @param[in] fd The given fd
 * @see #tdm_helper_set_tbm_master_fd()
 * @see #tbm_drm_helper_unset_tbm_master_fd()
 */
void tbm_drm_helper_set_tbm_master_fd(int fd);

/**
 * @brief Unset the TBM_DRM_MASTER_FD enviroment variable.
 * @details
 * In DRM system, a drm-master-fd @b SHOULD be shared between TDM backend and
   TBM backend in display server side by using "TDM_DRM_MASTER_FD"
   and "TBM_DRM_MASTER_FD".
 * @param[in] env The given enviroment variable
 * @param[in] fd The given fd
 * @see #tdm_helper_set_tbm_master_fd()
 * @see #tbm_drm_helper_unset_tbm_master_fd()
 */
void tbm_drm_helper_unset_tbm_master_fd(void);

/**
 * @brief Get infomation of drm authentication.
 * @details
 * In DRM system, client sholud get authenticated fd from display server for using drm.
   Tbm provides wayland protocol and helper function for passing and authenticating
   fd from display server.
 * @param[out] fd The authenticated fd
 * @param[out] device The device name
 * @param[out] capabilities The capabilities of device
 * @see #tdm_helper_set_tbm_master_fd()
 * @see #tbm_drm_helper_unset_tbm_master_fd()
 */
int tbm_drm_helper_get_auth_info(int *auth_fd, char **device, uint32_t *capabilities);

/**
 * @brief Set the given fd to TBM_DRM_FD enviroment variable.
 * @details
 * Some client want to get drm fd used tbm_backend.
   if tbm_backend allow that client use drm_fd, it SHOULD be set.
 * @param[in] fd The given fd
 * @see #tbm_drm_helper_get_fd()
 */
void tbm_drm_helper_set_fd(int fd);

/**
 * @brief Unset the given fd to TBM_DRM_FD enviroment variable.
 * @details
 * Some client want to get drm fd used tbm_backend.
   if tbm_backend allow that client use drm_fd, it SHOULD be set.
 * @param[in] fd The given fd
 * @see #tbm_drm_helper_get_fd()
 */
void tbm_drm_helper_unset_fd(void);

/**
 * @brief Get the fd from TBM_DRM_FD enviroment variable.
 * @details
 * Some client want to get drm fd used tbm_backend.
   client can get drm fd from this fucntion.
   The Caller SHOULD close the fd.
 * @return fd if success. Otherwise, -1.
 * @see #tdm_helper_set_tbm_master_fd()
 * @see #tbm_drm_helper_unset_tbm_master_fd()
 */
int tbm_drm_helper_get_fd(void);

#endif							/* _TBM_DRM_HELPER_H_ */
