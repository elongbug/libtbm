/**************************************************************************

libtbm

Copyright 2014 Samsung Electronics co., Ltd. All Rights Reserved.

Contact: SooChan Lim <sc1.lim@samsung.com>, Sangjin Lee <lsj119@samsung.com>
Inpyo Kang <mantiger@samsung.com>, Dongyeon Kim <dy5.kim@samsung.com>
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

#ifndef _TBM_SURFACE_H_
#define _TBM_SURFACE_H_

/**
 * @addtogroup CAPI_UI_TBM_SURFACE_MODULE
 * @{
 */

#include <tbm_type.h>
#include <tizen.h>

/**
 * \file tbm_surface.h
 * \brief TBM Surface
 */

/**
 * @brief Enumeration for tbm_surface error type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	TBM_SURFACE_ERROR_NONE = TIZEN_ERROR_NONE,							  /**< Successful */
	TBM_SURFACE_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,  /**< Invalid parameter */
	TBM_SURFACE_ERROR_INVALID_OPERATION = TIZEN_ERROR_INVALID_OPERATION,  /**< Invalid Operation */
} tbm_surface_error_e;

/**
 * @brief Definition for the max number of TBM surface plane.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_SURF_PLANE_MAX 4

/* option to map the tbm_surface */
/**
 * @brief Definition for the access option to read.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_SURF_OPTION_READ      (1 << 0)
/**
 * @brief Definition for the access option to write.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_SURF_OPTION_WRITE     (1 << 1)

/**
 * @brief Definition for the TBM plane struct.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct _tbm_surface_plane {
	unsigned char *ptr;	  /**< Plane pointer */
	uint32_t size;		  /**< Plane size */
	uint32_t offset;	  /**< Plane offset */
	uint32_t stride;	  /**< Plane stride */

	void *reserved1;	  /**< Reserved pointer1 */
	void *reserved2;	  /**< Reserved pointer2 */
	void *reserved3;	  /**< Reserved pointer3 */
} tbm_surface_plane_s;

/**
 * @brief Definition for the TBM surface information struct.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct _tbm_surface_info {
	uint32_t width;		 /**< TBM surface width */
	uint32_t height;	 /**< TBM surface height */
	tbm_format format;	 /**< TBM surface format*/
	uint32_t bpp;		 /**< TBM surface bbp */
	uint32_t size;		 /**< TBM surface size */

	uint32_t num_planes;							/**< The number of planes */
	tbm_surface_plane_s planes[TBM_SURF_PLANE_MAX];	/**< Array of planes */

	void *reserved4;   /**< Reserved pointer4 */
	void *reserved5;   /**< Reserved pointer5 */
	void *reserved6;   /**< Reserved pointer6 */
} tbm_surface_info_s;

#define __tbm_fourcc_code(a, b, c, d) ((uint32_t)(a) | ((uint32_t)(b) << 8) | \
				((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

/* color index */
/**
 * @brief Definition for the TBM surface format C8 ([7:0] C).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_C8       __tbm_fourcc_code('C', '8', ' ', ' ')

/* 8 bpp RGB */
/**
 * @brief Definition for the TBM surface format RGB322 ([7:0] R:G:B 3:3:2).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGB332   __tbm_fourcc_code('R', 'G', 'B', '8')
/**
 * @brief Definition for the TBM surface format RGB233 ([7:0] B:G:R 2:3:3).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGR233   __tbm_fourcc_code('B', 'G', 'R', '8')

/* 16 bpp RGB */
/**
 * @brief Definition for the TBM surface format XRGB4444 ([15:0] x:R:G:B 4:4:4:4 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_XRGB4444 __tbm_fourcc_code('X', 'R', '1', '2')
/**
 * @brief Definition for the TBM surface format XBRG4444 ([15:0] x:B:G:R 4:4:4:4 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_XBGR4444 __tbm_fourcc_code('X', 'B', '1', '2')
/**
 * @brief Definition for the TBM surface format RGBX4444 ([15:0] R:G:B:x 4:4:4:4 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGBX4444 __tbm_fourcc_code('R', 'X', '1', '2')
/**
 * @brief Definition for the TBM surface format BGRX4444 ([15:0] B:G:R:x 4:4:4:4 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGRX4444 __tbm_fourcc_code('B', 'X', '1', '2')

/**
 * @brief Definition for the TBM surface format ARGB4444 ([15:0] A:R:G:B 4:4:4:4 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_ARGB4444 __tbm_fourcc_code('A', 'R', '1', '2')
/**
 * @brief Definition for the TBM surface format ABGR4444 ([15:0] A:B:G:R 4:4:4:4 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_ABGR4444 __tbm_fourcc_code('A', 'B', '1', '2')
/**
 * @brief Definition for the TBM surface format RGBA4444 ([15:0] R:G:B:A 4:4:4:4 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGBA4444 __tbm_fourcc_code('R', 'A', '1', '2')
/**
 * @brief Definition for the TBM surface format BGRA4444 ([15:0] B:G:R:A 4:4:4:4 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGRA4444 __tbm_fourcc_code('B', 'A', '1', '2')

/**
 * @brief Definition for the TBM surface format XRGB1555 ([15:0] x:R:G:B 1:5:5:5 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_XRGB1555 __tbm_fourcc_code('X', 'R', '1', '5')
/**
 * @brief Definition for the TBM surface format XBGR1555 ([15:0] x:B:G:R 1:5:5:5 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_XBGR1555 __tbm_fourcc_code('X', 'B', '1', '5')
/**
 * @brief Definition for the TBM surface format RGBX5551 ([15:0] R:G:B:x 5:5:5:1 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGBX5551 __tbm_fourcc_code('R', 'X', '1', '5')
/**
 * @brief Definition for the TBM surface format BGRX5551 ([15:0] B:G:R:x 5:5:5:1 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGRX5551 __tbm_fourcc_code('B', 'X', '1', '5')

/**
 * @brief Definition for the TBM surface format ARGB1555 ([15:0] A:R:G:B 1:5:5:5 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_ARGB1555 __tbm_fourcc_code('A', 'R', '1', '5')
/**
 * @brief Definition for the TBM surface format ABGR1555 ([15:0] A:B:G:R 1:5:5:5 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_ABGR1555 __tbm_fourcc_code('A', 'B', '1', '5')
/**
 * @brief Definition for the TBM surface format RGBA5551 ([15:0] R:G:B:A 5:5:5:1 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGBA5551 __tbm_fourcc_code('R', 'A', '1', '5')
/**
 * @brief Definition for the TBM surface format BGRA5551 ([15:0] B:G:R:A 5:5:5:1 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGRA5551 __tbm_fourcc_code('B', 'A', '1', '5')

/**
 * @brief Definition for the TBM surface format RGB565 ([15:0] R:G:B 5:6:5 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGB565   __tbm_fourcc_code('R', 'G', '1', '6')
/**
 * @brief Definition for the TBM surface format BGR565 ([15:0] B:G:R 5:6:5 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGR565   __tbm_fourcc_code('B', 'G', '1', '6')

/* 24 bpp RGB */
/**
 * @brief Definition for the TBM surface format RGB888 ([23:0] R:G:B little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGB888   __tbm_fourcc_code('R', 'G', '2', '4')
/**
 * @brief Definition for the TBM surface format BGR888 ([23:0] B:G:R little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGR888   __tbm_fourcc_code('B', 'G', '2', '4')

/* 32 bpp RGB */
/**
 * @brief Definition for the TBM surface format XRGB8888 ([31:0] x:R:G:B 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_XRGB8888 __tbm_fourcc_code('X', 'R', '2', '4')
/**
 * @brief Definition for the TBM surface format XBGR8888 ([31:0] x:B:G:R 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_XBGR8888 __tbm_fourcc_code('X', 'B', '2', '4')
/**
 * @brief Definition for the TBM surface format RGBX8888 ([31:0] R:G:B:x 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGBX8888 __tbm_fourcc_code('R', 'X', '2', '4')
/**
 * @brief Definition for the TBM surface format BGRX8888 ([31:0] B:G:R:x 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGRX8888 __tbm_fourcc_code('B', 'X', '2', '4')

/**
 * @brief Definition for the TBM surface format ARGB8888 ([31:0] A:R:G:B 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_ARGB8888 __tbm_fourcc_code('A', 'R', '2', '4')
/**
 * @brief Definition for the TBM surface format ABGR8888 ([31:0] [31:0] A:B:G:R 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_ABGR8888 __tbm_fourcc_code('A', 'B', '2', '4')
/**
 * @brief Definition for the TBM surface format RGBA8888 ([31:0] R:G:B:A 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGBA8888 __tbm_fourcc_code('R', 'A', '2', '4')
/**
 * @brief Definition for the TBM surface format BGRA8888 ([31:0] B:G:R:A 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGRA8888 __tbm_fourcc_code('B', 'A', '2', '4')

/**
 * @brief Definition for the TBM surface format XRGB2101010 ([31:0] x:R:G:B 2:10:10:10 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_XRGB2101010  __tbm_fourcc_code('X', 'R', '3', '0')
/**
 * @brief Definition for the TBM surface format XBGR2101010 ([31:0] x:B:G:R 2:10:10:10 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_XBGR2101010  __tbm_fourcc_code('X', 'B', '3', '0')
/**
 * @brief Definition for the TBM surface format RGBX1010102 ([31:0] R:G:B:x 10:10:10:2 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGBX1010102  __tbm_fourcc_code('R', 'X', '3', '0')
/**
 * @brief Definition for the TBM surface format BGRX1010102 ([31:0] B:G:R:x 10:10:10:2 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGRX1010102  __tbm_fourcc_code('B', 'X', '3', '0')

/**
 * @brief Definition for the TBM surface format ARGB2101010 ([31:0] A:R:G:B 2:10:10:10 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_ARGB2101010  __tbm_fourcc_code('A', 'R', '3', '0')
/**
 * @brief Definition for the TBM surface format ABGR2101010 ([31:0] A:B:G:R 2:10:10:10 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_ABGR2101010  __tbm_fourcc_code('A', 'B', '3', '0')
/**
 * @brief Definition for the TBM surface format RGBA1010102 ([31:0] R:G:B:A 10:10:10:2 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_RGBA1010102  __tbm_fourcc_code('R', 'A', '3', '0')
/**
 * @brief Definition for the TBM surface format BGRA1010102 ([31:0] B:G:R:A 10:10:10:2 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_BGRA1010102  __tbm_fourcc_code('B', 'A', '3', '0')	/*  */

/* packed YCbCr */
/**
 * @brief Definition for the TBM surface format YUYV ([31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YUYV     __tbm_fourcc_code('Y', 'U', 'Y', 'V')
/**
 * @brief Definition for the TBM surface format YVYU ([31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YVYU     __tbm_fourcc_code('Y', 'V', 'Y', 'U')	/*  */
/**
 * @brief Definition for the TBM surface format UYVY ([31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_UYVY     __tbm_fourcc_code('U', 'Y', 'V', 'Y')
/**
 * @brief Definition for the TBM surface format VYUY ([31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_VYUY     __tbm_fourcc_code('V', 'Y', 'U', 'Y')

/**
 * @brief Definition for the TBM surface format AYUV ([31:0] A:Y:Cb:Cr 8:8:8:8 little endian).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_AYUV     __tbm_fourcc_code('A', 'Y', 'U', 'V')

/*
 * 2 plane YCbCr
 * index 0 = Y plane, [7:0] Y
 * index 1 = Cr:Cb plane, [15:0] Cr:Cb little endian
 * or
 * index 1 = Cb:Cr plane, [15:0] Cb:Cr little endian
 */
/**
 * @brief Definition for the TBM surface format NV12 (2x2 subsampled Cr:Cb plane).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_NV12     __tbm_fourcc_code('N', 'V', '1', '2')
/**
 * @brief Definition for the TBM surface format NV21 (2x2 subsampled Cb:Cr plane).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_NV21     __tbm_fourcc_code('N', 'V', '2', '1')	/*  */
/**
 * @brief Definition for the TBM surface format NV16 (2x1 subsampled Cr:Cb plane).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_NV16     __tbm_fourcc_code('N', 'V', '1', '6')
/**
 * @brief Definition for the TBM surface format NV61 (2x1 subsampled Cb:Cr plane).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_NV61     __tbm_fourcc_code('N', 'V', '6', '1')

/*
 * 3 plane YCbCr
 * index 0: Y plane, [7:0] Y
 * index 1: Cb plane, [7:0] Cb
 * index 2: Cr plane, [7:0] Cr
 * or
 * index 1: Cr plane, [7:0] Cr
 * index 2: Cb plane, [7:0] Cb
 */
/**
 * @brief Definition for the TBM surface format YUV410 (4x4 subsampled Cb (1) and Cr (2) planes).
 */
#define TBM_FORMAT_YUV410   __tbm_fourcc_code('Y', 'U', 'V', '9')
/**
 * @brief Definition for the TBM surface format YVU410 (4x4 subsampled Cr (1) and Cb (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YVU410   __tbm_fourcc_code('Y', 'V', 'U', '9')
/**
 * @brief Definition for the TBM surface format YUV411 (4x1 subsampled Cb (1) and Cr (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YUV411   __tbm_fourcc_code('Y', 'U', '1', '1')
/**
 * @brief Definition for the TBM surface format YVU411 (4x1 subsampled Cr (1) and Cb (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YVU411   __tbm_fourcc_code('Y', 'V', '1', '1')
/**
 * @brief Definition for the TBM surface format YUV420 (2x2 subsampled Cb (1) and Cr (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YUV420   __tbm_fourcc_code('Y', 'U', '1', '2')
/**
 * @brief Definition for the TBM surface format YVU420 (2x2 subsampled Cr (1) and Cb (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YVU420   __tbm_fourcc_code('Y', 'V', '1', '2')
/**
 * @brief Definition for the TBM surface format YUV422 (2x1 subsampled Cb (1) and Cr (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YUV422   __tbm_fourcc_code('Y', 'U', '1', '6')
/**
 * @brief Definition for the TBM surface format YVU422 (2x1 subsampled Cr (1) and Cb (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YVU422   __tbm_fourcc_code('Y', 'V', '1', '6')
/**
 * @brief Definition for the TBM surface format YUV444 (non-subsampled Cb (1) and Cr (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YUV444   __tbm_fourcc_code('Y', 'U', '2', '4')
/**
 * @brief Definition for the TBM surface format YVU444 (non-subsampled Cr (1) and Cb (2) planes).
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define TBM_FORMAT_YVU444   __tbm_fourcc_code('Y', 'V', '2', '4')

/* 2 plane YCbCr */
/**
 * @brief Definition for the TBM surface format NV12MT (tiled '64x32' & multi-plane version of NV12).
 * @since_tizen 3.0
 */
#define TBM_FORMAT_NV12MT   __tbm_fourcc_code('T', 'M', '1', '2')

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Queries surface format list and number of format supported by the system.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks You must release the formats using free().
 *
 * @param[out] formats  The format array which the system can support \n
 *                      This pointer has to be freed by user.
 * @param[out] num      The number of formats
 *
 * @return  #TBM_SURFACE_ERROR_NONE if this function succeeds,
 *          otherwise an error status value
 *
 * @retval #TBM_SURFACE_ERROR_NONE               Success
 * @retval #TBM_SURFACE_ERROR_INVALID_OPERATION  Invalid operation
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   uint32_t *formats;
   uint32_t format_num;
   int ret, i;
   tbm_surface_error_e tse;

   tse = tbm_surface_query_formats (&formats, &format_num))

   for( i = 0 ; i < format_num ; i++)
   {
       if (formats[i] == TBM_FORMAT_RGB332)
       {
   ....

   free (formats);
   @endcode
 */
int tbm_surface_query_formats(uint32_t **formats, uint32_t *num);

/**
 * @brief Creates the tbm_surface.
 * @details This function creates the tbm_surface with the given width, height, and format.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remark The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 *
 * @param[in] width   The width of surface
 * @param[in] height  The height of surface
 * @param[in] format  The format of surface
 *
 * @return   #tbm_surface_h on success,
 *           otherwise @c NULL
 *
 * @retval #tbm_surface_h  The TBM surface handle
 *
 * @exception #TBM_SURFACE_ERROR_NONE               Success
 * @exception #TBM_SURFACE_ERROR_INVALID_PARAMETER  Invalid parameter
 * @exception #TBM_SURFACE_ERROR_INVALID_OPERATION  Invalid operation
 *
 * @see tbm_surface_destroy()
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   tbm_surface_h surface;

   surface = tbm_surface_create (128, 128, TBM_FORMAT_RGB332);

   ...

   tbm_surface_destroy (surface);
   @endcode
 */
tbm_surface_h tbm_surface_create(int width, int height, tbm_format format);

/**
 * @brief Destroys the tbm_surface.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] surface  The #tbm_surface_h
 *
 * @return  #TBM_SURFACE_ERROR_NONE on success,
 *          otherwise an error status value
 *
 * @retval #TBM_SURFACE_ERROR_NONE               Success
 * @retval #TBM_SURFACE_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see tbm_surface_create()
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   tbm_surface_h surface;

   surface = tbm_surface_create (128, 128, TBM_FORMAT_RGB332);

   ...

   tbm_surface_destroy (surface);
   @endcode
 */
int tbm_surface_destroy(tbm_surface_h surface);

/**
 * @brief Maps the tbm_surface according to the access option.
 * @details After mapping tbm_surface, the information of tbm_surface is assigned in #tbm_surface_info_s struct. \n
 *          The information of tbm_surface has width, height, format, bpp, size, number of planes and information of planes. \n
 *          The information of planes has stride, offset, size and pointer of plane. \n
 *          #TBM_SURF_OPTION_READ indicates access option to read. \n
 *          #TBM_SURF_OPTION_WRITE indicates access option to write.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in]  surface  The #tbm_surface_h
 * @param[in]  opt      The option to access the tbm_surface
 * @param[out] info     The information of the tbm_surface
 *
 * @return  #TBM_SURFACE_ERROR_NONE on success,
 *          otherwise an error status value
 *
 * @retval #TBM_SURFACE_ERROR_NONE               Success
 * @retval #TBM_SURFACE_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #TBM_SURFACE_ERROR_INVALID_OPERATION  Invalid operation
 *
 * @see tbm_surface_unmap();
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   tbm_surface_h surface;
   tbm_surface_info_s info;
   int ret;

   surface = tbm_surface_create (128, 128, TBM_FORMAT_RGB332);
   ret = tbm_surface_map (surface, TBM_SURF_OPTION_WRITE|TBM_SURF_OPTION_READ, &info);

   ...

   tbm_surface_unmap (surface);
   tbm_surface_destroy (surface);
   @endcode
 */
int tbm_surface_map(tbm_surface_h surface, int opt, tbm_surface_info_s *info);

/**
 * @brief Unmaps the tbm_surface.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] surface  The #tbm_surface_h
 *
 * @return  #TBM_SURFACE_ERROR_NONE on success,
 *          otherwise an error status value
 *
 * @retval #TBM_SURFACE_ERROR_NONE               Success
 * @retval #TBM_SURFACE_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see tbm_surface_map()
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   tbm_surface_h surface;
   tbm_surface_info_s info;
   int ret;

   surface = tbm_surface_create (128, 128, TBM_FORMAT_RGB332);
   ret = tbm_surface_map (surface, TBM_SURF_OPTION_WRITE|TBM_SURF_OPTION_READ, &info);

   ...

   tbm_surface_unmap (surface);
   tbm_surface_destroy (surface);
   @endcode
 */
int tbm_surface_unmap(tbm_surface_h surface);

/**
 * @brief Gets the information of the tbm_surface.
 * @details The information of tbm_surface is assigned in #tbm_surface_info_s struct. \n
 *          The information of tbm_surface has width, height, format, bpp, size, number of planes and information of planes. \n
 *          The information of planes has stride, offset, size and pointer of plane.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in]   surface  The #tbm_surface_h
 * @param[out]  info     The information of the tbm_surface
 *
 * @return  #TBM_SURFACE_ERROR_NONE on success,
 *          otherwise an error status value
 *
 * @retval #TBM_SURFACE_ERROR_NONE               Success
 * @retval #TBM_SURFACE_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #TBM_SURFACE_ERROR_INVALID_OPERATION  Invalid operation
 *
 * @see tbm_surface_map()
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   tbm_surface_h surface;
   tbm_surface_info_s info;
   int ret;

   surface = tbm_surface_create (128, 128, TBM_FORMAT_RGB332);
   ret = tbm_surface_get_info (surface, &info);

   ...

   tbm_surface_destroy (surface);
   @endcode
 */
int tbm_surface_get_info(tbm_surface_h surface, tbm_surface_info_s *info);

/**
 * @brief Gets the width of the tbm_surface.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] surface  The #tbm_surface_h
 *
 * @return  The width of the tbm_surface on success,
 *          otherwise an error status value
 *
 * @retval #TBM_SURFACE_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   tbm_surface_h surface;
   int width;

   surface = tbm_surface_create (128, 128, TBM_FORMAT_RGB332);

   ...

   width = tbm_surface_get_width (surface);

   ...

   tbm_surface_destroy (surface);
   @endcode
 */
int tbm_surface_get_width(tbm_surface_h surface);

/**
 * @brief Gets the height of the tbm_surface.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] surface  The #tbm_surface_h
 *
 * @return  The height of the tbm_surface if this function succeeds,
 *          otherwise an error status value
 *
 * @retval #TBM_SURFACE_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   tbm_surface_h surface;
   int height;

   surface = tbm_surface_create (128, 128, TBM_FORMAT_RGB332);

   ...

   height = tbm_surface_get_height (surface);

   ...

   tbm_surface_destroy (surface);
   @endcode
 */
int tbm_surface_get_height(tbm_surface_h surface);

/**
 * @brief Gets the format of the tbm_surface.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remark The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 *
 * @param[in] surface  The #tbm_surface_h
 *
 * @return  The format of the tbm_surface on success,
 *          otherwise @c 0 on failure
 *
 * @retval #tbm_format  The format of surface
 *
 * @exception #TBM_SURFACE_ERROR_NONE               Success
 * @exception #TBM_SURFACE_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @par Example
   @code
   #include <tbm_surface.h>

   tbm_surface_s surface;
   tbm_format format;

   surface = tbm_surface_create (128, 128, TBM_FORMAT_RGB332);

   ...

   format = tbm_surface_get_format (surface);

   ...

   tbm_surface_destroy (surface);
   @endcode
 */
tbm_format tbm_surface_get_format(tbm_surface_h surface);

#ifdef __cplusplus
}
#endif
/**
* @}
*/
#endif							/* _TBM_SURFACE_H_ */
