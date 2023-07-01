mcro my_instruction_mcro
sub @r1, @r4
bne END
endmcro
my_instruction_mcro
mcro my_data_mcro
.data 1, 2, 3
.data -4, -5, -6
.data 7, 8, 9
endmcro
my_data_mcro