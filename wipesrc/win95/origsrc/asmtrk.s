
; (C) Psygnosis 1994. By Jason Carl Denton



   opt   c+,at-


   section code

   xref  track
   
   xref  SetSkeletonTransform
   
   xref  CurrentScreen

   xref  TrackLo
   xref  TrackLoCount
   
   xref  TrackMed
   xref  TrackMedCount
   
   xref  TrackHi
   xref  TrackHiCount
   
   xref  TrackSurface
   
   xref  OT

   xref  prims

   xdef  AsmTrackLo
   xdef  AsmTrackMed
   xdef  AsmTrackHi

   include  asm.i







shift equ   $f




; a0 : TrackSection*    firstSection
; a1 : Skeleton*        camera
; a2 : short            loCount


AsmTrackLo:

   module

; Link space on stack and save registers

   addiu sp,sp,-$28
   sw    ra,$0(sp)
   sw    fp,$4(sp)
   sw    s0,$8(sp)
   sw    s1,$c(sp)
   sw    s2,$10(sp)
   sw    s3,$14(sp)
   sw    s4,$18(sp)
   sw    s5,$1c(sp)
   sw    s6,$20(sp)
   sw    s7,$24(sp)

; Copy the function parameters

   move  s0,a0
   move  s1,a1
   move  s2,a2

; Register fp also points to base of fastram

   lui   fp,$1f80       ; svector     $20 bytes

; Pointer to the OT

   lh    v0,CurrentScreen

   lui   v1,OT>>16
   ori   v1,OT&$ffff

   sll   v0,v0,$2
   add   v0,v0,v1

   lw    s3,0(v0)

; Set the RotTrans matrix

   addiu sp,sp,-$4

   lw    a0,Skeleton.super(s1)
   jal   SetSkeletonTransform
   nop

   addiu sp,sp,$4

; Pointer to the track data

   lw    s7,track

; Copy campos into registers

   lw    t0,Skeleton.relative.t+$0(s1)
   lw    t1,Skeleton.relative.t+$4(s1)
   lw    t2,Skeleton.relative.t+$8(s1)

; Get vertex and face pointers from track data

   lw    t3,Track.vertices(s7)
   lw    t4,Track.faces(s7)

; Flags mask ( CrtClipped | ZLarge | ZNegative | ZSmall )

   lui   t8,ClipFlags>>16

; Get TrackLo pointer

   lh    v0,CurrentScreen

   lui   v1,TrackLo>>16
   ori   v1,TrackLo&$ffff

   sll   v0,v0,$2
   add   v0,v0,v1

   lw    s4,$0(v0)

; Get TrackLoCount and increment TrackLo pointer

   lhu   s5,TrackLoCount

   addi  v0,zero,PolyFT4.sizeof
   multu v0,s5
   mflo  v0
   add   s4,s4,v0

; Get TrackSurface pointer

   lui   s6,TrackSurface>>16
   ori   s6,TrackSurface&$ffff

; Section loop ( loCount )

@SectionLoop:

   lhu   t5,Section.faceCount(s0)

   lhu   v0,Section.faceStart(s0)
   addiu v1,zero,Face.sizeof
   multu v0,v1
   mflo  v0
   addu  t6,t4,v0

; Face loop ( faceCount )

@FaceLoop:

   ori   a3,zero,$ffff

; Vertex 3

   lh    v0,Face.vertex3(t6)
   nop
   sll   v0,v0,$4
   addu  v0,v0,t3

   lw    a0,$0(v0)
   lw    a1,$4(v0)
   lw    a2,$8(v0)

   add   a0,a0,t0
   add   a1,a1,t1
   add   a2,a2,t2

; Test for overflow of 16 bit world

   sra   v0,a0,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

   sra   v0,a1,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

   sra   v0,a2,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

; Combine 16 bit coords into 32 bit registers

   and   a0,a0,a3
   sll   a1,a1,$10
   or    a0,a0,a1

   mtc2  a0,zero
   mtc2  a2,at

; RotTransPers

   nop
   cop2  $180001              ; RotTransPers
   cfc2  v0,r31               ; flags
   swc2  t6,PolyFT4.xy3(s4)   ; screen 3

   and   v0,v0,t8
   bne   v0,zero,@NextFace

;  nop

; Vertex 0

   lh    v0,Face.vertex0(t6)
   nop
   sll   v0,v0,$4
   addu  v0,v0,t3

   lw    a0,$0(v0)
   lw    a1,$4(v0)
   lw    a2,$8(v0)

   add   a0,a0,t0
   add   a1,a1,t1
   add   a2,a2,t2

; Test for overflow of 16 bit world

   sra   v0,a0,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

   sra   v0,a1,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

   sra   v0,a2,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

; Combine 16 bit coords into 32 bit registers

   and   a0,a0,a3
   sll   a1,a1,$10
   or    a0,a0,a1

   mtc2  a0,zero
   mtc2  a2,at

; Vertex 1

   lh    v0,Face.vertex1(t6)
   nop
   sll   v0,v0,$4
   addu  v0,v0,t3

   lw    a0,$0(v0)
   lw    a1,$4(v0)
   lw    a2,$8(v0)

   add   a0,a0,t0
   add   a1,a1,t1
   add   a2,a2,t2

; Test for overflow of 16 bit world

   sra   v0,a0,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

   sra   v0,a1,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

   sra   v0,a2,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

; Combine 16 bit coords into 32 bit registers

   and   a0,a0,a3
   sll   a1,a1,$10
   or    a0,a0,a1

   mtc2  a0,v0
   mtc2  a2,v1

; Vertex 2

   lh    v0,Face.vertex2(t6)
   nop
   sll   v0,v0,$4
   addu  v0,v0,t3

   lw    a0,$0(v0)
   lw    a1,$4(v0)
   lw    a2,$8(v0)

   add   a0,a0,t0
   add   a1,a1,t1
   add   a2,a2,t2

; Test for overflow of 16 bit world

   sra   v0,a0,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

   sra   v0,a1,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

   sra   v0,a2,shift
   bgtz  v0,@NextFace
   nop
   addi  v0,v0,$1
   bltz  v0,@NextFace
   nop

; Combine 16 bit coords into 32 bit registers

   and   a0,a0,a3
   sll   a1,a1,$10
   or    a0,a0,a1

   mtc2  a0,a0
   mtc2  a2,a1

; RotTransPers3

   nop
   cop2  $280030              ; RotTransPers3
   cfc2  v0,r31               ; flags
   swc2  t4,PolyFT4.xy1(s4)   ; screen 0
   swc2  t5,PolyFT4.xy0(s4)   ; screen 1
   swc2  t6,PolyFT4.xy2(s4)   ; screen 2

   and   v0,v0,t8
   bne   v0,zero,@NextFace

;  nop

   mfc2  t9,r19               ; depth ; branch delay slot!

; Get tile data

   lbu   v0,Face.texture(t6)

   addi  v1,zero,Surface.sizeof
   sll   v1,v1,$1

   multu v0,v1
   mflo  v0

   lbu   v1,Face.flags(t6) ; moved back for load delay

   add   t7,s6,v0

; Flip texture?

;  lbu   v1,Face.flags(t6) ; moved back for load delay
;  nop

   andi  v1,v1,FlipTexture
   beq   v1,zero,@NoFlip

;  nop
   lw    v0,Tile.tsb(t7)   ; moved back into branch delay slot

   addi  t7,t7,Surface.sizeof

@NoFlip:

;  lw    v0,Tile.tsb(t7)   ; moved back into branch delay slot

   lhu   a0,Tile.u0(t7)
   lhu   a1,Tile.u1(t7)
   lhu   a2,Tile.u2(t7)
   lhu   a3,Tile.u3(t7)

   sh    v0,PolyFT4.tsb(s4)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s4)

   sh    a0,PolyFT4.uv0(s4)
   sh    a1,PolyFT4.uv1(s4)
   sh    a2,PolyFT4.uv2(s4)
   sh    a3,PolyFT4.uv3(s4)

; Set colour values, but don't trash code

   lw    v0,Face.colour(t6)
   nop
   sb    v0,PolyFT4.colour+$0(s4)
   srl   v0,v0,$8
   sb    v0,PolyFT4.colour+$1(s4)
   srl   v0,v0,$8
   sb    v0,PolyFT4.colour+$2(s4)

; AddPrim

   andi  t9,t9,$fffc

   add   a0,s3,t9
   move  a1,s4

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s4,s4,PolyFT4.sizeof
   addi  s5,s5,$1

; Next face

@NextFace:

   addi  t5,t5,-$1
   bgtz  t5,@FaceLoop
   addi  t6,t6,Face.sizeof    ; branch delay slot!

; Next section

@NextSection:

   addi  s2,s2,-$1
   bgtz  s2,@SectionLoop  
   addi  s0,s0,Section.sizeof ; branch delay slot!

; Save TrackLoCount

   opt   at+

   sh    s5,TrackLoCount

   opt   at-

; Restore registers and unlink stack space

AsmTrackLoEnd:

   lw    ra,$0(sp)
   lw    fp,$4(sp)
   lw    s0,$8(sp)
   lw    s1,$c(sp)
   lw    s2,$10(sp)
   lw    s3,$14(sp)
   lw    s4,$18(sp)
   lw    s5,$1c(sp)
   lw    s6,$20(sp)
   lw    s7,$24(sp)

   jr    ra
   addiu sp,sp,$28

   modend

; AsmTrackLo End























; AsmTrackMed

; PARAMETERS

; a0 : TrackSection*    firstSection
; a1 : Skeleton*        camera
; a2 : short            medCount


; REGISTER ASSIGNMENT

; s0 : section ptr
; s1 : medCount
; s2 : OT ptr
; s3 : trackMed ptr
; s4 : trackMedCount
; s5 : vertex ptr
; s6 : face ptr
; s7 : surface ptr

; t0 : campos->vx
; t1 : campos->vy
; t2 : campos->vz
; t3 : surface/tile ptr
; t4 :
; t5 :
; t6 :
; t7 :
; t8 : faceCount
; t9 : face ptr

; fp : fastram ptr
; gp :
; at :
; k0 :
; k1 :


AsmTrackMed:

   module

VTL   set   $00
VT    set   $10
VTR   set   $20
VL    set   $30
VC    set   $40
VR    set   $50
VBL   set   $60
VB    set   $70
VBR   set   $80

; Link space on stack and save registers

   addiu sp,sp,-$28
   sw    ra,$0(sp)
   sw    fp,$4(sp)
   sw    s0,$8(sp)
   sw    s1,$c(sp)
   sw    s2,$10(sp)
   sw    s3,$14(sp)
   sw    s4,$18(sp)
   sw    s5,$1c(sp)
   sw    s6,$20(sp)
   sw    s7,$24(sp)

; Copy the function parameters

   move  s0,a0
   move  s1,a2
   move  s2,a1


; Set the RotTrans matrix

   addiu sp,sp,-$4

   lw    a0,Skeleton.super(s2)
   jal   SetSkeletonTransform
   nop

   addiu sp,sp,$4

; Copy campos into registers

   lw    t0,Skeleton.relative.t+$0(s2)
   lw    t1,Skeleton.relative.t+$4(s2)
   lw    t2,Skeleton.relative.t+$8(s2)

; Don't need camera (s2) any more!


; Pointer to the current OT

   lh    v0,CurrentScreen

   lui   v1,OT>>16
   ori   v1,OT&$ffff

   sll   v0,v0,$2
   
   add   v1,v0,v1
   lw    s2,0(v1)

; Get TrackMed pointer
; v0 still CurrentScreen!

   lui   v1,TrackMed>>16
   ori   v1,TrackMed&$ffff

   add   v1,v0,v1
   lw    s3,$0(v1)

; Get TrackMedCount 

   lhu   s4,TrackMedCount

; Increment TrackMed pointer by TrackMedCount

   addi  v0,zero,PolyFT4.sizeof
   multu v0,s4
   mflo  v0
   add   s3,s3,v0


; Get vertex and face pointers from track data

   lw    v0,track
   nop
   lw    s5,Track.vertices(v0)
   lw    s6,Track.faces(v0)


; Get TrackSurface pointer

   lui   s7,TrackSurface>>16
   ori   s7,TrackSurface&$ffff


; Register fp points to base of fastram

   lui   fp,$1f80       ; svector     $20 bytes


; Section loop ( medCount s1 )

@SectionLoop:

   lhu   t8,Section.faceCount(s0)

   lhu   v0,Section.faceStart(s0)
   addiu v1,zero,Face.sizeof
   multu v0,v1
   mflo  v0
   addu  t9,s6,v0

; Face loop ( faceCount t8 )

@FaceLoop:

   lh    t4,Face.vertex0(t9)  ; get vertex indices
   lh    t5,Face.vertex1(t9)
   lh    t6,Face.vertex2(t9)
   lh    t7,Face.vertex3(t9)

   sll   t4,t4,$4    ; multiply by 16 ( sizeof VECTOR )
   sll   t5,t5,$4
   sll   t6,t6,$4
   sll   t7,t7,$4

   add   t4,t4,s5
   add   t5,t5,s5
   add   t6,t6,s5
   add   t7,t7,s5

; Mesh x

   lw    a0,VECTOR.vx(t4)     ; load x values
   lw    a1,VECTOR.vx(t5)
   lw    a2,VECTOR.vx(t6)
   lw    a3,VECTOR.vx(t7)

   add   a0,a0,t0             ; add campos->vx
   add   a1,a1,t0
   add   a2,a2,t0
   add   a3,a3,t0

   sh    a0,VTR+Mesh.vx(fp)   ; store top right x
   sh    a1,VTL+Mesh.vx(fp)   ; store top left x
   sh    a2,VBL+Mesh.vx(fp)   ; store bottom left x
   sh    a3,VBR+Mesh.vx(fp)   ; store bottom right x

   add   v0,a0,a1             ; mesh
   sra   v0,v0,$1
   sh    v0,VT+Mesh.vx(fp)    ; store top x

   add   v0,a2,a3             ; mesh
   sra   v0,v0,$1
   sh    v0,VB+Mesh.vx(fp)    ; store bottom x

   add   v0,a1,a2             ; mesh
   sra   v0,v0,$1
   sh    v0,VL+Mesh.vx(fp)    ; store left x

   add   v1,a0,a3             ; mesh
   sra   v1,v1,$1
   sh    v1,VR+Mesh.vx(fp)    ; store right x

   add   v0,v0,v1             ; mesh
   sra   v0,v0,$1
   sh    v0,VC+Mesh.vx(fp)    ; store centre x

; Mesh y

   lw    a0,VECTOR.vy(t4)     ; load y values
   lw    a1,VECTOR.vy(t5)
   lw    a2,VECTOR.vy(t6)
   lw    a3,VECTOR.vy(t7)

   add   a0,a0,t1             ; add campos->vy
   add   a1,a1,t1
   add   a2,a2,t1
   add   a3,a3,t1

   sh    a0,VTR+Mesh.vy(fp)   ; store top right y
   sh    a1,VTL+Mesh.vy(fp)   ; store top left y
   sh    a2,VBL+Mesh.vy(fp)   ; store bottom left y
   sh    a3,VBR+Mesh.vy(fp)   ; store bottom right y
                                                   
   add   v0,a0,a1             ; mesh
   sra   v0,v0,$1
   sh    v0,VT+Mesh.vy(fp)    ; store top y

   add   v0,a2,a3             ; mesh
   sra   v0,v0,$1
   sh    v0,VB+Mesh.vy(fp)    ; store bottom y

   add   v0,a1,a2             ; mesh
   sra   v0,v0,$1
   sh    v0,VL+Mesh.vy(fp)    ; store left y

   add   v1,a0,a3             ; mesh
   sra   v1,v1,$1
   sh    v1,VR+Mesh.vy(fp)    ; store right y

   add   v0,v0,v1             ; mesh
   sra   v0,v0,$1
   sh    v0,VC+Mesh.vy(fp)    ; store centre y

; Mesh z

   lw    a0,VECTOR.vz(t4)     ; load z values
   lw    a1,VECTOR.vz(t5)
   lw    a2,VECTOR.vz(t6)
   lw    a3,VECTOR.vz(t7)

   add   a0,a0,t2             ; add campos->vz
   add   a1,a1,t2
   add   a2,a2,t2
   add   a3,a3,t2

   sh    a0,VTR+Mesh.vz(fp)   ; store top right z
   sh    a1,VTL+Mesh.vz(fp)   ; store top left z
   sh    a2,VBL+Mesh.vz(fp)   ; store bottom left z
   sh    a3,VBR+Mesh.vz(fp)   ; store bottom right z

   add   v0,a0,a1             ; mesh
   sra   v0,v0,$1
   sh    v0,VT+Mesh.vz(fp)    ; store top z

   add   v0,a2,a3             ; mesh
   sra   v0,v0,$1
   sh    v0,VB+Mesh.vz(fp)    ; store bottom z

   add   v0,a1,a2             ; mesh
   sra   v0,v0,$1
   sh    v0,VL+Mesh.vz(fp)    ; store left z

   add   v1,a0,a3             ; mesh
   sra   v1,v1,$1
   sh    v1,VR+Mesh.vz(fp)    ; store right z

   add   v0,v0,v1             ; mesh
   sra   v0,v0,$1
   sh    v0,VC+Mesh.vz(fp)    ; store centre z


; RotTransPersN

   move  a0,fp                ; first vertex
   addi  a1,zero,$9           ; 9 vertices

   addi  a2,zero,$1           ; all bad flag
   addi  a3,zero,$0           ; all good flag

@RotTransPersN:

   lwc2  zero,Mesh.vx(a0)     ; get vx and vy
   lwc2  at,Mesh.vz(a0)       ; get vz
   addi  a1,a1,-$1            ; load delay slot!
   cop2  $180001              ; RotTransPers
   
   cfc2  v0,r31               ; get flags
   mfc2  v1,r19               ; get depth
   
   swc2  t6,Mesh.screen(a0)   ; store screen coords
   
   srl   v0,v0,$c             ; short flags
   andi  v0,v0,ShortClipFlags ; clip flags
   sh    v0,Mesh.flags(a0)    ; store flags

   sh    v1,Mesh.depth(a0)    ; store depth

   sltu  v1,zero,v0           ; good/bad flag
   and   a2,a2,v1             ; accumulate all bad flag
   or    a3,a3,v1             ; accumulate all good flag

   bgtz  a1,@RotTransPersN
   addi  a0,a0,Mesh.sizeof    ; branch delay slot!

; All flags bad ?

   bne   a2,zero,@NextFace    ; dont bother if all flags bad
   nop

; Get tile data

   lbu   v0,Face.texture(t9)

   addi  v1,zero,Surface.sizeof
   sll   v1,v1,$1

   multu v0,v1
   mflo  v0

   add   t3,s7,v0

; Flip texture?

   lbu   v1,Face.flags(t9) ; move back for load delay?
   nop

   andi  v1,v1,FlipTexture
   beq   v1,zero,@NoFlip
   addi  t3,t3,Surface.med ; branch delay slot!

   addi  t3,t3,Surface.sizeof

@NoFlip:

; Test flags on individual tile basis ?

   bne   a3,zero,@Clip        ; test flags if not all good
;  nop



@NoClip:

; TOP LEFT QUAD

; Store screen coords in quad

   lw    a0,VTL+Mesh.screen(fp)
   lw    a1,VT+Mesh.screen(fp)
   lw    a2,VL+Mesh.screen(fp)
   lw    a3,VC+Mesh.screen(fp)

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(t3)

   lhu   a0,Tile.u0(t3)
   lhu   a1,Tile.u1(t3)
   lhu   a2,Tile.u2(t3)
   lhu   a3,Tile.u3(t3)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Store colour values, but don't trash code field

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

; AverageZ4

   lhu   a0,VTL+Mesh.depth(fp)
   lhu   a1,VT+Mesh.depth(fp)
   lhu   a2,VL+Mesh.depth(fp)
   lhu   a3,VC+Mesh.depth(fp)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof
   addi  s4,s4,$1

; Increment tile pointer

   addi  t3,t3,Tile.sizeof



; TOP RIGHT QUAD

; Store screen coords in quad

   lw    a0,VT+Mesh.screen(fp)
   lw    a1,VTR+Mesh.screen(fp)
   lw    a2,VC+Mesh.screen(fp)
   lw    a3,VR+Mesh.screen(fp)

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(t3)

   lhu   a0,Tile.u0(t3)
   lhu   a1,Tile.u1(t3)
   lhu   a2,Tile.u2(t3)
   lhu   a3,Tile.u3(t3)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Store colour values, but don't trash code field

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

; AverageZ4

   lhu   a0,VT+Mesh.depth(fp)
   lhu   a1,VTR+Mesh.depth(fp)
   lhu   a2,VC+Mesh.depth(fp)
   lhu   a3,VR+Mesh.depth(fp)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof
   addi  s4,s4,$1

; Increment tile pointer

   addi  t3,t3,Tile.sizeof



; BOTTOM LEFT QUAD

; Store screen coords in quad

   lw    a0,VL+Mesh.screen(fp)
   lw    a1,VC+Mesh.screen(fp)
   lw    a2,VBL+Mesh.screen(fp)
   lw    a3,VB+Mesh.screen(fp)

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(t3)

   lhu   a0,Tile.u0(t3)
   lhu   a1,Tile.u1(t3)
   lhu   a2,Tile.u2(t3)
   lhu   a3,Tile.u3(t3)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Store colour values, but don't trash code field

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

; AverageZ4

   lhu   a0,VL+Mesh.depth(fp)
   lhu   a1,VC+Mesh.depth(fp)
   lhu   a2,VBL+Mesh.depth(fp)
   lhu   a3,VB+Mesh.depth(fp)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof
   addi  s4,s4,$1

; Increment tile pointer

   addi  t3,t3,Tile.sizeof



; BOTTOM RIGHT QUAD

; Store screen coords in quad

   lw    a0,VC+Mesh.screen(fp)
   lw    a1,VR+Mesh.screen(fp)
   lw    a2,VB+Mesh.screen(fp)
   lw    a3,VBR+Mesh.screen(fp)

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(t3)

   lhu   a0,Tile.u0(t3)
   lhu   a1,Tile.u1(t3)
   lhu   a2,Tile.u2(t3)
   lhu   a3,Tile.u3(t3)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Store colour values, but don't trash code field

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

; AverageZ4

   lhu   a0,VC+Mesh.depth(fp)
   lhu   a1,VR+Mesh.depth(fp)
   lhu   a2,VB+Mesh.depth(fp)
   lhu   a3,VBR+Mesh.depth(fp)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof
   addi  s4,s4,$1

; Increment tile pointer

   j     @NextFace

   addi  t3,t3,Tile.sizeof



@Clip:

; TOP LEFT QUAD

   lhu   a0,VTL+Mesh.flags(fp)
   lhu   a1,VT+Mesh.flags(fp)
   lhu   a2,VL+Mesh.flags(fp)
   lhu   a3,VC+Mesh.flags(fp)

   or    a0,a0,a1
   or    a0,a0,a2
   or    a0,a0,a3
   bne   a0,zero,@EndQuad1

; Store screen coords in quad

   lw    a0,VTL+Mesh.screen(fp)
   lw    a1,VT+Mesh.screen(fp)
   lw    a2,VL+Mesh.screen(fp)
   lw    a3,VC+Mesh.screen(fp)

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(t3)

   lhu   a0,Tile.u0(t3)
   lhu   a1,Tile.u1(t3)
   lhu   a2,Tile.u2(t3)
   lhu   a3,Tile.u3(t3)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Store colour values, but don't trash code field

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

; AverageZ4

   lhu   a0,VTL+Mesh.depth(fp)
   lhu   a1,VT+Mesh.depth(fp)
   lhu   a2,VL+Mesh.depth(fp)
   lhu   a3,VC+Mesh.depth(fp)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof
   addi  s4,s4,$1

@EndQuad1:

; Increment tile pointer

   addi  t3,t3,Tile.sizeof



; TOP RIGHT QUAD

   lhu   a0,VT+Mesh.flags(fp)
   lhu   a1,VTR+Mesh.flags(fp)
   lhu   a2,VC+Mesh.flags(fp)
   lhu   a3,VR+Mesh.flags(fp)

   or    a0,a0,a1
   or    a0,a0,a2
   or    a0,a0,a3
   bne   a0,zero,@EndQuad2

; Store screen coords in quad

   lw    a0,VT+Mesh.screen(fp)
   lw    a1,VTR+Mesh.screen(fp)
   lw    a2,VC+Mesh.screen(fp)
   lw    a3,VR+Mesh.screen(fp)

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(t3)

   lhu   a0,Tile.u0(t3)
   lhu   a1,Tile.u1(t3)
   lhu   a2,Tile.u2(t3)
   lhu   a3,Tile.u3(t3)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Store colour values, but don't trash code field

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

; AverageZ4

   lhu   a0,VT+Mesh.depth(fp)
   lhu   a1,VTR+Mesh.depth(fp)
   lhu   a2,VC+Mesh.depth(fp)
   lhu   a3,VR+Mesh.depth(fp)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof
   addi  s4,s4,$1

@EndQuad2:

; Increment tile pointer

   addi  t3,t3,Tile.sizeof



; BOTTOM LEFT QUAD

   lhu   a0,VL+Mesh.flags(fp)
   lhu   a1,VC+Mesh.flags(fp)
   lhu   a2,VBL+Mesh.flags(fp)
   lhu   a3,VB+Mesh.flags(fp)

   or    a0,a0,a1
   or    a0,a0,a2
   or    a0,a0,a3
   bne   a0,zero,@EndQuad3

; Store screen coords in quad

   lw    a0,VL+Mesh.screen(fp)
   lw    a1,VC+Mesh.screen(fp)
   lw    a2,VBL+Mesh.screen(fp)
   lw    a3,VB+Mesh.screen(fp)

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(t3)

   lhu   a0,Tile.u0(t3)
   lhu   a1,Tile.u1(t3)
   lhu   a2,Tile.u2(t3)
   lhu   a3,Tile.u3(t3)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Store colour values, but don't trash code field

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

; AverageZ4

   lhu   a0,VL+Mesh.depth(fp)
   lhu   a1,VC+Mesh.depth(fp)
   lhu   a2,VBL+Mesh.depth(fp)
   lhu   a3,VB+Mesh.depth(fp)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof
   addi  s4,s4,$1

@EndQuad3:

; Increment tile pointer

   addi  t3,t3,Tile.sizeof



; BOTTOM RIGHT QUAD

   lhu   a0,VC+Mesh.flags(fp)
   lhu   a1,VR+Mesh.flags(fp)
   lhu   a2,VB+Mesh.flags(fp)
   lhu   a3,VBR+Mesh.flags(fp)

   or    a0,a0,a1
   or    a0,a0,a2
   or    a0,a0,a3
   bne   a0,zero,@EndQuad4

; Store screen coords in quad

   lw    a0,VC+Mesh.screen(fp)
   lw    a1,VR+Mesh.screen(fp)
   lw    a2,VB+Mesh.screen(fp)
   lw    a3,VBR+Mesh.screen(fp)

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(t3)

   lhu   a0,Tile.u0(t3)
   lhu   a1,Tile.u1(t3)
   lhu   a2,Tile.u2(t3)
   lhu   a3,Tile.u3(t3)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Store colour values, but don't trash code field

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

; AverageZ4

   lhu   a0,VC+Mesh.depth(fp)
   lhu   a1,VR+Mesh.depth(fp)
   lhu   a2,VB+Mesh.depth(fp)
   lhu   a3,VBR+Mesh.depth(fp)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof
   addi  s4,s4,$1

@EndQuad4:

; Increment tile pointer

   addi  t3,t3,Tile.sizeof





; Next face ( faceCount t8 )

@NextFace:

   addi  t8,t8,-$1
   bgtz  t8,@FaceLoop
   addi  t9,t9,Face.sizeof    ; branch delay slot!



; Next section ( medCount s1 )

@NextSection:

   addi  s1,s1,-$1
   bgtz  s1,@SectionLoop
   addi  s0,s0,Section.sizeof ; branch delay slot!



; Save TrackMedCount ( needs at register! )

   opt   at+

   sh    s4,TrackMedCount

   opt   at-


; Restore registers and unlink stack space

AsmTrackMedEnd:

   lw    ra,$0(sp)
   lw    fp,$4(sp)
   lw    s0,$8(sp)
   lw    s1,$c(sp)
   lw    s2,$10(sp)
   lw    s3,$14(sp)
   lw    s4,$18(sp)
   lw    s5,$1c(sp)
   lw    s6,$20(sp)
   lw    s7,$24(sp)

   jr    ra
   addiu sp,sp,$28

   modend

; AsmTrackMed End






























; AsmTrackHi

; PARAMETERS

; a0 : TrackSection*    firstSection
; a1 : Skeleton*        camera
; a2 : AutoMeshData*    autoMesh
; a3 : short            hiCount


; REGISTER ASSIGNMENT

; s0 : section ptr
; s1 : hiCount
; s2 : OT ptr
; s3 : trackHi ptr
; s4 : trackHiCount
; s5 : vertex ptr
; s6 : face ptr
; s7 : surface ptr

; t0 : 
; t1 : 
; t2 : 
; t3 : 
; t4 :
; t5 :
; t6 :
; t7 :
; t8 : faceCount
; t9 : face ptr

; fp : fastram ptr
; gp : tile ptr
; at : 
; k0 :
; k1 :


AsmTrackHi:

   module

CAMX  set   $000
CAMY  set   $004
CAMZ  set   $008

VTL   set   $010
VTML  set   $020
VT    set   $030
VTMR  set   $040
VTR   set   $050
VLMT  set   $060
VCTL  set   $070
VCT   set   $080
VCTR  set   $090
VRMT  set   $0a0
VL    set   $0b0
VCL   set   $0c0
VC    set   $0d0
VCR   set   $0e0
VR    set   $0f0
VLMB  set   $100
VCBL  set   $110
VCB   set   $120
VCBR  set   $130
VRMB  set   $140
VBL   set   $150
VBML  set   $160
VB    set   $170
VBMR  set   $180
VBR   set   $190

; automesh data

MT    set   $200
ML    set   $210
MC    set   $220
MR    set   $230
MB    set   $240

UVT   set   $250
UVL   set   $254
UVC   set   $258
UVR   set   $25c
UVB   set   $260






; Link space on stack and save registers

   addiu sp,sp,-$2c
   sw    ra,$0(sp)
   sw    fp,$4(sp)
   sw    gp,$8(sp)
   sw    s0,$c(sp)
   sw    s1,$10(sp)
   sw    s2,$14(sp)
   sw    s3,$18(sp)
   sw    s4,$1c(sp)
   sw    s5,$20(sp)
   sw    s6,$24(sp)
   sw    s7,$28(sp)

; Copy the function parameters

   move  s0,a0
   move  s1,a3
   move  s2,a1

; Set the RotTrans matrix

   addiu sp,sp,-$4

   lw    a0,Skeleton.super(s2)
   jal   SetSkeletonTransform
   nop

   addiu sp,sp,$4

; Register fp points to base of fastram

   lui   fp,$1f80


; Copy campos into fastram

   addiu v0,s2,Skeleton.relative.t

   lw    t0,$0(v0)
   lw    t1,$4(v0)
   lw    t2,$8(v0)

   sw    t0,CAMX(fp)
   sw    t1,CAMY(fp)
   sw    t2,CAMZ(fp)

; Don't need camera (s2) any more!


; Pointer to the current OT

   lh    v0,CurrentScreen

   lui   v1,OT>>16
   ori   v1,OT&$ffff

   sll   v0,v0,$2
   
   add   v1,v0,v1
   lw    s2,0(v1)

; Get TrackHi pointer
; v0 still CurrentScreen!

   lui   v1,TrackHi>>16
   ori   v1,TrackHi&$ffff

   add   v1,v0,v1
   lw    s3,$0(v1)

; Get TrackHiCount 

   lhu   s4,TrackHiCount

; Increment TrackHi pointer by TrackHiCount

   addi  v0,zero,PolyFT4.sizeof
   multu v0,s4
   mflo  v0
   add   s3,s3,v0


; Get vertex and face pointers from track data

   lw    v0,track
   nop
   lw    s5,Track.vertices(v0)
   lw    s6,Track.faces(v0)


; Get TrackSurface pointer

   lui   s7,TrackSurface>>16
   ori   s7,TrackSurface&$ffff


; Section loop ( hiCount s1 )

@SectionLoop:

   lhu   t8,Section.faceCount(s0)

   lhu   v0,Section.faceStart(s0)
   addiu v1,zero,Face.sizeof
   multu v0,v1
   mflo  v0
   addu  t9,s6,v0

; Face loop ( faceCount t8 )

@FaceLoop:

   lh    t0,Face.vertex0(t9)  ; get vertex indices
   lh    t1,Face.vertex1(t9)
   lh    t2,Face.vertex2(t9)
   lh    t3,Face.vertex3(t9)

   sll   t0,t0,$4    ; multiply by 16 ( sizeof VECTOR )
   sll   t1,t1,$4
   sll   t2,t2,$4
   sll   t3,t3,$4

   add   t0,t0,s5
   add   t1,t1,s5
   add   t2,t2,s5
   add   t3,t3,s5



; **** Mesh x ****

   lw    v0,CAMX(fp)

   lw    a0,VECTOR.vx(t0)     ; load x values
   lw    a1,VECTOR.vx(t1)
   lw    a2,VECTOR.vx(t2)
   lw    a3,VECTOR.vx(t3)

   add   a0,a0,v0             ; add campos->vx
   add   a1,a1,v0
   add   a2,a2,v0
   add   a3,a3,v0

   sh    a0,VTR+Mesh.vx(fp)   ; store VTR
   sh    a1,VTL+Mesh.vx(fp)   ; store VTL
   sh    a2,VBL+Mesh.vx(fp)   ; store VBL
   sh    a3,VBR+Mesh.vx(fp)   ; store VBR

   add   t4,a0,a1             ; mesh VT
   sra   t4,t4,$1
   sh    t4,VT+Mesh.vx(fp)

   add   t5,a2,a3             ; mesh VB
   sra   t5,t5,$1
   sh    t5,VB+Mesh.vx(fp)

   add   t6,a1,a2             ; mesh VL
   sra   t6,t6,$1
   sh    t6,VL+Mesh.vx(fp)

   add   t7,a0,a3             ; mesh VR
   sra   t7,t7,$1
   sh    t7,VR+Mesh.vx(fp)

   add   at,t6,t7             ; mesh VC
   sra   at,at,$1
   sh    at,VC+Mesh.vx(fp)


; top left sub mesh

   add   v0,a1,t4             ; mesh VTML
   sra   v0,v0,$1
   sh    v0,VTML+Mesh.vx(fp)

   add   v0,a1,t6             ; mesh VLMT
   sra   v0,v0,$1
   sh    v0,VLMT+Mesh.vx(fp)

   add   v1,at,t4             ; mesh VCT
   sra   v1,v1,$1
   sh    v1,VCT+Mesh.vx(fp)

   add   v0,v0,v1             ; mesh VCTL
   sra   v0,v0,$1
   sh    v0,VCTL+Mesh.vx(fp)


; top right sub mesh

   add   v0,a0,t7             ; mesh VRMT
   sra   v0,v0,$1
   sh    v0,VRMT+Mesh.vx(fp)

   add   v0,a0,t4             ; mesh VTMR
   sra   v0,v0,$1
   sh    v0,VTMR+Mesh.vx(fp)

   add   v1,at,t7             ; mesh VCR
   sra   v1,v1,$1
   sh    v1,VCR+Mesh.vx(fp)

   add   v0,v0,v1             ; mesh VCTR
   sra   v0,v0,$1
   sh    v0,VCTR+Mesh.vx(fp)


; bottom left sub mesh

   add   v0,a2,t6             ; mesh VLMB
   sra   v0,v0,$1
   sh    v0,VLMB+Mesh.vx(fp)

   add   v0,a2,t5             ; mesh VBML
   sra   v0,v0,$1
   sh    v0,VBML+Mesh.vx(fp)

   add   v1,at,t6             ; mesh VCL
   sra   v1,v1,$1
   sh    v1,VCL+Mesh.vx(fp)

   add   v0,v0,v1             ; mesh VCBL
   sra   v0,v0,$1
   sh    v0,VCBL+Mesh.vx(fp)


; bottom right sub mesh

   add   v0,a3,t5             ; mesh VBMR
   sra   v0,v0,$1
   sh    v0,VBMR+Mesh.vx(fp)

   add   v0,a3,t7             ; mesh VRMB
   sra   v0,v0,$1
   sh    v0,VRMB+Mesh.vx(fp)

   add   v1,at,t5             ; mesh VCB
   sra   v1,v1,$1
   sh    v1,VCB+Mesh.vx(fp)

   add   v0,v0,v1             ; mesh VCBR
   sra   v0,v0,$1
   sh    v0,VCBR+Mesh.vx(fp)


; **** Mesh y ****

   lw    v0,CAMY(fp)

   lw    a0,VECTOR.vy(t0)     ; load y values
   lw    a1,VECTOR.vy(t1)
   lw    a2,VECTOR.vy(t2)
   lw    a3,VECTOR.vy(t3)

   add   a0,a0,v0             ; add campos->vy
   add   a1,a1,v0
   add   a2,a2,v0
   add   a3,a3,v0

   sh    a0,VTR+Mesh.vy(fp)   ; store VTR
   sh    a1,VTL+Mesh.vy(fp)   ; store VTL
   sh    a2,VBL+Mesh.vy(fp)   ; store VBL
   sh    a3,VBR+Mesh.vy(fp)   ; store VBR
                                      
   add   t4,a0,a1             ; mesh VT
   sra   t4,t4,$1
   sh    t4,VT+Mesh.vy(fp)    

   add   t5,a2,a3             ; mesh VB
   sra   t5,t5,$1
   sh    t5,VB+Mesh.vy(fp)    

   add   t6,a1,a2             ; mesh VL
   sra   t6,t6,$1
   sh    t6,VL+Mesh.vy(fp)    

   add   t7,a0,a3             ; mesh VR
   sra   t7,t7,$1
   sh    t7,VR+Mesh.vy(fp)    

   add   at,t6,t7             ; mesh VC
   sra   at,at,$1
   sh    at,VC+Mesh.vy(fp)    


; top left sub mesh

   add   v0,a1,t4             ; mesh VTML
   sra   v0,v0,$1
   sh    v0,VTML+Mesh.vy(fp)

   add   v0,a1,t6             ; mesh VLMT
   sra   v0,v0,$1
   sh    v0,VLMT+Mesh.vy(fp)

   add   v1,at,t4             ; mesh VCT
   sra   v1,v1,$1
   sh    v1,VCT+Mesh.vy(fp)

   add   v0,v0,v1             ; mesh VCTL
   sra   v0,v0,$1
   sh    v0,VCTL+Mesh.vy(fp)


; top right sub mesh

   add   v0,a0,t7             ; mesh VRMT
   sra   v0,v0,$1
   sh    v0,VRMT+Mesh.vy(fp)

   add   v0,a0,t4             ; mesh VTMR
   sra   v0,v0,$1
   sh    v0,VTMR+Mesh.vy(fp)

   add   v1,at,t7             ; mesh VCR
   sra   v1,v1,$1
   sh    v1,VCR+Mesh.vy(fp)

   add   v0,v0,v1             ; mesh VCTR
   sra   v0,v0,$1
   sh    v0,VCTR+Mesh.vy(fp)


; bottom left sub mesh

   add   v0,a2,t6             ; mesh VLMB
   sra   v0,v0,$1
   sh    v0,VLMB+Mesh.vy(fp)

   add   v0,a2,t5             ; mesh VBML
   sra   v0,v0,$1
   sh    v0,VBML+Mesh.vy(fp)

   add   v1,at,t6             ; mesh VCL
   sra   v1,v1,$1
   sh    v1,VCL+Mesh.vy(fp)

   add   v0,v0,v1             ; mesh VCBL
   sra   v0,v0,$1
   sh    v0,VCBL+Mesh.vy(fp)


; bottom right sub mesh

   add   v0,a3,t5             ; mesh VBMR
   sra   v0,v0,$1
   sh    v0,VBMR+Mesh.vy(fp)

   add   v0,a3,t7             ; mesh VRMB
   sra   v0,v0,$1
   sh    v0,VRMB+Mesh.vy(fp)

   add   v1,at,t5             ; mesh VCB
   sra   v1,v1,$1
   sh    v1,VCB+Mesh.vy(fp)

   add   v0,v0,v1             ; mesh VCBR
   sra   v0,v0,$1
   sh    v0,VCBR+Mesh.vy(fp)


; **** Mesh z ****

   lw    v0,CAMZ(fp)

   lw    a0,VECTOR.vz(t0)     ; load z values
   lw    a1,VECTOR.vz(t1)
   lw    a2,VECTOR.vz(t2)
   lw    a3,VECTOR.vz(t3)

   add   a0,a0,v0             ; add campos->vz
   add   a1,a1,v0
   add   a2,a2,v0
   add   a3,a3,v0

   sh    a0,VTR+Mesh.vz(fp)   ; store VTR
   sh    a1,VTL+Mesh.vz(fp)   ; store VTL
   sh    a2,VBL+Mesh.vz(fp)   ; store VBL
   sh    a3,VBR+Mesh.vz(fp)   ; store VBR

   add   t4,a0,a1             ; mesh VT
   sra   t4,t4,$1
   sh    t4,VT+Mesh.vz(fp)    

   add   t5,a2,a3             ; mesh VB
   sra   t5,t5,$1
   sh    t5,VB+Mesh.vz(fp)    

   add   t6,a1,a2             ; mesh VL
   sra   t6,t6,$1
   sh    t6,VL+Mesh.vz(fp)    

   add   t7,a0,a3             ; mesh VR
   sra   t7,t7,$1
   sh    t7,VR+Mesh.vz(fp)    

   add   at,t6,t7             ; mesh VC
   sra   at,at,$1
   sh    at,VC+Mesh.vz(fp)    


; top left sub mesh

   add   v0,a1,t4             ; mesh VTML
   sra   v0,v0,$1
   sh    v0,VTML+Mesh.vz(fp)

   add   v0,a1,t6             ; mesh VLMT
   sra   v0,v0,$1
   sh    v0,VLMT+Mesh.vz(fp)

   add   v1,at,t4             ; mesh VCT
   sra   v1,v1,$1
   sh    v1,VCT+Mesh.vz(fp)

   add   v0,v0,v1             ; mesh VCTL
   sra   v0,v0,$1
   sh    v0,VCTL+Mesh.vz(fp)


; top right sub mesh

   add   v0,a0,t7             ; mesh VRMT
   sra   v0,v0,$1
   sh    v0,VRMT+Mesh.vz(fp)

   add   v0,a0,t4             ; mesh VTMR
   sra   v0,v0,$1
   sh    v0,VTMR+Mesh.vz(fp)

   add   v1,at,t7             ; mesh VCR
   sra   v1,v1,$1
   sh    v1,VCR+Mesh.vz(fp)

   add   v0,v0,v1             ; mesh VCTR
   sra   v0,v0,$1
   sh    v0,VCTR+Mesh.vz(fp)


; bottom left sub mesh

   add   v0,a2,t6             ; mesh VLMB
   sra   v0,v0,$1
   sh    v0,VLMB+Mesh.vz(fp)

   add   v0,a2,t5             ; mesh VBML
   sra   v0,v0,$1
   sh    v0,VBML+Mesh.vz(fp)

   add   v1,at,t6             ; mesh VCL
   sra   v1,v1,$1
   sh    v1,VCL+Mesh.vz(fp)

   add   v0,v0,v1             ; mesh VCBL
   sra   v0,v0,$1
   sh    v0,VCBL+Mesh.vz(fp)


; bottom right sub mesh

   add   v0,a3,t5             ; mesh VBMR
   sra   v0,v0,$1
   sh    v0,VBMR+Mesh.vz(fp)

   add   v0,a3,t7             ; mesh VRMB
   sra   v0,v0,$1
   sh    v0,VRMB+Mesh.vz(fp)

   add   v1,at,t5             ; mesh VCB
   sra   v1,v1,$1
   sh    v1,VCB+Mesh.vz(fp)

   add   v0,v0,v1             ; mesh VCBR
   sra   v0,v0,$1
   sh    v0,VCBR+Mesh.vz(fp)



; RotTransPersN

   addi  a0,fp,VTL            ; first vertex
   addi  a1,zero,$19          ; 25 vertices

   addi  a2,zero,$1           ; all bad flag
   addi  a3,zero,$0           ; all good flag

@RotTransPersN:

   lwc2  zero,Mesh.vx(a0)     ; get vx and vy
   lwc2  at,Mesh.vz(a0)       ; get vz
   addi  a1,a1,-$1            ; load delay slot!
   cop2  $180001              ; RotTransPers
   
   cfc2  v0,r31               ; get flags
   mfc2  v1,r19               ; get depth
   
   swc2  t6,Mesh.screen(a0)   ; store screen coords
   
   srl   v0,v0,$c             ; short flags
   andi  v0,v0,ShortClipFlags ; clip flags
   sh    v0,Mesh.flags(a0)    ; store flags

   sh    v1,Mesh.depth(a0)    ; store depth
   
   sltu  v1,zero,v0           ; good/bad flag
   and   a2,a2,v1             ; accumulate all bad flag
   or    a3,a3,v1             ; accumulate all good flag
   
   bgtz  a1,@RotTransPersN
   addi  a0,a0,Mesh.sizeof    ; branch delay slot!

; All flags bad ?

   bne   a2,zero,@NextFace    ; dont bother if all flags bad
   nop

; Get tile data

   lbu   v0,Face.texture(t9)

   addi  v1,zero,Surface.sizeof
   sll   v1,v1,$1

   multu v0,v1
   mflo  v0

   add   gp,s7,v0

; Flip texture?

   lbu   v1,Face.flags(t9)    ; move back for load delay?
   nop

   andi  v1,v1,FlipTexture
   beq   v1,zero,@NoFlip
   addi  gp,gp,Surface.high   ; branch delay slot!

   addi  gp,gp,Surface.sizeof

@NoFlip:

; Test flags on individual tile basis ?

   bne   a3,zero,@Clip        ; test flags if not all good
;  nop


; All flags are good so don't bother checking them

@NoClip:

; QUAD 1

   lw    a0,VTL+Mesh.screen(fp)
   lw    a1,VTML+Mesh.screen(fp)
   lw    a2,VLMT+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCTL+Mesh.screen(fp)

   lhu   a0,VTL+Mesh.depth(fp)
   lhu   a1,VTML+Mesh.depth(fp)
   lhu   a2,VLMT+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCTL+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 2

   lw    a0,VTML+Mesh.screen(fp)
   lw    a1,VT+Mesh.screen(fp)
   lw    a2,VCTL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCT+Mesh.screen(fp)

   lhu   a0,VTML+Mesh.depth(fp)
   lhu   a1,VT+Mesh.depth(fp)
   lhu   a2,VCTL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCT+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 3

   lw    a0,VT+Mesh.screen(fp)
   lw    a1,VTMR+Mesh.screen(fp)
   lw    a2,VCT+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCTR+Mesh.screen(fp)

   lhu   a0,VT+Mesh.depth(fp)
   lhu   a1,VTMR+Mesh.depth(fp)
   lhu   a2,VCT+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCTR+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 4

   lw    a0,VTMR+Mesh.screen(fp)
   lw    a1,VTR+Mesh.screen(fp)
   lw    a2,VCTR+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VRMT+Mesh.screen(fp)

   lhu   a0,VTMR+Mesh.depth(fp)
   lhu   a1,VTR+Mesh.depth(fp)
   lhu   a2,VCTR+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VRMT+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 5

   lw    a0,VLMT+Mesh.screen(fp)
   lw    a1,VCTL+Mesh.screen(fp)
   lw    a2,VL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCL+Mesh.screen(fp)

   lhu   a0,VLMT+Mesh.depth(fp)
   lhu   a1,VCTL+Mesh.depth(fp)
   lhu   a2,VL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCL+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 6

   lw    a0,VCTL+Mesh.screen(fp)
   lw    a1,VCT+Mesh.screen(fp)
   lw    a2,VCL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VC+Mesh.screen(fp)

   lhu   a0,VCTL+Mesh.depth(fp)
   lhu   a1,VCT+Mesh.depth(fp)
   lhu   a2,VCL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VC+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 7

   lw    a0,VCT+Mesh.screen(fp)
   lw    a1,VCTR+Mesh.screen(fp)
   lw    a2,VC+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCR+Mesh.screen(fp)

   lhu   a0,VCT+Mesh.depth(fp)
   lhu   a1,VCTR+Mesh.depth(fp)
   lhu   a2,VC+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCR+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 8

   lw    a0,VCTR+Mesh.screen(fp)
   lw    a1,VRMT+Mesh.screen(fp)
   lw    a2,VCR+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VR+Mesh.screen(fp)

   lhu   a0,VCTR+Mesh.depth(fp)
   lhu   a1,VRMT+Mesh.depth(fp)
   lhu   a2,VCR+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VR+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 9

   lw    a0,VL+Mesh.screen(fp)
   lw    a1,VCL+Mesh.screen(fp)
   lw    a2,VLMB+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCBL+Mesh.screen(fp)

   lhu   a0,VL+Mesh.depth(fp)
   lhu   a1,VCL+Mesh.depth(fp)
   lhu   a2,VLMB+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCBL+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 10

   lw    a0,VCL+Mesh.screen(fp)
   lw    a1,VC+Mesh.screen(fp)
   lw    a2,VCBL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCB+Mesh.screen(fp)

   lhu   a0,VCL+Mesh.depth(fp)
   lhu   a1,VC+Mesh.depth(fp)
   lhu   a2,VCBL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCB+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 11

   lw    a0,VC+Mesh.screen(fp)
   lw    a1,VCR+Mesh.screen(fp)
   lw    a2,VCB+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCBR+Mesh.screen(fp)

   lhu   a0,VC+Mesh.depth(fp)
   lhu   a1,VCR+Mesh.depth(fp)
   lhu   a2,VCB+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCBR+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 12

   lw    a0,VCR+Mesh.screen(fp)
   lw    a1,VR+Mesh.screen(fp)
   lw    a2,VCBR+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VRMB+Mesh.screen(fp)

   lhu   a0,VCR+Mesh.depth(fp)
   lhu   a1,VR+Mesh.depth(fp)
   lhu   a2,VCBR+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VRMB+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 13

   lw    a0,VLMB+Mesh.screen(fp)
   lw    a1,VCBL+Mesh.screen(fp)
   lw    a2,VBL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VBML+Mesh.screen(fp)

   lhu   a0,VLMB+Mesh.depth(fp)
   lhu   a1,VCBL+Mesh.depth(fp)
   lhu   a2,VBL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VBML+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 14

   lw    a0,VCBL+Mesh.screen(fp)
   lw    a1,VCB+Mesh.screen(fp)
   lw    a2,VBML+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VB+Mesh.screen(fp)

   lhu   a0,VCBL+Mesh.depth(fp)
   lhu   a1,VCB+Mesh.depth(fp)
   lhu   a2,VBML+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VB+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 15

   lw    a0,VCB+Mesh.screen(fp)
   lw    a1,VCBR+Mesh.screen(fp)
   lw    a2,VB+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VBMR+Mesh.screen(fp)

   lhu   a0,VCB+Mesh.depth(fp)
   lhu   a1,VCBR+Mesh.depth(fp)
   lhu   a2,VB+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VBMR+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; QUAD 16

   lw    a0,VCBR+Mesh.screen(fp)
   lw    a1,VRMB+Mesh.screen(fp)
   lw    a2,VBMR+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VBR+Mesh.screen(fp)

   lhu   a0,VCBR+Mesh.depth(fp)
   lhu   a1,VRMB+Mesh.depth(fp)
   lhu   a2,VBMR+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VBR+Mesh.depth(fp)

   addi  gp,gp,Tile.sizeof

; Next face ( faceCount t8 )

@NextFace:

   addi  t8,t8,-$1
   bgtz  t8,@FaceLoop
   addi  t9,t9,Face.sizeof    ; branch delay slot!


; Next section ( hiCount s1 )

@NextSection:


   addi  s1,s1,-$1
   bgtz  s1,@SectionLoop
   addi  s0,s0,Section.sizeof ; branch delay slot!


; Save TrackHiCount ( needs at register! )

   opt   at+

   sh    s4,TrackHiCount

   opt   at-


; Restore registers and unlink stack space

AsmTrackHiEnd:

   lw    ra,$0(sp)
   lw    fp,$4(sp)
   lw    gp,$8(sp)
   lw    s0,$c(sp)
   lw    s1,$10(sp)
   lw    s2,$14(sp)
   lw    s3,$18(sp)
   lw    s4,$1c(sp)
   lw    s5,$20(sp)
   lw    s6,$24(sp)
   lw    s7,$28(sp)

   jr    ra
   addiu sp,sp,$2c

; End of AsmTrackHi main routine





; Some flags are bad, so need to check each tile

@Clip:

@Quad1:

   lh    a0,VTL+Mesh.flags(fp)
   lh    a1,VTML+Mesh.flags(fp)
   lh    a2,VLMT+Mesh.flags(fp)
   lh    a3,VCTL+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad1
   nop

   lw    a0,VTL+Mesh.screen(fp)
   lw    a1,VTML+Mesh.screen(fp)
   lw    a2,VLMT+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCTL+Mesh.screen(fp)

   lhu   a0,VTL+Mesh.depth(fp)
   lhu   a1,VTML+Mesh.depth(fp)
   lhu   a2,VLMT+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCTL+Mesh.depth(fp)

@EndQuad1:

   addi  gp,gp,Tile.sizeof


@Quad2:

   lh    a0,VTML+Mesh.flags(fp)
   lh    a1,VT+Mesh.flags(fp)
   lh    a2,VCTL+Mesh.flags(fp)
   lh    a3,VCT+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad2
   nop

   lw    a0,VTML+Mesh.screen(fp)
   lw    a1,VT+Mesh.screen(fp)
   lw    a2,VCTL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCT+Mesh.screen(fp)

   lhu   a0,VTML+Mesh.depth(fp)
   lhu   a1,VT+Mesh.depth(fp)
   lhu   a2,VCTL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCT+Mesh.depth(fp)

@EndQuad2:

   addi  gp,gp,Tile.sizeof


@Quad3:

   lh    a0,VT+Mesh.flags(fp)
   lh    a1,VTMR+Mesh.flags(fp)
   lh    a2,VCT+Mesh.flags(fp)
   lh    a3,VCTR+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad3
   nop

   lw    a0,VT+Mesh.screen(fp)
   lw    a1,VTMR+Mesh.screen(fp)
   lw    a2,VCT+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCTR+Mesh.screen(fp)

   lhu   a0,VT+Mesh.depth(fp)
   lhu   a1,VTMR+Mesh.depth(fp)
   lhu   a2,VCT+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCTR+Mesh.depth(fp)

@EndQuad3:

   addi  gp,gp,Tile.sizeof


@Quad4:

   lh    a0,VTMR+Mesh.flags(fp)
   lh    a1,VTR+Mesh.flags(fp)
   lh    a2,VCTR+Mesh.flags(fp)
   lh    a3,VRMT+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad4
   nop

   lw    a0,VTMR+Mesh.screen(fp)
   lw    a1,VTR+Mesh.screen(fp)
   lw    a2,VCTR+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VRMT+Mesh.screen(fp)

   lhu   a0,VTMR+Mesh.depth(fp)
   lhu   a1,VTR+Mesh.depth(fp)
   lhu   a2,VCTR+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VRMT+Mesh.depth(fp)

@EndQuad4:

   addi  gp,gp,Tile.sizeof


@Quad5:

   lh    a0,VLMT+Mesh.flags(fp)
   lh    a1,VCTL+Mesh.flags(fp)
   lh    a2,VL+Mesh.flags(fp)
   lh    a3,VCL+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad5
   nop

   lw    a0,VLMT+Mesh.screen(fp)
   lw    a1,VCTL+Mesh.screen(fp)
   lw    a2,VL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCL+Mesh.screen(fp)

   lhu   a0,VLMT+Mesh.depth(fp)
   lhu   a1,VCTL+Mesh.depth(fp)
   lhu   a2,VL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCL+Mesh.depth(fp)

@EndQuad5:

   addi  gp,gp,Tile.sizeof


@Quad6:

   lh    a0,VCTL+Mesh.flags(fp)
   lh    a1,VCT+Mesh.flags(fp)
   lh    a2,VCL+Mesh.flags(fp)
   lh    a3,VC+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad6
   nop

   lw    a0,VCTL+Mesh.screen(fp)
   lw    a1,VCT+Mesh.screen(fp)
   lw    a2,VCL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VC+Mesh.screen(fp)

   lhu   a0,VCTL+Mesh.depth(fp)
   lhu   a1,VCT+Mesh.depth(fp)
   lhu   a2,VCL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VC+Mesh.depth(fp)

@EndQuad6:

   addi  gp,gp,Tile.sizeof


@Quad7:

   lh    a0,VCT+Mesh.flags(fp)
   lh    a1,VCTR+Mesh.flags(fp)
   lh    a2,VC+Mesh.flags(fp)
   lh    a3,VCR+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad7
   nop

   lw    a0,VCT+Mesh.screen(fp)
   lw    a1,VCTR+Mesh.screen(fp)
   lw    a2,VC+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCR+Mesh.screen(fp)

   lhu   a0,VCT+Mesh.depth(fp)
   lhu   a1,VCTR+Mesh.depth(fp)
   lhu   a2,VC+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCR+Mesh.depth(fp)

@EndQuad7:

   addi  gp,gp,Tile.sizeof


@Quad8:

   lh    a0,VCTR+Mesh.flags(fp)
   lh    a1,VRMT+Mesh.flags(fp)
   lh    a2,VCR+Mesh.flags(fp)
   lh    a3,VR+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad8
   nop

   lw    a0,VCTR+Mesh.screen(fp)
   lw    a1,VRMT+Mesh.screen(fp)
   lw    a2,VCR+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VR+Mesh.screen(fp)

   lhu   a0,VCTR+Mesh.depth(fp)
   lhu   a1,VRMT+Mesh.depth(fp)
   lhu   a2,VCR+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VR+Mesh.depth(fp)

@EndQuad8:

   addi  gp,gp,Tile.sizeof


@Quad9:

   lh    a0,VL+Mesh.flags(fp)
   lh    a1,VCL+Mesh.flags(fp)
   lh    a2,VLMB+Mesh.flags(fp)
   lh    a3,VCBL+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad9
   nop

   lw    a0,VL+Mesh.screen(fp)
   lw    a1,VCL+Mesh.screen(fp)
   lw    a2,VLMB+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCBL+Mesh.screen(fp)

   lhu   a0,VL+Mesh.depth(fp)
   lhu   a1,VCL+Mesh.depth(fp)
   lhu   a2,VLMB+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCBL+Mesh.depth(fp)

@EndQuad9:

   addi  gp,gp,Tile.sizeof


@Quad10:

   lh    a0,VCL+Mesh.flags(fp)
   lh    a1,VC+Mesh.flags(fp)
   lh    a2,VCBL+Mesh.flags(fp)
   lh    a3,VCB+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad10
   nop

   lw    a0,VCL+Mesh.screen(fp)
   lw    a1,VC+Mesh.screen(fp)
   lw    a2,VCBL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCB+Mesh.screen(fp)

   lhu   a0,VCL+Mesh.depth(fp)
   lhu   a1,VC+Mesh.depth(fp)
   lhu   a2,VCBL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCB+Mesh.depth(fp)

@EndQuad10:

   addi  gp,gp,Tile.sizeof


@Quad11:

   lh    a0,VC+Mesh.flags(fp)
   lh    a1,VCR+Mesh.flags(fp)
   lh    a2,VCB+Mesh.flags(fp)
   lh    a3,VCBR+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad11
   nop

   lw    a0,VC+Mesh.screen(fp)
   lw    a1,VCR+Mesh.screen(fp)
   lw    a2,VCB+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VCBR+Mesh.screen(fp)

   lhu   a0,VC+Mesh.depth(fp)
   lhu   a1,VCR+Mesh.depth(fp)
   lhu   a2,VCB+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VCBR+Mesh.depth(fp)

@EndQuad11:

   addi  gp,gp,Tile.sizeof


@Quad12:

   lh    a0,VCR+Mesh.flags(fp)
   lh    a1,VR+Mesh.flags(fp)
   lh    a2,VCBR+Mesh.flags(fp)
   lh    a3,VRMB+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad12
   nop

   lw    a0,VCR+Mesh.screen(fp)
   lw    a1,VR+Mesh.screen(fp)
   lw    a2,VCBR+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VRMB+Mesh.screen(fp)

   lhu   a0,VCR+Mesh.depth(fp)
   lhu   a1,VR+Mesh.depth(fp)
   lhu   a2,VCBR+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VRMB+Mesh.depth(fp)

@EndQuad12:

   addi  gp,gp,Tile.sizeof


@Quad13:

   lh    a0,VLMB+Mesh.flags(fp)
   lh    a1,VCBL+Mesh.flags(fp)
   lh    a2,VBL+Mesh.flags(fp)
   lh    a3,VBML+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad13
   nop

   lw    a0,VLMB+Mesh.screen(fp)
   lw    a1,VCBL+Mesh.screen(fp)
   lw    a2,VBL+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VBML+Mesh.screen(fp)

   lhu   a0,VLMB+Mesh.depth(fp)
   lhu   a1,VCBL+Mesh.depth(fp)
   lhu   a2,VBL+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VBML+Mesh.depth(fp)

@EndQuad13:

   addi  gp,gp,Tile.sizeof


@Quad14:

   lh    a0,VCBL+Mesh.flags(fp)
   lh    a1,VCB+Mesh.flags(fp)
   lh    a2,VBML+Mesh.flags(fp)
   lh    a3,VB+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad14
   nop

   lw    a0,VCBL+Mesh.screen(fp)
   lw    a1,VCB+Mesh.screen(fp)
   lw    a2,VBML+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VB+Mesh.screen(fp)

   lhu   a0,VCBL+Mesh.depth(fp)
   lhu   a1,VCB+Mesh.depth(fp)
   lhu   a2,VBML+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VB+Mesh.depth(fp)

@EndQuad14:

   addi  gp,gp,Tile.sizeof


@Quad15:

   lh    a0,VCB+Mesh.flags(fp)
   lh    a1,VCBR+Mesh.flags(fp)
   lh    a2,VB+Mesh.flags(fp)
   lh    a3,VBMR+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad15
   nop

   lw    a0,VCB+Mesh.screen(fp)
   lw    a1,VCBR+Mesh.screen(fp)
   lw    a2,VB+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VBMR+Mesh.screen(fp)

   lhu   a0,VCB+Mesh.depth(fp)
   lhu   a1,VCBR+Mesh.depth(fp)
   lhu   a2,VB+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VBMR+Mesh.depth(fp)

@EndQuad15:

   addi  gp,gp,Tile.sizeof


@Quad16:

   lh    a0,VCBR+Mesh.flags(fp)
   lh    a1,VRMB+Mesh.flags(fp)
   lh    a2,VBMR+Mesh.flags(fp)
   lh    a3,VBR+Mesh.flags(fp)

   or    v0,a0,a1
   or    v0,v0,a2
   or    v0,v0,a3
   bne   v0,zero,@MeshQuad16
   nop

   lw    a0,VCBR+Mesh.screen(fp)
   lw    a1,VRMB+Mesh.screen(fp)
   lw    a2,VBMR+Mesh.screen(fp)
   jal   @BuildTile
   lw    a3,VBR+Mesh.screen(fp)

   lhu   a0,VCBR+Mesh.depth(fp)
   lhu   a1,VRMB+Mesh.depth(fp)
   lhu   a2,VBMR+Mesh.depth(fp)
   jal   @AddTile
   lhu   a3,VBR+Mesh.depth(fp)

@EndQuad16:

   addi  gp,gp,Tile.sizeof

@ClipEnd:

   j     @NextFace
   nop






; Bad quad so test flags for automesh

@MeshQuad1:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad1
   nop

   addi  t0,fp,VTL
   addi  t1,fp,VTML
   addi  t2,fp,VLMT
   addi  t3,fp,VCTL

   jal   @Automesh
   nop

   j     @EndQuad1
   nop


@MeshQuad2:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad2
   nop

   addi  t0,fp,VTML
   addi  t1,fp,VT
   addi  t2,fp,VCTL
   addi  t3,fp,VCT

   jal   @Automesh
   nop

   j     @EndQuad2
   nop


@MeshQuad3:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad3
   nop

   addi  t0,fp,VT
   addi  t1,fp,VTMR
   addi  t2,fp,VCT
   addi  t3,fp,VCTR

   jal   @Automesh
   nop

   j     @EndQuad3
   nop


@MeshQuad4:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad4
   nop

   addi  t0,fp,VTMR
   addi  t1,fp,VTR
   addi  t2,fp,VCTR
   addi  t3,fp,VRMT

   jal   @Automesh
   nop

   j     @EndQuad4
   nop


@MeshQuad5:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad5
   nop

   addi  t0,fp,VLMT
   addi  t1,fp,VCTL
   addi  t2,fp,VL
   addi  t3,fp,VCL

   jal   @Automesh
   nop

   j     @EndQuad5
   nop


@MeshQuad6:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad6
   nop

   addi  t0,fp,VCTL
   addi  t1,fp,VCT
   addi  t2,fp,VCL
   addi  t3,fp,VC

   jal   @Automesh
   nop

   j     @EndQuad6
   nop


@MeshQuad7:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad7
   nop

   addi  t0,fp,VCT
   addi  t1,fp,VCTR
   addi  t2,fp,VC
   addi  t3,fp,VCR

   jal   @Automesh
   nop

   j     @EndQuad7
   nop


@MeshQuad8:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad8
   nop

   addi  t0,fp,VCTR
   addi  t1,fp,VRMT
   addi  t2,fp,VCR
   addi  t3,fp,VR

   jal   @Automesh
   nop

   j     @EndQuad8
   nop


@MeshQuad9:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad9
   nop

   addi  t0,fp,VL
   addi  t1,fp,VCL
   addi  t2,fp,VLMB
   addi  t3,fp,VCBL

   jal   @Automesh
   nop

   j     @EndQuad9
   nop


@MeshQuad10:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad10
   nop

   addi  t0,fp,VCL
   addi  t1,fp,VC
   addi  t2,fp,VCBL
   addi  t3,fp,VCB

   jal   @Automesh
   nop

   j     @EndQuad10
   nop


@MeshQuad11:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad11
   nop

   addi  t0,fp,VC
   addi  t1,fp,VCR
   addi  t2,fp,VCB
   addi  t3,fp,VCBR

   jal   @Automesh
   nop

   j     @EndQuad11
   nop


@MeshQuad12:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad12
   nop

   addi  t0,fp,VCR
   addi  t1,fp,VR
   addi  t2,fp,VCBR
   addi  t3,fp,VRMB

   jal   @Automesh
   nop

   j     @EndQuad12
   nop


@MeshQuad13:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad13
   nop

   addi  t0,fp,VLMB
   addi  t1,fp,VCBL
   addi  t2,fp,VBL
   addi  t3,fp,VBML

   jal   @Automesh
   nop

   j     @EndQuad13
   nop


@MeshQuad14:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad14
   nop

   addi  t0,fp,VCBL
   addi  t1,fp,VCB
   addi  t2,fp,VBML
   addi  t3,fp,VB

   jal   @Automesh
   nop

   j     @EndQuad14
   nop


@MeshQuad15:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad15
   nop

   addi  t0,fp,VCB
   addi  t1,fp,VCBR
   addi  t2,fp,VB
   addi  t3,fp,VBMR

   jal   @Automesh
   nop

   j     @EndQuad15
   nop


@MeshQuad16:

   sltu  v0,zero,a0
   sltu  v1,zero,a1
   and   v0,v0,v1
   sltu  v1,zero,a2
   and   v0,v0,v1
   sltu  v1,zero,a3
   and   v0,v0,v1

   bne   v0,zero,@EndQuad16
   nop

   addi  t0,fp,VCBR
   addi  t1,fp,VRMB
   addi  t2,fp,VBMR
   addi  t3,fp,VBR

   jal   @Automesh
   nop

   j     @EndQuad16
   nop





; Subroutine to fill in the primitives and add them to the OT

; Subroutine is faster than inline code! 
; Must be making good use of instruction cache

@BuildTile:

   sw    a0,PolyFT4.xy0(s3)
   sw    a1,PolyFT4.xy1(s3)
   sw    a2,PolyFT4.xy2(s3)
   sw    a3,PolyFT4.xy3(s3)

; Store texture info in quad

   lw    v0,Tile.tsb(gp)

   lhu   a0,Tile.u0(gp)
   lhu   a1,Tile.u1(gp)
   lhu   a2,Tile.u2(gp)
   lhu   a3,Tile.u3(gp)

   sh    v0,PolyFT4.tsb(s3)
   srl   v0,v0,$10
   sh    v0,PolyFT4.cba(s3)

   sh    a0,PolyFT4.uv0(s3)
   sh    a1,PolyFT4.uv1(s3)
   sh    a2,PolyFT4.uv2(s3)
   sh    a3,PolyFT4.uv3(s3)

; Stores colour values, but doesn't trash code field
; lwl/swl with offset of $2 loads/stores only 3 bytes!

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(s3)

   jr    ra
   nop





; Subroutine to add the primitive to the OT

@AddTile:

; AverageZ4

   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   sll   v0,v0,$2

   add   a0,s2,v0
   move  a1,s3

   lui   a2,$00ff
   ori   a2,$ffff

   lui   a3,$ff00

   lw    v0,0(a0)
   lw    v1,0(a1)

   and   v0,v0,a2
   and   v1,v1,a3
   or    v0,v0,v1
   
   sw    v0,0(a1)

   and   a1,a1,a2
   
   sw    a1,0(a0)

; Increment ft4 pointer and counter

   addi  s3,s3,PolyFT4.sizeof

   jr    ra
   addi  s4,s4,$1








; Automesh a quad

; t0 : Top left
; t1 : Top right
; t2 : Bottom left
; t3 : Bottom right


@Automesh:

   module

   move  t7,ra    ; save return address

; mesh u

   lbu   a0,Tile.u0(gp)    ; tl
   lbu   a1,Tile.u1(gp)    ; tr
   lbu   a2,Tile.u2(gp)    ; bl
   lbu   a3,Tile.u3(gp)    ; br

   add   v0,a0,a1          ; top
   sra   v0,v0,$1
   sb    v0,UVT+TEX.u(fp)

   add   v0,a2,a3          ; bottom
   sra   v0,v0,$1
   sb    v0,UVB+TEX.u(fp)

   add   v0,a0,a2          ; left
   sra   v0,v0,$1
   sb    v0,UVL+TEX.u(fp)

   add   v1,a1,a3          ; right
   sra   v1,v1,$1
   sb    v1,UVR+TEX.u(fp)

   add   v0,v0,v1          ; centre
   sra   v0,v0,$1
   sb    v0,UVC+TEX.u(fp)

; mesh v

   lbu   a0,Tile.v0(gp)    ; tl
   lbu   a1,Tile.v1(gp)    ; tr
   lbu   a2,Tile.v2(gp)    ; bl
   lbu   a3,Tile.v3(gp)    ; br

   add   v0,a0,a1          ; top
   sra   v0,v0,$1
   sb    v0,UVT+TEX.v(fp)

   add   v0,a2,a3          ; bottom
   sra   v0,v0,$1
   sb    v0,UVB+TEX.v(fp)

   add   v0,a0,a2          ; left
   sra   v0,v0,$1
   sb    v0,UVL+TEX.v(fp)

   add   v1,a1,a3          ; right
   sra   v1,v1,$1
   sb    v1,UVR+TEX.v(fp)

   add   v0,v0,v1          ; centre
   sra   v0,v0,$1
   sb    v0,UVC+TEX.v(fp)

; mesh x

   lh    a0,Mesh.vx(t0)    ; tl
   lh    a1,Mesh.vx(t1)    ; tr
   lh    a2,Mesh.vx(t2)    ; bl
   lh    a3,Mesh.vx(t3)    ; br

   add   v0,a0,a1          ; top
   sra   v0,v0,$1
   sh    v0,MT+Mesh.vx(fp)

   add   v0,a2,a3          ; bottom
   sra   v0,v0,$1
   sh    v0,MB+Mesh.vx(fp)

   add   v0,a0,a2          ; left
   sra   v0,v0,$1
   sh    v0,ML+Mesh.vx(fp)

   add   v1,a1,a3          ; right
   sra   v1,v1,$1
   sh    v1,MR+Mesh.vx(fp)

   add   v0,v0,v1          ; centre
   sra   v0,v0,$1
   sh    v0,MC+Mesh.vx(fp)

; mesh y

   lh    a0,Mesh.vy(t0)    ; tl
   lh    a1,Mesh.vy(t1)    ; tr
   lh    a2,Mesh.vy(t2)    ; bl
   lh    a3,Mesh.vy(t3)    ; br

   add   v0,a0,a1          ; top
   sra   v0,v0,$1
   sh    v0,MT+Mesh.vy(fp)

   add   v0,a2,a3          ; bottom
   sra   v0,v0,$1
   sh    v0,MB+Mesh.vy(fp)

   add   v0,a0,a2          ; left
   sra   v0,v0,$1
   sh    v0,ML+Mesh.vy(fp)

   add   v1,a1,a3          ; right
   sra   v1,v1,$1
   sh    v1,MR+Mesh.vy(fp)

   add   v0,v0,v1          ; centre
   sra   v0,v0,$1
   sh    v0,MC+Mesh.vy(fp)

; mesh z

   lh    a0,Mesh.vz(t0)    ; tl
   lh    a1,Mesh.vz(t1)    ; tr
   lh    a2,Mesh.vz(t2)    ; bl
   lh    a3,Mesh.vz(t3)    ; br

   add   v0,a0,a1          ; top
   sra   v0,v0,$1
   sh    v0,MT+Mesh.vz(fp)

   add   v0,a2,a3          ; bottom
   sra   v0,v0,$1
   sh    v0,MB+Mesh.vz(fp)

   add   v0,a0,a2          ; left
   sra   v0,v0,$1
   sh    v0,ML+Mesh.vz(fp)

   add   v1,a1,a3          ; right
   sra   v1,v1,$1
   sh    v1,MR+Mesh.vz(fp)

   add   v0,v0,v1          ; centre
   sra   v0,v0,$1
   sh    v0,MC+Mesh.vz(fp)


; RotTransPersN

   addi  a0,fp,MT             ; first vertex
   addi  a1,zero,$5           ; 5 vertices

@RotTransPersN:

   lwc2  zero,Mesh.vx(a0)     ; get vx and vy
   lwc2  at,Mesh.vz(a0)       ; get vz
   addi  a1,a1,-$1            ; load delay slot!
   cop2  $180001              ; RotTransPers
   
   cfc2  v0,r31               ; get flags
   mfc2  v1,r19               ; get depth
   
   swc2  t6,Mesh.screen(a0)   ; store screen coords
   
   srl   v0,v0,$c             ; short flags
   andi  v0,v0,ShortClipFlags ; clip flags
   sh    v0,Mesh.flags(a0)    ; store flags

   sh    v1,Mesh.depth(a0)    ; store depth
   
   bgtz  a1,@RotTransPersN
   addi  a0,a0,Mesh.sizeof    ; branch delay slot!


; Get current primitive pointer

   lw    t4,prims


; Generate top left poly

@TopLeft:

   lh    a0,Mesh.flags(t0)
   lh    a1,Mesh.flags+MT(fp)
   lh    a2,Mesh.flags+ML(fp)
   lh    a3,Mesh.flags+MC(fp)

   or    a0,a0,a1
   or    a0,a0,a2
   or    a0,a0,a3
   bne   a0,zero,@TopLeftEnd
   nop

   lw    a0,Mesh.screen(t0)
   lw    a1,Mesh.screen+MT(fp)
   lw    a2,Mesh.screen+ML(fp)
   jal   @AutomeshBuildTile
   lw    a3,Mesh.screen+MC(fp)

   lhu   a0,Tile.u0(gp)
   lhu   a1,UVT+TEX.uv(fp)
   lhu   a2,UVL+TEX.uv(fp)
   jal   @AutomeshTexture
   lhu   a3,UVC+TEX.uv(fp)

   lhu   a0,Mesh.depth(t0)
   lhu   a1,Mesh.depth+MT(fp)
   lhu   a2,Mesh.depth+ML(fp)
   jal   @AutomeshAddTile
   lhu   a3,Mesh.depth+MC(fp)

@TopLeftEnd:


; Generate top right poly

@TopRight:

   lh    a0,Mesh.flags+MT(fp)
   lh    a1,Mesh.flags(t1)
   lh    a2,Mesh.flags+MC(fp)
   lh    a3,Mesh.flags+MR(fp)

   or    a0,a0,a1
   or    a0,a0,a2
   or    a0,a0,a3
   bne   a0,zero,@TopRightEnd
   nop

   lw    a0,Mesh.screen+MT(fp)
   lw    a1,Mesh.screen(t1)
   lw    a2,Mesh.screen+MC(fp)
   jal   @AutomeshBuildTile
   lw    a3,Mesh.screen+MR(fp)

   lhu   a0,UVT+TEX.uv(fp)
   lhu   a1,Tile.u1(gp)
   lhu   a2,UVC+TEX.uv(fp)
   jal   @AutomeshTexture
   lhu   a3,UVR+TEX.uv(fp)

   lhu   a0,Mesh.depth+MT(fp)
   lhu   a1,Mesh.depth(t1)
   lhu   a2,Mesh.depth+MC(fp)
   jal   @AutomeshAddTile
   lhu   a3,Mesh.depth+MR(fp)

@TopRightEnd:


; Generate bottom left poly

@BottomLeft:

   lh    a0,Mesh.flags+ML(fp)
   lh    a1,Mesh.flags+MC(fp)
   lh    a2,Mesh.flags(t2)
   lh    a3,Mesh.flags+MB(fp)

   or    a0,a0,a1
   or    a0,a0,a2
   or    a0,a0,a3
   bne   a0,zero,@BottomLeftEnd
   nop

   lw    a0,Mesh.screen+ML(fp)
   lw    a1,Mesh.screen+MC(fp)
   lw    a2,Mesh.screen(t2)
   jal   @AutomeshBuildTile
   lw    a3,Mesh.screen+MB(fp)

   lhu   a0,UVL+TEX.uv(fp)
   lhu   a1,UVC+TEX.uv(fp)
   lhu   a2,Tile.u2(gp)
   jal   @AutomeshTexture
   lhu   a3,UVB+TEX.uv(fp)

   lhu   a0,Mesh.depth+ML(fp)
   lhu   a1,Mesh.depth+MC(fp)
   lhu   a2,Mesh.depth(t2)
   jal   @AutomeshAddTile 
   lhu   a3,Mesh.depth+MB(fp)

@BottomLeftEnd:


; Generate bottom right poly

@BottomRight:

   lh    a0,Mesh.flags+MC(fp)
   lh    a1,Mesh.flags+MR(fp)
   lh    a2,Mesh.flags+MB(fp)
   lh    a3,Mesh.flags(t3)

   or    a0,a0,a1
   or    a0,a0,a2
   or    a0,a0,a3
   bne   a0,zero,@BottomRightEnd
   nop

   lw    a0,Mesh.screen+MC(fp)
   lw    a1,Mesh.screen+MR(fp)
   lw    a2,Mesh.screen+MB(fp)
   jal   @AutomeshBuildTile
   lw    a3,Mesh.screen(t3)

   lhu   a0,UVC+TEX.uv(fp)
   lhu   a1,UVR+TEX.uv(fp)
   lhu   a2,UVB+TEX.uv(fp)
   jal   @AutomeshTexture
   lhu   a3,Tile.u3(gp)

   lhu   a0,Mesh.depth+MC(fp)
   lhu   a1,Mesh.depth+MR(fp)
   lhu   a2,Mesh.depth+MB(fp)
   jal   @AutomeshAddTile 
   lhu   a3,Mesh.depth(t3)

@BottomRightEnd:


; Store the primitive pointer back
   
   opt   at+

   sw    t4,prims

   opt   at-

@AutomeshEnd:

   move  ra,t7    ; restore return address
   jr    ra
   nop




@AutomeshBuildTile:

; SetPolyFT4

   ori   v0,zero,$9
   sb    v0,$3(t4)

   ori   v0,zero,$2c
   sb    v0,$7(t4)

; Store screen coords

   sw    a0,PolyFT4.xy0(t4)
   sw    a1,PolyFT4.xy1(t4)
   sw    a2,PolyFT4.xy2(t4)
   jr    ra
   sw    a3,PolyFT4.xy3(t4)


@AutomeshTexture:

; Store texture info in quad ( temporary! need to mesh uv's ! )

   sh    a0,PolyFT4.uv0(t4)
   sh    a1,PolyFT4.uv1(t4)
   sh    a2,PolyFT4.uv2(t4)
   sh    a3,PolyFT4.uv3(t4)

   lhu   v0,Tile.tsb(gp)
   lhu   v1,Tile.cba(gp)
   sh    v0,PolyFT4.tsb(t4)
   sh    v1,PolyFT4.cba(t4)

; Stores colour values, but doesn't trash code field
; lwl/swl with offset of $2 loads/stores only 3 bytes!

   lwl   v0,Face.colour+$2(t9)
   nop
   swl   v0,PolyFT4.colour+$2(t4)

   jr    ra
   nop


@AutomeshAddTile:

; Get depth

   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
   nop

; AddPrim

   move  a1,t4          ; primitive address
   sll   v0,v0,$2       ; depth * size of ptr
   add   a0,s2,v0       ; OT + depth

   lwl   v0,$2(a0)      ; load low 3 bytes of OT pointer high
   sll   v1,a1,$8       ; move low 3 bytes of prim ptr high

   swl   v0,$2(a1)      ; primitive points to what OT used to
   swl   v1,$2(a0)      ; OT points to primitive

; Increment primitive pointer and return

   jr    ra
   add   t4,t4,PolyFT4.sizeof

   modend

; Mesh End





   modend

; AsmTrackHi End










