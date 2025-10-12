#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>

#include <pthread.h>

#include "sum.h"
#include "utils.h"

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;

  // Парсинг аргументов командной строки
  static struct option options[] = {
      {"threads_num", required_argument, 0, 0},
      {"seed", required_argument, 0, 0},
      {"array_size", required_argument, 0, 0},
      {0, 0, 0, 0}
  };

  int option_index = 0;
  while (1) {
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            threads_num = atoi(optarg);
            if (threads_num <= 0) {
              printf("threads_num must be positive\n");
              return 1;
            }
            break;
          case 1:
            seed = atoi(optarg);
            break;
          case 2:
            array_size = atoi(optarg);
            if (array_size <= 0) {
              printf("array_size must be positive\n");
              return 1;
            }
            break;
        }
        break;
      case '?':
        break;
      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (threads_num == 0 || array_size == 0) {
    printf("Usage: %s --threads_num \"num\" --seed \"num\" --array_size \"num\"\n", argv[0]);
    return 1;
  }

  // Генерация массива (не входит в замер времени)
  int *array = malloc(sizeof(int) * array_size);
  if (array == NULL) {
    printf("Memory allocation failed for array\n");
    return 1;
  }
  GenerateArray(array, array_size, seed);

  // Используем динамическое выделение памяти для массивов
  pthread_t *threads = malloc(threads_num * sizeof(pthread_t));
  struct SumArgs *args = malloc(threads_num * sizeof(struct SumArgs));

  if (threads == NULL || args == NULL) {
    printf("Memory allocation failed for threads or args\n");
    free(array);
    return 1;
  }

  // Распределение работы между потоками
  int segment_size = array_size / threads_num;
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * segment_size;
    args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * segment_size;
  }

  // Замер времени начала вычислений
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // Создание потоков
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      free(array);
      free(threads);
      free(args);
      return 1;
    }
  }

  // Сбор результатов
  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    // Явное приведение типа для pthread_join
    void *thread_result;
    pthread_join(threads[i], &thread_result);
    sum = (int)(size_t)thread_result;
    total_sum += sum;
  }

  // Замер времени окончания вычислений
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  // Вычисление времени выполнения
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  free(threads);
  free(args);
  printf("Total: %d\n", total_sum);
  printf("Elapsed time: %fms\n", elapsed_time);
  
  return 0;
}