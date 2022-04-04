#ifndef SEARCH_H
#define SEARCH_H

#define LOOK_BACK 3
#define LOOK_AHEAD 6

int32_t			 	SectionSearch(ShipData*);
TrackSection* 	FindNearestSection(TrackSection*, VECTOR);

#endif
