#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = 0; // Таймаут в секундах (0 - без таймаута)
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {
      {"seed", required_argument, 0, 0},
      {"array_size", required_argument, 0, 0},
      {"pnum", required_argument, 0, 0},
      {"timeout", required_argument, 0, 0}, // Новый параметр --timeout
      {"by_files", no_argument, 0, 'f'},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0: // --seed
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("seed must be a positive number\n");
                return 1;
            }
            break;
          case 1: // --array_size
            array_size = atoi(optarg);
            if (array_size <= 0) {
                printf("array_size must be a positive number\n");
                return 1;
            }
            break;
          case 2: // --pnum
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("pnum must be a positive number\n");
                return 1;
            }
            break;
          case 3: // --timeout (НОВЫЙ ПАРАМЕТР)
            timeout = atoi(optarg);
            if (timeout <= 0) {
                printf("timeout must be a positive number\n");
                return 1;
            }
            break;
          case 4: // --by_files
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"] [--by_files]\n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  // Создаем pipes для коммуникации с дочерними процессами
  int pipes[pnum][2];
  if (!with_files) {
    for (int i = 0; i < pnum; i++) {
      if (pipe(pipes[i]) == -1) {
        perror("pipe failed");
        free(array);
        return 1;
      }
    }
  }

  int active_child_processes = 0;
  pid_t child_pids[pnum]; // Массив для хранения PID дочерних процессов
  
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // Создаем дочерние процессы
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      active_child_processes += 1;
      child_pids[i] = child_pid; // Сохраняем PID дочернего процесса
      
      if (child_pid == 0) {
        // Дочерний процесс
        int start = i * (array_size / pnum);
        int end = (i == pnum - 1) ? array_size : (i + 1) * (array_size / pnum);

        struct MinMax local_min_max = GetMinMax(array, start, end);

        if (with_files) {
          char filename[20];
          sprintf(filename, "min_max_%d.txt", i);
          FILE *file = fopen(filename, "w");
          if (file != NULL) {
            fprintf(file, "%d %d", local_min_max.min, local_min_max.max);
            fclose(file);
          }
        } else {
          close(pipes[i][0]);
          write(pipes[i][1], &local_min_max.min, sizeof(int));
          write(pipes[i][1], &local_min_max.max, sizeof(int));
          close(pipes[i][1]);
        }
        free(array);
        exit(0);
      }
    } else {
      printf("Fork failed!\n");
      free(array);
      return 1;
    }
  }

  if (timeout > 0) {
    printf("Waiting for child processes with timeout: %d seconds\n", timeout);
    
    // Ждем завершения процессов с таймаутом
    int remaining_time = timeout;
    while (active_child_processes > 0 && remaining_time > 0) {
      sleep(1); // Ждем 1 секунду
      remaining_time--;
      
      // Проверяем, не завершились ли какие-то процессы
      pid_t finished_pid;
      int status;
      while ((finished_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        active_child_processes -= 1;
        if (WIFEXITED(status)) {
          printf("Process %d finished normally\n", finished_pid);
        } else if (WIFSIGNALED(status)) {
          printf("Process %d terminated by signal %d\n", finished_pid, WTERMSIG(status));
        }
      }
    }

    // Если время вышло, а процессы еще остались - посылаем SIGKILL
    if (active_child_processes > 0) {
      printf("Timeout reached! Sending SIGKILL to %d remaining child processes\n", active_child_processes);
      
      for (int i = 0; i < pnum; i++) {
        if (kill(child_pids[i], 0) == 0) {
          if (kill(child_pids[i], SIGKILL) == 0) {
            printf("Sent SIGKILL to process %d\n", child_pids[i]);
          } else {
            perror("Failed to send SIGKILL");
          }
        }
      }
      
      while (active_child_processes > 0) {
        wait(NULL);
        active_child_processes -= 1;
      }
    }
  } else {
    while (active_child_processes > 0) {
      wait(NULL);
      active_child_processes -= 1;
    }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  // Собираем результаты от всех процессов
  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      char filename[20];
      sprintf(filename, "min_max_%d.txt", i);
      FILE *file = fopen(filename, "r");
      if (file != NULL) {
        fscanf(file, "%d %d", &min, &max);
        fclose(file);
        remove(filename); 
      }
    } else {
      close(pipes[i][1]);
      // Пытаемся прочитать данные, но они могут быть недоступны если процесс был убит
      ssize_t bytes_read_min = read(pipes[i][0], &min, sizeof(int));
      ssize_t bytes_read_max = read(pipes[i][0], &max, sizeof(int));
      close(pipes[i][0]);
      
      // Если не удалось прочитать данные (процесс был убит), пропускаем
      if (bytes_read_min != sizeof(int) || bytes_read_max != sizeof(int)) {
        continue;
      }
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}