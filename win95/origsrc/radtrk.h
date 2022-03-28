char SectionPosition
( 
	TrackSection      *start, 
	TrackSection      *end
);

short *compress ( short *numbers, short *count );

void sortDistances ( long numbers[ MaxGlobalList ][ 2 ], short count );

void TrackRadiusCheckNew( );
