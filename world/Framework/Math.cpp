#include "Math.h"

namespace Math {

	int NearestUpperPow2( const int value ) {
		if ( value <= 0 ) {
			return 1;
		}
		int pow = 1;
		while ( value < pow ) {
			pow *= 2;
		}
		return pow;
	}

	int NearestLowerPow2( const int value ) {
		if ( value <= 0 ) {
			return 1;
		}
		int pow = 1;
		while ( value < pow * 2 ) {
			pow *= 2;
		}
		return pow;
	}
}