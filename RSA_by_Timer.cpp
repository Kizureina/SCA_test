#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include <cstdio>
#include "main.hpp"

#define MAXSIZE 256

using namespace std;

// 全局变量
struct MODPOW
{
    uint32_t a;
    uint32_t b;
    uint32_t m;
} test_data[MAXSIZE];

uint64_t time_results[MAXSIZE];

// RDTSC计时函数
#ifdef _WIN32
//  Windows

#include <intrin.h>
uint64_t rdtsc() {
    return __rdtsc();
}

#else
//  Linux/GCC

uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

#endif

// 模幂运算
uint64_t mod_pow_RL(uint64_t a, uint64_t b, uint64_t m) {
    // a为底数，b为指数，m为模数
    uint64_t res = 1;
    while (b > 0) {
        if (b % 2 == 1) {
            res = (res * a) % m;
        }
        a = (a * a) % m;
        b /= 2;
    }
    return res;
}


// 计时分析
void time_test_RL(int n) {
    FILE* fp = fopen("RL_data.txt", "w");
    if (fp == NULL) {
        cout << "Error opening file.\n";
        return;
    }

    srand(time(NULL)); // Initialize random number generator

    for (int i = 0; i < n; i++) {
        uint64_t a = (uint64_t)rand() % ((uint64_t)1 << 32);
        uint64_t b = (uint64_t)rand() % ((uint64_t)1 << 32);
        uint64_t m = (uint64_t)rand() % ((uint64_t)1 << 32) + 1;

        // 使用RDTSC计时
        uint64_t start_time = rdtsc();
        mod_pow_RL(a, b, m);
        uint64_t end_time = rdtsc();

        uint64_t execution_time = (end_time - start_time);
        fprintf(fp, "%llu\n", execution_time);
        cout << execution_time << endl;
    }

    fclose(fp);
}


uint64_t mod_pow_LR(uint64_t a, uint64_t b, uint64_t m) {
    uint64_t res = 1;
    while (b > 0) {
        if (b & 1) {
            res = (res * a) % m;
        }
        a = (a * a) % m;
        b >>= 1;
    }
    return res;
}


void time_test_LR(int n) {
    FILE* fp = fopen("LR_data.txt", "w");
    if (fp == NULL) {
        cout << "Error opening file.\n";
        return;
    }

    srand(time(NULL)); // Initialize random number generator

    for (int i = 0; i < n; i++) {
        uint64_t a = (uint64_t)rand() % ((uint64_t)1 << 32);
        uint64_t b = (uint64_t)rand() % ((uint64_t)1 << 32);
        uint64_t m = (uint64_t)rand() % ((uint64_t)1 << 32) + 1;

        // 使用RDTSC计时
        uint64_t start_time = rdtsc();
        mod_pow_LR(a, b, m);
        uint64_t end_time = rdtsc();

        uint64_t execution_time = (end_time - start_time);
        fprintf(fp, "%llu\n", execution_time);
        cout << execution_time << endl;

    }

    fclose(fp);
}


// 生成n个用于测试的底数，指数和模数
void generate_test_base(int n)
{
    FILE* fp = fopen("test.txt", "w");
    if (fp == NULL) {
        cout << "Error opening file.\n";
        return;
    }

    // 设置随机数种子
    srand(time(NULL)); // 使用当前时间作为种子

    // 标记出现过的底数数组
    uint64_t bases[256] = { 0 };

    for (size_t i = 0; i < n; i++)
    {
        uint64_t base = (uint64_t)rand() % ((uint64_t)1 << 32);
        
        // 判断是否重复，重复则重新生成
        size_t j = 0;
        while (bases[j] != 0)
        {
            if (bases[j] == base)
            {
                base = (uint64_t)rand() % ((uint64_t)1 << 32);
                // 再次检查新生成的是否重复
                j = 0;
            }
            else 
            {
                j++;
            }
        }

        bases[i] = base;
        uint64_t exponent = (uint64_t)rand() % ((uint64_t)1 << 32);
        uint64_t modulus = (uint64_t)rand() % ((uint64_t)1 << 32) + 1;

        test_data[i].a = base;
        test_data[i].b = exponent;
        test_data[i].m = modulus;


        // 将测试用例记录在文件中
        fprintf(fp, "%llu %llu %llu\n", base, exponent, modulus);
    }

    fclose(fp);
}

// 打印
void print_test_data(int n)
{
    int length = n;
    for (size_t i = 0; i < length; i++)
    {
        cout << test_data[i].a << " " << test_data[i].b << " " << test_data[i].m << endl;
    }
}


// 计算消耗时间的平均值
void results_analysis()
{
    size_t j = 0;
    uint64_t sum = 0;
    while (time_results[j])
    {
        sum += time_results[j];
        j++;
    }

    uint64_t time_aver = sum / j;

    cout << "Time:" << time_aver << " j:" << j << endl;
}


void read_data_test(int n)
{
    uint64_t a, b, m;
    FILE* fp = fopen("RL_test_result.txt", "w");
    if (fp == NULL) {
        cout << "Error opening file.\n";
        return;
    }

    for (int i = 0; i < n; i++) {
        a = test_data[i].a;
        b = test_data[i].b;
        m = test_data[i].m;

        // 使用RDTSC计时
        uint64_t start_time = rdtsc();
        mod_pow_RL(a, b, m);
        uint64_t end_time = rdtsc();

        uint64_t execution_time = (end_time - start_time);
        fprintf(fp, "%llu\n", execution_time);

        // 存入结果数组
        time_results[i] = execution_time;
        cout << execution_time << " ";
    }

    cout << endl;
    fclose(fp); // 关闭文件
}


// 攻击每个位
void attack_per_byte()
{
    int a = (int) test_data[0].a;
    int b = (int) test_data[0].b;
    int m = (int) test_data[0].m;
    cout << "测试数据：底数为" << a << " 幂数为" << b << " 模数为" << m << endl;

    // 使用RDTSC计时
    uint64_t start_time = rdtsc();
    mod_pow_LR(a, b, m);
    uint64_t end_time = rdtsc();

    uint64_t execution_time = (end_time - start_time);
    cout << "实际运行时间：" << execution_time << endl;

    int length = 64;
    double correct_rate = 0;
    for (size_t i = 0; i < length; i++)
    {
        int exponent_bit = get_bit(b, i);

        // 计算当第i位被赋值为0时
        clear_bit(&b, i);

        // 运行时间与实际时间的方差
        uint64_t data[100] = { 0 };
        for (size_t j = 0; j < 100; j++)
        {
            start_time = rdtsc();
            mod_pow_LR(a, b, m);
            end_time = rdtsc();

            data[j] = end_time - start_time;
        }

        double bit_value_0 = calculate_variance(execution_time, data, 100);
        cout << "当测试第" << i << "个bit为 0 时方差为：" << bit_value_0 << endl;
        
        // 计算当第i位被赋值为0时
        set_bit(&b, i);

        // 运行时间与实际时间的方差
        for (size_t j = 0; j < 100; j++)
        {
            start_time = rdtsc();
            mod_pow_LR(a, b, m);
            end_time = rdtsc();

            data[j] = end_time - start_time;
        }

        double bit_value_1 = calculate_variance(execution_time, data, 100);
        cout << "当测试第" << i << "个bit为 1 时方差为：" << bit_value_1 << endl;

        int check;
        if (bit_value_0 < bit_value_1)
        {
            check = 0;
            cout << "推断第" << i << "个bit为 0 ";
        }
        else
        {
            check = 1;
            cout << "推断第" << i << "个bit为 1 ";
        }

        cout << "====> 实际上第" << i << "个bit为 " << exponent_bit << endl;
        if (check == exponent_bit)
        {
            correct_rate++;
        }
    }

    cout << "正确率为：" << (correct_rate / 64) * 100 << "%" << endl;
}



int main()
{
    // n不能超过MAXSIZE
    int n = 1;

    //time_test_LR(30);

    generate_test_base(n);
    attack_per_byte();

    //cout << "测试:" << mod_pow_RL(2, 4, 3) << endl;
    return 0;
}

