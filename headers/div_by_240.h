#ifndef DIV_BY_240_INCLUDED
#define DIV_BY_240_INCLUDED

#ifndef DIV_T_DEFINED
#define DIV_T_DEFINED
typedef struct div_t { unsigned q; unsigned r; } div_t;
#endif

static
__inline
__pure
__value_in_regs
div_t
div_by_240_qr(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #240;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #7;
     RSB     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #4;
     ADDPL   rv.q, rv.q, #1;
     ADDMI   rv.r, rv.r, #240;
  }

  return rv;
}

static
__inline
__pure
unsigned
div_by_240_q(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #240;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #7;
     RSB     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #4;
     ADDPL   rv.q, rv.q, #1;
  }

  return rv.q;
}

static
__inline
__pure
unsigned
div_by_240_r(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #240;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #7;
     RSB     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #4;
     ADDMI   rv.r, rv.r, #240;
  }

  return rv.r;
}

#endif
