#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// ���������Ʒ����������
#define MAX_ITEMS 20
#define MAX_CAPACITY 1000

typedef struct {
    int weight;
    float value;
    float density;  // ��ֵ�ܶ�
    int index;      // ԭʼ����
} Item;

// �ȽϺ���������ֵ�ܶȽ�������
int compare(const void *a, const void *b) {
    Item *itemA = (Item *)a;
    Item *itemB = (Item *)b;
    return (itemB->density - itemA->density) > 0 ? 1 : -1;
}

// 1. ��������ⱳ������
void bruteForce(Item *items, int n, int capacity) {
    clock_t start = clock();
    float maxValue = 0;
    int *bestSet = (int *)calloc(n, sizeof(int));

    printf("������������...\n");
    // ö������2^n�ֿ��ܵ����
    for (int mask = 0; mask < (1 << n); mask++) {
        int totalWeight = 0;
        float totalValue = 0;

        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                totalWeight += items[i].weight;
                totalValue += items[i].value;
            }
        }

        if (totalWeight <= capacity && totalValue > maxValue) {
            maxValue = totalValue;
            for (int i = 0; i < n; i++)
                bestSet[i] = (mask & (1 << i)) ? 1 : 0;
        }
    }

    printf("�����������\n");
    printf("�ܼ�ֵ��%.2f\n", maxValue);
    printf("ѡ����Ʒ������");
    int count = 0;
    for (int i = 0; i < n; i++)
        if (bestSet[i]) count++;
    printf("%d\n", count);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("ִ��ʱ�䣺%I64d ms\n\n", executionTime);

    free(bestSet);
}

// 2. ���ݷ���ⱳ������
void backtracking(Item *items, int n, int capacity) {
    clock_t start = clock();

    // ������Ʒ����
    Item *sortedItems = (Item *)malloc(n * sizeof(Item));
    if (!sortedItems) {
        printf("�ڴ����ʧ�ܣ�\n");
        return;
    }
    memcpy(sortedItems, items, n * sizeof(Item));

    // ����ֵ�ܶ����������Ż�����˳��
    for (int i = 0; i < n; i++) {
        sortedItems[i].density = sortedItems[i].value / sortedItems[i].weight;
    }
    qsort(sortedItems, n, sizeof(Item), compare);

    float maxValue = 0;
    int *bestSet = (int *)calloc(n, sizeof(int));
    int *currentSet = (int *)calloc(n, sizeof(int));

    // �ݹ���ݺ���
    void backtrack(int level, int weight, float value) {
        if (level == n) {
            if (weight <= capacity && value > maxValue) {
                maxValue = value;
                memcpy(bestSet, currentSet, n * sizeof(int));
            }
            return;
        }

        // ����ѡ��ǰ��Ʒ
        currentSet[level] = 1;
        backtrack(level + 1, weight + sortedItems[level].weight, value + sortedItems[level].value);

        // ���ǲ�ѡ��ǰ��Ʒ
        currentSet[level] = 0;
        backtrack(level + 1, weight, value);
    }

    printf("���ݷ�������...\n");
    backtrack(0, 0, 0);

    printf("���ݷ������\n");
    printf("�ܼ�ֵ��%.2f\n", maxValue);
    printf("ѡ����Ʒ������");
    int count = 0;
    for (int i = 0; i < n; i++)
        if (bestSet[i]) count++;
    printf("%d\n", count);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("ִ��ʱ�䣺%I64d ms\n\n", executionTime);

    free(sortedItems);
    free(bestSet);
    free(currentSet);
}

// 3. ��֦����ⱳ�����⣨��֧�޽編˼�룩
void pruning(Item *items, int n, int capacity) {
    clock_t start = clock();

    // ������Ʒ���鲢����ֵ�ܶ�����
    Item *sortedItems = (Item *)malloc(n * sizeof(Item));
    if (!sortedItems) {
        printf("�ڴ����ʧ�ܣ�\n");
        return;
    }
    memcpy(sortedItems, items, n * sizeof(Item));

    // �����ֵ�ܶȲ�����
    for (int i = 0; i < n; i++) {
        sortedItems[i].density = (float)sortedItems[i].value / sortedItems[i].weight;
    }
    qsort(sortedItems, n, sizeof(Item), compare);

    float maxValue = 0;
    int *bestSet = (int *)calloc(n, sizeof(int));
    int *currentSet = (int *)calloc(n, sizeof(int));

    // ����ʣ����Ʒ�������ܼ�ֵ���Ͻ磩
    float calculateBound(int level, int weight, float value) {
        float bound = value;
        int remainingWeight = capacity - weight;

        for (int i = level; i < n && remainingWeight > 0; i++) {
            if (sortedItems[i].weight <= remainingWeight) {
                bound += sortedItems[i].value;
                remainingWeight -= sortedItems[i].weight;
            } else {
                // ����װ�루����������
                bound += sortedItems[i].density * remainingWeight;
                remainingWeight = 0;
            }
        }
        return bound;
    }

    // �ݹ��֦����
    void prune(int level, int weight, float value) {
        if (level == n) {
            if (weight <= capacity && value > maxValue) {
                maxValue = value;
                memcpy(bestSet, currentSet, n * sizeof(int));
            }
            return;
        }

        // ѡ��ǰ��Ʒ�����������������
        if (weight + sortedItems[level].weight <= capacity) {
            currentSet[level] = 1;
            prune(level + 1, weight + sortedItems[level].weight, value + sortedItems[level].value);
        }

        // ���㲻ѡ��ǰ��Ʒ��ʣ���ֵ�Ͻ�
        float bound = calculateBound(level + 1, weight, value);
        if (bound > maxValue) {
            currentSet[level] = 0;
            prune(level + 1, weight, value);
        }
    }

    printf("��֦��������...\n");
    prune(0, 0, 0);

    printf("��֦�������\n");
    printf("�ܼ�ֵ��%.2f\n", maxValue);
    printf("ѡ����Ʒ������");
    int count = 0;
    for (int i = 0; i < n; i++)
        if (bestSet[i]) count++;
    printf("%d\n", count);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("ִ��ʱ�䣺%I64d ms\n\n", executionTime);

    free(sortedItems);
    free(bestSet);
    free(currentSet);
}

// ���ɲ�������
void generateTestData(Item *items, int n) {
    printf("�����������...\n");
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        // ������Χ��1-100
        items[i].weight = rand() % 100 + 1;
        // ��ֵ��Χ��100.00-999.99
        items[i].value = (float)((rand()%900 + 100) * 100 + rand()%100) / 100;
        items[i].index = i;
    }
}

int main() {
    // ���Բ�ͬ��ģ������
    int testSizes[] = {10, 15, 20, 25, 30, 35, 40};  // С��ģ��
    int capacity = 500;  // �̶���������

    printf("===== ���ݷ����������ԱȲ��� =====\n\n");

    for (int i = 0; i < 7; i++) {
        int n = testSizes[i];
        printf("=== ���Թ�ģ����Ʒ����=%d, ��������=%d ===\n", n, capacity);

        // ��̬������Ʒ����
        Item *items = (Item *)malloc(n * sizeof(Item));
        if (!items) {
            printf("�ڴ����ʧ�ܣ�\n");
            return 1;
        }

        // ���ɲ�������
        generateTestData(items, n);

        // ִ��������
        printf("--- 1. ������ ---\n");
        bruteForce(items, n, capacity);

        // ִ�л��ݷ�
        printf("--- 2. ���ݷ� ---\n");
        backtracking(items, n, capacity);

        // ִ�д���֦�Ļ��ݷ�
        printf("--- 3. ���ݷ�������֦�� ---\n");
        pruning(items, n, capacity);

        free(items);
        printf("------------------------\n\n");
    }

    return 0;
}
