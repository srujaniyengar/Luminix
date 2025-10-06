#include "luminix.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float color_distance(unsigned char r1, unsigned char g1, unsigned char b1,
                     unsigned char r2, unsigned char g2, unsigned char b2) {
  int dr = (int)r1 - (int)r2;
  int dg = (int)g1 - (int)g2;
  int db = (int)b1 - (int)b2;
  return sqrtf(dr * dr + dg * dg + db * db);
}

int luminix(const char *src_path, const char *tgt_path, const char *out_path) {
  int w1, h1, c1, w2, h2, c2;
  unsigned char *img1 = NULL, *img2 = NULL, *out_img = NULL;
  int status = 0;

  img1 = stbi_load(src_path, &w1, &h1, &c1, 3);
  img2 = stbi_load(tgt_path, &w2, &h2, &c2, 3);
  if (!img1 || !img2) {
    status = 1;
    goto cleanup;
  }

  int total1 = w1 * h1, total2 = w2 * h2;
  int N = (total1 < total2) ? total1 : total2;
  if (total1 != total2) {
    fprintf(stderr, "Warn\n");
  }

  out_img = (unsigned char *)malloc(3 * w2 * h2);
  if (!out_img) {
    status = 2;
    goto cleanup;
  }
  memset(out_img, 0, 3 * w2 * h2);

  bool *src_used = (bool *)calloc(total1, sizeof(bool));
  if (!src_used) {
    status = 3;
    goto cleanup;
  }

  for (int y = 0; y < h2; ++y) {
    for (int x = 0; x < w2; ++x) {
      int tgt_idx = (y * w2 + x) * 3;
      unsigned char tr = img2[tgt_idx + 0];
      unsigned char tg = img2[tgt_idx + 1];
      unsigned char tb = img2[tgt_idx + 2];

      float min_dist = 1e12f;
      int best_src = -1;
      for (int s = 0; s < total1; ++s) {
        if (src_used[s])
          continue;
        int src_idx = s * 3;
        unsigned char sr = img1[src_idx + 0];
        unsigned char sg = img1[src_idx + 1];
        unsigned char sb = img1[src_idx + 2];
        float dist = color_distance(tr, tg, tb, sr, sg, sb);
        if (dist < min_dist) {
          min_dist = dist;
          best_src = s;
          if (dist == 0)
            break;
        }
      }
      if (best_src >= 0) {
        int src_idx = best_src * 3;
        out_img[tgt_idx + 0] = img1[src_idx + 0];
        out_img[tgt_idx + 1] = img1[src_idx + 1];
        out_img[tgt_idx + 2] = img1[src_idx + 2];
        src_used[best_src] = true;
      }
    }
    if (y % 20 == 0)
      printf("Row %d/%d processed\n", y, h2);
  }

  int ok = stbi_write_png(out_path, w2, h2, 3, out_img, w2 * 3);
  if (!ok) {
    status = 4;
    goto cleanup;
  }

  printf("Done! %s\n", out_path);

cleanup:
  if (img1)
    stbi_image_free(img1);
  if (img2)
    stbi_image_free(img2);
  if (out_img)
    free(out_img);
  return status;
}
