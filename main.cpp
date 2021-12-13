#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

// семафор, имитирующий табак и спички на столе
sem_t tobacco_and_matches_sem;
// семафор, имитирующий спички и бумагу на столе
sem_t matches_and_paper_sem;
// семафор, имитирующий бумагу и табак на столе
sem_t paper_and_tobacco_sem;
// семафор, отвечающий за то, покурил ли курильщик
sem_t has_smoked;
// время курения первого курильщика (с бумагой)
int smoke_time_1;
// время курения второго курильщика (с табаком)
int smoke_time_2;
// время курения третьего курильщика (со спичками)
int smoke_time_3;
// количество наборов ингредиентов в запасе у агента-посредника
int number_of_ingredients;

// метод, который имитирует процесс курения
void smoke(int time) {
    // поток засыпает на заданное время
    sleep(time);
    // после пробуждения поток пишет, сколько секунд он поспал
    // (на языке задания на экран выводится время,
    // которое потребовалось курильщику, чтобы выкурить сигарету)
    printf("%d seconds left...\n", time);
    --number_of_ingredients;
}

// функция курильщика с бумагой
void *smokerWithPaper(void *arg) {
    while (true) {
        // семафор табака и спичек заблокирован, то есть курильщик забрал их со стола
        sem_wait(&tobacco_and_matches_sem);
        printf("Smoker with paper grabbed matches and tobacco. He is smoking now.\n");
        // имитация процесса курения
        smoke(smoke_time_1);
        printf("Smoker with paper finished smoking.\n");
        // курильщик сообщает посреднику, что выкурил сигарету
        sem_post(&has_smoked);
        printf("\n");
    }
}

// функция курильщика с табаком
void *smokerWithTobacco(void *arg) {
    while (true) {
        // семафор бумаги и спичек заблокирован, то есть курильщик забрал их со стола
        sem_wait(&matches_and_paper_sem);
        printf("Smoker with tobacco grabbed matches and paper. He is smoking now.\n");
        // имитация процесса курения
        smoke(smoke_time_2);
        printf("Smoker with tobacco finished smoking.\n");
        // курильщик сообщает посреднику, что выкурил сигарету
        sem_post(&has_smoked);
        printf("\n");
    }
}

// функция курильщика со спичками
void *smokerWithMatches(void *arg) {
    while (true) {
        // семафор табака и бумаги заблокирован, то есть курильщик забрал их со стола
        sem_wait(&paper_and_tobacco_sem);
        printf("Smoker with matches grabbed paper and tobacco. He is smoking now.\n");
        // имитация процесса курения
        smoke(smoke_time_3);
        printf("Smoker with matches finished smoking.\n");
        // курильщик сообщает посреднику, что выкурил сигарету
        sem_post(&has_smoked);
        printf("\n");
    }
}

// функция агента-посредника
void *agentFunc(void *arg) {
    // поток курильщика с бумагой
    pthread_t smoker_with_paper_t;
    // поток курильщика с табаком
    pthread_t smoker_with_tobacco_t;
    // поток курильщика со спичками
    pthread_t smoker_with_matches_t;

    // инициализация семафора спичек и табака
    sem_init(&tobacco_and_matches_sem, 0, 0);
    // создание потока курильщика с бумагой
    if (pthread_create(&smoker_with_paper_t, NULL, smokerWithPaper, NULL) == EAGAIN) {
        perror("Impossible to create a thread for smoker with paper.\n");
        return 0;
    }

    // инициализация семафора спичек и бумаги
    sem_init(&matches_and_paper_sem, 0, 0);
    // создание потока курильщика с табаком
    if (pthread_create(&smoker_with_tobacco_t, NULL, smokerWithTobacco, NULL) == EAGAIN) {
        perror("Impossible to create a thread for smoker with tobacco.\n");
        return 0;
    }

    // инициализация семафора табака и бумаги
    sem_init(&paper_and_tobacco_sem, 0, 0);
    // создание потока курильщика со спичками
    if (pthread_create(&smoker_with_matches_t, NULL, smokerWithMatches, NULL) == EAGAIN) {
        perror("Impossible to create a thread for smoker with matches.\n");
        return 0;
    }

    // пока запас ингредиентов не иссякнет, выдавать их
    while (number_of_ingredients > 0) {
        sem_wait(&has_smoked);
        int wait = rand() % 3;
        if (wait == 0 && number_of_ingredients > 0) {
            printf("Agent put tobacco and matches on the table.\n");
            sem_post(&tobacco_and_matches_sem);
        } else if (wait == 1 && number_of_ingredients > 0) {
            printf("Agent put paper and matches on the table.\n");
            sem_post(&matches_and_paper_sem);
        } else if (wait == 2 && number_of_ingredients > 0) {
            printf("Agent put tobacco and paper on the table.\n");
            sem_post(&paper_and_tobacco_sem);
        }
    }
    // завершение потока
    pthread_exit(NULL);
}

int main(int argc, char *arvg[]) {
    // инициализация семафора, отвечающего за сообщение посреднику
    // о том, что курильщик скурил сигарету
    sem_init(&has_smoked, 0, 1);
    // поток посредника
    pthread_t agent_t;
    // инициализация времени курения и количества ингредиентов
    if (argc == 2) {
        number_of_ingredients = atoi(arvg[1]);
        smoke_time_1 = rand() % 5 + 1;
        smoke_time_2 = rand() % 5 + 1;
        smoke_time_3 = rand() % 5 + 1;
    } else if (argc == 5) {
        number_of_ingredients = atoi(arvg[1]);
        smoke_time_1 = atoi(arvg[2]);
        smoke_time_2 = atoi(arvg[3]);
        smoke_time_3 = atoi(arvg[4]);
    } else if (argc == 1) {
        number_of_ingredients = rand() % 500 + 1;
        smoke_time_1 = rand() % 5 + 1;
        smoke_time_2 = rand() % 5 + 1;
        smoke_time_3 = rand() % 5 + 1;
    } else {
        printf("Input data is incorrect. Try to run program again with the correct input data.");
        return 0;
    }

    // создание потока посредника
    if (pthread_create(&agent_t, NULL, agentFunc, NULL) == EAGAIN) {
        perror("Impossible to create a thread for agent.\n");
        return 0;
    }

    // ожидание потока посредника, пока он не закончит выдавать ингредиенты
    pthread_join(agent_t, NULL);
    if (smoke_time_1 <= 0 && smoke_time_2 <= 0 && smoke_time_3 <= 0 && number_of_ingredients <= 0) {
        printf("Input data is incorrect. Try to run program again with the correct input data.");
    } else {
        printf("Agent gave away all the ingredients. Smoking finished.");
    }
    exit(0);
}
