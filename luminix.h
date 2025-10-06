#ifndef LUMINIX_H
#define LUMINIX_H

float color_distance(unsigned char r1, unsigned char g1, unsigned char b1,
                     unsigned char r2, unsigned char g2, unsigned char b2);
int luminix(const char *src_path, const char *tgt_path, const char *out_path);

#endif // LUMINIX_H