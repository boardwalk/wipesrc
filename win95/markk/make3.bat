@move *.bak bak
@del log\*.log
wcc386 -d2 -dWIPEOUTPC -bt=dos4g etcvars.c > log\etcvars.log
wcc386 -d2 -dWIPEOUTPC -s -bt=dos4g pmkey.c > log\pmkey.log
wcc386 -d2 -dWIPEOUTPC -s -bt=dos4g pmjoy.c > log\pmjoy.log
wcc386 -d2 -dWIPEOUTPC -s -bt=dos4g pmmouse.c > log\pmmouse.log
wcc386 -d2 -dWIPEOUTPC -bt=dos4g initpad.c > log\initpad.log
wcc386 -d2 -dWIPEOUTPC -bt=dos4g startpad.c > log\startpad.log
wcc386 -d2 -dWIPEOUTPC -bt=dos4g stoppad.c > log\stoppad.log
wcc386 -d2 -dWIPEOUTPC -bt=dos4g handler.c > log\handler.log
wcc386 -d2 -dWIPEOUTPC -bt=dos4g config.c > log\config.log
wcc386 -d2 -dWIPEOUTPC -bt=dos4g timer.c > log\timer.log
wcc386 -d2 -dWIPEOUTPC -bt=dos4g pmtimer.c > log\pmtimer.log
wlib -n libapi.lib @libapi.mod > log\libapi.log
wcl386 /s /d2 /l=dos4g test3.c libapi.lib > log\test3.log
@cd log
@copy /a etcvars.log + pmkey.log + pmjoy.log + pmmouse.log 0.log
@copy /a initpad.log + startpad.log + stoppad.log 1.log
@copy /a handler.log + config.log 2.log
@copy /a timer.log + pmtimer.log 3.log
@copy /a 0.log + 1.log + 2.log + 3.log +libapi.log + test3.log test3.err
@cd ..
@copy log\test3.err .


