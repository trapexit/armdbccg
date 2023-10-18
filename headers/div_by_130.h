#ifndef DIV_BY_130_INCLUDED
#define DIV_BY_130_INCLUDED

#ifndef DIV_T_DEFINED
#define DIV_T_DEFINED
typedef struct div_t { unsigned q; unsigned r; } div_t;
#endif

static
__inline
__pure
__value_in_regs
div_t
div_by_130_qr(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #130;
     SUB     rv.q, rv.q, rv.q, LSR #6;
     ADD     rv.q, rv.q, rv.q, LSR #12;
     ADD     rv.q, rv.q, rv.q, LSR #24;
     MOV     rv.q, rv.q, LSR #7;
     ADD     tmp, rv.q, rv.q, ASL #6;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDPL   rv.q, rv.q, #1;
     ADDMI   rv.r, rv.r, #130;
  }

  return rv;
}

static
__inline
__pure
unsigned
div_by_130_q(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #130;
     SUB     rv.q, rv.q, rv.q, LSR #6;
     ADD     rv.q, rv.q, rv.q, LSR #12;
     ADD     rv.q, rv.q, rv.q, LSR #24;
     MOV     rv.q, rv.q, LSR #7;
     ADD     tmp, rv.q, rv.q, ASL #6;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDPL   rv.q, rv.q, #1;
  }

  return rv.q;
}

static
__inline
__pure
unsigned
div_by_130_r(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #130;
     SUB     rv.q, rv.q, rv.q, LSR #6;
     ADD     rv.q, rv.q, rv.q, LSR #12;
     ADD     rv.q, rv.q, rv.q, LSR #24;
     MOV     rv.q, rv.q, LSR #7;
     ADD     tmp, rv.q, rv.q, ASL #6;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDMI   rv.r, rv.r, #130;
  }

  return rv.r;
}

#endif
