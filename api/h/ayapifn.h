#ifndef	_AYAPIFN_
#define	_AYAPIFN_

//Please remember that changes to this MUST be reflected in ayapidll.h
extern	AYERR	AYSetDisplayMask(int	mask);
extern	AYERR	AYLoadDriver(int	DriverID);
extern	AYERR	AYInitDriver(void);
extern	AYERR	AYDeInitDriver(void);
extern	AYERR	AYUnloadDriver(void);
extern	AYERR	AYResetDriver(void);
extern	AYERR	AYSetClipRegion(AYRECT	*clip_region);
extern	AYERR	AYSetDrawMode(AYDRAWMODE	*draw_mode);
extern	AYERR	AYSetDrawBuffer(int	buffer_number);
extern	AYERR	AYSetDispBuffer(int	buffer_number);
extern	int		AYGetDispBuffer(void);
extern	int		AYGetDrawBuffer(void);
extern	AYERR	AYGetCaps(AYCAPS	*capabilities);
extern	AYERR	AYLoadVRAM(void *data, AYTEXPAGE	tpage, AYRECT	*coord, int	depth);
extern	AYERR	AYSaveVRAM(void *data, AYTEXPAGE	tpage, AYRECT	*coord, int	depth);
extern	AYERR	AYMoveVRAM(AYTEXPAGE	srctpage, AYRECT	*srccoord, AYTEXPAGE	desttpage, AYRECT	*destcoord, int	depth);
extern	AYERR	AYRenderPrim(AYPRIM	*primitive);
extern	AYERR	AYRenderOT(AYPRIM	*primitive);
extern	AYERR	AYDrawSync(void);
extern	AYERR	AYVSync(void);

#endif
