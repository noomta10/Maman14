;check source operand for mov, cmp, add, sub
mov , no_source_operand
mov 1, validMov

;check source operand for lea
lea , no_source_operand
lea @r3, badLea
lea A, goodLea
lea 1, badLea

;check source operand for not, clr, inc, dec, jmp, bne, red, prn, jsr, rts
not validNot
not @r3, badNot
not A, badNot
not 1, badNot

;check target mov, add, sub, not, clr, lea, inc, dec, jmp, bne, red, jsr
mov validMov, validMov
mov validMov, @r3
mov badMov, 1

;check target operand for cmp, prn
cmp validMov, validMov
cmp validMov, @r3
cmp validMov, 1

;check target operand for rts, stop
rts
rts 1
rts @r3
rts A

