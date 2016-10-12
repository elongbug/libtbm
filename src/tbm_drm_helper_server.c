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

#define WL_HIDE_DEPRECATED

#include "config.h"

#include <xf86drm.h>

#include "tbm_bufmgr_int.h"

#include "wayland-tbm-drm-auth-server-protocol.h"

struct wayland_tbm_drm_auth_server {
	struct wl_display *display;
	struct wl_global *wl_tbm_drm_auth_global;

	char *device_name;
	uint32_t fd;
	uint32_t flags;
};

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct wayland_tbm_drm_auth_server *tbm_drm_auth_srv;

/* LCOV_EXCL_START */
static void
_send_server_auth_info(struct wayland_tbm_drm_auth_server *tbm_drm_auth_srv,
		       struct wl_resource *resource)
{
	int fd = -1;
	uint32_t capabilities;
	char *device_name = NULL;
	drm_magic_t magic = 0;

	fd = open(tbm_drm_auth_srv->device_name, O_RDWR | O_CLOEXEC);
	if (fd == -1 && errno == EINVAL) {
		fd = open(tbm_drm_auth_srv->device_name, O_RDWR);
		if (fd != -1)
			fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
	}

	if (fd < 0) {
		TBM_LOG_E("failed to open drm : device_name, %s\n", tbm_drm_auth_srv->device_name);

		wl_resource_post_error(resource, WL_TBM_DRM_AUTH_ERROR_AUTHENTICATE_FAIL,
				       "authenicate failed::open_drm");
		goto fini;
	}

	if (drmGetMagic(fd, &magic) < 0) {
		if (errno != EACCES) {
			TBM_LOG_E("failed to get magic\n");

			wl_resource_post_error(resource, WL_TBM_DRM_AUTH_ERROR_AUTHENTICATE_FAIL,
					       "authenicate failed::get_magic");
			goto fini;
		}
	}

	if (drmAuthMagic(tbm_drm_auth_srv->fd, magic) < 0) {
		TBM_LOG_E("failed to authenticate magic\n");

		wl_resource_post_error(resource, WL_TBM_DRM_AUTH_ERROR_AUTHENTICATE_FAIL,
				       "authenicate failed::auth_magic");
		goto fini;
	}

	capabilities = tbm_drm_auth_srv->flags;
	device_name = tbm_drm_auth_srv->device_name;

	/* send */
	wl_tbm_drm_auth_send_authentication_info(resource, device_name, capabilities, fd);

fini:
	if (fd >= 0)
		close(fd);

	if (device_name && device_name != tbm_drm_auth_srv->device_name)
		free(device_name);

}

static void
_wayland_tbm_drm_auth_server_impl_get_authentication_info(struct wl_client *client,
		struct wl_resource *resource)
{
	struct wayland_tbm_drm_auth_server *tbm_drm_auth_srv = wl_resource_get_user_data(resource);

	/* if display server is the client of the host display server, for embedded server */
	_send_server_auth_info(tbm_drm_auth_srv, resource);
}


static const struct wl_tbm_drm_auth_interface _wayland_tbm_drm_auth_server_implementation = {
	_wayland_tbm_drm_auth_server_impl_get_authentication_info,
};

static void
_wayland_tbm_drm_auth_server_bind_cb(struct wl_client *client, void *data,
			    uint32_t version,
			    uint32_t id)
{
	struct wl_resource *resource;

	resource = wl_resource_create(client, &wl_tbm_drm_auth_interface, MIN(version, 1), id);
	if (!resource) {
		wl_client_post_no_memory(client);
		return;
	}

	wl_resource_set_implementation(resource,
				       &_wayland_tbm_drm_auth_server_implementation,
				       data,
				       NULL);
}

int
tbm_drm_helper_wl_auth_server_init(void *wl_display,   int fd, const char *device_name, uint32_t flags)
{
	if (!tbm_drm_auth_srv) {
		TBM_RETURN_VAL_IF_FAIL(wl_display != NULL, 0);

		tbm_drm_auth_srv = calloc(1, sizeof(struct wayland_tbm_drm_auth_server));
		TBM_RETURN_VAL_IF_FAIL(tbm_drm_auth_srv != NULL, 0);

		tbm_drm_auth_srv->display = (struct wl_display *)wl_display;
		tbm_drm_auth_srv->device_name = strdup(device_name);
		tbm_drm_auth_srv->fd = fd;
		tbm_drm_auth_srv->flags = flags;

		if (wl_display_add_socket(tbm_drm_auth_srv->display, "tbm-drm-auth")) {
			TBM_LOG_E("[TBM_DRM] fail to add socket\n");

			if (tbm_drm_auth_srv->device_name)
				free(tbm_drm_auth_srv->device_name);

			free(tbm_drm_auth_srv);
			tbm_drm_auth_srv = NULL;

			return 0;
		}

		/* init the client resource list */
		tbm_drm_auth_srv->wl_tbm_drm_auth_global = wl_global_create(tbm_drm_auth_srv->display, &wl_tbm_drm_auth_interface, 1,
					 tbm_drm_auth_srv, _wayland_tbm_drm_auth_server_bind_cb);
	}

	return 1;
}

void
tbm_drm_helper_wl_auth_server_deinit(void)
{
	if (tbm_drm_auth_srv) {
		wl_global_destroy(tbm_drm_auth_srv->wl_tbm_drm_auth_global);

		if (tbm_drm_auth_srv->device_name)
			free(tbm_drm_auth_srv->device_name);

		free(tbm_drm_auth_srv);
		tbm_drm_auth_srv = NULL;
	}
}

int
tbm_drm_helper_get_master_fd(void)
{
	const char *value;
	int ret, flags, fd = -1;
	int new_fd = -1;

	value = (const char*)getenv("TDM_DRM_MASTER_FD");
	if (!value)
		return -1;

	ret = sscanf(value, "%d", &fd);
	if (ret <= 0)
		return -1;

	TBM_LOG_I("TDM_DRM_MASTER_FD: %d\n", fd);

	flags = fcntl(fd, F_GETFD);
	if (flags == -1) {
		TBM_LOG_E("fcntl failed: %m");
		return -1;
	}

	new_fd = dup(fd);
	if (new_fd < 0) {
		TBM_LOG_E("dup failed: %m");
		return -1;
	}

	fcntl(new_fd, F_SETFD, flags|FD_CLOEXEC);

	TBM_LOG_I("Return MASTER_FD: %d\n", new_fd);

	return new_fd;
}

void
tbm_drm_helper_set_tbm_master_fd(int fd)
{
	char buf[32];
	int ret;

	snprintf(buf, sizeof(buf), "%d", fd);

	ret = setenv("TBM_DRM_MASTER_FD", (const char*)buf, 1);
	if (ret) {
		TBM_LOG_E("failed to set TIZEN_DRM_MASTER_FD to %d", fd);
		return;
	}

	TBM_LOG_I("TBM_DRM_MASTER_FD: %d\n", fd);
}

void
tbm_drm_helper_unset_tbm_master_fd(void)
{
	int ret;

	ret = unsetenv("TBM_DRM_MASTER_FD");
	if (ret) {
		TBM_LOG_E("failed to unset TBM_DRM_MASTER_FD");
		return;
	}
}
/* LCOV_EXCL_STOP */

