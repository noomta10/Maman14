;check source operand for mov, cmp, add, sub
mov , no_source_operand
mov 1, validMov

;check source operand for lea
lea , no_source_operand
lea @r3, badLea
lea A, badLea
lea 1, validLea

;check source operand for not, clr, inc, dec, jmp, bne, red, prn, jsr, rts, SET_PREV_POINTER
not validNot
not @r3, badNot
not A, badNot
not 1, badNot



