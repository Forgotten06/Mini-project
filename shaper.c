#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int opt;
    long cpu_limit = -1;
    long mem_limit = -1;
    long file_limit = -1;
    while ((opt = getopt(argc, argv, "c:m:f:")) != -1) {
        switch (opt) {
            case 'c':
                cpu_limit = atol(optarg);
                break;
            case 'm':
                mem_limit = atol(optarg);
                break;
            case 'f':
                file_limit = atol(optarg);
                break;
            default:
                fprintf(stderr, "Використання: %s [-c секунди] [-m байти] [-f файли] <команда> [аргументи...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Помилка: Не вказано команду для запуску!\n");
        fprintf(stderr, "Приклад: %s -c 2 ./loop\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();

    if (pid < 0) {
        perror("Помилка fork()");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        struct rlimit old_cpu;
        if (getrlimit(RLIMIT_CPU, &old_cpu) == 0) {
            printf("[Дочірній] Початковий ліміт CPU системи (Soft): %ld сек, (Hard): %ld сек\n", 
                    (long)old_cpu.rlim_cur, (long)old_cpu.rlim_max);
        }
        if (cpu_limit > 0) {
            struct rlimit lim;
            lim.rlim_cur = cpu_limit;
            lim.rlim_max = cpu_limit + 1;
            if (setrlimit(RLIMIT_CPU, &lim) < 0) {
                perror("[Дочірній] Помилка setrlimit(RLIMIT_CPU)");
                exit(EXIT_FAILURE);
            }
        }
        if (mem_limit > 0) {
            struct rlimit lim;
            lim.rlim_cur = mem_limit;
            lim.rlim_max = mem_limit;
            if (setrlimit(RLIMIT_AS, &lim) < 0) {
                perror("[Дочірній] Помилка setrlimit(RLIMIT_AS)");
                exit(EXIT_FAILURE);
            }
        }
        if (file_limit > 0) {
            struct rlimit lim;
            lim.rlim_cur = file_limit;
            lim.rlim_max = file_limit;
            if (setrlimit(RLIMIT_NOFILE, &lim) < 0) {
                perror("[Дочірній] Помилка setrlimit(RLIMIT_NOFILE)");
                exit(EXIT_FAILURE);
            }
        }
        printf("[Дочірній] Запускаю: %s\n", argv[optind]);
        execvp(argv[optind], &argv[optind]);
        perror("[Дочірній] Помилка execvp()");
        exit(EXIT_FAILURE);

    } else {
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("Помилка waitpid()");
            exit(EXIT_FAILURE);
        }

        printf("\n=== ЗВІТ ОБМЕЖУВАЧА РЕСУРСІВ ===\n");
        if (WIFEXITED(status)) {
            printf("Процес завершився нормально.\nКод повернення: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            printf("Процес був примусово ЗАВЕРШЕНИЙ ОС.\nСигнал: %d (%s)\n", sig, strsignal(sig));
            
            if (sig == SIGXCPU) {
                printf("Причина: Перевищено ліміт процесорного часу (CPU time limit exceeded).\n");
            } else if (sig == SIGSEGV) {
                printf("Причина: Спроба доступу до недозволеної пам'яті (можливо, через обмеження RLIMIT_AS).\n");
            }
        }
        printf("================================\n");
    }

    return 0;
}