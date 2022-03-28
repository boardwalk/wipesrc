// structures needed by system configuration checking module
// written by Andy Satterthwaite (c) Psygnosis, 1995
// for use in PC CD based games inc. RidgeRacer, Krazy Ivan & Wipeout
// based on Psygnosis Install program by Mark Kelly & Andy Satterthwaite

typedef struct	sbconfig_type
{
	short	port;
	short	irq;		
	short	dma8;		
	short	dma16;
}	SBCONFIG;	// sound blaster configuration structure
		  
typedef	struct	usconfig_type
{
	short port;
	short dramdma;
	short adcdma;
	short gf1irq;
	short midiirq;
}	USCONFIG;	// ultrasound configuration structure

typedef struct sysconfig_type
{
	unsigned	char	processor_type;		// 3=386, 4=486, 5=Pentium
	unsigned	char	processor_manufacturer;	// 1=genuine intel, 0=scabby make
	unsigned	char	processor_speed;	// in Mhz (+-2ish)
	unsigned	char	fpu;				// 1 if installed, 0 if absent
	unsigned	char	vga_card;			// 1 if detected, 0 if absent
	unsigned	char	svga_card;			// 1 if VESA standard card detected, 0 if absent
	unsigned	char	mouse;				// 1 if mouse installed, 0 if not installed
	unsigned	short	memory;				// amount (in Kb) of hardware memory detected (not free mem!)
	unsigned	long	cdrive_free;		// bytes free on drive C:
	unsigned	long	cdrive_total;		// total size of C: drive in bytes
	  signed	char	soundcard;			// -1 = NO SOUND CARD DETECTED
											//  0 = SB1.5/SB MCV
											//  1 = SBPRO/SBPRO MCV
											//  2 = SB16/SB16 ASP/SB
											//  3 = AWE32
											//  4 = SB2.0
											//  8 = Gravis Ultrasound
	USCONFIG			ultrasound;			// only filled in uf ultra sound card detected - see structure above
	SBCONFIG			soundblaster;		// obly filled in if one of the sound blaster range of cards is detected - see structure above
}	SYSCONFIG;
