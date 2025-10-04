#include "luminix.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
//#include <math.h>
//#include <stdio.h>
#include <stdlib.h>

typedef struct {
  float lum;
  int r, g, b;
  int x, y;
} src_pix_info;

typedef struct {
  float lum;
  int x, y;
} trgt_pix_info;

static int cmp_luminance_src(const void *a, const void *b) {
  float la = ((const src_pix_info *)a)->lum;
  float lb = ((const src_pix_info *)b)->lum;
  return (la > lb) - (la < lb);
}

static int cmp_luminance_tgt(const void *a, const void *b) {
  float la = ((const trgt_pix_info *)a)->lum;
  float lb = ((const trgt_pix_info *)b)->lum;
  return (la > lb) - (la < lb);
}

float luminance(unsigned char r, unsigned char g, unsigned char b) {
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}
int luminix(const char *src_path, const char *tgt_path, const char *out_path) {
    int w1, h1, c1, w2, h2, c2;
    unsigned char *img1 = NULL, *img2 = NULL, *out_img = NULL;
    src_pix_info *src_pixels = NULL;
    trgt_pix_info *tgt_pixels = NULL;
    int sat = 0;

    img1 = stbi_load(src_path, &w1, &h1, &c1, 3);
    img2 = stbi_load(tgt_path, &w2, &h2, &c2, 3);
    if (!img1 || !img2) {
        sat = 1; goto cleanup;
    }

    int total1 = w1 * h1, total2 = w2 * h2, N = (total1 < total2) ? total1 : total2;
    src_pixels = (src_pix_info*)malloc(sizeof(src_pix_info) * total1);
    if (!src_pixels) { sat = 2; goto cleanup; }

    for (int y = 0; y < h1; ++y)
        for (int x = 0; x < w1; ++x) {
            int idx = (y * w1 + x) * 3;
            src_pix_info *p = &src_pixels[y * w1 + x];
            p->r = img1[idx+0]; p->g = img1[idx+1]; p->b = img1[idx+2];
            p->lum = luminance(p->r, p->g, p->b); p->x = x; p->y = y;
        }
    qsort(src_pixels, total1, sizeof(src_pix_info), cmp_luminance_src);

    tgt_pixels = (trgt_pix_info*)malloc(sizeof(trgt_pix_info) * total2);
    if (!tgt_pixels) { sat = 3; goto cleanup; }

    for (int y = 0; y < h2; ++y)
        for (int x = 0; x < w2; ++x) {
            int idx = (y * w2 + x) * 3;
            trgt_pix_info *p = &tgt_pixels[y * w2 + x];
            p->lum = luminance(img2[idx+0], img2[idx+1], img2[idx+2]); p->x = x; p->y = y;
        }
    qsort(tgt_pixels, total2, sizeof(trgt_pix_info), cmp_luminance_tgt);

    out_img = (unsigned char*)malloc(3 * w2 * h2);
    if (!out_img) { sat = 4; goto cleanup; }

    for (int i = 0; i < w2 * h2 * 3; ++i) out_img[i] = 0;
    for (int i = 0; i < N; ++i) {
        int x = tgt_pixels[i].x, y = tgt_pixels[i].y, idx = (y * w2 + x) * 3;
        out_img[idx+0] = src_pixels[i].r; out_img[idx+1] = src_pixels[i].g; out_img[idx+2] = src_pixels[i].b;
    }

    if (!stbi_write_jpg(out_path, w2, h2, 3, out_img, 100)) { sat = 5; goto cleanup; }

cleanup:
    if (src_pixels) free(src_pixels);
    if (tgt_pixels) free(tgt_pixels);
    if (img1) stbi_image_free(img1);
    if (img2) stbi_image_free(img2);
    if (out_img) free(out_img);

    return sat;
}