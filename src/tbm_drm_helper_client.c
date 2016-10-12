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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>

#include "tbm_bufmgr_int.h"

#include "wayland-tbm-drm-auth-client-protocol.h"

struct wayland_tbm_drm_auth_client {
	struct wl_display *display;
	struct wl_tbm_drm_auth *wl_tbm_drm_auth;
	int auth_fd;
	char *device;
	uint32_t capabilities;
};

/* LCOV_EXCL_START */
static void
handle_tbm_drm_authentication_info(void *data, struct wl_tbm_drm_auth *wl_tbm_drm_auth, const char *device_name, uint32_t capabilities, int32_t auth_fd)
{
	struct wayland_tbm_drm_auth_client *tbm_drm_client = (struct wayland_tbm_drm_auth_client *)data;

	/* client authentication infomation */
	tbm_drm_client->auth_fd = auth_fd;
	tbm_drm_client->capabilities = capabilities;
	if (device_name)
	    tbm_drm_client->device = strdup(device_name);
}

static const struct wl_tbm_drm_auth_listener wl_tbm_drm_auth_client_listener = {
	handle_tbm_drm_authentication_info
};

static void
_wayland_tbm_drm_auth_client_registry_handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
	struct wayland_tbm_drm_auth_client *tbm_drm_client = (struct wayland_tbm_drm_auth_client *)data;

	if (!strcmp(interface, "wl_tbm_drm_auth")) {
		tbm_drm_client->wl_tbm_drm_auth = wl_registry_bind(registry, name, &wl_tbm_drm_auth_interface, version);
		TBM_RETURN_IF_FAIL(tbm_drm_client->wl_tbm_drm_auth != NULL);

		wl_tbm_drm_auth_add_listener(tbm_drm_client->wl_tbm_drm_auth, &wl_tbm_drm_auth_client_listener, tbm_drm_client);
	}
}

static const struct wl_registry_listener registry_listener = {
	_wayland_tbm_drm_auth_client_registry_handle_global,
	NULL
};

int
tbm_drm_helper_get_auth_info(int *auth_fd, char **device, uint32_t *capabilities)
{
	struct wl_display *display;
	struct wl_registry *wl_registry;
	struct wayland_tbm_drm_auth_client *tbm_drm_client;

	tbm_drm_client = calloc(1, sizeof(struct wayland_tbm_drm_auth_client));
	TBM_RETURN_VAL_IF_FAIL(tbm_drm_client != NULL, 0);

	tbm_drm_client->auth_fd = -1;

	display = wl_display_connect("tbm-drm-auth");
	if (!display) {
		TBM_LOG_E("Failed to connect display\n");
		free(tbm_drm_client);

		return 0;
	}

	tbm_drm_client->display = display;

	wl_registry = wl_display_get_registry(display);
	if (!wl_registry) {
		TBM_LOG_E("Failed to get registry\n");
		wl_display_disconnect(display);
		free(tbm_drm_client);

		return 0;
	}

	wl_registry_add_listener(wl_registry, &registry_listener, tbm_drm_client);
	wl_display_roundtrip(display); //For Gloabl registry

	if (!tbm_drm_client->wl_tbm_drm_auth) {
		TBM_LOG_E("Failed to get wl_tbm_drm_auth interface\n");
		wl_registry_destroy(wl_registry);
		wl_display_disconnect(display);
		free(tbm_drm_client);

		return 0;
	}

	wl_tbm_drm_auth_get_authentication_info(tbm_drm_client->wl_tbm_drm_auth);
	wl_display_roundtrip(display);

	if (tbm_drm_client->auth_fd < 0) {
		TBM_LOG_E("Failed to get auth info\n");
		wl_tbm_drm_auth_set_user_data(tbm_drm_client->wl_tbm_drm_auth, NULL);
		wl_tbm_drm_auth_destroy(tbm_drm_client->wl_tbm_drm_auth);
		wl_registry_destroy(wl_registry);
		wl_display_disconnect(display);
		free(tbm_drm_client);

		return 0;
	}

	if (auth_fd)
		*auth_fd = tbm_drm_client->auth_fd;
	else
		close(tbm_drm_client->auth_fd);

	if (capabilities)
		*capabilities = tbm_drm_client->capabilities;

	if (device) {
		if (tbm_drm_client->device)
			*device = strdup(tbm_drm_client->device);
		else
			*device = NULL;
	}

	wl_tbm_drm_auth_set_user_data(tbm_drm_client->wl_tbm_drm_auth, NULL);
	wl_tbm_drm_auth_destroy(tbm_drm_client->wl_tbm_drm_auth);

	if (tbm_drm_client->device)
		free(tbm_drm_client->device);

	free(tbm_drm_client);

	wl_registry_destroy(wl_registry);
	wl_display_disconnect(display);

	return 1;
}
/* LCOV_EXCL_STOP */

