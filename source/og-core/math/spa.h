// credits: based on http://www.nrel.gov/docs/fy04osti/34302b.pdf

#ifndef __solar_position_algorithm_header
#define __solar_position_algorithm_header

#include <iostream>

class spa_data
{
public:
	//----------------------INPUT VALUES------------------------

	int year;			   // 4-digit year
	int month;			   // 2-digit month (1-12)
	int day;			      // 2-digit day (1-31)
	int hour;			   // observer local hour
	int minute;			   // observer local minute
	int second;			   // observer local second

	float delta_t;	 	   // difference between earth rotation time and terrestrial time
						      // (from observation) [seconds]
	float timezone;	   // observer timezone (negative west of greenwich) [hours]
	float longitude;	   // observer longitude (negative west of greenwich) [degrees]
	float latitude;	   // observer latitude (negative south of equator) [degrees]
	float elevation;	   // observer elevation [meters]
	float pressure;		// annual average local pressure [millibars]
	float temperature;	// annual average local temperature [degrees celcius]

	float slope;		   // surface slope (measured from the horizontal plane) [degrees]
	float azm_rotation;	// surface azimuth rotation (measured from south to projection of
						      // surface normal on horizontal plane, negative west) [degrees]

	float atmos_refract; // atmospheric refraction at sunrise and sunset [degrees]
						      // 0.5667 degrees is typical

	//-----------------Intermediate OUTPUT VALUES--------------------

	double jd;		      // julian day
	double jc;		      // julian century;

	double jde;		      // julian ephemeris day
	double jce;		      // julian ephemeris century
	double jme;		      // julian ephemeris millennium

	double l;		      // earth heliocentric longitude [degrees]
	double b;		      // earth heliocentric latitude [degrees]
	double r;		      // earth radius vector [Astronomical Units, AU]

	double theta;	      // geocentric longitude [degrees]
	double beta;	      // geocentric latitude [degrees]

	double x0;		      // mean elongation (moon-sun) [degrees]
	double x1;		      // mean anomaly (sun) [degrees]
	double x2;		      // mean anomaly (moon) [degrees]
	double x3;		      // argument latitude (moon) [degrees]
	double x4;		      // ascending longitude (moon) [degrees]

	double del_psi;		// nutation longitude [degrees]
	double del_epsilon;  // nutation obliquity [degrees]
	double epsilon0;	   // ecliptic mean obliquity [arc seconds]
	double epsilon;		// ecliptic true obliquity	[degrees]

	double del_tau;		// aberration correction [degrees]
	double lamda;		   // apparent sun longitude [degrees]
	double nu0;			   // greenwich mean sidereal time [degrees]
	double nu;			   // greenwich sidereal time [degrees]

	double alpha;		   // geocentric sun right ascension [degrees]
	double delta;		   // geocentric sun declination [degrees]

	double h;			   // observer hour angle [degrees]
	double xi;			   // sun equatorial horizontal parallax [degrees]
	double del_alpha;	   // sun right ascension parallax [degrees]
	double delta_prime;  // topocentric sun declination [degrees]
	double alpha_prime;  // topocentric sun right ascension [degrees]
	double h_prime;		// topocentric local hour angle [degrees]

	double e0;			   // topocentric elevation angle (uncorrected) [degrees]
	double del_e;		   // atmospheric refraction correction [degrees]
	double e;			   // topocentric elevation angle (corrected) [degrees]

	//---------------------Final OUTPUT VALUES------------------------

	double zenith;		   // topocentric zenith angle [degrees]
	double azimuth180;	// topocentric azimuth angle (westward from south) [-180 to 180 degrees]
	double azimuth;		// topocentric azimuth angle (eastward from north) [0 to 360 degrees]
	double incidence;	   // surface incidence angle [degrees]
	double altitude;	   // topocentric altitude angle of the sun

	double eot;			   // equation of time [degrees]
	double suntransit;	// local sun transit time (or solar noon) [fractional hour]
	double sunrise;		// local sunrise time [fractional hour]
	double sunset;		   // local sunset time [fracitonal hour]

   void print(void)
   {
      std::cout << "julian day: " << jd << "\n";
      std::cout << "julian century: " << jc << "\n";
      std::cout << "julian ephemeris day: " << jde << "\n";
      std::cout << "julian ephemeris century: " << jce << "\n";
      std::cout << "julian ephemeris millennium: " << jme << "\n";

      std::cout << "earth heliocentric longitude: " << l << "\n";
      std::cout << "earth heliocentric latitude: " << b << "\n";
      std::cout << "earth radius vector [AU]: " << r << "\n";
      
      std::cout << "geocentric longitude: " << theta << "\n";
      std::cout << "geocentric latitude: " << beta << "\n";

      std::cout << "topocentric zenith angle: " << zenith << "\n";
      std::cout << "topocentric azimuth angle (west from south): " << azimuth180 << "\n";
      std::cout << "topocentric azimuth angle (east from north): " << azimuth << "\n";
      std::cout << "surface incidence angle: " << incidence << "\n";
      std::cout << "topocentric altitude angle (corrected): " << altitude << "\n";
      std::cout << "topocentric altitude angle (uncorrected): " << e0 << "\n";
      std::cout << "atmospheric refraction correction: " << del_e << "\n";


      std::cout << "local sun transit time: " << suntransit << "\n";
      std::cout << "local sunrise time: " << sunrise << "\n";
      std::cout << "local sunset time: " << sunset << "\n";
   }
};

//Calculate all SPA output values (in structure) based on input values passed in structure
void SetCommonValues(spa_data &spa, float longitude, float latitude,
		int year, int month, int day, int hour, int minute, int second,
		float timezone, float elevation);
void spa_calculate(spa_data *spa);

#endif



