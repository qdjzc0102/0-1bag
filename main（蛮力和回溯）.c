#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// 定义最大物品数量和容量
#define MAX_ITEMS 20
#define MAX_CAPACITY 1000

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

// 1. 蛮力法求解背包问题
void bruteForce(Item *items, int n, int capacity) {
    clock_t start = clock();
    float maxValue = 0;
    int *bestSet = (int *)calloc(n, sizeof(int));

    printf("蛮力法计算中...\n");
    // 枚举所有2^n种可能的组合
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

    printf("蛮力法结果：\n");
    printf("总价值：%.2f\n", maxValue);
    printf("选中物品数量：");
    int count = 0;
    for (int i = 0; i < n; i++)
        if (bestSet[i]) count++;
    printf("%d\n", count);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("执行时间：%I64d ms\n\n", executionTime);

    free(bestSet);
}

// 2. 回溯法求解背包问题
void backtracking(Item *items, int n, int capacity) {
    clock_t start = clock();

    // 复制物品数组
    Item *sortedItems = (Item *)malloc(n * sizeof(Item));
    if (!sortedItems) {
        printf("内存分配失败！\n");
        return;
    }
    memcpy(sortedItems, items, n * sizeof(Item));

    // 按价值密度排序（用于优化搜索顺序）
    for (int i = 0; i < n; i++) {
        sortedItems[i].density = sortedItems[i].value / sortedItems[i].weight;
    }
    qsort(sortedItems, n, sizeof(Item), compare);

    float maxValue = 0;
    int *bestSet = (int *)calloc(n, sizeof(int));
    int *currentSet = (int *)calloc(n, sizeof(int));

    // 递归回溯函数
    void backtrack(int level, int weight, float value) {
        if (level == n) {
            if (weight <= capacity && value > maxValue) {
                maxValue = value;
                memcpy(bestSet, currentSet, n * sizeof(int));
            }
            return;
        }

        // 考虑选择当前物品
        currentSet[level] = 1;
        backtrack(level + 1, weight + sortedItems[level].weight, value + sortedItems[level].value);

        // 考虑不选择当前物品
        currentSet[level] = 0;
        backtrack(level + 1, weight, value);
    }

    printf("回溯法计算中...\n");
    backtrack(0, 0, 0);

    printf("回溯法结果：\n");
    printf("总价值：%.2f\n", maxValue);
    printf("选中物品数量：");
    int count = 0;
    for (int i = 0; i < n; i++)
        if (bestSet[i]) count++;
    printf("%d\n", count);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("执行时间：%I64d ms\n\n", executionTime);

    free(sortedItems);
    free(bestSet);
    free(currentSet);
}

// 3. 剪枝法求解背包问题（分支限界法思想）
void pruning(Item *items, int n, int capacity) {
    clock_t start = clock();

    // 复制物品数组并按价值密度排序
    Item *sortedItems = (Item *)malloc(n * sizeof(Item));
    if (!sortedItems) {
        printf("内存分配失败！\n");
        return;
    }
    memcpy(sortedItems, items, n * sizeof(Item));

    // 计算价值密度并排序
    for (int i = 0; i < n; i++) {
        sortedItems[i].density = (float)sortedItems[i].value / sortedItems[i].weight;
    }
    qsort(sortedItems, n, sizeof(Item), compare);

    float maxValue = 0;
    int *bestSet = (int *)calloc(n, sizeof(int));
    int *currentSet = (int *)calloc(n, sizeof(int));

    // 计算剩余物品的最大可能价值（上界）
    float calculateBound(int level, int weight, float value) {
        float bound = value;
        int remainingWeight = capacity - weight;

        for (int i = level; i < n && remainingWeight > 0; i++) {
            if (sortedItems[i].weight <= remainingWeight) {
                bound += sortedItems[i].value;
                remainingWeight -= sortedItems[i].weight;
            } else {
                // 部分装入（分数背包）
                bound += sortedItems[i].density * remainingWeight;
                remainingWeight = 0;
            }
        }
        return bound;
    }

    // 递归剪枝函数
    void prune(int level, int weight, float value) {
        if (level == n) {
            if (weight <= capacity && value > maxValue) {
                maxValue = value;
                memcpy(bestSet, currentSet, n * sizeof(int));
            }
            return;
        }

        // 选择当前物品（如果不超过容量）
        if (weight + sortedItems[level].weight <= capacity) {
            currentSet[level] = 1;
            prune(level + 1, weight + sortedItems[level].weight, value + sortedItems[level].value);
        }

        // 计算不选择当前物品的剩余价值上界
        float bound = calculateBound(level + 1, weight, value);
        if (bound > maxValue) {
            currentSet[level] = 0;
            prune(level + 1, weight, value);
        }
    }

    printf("剪枝法计算中...\n");
    prune(0, 0, 0);

    printf("剪枝法结果：\n");
    printf("总价值：%.2f\n", maxValue);
    printf("选中物品数量：");
    int count = 0;
    for (int i = 0; i < n; i++)
        if (bestSet[i]) count++;
    printf("%d\n", count);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("执行时间：%I64d ms\n\n", executionTime);

    free(sortedItems);
    free(bestSet);
    free(currentSet);
}

// 生成测试数据
void generateTestData(Item *items, int n) {
    printf("生成随机数据...\n");
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        // 重量范围：1-100
        items[i].weight = rand() % 100 + 1;
        // 价值范围：100.00-999.99
        items[i].value = (float)((rand()%900 + 100) * 100 + rand()%100) / 100;
        items[i].index = i;
    }
}

int main() {
    // 测试不同规模的数据
    int testSizes[] = {10, 15, 20, 25, 30, 35, 40};  // 小规模量
    int capacity = 500;  // 固定背包容量

    printf("===== 回溯法与蛮力法对比测试 =====\n\n");

    for (int i = 0; i < 7; i++) {
        int n = testSizes[i];
        printf("=== 测试规模：物品数量=%d, 背包容量=%d ===\n", n, capacity);

        // 动态分配物品数组
        Item *items = (Item *)malloc(n * sizeof(Item));
        if (!items) {
            printf("内存分配失败！\n");
            return 1;
        }

        // 生成测试数据
        generateTestData(items, n);

        // 执行蛮力法
        printf("--- 1. 蛮力法 ---\n");
        bruteForce(items, n, capacity);

        // 执行回溯法
        printf("--- 2. 回溯法 ---\n");
        backtracking(items, n, capacity);

        // 执行带剪枝的回溯法
        printf("--- 3. 回溯法（带剪枝） ---\n");
        pruning(items, n, capacity);

        free(items);
        printf("------------------------\n\n");
    }

    return 0;
}
