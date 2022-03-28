
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
   xref  TextureTable
   xref  rcos
   xref  rsin
   xref  SlowMem



   if 0


   xdef  AsmDrawLibObj
   xdef  AsmDrawObj

   include  asm.i


   endif






   if 0




; a0 : Object*       object
; a1 : Skeleton*     camera
; a2 : short         roll 

AsmDrawLibObj: 

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
   move  at,a1
   move  s1,a2

; Set the RotTrans matrix

   addiu sp,sp,-$4

   lw    a0,Object.skeleton(s0)
   jal   SetSkeletonTransform
   nop

   addiu sp,sp,$4

; Get vertex pointer and counter

   lw    s2,Object.vertices(s0)
   lhu   s3,Object.vertexCount(s0)

; Get primitive pointer, counter and index

   lw    v0,Object.libraryObject(s0)  
   nop

   lw    s4,LibObj.primitives(v0)     
   lhu   s5,LibObj.primitiveCount(v0)

   lhu   v0,LibObj.index(v0)
   nop

; Get shared primitive count

   lui   a0,sharedPrimCount>>16
   ori   a0,sharedPrimCount&$ffff

   sll   v1,v0,$1
   add   a0,a0,v1

   lhu   v1,$0(a0)   ; sharedPrimCount[ index ]
   nop

; Get shared primitives

   lui   a1,sharedPrims>>16
   ori   a1,sharedPrims&$ffff

   addi  a2,zero,MaxObjects
   sll   a2,a2,$2    ; MaxObjects * 4
   multu v0,a2
   mflo  a2          ; index * MaxObjects * 4
   add   a1,a1,a2

   sll   a2,v1,$2    ; sharedPrimCount[ index ] * 4
   add   a1,a1,a2

   lw    s6,$0(a1)   ; shared primitive pointer

; Increment sharedPrimCount[ index ] and store it back

   addi  v1,v1,$1
   sh    v1,$0(a0)

; Test if out of objects

   beq   s6,zero,AsmDrawLibObjEnd
   nop

; If vertex count < 120 use fast ram else use normal ram

   addi  v0,s3,-$78
   blez  v0,@FastRam

   lui   s7,$1f80          ; Branch delay slot!

@SlowRam:

   lui   s7,SlowMem>>16
   ori   s7,SlowMem&$ffff

@FastRam:

; Pointer to the OT

   lhu   t8,CurrentScreen

   lui   v0,OT>>16
   ori   v0,OT&$ffff

   sll   v1,t8,$2
   add   v0,v0,v1

   lw    t9,0(v0)

; RotTransPersN

   move  a0,s2
   move  a1,s7

   addi  gp,zero,$0

@RotTransPersN
   
   lwc2  zero,$0(a0)
   lwc2  at,$4(a0)
   addi  a0,a0,$8
   cop2  $180001              ; RotTransPers
   swc2  t6,RTPN.screen(a1)
   mfc2  a2,r19
   cfc2  a3,r31
   sh    a2,RTPN.depth(a1)
   srl   a3,a3,$c             ; optimise later!
   sh    a3,RTPN.flags(a1)
   or    gp,gp,a3
   addi  s3,s3,-$1
   bgtz  s3,@RotTransPersN
   addi  a1,a1,RTPN.sizeof    ; Branch delay slot!

   andi  gp,gp,ShortClipFlags

   lui   fp,$00ff
   ori   fp,$ffff

   beq   gp,zero,@NoClip
   lui   gp,$ff00          ; Branch delay slot!












@Clip:

; Fill in the gpu packets

@ClipLoop:

   lbu   v0,F3.type(s6)
   addi  v1,zero,TypeGT3

; **** GOURAUD TEXTURED TRIANGLES ****

@ClipGT3:

   bne   v0,v1,@ClipGT4
   addi  v1,zero,TypeGT4

   module

; Get coords

   lh    t0,GT3.coords+$0(s6)
   lh    t1,GT3.coords+$2(s6)
   lh    t2,GT3.coords+$4(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    a0,t4,t5
   or    a0,a0,t6
   andi  a0,a0,ShortClipFlags

   if 0
   bne   a0,zero,@Mesh
   nop
   else
   bne   a0,zero,@End
   nop
   endif

; Get PolyGT3

   beq   t8,zero,@GotPoly
   addi  s3,s6,GT3.tri     ; branch delay slot

; Get second PolyGT3

   addi  s3,s3,PolyGT3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT3.flag(s6)

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

   lwl   t4,LibGT3.colour0+$2(s4)
   lw    t5,LibGT3.colour1(s4)
   lw    t6,LibGT3.colour2(s4)

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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibGT3.sizeof
   addi  s6,s6,GT3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!








   if 0

; Mesh GT3

@Mesh:

; Don't mesh if all points bad

   andi  t4,t4,ShortClipFlags
   beq   t4,zero,@MeshOk

   andi  t5,t5,ShortClipFlags
   beq   t5,zero,@MeshOk

   andi  t6,t6,ShortClipFlags
   beq   t6,zero,@MeshOk

   nop
   j     @End
   nop

@MeshOk:

; Stack registers s0-s7

   addiu sp,sp,-$20
   sw    s0,$00(sp)
   sw    s1,$04(sp)
   sw    s2,$08(sp)
   sw    s3,$0c(sp)
   sw    s4,$10(sp)
   sw    s5,$14(sp)
   sw    s6,$18(sp)
   sw    s7,$1c(sp)

   move  t6,s2
   move  t7,s3

; s0 coord 0
; s1 coord 1
; s2 coord 2
; s3 coord 3
; s4 lib ptr         LibGT3*
; s5 prim count    
; s6 shared prim     GT3*
; s7 ram ptr         RTPN*

; t0
; t1
; t2
; t3
; t4 mesh poly ptr   PolyGT3*
; t5 fastram         Mesh*
; t6 vertex pointer  SVECTOR*
; t7 poly ptr        PolyGT3*


; Set mesh ptr and fast ram ptr

   lui   v0,MeshPtr>>16
   ori   v0,MeshPtr&$ffff
   lw    t4,$0(v0)

   lui   t5,$1f80
   ori   t5,$03c0

; Get coords

   lh    s0,GT3.coords+$0(s6)
   lh    s1,GT3.coords+$2(s6)
   lh    s2,GT3.coords+$4(s6)

; Multiply coords by 8 ( sizeof svector )

   sll   s0,s0,$3
   sll   s1,s1,$3
   sll   s2,s2,$3

; Add vertex base pointer

   add   s0,s0,t6
   add   s1,s1,t6
   add   s2,s2,t6

; Sub divide x

   lh    a0,$0(s0)
   lh    a1,$0(s1)
   lh    a2,$0(s2)

   add   v0,a0,a1
   sra   v0,v0,$1
   sh    v0,$00(t5)

   add   v0,a1,a2
   sra   v0,v0,$1
   sh    v0,$10(t5)

   add   v0,a2,a0
   sra   v0,v0,$1
   sh    v0,$20(t5)

; Sub divide y

   lh    a0,$2(s0)
   lh    a1,$2(s1)
   lh    a2,$2(s2)

   add   v0,a0,a1
   sra   v0,v0,$1
   sh    v0,$02(t5)

   add   v0,a1,a2
   sra   v0,v0,$1
   sh    v0,$12(t5)

   add   v0,a2,a0
   sra   v0,v0,$1
   sh    v0,$22(t5)

; Sub divide z

   lh    a0,$4(s0)
   lh    a1,$4(s1)
   lh    a2,$4(s2)

   add   v0,a0,a1
   sra   v0,v0,$1
   sh    v0,$04(t5)

   add   v0,a1,a2
   sra   v0,v0,$1
   sh    v0,$14(t5)

   add   v0,a2,a0
   sra   v0,v0,$1
   sh    v0,$24(t5)

; RotTransPersN

   move  a0,t5
   addi  v0,zero,$3

@MeshRTPN:
   
   lwc2  zero,Mesh.coords+$0(a0)
   lwc2  at,Mesh.coords+$4(a0)
   nop
   cop2  $180001              ; RotTransPers
   swc2  t6,Mesh.screen(a0)
   mfc2  a2,r19
   cfc2  a3,r31
   sh    a2,Mesh.depth(a0)
   srl   a3,a3,$c             ; optimise later!
   sh    a3,Mesh.flags(a0)
   addi  v0,v0,-$1
   bgtz  v0,@MeshRTPN
   addi  a0,a0,Mesh.sizeof    ; Branch delay slot!

; Get flags

   lh    a0,$00+Mesh.flags(t5)
   lh    a1,$10+Mesh.flags(t5)
   lh    a2,$20+Mesh.flags(t5)

   or    v0,a0,a1
   or    v0,a0,a2
   andi  v0,v0,ShortClipFlags
   bne   v0,zero,@MeshNext1
   nop

; Mesh centre triangle

   if 0

   lw    v0,LibGT3.colour0(s4)
   lw    v1,LibGT3.colour1(s4)
   lw    v2,LibGT3.colour2(s4)

   endif

; Colour

   lui   v0,$007f
   sw    v0,PolyGT3.colour0(t4)
   addiu v0,zero,$7f00
   sw    v0,PolyGT3.colour1(t4)
   addiu v0,zero,$007f
   sw    v0,PolyGT3.colour2(t4)

; GPU codes

   addi  v0,zero,$9
   sb    v0,PolyGT3.tag+$3(t4)

   addi  v0,zero,$34
   sb    v0,PolyGT3.colour0+$3(t4)

; tsb and cba

   lhu   v0,PolyFT3.cba(t7)
   lhu   v1,PolyFT3.tsb(t7)

   sh    v0,PolyFT3.cba(t4)
   sh    v1,PolyFT3.tsb(t4)

; U coords

   lbu   a0,PolyGT3.uv0+$0(t7)
   lbu   a1,PolyGT3.uv1+$0(t7)
   lbu   a2,PolyGT3.uv2+$0(t7)

   add   v0,a0,a1
   srl   v0,v0,$1
   sb    v0,PolyGT3.uv0+$0(t4)

   add   v0,a1,a2
   srl   v0,v0,$1
   sb    v0,PolyGT3.uv1+$0(t4)

   add   v0,a2,a0
   srl   v0,v0,$1
   sb    v0,PolyGT3.uv2+$0(t4)

; V coords

   lbu   a0,PolyGT3.uv0+$1(t7)
   lbu   a1,PolyGT3.uv1+$1(t7)
   lbu   a2,PolyGT3.uv2+$1(t7)

   add   v0,a0,a1
   srl   v0,v0,$1
   sb    v0,PolyGT3.uv0+$1(t4)

   add   v0,a1,a2
   srl   v0,v0,$1
   sb    v0,PolyGT3.uv1+$1(t4)

   add   v0,a2,a0
   srl   v0,v0,$1
   sb    v0,PolyGT3.uv2+$1(t4)

; Screen coords

   lw    a0,$00+Mesh.screen(t5)
   lw    a1,$10+Mesh.screen(t5)
   lw    a2,$20+Mesh.screen(t5)

   sw    a0,PolyGT3.xy0(t4)
   sw    a1,PolyGT3.xy1(t4)
   sw    a2,PolyGT3.xy2(t4)

; Get depth

   lhu   a0,$00+Mesh.depth(t5)
   lhu   a1,$10+Mesh.depth(t5)
   lhu   a2,$20+Mesh.depth(t5)
   mtc2  a0,r17
   mtc2  a1,r18
   mtc2  a2,r19
   nop
   cop2  $158002d    ; AverageZ3
   mfc2  v0,r7

; AddPrim

   if 1

   move  a0,t4       ; Get poly address
   sll   v0,v0,$2
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

   endif

; Increment mesh pointer

   addi  t4,t4,PolyGT3.sizeof

@MeshNext1:

; Finished

   lui   v0,MeshPtr>>16
   ori   v0,MeshPtr&$ffff
   sw    t4,$0(v0)         ; Store MeshPtr back
   nop

; Restore registers s0-s7

   lw    s0,$00(sp)
   lw    s1,$04(sp)
   lw    s2,$08(sp)
   lw    s3,$0c(sp)
   lw    s4,$10(sp)
   lw    s5,$14(sp)
   lw    s6,$18(sp)
   lw    s7,$1c(sp)

   j     @End
   addiu sp,sp,$20


   endif













; **** GOURAUD TEXTURED QUADS ****

@ClipGT4:

   bne   v0,v1,@ClipFT3
   addi  v1,zero,TypeFT3

   module

; Get coords

   lh    t0,GT4.coords+$0(s6)
   lh    t1,GT4.coords+$2(s6)
   lh    t2,GT4.coords+$4(s6)
   lh    t3,GT4.coords+$6(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7
   add   t3,t3,s7

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   or    t4,t4,t7
   andi  t4,t4,ShortClipFlags
   bne   t4,zero,@End
   nop

; Get PolyGT4

   beq   t8,zero,@GotPoly
   addi  s3,s6,GT4.quad    ; branch delay slot

; Get second PolyGT4

   addi  s3,s3,PolyGT4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT4.flag(s6)

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

   lwl   t4,LibGT4.colour0+$2(s4)
   lw    t5,LibGT4.colour1(s4)
   lw    t6,LibGT4.colour2(s4)
   lw    t7,LibGT4.colour3(s4)

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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibGT4.sizeof
   addi  s6,s6,GT4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED TRIANGLES ****

@ClipFT3:

   bne   v0,v1,@ClipFT4
   addi  v1,zero,TypeFT4

   module

; Get coords

   lh    t0,FT3.coords+$0(s6)
   lh    t1,FT3.coords+$2(s6)
   lh    t2,FT3.coords+$4(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   andi  t4,t4,ShortClipFlags
   bne   t4,zero,@End
   nop

; Get PolyFT3

   beq   t8,zero,@GotPoly
   addi  s3,s6,FT3.tri     ; branch delay slot

; Get second PolyFT3

   addi  s3,s3,PolyFT3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,FT3.flag(s6)

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

   lwl   t4,LibFT3.colour+$2(s4)
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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibFT3.sizeof
   addi  s6,s6,FT3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT TEXTURED QUADS ****

@ClipFT4:

   bne   v0,v1,@ClipG3
   addi  v1,zero,TypeG3

   module

; Get coords

   lh    t0,FT4.coords+$0(s6)
   lh    t1,FT4.coords+$2(s6)
   lh    t2,FT4.coords+$4(s6)
   lh    t3,FT4.coords+$6(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7
   add   t3,t3,s7

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   or    t4,t4,t7
   andi  t4,t4,ShortClipFlags
   bne   t4,zero,@End
   nop

; Get PolyFT4

   beq   t8,zero,@GotPoly
   addi  s3,s6,FT4.quad    ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,FT4.flag(s6)

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

   lwl   t4,LibFT4.colour+$2(s4)
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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibFT4.sizeof
   addi  s6,s6,FT4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED TRIANGLES ****

@ClipG3:

   bne   v0,v1,@ClipG4
   addi  v1,zero,TypeG4

   module

; Get coords

   lh    t0,G3.coords+$0(s6)
   lh    t1,G3.coords+$2(s6)
   lh    t2,G3.coords+$4(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   andi  t4,t4,ShortClipFlags
   bne   t4,zero,@End
   nop

; Get PolyG3

   beq   t8,zero,@GotPoly
   addi  s3,s6,G3.tri      ; branch delay slot

; Get second PolyG3

   addi  s3,s3,PolyG3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,G3.flag(s6)

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

   lwl   t4,LibG3.colour0+$2(s4)
   lw    t5,LibG3.colour1(s4)
   lw    t6,LibG3.colour2(s4)

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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibG3.sizeof
   addi  s6,s6,G3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** GOURAUD SHADED QUADS ****

@ClipG4:

   bne   v0,v1,@ClipF3
   addi  v1,zero,TypeF3

   module

; Get coords

   lh    t0,G4.coords+$0(s6)
   lh    t1,G4.coords+$2(s6)
   lh    t2,G4.coords+$4(s6)
   lh    t3,G4.coords+$6(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7
   add   t3,t3,s7

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   or    t4,t4,t7
   andi  t4,t4,ShortClipFlags
   bne   t4,zero,@End
   nop

; Get PolyG4

   beq   t8,zero,@GotPoly
   addi  s3,s6,G4.quad     ; branch delay slot

; Get second PolyG4

   addi  s3,s3,PolyG4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,G4.flag(s6)

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

   lwl   t4,LibG4.colour0+$2(s4)
   lw    t5,LibG4.colour1(s4)
   lw    t6,LibG4.colour2(s4)
   lw    t7,LibG4.colour3(s4)

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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibG4.sizeof
   addi  s6,s6,G4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT SHADED TRIANGLES ****

@ClipF3:
   
   bne   v0,v1,@ClipF4
   addi  v1,zero,TypeF4

   module

; Get coords

   lh    t0,F3.coords+$0(s6)
   lh    t1,F3.coords+$2(s6)
   lh    t2,F3.coords+$4(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   andi  t4,t4,ShortClipFlags
   bne   t4,zero,@End
   nop

; Get PolyF3

   beq   t8,zero,@GotPoly
   addi  s3,s6,F3.tri      ; branch delay slot

; Get second PolyF3

   addi  s3,s3,PolyF3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,F3.flag(s6)

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

   lwl   t4,LibF3.colour+$2(s4)
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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibF3.sizeof
   addi  s6,s6,F3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT SHADED QUADS ****

@ClipF4:
   if 1

   bne   v0,v1,@ClipTSPR
   addi  v1,zero,TypeTSPR

   else

   bne   v0,v1,@ClipBadPoly
   addi  v1,zero,-$1

   endif

   module

; Get coords

   lh    t0,F4.coords+$0(s6)
   lh    t1,F4.coords+$2(s6)
   lh    t2,F4.coords+$4(s6)
   lh    t3,F4.coords+$6(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7
   add   t3,t3,s7

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    t4,t4,t5
   or    t4,t4,t6
   or    t4,t4,t7
   andi  t4,t4,ShortClipFlags
   bne   t4,zero,@End
   nop

; Get PolyF4

   beq   t8,zero,@GotPoly
   addi  s3,s6,F4.quad     ; branch delay slot

; Get second PolyF4

   addi  s3,s3,PolyF4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT3.flag(s6)

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

   lwl   t4,LibF4.colour+$2(s4)
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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibF4.sizeof
   addi  s6,s6,F4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** ANY OTHER POLY TYPES GO HERE ****


@ClipTSPR:

   bne   v0,v1,@ClipBSPR
   addi  v1,zero,TypeBSPR

   module

; Get coord

   lh    t0,SPR.coord+$0(s6)
   nop

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3

; Convert indices to addresses

   add   t0,t0,s7

; Get flags

   lh    v0,RTPN.flags(t0)
   nop

; Test flags

   andi  v0,v0,ShortClipFlags
   bne   v0,zero,@End
   nop

; Get PolyFT4

   beq   t8,zero,@GotPoly
   addi  s3,s6,SPR.quad     ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
      
@GotPoly:

; Get sprite size

   lh    v0,SPR.width+$0(s6)  ; sprite width
   lh    v1,SPR.height+$0(s6) ; sprite height
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

   srl    t4,t5,$1      ; t4 : xsize / 2
   srl    t7,t6,$1      ; t7 : ysize / 2

   if 1

; calc rcos and rsin

   move  a0,s1

   jal   rcos           ; returned in v0
   nop
   multu v0,t4
   mflo  t7
   srl   t7,t7,$c       ; t7 offsetX


   move  a0,s1

   jal   rsin           ; returned in v0
   nop
   multu v0,t4
   mflo  t4
   srl   t4,t4,$c       ; t4 offsetY



   addi  a1,zero,1024
   sub   a0,a1,s1

   jal   rcos           ; returned in v0
   nop
   multu v0,t6
   mflo  t5
   srl   t5,t5,$c       ; t5 offset2X

   addi  a1,zero,1024
   sub   a0,a1,s1

   jal   rsin           ; returned in v0
   nop
   multu v0,t6
   mflo  t6
   srl   t6,t6,$c       ; t6 offset2Y

   endif
   


; Get screen coordinates of top middle

   lw    v0,RTPN.screen(t0)
   nop
   move  v1,v0
   andi  v0,v0,$ffff    ; v0 : x screen
   srl   v1,v1,$10      ; v1 : y screen

; work out the bounding sprite box

   sub   a0,v0,t7       ; tlX
   sub   a1,v1,t4       ; tlY
   move  a2,a0          ; store tlX
   move  a3,a1          ; store tlY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy0(s3)


   sub   a0,a2,t5       ; blX    
   add   a1,a3,t6       ; blY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy2(s3)


   add   a0,v0,t7       ; trX
   add   a1,v1,t4       ; trY
   move  a2,a0          ; store trX
   move  a3,a1          ; store trY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy1(s3)


   sub   a0,a2,t5       ; brX
   add   a1,a3,t6       ; brY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy3(s3)


; Get colour

   lw    t4,LibSPR.colour(s4)
   nop

   if 0
   addi  t4,zero,$ff
   endif

   sh    t4,PolyFT4.colour+$0(s3)
   srl   t4,t4,$10
   sb    t4,PolyFT4.colour+$2(s3)


; poke in the texture coords

   lh    a0,SPR.texture+$0(s6)   ; get texture index into texture table
   lui   t4,TextureTable>>$10
   ori   t4,t4,TextureTable&$ffff
   sll   a0,a0,$2
   add   a0,a0,t4
   lw    a3,$0(a0)
   nop

; tsb, cba

   lh    a2,$2(a3)
   nop
   sh    a2,PolyFT4.tsb(s3)

   lh    a2,$4(a3)
   nop
   sh    a2,PolyFT4.cba(s3)

; uv0

   lh    a2,$16(a3)
   nop
   sb    a2,PolyFT4.uv0(s3)

   lh    a2,$18(a3)
   nop
   sb    a2,PolyFT4.uv0+$1(s3)

; uv1

   lh    a2,$1a(a3)
   nop
   sb    a2,PolyFT4.uv1(s3)

   lh    a2,$1c(a3)
   nop
   sb    a2,PolyFT4.uv1+$1(s3)

; uv2

   lh    a2,$1e(a3)
   nop
   sb    a2,PolyFT4.uv2(s3)

   lh    a2,$20(a3)
   nop
   sb    a2,PolyFT4.uv2+$1(s3)

; uv3

   lh    a2,$22(a3)
   nop
   sb    a2,PolyFT4.uv3(s3)

   lh    a2,$24(a3)
   nop
   sb    a2,PolyFT4.uv3+$1(s3)


; Get depth ( 1 )

   lhu   v0,RTPN.depth(t0)

; AddPrim

   move  a0,s3       ; Get poly address
   andi  v0,v0,$fffc ; Mask out bottom 2 bits
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibSPR.sizeof
   addi  s6,s6,SPR.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!




@ClipBSPR:

   bne   v0,v1,@ClipBadPoly
   addi  v1,zero,-$1

   module

; Get coord

   lh    t0,SPR.coord+$0(s6)
   nop

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3

; Convert indices to addresses

   add   t0,t0,s7

; Get flags

   lh    v0,RTPN.flags(t0)
   nop

; Test flags

   andi  v0,v0,ShortClipFlags
   bne   v0,zero,@End
   nop

; Get PolyFT4

   beq   t8,zero,@GotPoly
   addi  s3,s6,SPR.quad     ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
      
@GotPoly:

; Get sprite size

   lh    v0,SPR.width+$0(s6)  ; sprite width
   lh    v1,SPR.height+$0(s6) ; sprite height
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

   srl    t4,t5,$1      ; t4 : xsize / 2
   srl    t7,t6,$1      ; t7 : ysize / 2

   if 1

; calc rcos and rsin

   move  a0,s1

   jal   rcos           ; returned in v0
   nop
   multu v0,t4
   mflo  t7
   srl   t7,t7,$c       ; t7 offsetX


   move  a0,s1

   jal   rsin           ; returned in v0
   nop
   multu v0,t4
   mflo  t4
   srl   t4,t4,$c       ; t4 offsetY



   addi  a1,zero,1024
   sub   a0,a1,s1

   jal   rcos           ; returned in v0
   nop
   multu v0,t6
   mflo  t5
   srl   t5,t5,$c       ; t5 offset2X

   addi  a1,zero,1024
   sub   a0,a1,s1

   jal   rsin           ; returned in v0
   nop
   multu v0,t6
   mflo  t6
   srl   t6,t6,$c       ; t6 offset2Y

   endif
   


; Get screen coordinates of bottom middle

   lw    v0,RTPN.screen(t0)
   nop
   move  v1,v0
   andi  v0,v0,$ffff    ; v0 : x screen
   srl   v1,v1,$10      ; v1 : y screen

; work out the bounding sprite box

   sub   a0,v0,t7       ; blX
   sub   a1,v1,t4       ; blY
   move  a2,a0          ; store blX
   move  a3,a1          ; store blY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy2(s3)


   add   a0,a2,t5       ; tlX    
   sub   a1,a3,t6       ; tlY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy0(s3)


   add   a0,v0,t7       ; brX
   add   a1,v1,t4       ; brY
   move  a2,a0          ; store brX
   move  a3,a1          ; store brY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy3(s3)


   add   a0,a2,t5       ; trX
   sub   a1,a3,t6       ; trY
   andi  a0,a0,$ffff
   sll   a1,a1,$10
   or    a0,a0,a1
   sw    a0,PolyFT4.xy1(s3)


; Get colour

   lw    t4,LibSPR.colour(s4)
   nop

   if 0
   addi  t4,zero,$ff
   endif

   sh    t4,PolyFT4.colour+$0(s3)
   srl   t4,t4,$10
   sb    t4,PolyFT4.colour+$2(s3)


; poke in the texture coords

   lh    a0,SPR.texture+$0(s6)   ; get texture index into texture table
   lui   t4,TextureTable>>$10
   ori   t4,t4,TextureTable&$ffff
   sll   a0,a0,$2
   add   a0,a0,t4
   lw    a3,$0(a0)
   nop

; tsb, cba

   lh    a2,$2(a3)
   nop
   sh    a2,PolyFT4.tsb(s3)

   lh    a2,$4(a3)
   nop
   sh    a2,PolyFT4.cba(s3)

; uv0

   lh    a2,$16(a3)
   nop
   sb    a2,PolyFT4.uv0(s3)

   lh    a2,$18(a3)
   nop
   sb    a2,PolyFT4.uv0+$1(s3)

; uv1

   lh    a2,$1a(a3)
   nop
   sb    a2,PolyFT4.uv1(s3)

   lh    a2,$1c(a3)
   nop
   sb    a2,PolyFT4.uv1+$1(s3)

; uv2

   lh    a2,$1e(a3)
   nop
   sb    a2,PolyFT4.uv2(s3)

   lh    a2,$20(a3)
   nop
   sb    a2,PolyFT4.uv2+$1(s3)

; uv3

   lh    a2,$22(a3)
   nop
   sb    a2,PolyFT4.uv3(s3)

   lh    a2,$24(a3)
   nop
   sb    a2,PolyFT4.uv3+$1(s3)


; Get depth ( 1 )

   lhu   v0,RTPN.depth(t0)

; AddPrim

   move  a0,s3       ; Get poly address
   andi  v0,v0,$fffc ; Mask out bottom 2 bits
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibSPR.sizeof
   addi  s6,s6,SPR.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** UNRECOGNIZED POLY TYPE ****

@ClipBadPoly:

   bgtz  v0,AsmDrawLibObjEnd
   nop                     ; Error result is primitive type

   j     AsmDrawLibObjEnd
   addi  v0,zero,-$1       ; Error result is -1

; **** NEXT POLYGON ****

@ClipNext:

   bgtz  s5,@ClipLoop
   nop

   j     AsmDrawLibObjOk
   nop
















@NoClip:

; Fill in the gpu packets

@NoClipLoop:

   lbu   v0,F3.type(s6)
   addi  v1,zero,TypeGT3

; **** NON CLIPPED GOURAUD TEXTURED TRIANGLES ****

@NoClipGT3:

   bne   v0,v1,@NoClipGT4
   addi  v1,zero,TypeGT4

   module

; Get coords

   lh    t0,GT3.coords+$0(s6)
   lh    t1,GT3.coords+$2(s6)
   lh    t2,GT3.coords+$4(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7

; Get PolyGT3

   beq   t8,zero,@GotPoly
   addi  s3,s6,GT3.tri     ; branch delay slot

; Get second PolyGT3

   addi  s3,s3,PolyGT3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT3.flag(s6)

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

   lwl   t4,LibGT3.colour0+$2(s4)
   lw    t5,LibGT3.colour1(s4)
   lw    t6,LibGT3.colour2(s4)

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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibGT3.sizeof
   addi  s6,s6,GT3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** NON CLIPPED GOURAUD TEXTURED QUADS ****

@NoClipGT4:

   bne   v0,v1,@NoClipFT3
   addi  v1,zero,TypeFT3

   module

; Get coords

   lh    t0,GT4.coords+$0(s6)
   lh    t1,GT4.coords+$2(s6)
   lh    t2,GT4.coords+$4(s6)
   lh    t3,GT4.coords+$6(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7
   add   t3,t3,s7

; Get PolyGT4

   beq   t8,zero,@GotPoly
   addi  s3,s6,GT4.quad    ; branch delay slot

; Get second PolyGT4

   addi  s3,s3,PolyGT4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT4.flag(s6)

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

   lwl   t4,LibGT4.colour0+$2(s4)
   lw    t5,LibGT4.colour1(s4)
   lw    t6,LibGT4.colour2(s4)
   lw    t7,LibGT4.colour3(s4)

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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibGT4.sizeof
   addi  s6,s6,GT4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** NON CLIPPED FLAT TEXTURED TRIANGLES ****

@NoClipFT3:

   bne   v0,v1,@NoClipFT4
   addi  v1,zero,TypeFT4

   module

; Get coords

   lh    t0,FT3.coords+$0(s6)
   lh    t1,FT3.coords+$2(s6)
   lh    t2,FT3.coords+$4(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7

; Get PolyFT3

   beq   t8,zero,@GotPoly
   addi  s3,s6,FT3.tri     ; branch delay slot

; Get second PolyFT3

   addi  s3,s3,PolyFT3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,FT3.flag(s6)

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

   lwl   t4,LibFT3.colour+$2(s4)
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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibFT3.sizeof
   addi  s6,s6,FT3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** NON CLIPPED FLAT TEXTURED QUADS ****

@NoClipFT4:

   bne   v0,v1,@NoClipG3
   addi  v1,zero,TypeG3

   module

; Get coords

   lh    t0,FT4.coords+$0(s6)
   lh    t1,FT4.coords+$2(s6)
   lh    t2,FT4.coords+$4(s6)
   lh    t3,FT4.coords+$6(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7
   add   t3,t3,s7

; Get PolyFT4

   beq   t8,zero,@GotPoly
   addi  s3,s6,FT4.quad    ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,FT4.flag(s6)

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

   lwl   t4,LibFT4.colour+$2(s4)
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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibFT4.sizeof
   addi  s6,s6,FT4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** NON CLIPPED GOURAUD SHADED TRIANGLES ****

@NoClipG3:

   bne   v0,v1,@NoClipG4
   addi  v1,zero,TypeG4

   module

; Get coords

   lh    t0,G3.coords+$0(s6)
   lh    t1,G3.coords+$2(s6)
   lh    t2,G3.coords+$4(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7

; Get PolyG3

   beq   t8,zero,@GotPoly
   addi  s3,s6,G3.tri      ; branch delay slot

; Get second PolyG3

   addi  s3,s3,PolyG3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,G3.flag(s6)

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

   lwl   t4,LibG3.colour0+$2(s4)
   lw    t5,LibG3.colour1(s4)
   lw    t6,LibG3.colour2(s4)

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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibG3.sizeof
   addi  s6,s6,G3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** NON CLIPPED GOURAUD SHADED QUADS ****

@NoClipG4:

   bne   v0,v1,@NoClipF3
   addi  v1,zero,TypeF3

   module

; Get coords

   lh    t0,G4.coords+$0(s6)
   lh    t1,G4.coords+$2(s6)
   lh    t2,G4.coords+$4(s6)
   lh    t3,G4.coords+$6(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7
   add   t3,t3,s7

; Get PolyG4

   beq   t8,zero,@GotPoly
   addi  s3,s6,G4.quad     ; branch delay slot

; Get second PolyG4

   addi  s3,s3,PolyG4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,G4.flag(s6)

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

   lwl   t4,LibG4.colour0+$2(s4)
   lw    t5,LibG4.colour1(s4)
   lw    t6,LibG4.colour2(s4)
   lw    t7,LibG4.colour3(s4)

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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibG4.sizeof
   addi  s6,s6,G4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** NON CLIPPED FLAT SHADED TRIANGLES ****

@NoClipF3:

   bne   v0,v1,@NoClipF4
   addi  v1,zero,TypeF4

   module

; Get coords

   lh    t0,F3.coords+$0(s6)
   lh    t1,F3.coords+$2(s6)
   lh    t2,F3.coords+$4(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7

; Get PolyF3

   beq   t8,zero,@GotPoly
   addi  s3,s6,F3.tri      ; branch delay slot

; Get second PolyF3

   addi  s3,s3,PolyF3.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,F3.flag(s6)

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

   lwl   t4,LibF3.colour+$2(s4)
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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibF3.sizeof
   addi  s6,s6,F3.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** NON CLIPPED FLAT SHADED QUADS ****

@NoClipF4:

   bne   v0,v1,@NoClipBadPoly
   addi  v1,zero,-$1

   module

; Get coords

   lh    t0,F4.coords+$0(s6)
   lh    t1,F4.coords+$2(s6)
   lh    t2,F4.coords+$4(s6)
   lh    t3,F4.coords+$6(s6)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3
   sll   t3,t3,$3

; Convert indices to addresses

   add   t0,t0,s7
   add   t1,t1,s7
   add   t2,t2,s7
   add   t3,t3,s7

; Get PolyF4

   beq   t8,zero,@GotPoly
   addi  s3,s6,F4.quad     ; branch delay slot

; Get second PolyF4

   addi  s3,s3,PolyF4.sizeof
      
@GotPoly:

; Get flags

   lbu   v0,GT3.flag(s6)

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

   lwl   t4,LibF4.colour+$2(s4)
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
   add   a1,t9,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,gp
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,fp
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LibF4.sizeof
   addi  s6,s6,F4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** ANY OTHER POLY TYPES GO HERE ****


; **** UNRECOGNIZED POLY TYPE ****

@NoClipBadPoly:

   bgtz  v0,AsmDrawLibObjEnd
   nop                     ; Error result is primitive type
   j     AsmDrawLibObjEnd
   addi  v0,zero,-$1       ; Error result is -1

; **** NEXT POLYGON ****

@NoClipNext:

   bgtz  s5,@NoClipLoop
   nop






; Restore registers and unlink stack space

AsmDrawLibObjOk:

   addi  v0,zero,$0        ; Success result is zero

AsmDrawLibObjEnd:

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

; AsmDrawLibObj End





































; PARAMETERS

; a0 : Object*       object
; a1 : Skeleton*     camera

; REGISTER ASSIGNMENT

; s0 : object ptr
; s1 : camera ptr
; s2 : vertex ptr
; s3 : vertex count / poly ptr
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
; t8 : AddPrim mask $00ffffff
; t9 : AddPrim mask $ff000000

; fp : RTPN ptr
; gp : normal ptr    * temp *
; at :
; k0 :
; k1 :

AsmDrawObj: 

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

; Set the RotTrans matrix

   addiu sp,sp,-$4

   lw    a0,Object.skeleton(s0)
   jal   SetSkeletonTransform
   nop

   addiu sp,sp,$4

; Get vertex pointer and counter

   lw    s2,Object.vertices(s0)
   lhu   s3,Object.vertexCount(s0)

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

   addi  v0,s3,-$80
   blez  v0,@FastRam

   lui   fp,$1f80          ; Branch delay slot!

@SlowRam:

   lui   fp,SlowMem>>16
   ori   fp,SlowMem&$ffff

@FastRam:

; RotTransPersN

   move  a0,s2                   ; vertex ptr
   move  a1,fp                   ; rtpn ptr

   addi  v0,zero,$0              ; clear cumulative flags

@RotTransPersN
   
   lwc2  zero,$0(a0)             ; load svector
   lwc2  at,$4(a0)               ; load svector
   addi  a0,a0,SVECTOR.sizeof    ; increment svector ptr
   cop2  $180001                 ; RotTransPers
   
   mfc2  a2,r19                  ; get depth
   cfc2  a3,r31                  ; get flags

   swc2  t6,RTPN.screen(a1)      ; store screen coords
  
   srl   a2,a2,$1                ; far depth
   sh    a2,RTPN.depth(a1)       ; store depth

   srl   a3,a3,$c                ; short flags
   sh    a3,RTPN.flags(a1)       ; store flags
   
   or    v0,v0,a3                ; cumulative flags
   addi  s3,s3,-$1               ; decrement ...
   bgtz  s3,@RotTransPersN       ; ... and branch
   addi  a1,a1,RTPN.sizeof       ; increment rtpn ptr

; Zero translation matrix ( for rotating normals )

   ctc2  zero,r5
   ctc2  zero,r6
   ctc2  zero,r7

; Test cumulative flags and set AddPrim masks

   lui   t8,$00ff
   ori   t8,$ffff

   andi  v0,v0,ShortClipFlags    ; mask cumulative flags
   beq   v0,zero,@NoClip

   lui   t9,$ff00                ; Branch delay slot!




; **** NEED TO CHECK FLAGS SINCE SOME ARE BAD ****

@Clip:

; Fill in the gpu packets

@ClipLoop:

   lbu   v0,F3.type(s4)
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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyGT3

   beq   s6,zero,@GotPoly
   addi  s3,s4,GT3.tri     ; branch delay slot

; Get second PolyGT3

   addi  s3,s3,PolyGT3.sizeof
 
@GotPoly:

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
   sw    t4,PolyGT3.xy0(s3)      ; Delay slot!
   sw    t5,PolyGT3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyGT3.xy0(s3)
;  sw    t5,PolyGT3.xy1(s3)
;  sw    t6,PolyGT3.xy2(s3)


; Get Colour ?
 

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyGT4

   beq   s6,zero,@GotPoly
   addi  s3,s4,GT4.quad    ; branch delay slot

; Get second PolyGT4

   addi  s3,s3,PolyGT4.sizeof
 
@GotPoly:

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
   sw    t4,PolyGT4.xy0(s3)      ; Delay slot!
   sw    t5,PolyGT4.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT4.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyGT4.xy3(s3)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyGT4.xy0(s3)
;  sw    t5,PolyGT4.xy1(s3)
;  sw    t6,PolyGT4.xy2(s3)
;  sw    t7,PolyGT4.xy3(s3)


; Get Colour ?


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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyFT3

   beq   s6,zero,@GotPoly
   addi  s3,s4,FT3.tri     ; branch delay slot

; Get second PolyFT3

   addi  s3,s3,PolyFT3.sizeof
 
@GotPoly:

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
   sw    t4,PolyFT3.xy0(s3)      ; Delay slot!
   sw    t5,PolyFT3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyFT3.xy0(s3)
;  sw    t5,PolyFT3.xy1(s3)
;  sw    t6,PolyFT3.xy2(s3)


; Get Colour ?


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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyFT4

   beq   s6,zero,@GotPoly
   addi  s3,s4,FT4.quad    ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
 
@GotPoly:

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
   sw    t4,PolyFT4.xy0(s3)      ; Delay slot!
   sw    t5,PolyFT4.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT4.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyFT4.xy3(s3)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyFT4.xy0(s3)
;  sw    t5,PolyFT4.xy1(s3)
;  sw    t6,PolyFT4.xy2(s3)
;  sw    t7,PolyFT4.xy3(s3)


; Get Colour ?


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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyG3

   beq   s6,zero,@GotPoly
   addi  s3,s4,G3.tri      ; branch delay slot

; Get second PolyG3

   addi  s3,s3,PolyG3.sizeof
 
@GotPoly:

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
   sw    t4,PolyG3.xy0(s3)      ; Delay slot!
   sw    t5,PolyG3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyG3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyG3.xy0(s3)
;  sw    t5,PolyG3.xy1(s3)
;  sw    t6,PolyG3.xy2(s3)


; Get Colour ?


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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyG4

   beq   s6,zero,@GotPoly
   addi  s3,s4,G4.quad     ; branch delay slot

; Get second PolyG4

   addi  s3,s3,PolyG4.sizeof
 
@GotPoly:

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
   sw    t4,PolyG4.xy0(s3)       ; Delay slot!
   sw    t5,PolyG4.xy1(s3)       ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyG4.xy2(s3)       ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyG4.xy3(s3)       ; Delay slot!

; Store screen coords

;  sw    t4,PolyG4.xy0(s3)
;  sw    t5,PolyG4.xy1(s3)
;  sw    t6,PolyG4.xy2(s3)
;  sw    t7,PolyG4.xy3(s3)


; Get Colour ?


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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyF3

   beq   s6,zero,@GotPoly
   addi  s3,s4,F3.tri      ; branch delay slot

; Get second PolyF3

   addi  s3,s3,PolyF3.sizeof
 
@GotPoly:

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
   sw    t4,PolyF3.xy0(s3)      ; Delay slot!
   sw    t5,PolyF3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyF3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyF3.xy0(s3)
;  sw    t5,PolyF3.xy1(s3)
;  sw    t6,PolyF3.xy2(s3)


; Get Colour ?


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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,F3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!



; **** FLAT SHADED QUADS ****

@ClipF4:

   bne   v0,v1,@ClipLSGT3
   addi  v1,zero,TypeLSGT3

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyF4

   beq   s6,zero,@GotPoly
   addi  s3,s4,F4.quad     ; branch delay slot

; Get second PolyF4

   addi  s3,s3,PolyF4.sizeof
 
@GotPoly:

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
   sw    t4,PolyF4.xy0(s3)       ; Delay slot!
   sw    t5,PolyF4.xy1(s3)       ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyF4.xy2(s3)       ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyF4.xy3(s3)       ; Delay slot!

; Store screen coords

;  sw    t4,PolyF4.xy0(s3)
;  sw    t5,PolyF4.xy1(s3)
;  sw    t6,PolyF4.xy2(s3)
;  sw    t7,PolyF4.xy3(s3)


; Get Colour ?


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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,F4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!









; **** LIGHT SOURCED GOURAUD TEXTURED TRIANGLES ****

@ClipLSGT3:

   bne   v0,v1,@ClipLSGT4
   addi  v1,zero,TypeLSGT4

   module

; Get coords

   lh    t0,LSGT3.coords+$0(s4)
   lh    t1,LSGT3.coords+$2(s4)
   lh    t2,LSGT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof RTPN structure )

   sll   t0,t0,$3
   sll   t1,t1,$3
   sll   t2,t2,$3

; Convert indices to addresses

   add   t0,t0,fp
   add   t1,t1,fp
   add   t2,t2,fp

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyGT3

   beq   s6,zero,@GotPoly
   addi  s3,s4,LSGT3.tri     ; branch delay slot

; Get second PolyGT3

   addi  s3,s3,PolyGT3.sizeof
 
@GotPoly:

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
   sw    t4,PolyGT3.xy0(s3)      ; Delay slot!
   sw    t5,PolyGT3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyGT3.xy0(s3)
;  sw    t5,PolyGT3.xy1(s3)
;  sw    t6,PolyGT3.xy2(s3)


; Get Colour ?

   if 0


   nop

; Get coords for normals

   lh    a0,LSGT3.coords+$0(s4)
   lh    a1,LSGT3.coords+$2(s4)
   lh    a2,LSGT3.coords+$4(s4)

; Multiply coords by 8 ( sizeof SVECTOR )

   sll   a0,a0,$3
   sll   a1,a1,$3
   sll   a2,a2,$3
   
; Convert indices to addresses

   add   a0,a0,gp
   add   a1,a1,gp
   add   a2,a2,gp

; NormalColorCol for coord 0

   lwc2  r0,$0(a0)               ; normal
   lwc2  r1,$4(a0)
   

   if 1 

   nop
   cop2  $480012                 ; ApplyRotMatrix

   mfc2  v0,r25
   mfc2  v1,r26
   andi  v0,v0,$ffff
   sll   v1,v1,$10
   or    v0,v0,v1
   mtc2  v0,r0

   mfc2  v0,r27
   nop
   andi  v0,v0,$ffff
   mtc2  v0,r1

   endif


   lwc2  r6,LSGT3.colour0(s4)    ; colour

   mtc2  zero,r8

   nop
   cop2  $e80413                 ; NormalColorDpq

   mfc2  v0,r22
   nop

   sh    v0,PolyGT3.colour0+$0(s3)
   srl   v0,v0,$10
   sb    v0,PolyGT3.colour0+$2(s3)

; NormalColorCol for coord 1

   lwc2  r0,$0(a1)               ; normal
   lwc2  r1,$4(a1)

   
   if 1 

   nop
   cop2  $480012                 ; ApplyRotMatrix

   mfc2  v0,r25
   mfc2  v1,r26
   andi  v0,v0,$ffff
   sll   v1,v1,$10
   or    v0,v0,v1
   mtc2  v0,r0

   mfc2  v0,r27
   nop
   andi  v0,v0,$ffff
   mtc2  v0,r1

   endif


   lwc2  r6,LSGT3.colour1(s4)    ; colour

   mtc2  zero,r8

   nop
   cop2  $e80413                 ; NormalColorDpq

   mfc2  v0,r22
   nop

   sw    v0,PolyGT3.colour1(s3)

; NormalColorCol for coord 2

   lwc2  r0,$0(a2)               ; normal
   lwc2  r1,$4(a2)
   

   if 1 

   nop
   cop2  $480012                 ; ApplyRotMatrix

   mfc2  v0,r25
   mfc2  v1,r26
   andi  v0,v0,$ffff
   sll   v1,v1,$10
   or    v0,v0,v1
   mtc2  v0,r0

   mfc2  v0,r27
   nop
   andi  v0,v0,$ffff
   mtc2  v0,r1

   endif


   lwc2  r6,LSGT3.colour2(s4)    ; colour

   mtc2  zero,r8

   nop
   cop2  $e80413                 ; NormalColorDpq

   mfc2  v0,r22
   nop

   sw    v0,PolyGT3.colour2(s3)


   endif

; Get colour ?

   if 0

   lw    a0,LSGT3.colour0(s4)
   lw    a1,LSGT3.colour1(s4)
   lw    a2,LSGT3.colour2(s4)

   sh    a0,PolyGT3.colour0+$0(s3)
   srl   a0,a0,$10
   sb    a0,PolyGT3.colour0+$2(s3)

   sw    a1,PolyGT3.colour1(s3)

   sw    a2,PolyGT3.colour2(s3)

   endif

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LSGT3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!









; **** LIGHT SOURCED GOURAUD TEXTURED QUADS ****

@ClipLSGT4:

   bne   v0,v1,@ClipLSFT3
   addi  v1,zero,TypeLSFT3

   module

; Get coords

   lh    t0,LSGT4.coords+$0(s4)
   lh    t1,LSGT4.coords+$2(s4)
   lh    t2,LSGT4.coords+$4(s4)
   lh    t3,LSGT4.coords+$6(s4)

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyGT4

   beq   s6,zero,@GotPoly
   addi  s3,s4,LSGT4.quad  ; branch delay slot

; Get second PolyGT4

   addi  s3,s3,PolyGT4.sizeof
 
@GotPoly:

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
   sw    t4,PolyGT4.xy0(s3)      ; Delay slot!
   sw    t5,PolyGT4.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT4.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyGT4.xy3(s3)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyGT4.xy0(s3)
;  sw    t5,PolyGT4.xy1(s3)
;  sw    t6,PolyGT4.xy2(s3)
;  sw    t7,PolyGT4.xy3(s3)


; Get Colour ?

   lw    a0,LSGT4.colour0(s4)
   lw    a1,LSGT4.colour1(s4)
   lw    a2,LSGT4.colour2(s4)
   lw    a3,LSGT4.colour3(s4)

   sh    a0,PolyGT4.colour0+$0(s3)
   srl   a0,a0,$10
   sb    a0,PolyGT4.colour0+$2(s3)

   sw    a1,PolyGT4.colour1(s3)

   sw    a2,PolyGT4.colour2(s3)
   
   sw    a3,PolyGT4.colour3(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LSGT4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!






; **** LIGHT SOURCED FLAT TEXTURED TRIANGLES ****

@ClipLSFT3:

   bne   v0,v1,@ClipLSFT4
   addi  v1,zero,TypeLSFT4

   module



@End:

   addi  s4,s4,LSFT3.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** LIGHT SOURCED FLAT TEXTURED QUADS ****

@ClipLSFT4:

   bne   v0,v1,@ClipBadPoly
   addi  v1,zero,-$1

   module

   j     @End
   nop

; Get coords

   lh    t0,LSFT4.coords+$0(s4)
   lh    t1,LSFT4.coords+$2(s4)
   lh    t2,LSFT4.coords+$4(s4)
   lh    t3,LSFT4.coords+$6(s4)

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

; Get flags

   lh    t4,RTPN.flags(t0)
   lh    t5,RTPN.flags(t1)
   lh    t6,RTPN.flags(t2)
   lh    t7,RTPN.flags(t3)

; Test flags

   or    v0,t4,t5
   or    v0,v0,t6
   or    v0,v0,t7
   andi  v0,v0,ShortClipFlags

   bne   v0,zero,@End
   nop

; Get PolyFT4

   beq   s6,zero,@GotPoly
   addi  s3,s4,LSFT4.quad  ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
 
@GotPoly:

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
   sw    t4,PolyFT4.xy0(s3)      ; Delay slot!
   sw    t5,PolyFT4.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT4.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyFT4.xy3(s3)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyFT4.xy0(s3)
;  sw    t5,PolyFT4.xy1(s3)
;  sw    t6,PolyFT4.xy2(s3)
;  sw    t7,PolyFT4.xy3(s3)


; Get Colour ?


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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,LSFT4.sizeof

   modend

   j     @ClipNext
   addi  s5,s5,-$1         ; branch delay slot!






; **** ANY OTHER POLY TYPES GO HERE ****



; **** UNRECOGNIZED POLY TYPE ****

@ClipBadPoly:

   bgtz  v0,AsmDrawObjEnd
   nop                     ; Error result is primitive type

   j     AsmDrawObjEnd
   addi  v0,zero,-$1       ; Error result is -1



; **** NEXT POLYGON ****

@ClipNext:

   bgtz  s5,@ClipLoop
   nop

   j     AsmDrawObjOk
   nop





; **** ALL FLAGS GOOD SO NO CHECKS NEEDED ****


@NoClip:

; Fill in the gpu packets

@NoClipLoop:

   lbu   v0,F3.type(s4)
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

; Get PolyGT3

   beq   s6,zero,@GotPoly
   addi  s3,s4,GT3.tri     ; branch delay slot

; Get second PolyGT3

   addi  s3,s3,PolyGT3.sizeof
 
@GotPoly:

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
   sw    t4,PolyGT3.xy0(s3)      ; Delay slot!
   sw    t5,PolyGT3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyGT3.xy0(s3)
;  sw    t5,PolyGT3.xy1(s3)
;  sw    t6,PolyGT3.xy2(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get PolyGT4

   beq   s6,zero,@GotPoly
   addi  s3,s4,GT4.quad    ; branch delay slot

; Get second PolyGT4

   addi  s3,s3,PolyGT4.sizeof
 
@GotPoly:

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
   sw    t4,PolyGT4.xy0(s3)      ; Delay slot!
   sw    t5,PolyGT4.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyGT4.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyGT4.xy3(s3)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyGT4.xy0(s3)
;  sw    t5,PolyGT4.xy1(s3)
;  sw    t6,PolyGT4.xy2(s3)
;  sw    t7,PolyGT4.xy3(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get PolyFT3

   beq   s6,zero,@GotPoly
   addi  s3,s4,FT3.tri     ; branch delay slot

; Get second PolyFT3

   addi  s3,s3,PolyFT3.sizeof
 
@GotPoly:

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
   sw    t4,PolyFT3.xy0(s3)      ; Delay slot!
   sw    t5,PolyFT3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyFT3.xy0(s3)
;  sw    t5,PolyFT3.xy1(s3)
;  sw    t6,PolyFT3.xy2(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get PolyFT4

   beq   s6,zero,@GotPoly
   addi  s3,s4,FT4.quad    ; branch delay slot

; Get second PolyFT4

   addi  s3,s3,PolyFT4.sizeof
 
@GotPoly:

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
   sw    t4,PolyFT4.xy0(s3)      ; Delay slot!
   sw    t5,PolyFT4.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyFT4.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyFT4.xy3(s3)      ; Delay slot!

; Store screen coords

;  sw    t4,PolyFT4.xy0(s3)
;  sw    t5,PolyFT4.xy1(s3)
;  sw    t6,PolyFT4.xy2(s3)
;  sw    t7,PolyFT4.xy3(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get PolyG3

   beq   s6,zero,@GotPoly
   addi  s3,s4,G3.tri      ; branch delay slot

; Get second PolyG3

   addi  s3,s3,PolyG3.sizeof
 
@GotPoly:

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
   sw    t4,PolyG3.xy0(s3)      ; Delay slot!
   sw    t5,PolyG3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyG3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyG3.xy0(s3)
;  sw    t5,PolyG3.xy1(s3)
;  sw    t6,PolyG3.xy2(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get PolyG4

   beq   s6,zero,@GotPoly
   addi  s3,s4,G4.quad     ; branch delay slot

; Get second PolyG4

   addi  s3,s3,PolyG4.sizeof
 
@GotPoly:

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
   sw    t4,PolyG4.xy0(s3)       ; Delay slot!
   sw    t5,PolyG4.xy1(s3)       ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyG4.xy2(s3)       ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyG4.xy3(s3)       ; Delay slot!

; Store screen coords

;  sw    t4,PolyG4.xy0(s3)
;  sw    t5,PolyG4.xy1(s3)
;  sw    t6,PolyG4.xy2(s3)
;  sw    t7,PolyG4.xy3(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get PolyF3

   beq   s6,zero,@GotPoly
   addi  s3,s4,F3.tri      ; branch delay slot

; Get second PolyF3

   addi  s3,s3,PolyF3.sizeof
 
@GotPoly:

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
   sw    t4,PolyF3.xy0(s3)      ; Delay slot!
   sw    t5,PolyF3.xy1(s3)      ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyF3.xy2(s3)      ; Delay slot!
   blez  v0,@End
;  nop

; Store screen coords

;  sw    t4,PolyF3.xy0(s3)
;  sw    t5,PolyF3.xy1(s3)
;  sw    t6,PolyF3.xy2(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

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

; Get PolyF4

   beq   s6,zero,@GotPoly
   addi  s3,s4,F4.quad     ; branch delay slot

; Get second PolyF4

   addi  s3,s3,PolyF4.sizeof
 
@GotPoly:

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
   sw    t4,PolyF4.xy0(s3)       ; Delay slot!
   sw    t5,PolyF4.xy1(s3)       ; Delay slot!
   cop2  $140006  ; NormalClip
   mfc2  v0,r24
;  nop
   sw    t6,PolyF4.xy2(s3)       ; Delay slot!
   blez  v0,@End
;  nop
   sw    t7,PolyF4.xy3(s3)       ; Delay slot!

; Store screen coords

;  sw    t4,PolyF4.xy0(s3)
;  sw    t5,PolyF4.xy1(s3)
;  sw    t6,PolyF4.xy2(s3)
;  sw    t7,PolyF4.xy3(s3)

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

   move  a0,s3       ; Get poly address
   sll   v0,v0,$2
   add   a1,s7,v0    ; Get OT address

   lw    v0,0(a0)    ; Get poly's next pointer
   lw    v1,0(a1)    ; Get OT's next pointer

   and   v0,v0,t9
   or    v1,v0,v1
   sw    v1,0(a0)    ; Poly points to what OT used to

   and   a0,a0,t8
   sw    a0,0(a1)    ; OT points to poly

@End:

   addi  s4,s4,F4.sizeof

   modend

   j     @NoClipNext
   addi  s5,s5,-$1         ; branch delay slot!




; **** ANY OTHER POLY TYPES GO HERE ****



; **** UNRECOGNIZED POLY TYPE ****

@NoClipBadPoly:

   bgtz  v0,AsmDrawObjEnd
   nop                     ; Error result is primitive type

   j     AsmDrawObjEnd
   addi  v0,zero,-$1       ; Error result is -1



; **** NEXT POLYGON ****

@NoClipNext:

   bgtz  s5,@NoClipLoop
   nop

   j     AsmDrawObjOk
   nop










; Restore registers and unlink stack space

AsmDrawObjOk:

   addi  v0,zero,$0        ; Success result is zero

AsmDrawObjEnd:

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

; AsmDrawObj End




   endif

