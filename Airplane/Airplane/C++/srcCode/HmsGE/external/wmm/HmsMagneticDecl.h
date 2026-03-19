#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	extern double HmsGetMagneticDeclination(double lon, double lat, double heightKM, int year, int month, int day);
	extern double HmsMagneticHeadingToTrueHeading(double lon, double lat, double heightKM, double mag);


#ifdef __cplusplus
	}
#endif
