 !  .section SEGA_P
   .align   4

!   .global  _memcpy
!   .global  _memcpyb
   .global  _memcpyw
   .global  _memcpyl
   .global  _memsetl
!   .global  _memsetl_fast
    .global  _memset4_fast
!    .global  _memset4_fast2

! dst 	   = r4
! src 	   = r5
! size     = r6
!_memcpy:
!_memcpyb:
!   tst    r6,r6
!   bt     lendb

!lmemcpy:
!   mov.b  @r5+,r0
!   mov.b  r0,@r4
!   dt     r6
!   add    #1,r4
!   bf     lmemcpy
!lendb:
!   rts
!   nop

_memcpyw:
   shlr   r6
   tst    r6,r6
   bt     lendw

lmemcpyw:
   mov.w  @r5+,r0
   mov.w  r0,@r4
   dt     r6
   add    #2,r4
   bf     lmemcpyw

lendw:
   rts
   nop

_memcpyl:
   shlr2   r6
   tst    r6,r6
   bt     lendl

lmemcpyl:
   mov.l  @r5+,r0
   mov.l  r0,@r4
   dt     r6
   add    #4,r4
   bf     lmemcpyl

lendl:
   rts
   nop

! vbt add

!_memsetl:
!   shlr2   r6
!   tst    r6,r6
!   bt     lendmsl

!lmemsetl:
!   mov.l  @r5+,r0
!   mov.l  r0,@r4
!   dt     r6
!   add    #4,r4
!   bf     lmemsetl
!
!lendmsl:
!   rts
!   nop

! void memsetl_fast(uint32_t *ptr, uint32_t value, uint32_t len);
! ptr must be 32-bit aligned. value must be non-zero. len is in bytes and must
! be a multiple of 4.

!_memsetl_fast:
!    add     #4, r4
!    mov     r4, r2
!    add     r6, r2
!.loop2:
!    cmp/eq  r4, r2
!    bf/s    .loop2
!    mov.l   r5, @-r2
!    rts
!    nop

! void memsetl_fast(uint32_t *ptr, uint32_t value, uint32_t len);
! ptr must be 32-bit aligned. value must be non-zero. len is in bytes and must
! be a multiple of 4. None of these are checked in the function itself
! (although, it will throw an exception if the ptr isn't aligned, since the CPU
! won't do that access).

_memset4_fast:
    add     r4, r6
    add     #4, r4
.loop2:
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    mov.l   r5, @-r6
    cmp/eq  r4, r6
    bf/s    .loop2
    mov.l   r5, @-r6

    rts

    nop

    .end

