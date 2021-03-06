/* GStreamer
 *
 * Copyright (C) 2016 Igalia
 *
 * Authors:
 *  Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *  Javier Martin <javiermartin@by.com.es>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <drm_fourcc.h>

#include "gstkmsutils.h"

#ifndef DRM_FORMAT_Y8
#define DRM_FORMAT_Y8		fourcc_code('G', 'R', 'E', 'Y') /* 8  Greyscale	*/
#endif

#ifndef DRM_FORMAT_Y10
#define DRM_FORMAT_Y10		fourcc_code('Y', '1', '0', ' ') /* 10 Greyscale */
#endif

#ifndef DRM_FORMAT_XV15
#define DRM_FORMAT_XV15		fourcc_code('X', 'V', '1', '5') /* 2x2 subsampled Cr:Cb plane 2:10:10:10 */
#endif

#ifndef DRM_FORMAT_XV20
#define DRM_FORMAT_XV20		fourcc_code('X', 'V', '2', '0') /* 2x1 subsampled Cr:Cb plane 2:10:10:10 */
#endif

/* *INDENT-OFF* */
static const struct
{
  guint32 fourcc;
  GstVideoFormat format;
} format_map[] = {
#define DEF_FMT(fourcc, fmt) \
  { DRM_FORMAT_##fourcc,GST_VIDEO_FORMAT_##fmt }

  /* DEF_FMT (XRGB1555, ???), */
  /* DEF_FMT (XBGR1555, ???), */
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
  DEF_FMT (ARGB8888, BGRA),
  DEF_FMT (XRGB8888, BGRx),
  DEF_FMT (ABGR8888, RGBA),
  DEF_FMT (XBGR8888, RGBx),
  DEF_FMT (BGR888, RGB),
  DEF_FMT (RGB888, BGR),
#else
  DEF_FMT (ARGB8888, ARGB),
  DEF_FMT (XRGB8888, xRGB),
  DEF_FMT (ABGR8888, ABGR),
  DEF_FMT (XBGR8888, xBGR),
  DEF_FMT (RGB888, RGB),
  DEF_FMT (BGR888, BGR),
#endif
  DEF_FMT (Y8, GRAY8),
  DEF_FMT (UYVY, UYVY),
  DEF_FMT (YUYV, YUY2),
  DEF_FMT (YVYU, YVYU),
  DEF_FMT (YUV420, I420),
  DEF_FMT (YVU420, YV12),
  DEF_FMT (YUV422, Y42B),
  DEF_FMT (NV12, NV12),
  DEF_FMT (NV21, NV21),
  DEF_FMT (NV16, NV16),
#ifdef DRM_FORMAT_XV15
  /* Both formats have been added together */
  DEF_FMT (Y10, GRAY10_LE32),
  DEF_FMT (XV15, NV12_10LE32),
  DEF_FMT (XV20, NV16_10LE32),
#endif

#undef DEF_FMT
};
/* *INDENT-ON* */

GstVideoFormat
gst_video_format_from_drm (guint32 drmfmt)
{
  gint i;

  for (i = 0; i < G_N_ELEMENTS (format_map); i++) {
    if (format_map[i].fourcc == drmfmt)
      return format_map[i].format;
  }

  return GST_VIDEO_FORMAT_UNKNOWN;
}

guint32
gst_drm_format_from_video (GstVideoFormat fmt)
{
  gint i;

  for (i = 0; i < G_N_ELEMENTS (format_map); i++) {
    if (format_map[i].format == fmt)
      return format_map[i].fourcc;
  }

  return 0;
}

guint32
gst_drm_bpp_from_drm (guint32 drmfmt)
{
  guint32 bpp;

  switch (drmfmt) {
    case DRM_FORMAT_YUV420:
    case DRM_FORMAT_YVU420:
    case DRM_FORMAT_YUV422:
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
    case DRM_FORMAT_NV16:
      bpp = 8;
      break;
#ifdef DRM_FORMAT_XV15
    case DRM_FORMAT_XV15:
    case DRM_FORMAT_XV20:
      /* One 32b macro pixel: three 10b pixels + 2b padding */
      bpp = 32;
      break;
#endif
    case DRM_FORMAT_UYVY:
    case DRM_FORMAT_YUYV:
    case DRM_FORMAT_YVYU:
      bpp = 16;
      break;
    case DRM_FORMAT_BGR888:
    case DRM_FORMAT_RGB888:
      bpp = 24;
      break;
    default:
      bpp = 32;
      break;
  }

  return bpp;
}

guint32
gst_drm_width_from_drm (guint32 drmfmt, guint32 width)
{
  guint32 ret;

  switch (drmfmt) {
#ifdef DRM_FORMAT_XV15
      /* Convert pixel width to macropixel width */
    case DRM_FORMAT_XV15:
    case DRM_FORMAT_XV20:
      ret = gst_util_uint64_scale_round (width, 1, 3);
      break;
#endif
    default:
      ret = width;
      break;
  }


  return ret;
}

guint32
gst_drm_height_from_drm (guint32 drmfmt, guint32 height)
{
  guint32 ret;

  switch (drmfmt) {
    case DRM_FORMAT_YUV420:
    case DRM_FORMAT_YVU420:
    case DRM_FORMAT_YUV422:
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
#ifdef DRM_FORMAT_XV15
    case DRM_FORMAT_XV15:
#endif
      ret = height * 3 / 2;
      break;
    case DRM_FORMAT_NV16:
#ifdef DRM_FORMAT_XV20
    case DRM_FORMAT_XV20:
#endif
      ret = height * 2;
      break;
    default:
      ret = height;
      break;
  }

  return ret;
}

static GstStructure *
gst_video_format_to_structure (GstVideoFormat format)
{
  GstStructure *structure;

  structure = NULL;
  if (format != GST_VIDEO_FORMAT_UNKNOWN)
    structure = gst_structure_new ("video/x-raw", "format", G_TYPE_STRING,
        gst_video_format_to_string (format), NULL);

  return structure;
}


#define SYNC_IP_DEV_DECODER "/dev/xlnxsync1"

static gboolean
xlnx_ll_supported (void)
{
  return TRUE;
}

GstCaps *
gst_kms_add_xlnx_ll_caps (GstCaps * caps, gboolean if_supported)
{
  GstCaps *caps_xlnx_ll;
  guint i;

  if (if_supported && !xlnx_ll_supported ())
    return caps;

  caps_xlnx_ll = gst_caps_copy (caps);

  for (i = 0; i < gst_caps_get_size (caps_xlnx_ll); i++) {
    GstCapsFeatures *feats;

    feats = gst_caps_get_features (caps_xlnx_ll, i);
    if (feats) {
      gst_caps_features_remove (feats, GST_CAPS_FEATURE_MEMORY_SYSTEM_MEMORY);
      gst_caps_features_add (feats, GST_CAPS_FEATURE_MEMORY_XLNX_LL);
    } else {
      gst_caps_set_features (caps_xlnx_ll, gst_caps_get_size (caps_xlnx_ll) - 1,
          gst_caps_features_new (GST_CAPS_FEATURE_MEMORY_XLNX_LL, NULL));
    }
  }

  return gst_caps_merge (caps, caps_xlnx_ll);
}

GstCaps *
gst_kms_sink_caps_template_fill (void)
{
  gint i;
  GstCaps *caps, *caps_alternate;
  GstStructure *template;

  caps = gst_caps_new_empty ();
  for (i = 0; i < G_N_ELEMENTS (format_map); i++) {
    template = gst_video_format_to_structure (format_map[i].format);
    gst_structure_set (template,
        "width", GST_TYPE_INT_RANGE, 1, G_MAXINT,
        "height", GST_TYPE_INT_RANGE, 1, G_MAXINT,
        "framerate", GST_TYPE_FRACTION_RANGE, 0, 1, G_MAXINT, 1, NULL);
    gst_caps_append_structure (caps, template);
  }
  caps = gst_caps_simplify (caps);

  /* Add an 'alternate' variant of the caps with the feature */
  caps_alternate = gst_caps_copy (caps);
  gst_caps_set_simple (caps_alternate, "interlace-mode", G_TYPE_STRING,
      "alternate", NULL);

  for (i = 0; i < gst_caps_get_size (caps_alternate); i++) {
    gst_caps_set_features (caps_alternate, gst_caps_get_size (caps) - 1,
        gst_caps_features_new (GST_CAPS_FEATURE_FORMAT_INTERLACED, NULL));
  }

  caps = gst_caps_merge (caps, caps_alternate);
  caps = gst_kms_add_xlnx_ll_caps (caps, FALSE);

  return caps;
}

static const gint device_par_map[][2] = {
  {1, 1},                       /* regular screen */
  {16, 15},                     /* PAL TV */
  {11, 10},                     /* 525 line Rec.601 video */
  {54, 59},                     /* 625 line Rec.601 video */
  {64, 45},                     /* 1280x1024 on 16:9 display */
  {5, 3},                       /* 1280x1024 on 4:3 display */
  {4, 3}                        /* 800x600 on 16:9 display */
};

#define DELTA(ratio, idx, w) \
  (ABS(ratio - ((gdouble)device_par_map[idx][w] / device_par_map[idx][!(w)])))

void
gst_video_calculate_device_ratio (guint dev_width, guint dev_height,
    guint dev_width_mm, guint dev_height_mm,
    guint * dpy_par_n, guint * dpy_par_d)
{
  gdouble ratio, delta, cur_delta;
  gint i, j, index, windex;

  /* First, calculate the "real" ratio based on the X values; which is
   * the "physical" w/h divided by the w/h in pixels of the display */
  if (dev_width == 0 || dev_height == 0
      || dev_width_mm == 0 || dev_height_mm == 0)
    ratio = 1.0;
  else
    ratio = (gdouble) (dev_width_mm * dev_height) / (dev_height_mm * dev_width);

  /* Now, find the one from device_par_map[][2] with the lowest delta
   * to the real one */
  delta = DELTA (ratio, 0, 0);
  index = 0;
  windex = 0;

  for (i = 1; i < G_N_ELEMENTS (device_par_map); i++) {
    for (j = 0; j < 2; j++) {
      cur_delta = DELTA (ratio, i, j);
      if (cur_delta < delta) {
        index = i;
        windex = j;
        delta = cur_delta;
      }
    }
  }

  if (dpy_par_n)
    *dpy_par_n = device_par_map[index][windex];

  if (dpy_par_d)
    *dpy_par_d = device_par_map[index][windex ^ 1];
}
