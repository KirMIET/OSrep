#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>

// Глобальные переменные
long long global_result = 1;  // Глобальный результат
pthread_mutex_t mutex;        // Мьютекс для синхронизации

// Структура для передачи аргументов в поток
typedef struct {
    int start;      // Начало диапазона
    int end;        // Конец диапазона
    int mod;        // Модуль
} thread_args_t;

// Функция для вычисления произведения в диапазоне по модулю
void* compute_partial(void* args) {
    thread_args_t* data = (thread_args_t*)args;
    long long partial_result = 1;
    
    printf("Поток: вычисляю диапазон %d-%d\n", data->start, data->end);
    
    // Вычисляем произведение чисел в заданном диапазоне по модулю
    for (int i = data->start; i <= data->end; i++) {
        partial_result = (partial_result * i) % data->mod;
    }
    
    // Синхронизированное обновление глобального результата
    pthread_mutex_lock(&mutex);
    global_result = (global_result * partial_result) % data->mod;
    pthread_mutex_unlock(&mutex);
    
    printf("Поток: частичный результат для %d-%d = %lld\n", 
           data->start, data->end, partial_result);
    
    free(data);
    return NULL;
}

int main(int argc, char* argv[]) {
    int k = 0;          // Число для вычисления факториала
    int pnum = 1;       // Количество потоков
    int mod = 0;        // Модуль
    
    // Парсинг аргументов командной строки
    static struct option long_options[] = {
        {"pnum", required_argument, 0, 'p'},
        {"mod", required_argument, 0, 'm'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "k:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'k':
                k = atoi(optarg);
                if (k < 0) {
                    fprintf(stderr, "Ошибка: k должно быть неотрицательным\n");
                    return 1;
                }
                break;
            case 'p': // --pnum
                pnum = atoi(optarg);
                if (pnum <= 0) {
                    fprintf(stderr, "Ошибка: pnum должен быть положительным\n");
                    return 1;
                }
                break;
            case 'm': // --mod
                mod = atoi(optarg);
                if (mod <= 0) {
                    fprintf(stderr, "Ошибка: mod должен быть положительным\n");
                    return 1;
                }
                break;
        }
    }
    
    if (k == 0 || mod == 0) {
        fprintf(stderr, "Ошибка: необходимо указать k и mod\n");
        return 1;
    }
    
    if (k == 0) {
        printf("0! mod %d = 1\n", mod);
        return 0;
    }
    
    printf("Вычисление %d! mod %d с использованием %d потоков\n", k, mod, pnum);
    
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Ошибка инициализации мьютекса\n");
        return 1;
    }
    
    pthread_t threads[pnum];
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    int current_start = 1;
    
    for (int i = 0; i < pnum; i++) {
        int current_end = current_start + numbers_per_thread - 1;
        if (i < remainder) {
            current_end++; 
        }
        
        if (current_end > k) {
            current_end = k;
        }
        
        thread_args_t* args = malloc(sizeof(thread_args_t));
        if (args == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            return 1;
        }
        args->start = current_start;
        args->end = current_end;
        args->mod = mod;
        
        if (pthread_create(&threads[i], NULL, compute_partial, args) != 0) {
            fprintf(stderr, "Ошибка создания потока %d\n", i);
            free(args);
            continue;
        }
        
        printf("Создан поток %d для диапазона %d-%d\n", i, current_start, current_end);
        current_start = current_end + 1;
        
        if (current_start > k) {
            pnum = i + 1; 
            break;
        }
    }
    
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\nРезультат: %d! mod %d = %lld\n", k, mod, global_result);
    
    // Уничтожение мьютекса
    pthread_mutex_destroy(&mutex);
    
    return 0;
}