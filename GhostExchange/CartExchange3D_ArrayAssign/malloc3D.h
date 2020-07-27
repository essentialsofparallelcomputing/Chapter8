#ifndef _MALLOC3D_h
#define _MALLOC3D_h
#ifdef __cplusplus
extern "C" {
#endif
   double ***malloc3D(int kmax, int jmax, int imax, int koffset, int joffset, int ioffset);
   void malloc3D_free(double ***x, int koffset, int joffset, int ioffset);
#ifdef __cplusplus
}
#endif
#endif
