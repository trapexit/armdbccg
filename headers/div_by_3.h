#ifndef DIV_BY_3_INCLUDED
#define DIV_BY_3_INCLUDED

#ifndef DIV_T_DEFINED
#define DIV_T_DEFINED
typedef struct div_t { unsigned q; unsigned r; } div_t;
#endif

static
__inline
__pure
__value_in_regs
div_t
div_by_3_qr(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #3;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #2;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #1;
     RSB     tmp, rv.q, rv.q, ASL #2;
     SUBS    rv.r, rv.r, tmp, ASL #0;
     ADDPL   rv.q, rv.q, #1;
     ADDMI   rv.r, rv.r, #3;
     CMP     rv.r, #3;
     ADDGE   rv.q, rv.q, #1;
     SUBGE   rv.r, rv.r, #3;
  }

  return rv;
}

static
__inline
__pure
unsigned
div_by_3_q(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #3;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #2;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #1;
     RSB     tmp, rv.q, rv.q, ASL #2;
     SUBS    rv.r, rv.r, tmp, ASL #0;
     ADDPL   rv.q, rv.q, #1;
     CMP     rv.r, #3;
     ADDGE   rv.q, rv.q, #1;
  }

  return rv.q;
}

static
__inline
__pure
unsigned
div_by_3_r(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #3;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #2;
     ADD     rv.q, rv.q, rv.q, LSR #4;
     ADD     rv.q, rv.q, rv.q, LSR #8;
     ADD     rv.q, rv.q, rv.q, LSR #16;
     MOV     rv.q, rv.q, LSR #1;
     RSB     tmp, rv.q, rv.q, ASL #2;
     SUBS    rv.r, rv.r, tmp, ASL #0;
     ADDMI   rv.r, rv.r, #3;
     CMP     rv.r, #3;
     SUBGE   rv.r, rv.r, #3;
  }

  return rv.r;
}

#endif
