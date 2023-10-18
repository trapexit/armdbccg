#ifndef DIV_BY_66_INCLUDED
#define DIV_BY_66_INCLUDED

#ifndef DIV_T_DEFINED
#define DIV_T_DEFINED
typedef struct div_t { unsigned q; unsigned r; } div_t;
#endif

static
__inline
__pure
__value_in_regs
div_t
div_by_66_qr(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #66;
     SUB     rv.q, rv.q, rv.q, LSR #5;
     ADD     rv.q, rv.q, rv.q, LSR #10;
     ADD     rv.q, rv.q, rv.q, LSR #20;
     MOV     rv.q, rv.q, LSR #6;
     ADD     tmp, rv.q, rv.q, ASL #5;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDPL   rv.q, rv.q, #1;
     ADDMI   rv.r, rv.r, #66;
  }

  return rv;
}

static
__inline
__pure
unsigned
div_by_66_q(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #66;
     SUB     rv.q, rv.q, rv.q, LSR #5;
     ADD     rv.q, rv.q, rv.q, LSR #10;
     ADD     rv.q, rv.q, rv.q, LSR #20;
     MOV     rv.q, rv.q, LSR #6;
     ADD     tmp, rv.q, rv.q, ASL #5;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDPL   rv.q, rv.q, #1;
  }

  return rv.q;
}

static
__inline
__pure
unsigned
div_by_66_r(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #66;
     SUB     rv.q, rv.q, rv.q, LSR #5;
     ADD     rv.q, rv.q, rv.q, LSR #10;
     ADD     rv.q, rv.q, rv.q, LSR #20;
     MOV     rv.q, rv.q, LSR #6;
     ADD     tmp, rv.q, rv.q, ASL #5;
     SUBS    rv.r, rv.r, tmp, ASL #1;
     ADDMI   rv.r, rv.r, #66;
  }

  return rv.r;
}

#endif
