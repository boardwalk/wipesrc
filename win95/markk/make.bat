@move *.bak bak
wcc386 -d2 -bt=dos4g pmint09.c > log\pmint09.log
wcc386 -d2 -bt=dos4g padinit.c > log\padinit.log
wcc386 -d2 -bt=dos4g padread.c > log\padread.log
wcc386 -d2 -bt=dos4g padstat.c > log\padstat.log
wcc386 -d2 -bt=dos4g etc.c > log\etc.log
wlib -n libetc.lib @libetc.mod > log\lib.log
wcl386 /d2 /l=dos4g test.c libetc.lib > log\test.log
@cd log
@copy /a pmint09.log + padinit.log + padread.log + padstat.log 0.log
@copy /a 0.log + etc.log + lib.log + test.log test.err
@cd ..
@copy log\test.err .








											   
