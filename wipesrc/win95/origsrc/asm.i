
; (C) Psygnosis 1994. By Jason Carl Denton


; macro to set the rs variable to an even word boundary

evenw    macro

         if       __rs&3=1
         rb       3
         elseif   __rs&3=2
         rb       2
         elseif   __rs&3=3
         rb       1
         endif
         
         endm



; macro to set the rs variable to an even halfword boundary

evenh    macro

         if       __rs&3=1
         rb       1
         endif
         
         endm





; GTE flags

CrtClipped     equ      $80000000
XLarge         equ      $01000000
YLarge         equ      $00800000
ZLarge         equ      $00400000
ZNegative      equ      $00040000
ZSmall         equ      $00020000
XClipped       equ      $00004000
YClipped       equ      $00002000
OutFog         equ      $00001000

ClipFlags      equ      (CrtClipped|ZLarge|ZNegative|ZSmall)

ShortClipFlags equ      ((ZLarge|ZNegative|ZSmall|XClipped|YClipped)>>12)

FarClipFlags   equ      ((ZNegative|ZSmall|XClipped|YClipped)>>12)

; assembler SVECTOR data structure

   rsreset

SVECTOR.vx      rh      1
SVECTOR.vy      rh      1
SVECTOR.vz      rh      1
SVECTOR.pad     rh      1
SVECTOR.sizeof  rw      0

; assembler VECTOR data structure

   rsreset

VECTOR.vx      rw       1
VECTOR.vy      rw       1
VECTOR.vz      rw       1
VECTOR.pad     rw       1
VECTOR.sizeof  rw       0

; assembler Tile data structure

   rsreset

Tile.tsb             rh    1
Tile.cba             rh    1
Tile.u0              rb    1
Tile.v0              rb    1
Tile.u1              rb    1
Tile.v1              rb    1
Tile.u2              rb    1
Tile.v2              rb    1
Tile.u3              rb    1
Tile.v3              rb    1
Tile.sizeof          rw    0

; assembler Surface data structure

   rsreset

Surface.low          rb    Tile.sizeof
Surface.med          rb    Tile.sizeof*4
Surface.high         rb    Tile.sizeof*16
Surface.sizeof       rw    0

; assembler TEX data structure

   rsreset

TEX.uv         rh       0
TEX.u          rb       1
TEX.v          rb       1
TEX.sizeof     rw       0

; assembler RotTransPersN data structure

   rsreset

RTPN.screen    rw       1
RTPN.depth     rh       1
RTPN.flags     rh       1
RTPN.sizeof    rw       0

; assembler Mesh data structure

   rsreset

Mesh.coords    rh       0
Mesh.vx        rh       1
Mesh.vy        rh       1
Mesh.vz        rh       1
Mesh.pad       rh       1
Mesh.screen    rw       1
Mesh.depth     rh       1
Mesh.flags     rh       1
Mesh.sizeof    rw       0

; assembler primitive type equates

TypeF3      equ   1
TypeFT3     equ   2
TypeF4      equ   3
TypeFT4     equ   4
TypeG3      equ   5
TypeGT3     equ   6
TypeG4      equ   7
TypeGT4     equ   8

TypeTSPR    equ   10
TypeBSPR    equ   11
TypeSPR     equ   TypeTSPR

TypeLSF3    equ   12
TypeLSFT3   equ   13
TypeLSF4    equ   14
TypeLSFT4   equ   15
TypeLSG3    equ   16
TypeLSGT3   equ   17
TypeLSG4    equ   18
TypeLSGT4   equ   19


; assembler PolyF3 data structure

   rsreset

PolyF3.tag           rw    1
PolyF3.colour        rw    1
PolyF3.xy0           rw    1
PolyF3.xy1           rw    1
PolyF3.xy2           rw    1
PolyF3.sizeof        rw    0

; assembler PolyF4 data structure

   rsreset

PolyF4.tag           rw    1
PolyF4.colour        rw    1
PolyF4.xy0           rw    1
PolyF4.xy1           rw    1
PolyF4.xy2           rw    1
PolyF4.xy3           rw    1
PolyF4.sizeof        rw    0

; assembler PolyFT3 data structure

   rsreset

PolyFT3.tag          rw    1
PolyFT3.colour       rw    1
PolyFT3.xy0          rw    1
PolyFT3.uv0          rh    1
PolyFT3.cba          rh    1
PolyFT3.xy1          rw    1
PolyFT3.uv1          rh    1
PolyFT3.tsb          rh    1
PolyFT3.xy2          rw    1
PolyFT3.uv2          rh    1
PolyFT3.pad1         rh    1
PolyFT3.sizeof       rw    0

; assembler PolyFT4 data structure

   rsreset

PolyFT4.tag          rw    1
PolyFT4.colour       rw    1
PolyFT4.xy0          rw    1
PolyFT4.uv0          rh    1
PolyFT4.cba          rh    1
PolyFT4.xy1          rw    1
PolyFT4.uv1          rh    1
PolyFT4.tsb          rh    1
PolyFT4.xy2          rw    1
PolyFT4.uv2          rh    1
PolyFT4.pad1         rh    1
PolyFT4.xy3          rw    1
PolyFT4.uv3          rh    1
PolyFT4.pad2         rh    1
PolyFT4.sizeof       rw    0

; assembler PolyG3 data structure

   rsreset

PolyG3.tag           rw    1
PolyG3.colour0       rw    1
PolyG3.xy0           rw    1
PolyG3.colour1       rw    1
PolyG3.xy1           rw    1
PolyG3.colour2       rw    1
PolyG3.xy2           rw    1
PolyG3.sizeof        rw    0

; assembler PolyG4 data structure

   rsreset

PolyG4.tag           rw    1
PolyG4.colour0       rw    1
PolyG4.xy0           rw    1
PolyG4.colour1       rw    1
PolyG4.xy1           rw    1
PolyG4.colour2       rw    1
PolyG4.xy2           rw    1
PolyG4.colour3       rw    1
PolyG4.xy3           rw    1
PolyG4.sizeof        rw    0

; assembler PolyGT3 data structure

   rsreset

PolyGT3.tag          rw    1
PolyGT3.colour0      rw    1
PolyGT3.xy0          rw    1
PolyGT3.uv0          rh    1
PolyGT3.cba          rh    1
PolyGT3.colour1      rw    1
PolyGT3.xy1          rw    1
PolyGT3.uv1          rh    1
PolyGT3.tsb          rh    1
PolyGT3.colour2      rw    1
PolyGT3.xy2          rw    1
PolyGT3.uv2          rh    1
PolyGT3.pad1         rh    1
PolyGT3.sizeof       rw    0

; assembler PolyGT4 data structure

   rsreset

PolyGT4.tag          rw    1
PolyGT4.colour0      rw    1
PolyGT4.xy0          rw    1
PolyGT4.uv0          rh    1
PolyGT4.cba          rh    1
PolyGT4.colour1      rw    1
PolyGT4.xy1          rw    1
PolyGT4.uv1          rh    1
PolyGT4.tsb          rh    1
PolyGT4.colour2      rw    1
PolyGT4.xy2          rw    1
PolyGT4.uv2          rh    1
PolyGT4.pad1         rh    1
PolyGT4.colour3      rw    1
PolyGT4.xy3          rw    1
PolyGT4.uv3          rh    1
PolyGT4.pad2         rh    1
PolyGT4.sizeof       rw    0

; assembler F3 data structure

   rsreset

F3.type              rh    1
F3.flag              rh    1
F3.coords            rh    3
F3.pad1              rh    1
F3.colour            rw    1
F3.sizeof            rw    0

; assembler F4 data structure

   rsreset

F4.type              rh    1
F4.flag              rh    1
F4.coords            rh    4
F4.colour            rw    1
F4.sizeof            rw    0

; assembler FT3 data structure

   rsreset

FT3.type             rh    1
FT3.flag             rh    1
FT3.coords           rh    3
FT3.texture          rh    1
FT3.cba              rh    1
FT3.tsb              rh    1
FT3.u0               rb    1
FT3.v0               rb    1
FT3.u1               rb    1
FT3.v1               rb    1
FT3.u2               rb    1
FT3.v2               rb    1
FT3.pad1             rh    1
FT3.colour           rw    1
FT3.sizeof           rw    0

; assembler FT4 data structure

   rsreset

FT4.type             rh    1
FT4.flag             rh    1
FT4.coords           rh    4
FT4.texture          rh    1
FT4.cba              rh    1
FT4.tsb              rh    1
FT4.u0               rb    1
FT4.v0               rb    1
FT4.u1               rb    1
FT4.v1               rb    1
FT4.u2               rb    1
FT4.v2               rb    1
FT4.u3               rb    1
FT4.v3               rb    1
FT4.pad1             rh    1
FT4.colour           rw    1
FT4.sizeof           rw    0

; assembler G3 data structure

   rsreset

G3.type              rh    1
G3.flag              rh    1
G3.coords            rh    3
G3.pad1              rh    1
G3.colour0           rw    1
G3.colour1           rw    1
G3.colour2           rw    1
G3.sizeof            rw    0

; assembler G4 data structure

   rsreset

G4.type              rh    1
G4.flag              rh    1
G4.coords            rh    4
G4.colour0           rw    1
G4.colour1           rw    1
G4.colour2           rw    1
G4.colour3           rw    1
G4.sizeof            rw    0

; assembler GT3 data structure

   rsreset

GT3.type             rh    1
GT3.flag             rh    1
GT3.coords           rh    3
GT3.texture          rh    1
GT3.cba              rh    1
GT3.tsb              rh    1
GT3.u0               rb    1
GT3.v0               rb    1
GT3.u1               rb    1
GT3.v1               rb    1
GT3.u2               rb    1
GT3.v2               rb    1
GT3.pad1             rh    1
GT3.colour0          rw    1
GT3.colour1          rw    1
GT3.colour2          rw    1
GT3.sizeof           rw    0

; assembler GT4 data structure

   rsreset

GT4.type             rh    1
GT4.flag             rh    1
GT4.coords           rh    4
GT4.texture          rh    1
GT4.cba              rh    1
GT4.tsb              rh    1
GT4.u0               rb    1
GT4.v0               rb    1
GT4.u1               rb    1
GT4.v1               rb    1
GT4.u2               rb    1
GT4.v2               rb    1
GT4.u3               rb    1
GT4.v3               rb    1
GT4.pad1             rh    1
GT4.colour0          rw    1
GT4.colour1          rw    1
GT4.colour2          rw    1
GT4.colour3          rw    1
GT4.sizeof           rw    0

; assembler SPR data structure

   rsreset

SPR.type             rh    1
SPR.flag             rh    1
SPR.coord            rh    1
SPR.width            rh    1
SPR.height           rh    1
SPR.texture          rh    1
SPR.colour           rw    1
SPR.sizeof           rw    0

; assembler LSF3 data structure

   rsreset

LSF3.type            rh    1
LSF3.flag            rh    1
LSF3.coords          rh    3
LSF3.normals         rh    3
LSF3.pad1            rh    1
LSF3.colour          rw    1
LSF3.sizeof          rw    0

; assembler LSF4 data structure

   rsreset

LSF4.type            rh    1
LSF4.flag            rh    1
LSF4.coords          rh    4
LSF4.normals         rh    4
LSF4.pad1            rh    1
LSF4.colour          rw    1
LSF4.sizeof          rw    0

; assembler LSFT3 data structure

   rsreset

LSFT3.type           rh    1
LSFT3.flag           rh    1
LSFT3.coords         rh    3
LSFT3.normals        rh    3
LSFT3.pad1           rh    1
LSFT3.colour         rw    1
LSFT3.texture        rh    1
LSFT3.pad2           rh    1
LSFT3.sizeof         rw    0

; assembler LSFT4 data structure

   rsreset

LSFT4.type           rh    1
LSFT4.flag           rh    1
LSFT4.coords         rh    4
LSFT4.normals        rh    4
LSFT4.pad1           rh    1
LSFT4.colour         rw    1
LSFT4.texture        rh    1
LSFT4.pad2           rh    1
LSFT4.sizeof         rw    0

; assembler LSG3 data structure

   rsreset

LSG3.type            rh    1
LSG3.flag            rh    1
LSG3.coords          rh    3
LSG3.normals         rh    3
LSG3.pad1            rh    1
LSG3.colour0         rw    1
LSG3.colour1         rw    1
LSG3.colour2         rw    1
LSG3.sizeof          rw    0

; assembler LSG4 data structure

   rsreset

LSG4.type            rh    1
LSG4.flag            rh    1
LSG4.coords          rh    4
LSG4.normals         rh    4
LSG4.pad1            rh    1
LSG4.colour0         rw    1
LSG4.colour1         rw    1
LSG4.colour2         rw    1
LSG4.colour3         rw    1
LSG4.sizeof          rw    0

; assembler LSGT3 data structure

   rsreset

LSGT3.type           rh    1
LSGT3.flag           rh    1
LSGT3.coords         rh    3
LSGT3.normals        rh    3
LSGT3.pad1           rh    1
LSGT3.colour0        rw    1
LSGT3.colour1        rw    1
LSGT3.colour2        rw    1
LSGT3.texture        rh    1
LSGT3.pad2           rh    1
LSGT3.sizeof         rw    0

; assembler LSGT4 data structure

   rsreset

LSGT4.type           rh    1
LSGT4.flag           rh    1
LSGT4.coords         rh    4
LSGT4.normals        rh    4
LSGT4.pad1           rh    1
LSGT4.colour0        rw    1
LSGT4.colour1        rw    1
LSGT4.colour2        rw    1
LSGT4.colour3        rw    1
LSGT4.texture        rh    1
LSGT4.pad2           rh    1
LSGT4.sizeof         rw    0

; assembler flag equates

SingleSided          equ   $0001
ShipEngine           equ   $0002
Translucent          equ   $0004

; assembler track data structure

   rsreset

Track.vertexCount    rw    1
Track.faceCount      rw    1
Track.sectionCount   rw    1
Track.vertices       rw    1
Track.faces          rw    1
Track.sections       rw    1
Track.sizeof         rw    0

; assembler section data structure

   rsreset

Section.junction        rw    1
Section.prevSection     rw    1
Section.nextSection     rw    1
Section.centre          rw    3
Section.version         rh    1
Section.pad1            rh    1
Section.objects         rw    1
Section.objectCount     rh    1
Section.pad2            rh    1
Section.northView       rw    3
Section.southView       rw    3
Section.eastView        rw    3
Section.westView        rw    3
Section.northViewCount  rh    3
Section.southViewCount  rh    3
Section.eastViewCount   rh    3
Section.westViewCount   rh    3
Section.high            rh    4
Section.med             rh    4
Section.faceStart       rh    1
Section.faceCount       rh    1
Section.globalRadius    rh    1
Section.localRadius     rh    1
Section.flags           rh    1
Section.secNo           rh    1
Section.sizeof          rw    0

; assembler face data structure

   rsreset

Face.vertex0         rh    1
Face.vertex1         rh    1
Face.vertex2         rh    1
Face.vertex3         rh    1
Face.normal          rh    3
Face.texture         rb    1
Face.flags           rb    1
Face.colour          rw    1
Face.sizeof          rw    0

FlipTexture          equ   $4

; assembler skeleton data structure

   rsreset

Skeleton.relative    rw    0
Skeleton.relative.m  rw    5
Skeleton.relative.t  rw    3

Skeleton.absolute    rw    0
Skeleton.absolute.m  rw    5
Skeleton.absolute.t  rw    3

Skeleton.update      rw    1
Skeleton.super       rw    1
Skeleton.sub         rw    1
Skeleton.next        rw    1
Skeleton.sizeof      rw    0

; assembler object data structure

   rsreset

Object.name             rb    16
Object.vertexCount      rh    1
Object.pad1             rh    1
Object.vertices         rw    1
Object.normalCount      rh    1
Object.pad2             rh    1
Object.normals          rw    1
Object.primitiveCount   rh    1
Object.pad3             rh    1
Object.primitives       rw    1
Object.libraryObject    rw    1
Object.bspTree          rw    1
Object.skeleton         rw    1
Object.extent           rw    1
Object.flags            rh    1
Object.pad4             rh    1
Object.next             rw    1
Object.sizeof           rw    0

; assembler LibraryObject data structure

   rsreset

LibObj.index            rh    1
LibObj.primitiveCount   rh    1
LibObj.primitives       rw    1
LibObj.sizeof           rw    0

; assembler LibraryPrimitive data structures

   rsreset

LibF3.colour            rw    1
LibF3.sizeof            rw    0

   rsreset

LibFT3.colour           rw    1
LibFT3.sizeof           rw    0

   rsreset

LibF4.colour            rw    1
LibF4.sizeof            rw    0

   rsreset

LibFT4.colour           rw    1
LibFT4.sizeof           rw    0

   rsreset

LibG3.colour0           rw    1
LibG3.colour1           rw    1
LibG3.colour2           rw    1
LibG3.sizeof            rw    0

   rsreset

LibGT3.colour0          rw    1
LibGT3.colour1          rw    1
LibGT3.colour2          rw    1
LibGT3.sizeof           rw    0

   rsreset

LibG4.colour0           rw    1
LibG4.colour1           rw    1
LibG4.colour2           rw    1
LibG4.colour3           rw    1
LibG4.sizeof            rw    0

   rsreset

LibGT4.colour0          rw    1
LibGT4.colour1          rw    1
LibGT4.colour2          rw    1
LibGT4.colour3          rw    1
LibGT4.sizeof           rw    0





MaxObjects     equ      40
MaxUnique      equ      128







