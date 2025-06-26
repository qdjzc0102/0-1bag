#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// 定义最大物品数量和容量（根据需求调整）
#define MAX_ITEMS 320000
#define MAX_CAPACITY 1000000

typedef struct {
    int weight;
    float value;
    float density;  // 价值密度
    int index;      // 原始索引
} Item;

// 比较函数：按价值密度降序排序
int compare(const void *a, const void *b) {
    Item *itemA = (Item *)a;
    Item *itemB = (Item *)b;
    return (itemB->density - itemA->density) > 0 ? 1 : -1;
}

// 1. 动态规划法（一维数组+分块处理）
void dynamicProgrammingOptimized(Item *items, int n, int capacity) {
    clock_t start = clock();

    // 动态分配一维数组
    float *dp = (float *)malloc((capacity + 1) * sizeof(float));
    if (!dp) {
        printf("内存分配失败！\n");
        return;
    }
    memset(dp, 0, (capacity + 1) * sizeof(float));

    // 分块处理，每1000个物品输出一次进度
    printf("动态规划计算中...\n");
    for (int i = 0; i < n; i++) {
        if (i % 1000 == 0)
            printf("处理物品 %d/%d (%.2f%%)\r", i, n, 100.0 * i / n);

        int w = items[i].weight;
        float v = items[i].value;

        // 逆序更新DP表
        for (int j = capacity; j >= w; j--) {
            if (dp[j - w] + v > dp[j]) {
                dp[j] = dp[j - w] + v;
            }
        }
    }

    // 处理完成后显示100%进度
    printf("处理物品 %d/%d (100.00%%)\n", n, n);

    printf("动态规划结果：\n");
    printf("总价值：%.2f\n", dp[capacity]);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("执行时间：%I64d ms\n\n", executionTime);

    free(dp);
}

// 2. 贪心算法（适用于大规模数据）
void greedyAlgorithm(Item *items, int n, int capacity) {
    clock_t start = clock();

    // 复制并计算价值密度
    Item *sortedItems = (Item *)malloc(n * sizeof(Item));
    if (!sortedItems) {
        printf("内存分配失败！\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        sortedItems[i] = items[i];
        sortedItems[i].density = items[i].value / items[i].weight;
    }

    // 按价值密度排序
    qsort(sortedItems, n, sizeof(Item), compare);

    float totalValue = 0;
    int totalWeight = 0;
    int selectedCount = 0;

    printf("贪心算法计算中...\n");
    for (int i = 0; i < n; i++) {
        if (totalWeight + sortedItems[i].weight <= capacity) {
            totalWeight += sortedItems[i].weight;
            totalValue += sortedItems[i].value;
            selectedCount++;

            // 每处理10000个物品输出一次进度
            if (selectedCount % 10000 == 0)
                printf("已选择物品：%d/%d (%.2f%%)\r", selectedCount, n, 100.0 * selectedCount / n);
        }
    }
    printf("\n");

    printf("贪心算法结果：\n");
    printf("总价值：%.2f\n", totalValue);
    printf("选中物品数量：%d\n", selectedCount);
    printf("背包利用率：%.2f%%\n", 100.0 * totalWeight / capacity);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("执行时间：%I64d ms\n\n", executionTime);

    free(sortedItems);
}

int main() {
    // 测试数据规模
    int itemCounts[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 20000, 40000, 80000, 160000, 320000};
    int capacities[] = {10000, 100000, 1000000};
    int numItemCounts = sizeof(itemCounts) / sizeof(itemCounts[0]);

    // 用户选择背包容量
    int capacityIdx;
    printf("请选择背包容量（输入0-2）：\n");
    for (int i = 0; i < 3; i++)
        printf("%d: %d\n", i, capacities[i]);
    scanf("%d", &capacityIdx);

    int capacity = capacities[capacityIdx];
    printf("===== 测试背包容量=%d =====\n\n", capacity);

    // 动态分配物品数组（按最大规模分配）
    int maxItems = itemCounts[numItemCounts - 1];
    Item *items = (Item *)malloc(maxItems * sizeof(Item));
    if (!items) {
        printf("内存分配失败！\n");
        return 1;
    }

    Item *itemsCopy = (Item *)malloc(maxItems * sizeof(Item));
    if (!itemsCopy) {
        printf("内存分配失败！\n");
        free(items);
        return 1;
    }

    // 生成随机数据（只生成一次，后续复用）
    printf("生成随机数据...\n");
    srand(time(NULL));
    for (int i = 0; i < maxItems; i++) {
        // 重量范围：1-100
        items[i].weight = rand() % 100 + 1;
        // 价值范围：100.00-999.99
        items[i].value = (float)((rand()%900 + 100) * 100 + rand()%100) / 100;
        items[i].index = i;
    }

    // 对每个物品数量进行测试
    for (int i = 0; i < numItemCounts; i++) {
        int n = itemCounts[i];
        printf("\n===== 测试规模：物品数量=%d, 背包容量=%d =====\n\n", n, capacity);

        printf("=== 1. 动态规划法 ===\n");
        dynamicProgrammingOptimized(items, n, capacity);

        printf("=== 2. 贪心算法 ===\n");
        memcpy(itemsCopy, items, n * sizeof(Item));
        greedyAlgorithm(itemsCopy, n, capacity);
    }

    free(items);
    free(itemsCopy);

    return 0;
}
