#ifndef _MY_COM_MATH_
#define _MY_COM_MATH_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PI
#undef PI
#undef PIx2
#undef PI_2
#undef PI_4
#endif

    extern const double PI;//3.14159265358979323846;
    extern const double PIx2;//6.28318530717958647692;
    extern const double PI_2;//1.57079632679489661923;
    extern const double PI_4;//0.78539816339744830962;
    extern const double LN_2;//0.69314718055994530942;
    extern const double INVLN_2;//1.442695040888963;
    extern const double RadiansToDegrees;//57.295779513082320876846364344191;
    extern const double DegreesToRadians;//0.01745329251994329576922222222222;
    extern const double RadiansToDegrees_2;//0.00872664625997164788461111111111;
    extern const double MeterPerFeet;//0.3048f;
    extern const double FeetPerMeter;//3.280840f;

#define TAWS_MIN(a,b) (((a) < (b)) ? (a) : (b))
#define TAWS_MAX(a,b) (((a) > (b)) ? (a) : (b))

#ifdef __cplusplus
}
#endif

#endif
