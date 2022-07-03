from random import random
from math import pi

tamanho = 1000

def rng_array(titulo, max):
    txt = "float " + titulo + "[" + str(tamanho) + "] = {\n" 
    for _ in range(tamanho):
        value = random()*max
        txt = txt + "\t" + str(value);
        if(_ < tamanho-1):
            txt = txt + ",\n"
    txt = txt + "\n};\n\n"
    return txt

arquivo = open('rngout.txt', 'w')

arquivo.write("#define LENGTH_LUT " + str(tamanho) + "\n\n")
arquivo.writelines(rng_array("vetorCorrenteX", 3))
arquivo.writelines(rng_array("vetorCorrenteY", 3))
arquivo.writelines(rng_array("vetorCorrenteZ", 3))
arquivo.writelines(rng_array("vetorVelAngX", 10*pi))
arquivo.writelines(rng_array("vetorVelAngY", 10*pi))
arquivo.writelines(rng_array("vetorVelAngZ", 10*pi))
arquivo.writelines(rng_array("vetorPosicaoX", 1000))
arquivo.writelines(rng_array("vetorPosicaoY", 1000))
arquivo.writelines(rng_array("vetorPosicaoZ", 1000))


arquivo.close()