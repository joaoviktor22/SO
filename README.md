# SO

## Entrada e saída

O programa recebe como entrada um arquivo texto de nome `entrada.txt` na seguinte formação:

```
nome_processo prioridade
```

onde `nome do processo` é lido como uma string e `prioridade`,como um inteiro.

A saída é o **Makespan** e **Tempo de Execução** de cada processo.

## Como executar

Para executar o progama, basta digitar no terminal

```bash
make ; ./a.out
```

## Como visualizar durante a execução do programa

Para visualizar mais detalhes durante a execução, basta trocar a flag de `DEBUG` de `0` para `1` diretamente no código em `trabalho.c`.

Para visualizar a tabela de processos em tempo real, no terminal bash, basta usar o seguinte comando

```bash
watch -n 1 -d ps o euid,ruid,tty,tpgid,sess,pgrp,ppid,pid,pcpu,stat,comm
```

## Detalhes sobre a implementação

### Criação dos processos

A imagem a seguir mostra um esquema de como foi implementado o processo da criação da fila **ready** juntamente com a ação do escalonador de loteria.

![Alt imagem](/grafico-escalonador.jpg)

Na imagem temos o exemplo com o processo Pai, que contém o escalonador, e 3 processos filhos.

Na primeira etapa, em 1, temos a preparação em código para fazer a criação dos processos. Essa preparação se dá por meio da criação e população de um array de processos.

Em seguida, temos a execução do primeiro processo lido do arquivo de entradam, o Filho1. Ele é lido e é executado por 2 segundos. Quando este intervalo acaba, o processo Pai cria o Filho2. Ao criar, ele executa o Filho2 mas logo pausa sua execução pois o Filho1 estava executando antes. Depois, o Filho1 continua sua execução por 2 segundos e o Filho3 é criado da mesma forma que o Filho2. Esse processo de criação pode ser visto nas setas indicadas por 2 e 3 na imagem acima.

A alternância entre execução do processo Filho1 e a criação dos outros processos filhos se dará até que o tempo de execução do Filho1 atinja o quantum do escalonador de 6 segundos. Atingindo esse tempo, o escalonador de loteria entra em ação. No caso da imagem, o processo escolhido pelo escalonador foi o Filho3. É possível ver a ação do escalonador nos pontos indicados por 4 e 5.

### Escalonador

O escalonador implementado é o escalonador de loteria. Ele escolhe de maneira randômica um ticket e o processo que tiver o ticket será executado.

### Sobre os processos

Todos os processos criados executam o mesmo código. É um código simples com um `for` com diversas interações de tal forma que o tempo total de sua execução seja o mais próximo possível de 30 segundos. Esse código pode ser encontrado em `teste.c`.
