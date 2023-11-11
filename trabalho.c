#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define QUANTUM 6
#define MAX_ITERATIONS 2000000000 // Ajuste este valor conforme necessário

struct Process {
  char name[64];
  int priority;
  int tickets;
  int pid;
  int start_time;
  int end_time;
  int is_running;
  int status;
  int is_done;
};

struct Process processes[10]; // Ajuste o tamanho conforme necessário

int num_processes = 0; // Variável global para o número de processos

// Função para escolher um processo com base em seus tickets
struct Process* select_process() {
  int total_tickets = 0;
  for (int i = 0; i < num_processes; i++) {
      if(!processes[i].is_done){
       total_tickets += processes[i].tickets;
      }
  }
  if(total_tickets == 0){
   return NULL;
  }

  int winning_ticket = rand() % total_tickets;
  int current_ticket = 0;

  for (int i = 0; i < num_processes; i++) {
      if(!processes[i].is_done){
       current_ticket += processes[i].tickets;
       if (winning_ticket < current_ticket) {
          return &processes[i];
       }
      }
  }

  return NULL;
}

void sigchld_handler(int signo) {
   (void)signo;
   int status;
   pid_t child_pid;

   // Espera por um processo filho sem bloquear
   while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
       // Encontrar o índice do processo filho no array
       int i;
       for (i = 0; i < num_processes; i++) {
           if (processes[i].pid == child_pid) {
               break;
           }
       }

       // Se encontrado, processa as informações sobre o processo filho
       if (i < num_processes) {
           processes[i].status = status;
           processes[i].is_done = 1;
           processes[i].end_time = time(NULL);
           printf("Processo filho com PID %d terminou com status %d, comecou em %d e terminou em %d\n", child_pid, processes[i].status,processes[i].start_time,processes[i].end_time);
       } else {
           // Não deveria acontecer, mas trata caso o PID não seja encontrado
           fprintf(stderr, "Erro: PID %d não encontrado na lista de processos\n", child_pid);
       }
   }
}

// Criar processos
void create_processes() {
  // Abre o arquivo de entrada
  FILE *fp = fopen("entrada.txt", "r");
  if (fp == NULL) {
      perror("Erro ao abrir o arquivo de entrada");
      exit(EXIT_FAILURE);
  }

  // Lê o arquivo de entrada
  int i = 0;
  while (!feof(fp)) {
      char nome[64];
      int prioridade;
      int tickets;

      // Lê uma linha do arquivo
      fscanf(fp, "%s %d %d\n", nome, &prioridade, &tickets);

      // Cria o processo
      strcpy(processes[i].name, nome);
      processes[i].priority = prioridade;
      processes[i].tickets = tickets;
      num_processes++;

      i++;
  }

  // Fecha o arquivo de entrada
  fclose(fp);
}

int main() {
  srand(time(NULL));

  // Configurar o handler para SIGCHLD
  signal(SIGCHLD, sigchld_handler);

  create_processes();

  // Loop principal
  while (1) {
      // Escolhe o processo a ser executado
      struct Process* selected_process = select_process();

      if (selected_process != NULL) {
          if(selected_process->is_running == 0){
              // Cria um novo processo filho
              printf("Criar Processo Filho %s\n",selected_process->name);
              selected_process->pid = fork();
              if (selected_process->pid == -1) {
                  perror("Erro ao criar processo filho");
                  exit(EXIT_FAILURE);
              }

              if (selected_process->pid == 0) {
                  // Simulando uma tarefa intensiva na CPU sem sleep
                  printf("Processo Filho\n");
                  clock_t start_time = clock(); // Marca o início da execução

                  int i;
                  for (i = 0;i < MAX_ITERATIONS ; i++) {
                  //multiplicação simples para consumir CPU
                      int result = i * i;


                  }

                  clock_t end_time = clock(); // Marca o final da execução

                 // Cálculo do tempo de CPU utilizado
                 double cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
                 printf("Processo filho (%s) utilizou %.2f segundos de CPU.\n", selected_process->name, cpu_time_used);

                  exit(0);
              }else{
                  // Processo pai
                  printf("Processo Pai\n");
                  selected_process->is_running = 1;
                  selected_process->start_time = time(NULL);
                  // Aguardar até o próximo quantum (aproximadamente 6 segundos)
                  time_t start = time(NULL);
                  while (time(NULL) - start < QUANTUM) {
                  // Espera até o tempo de quantum passar
                  }
                  printf("Quantum\n");
                  // Parar execucao do processo filho apos quantum
                  kill(selected_process->pid, SIGSTOP);
              }
          } else{
              //is running
              printf("Denovo Processo Filho %s\n",selected_process->name);
              // Continuar Processo por 6 segundos aproximadamente
              kill(selected_process->pid, SIGCONT);
              time_t start = time(NULL);
              while (time(NULL) - start < QUANTUM) {
              // Espera até o tempo de quantum passar
              }
              printf("Quantum Denovo\n");
              // Parar execucao do processo
              kill(selected_process->pid, SIGSTOP);
          }
      } else {
   printf("Acabou os Processos\n");
   break;      
   }
  }

  return 0;
}