
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
   xref  prims

   xref  TextureTable
   xref  rcos
   xref  rsin

   xdef  AsmDrawObject
   xdef  AsmDrawObjectFixed

   include  asm.i












; PARAMETERS

; a0 : Object*       object
; a1 : Skeleton*     camera
; a2 : short         roll 

; REGISTER ASSIGNMENT

; s0 : object ptr
; s1 : camera ptr
; s2 : poly ptr
; s3 : roll
; s4 : primitive ptr
; s5 : primitive count
; s6 : CurrentScreen
; s7 : OT ptr

; t0 : 
; t1 : 
; t2 : 
; t3 : 
; t4 :
; t5 :
; t6 :
; t7 :
; t8 : vertex ptr   / AddPrim mask $00ffffff
; t9 : vertex count / AddPrim mask $ff000000

; fp : RTPN ptr
; gp : normal ptr    * temp *
; at :
; k0 :
; k1 :

AsmDrawObject: 

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

   move  s0,a0
   move  s1,a1
   move  s3,a2

; Set the RotTrans matrix

   addiu sp,sp,-$4

   lw    a0,Object.skeleton(s0)
   jal   SetSkeletonTransform
   nop

   addiu sp,sp,$4

; Get vertex pointer and counter

   lw    t8,Object.vertices(s0)
   lhu   t9,Object.vertexCount(s0)

; Get pointer to primitive buffers

   lw    s2,prims

; Get primitive pointer, counter and index

   lw    s4,Object.primitives(s0)
   lhu   s5,Object.primitiveCount(s0)

; Get normal pointer

   lw    gp,Object.normals(s0)

; Pointer to the OT

   lhu   s6,CurrentScreen

   lui   v0,OT>>16
   ori   v0,OT&$ffff

   sll   v1,s6,$2
   add   v0,v0,v1

   lw    s7,0(v0)

; If vertex count < 128 use fast ram else use normal ram

   addi  v0,t9,-$80
   blez  v0,@FastRam

   lui   fp,$1f80          ; Branch delay slot!

@SlowRam:

   lui   fp,SlowMem>>16
   ori   fp,SlowMem&$ffff

@FastRam:

; RotTransPersN

   move  a0,t8                   ; vertex ptr
   move  a1,fp                   ; rtpn ptr

   addi  v0,zero,$0              ; all good flags
   addi  v1,zero,$1              ; all bad flags

@RotTransPersN
   
   lwc2  zero,$0(a0)             ; load svector
   lwc2  at,$4(a0)               ; load svector
   addi  a0,a0,SVECTOR.sizeof    ; increment svector ptr
   cop2  $180001                 ; RotTransPers
   
   mfc2  a2,r19                  ; get depth
   cfc2  a3,r31                  ; get flags

   swc2  t6,RTPN.screen(a1)      ; store screen coords
  
   sh    a2,RTPN.depth(a1)       ; store depth

   srl   a3,a3,$c                ; short flags
   andi  a3,a3,ShortClipFlags    ; clip flags
   sh    a3,RTPN.flags(a1)       ; store flags
   
   sltu  a2,zero,a3              ; good / bad flag
   or    v0,v0,a3                ; accumulate all good flag
   and   v1,v1,a3                ; accumulate all bad flag
   
   addi  t9,t9,-$1               ; decrement ...
   bgtz  t9,@RotTransPersN       ; ... and branch
   addi  a1,a1,RTPN.sizeof       ; increment rtpn ptr


; Test all bad cumulative flag

   bne   v1,zero,AsmDrawObjectEnd   ; end if all bad
   nop


; Zero translation matrix ( for rotating normals )

   if 0

   ctc2  zero,r5
   ctc2  zero,r6
   ctc2  zero,r7

   endif


; Test all good cumulative flag and set AddPrim masks

   lui   t8,$00ff
   ori   t8,$ffff

   beq   v0,zero,@NoClip         ; don't clip if all good

   lui   t9,$ff00                ; Branch delay slot!


; **** NEED TO CHECK FLAGS SINCE SOME ARE BAD ****

@Clip:

; Fill in the gpu packets

@ClipLoop:

   lhu   v0,F3.type(s4)
   addi  v1,zero,TypeGT3



; **** GOURAUD TEXTURED TRIANGLES ****

@ClipGT3:

   bne   v0,v1,@ClipGT4
   addi  v1,zero,TypeGT4

   module

; Get coords

   lh    t0,GT3.coords+$0(s4)
   lh    t1,GT3.coords+$2(s4)
   lh    t2,GT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   bne   v0,zero,@End
   nop

; Get polygon flags

   lhu   v0,GT3.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyGT3.xy0(s2)
   sw    t5,PolyGT3.xy1(s2)
   sw    t6,PolyGT3.xy2(s2)

; SetPolyGT3
 
   ori   a0,zero,$9
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$34

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,GT3.cba(s4)
   lhu   v1,GT3.tsb(s4)
   
   sh    v0,PolyGT3.cba(s2)
   andi  v1,v1,$ff9f
   ori   v1,v1,$0020
   sh    v1,PolyGT3.tsb(s2)

; Set uv coords

   lhu   a0,GT3.u0(s4)
   lhu   a1,GT3.u1(s4)
   lhu   a2,GT3.u2(s4)
   
   sh    a0,PolyGT3.uv0(s2)
   sh    a1,PolyGT3.uv1(s2)
   sh    a2,PolyGT3.uv2(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,GT3.colour0+$2(s4)
   lw    a1,GT3.colour1(s4)
   lw    a2,GT3.colour2(s4)

   swl   a0,PolyGT3.colour0+$2(s2)
   sw    a1,PolyGT3.colour1(s2)
   sw    a2,PolyGT3.colour2(s2)

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
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyGT3.sizeof

@End:

   addi  s4,s4,GT3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD TEXTURED QUADS ****

@ClipGT4:

   bne   v0,v1,@ClipFT3
   addi  v1,zero,TypeFT3

   module

; Get coords

   lh    t0,GT4.coords+$0(s4)
   lh    t1,GT4.coords+$2(s4)
   lh    t2,GT4.coords+$4(s4)
   lh    t3,GT4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   bne   v0,zero,@End
   nop

; Get polygon flags

   lhu   v0,GT4.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyGT4.xy0(s2)
   sw    t5,PolyGT4.xy1(s2)
   sw    t6,PolyGT4.xy2(s2)
   sw    t7,PolyGT4.xy3(s2)

; SetPolyGT4

   ori   a0,zero,$c
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$3c

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,GT4.cba(s4)
   lhu   v1,GT4.tsb(s4)
   
   sh    v0,PolyGT4.cba(s2)
   andi  v1,v1,$ff9f
   ori   v1,v1,$0020
   sh    v1,PolyGT4.tsb(s2)

; Set uv coords

   lhu   a0,GT4.u0(s4)
   lhu   a1,GT4.u1(s4)
   lhu   a2,GT4.u2(s4)
   lhu   a3,GT4.u3(s4)
   
   sh    a0,PolyGT4.uv0(s2)
   sh    a1,PolyGT4.uv1(s2)
   sh    a2,PolyGT4.uv2(s2)
   sh    a3,PolyGT4.uv3(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,GT4.colour0+$2(s4)
   lw    a1,GT4.colour1(s4)
   lw    a2,GT4.colour2(s4)
   lw    a3,GT4.colour3(s4)

   swl   a0,PolyGT4.colour0+$2(s2)
   sw    a1,PolyGT4.colour1(s2)
   sw    a2,PolyGT4.colour2(s2)
   sw    a3,PolyGT4.colour3(s2)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   lhu   a3,RTPN.depth(t3)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyGT4.sizeof

@End:

   addi  s4,s4,GT4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED TRIANGLES ****

@ClipFT3:

   bne   v0,v1,@ClipFT4
   addi  v1,zero,TypeFT4

   module

; Get coords

   lh    t0,FT3.coords+$0(s4)
   lh    t1,FT3.coords+$2(s4)
   lh    t2,FT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   bne   v0,zero,@End
   nop

; Get polygon flags

   lhu   v0,FT3.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyFT3.xy0(s2)
   sw    t5,PolyFT3.xy1(s2)
   sw    t6,PolyFT3.xy2(s2)

; SetPolyFT3
 
   ori   a0,zero,$7
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$24

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,FT3.cba(s4)
   lhu   v1,FT3.tsb(s4)
   
   sh    v0,PolyFT3.cba(s2)
   andi  v1,v1,$ff9f
   ori   v1,v1,$0020
   sh    v1,PolyFT3.tsb(s2)

; Set uv coords

   lhu   a0,FT3.u0(s4)
   lhu   a1,FT3.u1(s4)
   lhu   a2,FT3.u2(s4)
   
   sh    a0,PolyFT3.uv0(s2)
   sh    a1,PolyFT3.uv1(s2)
   sh    a2,PolyFT3.uv2(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,FT3.colour+$2(s4)
   nop
   swl   a0,PolyFT3.colour+$2(s2)

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
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyFT3.sizeof

@End:

   addi  s4,s4,FT3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED QUADS ****

@ClipFT4:

   bne   v0,v1,@ClipG3
   addi  v1,zero,TypeG3

   module

; Get coords

   lh    t0,FT4.coords+$0(s4)
   lh    t1,FT4.coords+$2(s4)
   lh    t2,FT4.coords+$4(s4)
   lh    t3,FT4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   bne   v0,zero,@End
   nop

; Get polygon flags

   lhu   v0,FT4.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyFT4.xy0(s2)
   sw    t5,PolyFT4.xy1(s2)
   sw    t6,PolyFT4.xy2(s2)
   sw    t7,PolyFT4.xy3(s2)

; SetPolyFT4

   ori   a0,zero,$9
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$2c

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,FT4.cba(s4)
   lhu   v1,FT4.tsb(s4)
   
   sh    v0,PolyFT4.cba(s2)
   andi  v1,v1,$ff9f
   ori   v1,v1,$0020
   sh    v1,PolyFT4.tsb(s2)

; Set uv coords

   lhu   a0,FT4.u0(s4)
   lhu   a1,FT4.u1(s4)
   lhu   a2,FT4.u2(s4)
   lhu   a3,FT4.u3(s4)
   
   sh    a0,PolyFT4.uv0(s2)
   sh    a1,PolyFT4.uv1(s2)
   sh    a2,PolyFT4.uv2(s2)
   sh    a3,PolyFT4.uv3(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,FT4.colour+$2(s4)
   nop
   swl   a0,PolyFT4.colour+$2(s2)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   lhu   a3,RTPN.depth(t3)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyFT4.sizeof

@End:

   addi  s4,s4,FT4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED TRIANGLES ****

@ClipG3:

   bne   v0,v1,@ClipG4
   addi  v1,zero,TypeG4

   module

; Get coords

   lh    t0,G3.coords+$0(s4)
   lh    t1,G3.coords+$2(s4)
   lh    t2,G3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   bne   v0,zero,@End
   nop

; Get polygon flags

   lhu   v0,G3.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyG3.xy0(s2)
   sw    t5,PolyG3.xy1(s2)
   sw    t6,PolyG3.xy2(s2)

; SetPolyG3
 
   ori   a0,zero,$6
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$30

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,G3.colour0+$2(s4)
   lw    a1,G3.colour1(s4)
   lw    a2,G3.colour2(s4)

   swl   a0,PolyG3.colour0+$2(s2)
   sw    a1,PolyG3.colour1(s2)
   sw    a2,PolyG3.colour2(s2)

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
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyG3.sizeof

@End:

   addi  s4,s4,G3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED QUADS ****

@ClipG4:

   bne   v0,v1,@ClipF3
   addi  v1,zero,TypeF3

   module

; Get coords

   lh    t0,G4.coords+$0(s4)
   lh    t1,G4.coords+$2(s4)
   lh    t2,G4.coords+$4(s4)
   lh    t3,G4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   bne   v0,zero,@End
   nop

; Get polygon flags

   lhu   v0,G4.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyG4.xy0(s2)
   sw    t5,PolyG4.xy1(s2)
   sw    t6,PolyG4.xy2(s2)
   sw    t7,PolyG4.xy3(s2)

; SetPolyG4

   ori   a0,zero,$8
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$38

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,G4.colour0+$2(s4)
   lw    a1,G4.colour1(s4)
   lw    a2,G4.colour2(s4)
   lw    a3,G4.colour3(s4)

   swl   a0,PolyG4.colour0+$2(s2)
   sw    a1,PolyG4.colour1(s2)
   sw    a2,PolyG4.colour2(s2)
   sw    a3,PolyG4.colour3(s2)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   lhu   a3,RTPN.depth(t3)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyG4.sizeof

@End:

   addi  s4,s4,G4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT SHADED TRIANGLES ****

@ClipF3:

   bne   v0,v1,@ClipF4
   addi  v1,zero,TypeF4

   module

; Get coords

   lh    t0,F3.coords+$0(s4)
   lh    t1,F3.coords+$2(s4)
   lh    t2,F3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   bne   v0,zero,@End
   nop

; Get polygon flags

   lhu   v0,F3.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyF3.xy0(s2)
   sw    t5,PolyF3.xy1(s2)
   sw    t6,PolyF3.xy2(s2)

; SetPolyF3
 
   ori   a0,zero,$4
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$20

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,F3.colour+$2(s4)
   nop
   swl   a0,PolyF3.colour+$2(s2)

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
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyF3.sizeof

@End:

   addi  s4,s4,F3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT SHADED QUADS ****

@ClipF4:

   bne   v0,v1,@ClipTSPR
   addi  v1,zero,TypeTSPR

   module

; Get coords

   lh    t0,F4.coords+$0(s4)
   lh    t1,F4.coords+$2(s4)
   lh    t2,F4.coords+$4(s4)
   lh    t3,F4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   bne   v0,zero,@End
   nop

; Get polygon flags

   lhu   v0,F4.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyF4.xy0(s2)
   sw    t5,PolyF4.xy1(s2)
   sw    t6,PolyF4.xy2(s2)
   sw    t7,PolyF4.xy3(s2)

; SetPolyF4

   ori   a0,zero,$5
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$28

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,F4.colour+$2(s4)
   nop
   swl   a0,PolyF4.colour+$2(s2)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   lhu   a3,RTPN.depth(t3)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyF4.sizeof

@End:

   addi  s4,s4,F4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!












@ClipTSPR:

   bne   v0,v1,@ClipBSPR
   addi  v1,zero,TypeBSPR

   module

; Get coord

   lh    t0,SPR.coord+$0(s4)
   nop

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3

; Convert indices to addresses

   add   t0,t0,fp

; Get flags

   lh    v0,RTPN.flags(t0)
   nop

; Test flags

   bne   v0,zero,@End
   nop

; Get sprite size

   lh    v0,SPR.width+$0(s4)  ; sprite width
   lh    v1,SPR.height+$0(s4) ; sprite height
   addi  t4,zero,160
   multu v0,t4
   mflo  t5             ; width * 160
   nop
   nop
   multu v1,t4
   mflo  t6             ; height * 160

; calculate xsize and ysize

   lhu   t4,RTPN.depth(t0)
   nop
   divu  t5,t4
   mflo  t5             ; t5 : xsize
   nop
   nop
   divu  t6,t4
   mflo  t6             ; t6 : ysize

; calculate xsize/2 and ysize/2

   sra    t4,t5,$1      ; t4 : xsize / 2
   sra    t7,t6,$1      ; t7 : ysize / 2

; calc rcos and rsin

   move  a0,s3

   jal   rcos           ; returned in v0
   nop
   multu v0,t4
   mflo  t7
   sra   t7,t7,$c       ; t7 offsetX


   move  a0,s3

   jal   rsin           ; returned in v0
   nop
   multu v0,t4
   mflo  t4
   sra   t4,t4,$c       ; t4 offsetY


   addi  a1,zero,1024
   sub   a0,a1,s3

   jal   rcos           ; returned in v0
   nop
   multu v0,t6
   mflo  t5
   sra   t5,t5,$c       ; t5 offset2X

   addi  a1,zero,1024
   sub   a0,a1,s3

   jal   rsin           ; returned in v0
   nop
   multu v0,t6
   mflo  t6
   sra   t6,t6,$c       ; t6 offset2Y

; Get screen coordinates of top middle

   lw    v0,RTPN.screen(t0)
   nop
   move  v1,v0
   andi  v0,v0,$ffff    ; v0 : x screen
   sra   v1,v1,$10      ; v1 : y screen

; work out the bounding sprite box

   sub   a0,v0,t7       ; tlX
   sub   a1,v1,t4       ; tlY
   move  a2,a0          ; store tlX
   move  a3,a1          ; store tlY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy0(s2)


   sub   a0,a2,t5       ; blX    
   add   a1,a3,t6       ; blY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy2(s2)


   add   a0,v0,t7       ; trX
   add   a1,v1,t4       ; trY
   move  a2,a0          ; store trX
   move  a3,a1          ; store trY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy1(s2)


   sub   a0,a2,t5       ; brX
   add   a1,a3,t6       ; brY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy3(s2)

; Get colour

   lw    t4,SPR.colour(s4)
   nop

   sh    t4,PolyFT4.colour+$0(s2)
   srl   t4,t4,$10
   sb    t4,PolyFT4.colour+$2(s2)

; poke in the texture coords

   lh    a0,SPR.texture+$0(s4)   ; get texture index into texture table
   lui   t4,TextureTable>>$10
   ori   t4,t4,TextureTable&$ffff
   sll   a0,a0,$2
   add   a0,a0,t4
   lw    a3,$0(a0)
   nop

; SetPolyFT4

   ori   v0,zero,$9
   sb    v0,$3(s2)

   ori   v0,zero,$2c
   sb    v0,$7(s2)

; tsb, cba

   lh    a2,$2(a3)
   nop
   sh    a2,PolyFT4.tsb(s2)

   lh    a2,$4(a3)
   nop
   sh    a2,PolyFT4.cba(s2)

; uv0

   lh    a2,$16(a3)
   nop
   sb    a2,PolyFT4.uv0(s2)

   lh    a2,$18(a3)
   nop
   sb    a2,PolyFT4.uv0+$1(s2)

; uv1

   lh    a2,$1a(a3)
   nop
   sb    a2,PolyFT4.uv1(s2)

   lh    a2,$1c(a3)
   nop
   sb    a2,PolyFT4.uv1+$1(s2)

; uv2

   lh    a2,$1e(a3)
   nop
   sb    a2,PolyFT4.uv2(s2)

   lh    a2,$20(a3)
   nop
   sb    a2,PolyFT4.uv2+$1(s2)

; uv3

   lh    a2,$22(a3)
   nop
   sb    a2,PolyFT4.uv3(s2)

   lh    a2,$24(a3)
   nop
   sb    a2,PolyFT4.uv3+$1(s2)

; Get depth ( 1 )

   lhu   v0,RTPN.depth(t0)

; AddPrim

   move  a0,s2       ; Get poly address
   andi  v0,v0,$fffc ; Mask out bottom 2 bits
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

   addi  s2,s2,PolyFT4.sizeof

@End:

   addi  s4,s4,SPR.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!






@ClipBSPR:

   bne   v0,v1,@ClipBadPoly
   addi  v1,zero,-$1

   module

; Get coord

   lh    t0,SPR.coord+$0(s4)
   nop

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3

; Convert indices to addresses

   add   t0,t0,fp

; Get flags

   lh    v0,RTPN.flags(t0)
   nop

; Test flags

   bne   v0,zero,@End
   nop

; Get sprite size

   lh    v0,SPR.width+$0(s4)  ; sprite width
   lh    v1,SPR.height+$0(s4) ; sprite height
   addi  t4,zero,160
   multu v0,t4
   mflo  t5             ; width * 160
   nop
   nop
   multu v1,t4
   mflo  t6             ; height * 160

; calculate xsize and ysize

   lhu   t4,RTPN.depth(t0)
   nop
   divu  t5,t4
   mflo  t5             ; t5 : xsize
   nop
   nop
   divu  t6,t4
   mflo  t6             ; t6 : ysize

; calculate xsize/2 and ysize/2

   sra    t4,t5,$1      ; t4 : xsize / 2
   sra    t7,t6,$1      ; t7 : ysize / 2

; calc rcos and rsin

   move  a0,s3

   jal   rcos           ; returned in v0
   nop
   multu v0,t4
   mflo  t7
   sra   t7,t7,$c       ; t7 offsetX


   move  a0,s3

   jal   rsin           ; returned in v0
   nop
   multu v0,t4
   mflo  t4
   sra   t4,t4,$c       ; t4 offsetY


   addi  a1,zero,1024
   sub   a0,a1,s3

   jal   rcos           ; returned in v0
   nop
   multu v0,t6
   mflo  t5
   sra   t5,t5,$c       ; t5 offset2X

   addi  a1,zero,1024
   sub   a0,a1,s3

   jal   rsin           ; returned in v0
   nop
   multu v0,t6
   mflo  t6
   sra   t6,t6,$c       ; t6 offset2Y

; Get screen coordinates of top middle

   lw    v0,RTPN.screen(t0)
   nop
   move  v1,v0
   andi  v0,v0,$ffff    ; v0 : x screen
   sra   v1,v1,$10      ; v1 : y screen

; work out the bounding sprite box

   sub   a0,v0,t7       ; blX
   sub   a1,v1,t4       ; blY
   move  a2,a0          ; store blX
   move  a3,a1          ; store blY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy2(s2)


   add   a0,a2,t5       ; tlX    
   sub   a1,a3,t6       ; tlY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy0(s2)


   add   a0,v0,t7       ; brX
   add   a1,v1,t4       ; brY
   move  a2,a0          ; store brX
   move  a3,a1          ; store brY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy3(s2)


   add   a0,a2,t5       ; trX
   sub   a1,a3,t6       ; trY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy1(s2)

; Get colour

   lw    t4,SPR.colour(s4)
   nop

   sh    t4,PolyFT4.colour+$0(s2)
   srl   t4,t4,$10
   sb    t4,PolyFT4.colour+$2(s2)

; poke in the texture coords

   lh    a0,SPR.texture+$0(s4)   ; get texture index into texture table
   lui   t4,TextureTable>>$10
   ori   t4,t4,TextureTable&$ffff
   sll   a0,a0,$2
   add   a0,a0,t4
   lw    a3,$0(a0)
   nop

; SetPolyFT4

   ori   v0,zero,$9
   sb    v0,$3(s2)

   ori   v0,zero,$2c
   sb    v0,$7(s2)

; tsb, cba

   lh    a2,$2(a3)
   nop
   sh    a2,PolyFT4.tsb(s2)

   lh    a2,$4(a3)
   nop
   sh    a2,PolyFT4.cba(s2)

; uv0

   lh    a2,$16(a3)
   nop
   sb    a2,PolyFT4.uv0(s2)

   lh    a2,$18(a3)
   nop
   sb    a2,PolyFT4.uv0+$1(s2)

; uv1

   lh    a2,$1a(a3)
   nop
   sb    a2,PolyFT4.uv1(s2)

   lh    a2,$1c(a3)
   nop
   sb    a2,PolyFT4.uv1+$1(s2)

; uv2

   lh    a2,$1e(a3)
   nop
   sb    a2,PolyFT4.uv2(s2)

   lh    a2,$20(a3)
   nop
   sb    a2,PolyFT4.uv2+$1(s2)

; uv3

   lh    a2,$22(a3)
   nop
   sb    a2,PolyFT4.uv3(s2)

   lh    a2,$24(a3)
   nop
   sb    a2,PolyFT4.uv3+$1(s2)

; Get depth ( 1 )

   lhu   v0,RTPN.depth(t0)

; AddPrim

   move  a0,s2       ; Get poly address
   andi  v0,v0,$fffc ; Mask out bottom 2 bits
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

   addi  s2,s2,PolyFT4.sizeof

@End:

   addi  s4,s4,SPR.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!








; **** ANY OTHER POLY TYPES GO HERE ****










; **** UNRECOGNIZED POLY TYPE ****

@ClipBadPoly:

   bgtz  v0,AsmDrawObjectEnd
   nop                     ; Error result is primitive type

   j     AsmDrawObjectEnd
   addi  v0,zero,-$1       ; Error result is -1



; **** NEXT POLYGON ****

@ClipNext:

   bgtz  s5,@ClipLoop
   nop

   j     AsmDrawObjectOk
   nop







; **** ALL FLAGS GOOD SO NO CHECKS NEEDED ****


@NoClip:

; Fill in the gpu packets

@NoClipLoop:

   lhu   v0,F3.type(s4)
   addi  v1,zero,TypeGT3



; **** GOURAUD TEXTURED TRIANGLES ****

@NoClipGT3:

   bne   v0,v1,@NoClipGT4
   addi  v1,zero,TypeGT4

   module

; Get coords

   lh    t0,GT3.coords+$0(s4)
   lh    t1,GT3.coords+$2(s4)
   lh    t2,GT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get polygon flags

   lhu   v0,GT3.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyGT3.xy0(s2)
   sw    t5,PolyGT3.xy1(s2)
   sw    t6,PolyGT3.xy2(s2)

; SetPolyGT3
 
   ori   a0,zero,$9
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$34

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,GT3.cba(s4)
   lhu   v1,GT3.tsb(s4)
   
   sh    v0,PolyGT3.cba(s2)
   andi  v1,v1,$ff9f
   ori   v1,v1,$0020
   sh    v1,PolyGT3.tsb(s2)

; Set uv coords

   lhu   a0,GT3.u0(s4)
   lhu   a1,GT3.u1(s4)
   lhu   a2,GT3.u2(s4)
   
   sh    a0,PolyGT3.uv0(s2)
   sh    a1,PolyGT3.uv1(s2)
   sh    a2,PolyGT3.uv2(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,GT3.colour0+$2(s4)
   lw    a1,GT3.colour1(s4)
   lw    a2,GT3.colour2(s4)

   swl   a0,PolyGT3.colour0+$2(s2)
   sw    a1,PolyGT3.colour1(s2)
   sw    a2,PolyGT3.colour2(s2)

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
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyGT3.sizeof

@End:

   addi  s4,s4,GT3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD TEXTURED QUADS ****

@NoClipGT4:

   bne   v0,v1,@NoClipFT3
   addi  v1,zero,TypeFT3

   module

; Get coords

   lh    t0,GT4.coords+$0(s4)
   lh    t1,GT4.coords+$2(s4)
   lh    t2,GT4.coords+$4(s4)
   lh    t3,GT4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get polygon flags

   lhu   v0,GT4.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyGT4.xy0(s2)
   sw    t5,PolyGT4.xy1(s2)
   sw    t6,PolyGT4.xy2(s2)
   sw    t7,PolyGT4.xy3(s2)

; SetPolyGT4

   ori   a0,zero,$c
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$3c

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,GT4.cba(s4)
   lhu   v1,GT4.tsb(s4)
   
   sh    v0,PolyGT4.cba(s2)
   andi  v1,v1,$ff9f
   ori   v1,v1,$0020
   sh    v1,PolyGT4.tsb(s2)

; Set uv coords

   lhu   a0,GT4.u0(s4)
   lhu   a1,GT4.u1(s4)
   lhu   a2,GT4.u2(s4)
   lhu   a3,GT4.u3(s4)
   
   sh    a0,PolyGT4.uv0(s2)
   sh    a1,PolyGT4.uv1(s2)
   sh    a2,PolyGT4.uv2(s2)
   sh    a3,PolyGT4.uv3(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,GT4.colour0+$2(s4)
   lw    a1,GT4.colour1(s4)
   lw    a2,GT4.colour2(s4)
   lw    a3,GT4.colour3(s4)

   swl   a0,PolyGT4.colour0+$2(s2)
   sw    a1,PolyGT4.colour1(s2)
   sw    a2,PolyGT4.colour2(s2)
   sw    a3,PolyGT4.colour3(s2)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   lhu   a3,RTPN.depth(t3)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyGT4.sizeof

@End:

   addi  s4,s4,GT4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED TRIANGLES ****

@NoClipFT3:

   bne   v0,v1,@NoClipFT4
   addi  v1,zero,TypeFT4

   module

; Get coords

   lh    t0,FT3.coords+$0(s4)
   lh    t1,FT3.coords+$2(s4)
   lh    t2,FT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get polygon flags

   lhu   v0,FT3.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyFT3.xy0(s2)
   sw    t5,PolyFT3.xy1(s2)
   sw    t6,PolyFT3.xy2(s2)

; SetPolyFT3
 
   ori   a0,zero,$7
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$24

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,FT3.cba(s4)
   lhu   v1,FT3.tsb(s4)
   
   sh    v0,PolyFT3.cba(s2)
   andi  v1,v1,$ff9f
   ori   v1,v1,$0020
   sh    v1,PolyFT3.tsb(s2)

; Set uv coords

   lhu   a0,FT3.u0(s4)
   lhu   a1,FT3.u1(s4)
   lhu   a2,FT3.u2(s4)
   
   sh    a0,PolyFT3.uv0(s2)
   sh    a1,PolyFT3.uv1(s2)
   sh    a2,PolyFT3.uv2(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,FT3.colour+$2(s4)
   nop
   swl   a0,PolyFT3.colour+$2(s2)

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
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyFT3.sizeof

@End:

   addi  s4,s4,FT3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED QUADS ****

@NoClipFT4:

   bne   v0,v1,@NoClipG3
   addi  v1,zero,TypeG3

   module

; Get coords

   lh    t0,FT4.coords+$0(s4)
   lh    t1,FT4.coords+$2(s4)
   lh    t2,FT4.coords+$4(s4)
   lh    t3,FT4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get polygon flags

   lhu   v0,FT4.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyFT4.xy0(s2)
   sw    t5,PolyFT4.xy1(s2)
   sw    t6,PolyFT4.xy2(s2)
   sw    t7,PolyFT4.xy3(s2)

; SetPolyFT4

   ori   a0,zero,$9
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$2c

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,FT4.cba(s4)
   lhu   v1,FT4.tsb(s4)
   
   sh    v0,PolyFT4.cba(s2)
   andi  v1,v1,$ff9f
   ori   v1,v1,$0020
   sh    v1,PolyFT4.tsb(s2)

; Set uv coords

   lhu   a0,FT4.u0(s4)
   lhu   a1,FT4.u1(s4)
   lhu   a2,FT4.u2(s4)
   lhu   a3,FT4.u3(s4)
   
   sh    a0,PolyFT4.uv0(s2)
   sh    a1,PolyFT4.uv1(s2)
   sh    a2,PolyFT4.uv2(s2)
   sh    a3,PolyFT4.uv3(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,FT4.colour+$2(s4)
   nop
   swl   a0,PolyFT4.colour+$2(s2)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   lhu   a3,RTPN.depth(t3)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyFT4.sizeof

@End:

   addi  s4,s4,FT4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED TRIANGLES ****

@NoClipG3:

   bne   v0,v1,@NoClipG4
   addi  v1,zero,TypeG4

   module

; Get coords

   lh    t0,G3.coords+$0(s4)
   lh    t1,G3.coords+$2(s4)
   lh    t2,G3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get polygon flags

   lhu   v0,G3.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyG3.xy0(s2)
   sw    t5,PolyG3.xy1(s2)
   sw    t6,PolyG3.xy2(s2)

; SetPolyG3
 
   ori   a0,zero,$6
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$30

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,G3.colour0+$2(s4)
   lw    a1,G3.colour1(s4)
   lw    a2,G3.colour2(s4)

   swl   a0,PolyG3.colour0+$2(s2)
   sw    a1,PolyG3.colour1(s2)
   sw    a2,PolyG3.colour2(s2)

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
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyG3.sizeof

@End:

   addi  s4,s4,G3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED QUADS ****

@NoClipG4:

   bne   v0,v1,@NoClipF3
   addi  v1,zero,TypeF3

   module

; Get coords

   lh    t0,G4.coords+$0(s4)
   lh    t1,G4.coords+$2(s4)
   lh    t2,G4.coords+$4(s4)
   lh    t3,G4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get polygon flags

   lhu   v0,G4.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyG4.xy0(s2)
   sw    t5,PolyG4.xy1(s2)
   sw    t6,PolyG4.xy2(s2)
   sw    t7,PolyG4.xy3(s2)

; SetPolyG4

   ori   a0,zero,$8
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$38

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,G4.colour0+$2(s4)
   lw    a1,G4.colour1(s4)
   lw    a2,G4.colour2(s4)
   lw    a3,G4.colour3(s4)

   swl   a0,PolyG4.colour0+$2(s2)
   sw    a1,PolyG4.colour1(s2)
   sw    a2,PolyG4.colour2(s2)
   sw    a3,PolyG4.colour3(s2)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   lhu   a3,RTPN.depth(t3)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyG4.sizeof

@End:

   addi  s4,s4,G4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** FLAT SHADED TRIANGLES ****

@NoClipF3:

   bne   v0,v1,@NoClipF4
   addi  v1,zero,TypeF4

   module

; Get coords

   lh    t0,F3.coords+$0(s4)
   lh    t1,F3.coords+$2(s4)
   lh    t2,F3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get polygon flags

   lhu   v0,F3.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyF3.xy0(s2)
   sw    t5,PolyF3.xy1(s2)
   sw    t6,PolyF3.xy2(s2)

; SetPolyF3
 
   ori   a0,zero,$4
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$20

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,F3.colour+$2(s4)
   nop
   swl   a0,PolyF3.colour+$2(s2)

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
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyF3.sizeof

@End:

   addi  s4,s4,F3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** FLAT SHADED QUADS ****

@NoClipF4:

   bne   v0,v1,@NoClipTSPR
   addi  v1,zero,TypeTSPR

   module

; Get coords

   lh    t0,F4.coords+$0(s4)
   lh    t1,F4.coords+$2(s4)
   lh    t2,F4.coords+$4(s4)
   lh    t3,F4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get polygon flags

   lhu   v0,F4.flag(s4)

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection ?

   andi  v1,v0,SingleSided
   beq   v1,zero,@Visible
   nop

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
   nop
   nop
   cop2  $140006  ; NormalClip
   mfc2  a0,r24
   nop
   blez  a0,@End
   nop

@Visible:

; Store screen coords

   sw    t4,PolyF4.xy0(s2)
   sw    t5,PolyF4.xy1(s2)
   sw    t6,PolyF4.xy2(s2)
   sw    t7,PolyF4.xy3(s2)

; SetPolyF4

   ori   a0,zero,$5
   sb    a0,$3(s2)

   andi  v1,v0,Translucent
   beq   v1,zero,@Trans
   ori   a0,zero,$28

   ori   a0,a0,$2

@Trans:

   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,F4.colour+$2(s4)
   nop
   swl   a0,PolyF4.colour+$2(s2)

; Get depth

   lhu   a0,RTPN.depth(t0)
   lhu   a1,RTPN.depth(t1)
   lhu   a2,RTPN.depth(t2)
   lhu   a3,RTPN.depth(t3)
   mtc2  a0,r16
   mtc2  a1,r17
   mtc2  a2,r18
   mtc2  a3,r19
   nop
   cop2  $168002e    ; AverageZ4
   mfc2  v0,r7
;  nop

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyF4.sizeof

@End:

   addi  s4,s4,F4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!







@NoClipTSPR:

   bne   v0,v1,@NoClipBSPR
   addi  v1,zero,TypeBSPR

   module

; Get coord

   lh    t0,SPR.coord+$0(s4)
   nop

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3

; Convert indices to addresses

   add   t0,t0,fp

; Get flags

   lh    v0,RTPN.flags(t0)
   nop

; Test flags

   bne   v0,zero,@End
   nop

; Get sprite size

   lh    v0,SPR.width+$0(s4)  ; sprite width
   lh    v1,SPR.height+$0(s4) ; sprite height
   addi  t4,zero,160
   multu v0,t4
   mflo  t5             ; width * 160
   nop
   nop
   multu v1,t4
   mflo  t6             ; height * 160

; calculate xsize and ysize

   lhu   t4,RTPN.depth(t0)
   nop
   divu  t5,t4
   mflo  t5             ; t5 : xsize
   nop
   nop
   divu  t6,t4
   mflo  t6             ; t6 : ysize

; calculate xsize/2 and ysize/2

   sra    t4,t5,$1      ; t4 : xsize / 2
   sra    t7,t6,$1      ; t7 : ysize / 2

; calc rcos and rsin

   move  a0,s3

   jal   rcos           ; returned in v0
   nop
   multu v0,t4
   mflo  t7
   sra   t7,t7,$c       ; t7 offsetX


   move  a0,s3

   jal   rsin           ; returned in v0
   nop
   multu v0,t4
   mflo  t4
   sra   t4,t4,$c       ; t4 offsetY


   addi  a1,zero,1024
   sub   a0,a1,s3

   jal   rcos           ; returned in v0
   nop
   multu v0,t6
   mflo  t5
   sra   t5,t5,$c       ; t5 offset2X

   addi  a1,zero,1024
   sub   a0,a1,s3

   jal   rsin           ; returned in v0
   nop
   multu v0,t6
   mflo  t6
   sra   t6,t6,$c       ; t6 offset2Y

; Get screen coordinates of top middle

   lw    v0,RTPN.screen(t0)
   nop
   move  v1,v0
   andi  v0,v0,$ffff    ; v0 : x screen
   sra   v1,v1,$10      ; v1 : y screen

; work out the bounding sprite box

   sub   a0,v0,t7       ; tlX
   sub   a1,v1,t4       ; tlY
   move  a2,a0          ; store tlX
   move  a3,a1          ; store tlY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy0(s2)


   sub   a0,a2,t5       ; blX    
   add   a1,a3,t6       ; blY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy2(s2)


   add   a0,v0,t7       ; trX
   add   a1,v1,t4       ; trY
   move  a2,a0          ; store trX
   move  a3,a1          ; store trY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy1(s2)


   sub   a0,a2,t5       ; brX
   add   a1,a3,t6       ; brY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy3(s2)

; Get colour

   lw    t4,SPR.colour(s4)
   nop

   sh    t4,PolyFT4.colour+$0(s2)
   srl   t4,t4,$10
   sb    t4,PolyFT4.colour+$2(s2)

; poke in the texture coords

   lh    a0,SPR.texture+$0(s4)   ; get texture index into texture table
   lui   t4,TextureTable>>$10
   ori   t4,t4,TextureTable&$ffff
   sll   a0,a0,$2
   add   a0,a0,t4
   lw    a3,$0(a0)
   nop

; SetPolyFT4

   ori   v0,zero,$9
   sb    v0,$3(s2)

   ori   v0,zero,$2c
   sb    v0,$7(s2)

; tsb, cba

   lh    a2,$2(a3)
   nop
   sh    a2,PolyFT4.tsb(s2)

   lh    a2,$4(a3)
   nop
   sh    a2,PolyFT4.cba(s2)

; uv0

   lh    a2,$16(a3)
   nop
   sb    a2,PolyFT4.uv0(s2)

   lh    a2,$18(a3)
   nop
   sb    a2,PolyFT4.uv0+$1(s2)

; uv1

   lh    a2,$1a(a3)
   nop
   sb    a2,PolyFT4.uv1(s2)

   lh    a2,$1c(a3)
   nop
   sb    a2,PolyFT4.uv1+$1(s2)

; uv2

   lh    a2,$1e(a3)
   nop
   sb    a2,PolyFT4.uv2(s2)

   lh    a2,$20(a3)
   nop
   sb    a2,PolyFT4.uv2+$1(s2)

; uv3

   lh    a2,$22(a3)
   nop
   sb    a2,PolyFT4.uv3(s2)

   lh    a2,$24(a3)
   nop
   sb    a2,PolyFT4.uv3+$1(s2)

; Get depth ( 1 )

   lhu   v0,RTPN.depth(t0)

; AddPrim

   move  a0,s2       ; Get poly address
   andi  v0,v0,$fffc ; Mask out bottom 2 bits
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

   addi  s2,s2,PolyFT4.sizeof

@End:

   addi  s4,s4,SPR.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!






@NoClipBSPR:

   bne   v0,v1,@NoClipBadPoly
   addi  v1,zero,-$1

   module

; Get coord

   lh    t0,SPR.coord+$0(s4)
   nop

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3

; Convert indices to addresses

   add   t0,t0,fp

; Get flags

   lh    v0,RTPN.flags(t0)
   nop

; Test flags

   bne   v0,zero,@End
   nop

; Get sprite size

   lh    v0,SPR.width+$0(s4)  ; sprite width
   lh    v1,SPR.height+$0(s4) ; sprite height
   addi  t4,zero,160
   multu v0,t4
   mflo  t5             ; width * 160
   nop
   nop
   multu v1,t4
   mflo  t6             ; height * 160

; calculate xsize and ysize

   lhu   t4,RTPN.depth(t0)
   nop
   divu  t5,t4
   mflo  t5             ; t5 : xsize
   nop
   nop
   divu  t6,t4
   mflo  t6             ; t6 : ysize

; calculate xsize/2 and ysize/2

   sra    t4,t5,$1      ; t4 : xsize / 2
   sra    t7,t6,$1      ; t7 : ysize / 2

; calc rcos and rsin

   move  a0,s3

   jal   rcos           ; returned in v0
   nop
   multu v0,t4
   mflo  t7
   sra   t7,t7,$c       ; t7 offsetX


   move  a0,s3

   jal   rsin           ; returned in v0
   nop
   multu v0,t4
   mflo  t4
   sra   t4,t4,$c       ; t4 offsetY


   addi  a1,zero,1024
   sub   a0,a1,s3

   jal   rcos           ; returned in v0
   nop
   multu v0,t6
   mflo  t5
   sra   t5,t5,$c       ; t5 offset2X

   addi  a1,zero,1024
   sub   a0,a1,s3

   jal   rsin           ; returned in v0
   nop
   multu v0,t6
   mflo  t6
   sra   t6,t6,$c       ; t6 offset2Y

; Get screen coordinates of top middle

   lw    v0,RTPN.screen(t0)
   nop
   move  v1,v0
   andi  v0,v0,$ffff    ; v0 : x screen
   sra   v1,v1,$10      ; v1 : y screen

; work out the bounding sprite box

   sub   a0,v0,t7       ; blX
   sub   a1,v1,t4       ; blY
   move  a2,a0          ; store blX
   move  a3,a1          ; store blY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy2(s2)


   add   a0,a2,t5       ; tlX    
   sub   a1,a3,t6       ; tlY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy0(s2)


   add   a0,v0,t7       ; brX
   add   a1,v1,t4       ; brY
   move  a2,a0          ; store brX
   move  a3,a1          ; store brY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy3(s2)


   add   a0,a2,t5       ; trX
   sub   a1,a3,t6       ; trY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy1(s2)

; Get colour

   lw    t4,SPR.colour(s4)
   nop

   sh    t4,PolyFT4.colour+$0(s2)
   srl   t4,t4,$10
   sb    t4,PolyFT4.colour+$2(s2)

; poke in the texture coords

   lh    a0,SPR.texture+$0(s4)   ; get texture index into texture table
   lui   t4,TextureTable>>$10
   ori   t4,t4,TextureTable&$ffff
   sll   a0,a0,$2
   add   a0,a0,t4
   lw    a3,$0(a0)
   nop

; SetPolyFT4

   ori   v0,zero,$9
   sb    v0,$3(s2)

   ori   v0,zero,$2c
   sb    v0,$7(s2)

; tsb, cba

   lh    a2,$2(a3)
   nop
   sh    a2,PolyFT4.tsb(s2)

   lh    a2,$4(a3)
   nop
   sh    a2,PolyFT4.cba(s2)

; uv0

   lh    a2,$16(a3)
   nop
   sb    a2,PolyFT4.uv0(s2)

   lh    a2,$18(a3)
   nop
   sb    a2,PolyFT4.uv0+$1(s2)

; uv1

   lh    a2,$1a(a3)
   nop
   sb    a2,PolyFT4.uv1(s2)

   lh    a2,$1c(a3)
   nop
   sb    a2,PolyFT4.uv1+$1(s2)

; uv2

   lh    a2,$1e(a3)
   nop
   sb    a2,PolyFT4.uv2(s2)

   lh    a2,$20(a3)
   nop
   sb    a2,PolyFT4.uv2+$1(s2)

; uv3

   lh    a2,$22(a3)
   nop
   sb    a2,PolyFT4.uv3(s2)

   lh    a2,$24(a3)
   nop
   sb    a2,PolyFT4.uv3+$1(s2)

; Get depth ( 1 )

   lhu   v0,RTPN.depth(t0)

; AddPrim

   move  a0,s2       ; Get poly address
   andi  v0,v0,$fffc ; Mask out bottom 2 bits
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

   addi  s2,s2,PolyFT4.sizeof

@End:

   addi  s4,s4,SPR.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!








; **** ANY OTHER POLY TYPES GO HERE ****



; **** UNRECOGNIZED POLY TYPE ****

@NoClipBadPoly:

   bgtz  v0,AsmDrawObjectEnd
   nop                     ; Error result is primitive type

   j     AsmDrawObjectEnd
   addi  v0,zero,-$1       ; Error result is -1



; **** NEXT POLYGON ****

@NoClipNext:

   bgtz  s5,@NoClipLoop
   nop

   j     AsmDrawObjectOk
   nop










; Restore registers and unlink stack space

AsmDrawObjectOk:

; Success result is zero

   addi  v0,zero,$0        

AsmDrawObjectEnd:

; Store updated pointer to primitive buffers

   opt at+

   sw    s2,prims

   opt at-


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

; AsmDrawObject End
























































; PARAMETERS

; a0 : Object*       object
; a1 : Skeleton*     camera
; a2 : long          fixed depth

; REGISTER ASSIGNMENT

; s0 : object ptr
; s1 : camera ptr
; s2 : poly ptr
; s3 : fixed depth
; s4 : primitive ptr
; s5 : primitive count
; s6 : CurrentScreen
; s7 : OT ptr

; t0 : 
; t1 : 
; t2 : 
; t3 : 
; t4 :
; t5 :
; t6 :
; t7 :
; t8 : vertex ptr   / AddPrim mask $00ffffff
; t9 : vertex count / AddPrim mask $ff000000

; fp : RTPN ptr
; gp : normal ptr    * temp *
; at :
; k0 :
; k1 :

AsmDrawObjectFixed: 

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

   move  s0,a0
   move  s1,a1
   move  s3,a2

; Set the RotTrans matrix

   addiu sp,sp,-$4

   lw    a0,Object.skeleton(s0)
   jal   SetSkeletonTransform
   nop

   addiu sp,sp,$4

; Get vertex pointer and counter

   lw    t8,Object.vertices(s0)
   lhu   t9,Object.vertexCount(s0)

; Get pointer to primitive buffers

   lw    s2,prims

; Get primitive pointer, counter and index

   lw    s4,Object.primitives(s0)
   lhu   s5,Object.primitiveCount(s0)

; Get normal pointer

   lw    gp,Object.normals(s0)

; Pointer to the OT

   lhu   s6,CurrentScreen

   lui   v0,OT>>16
   ori   v0,OT&$ffff

   sll   v1,s6,$2
   add   v0,v0,v1

   lw    s7,0(v0)

; If vertex count < 128 use fast ram else use normal ram

   addi  v0,t9,-$80
   blez  v0,@FastRam

   lui   fp,$1f80          ; Branch delay slot!

@SlowRam:

   lui   fp,SlowMem>>16
   ori   fp,SlowMem&$ffff

@FastRam:

; RotTransPersN

   move  a0,t8                   ; vertex ptr
   move  a1,fp                   ; rtpn ptr

   addi  v0,zero,$0              ; all good flags
   addi  v1,zero,$1              ; all bad flags

@RotTransPersN
   
   lwc2  zero,$0(a0)             ; load svector
   lwc2  at,$4(a0)               ; load svector
   addi  a0,a0,SVECTOR.sizeof    ; increment svector ptr
   cop2  $180001                 ; RotTransPers
   
   mfc2  a2,r19                  ; get depth
   cfc2  a3,r31                  ; get flags

   swc2  t6,RTPN.screen(a1)      ; store screen coords
  
   sh    a2,RTPN.depth(a1)       ; store depth

   srl   a3,a3,$c                ; short flags
   andi  a3,a3,ShortClipFlags    ; clip flags
   sh    a3,RTPN.flags(a1)       ; store flags
   
   sltu  a2,zero,a3              ; good / bad flag
   or    v0,v0,a3                ; accumulate all good flag
   and   v1,v1,a3                ; accumulate all bad flag
   
   addi  t9,t9,-$1               ; decrement ...
   bgtz  t9,@RotTransPersN       ; ... and branch
   addi  a1,a1,RTPN.sizeof       ; increment rtpn ptr


; Test all bad cumulative flag

   bne   v1,zero,AsmDrawObjectFixedEnd   ; end if all bad
   nop


; Zero translation matrix ( for rotating normals )

   if 0

   ctc2  zero,r5
   ctc2  zero,r6
   ctc2  zero,r7

   endif


; Test all good cumulative flag and set AddPrim masks

   lui   t8,$00ff
   ori   t8,$ffff

;  beq   v0,zero,@NoClip         ; don't clip if all good

   lui   t9,$ff00                ; Branch delay slot!


; **** NEED TO CHECK FLAGS SINCE SOME ARE BAD ****

@Clip:

; Fill in the gpu packets

@ClipLoop:

   lhu   v0,F3.type(s4)
   addi  v1,zero,TypeGT3



; **** GOURAUD TEXTURED TRIANGLES ****

@ClipGT3:

   bne   v0,v1,@ClipGT4
   addi  v1,zero,TypeGT4

   module

; Get coords

   lh    t0,GT3.coords+$0(s4)
   lh    t1,GT3.coords+$2(s4)
   lh    t2,GT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   bne   v0,zero,@End
   nop

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyGT3.xy0(s2)      ; Delay slot!
   sw    t5,PolyGT3.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT3.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyGT3.xy0(s2)
;  sw    t5,PolyGT3.xy1(s2)
;  sw    t6,PolyGT3.xy2(s2)

; SetPolyGT3
 
   ori   a0,zero,$9
   sb    a0,$3(s2)

   ori   a0,zero,$34
   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,GT3.cba(s4)
   lhu   v1,GT3.tsb(s4)
   
   sh    v0,PolyGT3.cba(s2)
   sh    v1,PolyGT3.tsb(s2)

; Set uv coords

   lhu   a0,GT3.u0(s4)
   lhu   a1,GT3.u1(s4)
   lhu   a2,GT3.u2(s4)
   
   sh    a0,PolyGT3.uv0(s2)
   sh    a1,PolyGT3.uv1(s2)
   sh    a2,PolyGT3.uv2(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,GT3.colour0+$2(s4)
   lw    a1,GT3.colour1(s4)
   lw    a2,GT3.colour2(s4)

   swl   a0,PolyGT3.colour0+$2(s2)
   sw    a1,PolyGT3.colour1(s2)
   sw    a2,PolyGT3.colour2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyGT3.sizeof

@End:

   addi  s4,s4,GT3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD TEXTURED QUADS ****

@ClipGT4:

   bne   v0,v1,@ClipFT3
   addi  v1,zero,TypeFT3

   module

; Get coords

   lh    t0,GT4.coords+$0(s4)
   lh    t1,GT4.coords+$2(s4)
   lh    t2,GT4.coords+$4(s4)
   lh    t3,GT4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   bne   v0,zero,@End
   nop

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyGT4.xy0(s2)      ; Delay slot!
   sw    t5,PolyGT4.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT4.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyGT4.xy3(s2)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyGT4.xy0(s2)
;  sw    t5,PolyGT4.xy1(s2)
;  sw    t6,PolyGT4.xy2(s2)
;  sw    t7,PolyGT4.xy3(s2)

; SetPolyGT4

   ori   a0,zero,$c
   sb    a0,$3(s2)

   ori   a0,zero,$3c
   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,GT4.cba(s4)
   lhu   v1,GT4.tsb(s4)
   
   sh    v0,PolyGT4.cba(s2)
   sh    v1,PolyGT4.tsb(s2)

; Set uv coords

   lhu   a0,GT4.u0(s4)
   lhu   a1,GT4.u1(s4)
   lhu   a2,GT4.u2(s4)
   lhu   a3,GT4.u3(s4)
   
   sh    a0,PolyGT4.uv0(s2)
   sh    a1,PolyGT4.uv1(s2)
   sh    a2,PolyGT4.uv2(s2)
   sh    a3,PolyGT4.uv3(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,GT4.colour0+$2(s4)
   lw    a1,GT4.colour1(s4)
   lw    a2,GT4.colour2(s4)
   lw    a3,GT4.colour3(s4)

   swl   a0,PolyGT4.colour0+$2(s2)
   sw    a1,PolyGT4.colour1(s2)
   sw    a2,PolyGT4.colour2(s2)
   sw    a3,PolyGT4.colour3(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyGT4.sizeof

@End:

   addi  s4,s4,GT4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED TRIANGLES ****

@ClipFT3:

   bne   v0,v1,@ClipFT4
   addi  v1,zero,TypeFT4

   module

; Get coords

   lh    t0,FT3.coords+$0(s4)
   lh    t1,FT3.coords+$2(s4)
   lh    t2,FT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   bne   v0,zero,@End
   nop

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyFT3.xy0(s2)      ; Delay slot!
   sw    t5,PolyFT3.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT3.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyFT3.xy0(s2)
;  sw    t5,PolyFT3.xy1(s2)
;  sw    t6,PolyFT3.xy2(s2)

; SetPolyFT3
 
   ori   a0,zero,$7
   sb    a0,$3(s2)

   ori   a0,zero,$24
   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,FT3.cba(s4)
   lhu   v1,FT3.tsb(s4)
   
   sh    v0,PolyFT3.cba(s2)
   sh    v1,PolyFT3.tsb(s2)

; Set uv coords

   lhu   a0,FT3.u0(s4)
   lhu   a1,FT3.u1(s4)
   lhu   a2,FT3.u2(s4)
   
   sh    a0,PolyFT3.uv0(s2)
   sh    a1,PolyFT3.uv1(s2)
   sh    a2,PolyFT3.uv2(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,FT3.colour+$2(s4)
   nop
   swl   a0,PolyFT3.colour+$2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyFT3.sizeof

@End:

   addi  s4,s4,FT3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED QUADS ****

@ClipFT4:

   bne   v0,v1,@ClipG3
   addi  v1,zero,TypeG3

   module

; Get coords

   lh    t0,FT4.coords+$0(s4)
   lh    t1,FT4.coords+$2(s4)
   lh    t2,FT4.coords+$4(s4)
   lh    t3,FT4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   bne   v0,zero,@End
   nop

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyFT4.xy0(s2)      ; Delay slot!
   sw    t5,PolyFT4.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT4.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyFT4.xy3(s2)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyFT4.xy0(s2)
;  sw    t5,PolyFT4.xy1(s2)
;  sw    t6,PolyFT4.xy2(s2)
;  sw    t7,PolyFT4.xy3(s2)

; SetPolyFT4

   ori   a0,zero,$9
   sb    a0,$3(s2)

   ori   a0,zero,$2c
   sb    a0,$7(s2)

; Set cba, tsb

   lhu   v0,FT4.cba(s4)
   lhu   v1,FT4.tsb(s4)
   
   sh    v0,PolyFT4.cba(s2)
   sh    v1,PolyFT4.tsb(s2)

; Set uv coords

   lhu   a0,FT4.u0(s4)
   lhu   a1,FT4.u1(s4)
   lhu   a2,FT4.u2(s4)
   lhu   a3,FT4.u3(s4)
   
   sh    a0,PolyFT4.uv0(s2)
   sh    a1,PolyFT4.uv1(s2)
   sh    a2,PolyFT4.uv2(s2)
   sh    a3,PolyFT4.uv3(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,FT4.colour+$2(s4)
   nop
   swl   a0,PolyFT4.colour+$2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyFT4.sizeof

@End:

   addi  s4,s4,FT4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED TRIANGLES ****

@ClipG3:

   bne   v0,v1,@ClipG4
   addi  v1,zero,TypeG4

   module

; Get coords

   lh    t0,G3.coords+$0(s4)
   lh    t1,G3.coords+$2(s4)
   lh    t2,G3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   bne   v0,zero,@End
   nop

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyG3.xy0(s2)      ; Delay slot!
   sw    t5,PolyG3.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyG3.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyG3.xy0(s2)
;  sw    t5,PolyG3.xy1(s2)
;  sw    t6,PolyG3.xy2(s2)

; SetPolyG3
 
   ori   a0,zero,$6
   sb    a0,$3(s2)

   ori   a0,zero,$30
   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,G3.colour0+$2(s4)
   lw    a1,G3.colour1(s4)
   lw    a2,G3.colour2(s4)

   swl   a0,PolyG3.colour0+$2(s2)
   sw    a1,PolyG3.colour1(s2)
   sw    a2,PolyG3.colour2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyG3.sizeof

@End:

   addi  s4,s4,G3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED QUADS ****

@ClipG4:

   bne   v0,v1,@ClipF3
   addi  v1,zero,TypeF3

   module

; Get coords

   lh    t0,G4.coords+$0(s4)
   lh    t1,G4.coords+$2(s4)
   lh    t2,G4.coords+$4(s4)
   lh    t3,G4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   bne   v0,zero,@End
   nop

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyG4.xy0(s2)       ; Delay slot!
   sw    t5,PolyG4.xy1(s2)       ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyG4.xy2(s2)       ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyG4.xy3(s2)       ; Delay slot!

; Store screen coords

;  sw    t4,PolyG4.xy0(s2)
;  sw    t5,PolyG4.xy1(s2)
;  sw    t6,PolyG4.xy2(s2)
;  sw    t7,PolyG4.xy3(s2)

; SetPolyG4

   ori   a0,zero,$8
   sb    a0,$3(s2)

   ori   a0,zero,$38
   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,G4.colour0+$2(s4)
   lw    a1,G4.colour1(s4)
   lw    a2,G4.colour2(s4)
   lw    a3,G4.colour3(s4)

   swl   a0,PolyG4.colour0+$2(s2)
   sw    a1,PolyG4.colour1(s2)
   sw    a2,PolyG4.colour2(s2)
   sw    a3,PolyG4.colour3(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyG4.sizeof

@End:

   addi  s4,s4,G4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT SHADED TRIANGLES ****

@ClipF3:

   bne   v0,v1,@ClipF4
   addi  v1,zero,TypeF4

   module

; Get coords

   lh    t0,F3.coords+$0(s4)
   lh    t1,F3.coords+$2(s4)
   lh    t2,F3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   bne   v0,zero,@End
   nop

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyF3.xy0(s2)      ; Delay slot!
   sw    t5,PolyF3.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyF3.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyF3.xy0(s2)
;  sw    t5,PolyF3.xy1(s2)
;  sw    t6,PolyF3.xy2(s2)

; SetPolyF3
 
   ori   a0,zero,$4
   sb    a0,$3(s2)

   ori   a0,zero,$20
   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,F3.colour+$2(s4)
   nop
   swl   a0,PolyF3.colour+$2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyF3.sizeof

@End:

   addi  s4,s4,F3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT SHADED QUADS ****

@ClipF4:

   bne   v0,v1,@ClipBadPoly
   addi  v1,zero,-$1

   module

; Get coords

   lh    t0,F4.coords+$0(s4)
   lh    t1,F4.coords+$2(s4)
   lh    t2,F4.coords+$4(s4)
   lh    t3,F4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get vertex flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test vertex flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   bne   v0,zero,@End
   nop

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyF4.xy0(s2)       ; Delay slot!
   sw    t5,PolyF4.xy1(s2)       ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyF4.xy2(s2)       ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyF4.xy3(s2)       ; Delay slot!

; Store screen coords

;  sw    t4,PolyF4.xy0(s2)
;  sw    t5,PolyF4.xy1(s2)
;  sw    t6,PolyF4.xy2(s2)
;  sw    t7,PolyF4.xy3(s2)

; SetPolyF4

   ori   a0,zero,$5
   sb    a0,$3(s2)

   ori   a0,zero,$28
   sb    a0,$7(s2)

; Set colour ( lwl/swl colour0 so we don't trash code field )

   lwl   a0,F4.colour+$2(s4)
   nop
   swl   a0,PolyF4.colour+$2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyF4.sizeof

@End:

   addi  s4,s4,F4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** ANY OTHER POLY TYPES GO HERE ****



; **** UNRECOGNIZED POLY TYPE ****

@ClipBadPoly:

   bgtz  v0,AsmDrawObjectFixedEnd
   nop                     ; Error result is primitive type

   j     AsmDrawObjectFixedEnd
   addi  v0,zero,-$1       ; Error result is -1



; **** NEXT POLYGON ****

@ClipNext:

   bgtz  s5,@ClipLoop
   nop

   j     AsmDrawObjectFixedOk
   nop





; **** ALL FLAGS GOOD SO NO CHECKS NEEDED ****


@NoClip:

; Fill in the gpu packets

@NoClipLoop:

   lhu   v0,F3.type(s4)
   addi  v1,zero,TypeGT3



; **** GOURAUD TEXTURED TRIANGLES ****

@NoClipGT3:

   bne   v0,v1,@NoClipGT4
   addi  v1,zero,TypeGT4

   module

; Get coords

   lh    t0,GT3.coords+$0(s4)
   lh    t1,GT3.coords+$2(s4)
   lh    t2,GT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyGT3.xy0(s2)      ; Delay slot!
   sw    t5,PolyGT3.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT3.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyGT3.xy0(s2)
;  sw    t5,PolyGT3.xy1(s2)
;  sw    t6,PolyGT3.xy2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyGT3.sizeof

@End:

   addi  s4,s4,GT3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD TEXTURED QUADS ****

@NoClipGT4:

   bne   v0,v1,@NoClipFT3
   addi  v1,zero,TypeFT3

   module

; Get coords

   lh    t0,GT4.coords+$0(s4)
   lh    t1,GT4.coords+$2(s4)
   lh    t2,GT4.coords+$4(s4)
   lh    t3,GT4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyGT4.xy0(s2)      ; Delay slot!
   sw    t5,PolyGT4.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT4.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyGT4.xy3(s2)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyGT4.xy0(s2)
;  sw    t5,PolyGT4.xy1(s2)
;  sw    t6,PolyGT4.xy2(s2)
;  sw    t7,PolyGT4.xy3(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyGT4.sizeof

@End:

   addi  s4,s4,GT4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED TRIANGLES ****

@NoClipFT3:

   bne   v0,v1,@NoClipFT4
   addi  v1,zero,TypeFT4

   module

; Get coords

   lh    t0,FT3.coords+$0(s4)
   lh    t1,FT3.coords+$2(s4)
   lh    t2,FT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyFT3.xy0(s2)      ; Delay slot!
   sw    t5,PolyFT3.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT3.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyFT3.xy0(s2)
;  sw    t5,PolyFT3.xy1(s2)
;  sw    t6,PolyFT3.xy2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyFT3.sizeof

@End:

   addi  s4,s4,FT3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED QUADS ****

@NoClipFT4:

   bne   v0,v1,@NoClipG3
   addi  v1,zero,TypeG3

   module

; Get coords

   lh    t0,FT4.coords+$0(s4)
   lh    t1,FT4.coords+$2(s4)
   lh    t2,FT4.coords+$4(s4)
   lh    t3,FT4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyFT4.xy0(s2)      ; Delay slot!
   sw    t5,PolyFT4.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT4.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyFT4.xy3(s2)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyFT4.xy0(s2)
;  sw    t5,PolyFT4.xy1(s2)
;  sw    t6,PolyFT4.xy2(s2)
;  sw    t7,PolyFT4.xy3(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyFT4.sizeof

@End:

   addi  s4,s4,FT4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED TRIANGLES ****

@NoClipG3:

   bne   v0,v1,@NoClipG4
   addi  v1,zero,TypeG4

   module

; Get coords

   lh    t0,G3.coords+$0(s4)
   lh    t1,G3.coords+$2(s4)
   lh    t2,G3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyG3.xy0(s2)      ; Delay slot!
   sw    t5,PolyG3.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyG3.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyG3.xy0(s2)
;  sw    t5,PolyG3.xy1(s2)
;  sw    t6,PolyG3.xy2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyG3.sizeof

@End:

   addi  s4,s4,G3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED QUADS ****

@NoClipG4:

   bne   v0,v1,@NoClipF3
   addi  v1,zero,TypeF3

   module

; Get coords

   lh    t0,G4.coords+$0(s4)
   lh    t1,G4.coords+$2(s4)
   lh    t2,G4.coords+$4(s4)
   lh    t3,G4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyG4.xy0(s2)       ; Delay slot!
   sw    t5,PolyG4.xy1(s2)       ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyG4.xy2(s2)       ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyG4.xy3(s2)       ; Delay slot!

; Store screen coords

;  sw    t4,PolyG4.xy0(s2)
;  sw    t5,PolyG4.xy1(s2)
;  sw    t6,PolyG4.xy2(s2)
;  sw    t7,PolyG4.xy3(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyG4.sizeof

@End:

   addi  s4,s4,G4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** FLAT SHADED TRIANGLES ****

@NoClipF3:

   bne   v0,v1,@NoClipF4
   addi  v1,zero,TypeF4

   module

; Get coords

   lh    t0,F3.coords+$0(s4)
   lh    t1,F3.coords+$2(s4)
   lh    t2,F3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyF3.xy0(s2)      ; Delay slot!
   sw    t5,PolyF3.xy1(s2)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyF3.xy2(s2)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyF3.xy0(s2)
;  sw    t5,PolyF3.xy1(s2)
;  sw    t6,PolyF3.xy2(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyF3.sizeof

@End:

   addi  s4,s4,F3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** FLAT SHADED QUADS ****

@NoClipF4:

   bne   v0,v1,@NoClipBadPoly
   addi  v1,zero,-$1

   module

; Get coords

   lh    t0,F4.coords+$0(s4)
   lh    t1,F4.coords+$2(s4)
   lh    t2,F4.coords+$4(s4)
   lh    t3,F4.coords+$6(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp
   add   t3,t3,fp

; Get screen coordinates

   lw    t4,RTPN.screen(t0)
   lw    t5,RTPN.screen(t1)
   lw    t6,RTPN.screen(t2)
   lw    t7,RTPN.screen(t3)

; Back face rejection

   mtc2  t4,r12
   mtc2  t5,r13
   mtc2  t6,r14
;  nop
;  nop
   sw    t4,PolyF4.xy0(s2)       ; Delay slot!
   sw    t5,PolyF4.xy1(s2)       ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyF4.xy2(s2)       ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyF4.xy3(s2)       ; Delay slot!

; Store screen coords

;  sw    t4,PolyF4.xy0(s2)
;  sw    t5,PolyF4.xy1(s2)
;  sw    t6,PolyF4.xy2(s2)
;  sw    t7,PolyF4.xy3(s2)

; Get depth

   move  v0,s3

; AddPrim

   move  a0,s2       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   sll   v0,a0,$8    ; move low 3 bytes of prim ptr high
   lwl   v1,$2(a1)   ; load low 3 bytes of OT pointer high

   swl   v0,$2(a1)   ; primitive points to what OT used to
   swl   v1,$2(a0)   ; OT points to primitive


   addi  s2,s2,PolyF4.sizeof

@End:

   addi  s4,s4,F4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** ANY OTHER POLY TYPES GO HERE ****



; **** UNRECOGNIZED POLY TYPE ****

@NoClipBadPoly:

   bgtz  v0,AsmDrawObjectFixedEnd
   nop                     ; Error result is primitive type

   j     AsmDrawObjectFixedEnd
   addi  v0,zero,-$1       ; Error result is -1



; **** NEXT POLYGON ****

@NoClipNext:

   bgtz  s5,@NoClipLoop
   nop

   j     AsmDrawObjectFixedOk
   nop





; Restore registers and unlink stack space

AsmDrawObjectFixedOk:

; Success result is zero

   addi  v0,zero,$0        

AsmDrawObjectFixedEnd:

; Store updated pointer to primitive buffers

   opt at+

   sw    s2,prims

   opt at-


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

; AsmDrawObjectFixed End








