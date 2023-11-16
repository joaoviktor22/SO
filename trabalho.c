#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define QUANTUM 6


struct Process {
  char name[64];
  int priority;
  int tickets;
  int pid;
  int created_time;
  int start_time;
  int end_time;
  int is_running;
  int status;
  int is_done;
};

typedef struct Process process_t;

process_t processes[10]; // Ajuste o tamanho conforme necessário

int num_processes = 0; // Variável global para o número de processos

process_t *running_process = NULL;

//TODO: mudar o arquivo de entrada para conter apenas o processo e a prioridade
// os tickets devem ser dados depois de ler

// Função para escolher um processo com base em seus tickets
process_t* select_process() {
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
      double aux = processes[i].end_time - processes[i].start_time;
      printf("Processo filho com PID %d terminou com status %d, comecou em %d e terminou em %d (%lf)\n", child_pid, processes[i].status,processes[i].start_time,processes[i].end_time, aux);
    } else {
      // Não deveria acontecer, mas trata caso o PID não seja encontrado
      fprintf(stderr, "Erro: PID %d não encontrado na lista de processos\n", child_pid);
    }
  }
}

int get_ticket(int prioridade){
  if(prioridade == 1) return 2;
  else if(prioridade == 2) return 3;
  else return 4; 
}

// Preenche array de processos
void fill_processes_array() {
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

    // Lê uma linha do arquivo
    fscanf(fp, "%s %d\n", nome, &prioridade);

    // Cria o processo
    strcpy(processes[i].name, nome);
    processes[i].priority = prioridade;
    processes[i].tickets = get_ticket(prioridade);
    processes[i].start_time = -1;
    num_processes++;

    i++;
  }

  // Fecha o arquivo de entrada
  fclose(fp);
}

void busy_wait_for_time_or_process_end(int delta_time) {
  time_t start = time(NULL);
  while (time(NULL) < start + delta_time && running_process != NULL && running_process->is_done != 1) {
    // Espera até o tempo dado passar ou processo finalizar
  }
}

int main() {
  srand(time(NULL));

  // Configurar o handler para SIGCHLD
  signal(SIGCHLD, sigchld_handler);

  fill_processes_array();

  int current_quantum = 0;

  // Criando processos na fila ready
  int i;
  for(i = 0; i < num_processes; i++){
    processes[i].pid = fork();
    if (processes[i].pid == -1) {
      perror("Erro ao criar processo filho");
      exit(EXIT_FAILURE);
    } else if(processes[i].pid == 0) {
      // Roda programa no lugar do filho
      execl("b.out", "b.out", NULL);
    } else {
      processes[i].created_time = time(NULL);
      if(running_process == NULL){
        // Caso seja o primeiro processo, deixa ele continuar rodando
        running_process = &processes[i];
        processes[i].is_running = 1;
        processes[i].start_time = time(NULL);
      } else {
        // Pausa todos os outros processos assim que são criados
        kill(processes[i].pid, SIGSTOP);
      }

      busy_wait_for_time_or_process_end(2);

      current_quantum++;

      // Caso tenha atingido o quantum, tem que trocar o processo rodando
      if(current_quantum > 0 && current_quantum % 3 == 0) {
        kill(running_process->pid, SIGSTOP);
        running_process->is_running = 0;

        running_process = select_process();

        if(running_process == NULL) {
          printf("Acabaram os Processos\n");
        }

        running_process->is_running = 1;
        kill(running_process->pid, SIGCONT);
        if(running_process->start_time == -1) {
          running_process->start_time = time(NULL);
        }
      }
    }
  }

  // Pra alinhar os slices de tempo
  if(select_process() != NULL && current_quantum % 3 != 0) {
    busy_wait_for_time_or_process_end(((current_quantum % 3) - 1) * 2);
  }

  while(1) {
    // Pausa processo que está rodando agora
    kill(running_process->pid, SIGSTOP);
    running_process->is_running = 0;

    printf("'%s' [%d] ==> ", running_process->name, running_process->pid);

    // Loteria do proximo processo
    running_process = select_process();
    if(running_process == NULL) {
      printf("Acabaram os Processos\n");
      break;
    }

    printf("'%s' [PID %d]\n", running_process->name, running_process->pid);

    // Continua o processo sorteado
    kill(running_process->pid, SIGCONT);
    running_process->is_running = 1;
    // Atualiza start_time caso seja a primeira vez rodando
    if(running_process->start_time == -1) {
      running_process->start_time = time(NULL);
    }

    busy_wait_for_time_or_process_end(6);
  }

  return 0;

  // Loop principal
  while (1) {
    // Escolhe o processo a ser executado
    process_t* selected_process = select_process();

    if (selected_process != NULL) {
      if(selected_process->is_running == 0){
        // Cria um novo processo filho
        printf("Criar Processo Filho %s\n",selected_process->name);
        selected_process->pid = fork();
        if (selected_process->pid == -1) {
          perror("Erro ao criar processo filho");
          exit(EXIT_FAILURE);
        }else if (selected_process->pid == 0) {
          // Simulando uma tarefa intensiva na CPU sem sleep
          printf("Processo Filho\n");
          clock_t start_time = clock(); // Marca o início da execução

          execl("b.out","b.out", NULL);

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
      } else {
        //is running
        printf("De novo Processo Filho %s\n",selected_process->name);
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
      printf("Acabaram os Processos\n");
      break;
    }
  }

  return 0;
}