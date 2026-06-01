#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    printf("[Target] Програма запустилася. Починаю виділення пам'яті...\n");
    
    size_t block_size = 10 * 1024 * 1024;
    int counter = 0;

    while (1) {
        char *ptr = malloc(block_size);
        
        if (ptr == NULL) {
            printf("[Target] Помилка: malloc повернув NULL! Пам'ять закінчилася.\n");
            return 5;
        }
        memset(ptr, 0, block_size);
        counter += 10;
        printf("[Target] Успішно виділено: %d МБ\n", counter);
        sleep(1);
    }

    return 0;
}