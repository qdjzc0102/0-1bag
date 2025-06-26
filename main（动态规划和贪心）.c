#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// ���������Ʒ�������������������������
#define MAX_ITEMS 320000
#define MAX_CAPACITY 1000000

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

// 1. ��̬�滮����һά����+�ֿ鴦��
void dynamicProgrammingOptimized(Item *items, int n, int capacity) {
    clock_t start = clock();

    // ��̬����һά����
    float *dp = (float *)malloc((capacity + 1) * sizeof(float));
    if (!dp) {
        printf("�ڴ����ʧ�ܣ�\n");
        return;
    }
    memset(dp, 0, (capacity + 1) * sizeof(float));

    // �ֿ鴦��ÿ1000����Ʒ���һ�ν���
    printf("��̬�滮������...\n");
    for (int i = 0; i < n; i++) {
        if (i % 1000 == 0)
            printf("������Ʒ %d/%d (%.2f%%)\r", i, n, 100.0 * i / n);

        int w = items[i].weight;
        float v = items[i].value;

        // �������DP��
        for (int j = capacity; j >= w; j--) {
            if (dp[j - w] + v > dp[j]) {
                dp[j] = dp[j - w] + v;
            }
        }
    }

    // ������ɺ���ʾ100%����
    printf("������Ʒ %d/%d (100.00%%)\n", n, n);

    printf("��̬�滮�����\n");
    printf("�ܼ�ֵ��%.2f\n", dp[capacity]);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("ִ��ʱ�䣺%I64d ms\n\n", executionTime);

    free(dp);
}

// 2. ̰���㷨�������ڴ��ģ���ݣ�
void greedyAlgorithm(Item *items, int n, int capacity) {
    clock_t start = clock();

    // ���Ʋ������ֵ�ܶ�
    Item *sortedItems = (Item *)malloc(n * sizeof(Item));
    if (!sortedItems) {
        printf("�ڴ����ʧ�ܣ�\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        sortedItems[i] = items[i];
        sortedItems[i].density = items[i].value / items[i].weight;
    }

    // ����ֵ�ܶ�����
    qsort(sortedItems, n, sizeof(Item), compare);

    float totalValue = 0;
    int totalWeight = 0;
    int selectedCount = 0;

    printf("̰���㷨������...\n");
    for (int i = 0; i < n; i++) {
        if (totalWeight + sortedItems[i].weight <= capacity) {
            totalWeight += sortedItems[i].weight;
            totalValue += sortedItems[i].value;
            selectedCount++;

            // ÿ����10000����Ʒ���һ�ν���
            if (selectedCount % 10000 == 0)
                printf("��ѡ����Ʒ��%d/%d (%.2f%%)\r", selectedCount, n, 100.0 * selectedCount / n);
        }
    }
    printf("\n");

    printf("̰���㷨�����\n");
    printf("�ܼ�ֵ��%.2f\n", totalValue);
    printf("ѡ����Ʒ������%d\n", selectedCount);
    printf("���������ʣ�%.2f%%\n", 100.0 * totalWeight / capacity);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("ִ��ʱ�䣺%I64d ms\n\n", executionTime);

    free(sortedItems);
}

int main() {
    // �������ݹ�ģ
    int itemCounts[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 20000, 40000, 80000, 160000, 320000};
    int capacities[] = {10000, 100000, 1000000};
    int numItemCounts = sizeof(itemCounts) / sizeof(itemCounts[0]);

    // �û�ѡ�񱳰�����
    int capacityIdx;
    printf("��ѡ�񱳰�����������0-2����\n");
    for (int i = 0; i < 3; i++)
        printf("%d: %d\n", i, capacities[i]);
    scanf("%d", &capacityIdx);

    int capacity = capacities[capacityIdx];
    printf("===== ���Ա�������=%d =====\n\n", capacity);

    // ��̬������Ʒ���飨������ģ���䣩
    int maxItems = itemCounts[numItemCounts - 1];
    Item *items = (Item *)malloc(maxItems * sizeof(Item));
    if (!items) {
        printf("�ڴ����ʧ�ܣ�\n");
        return 1;
    }

    Item *itemsCopy = (Item *)malloc(maxItems * sizeof(Item));
    if (!itemsCopy) {
        printf("�ڴ����ʧ�ܣ�\n");
        free(items);
        return 1;
    }

    // ����������ݣ�ֻ����һ�Σ��������ã�
    printf("�����������...\n");
    srand(time(NULL));
    for (int i = 0; i < maxItems; i++) {
        // ������Χ��1-100
        items[i].weight = rand() % 100 + 1;
        // ��ֵ��Χ��100.00-999.99
        items[i].value = (float)((rand()%900 + 100) * 100 + rand()%100) / 100;
        items[i].index = i;
    }

    // ��ÿ����Ʒ�������в���
    for (int i = 0; i < numItemCounts; i++) {
        int n = itemCounts[i];
        printf("\n===== ���Թ�ģ����Ʒ����=%d, ��������=%d =====\n\n", n, capacity);

        printf("=== 1. ��̬�滮�� ===\n");
        dynamicProgrammingOptimized(items, n, capacity);

        printf("=== 2. ̰���㷨 ===\n");
        memcpy(itemsCopy, items, n * sizeof(Item));
        greedyAlgorithm(itemsCopy, n, capacity);
    }

    free(items);
    free(itemsCopy);

    return 0;
}
