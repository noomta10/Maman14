MAIN: mov @r3 ,LENGTH
LOOP: jmp L1
mcro m1 
sub @r1, @r4
bne END
endmcro
prn -5
bne LOOP
m1
mcro cool
hello world
endmcro

123
cool
123