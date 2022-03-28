// quick program which calls the GetSystemConfig() function (in syscheck.lib)
// and then displays the values returned in the SYSCONFIG structure	(definition in syscheck.h)

#include <stdio.h>
#include "syscheck.h"

void	syscheck()
{
	SYSCONFIG	config;
	
// 	printf("***** SYSTEM CONFIGURATION CHECK *****\n");

	GetSystemConfig(&config);

	if (config.processor_type<5)
	{
		printf("\n'wipEout' runs best on an Intel Pentium Processor\n");
		printf("\nPress A Key\n");
		getch();
	}


//	switch(config.processor_type)
// 	{
// 		case 3:	printf("\n%d Mhz 80386 processor detected (never mind)\n",config.processor_speed); break;
// 		case 4:  printf("\n%d Mhz 80486 processor detected\n",config.processor_speed);	break;
// 		case 5:  printf("\n%d Mhz Pentium processor detected\n",config.processor_speed);	break;
// 		default:	printf("\n%d Mhz unknown processor (type %d) detected\n",config.processor_speed,config.processor_type); 	break;
// 	}
//	if (config.processor_manufacturer)
//		printf("*** A Genuine Intel Processor ***\n");
//	else
// 		printf("THIS IS NOT A GENUINE INTEL PROCESSOR\n");
//
// 	if (config.fpu) printf("\nFPU detected\n");
// 
// 	printf("\n%dK Hardware Memory Detected\n",config.memory);
// 
// 	printf("\n%ld bytes free on drive C: (Total Disk Size = %ld bytes)\n",config.cdrive_free,config.cdrive_total);
// 
// 	if (config.vga_card) printf("\nVGA  compatible graphics card found\n");
// 	if (config.svga_card) printf("SVGA compatible graphics card found\n");
// 
// 	switch (config.soundcard)
// 	{
// 		case	-1: printf("\nNo Sound Card"); break;
// 		case	 0: printf("\nSound Card Detected:\nSoundBlaster: port=0x%x, irq=%d, dma8=%d, dma16=%d\n", config.soundblaster.port, config.soundblaster.irq, config.soundblaster.dma8, config.soundblaster.dma16); break;
// 		case	 4: printf("\nSound Card Detected:\nSoundBlaster 2: port=0x%x, irq=%d, dma8=%d, dma16=%d\n", config.soundblaster.port, config.soundblaster.irq, config.soundblaster.dma8, config.soundblaster.dma16); break;
// 		case	 1: printf("\nSound Card Detected:\nSoundBlaster PRO: port=0x%x, irq=%d, dma8=%d, dma16=%d\n", config.soundblaster.port, config.soundblaster.irq, config.soundblaster.dma8, config.soundblaster.dma16); break;
// 		case	 2: printf("\nSound Card Detected:\nSoundBlaster 16: port=0x%x, irq=%d, dma8=%d, dma16=%d\n", config.soundblaster.port, config.soundblaster.irq, config.soundblaster.dma8, config.soundblaster.dma16); break;
// 		case	 3: printf("\nSound Card Detected:\nSoundBlaster AWE32: port=0x%x, irq=%d, dma8=%d, dma16=%d\n", config.soundblaster.port, config.soundblaster.irq, config.soundblaster.dma8, config.soundblaster.dma16); break;
// 		case	 8: printf("\nSound Card Detected:\nGravis Ultrasound: port=0x%x, gf1 irq=%d, midi irq=%d, dram dma=%d, adc dma=%d\n", config.ultrasound.port, config.ultrasound.gf1irq, config.ultrasound.midiirq, config.ultrasound.dramdma, config.ultrasound.adcdma); break;
// 		default:	printf("\nSound Card Detected:\nUnknown Card"); break;
// 	} 
// 
// 	if (config.mouse) printf("\nMouse Installed\n");
// 	else printf("\nMouse NOT Installed\n");
}


