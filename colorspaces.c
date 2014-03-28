#include <math.h>
#define TAU 6.283185307179586476925287

void hcl2lab(double* L, double* a, double* b, double h, double c, double l) {
  double r;
  *L=l*0.7;
  r=0.426*c;
  double angle = TAU/6.0-h*TAU;
  *a = sin(angle)*r;
  *b = cos(angle)*r;
}

double finv(double t) {
  return (t>(6.0/29.0))?(t*t*t):(3*(6.0/29.0)*(6.0/29.0)*(t-4.0/29.0));
}

void lab2xyz(double* x, double* y, double* z, double l, double a, double b) {
  double sl = (l+0.16)/1.16;
  double ill[3] = {0.9643,1.00,0.8251}; //D50
  *y = ill[1] * finv(sl);
  *x = ill[0] * finv(sl + (a/5.0));
  *z = ill[2] * finv(sl - (b/2.0));
}

void hcl2xyz(double *x, double *y, double *z, double h, double c, double l) {
  double L, a, b;
  hcl2lab(&L,&a,&b,h,c,l);
  lab2xyz(x,y,z,L,a,b);
}

double correct(double cl) {
  double a = 0.055;
  return (cl<=0.0031308)?(12.92*cl):((1+a)*pow(cl,1/2.4)-a);
}

/* Convert from XYZ doubles to sRGB bytes */
void xyz2rgb(unsigned char* r, unsigned char* g, unsigned char* b, double x, double y, double z) {
  double rl =  3.2406*x - 1.5372*y - 0.4986*z;
  double gl = -0.9689*x + 1.8758*y + 0.0415*z;
  double bl =  0.0557*x - 0.2040*y + 1.0570*z;
  int clip = (rl < 0.001 || rl > 0.999 || gl < 0.001 || gl > 0.999 || bl < 0.001 || bl > 0.999);
  if(clip) {
    rl = (rl<0.001)?0.0:((rl>0.999)?1.0:rl);
    gl = (gl<0.001)?0.0:((gl>0.999)?1.0:gl);
    bl = (bl<0.001)?0.0:((bl>0.999)?1.0:bl);
  }
  *r = (unsigned char)(255.0*correct(rl));
  *g = (unsigned char)(255.0*correct(gl));
  *b = (unsigned char)(255.0*correct(bl));
}

/* Convert from LAB doubles to sRGB bytes */
void lab2rgb(unsigned char* R, unsigned char* G, unsigned char* B, double l, double a, double b) {
  double x,y,z;
  lab2xyz(&x,&y,&z,l,a,b);
  xyz2rgb(R,G,B,x,y,z);
}

void xyz2pix(void* rgb, double x, double y, double z) {
  unsigned char* ptr = (unsigned char*)rgb;
  xyz2rgb(ptr,ptr+1,ptr+2,x,y,z);
}

void lab2pix(void* rgb, double l, double a, double b) {
  unsigned char* ptr = (unsigned char*)rgb;
  lab2rgb(ptr,ptr+1,ptr+2,l,a,b);
}

/* Convert from a qualitative parameter h and a quantitative parameter l to a 24-bit pixel */
void hl2pix(void* rgb, double h, double l) {
  unsigned char* ptr = (unsigned char*)rgb;
  double L,r;
  L = l*0.7;         //L of L*a*b*
  r = l*0.301+0.125; //chroma
  double angle = TAU/6.0-h*TAU;
  double a = sin(angle)*r;
  double b = cos(angle)*r;
  lab2rgb(ptr,ptr+1,ptr+2,L,a,b);
}

/* Convert from a qualitative parameter h and two quantitative parameter c and l to a 24-bit pixel */
void hcl2pix(void* rgb, double h, double c, double l) {
  double L, a, b;
  hcl2lab(&L,&a,&b,h,c,l);
  lab2pix(rgb,L,a,b);
}
