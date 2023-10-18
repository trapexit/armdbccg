#ifndef DIV_BY_36_INCLUDED
#define DIV_BY_36_INCLUDED

#ifndef DIV_T_DEFINED
#define DIV_T_DEFINED
typedef struct div_t { unsigned q; unsigned r; } div_t;
#endif

static
__inline
__pure
__value_in_regs
div_t
div_by_36_qr(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #36;
     SUB     rv.q, rv.q, rv.q, LSR #3;
     ADD     rv.q, rv.q, rv.q, LSR #6;
     ADD     rv.q, rv.q, rv.q, LSR #12;
     ADD     rv.q, rv.q, rv.q, LSR #24;
     MOV     rv.q, rv.q, LSR #5;
     ADD     tmp, rv.q, rv.q, ASL #3;
     SUBS    rv.r, rv.r, tmp, ASL #2;
     ADDPL   rv.q, rv.q, #1;
     ADDMI   rv.r, rv.r, #36;
  }

  return rv;
}

static
__inline
__pure
unsigned
div_by_36_q(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #36;
     SUB     rv.q, rv.q, rv.q, LSR #3;
     ADD     rv.q, rv.q, rv.q, LSR #6;
     ADD     rv.q, rv.q, rv.q, LSR #12;
     ADD     rv.q, rv.q, rv.q, LSR #24;
     MOV     rv.q, rv.q, LSR #5;
     ADD     tmp, rv.q, rv.q, ASL #3;
     SUBS    rv.r, rv.r, tmp, ASL #2;
     ADDPL   rv.q, rv.q, #1;
  }

  return rv.q;
}

static
__inline
__pure
unsigned
div_by_36_r(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #36;
     SUB     rv.q, rv.q, rv.q, LSR #3;
     ADD     rv.q, rv.q, rv.q, LSR #6;
     ADD     rv.q, rv.q, rv.q, LSR #12;
     ADD     rv.q, rv.q, rv.q, LSR #24;
     MOV     rv.q, rv.q, LSR #5;
     ADD     tmp, rv.q, rv.q, ASL #3;
     SUBS    rv.r, rv.r, tmp, ASL #2;
     ADDMI   rv.r, rv.r, #36;
  }

  return rv.r;
}

#endif
