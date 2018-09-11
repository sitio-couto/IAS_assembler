#!/usr/bin/python
# -*- coding: iso-8859-15 -*-
'''
Colocar esse script na pasta do projeto
Mudar o path dos testes ou colocar os testes na pasta do projeto
Rodar o script: python {nome do script}
Os diffs estarão no arquivo da variavel output_file_path
'''
from subprocess import call, check_output, Popen, PIPE, STDOUT
from os import walk
from difflib import Differ


path_to_the_tests = 'testes'
# This file will be created with the diffs for the tests you got wrong
output_file_path = 'tester_output.txt'
# Set this to false if testing part 2
part1 = False

# Compiling your code
error = check_output(["make","-B"])
if error != 'gcc emitirMapaDeMemoria.c main.c processarEntrada.c token.c -lm -o montador.x\n':
    try:
        sys.exit() # Creating a exception to quit, don't ever do this
    except:
        print 'You fucked up, go fix your code'
        print error

# Getting all the tests
output_list = []
for (dirpath, dirnames, filenames) in walk(path_to_the_tests):
    output_list = [name for name in filenames if ((part1 and '.parte1.res' in name) or (not part1 and '.res' in name and '.parte1' not in name))]

# creating or cleaning output file
with open(output_file_path,'w') as output_file:
    output_file.write('- o que faltou no seu arquivo\n')
    output_file.write('+ o que tem a mais no seu arquivo')
    output_file.write('\n--------------------------\n')

right = 0
wrong = 0
d = Differ()
for output_path in output_list:
    input_path = output_path.replace('.res','.in').replace('.parte1','')

    # Getting answer
    with open(path_to_the_tests+'/'+output_path) as right_file:
        right_output = right_file.read()
        #Running your code
        out = Popen(["./montador.x", path_to_the_tests+input_path], stdout=PIPE, stderr=STDOUT).stdout.read()
    if right_output == out:
        print 'Right - ' + input_path
        right += 1
    else:
        print 'Wrong - ' + input_path
        wrong += 1
        with open(output_file_path,'a') as output_file:
            # Diff of your output and the correct one
            difference = d.compare(right_output.split('\n'), out.split('\n'))
            # Printing diff into file
            output_file.write(input_path+'\n')
            output_file.write('\n'.join(difference))
            output_file.write('\n--------------------------\n')

print 'Right:'+str(right)
print 'Wrong:'+str(wrong)
print 'Total:'+str(right+wrong)
print 'Total:'+str(((right*1.0)/(right+wrong))*100.0)+'%'

if (wrong == 0):
    print 'GG'
