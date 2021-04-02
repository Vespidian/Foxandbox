#include "../../global.h"
#include "../../utility.h"
#include "perlin.h"
#include <math.h>

int  perlin_seed = 1985;

static double lin_inter(double x, double y, double s)
{
    return x + s * (y-x);
}

static double smooth_inter(double x, double y, double s)
{
    return lin_inter( x, y, s * s * (3-2*s) );
    // t * t * t * (t * (t * 6 - 15) + 10)
    // return lin_inter( x, y, s * s * s * ((s * 6 - 15) + 10) );
}

double noise2d(double x, double y){
    const int  x_int = floor( x );
    const int  y_int = floor( y );
    const double  x_frac = x - x_int;
    const double  y_frac = y - y_int;
    const int  s = WhiteNoise( x_int, y_int );
    const int  t = WhiteNoise( x_int+1, y_int );
    const int  u = WhiteNoise( x_int, y_int+1 );
    const int  v = WhiteNoise( x_int+1, y_int+1 );
    const double  low = smooth_inter( s, t, x_frac );
    const double  high = smooth_inter( u, v, x_frac );
    const double  result = smooth_inter( low, high, y_frac );
    return result;
}

double Perlin_Get2d(double x, double y, double scale, int octaves){
    double  xa = x * scale;
    double  ya = y * scale;
    double  amp = 1.0;
    double  fin = 0;
    double  div = 0.0;
    for (int o = 0; o < octaves; o++){
        div += 255 * amp;
        fin += noise2d( xa, ya ) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }
    return fin/div;
}