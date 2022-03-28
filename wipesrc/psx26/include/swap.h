#define	get_obj_start(a)		DsGetObjStart(a)
#define	get_obj_end(a)			DsGetObjEnd(a)
#define	place_obj(a,b,c,d)	DsPlaceObj(a,b,c,d)
#define	kill_obj(a)				DsKillObj(a)
#define	load_file(a,b)			DsLoadFile(a,b)
#define	move_objs()				DsMoveObjs()
#define	see_objs()				DsSeeObjs()
#define	clear_objs()				DsClearObjs()
#define	set_view(a,b,c,d,e,f)	DsSetView(a,b,c,d,e,f)
#define	init_hardware(a,b,c)		DsInitHardware(a,b,c)
#define	init_shape(a)				DsInitShape(a)
#define	load_texture_into_vram(a)	DsLoadTextureIntoVram(a)
#define	set_fog_depth(a)			DsSetFogDepth(a)
#define	set_near_clip(a)			DsSetNearClip(a)
#define	set_far_clip(a)			DsSetFarClip(a)
#define	set_fog_colour(a,b,c)		DsSetFogColour(a,b,c)
#define	set_ambient_colour(a,b,c)	DsSetAmbientColour(a,b,c)
#define	colour_light(a,b,c,d)		DsColourLight(a,b,c,d)
#define	point_light(a,b,c,d)			DsPointLight(a,b,c,d)
#define	set_world_matrix_to_view(a)	DsSetWorldMatrixToView(a)

#define	ReadJoy(a)		DsReadJoy(a)
#define	go(x,y)			DsGo(x,y)
#define	print(a)			DsPrint(a)
#define	printx(a)		DsPrintx(a)
#define	cprintx(a)		DsCprintx(a)
#define	prdec(a)			DsPrdec(a)
#define	prdig(a)			DsPrdig(a)


#define	colour(a,b,c)	DsColour(a,b,c)
#define	swap()			DsSwap()
#define	syncup()			DsSyncUp()
#define	cls(a,b,c)		DsCls(a,b,c)
#define	begin(a)			DsBegin(a)
#define	centre_tmd(a)	DsCentreTmd(a)
#define	format_tmd(a)	DsFormatTmd(a)
#define	colour_tmd(a,r,g,b)	DsColourTmd(a,r,g,b)


#define	OBJECT	DsOBJECT
#define	POINT		DsPOINT
#define	SHAPE		DsSHAPE
#define	TEXT		DsTEXT
#define	COORD		DsCOORD




#define	_CLIP		Ds_CLIP
#define	_MESH		Ds_MESH
