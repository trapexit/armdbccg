#ifndef DIV_BY_30_INCLUDED
#define DIV_BY_30_INCLUDED

#ifndef DIV_T_DEFINED
#define DIV_T_DEFINED
typedef struct div_t { unsigned q; unsigned r; } div_t;
#endif

static
__inline
__pure
__value_in_regs
div_t
div_by_30_qr(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #30;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #4;
     RSB     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDPL   rv.q, rv.q, #1;
     ADDMI   rv.r, rv.r, #30;
  }

  return rv;
}

static
__inline
__pure
unsigned
div_by_30_q(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #30;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #4;
     RSB     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDPL   rv.q, rv.q, #1;
  }

  return rv.q;
}

static
__inline
__pure
unsigned
div_by_30_r(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #30;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #4;
     RSB     tmp, rv.q, rv.q, ASL #4;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDMI   rv.r, rv.r, #30;
  }

  return rv.r;
}

#endif
