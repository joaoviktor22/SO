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

Explicar aqui como foi implementado
