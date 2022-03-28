@move *.bak bak
wcc386 -d2 -bt=dos4g pmint09.c > log\pmint09.log
wcc386 -d2 -bt=dos4g initpad.c > log\initpad.log
wcc386 -d2 -bt=dos4g startpad.c > log\startpad.log
wcc386 -d2 -bt=dos4g stoppad.c > log\stoppad.log
wcc386 -d2 -bt=dos4g etc.c > log\etc.log
wlib -n libapi.lib @libapi.mod > log\libapi.log
wcl386 /d2 /l=dos4g test2.c libapi.lib > log\test2.log
@cd log
@copy /a pmint09.log + initpad.log + startpad.log + stoppad.log 0.log
@copy /a 0.log + etc.log + libapi.log + test2.log test2.err
@cd ..
@copy log\test2.err .


