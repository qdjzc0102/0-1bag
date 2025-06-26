#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// 定义最大物品数量（根据需求调整）
#define MAX_ITEMS 320000

typedef struct {
    int weight;       // 物品重量（整数，后续转换为带两位小数的浮点数）
    float value;      // 物品价值
    int index;        // 物品编号
} Item;

// 1. 生成物品数据并写入Excel兼容的CSV文件
void generateExcelData(int n) {
    clock_t start = clock();
    FILE *file = fopen("items_data.csv", "w");
    if (!file) {
        printf("无法创建CSV文件！\n");
        return;
    }

    // 写入CSV表头
    fprintf(file, "物品编号,物品重量,物品价值\n");

    // 动态分配物品数组
    Item *items = (Item *)malloc(n * sizeof(Item));
    if (!items) {
        printf("内存分配失败！\n");
        fclose(file);
        return;
    }

    // 生成随机数据
    printf("生成%d个物品数据...\n", n);
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].index = i + 1;  // 物品编号从1开始
        items[i].weight = rand() % 100 + 1;  // 重量范围：1-100（整数）
        items[i].value = (float)((rand()%900 + 100) * 100 + rand()%100) / 100;  // 价值范围：100.00-999.99
    }

    // 将数据写入CSV文件（重量转换为带两位小数的浮点数）
    printf("写入CSV文件...\n");
    for (int i = 0; i < n; i++) {
        // 将整数重量转换为带两位小数的浮点数（如8 -> 8.00）
        float weightFloat = (float)items[i].weight;
        fprintf(file, "%d,%.2f,%.2f\n",
                items[i].index, weightFloat, items[i].value);
    }

    fclose(file);
    free(items);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("数据生成完成！已写入items_data.csv\n");
    printf("执行时间：%I64d ms\n\n", executionTime);
}

int main() {
    int n = 1000;  // 生成1000个物品
    printf("===== 生成%d个物品的Excel数据 =====\n\n", n);

    // 调用数据生成函数
    generateExcelData(n);

    printf("使用说明：\n");
    printf("1. 打开Excel软件\n");
    printf("2. 选择\"文件\"->\"打开\"->找到items_data.csv文件\n");
    printf("3. 在导入向导中选择\"分隔符号\"，并确认逗号(,)为分隔符\n");
    printf("4. 完成导入后，数据将按列显示：物品编号、物品重量、物品价值\n");

    return 0;
}
