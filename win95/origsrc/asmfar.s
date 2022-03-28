
; (C) Psygnosis 1994. By Jason Carl Denton


   opt   c+,at-


   section code

   xref  SetSkeletonTransform
   xref  track
   xref  CurrentScreen
   xref  OT
   xref  sharedPrims
   xref  sharedPrimCount
   xref  MeshPtr

   xref  SlowMem


   if 0


   xdef  AsmDrawLibObjFar

   include  asm.i




; AsmDrawLibObjFar Parameters

; a0 : Object*       object
; a1 : Skeleton*     camera


; AsmDrawLibObjFar Permanent Register Assignment

; s0 : vertex ptr
; s1 : lib primitive ptr
; s2 : shared prim ptr
; s3 : poly ptr
; s4 : rtpn ptr
; s5 : current screen
; s6 : OT
; s7 : primitive count

; t0 : 
; t1 :
; t2 :
; t3 :
; t4 :
; t5 :
; t6 :
; t7 :
; t8 : 
; t9 : 

; fp : AddPrim mask
; gp : AddPrim mask
; at :
; k0 :
; k1 :

; a0 :
; a1 :
; a2 :
; a3 :

; v0 :
; v1 :


; Permanent Register Assignments

vertex         equr  s0
libprim        equr  s1
shared         equr  s2
poly           equr  s3
rtpn           equr  s4
cs             equr  s5
ot             equr  s6
primitives     equr  s7


; Temporary Register Assignments

tmpObj         equr  s6
tmpCam         equr  s7      

vertices       equr  t9







AsmDrawLibObjFar: 

   module

; Link space on stack and save registers

   addiu sp,sp,-$2c
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
   sw    gp,$28(sp)

; Copy the function parameters

   move  s6,a0
   move  s7,a1

; Set the RotTrans matrix

   lw    a0,Skeleton.super(s7)
   jal   SetSkeletonTransform
   nop

; Get the camera position

   lw    t0,Skeleton.relative.t+$0(s7)
   lw    t1,Skeleton.relative.t+$4(s7)
   lw    t2,Skeleton.relative.t+$8(s7)

   lw    a0,Object.skeleton(s6)
   nop

   lw    t3,Skeleton.relative.t+$0(a0)
   lw    t4,Skeleton.relative.t+$4(a0)
   lw    t5,Skeleton.relative.t+$8(a0)

   add   t0,t0,t3
   add   t1,t1,t4
   add   t2,t2,t5

; Get vertex pointer and counter

   lw    s0,Object.vertices(s6)
   lhu   t9,Object.vertexCount(s6)

; Get primitive pointer, counter and index

   lw    v0,Object.libraryObject(s6)  
   nop

   lw    s1,LibObj.primitives(v0)     
   lhu   s7,LibObj.primitiveCount(v0)

   lhu   v0,LibObj.index(v0)
   nop

; Get shared primitive count

   lui   a0,sharedPrimCount>>16
   ori   a0,sharedPrimCount&$ffff

   sll   v1,v0,$1
   add   a0,a0,v1

   lhu   v1,$0(a0)            ; sharedPrimCount[ index ]
   nop

; Get shared primitives

   lui   a1,sharedPrims>>16
   ori   a1,sharedPrims&$ffff

   addi  a2,zero,MaxObjects
   sll   a2,a2,$2             ; MaxObjects * 4
   multu v0,a2
   mflo  a2                   ; index * MaxObjects * 4
   add   a1,a1,a2

   sll   a2,v1,$2             ; sharedPrimCount[ index ] * 4
   add   a1,a1,a2

   lw    s2,$0(a1)        ; shared primitive pointer
   nop

; Test if out of objects

   beq   s2,zero,@AsmDrawLibObjFarEnd
   nop

; Increment sharedPrimCount[ index ] and store it back

   addi  v1,v1,$1
   sh    v1,$0(a0)

; If vertex count < 128 use fast ram else use normal ram

   addi  v0,t9,-$80
   blez  v0,@FastRam

   lui   s4,$1f80          ; Branch delay slot!

@SlowRam:

   lui   s4,SlowMem>>16
   ori   s4,SlowMem&$ffff

@FastRam:

; Pointer to the OT

   lhu   s5,CurrentScreen

   lui   v0,OT>>16
   ori   v0,OT&$ffff

   sll   v1,s5,$2
   add   v0,v0,v1

   lw    s6,0(v0)

; RotTransPersN

   move  a0,s0
   move  a1,s4

   addi  a2,zero,$1           ; all bad flag           
   addi  a3,zero,$0           ; all good flag

@RotTransPersN
   
   if 1

   lh    t4,$0(a0)            ; load vx
   lh    t5,$2(a0)            ; load vy
   lh    t6,$4(a0)            ; load vz

   add   t4,t4,t0             ; add campos->vx
   add   t5,t5,t1             ; add campos->vy
   add   t6,t6,t2             ; add campos->vz

   sh    t4,$0(a1)            ; store vx
   sh    t5,$2(a1)            ; store vy
   sh    t6,$4(a1)            ; store vz

   lwc2  zero,$0(a1)          ; load vx and vy
   lwc2  at,$4(a1)            ; load vz
 
   else

   lwc2  zero,$0(a0)          ; load vx and vy
   lwc2  at,$4(a0)            ; load vz
   
   endif
   
   addi  a0,a0,SVECTOR.sizeof ; next svector
   cop2  $180001              ; RotTransPers

   mfc2  v0,r19               ; get depth
   cfc2  v1,r31               ; get flags

   swc2  t6,RTPN.screen(a1)   ; store screen coords
 
   srl   v0,v0,$1             ; far depth
   sh    v0,RTPN.depth(a1)    ; store depth

   srl   v1,v1,$c             ; short flags
   andi  v1,v1,FarClipFlags   ; clip flags
   sh    v1,RTPN.flags(a1)    ; store flags

   sltu  v0,zero,v1           ; good/bad flag
   and   a2,a2,v0             ; accumulate all bad flag
   or    a3,a3,v0             ; accumulate all good flag

   addi  t9,t9,-$1
   bgtz  t9,@RotTransPersN
   addi  a1,a1,RTPN.sizeof    ; Branch delay slot!



; Finish if all flags bad

   bne   a2,zero,@AsmDrawLibObjFarOk
   nop


; AddPrim masks

   lui   fp,$00ff
   ori   fp,$ffff

   lui   gp,$ff00



; Don't clip if no flags bad

   if 0

   beq   a3,zero,@NoClip
   nop

   endif



@Clip:

; Fill in the gpu packets

@ClipLoop:

   lbu   v0,F3.type(s2)
   addi  v1,zero,TypeGT3

; **** GOURAUD TEXTURED TRIANGLES ****

@ClipGT3:

   bne   v0,v1,@ClipGT4
   addi  v1,zero,TypeGT4

   module

; Get coords

   lh    t0,GT3.coords+$0(s2)
   lh    t1,GT3.coords+$2(s2)
   lh    t2,GT3.coords+$4(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    a0,t4,t5
   or    a0,a0,t6
   bne   a0,zero,@End
   nop

; Get PolyGT3

   beq   s5,zero,@GotPoly
   addi  s3,s2,GT3.tri     ; branch delay slot

; Get second PolyGT3

   addi  s3,s3,PolyGT3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyGT3.xy0(s3)
   sw    t5,PolyGT3.xy1(s3)
   sw    t6,PolyGT3.xy2(s3)

; Get colour

   lwl   t4,LibGT3.colour0+$2(s1)
   lw    t5,LibGT3.colour1(s1)
   lw    t6,LibGT3.colour2(s1)

   swl   t4,PolyGT3.colour0+$2(s3)
   sw    t5,PolyGT3.colour1(s3)
   sw    t6,PolyGT3.colour2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibGT3.sizeof
   addi  s2,s2,GT3.sizeof

   modend

   j     @ClipNext
   addi  s7,s7,-$1  ; branch delay slot!



; **** GOURAUD TEXTURED QUADS ****

@ClipGT4:

   bne   v0,v1,@ClipFT3
   addi  v1,zero,TypeFT3

   module

; Get coords

   lh    t0,GT4.coords+$0(s2)
   lh    t1,GT4.coords+$2(s2)
   lh    t2,GT4.coords+$4(s2)
   lh    t3,GT4.coords+$6(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4
   add   t3,t3,s4

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   or    t4,t4,t7
   bne   t4,zero,@End
   nop

; Get PolyGT4

   beq   s5,zero,@GotPoly
   addi  s3,s2,GT4.quad    ; branch delay slot

; Get second PolyGT4

   addi  s3,s3,PolyGT4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT4.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyGT4.xy0(s3)
   sw    t5,PolyGT4.xy1(s3)
   sw    t6,PolyGT4.xy2(s3)
   sw    t7,PolyGT4.xy3(s3)

; Get colour

   lwl   t4,LibGT4.colour0+$2(s1)
   lw    t5,LibGT4.colour1(s1)
   lw    t6,LibGT4.colour2(s1)
   lw    t7,LibGT4.colour3(s1)

   swl   t4,PolyGT4.colour0+$2(s3)
   sw    t5,PolyGT4.colour1(s3)
   sw    t6,PolyGT4.colour2(s3)
   sw    t7,PolyGT4.colour3(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibGT4.sizeof
   addi  s2,s2,GT4.sizeof

   modend

   j     @ClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** FLAT TEXTURED TRIANGLES ****

@ClipFT3:

   bne   v0,v1,@ClipFT4
   addi  v1,zero,TypeFT4

   module

; Get coords

   lh    t0,FT3.coords+$0(s2)
   lh    t1,FT3.coords+$2(s2)
   lh    t2,FT3.coords+$4(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   bne   t4,zero,@End
   nop

; Get PolyFT3

   beq   s5,zero,@GotPoly
   addi  s3,s2,FT3.tri     ; branch delay slot

; Get second PolyFT3

   addi  s3,s3,PolyFT3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,FT3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyFT3.xy0(s3)
   sw    t5,PolyFT3.xy1(s3)
   sw    t6,PolyFT3.xy2(s3)

; Get colour

   lwl   t4,LibFT3.colour+$2(s1)
   nop
   swl   t4,PolyFT3.colour+$2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibFT3.sizeof
   addi  s2,s2,FT3.sizeof

   modend

   j     @ClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** FLAT TEXTURED QUADS ****

@ClipFT4:

   bne   v0,v1,@ClipG3
   addi  v1,zero,TypeG3

   module

; Get coords

   lh    t0,FT4.coords+$0(s2)
   lh    t1,FT4.coords+$2(s2)
   lh    t2,FT4.coords+$4(s2)
   lh    t3,FT4.coords+$6(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4
   add   t3,t3,s4

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   or    t4,t4,t7
   bne   t4,zero,@End
   nop

; Get PolyFT4

   beq   s5,zero,@GotPoly
   addi  s3,s2,FT4.quad    ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,FT4.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyFT4.xy0(s3)
   sw    t5,PolyFT4.xy1(s3)
   sw    t6,PolyFT4.xy2(s3)
   sw    t7,PolyFT4.xy3(s3)

; Get colour

   lwl   t4,LibFT4.colour+$2(s1)
   nop
   swl   t4,PolyFT4.colour+$2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibFT4.sizeof
   addi  s2,s2,FT4.sizeof

   modend

   j     @ClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** GOURAUD SHADED TRIANGLES ****

@ClipG3:

   bne   v0,v1,@ClipG4
   addi  v1,zero,TypeG4

   module

; Get coords

   lh    t0,G3.coords+$0(s2)
   lh    t1,G3.coords+$2(s2)
   lh    t2,G3.coords+$4(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   bne   t4,zero,@End
   nop

; Get PolyG3

   beq   s5,zero,@GotPoly
   addi  s3,s2,G3.tri      ; branch delay slot

; Get second PolyG3

   addi  s3,s3,PolyG3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,G3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyG3.xy0(s3)
   sw    t5,PolyG3.xy1(s3)
   sw    t6,PolyG3.xy2(s3)

; Get colour

   lwl   t4,LibG3.colour0+$2(s1)
   lw    t5,LibG3.colour1(s1)
   lw    t6,LibG3.colour2(s1)

   swl   t4,PolyG3.colour0+$2(s3)
   sw    t5,PolyG3.colour1(s3)
   sw    t6,PolyG3.colour2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibG3.sizeof
   addi  s2,s2,G3.sizeof

   modend

   j     @ClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** GOURAUD SHADED QUADS ****

@ClipG4:

   bne   v0,v1,@ClipF3
   addi  v1,zero,TypeF3

   module

; Get coords

   lh    t0,G4.coords+$0(s2)
   lh    t1,G4.coords+$2(s2)
   lh    t2,G4.coords+$4(s2)
   lh    t3,G4.coords+$6(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4
   add   t3,t3,s4

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   or    t4,t4,t7
   bne   t4,zero,@End
   nop

; Get PolyG4

   beq   s5,zero,@GotPoly
   addi  s3,s2,G4.quad     ; branch delay slot

; Get second PolyG4

   addi  s3,s3,PolyG4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,G4.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyG4.xy0(s3)
   sw    t5,PolyG4.xy1(s3)
   sw    t6,PolyG4.xy2(s3)
   sw    t7,PolyG4.xy3(s3)

; Get colour

   lwl   t4,LibG4.colour0+$2(s1)
   lw    t5,LibG4.colour1(s1)
   lw    t6,LibG4.colour2(s1)
   lw    t7,LibG4.colour3(s1)

   swl   t4,PolyG4.colour0+$2(s3)
   sw    t5,PolyG4.colour1(s3)
   sw    t6,PolyG4.colour2(s3)
   sw    t7,PolyG4.colour3(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibG4.sizeof
   addi  s2,s2,G4.sizeof

   modend

   j     @ClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** FLAT SHADED TRIANGLES ****

@ClipF3:
   
   bne   v0,v1,@ClipF4
   addi  v1,zero,TypeF4

   module

; Get coords

   lh    t0,F3.coords+$0(s2)
   lh    t1,F3.coords+$2(s2)
   lh    t2,F3.coords+$4(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   bne   t4,zero,@End
   nop

; Get PolyF3

   beq   s5,zero,@GotPoly
   addi  s3,s2,F3.tri      ; branch delay slot

; Get second PolyF3

   addi  s3,s3,PolyF3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,F3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyF3.xy0(s3)
   sw    t5,PolyF3.xy1(s3)
   sw    t6,PolyF3.xy2(s3)

; Get colour

   lwl   t4,LibF3.colour+$2(s1)
   nop
   swl   t4,PolyF3.colour+$2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibF3.sizeof
   addi  s2,s2,F3.sizeof

   modend

   j     @ClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** FLAT SHADED QUADS ****

@ClipF4:

   bne   v0,v1,@ClipBadPoly
   addi  v1,zero,-$1

   module

; Get coords

   lh    t0,F4.coords+$0(s2)
   lh    t1,F4.coords+$2(s2)
   lh    t2,F4.coords+$4(s2)
   lh    t3,F4.coords+$6(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4
   add   t3,t3,s4

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   or    t4,t4,t7
   bne   t4,zero,@End
   nop

; Get PolyF4

   beq   s5,zero,@GotPoly
   addi  s3,s2,F4.quad     ; branch delay slot

; Get second PolyF4

   addi  s3,s3,PolyF4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyF4.xy0(s3)
   sw    t5,PolyF4.xy1(s3)
   sw    t6,PolyF4.xy2(s3)
   sw    t7,PolyF4.xy3(s3)

; Get colour

   lwl   t4,LibF4.colour+$2(s1)
   nop
   swl   t4,PolyF4.colour+$2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibF4.sizeof
   addi  s2,s2,F4.sizeof

   modend

   j     @ClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** ANY OTHER POLY TYPES GO HERE ****


; **** UNRECOGNIZED POLY TYPE ****

@ClipBadPoly:

   bgtz  v0,@AsmDrawLibObjFarEnd
   nop                     ; Error result is primitive type

   j     @AsmDrawLibObjFarEnd
   addi  v0,zero,-$1       ; Error result is -1

; **** NEXT POLYGON ****

@ClipNext:

   bgtz  s7,@ClipLoop
   nop

   j     @AsmDrawLibObjFarOk
   nop
















@NoClip:

; Fill in the gpu packets

@NoClipLoop:

   lbu   v0,F3.type(s2)
   addi  v1,zero,TypeGT3

; **** NON CLIPPED GOURAUD TEXTURED TRIANGLES ****

@NoClipGT3:

   bne   v0,v1,@NoClipGT4
   addi  v1,zero,TypeGT4

   module

; Get coords

   lh    t0,GT3.coords+$0(s2)
   lh    t1,GT3.coords+$2(s2)
   lh    t2,GT3.coords+$4(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4

; Get PolyGT3

   beq   s5,zero,@GotPoly
   addi  s3,s2,GT3.tri     ; branch delay slot

; Get second PolyGT3

   addi  s3,s3,PolyGT3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyGT3.xy0(s3)
   sw    t5,PolyGT3.xy1(s3)
   sw    t6,PolyGT3.xy2(s3)

; Get colour

   lwl   t4,LibGT3.colour0+$2(s1)
   lw    t5,LibGT3.colour1(s1)
   lw    t6,LibGT3.colour2(s1)

   swl   t4,PolyGT3.colour0+$2(s3)
   sw    t5,PolyGT3.colour1(s3)
   sw    t6,PolyGT3.colour2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibGT3.sizeof
   addi  s2,s2,GT3.sizeof

   modend

   j     @NoClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** NON CLIPPED GOURAUD TEXTURED QUADS ****

@NoClipGT4:

   bne   v0,v1,@NoClipFT3
   addi  v1,zero,TypeFT3

   module

; Get coords

   lh    t0,GT4.coords+$0(s2)
   lh    t1,GT4.coords+$2(s2)
   lh    t2,GT4.coords+$4(s2)
   lh    t3,GT4.coords+$6(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4
   add   t3,t3,s4

; Get PolyGT4

   beq   s5,zero,@GotPoly
   addi  s3,s2,GT4.quad    ; branch delay slot

; Get second PolyGT4

   addi  s3,s3,PolyGT4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT4.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyGT4.xy0(s3)
   sw    t5,PolyGT4.xy1(s3)
   sw    t6,PolyGT4.xy2(s3)
   sw    t7,PolyGT4.xy3(s3)

; Get colour

   lwl   t4,LibGT4.colour0+$2(s1)
   lw    t5,LibGT4.colour1(s1)
   lw    t6,LibGT4.colour2(s1)
   lw    t7,LibGT4.colour3(s1)

   swl   t4,PolyGT4.colour0+$2(s3)
   sw    t5,PolyGT4.colour1(s3)
   sw    t6,PolyGT4.colour2(s3)
   sw    t7,PolyGT4.colour3(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibGT4.sizeof
   addi  s2,s2,GT4.sizeof

   modend

   j     @NoClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** NON CLIPPED FLAT TEXTURED TRIANGLES ****

@NoClipFT3:

   bne   v0,v1,@NoClipFT4
   addi  v1,zero,TypeFT4

   module

; Get coords

   lh    t0,FT3.coords+$0(s2)
   lh    t1,FT3.coords+$2(s2)
   lh    t2,FT3.coords+$4(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4

; Get PolyFT3

   beq   s5,zero,@GotPoly
   addi  s3,s2,FT3.tri     ; branch delay slot

; Get second PolyFT3

   addi  s3,s3,PolyFT3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,FT3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyFT3.xy0(s3)
   sw    t5,PolyFT3.xy1(s3)
   sw    t6,PolyFT3.xy2(s3)

; Get colour

   lwl   t4,LibFT3.colour+$2(s1)
   nop
   swl   t4,PolyFT3.colour+$2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibFT3.sizeof
   addi  s2,s2,FT3.sizeof

   modend

   j     @NoClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** NON CLIPPED FLAT TEXTURED QUADS ****

@NoClipFT4:

   bne   v0,v1,@NoClipG3
   addi  v1,zero,TypeG3

   module

; Get coords

   lh    t0,FT4.coords+$0(s2)
   lh    t1,FT4.coords+$2(s2)
   lh    t2,FT4.coords+$4(s2)
   lh    t3,FT4.coords+$6(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4
   add   t3,t3,s4

; Get PolyFT4

   beq   s5,zero,@GotPoly
   addi  s3,s2,FT4.quad    ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,FT4.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyFT4.xy0(s3)
   sw    t5,PolyFT4.xy1(s3)
   sw    t6,PolyFT4.xy2(s3)
   sw    t7,PolyFT4.xy3(s3)

; Get colour

   lwl   t4,LibFT4.colour+$2(s1)
   nop
   swl   t4,PolyFT4.colour+$2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibFT4.sizeof
   addi  s2,s2,FT4.sizeof

   modend

   j     @NoClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** NON CLIPPED GOURAUD SHADED TRIANGLES ****

@NoClipG3:

   bne   v0,v1,@NoClipG4
   addi  v1,zero,TypeG4

   module

; Get coords

   lh    t0,G3.coords+$0(s2)
   lh    t1,G3.coords+$2(s2)
   lh    t2,G3.coords+$4(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4

; Get PolyG3

   beq   s5,zero,@GotPoly
   addi  s3,s2,G3.tri      ; branch delay slot

; Get second PolyG3

   addi  s3,s3,PolyG3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,G3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyG3.xy0(s3)
   sw    t5,PolyG3.xy1(s3)
   sw    t6,PolyG3.xy2(s3)

; Get colour

   lwl   t4,LibG3.colour0+$2(s1)
   lw    t5,LibG3.colour1(s1)
   lw    t6,LibG3.colour2(s1)

   swl   t4,PolyG3.colour0+$2(s3)
   sw    t5,PolyG3.colour1(s3)
   sw    t6,PolyG3.colour2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibG3.sizeof
   addi  s2,s2,G3.sizeof

   modend

   j     @NoClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** NON CLIPPED GOURAUD SHADED QUADS ****

@NoClipG4:

   bne   v0,v1,@NoClipF3
   addi  v1,zero,TypeF3

   module

; Get coords

   lh    t0,G4.coords+$0(s2)
   lh    t1,G4.coords+$2(s2)
   lh    t2,G4.coords+$4(s2)
   lh    t3,G4.coords+$6(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4
   add   t3,t3,s4

; Get PolyG4

   beq   s5,zero,@GotPoly
   addi  s3,s2,G4.quad     ; branch delay slot

; Get second PolyG4

   addi  s3,s3,PolyG4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,G4.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyG4.xy0(s3)
   sw    t5,PolyG4.xy1(s3)
   sw    t6,PolyG4.xy2(s3)
   sw    t7,PolyG4.xy3(s3)

; Get colour

   lwl   t4,LibG4.colour0+$2(s1)
   lw    t5,LibG4.colour1(s1)
   lw    t6,LibG4.colour2(s1)
   lw    t7,LibG4.colour3(s1)

   swl   t4,PolyG4.colour0+$2(s3)
   sw    t5,PolyG4.colour1(s3)
   sw    t6,PolyG4.colour2(s3)
   sw    t7,PolyG4.colour3(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibG4.sizeof
   addi  s2,s2,G4.sizeof

   modend

   j     @NoClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** NON CLIPPED FLAT SHADED TRIANGLES ****

@NoClipF3:

   bne   v0,v1,@NoClipF4
   addi  v1,zero,TypeF4

   module

; Get coords

   lh    t0,F3.coords+$0(s2)
   lh    t1,F3.coords+$2(s2)
   lh    t2,F3.coords+$4(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4

; Get PolyF3

   beq   s5,zero,@GotPoly
   addi  s3,s2,F3.tri      ; branch delay slot

; Get second PolyF3

   addi  s3,s3,PolyF3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,F3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyF3.xy0(s3)
   sw    t5,PolyF3.xy1(s3)
   sw    t6,PolyF3.xy2(s3)

; Get colour

   lwl   t4,LibF3.colour+$2(s1)
   nop
   swl   t4,PolyF3.colour+$2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibF3.sizeof
   addi  s2,s2,F3.sizeof

   modend

   j     @NoClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** NON CLIPPED FLAT SHADED QUADS ****

@NoClipF4:

   bne   v0,v1,@NoClipBadPoly
   addi  v1,zero,-$1

   module

; Get coords

   lh    t0,F4.coords+$0(s2)
   lh    t1,F4.coords+$2(s2)
   lh    t2,F4.coords+$4(s2)
   lh    t3,F4.coords+$6(s2)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s4
   add   t1,t1,s4
   add   t2,t2,s4
   add   t3,t3,s4

; Get PolyF4

   beq   s5,zero,@GotPoly
   addi  s3,s2,F4.quad     ; branch delay slot

; Get second PolyF4

   addi  s3,s3,PolyF4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT3.flag(s2)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Test flags

   andi  v0,v0,SingleSided
   beq   v0,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
   nop
   blez  v0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyF4.xy0(s3)
   sw    t5,PolyF4.xy1(s3)
   sw    t6,PolyF4.xy2(s3)
   sw    t7,PolyF4.xy3(s3)

; Get colour

   lwl   t4,LibF4.colour+$2(s1)
   nop
   swl   t4,PolyF4.colour+$2(s3)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s6,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s1,s1,LibF4.sizeof
   addi  s2,s2,F4.sizeof

   modend

   j     @NoClipNext
   addi  s7,s7,-$1         ; branch delay slot!



; **** ANY OTHER POLY TYPES GO HERE ****


; **** UNRECOGNIZED POLY TYPE ****

@NoClipBadPoly:

   bgtz  v0,@AsmDrawLibObjFarEnd
   nop                     ; Error result is primitive type
   j     @AsmDrawLibObjFarEnd
   addi  v0,zero,-$1       ; Error result is -1

; **** NEXT POLYGON ****

@NoClipNext:

   bgtz  s7,@NoClipLoop
   nop






; Restore registers and unlink stack space

@AsmDrawLibObjFarOk:

   addi  v0,zero,$0        ; Success result is zero

@AsmDrawLibObjFarEnd:

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
   lw    gp,$28(sp)

   jr    ra
   addiu sp,sp,$2c

   modend

; AsmDrawLibObjFar End



   endif



























