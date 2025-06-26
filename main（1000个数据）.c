#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// ���������Ʒ�������������������
#define MAX_ITEMS 320000

typedef struct {
    int weight;       // ��Ʒ����������������ת��Ϊ����λС���ĸ�������
    float value;      // ��Ʒ��ֵ
    int index;        // ��Ʒ���
} Item;

// 1. ������Ʒ���ݲ�д��Excel���ݵ�CSV�ļ�
void generateExcelData(int n) {
    clock_t start = clock();
    FILE *file = fopen("items_data.csv", "w");
    if (!file) {
        printf("�޷�����CSV�ļ���\n");
        return;
    }

    // д��CSV��ͷ
    fprintf(file, "��Ʒ���,��Ʒ����,��Ʒ��ֵ\n");

    // ��̬������Ʒ����
    Item *items = (Item *)malloc(n * sizeof(Item));
    if (!items) {
        printf("�ڴ����ʧ�ܣ�\n");
        fclose(file);
        return;
    }

    // �����������
    printf("����%d����Ʒ����...\n", n);
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].index = i + 1;  // ��Ʒ��Ŵ�1��ʼ
        items[i].weight = rand() % 100 + 1;  // ������Χ��1-100��������
        items[i].value = (float)((rand()%900 + 100) * 100 + rand()%100) / 100;  // ��ֵ��Χ��100.00-999.99
    }

    // ������д��CSV�ļ�������ת��Ϊ����λС���ĸ�������
    printf("д��CSV�ļ�...\n");
    for (int i = 0; i < n; i++) {
        // ����������ת��Ϊ����λС���ĸ���������8 -> 8.00��
        float weightFloat = (float)items[i].weight;
        fprintf(file, "%d,%.2f,%.2f\n",
                items[i].index, weightFloat, items[i].value);
    }

    fclose(file);
    free(items);

    clock_t end = clock();
    long long executionTime = (long long)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("����������ɣ���д��items_data.csv\n");
    printf("ִ��ʱ�䣺%I64d ms\n\n", executionTime);
}

int main() {
    int n = 1000;  // ����1000����Ʒ
    printf("===== ����%d����Ʒ��Excel���� =====\n\n", n);

    // �����������ɺ���
    generateExcelData(n);

    printf("ʹ��˵����\n");
    printf("1. ��Excel���\n");
    printf("2. ѡ��\"�ļ�\"->\"��\"->�ҵ�items_data.csv�ļ�\n");
    printf("3. �ڵ�������ѡ��\"�ָ�����\"����ȷ�϶���(,)Ϊ�ָ���\n");
    printf("4. ��ɵ�������ݽ�������ʾ����Ʒ��š���Ʒ��������Ʒ��ֵ\n");

    return 0;
}
