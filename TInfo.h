#ifndef _TINFO_H_
#define _TINFO_H_

// Struct TInfo is an arbitrary sort of structure... at some point, I'll
// probably expand it to hold all the sorts of data that gets stuffed into
// a GeoTIFF, but for the moment, it only covers a few fields that are
// stored in a USGS DRG. As it's only a struct (as opposed to an object),
// addition, removal, and reordering of fields is pretty much arbitrary... 

struct TInfo
{
	long	width;
	long	height;
	double	utmx;
	double	utmy;
	double	utmxres;
	double	utmyres;
	short	utmzone;
	double	pixsizex;
	double	pixsizey;
	double	pixsizez;
	long	scale;
	long	pid;
	char	cellname[32];
	unsigned short	CSProj;
};

#endif
