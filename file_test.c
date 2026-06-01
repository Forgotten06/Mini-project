#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    printf("[Target] Програма запустилася. Починаю відкривати файли...\n");
    int count = 0;

    while (1) {
        int fd = open("/dev/null", O_RDONLY);
        
        if (fd < 0) {
            printf("[Target] Помилка відкриття: %s (код: %d)\n", strerror(errno), errno);
            return 9;
        }

        count++;
        printf("[Target] Успішно відкрито файлів: %d (дескриптор: %d)\n", count, fd);
    }

    return 0;
}