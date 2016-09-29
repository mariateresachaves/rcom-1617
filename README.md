# Redes de Computadores

## Primeira experiência de programação da porta série

Um grupo faz um programa que lê uma linha do stdin e envia para a porta série.

O outro grupo lê os caracteres da porta série e, assim que tiver a linha completa, imprime-a no stdout.

Utilização da porta série em modo não canónico.

Usar os exemplos de código fornecidos (writenoncanonical.c e noncanonical.c), com as alterações necessárias para cumprir o objectivo descrito.

O trabalho pode ser completado do seguinte modo: o segundo grupo reenvia os caracteres que recebeu; o primeiro grupo efectua a operação de leitura e imprime a linha no stdout.

**Emissor:**
* lê uma linha do stdin.
* sugestão: utilizar a função gets() para obter a linha do stdin;
* determina número de caracteres até ‘\0’;
* escreve na porta série os caracteres lidos usando a configuração em modo não canónico (incluir
o ‘\0’ para indicar o fim da transmissão ao receptor);
* lê da porta série (ver Receptor) a string que deve ter sido reenviada pelo Receptor.
Usar writenoncanonical.c e noncanonical.c

**Receptor:**
* lê os caracteres (um a um) da porta série, em modo não canónico, até receber o caracter de fim
de string (‘\0’);
* imprime a string com printf("%s\n", ...).
* reenvia a a string recebida do Emissor, escrevendo os caracteres respectivos (incluindo ‘\0’) na porta série (ver Emissor).
Usar noncanonical.c e writenoncanonical.c

## Estabelecimento da ligação lógica

1. Implementar o estabelecimento da ligação lógica (ver guião do primeiro trabalho laboratorial). 
2. Implementar o mecanismo de retransmissão, do lado do Emissor, com time-out.

* Quando o Receptor não responde (ou o Emissor não recebe uma resposta válida), o Emissor reenvia trama SET ao fim do intervalo de time-out configurado (por exemplo, 3 s) e retransmite no máximo 3 vezes.

* Usar a função alarm (man alarm) – exemplo de código em alarme.c (ver Documentação do 1º trabalho)

**Nota:** A leitura de caracteres da porta série no modo não canónico pode ser controlada por meio de dois parâmetros, conforme se descreve a seguir (extraído de “Serial-Programming-HOWTO”, disponível na Documentação do 1º trabalho):

**Non-Canonical Input Processing**

In non-canonical input processing mode, input is not assembled into lines and input processing (erase, kill,
delete, etc.) does not occur. Two parameters control the behavior of this mode: c_cc[VTIME] sets the
character timer, and c_cc[VMIN] sets the minimum number of characters to receive before satisfying the
read.

* If **MIN > 0 and TIME = 0**, MIN sets the number of characters to receive before the read is satisfied. As TIME is zero, the timer is not used.
* If **MIN = 0 and TIME > 0**, TIME serves as a timeout value. The read will be satisfied if a single character is read, or TIME is exceeded (t = TIME *0.1 s). If TIME is exceeded, no character will be returned.
* If **MIN > 0 and TIME > 0**, TIME serves as an inter-character timer. The read will be satisfied if MIN characters are received, or the time between two characters exceeds TIME. The timer is restarted every time a character is received and only becomes active after the first character has been received.
* If **MIN = 0 and TIME = 0**, read will be satisfied immediately. The number of characters currently available, or the number of characters requested will be returned.

By modifying newtio.c_cc[VTIME] and newtio.c_cc[VMIN] all modes described above can be tested. 
