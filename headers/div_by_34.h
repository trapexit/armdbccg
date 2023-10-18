#ifndef DIV_BY_34_INCLUDED
#define DIV_BY_34_INCLUDED

#ifndef DIV_T_DEFINED
#define DIV_T_DEFINED
typedef struct div_t { unsigned q; unsigned r; } div_t;
#endif

static
__inline
__pure
__value_in_regs
div_t
div_by_34_qr(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #34;
     SUB     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #5;
     ADD     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDPL   rv.q, rv.q, #1;
     ADDMI   rv.r, rv.r, #34;
  }

  return rv;
}

static
__inline
__pure
unsigned
div_by_34_q(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #34;
     SUB     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #5;
     ADD     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDPL   rv.q, rv.q, #1;
  }

  return rv.q;
}

static
__inline
__pure
unsigned
div_by_34_r(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #34;
     SUB     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #5;
     ADD     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDMI   rv.r, rv.r, #34;
  }

  return rv.r;
}

#endif
