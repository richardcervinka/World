#include "Graphicsinfrastructure.h"
#include "Framework/Math.h"

/*
Funkce FindMode() je umistena je v zakladni tride, aby bylo zajisteno, ze je algoritmus
vyhledani rezimu displeje stejny pro vsechny implementace (platformy).
*/
bool Display::FindMode( const DisplayMode& request, DisplayMode& result ) const noexcept {

	DisplayMode best;
	if ( !GetMode( 0, best ) ) {
		return false;
	}
	int bestDivergence = Math::Abs( request.width - best.width ) + Math::Abs( request.height - best.height );
	int bestId = 0;

	DisplayMode mode;
	int id = 0;
	while ( GetMode( ++id, mode ) ) {
		int divergence = Math::Abs( mode.width - request.width ) + Math::Abs( mode.height - request.height );

		// horsi odchylka rozlyseni, porovnat dalsi rezim
		if ( divergence > bestDivergence ) {
			continue;
		}
		// stejna odchylka rozlyseni, porovnat refresh rate
		if ( divergence == bestDivergence ) {

			// porovnat refresh rate
			const float refreshRate = static_cast< float >( mode.refreshRateNumerator ) / static_cast< float >( mode.refreshRateDenominator );
			const float bestRefreshRate = static_cast< float >( best.refreshRateNumerator ) / static_cast< float >( best.refreshRateDenominator );
			
			// nalezeny mod ma horsi refresh rate, hledat dal
			if ( refreshRate < bestRefreshRate ) {
				continue;
			}
			// nalezeny mod ma lepsi refresh rate, ulozit novy mod
			best = mode;
			bestDivergence = divergence;
			bestId = id;
			continue;
		}
		// lepsi odchylka rozlyseni, ulozit mod
		best = mode;
		bestDivergence = divergence;
		bestId = id;
	}
	result = best;
	return true;
}