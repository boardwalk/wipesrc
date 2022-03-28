wcc386 -d2 -bt=dos4g etcvars.c
wcc386 -d2 -s -bt=dos4g pmkey.c
wcc386 -d2 -s -bt=dos4g pmjoy.c
wcc386 -d2 -s -bt=dos4g pmmouse.c
wcc386 -d2 -bt=dos4g initpad.c
wcc386 -d2 -bt=dos4g startpad.c
wcc386 -d2 -bt=dos4g stoppad.c
wcc386 -d2 -bt=dos4g handler.c
wcc386 -d2 -bt=dos4g config.c
wcc386 -d2 -bt=dos4g timer.c
wcc386 -d2 -bt=dos4g pmtimer.c
wlib -n libapi.lib @libapi.mod
wcl386 /s /d2 /l=dos4g test3.c libapi.lib

