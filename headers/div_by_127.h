#ifndef DIV_BY_127_INCLUDED
#define DIV_BY_127_INCLUDED

#ifndef DIV_T_DEFINED
#define DIV_T_DEFINED
typedef struct div_t { unsigned q; unsigned r; } div_t;
#endif

static
__inline
__pure
__value_in_regs
div_t
div_by_127_qr(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #127;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #7;
     ADD     rv.q, rv.q, rv.q, LSR #14;
     ADD     rv.q, rv.q, rv.q, LSR #28;
     MOV     rv.q, rv.q, LSR #6;
     RSB     tmp, rv.q, rv.q, ASL #7;
     SUBS    rv.r, rv.r, tmp, ASL #0;
     ADDPL   rv.q, rv.q, #1;
     ADDMI   rv.r, rv.r, #127;
  }

  return rv;
}

static
__inline
__pure
unsigned
div_by_127_q(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #127;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #7;
     ADD     rv.q, rv.q, rv.q, LSR #14;
     ADD     rv.q, rv.q, rv.q, LSR #28;
     MOV     rv.q, rv.q, LSR #6;
     RSB     tmp, rv.q, rv.q, ASL #7;
     SUBS    rv.r, rv.r, tmp, ASL #0;
     ADDPL   rv.q, rv.q, #1;
  }

  return rv.q;
}

static
__inline
__pure
unsigned
div_by_127_r(register unsigned d_)
{
  register div_t rv;
  register unsigned tmp;

  rv.q = d_;

  __asm
  {
     SUB     rv.r, rv.q, #127;
     MOV     rv.q, rv.q, LSR #1;
     ADD     rv.q, rv.q, rv.q, LSR #7;
     ADD     rv.q, rv.q, rv.q, LSR #14;
     ADD     rv.q, rv.q, rv.q, LSR #28;
     MOV     rv.q, rv.q, LSR #6;
     RSB     tmp, rv.q, rv.q, ASL #7;
     SUBS    rv.r, rv.r, tmp, ASL #0;
     ADDMI   rv.r, rv.r, #127;
  }

  return rv.r;
}

#endif
