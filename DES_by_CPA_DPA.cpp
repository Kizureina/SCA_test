#include <iostream>
#include <math.h>
#include <iomanip>
#include <bitset>
// 正确密钥为43，即101011
#define KEY 43
using namespace std;

// 计算汉明重量
int HWFun(int num);

// S盒输出
int Sboxout(int num, int Nkey, int i);

// 计算相关系数
double Corr(int n, int Pstd[], int Ptest[]);

// 计算平均值
double meanNum(int n, int num[]);
void Dpa();

// S盒1
int S_Box1[4][16] = 
{
    {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
    {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
    {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
    {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
};


// 随机生成的明文，因为做了分组，所以均为6bit
int mingwen[20] = {
    41,35,62,4,33,
    44,22,46,18,16,
    9,49,49,59,41,
    43,51,38,27,60 
};


int HWw[64][20];
int Ptest[64][20];
int Pstd[20];
int HWstd[20];

//DPA
double DpaP[64][2];

//记录上述差值
double Deta[64];
int DpaMax;

int Sout[64][20];
double Rl[64];

//相关系数最大位置
int Max;


void getPerBit(int num, int bits[])
{
    // 使用掩码操作提取每一位
    for (int i = 5; i >= 0; i--) {
        bits[i] = (num >> i) & 1; // 右移 i 位，然后与 1 进行按位与操作
    }
}


int HWFun(int num) {
    int count = 0;

    if (num == 0)
        return 0;

    while (num > 0)
    {
        // n &= (n – 1)能清除最右边的1
        // 因为从二进制的角度讲，n相当于在n - 1的最低位加上1
        // 比如7（0111）= 6（0110）+ 1（0001），所以7 & 6 = （0111）&（0110）= 6（0110）
        // 清除了7的二进制表示中最右边的1（也就是最低位的1）。
        num &= (num - 1);
        count += 1;
    }

    return count;
}


double meanNum(int n, int num[]) {
    double total = 0.0;

    for (int i = 0; i < n; i++)
    {
        total = total + num[i];
    }

    return total / n;
}


double Corr(int n, int Pstd[], int Ptest[]) {
    // 返回两个向量数组的相关系数值
    double fenzi = 0.0;

    // 求两个数组的均值
    double PstdMean = meanNum(n, Pstd);
    double PtestdMean = meanNum(n, Ptest);

    double fenmu1 = 0.0;
    double fenmu2 = 0.0;
    double corr = 0.0;

    for (int i = 0; i < n; i++) {
        fenzi += (Pstd[i] - PstdMean) * (Ptest[i] - PtestdMean);
        fenmu2 += (Ptest[i] - PtestdMean) * (Ptest[i] - PtestdMean);
        fenmu1 += (Pstd[i] - PstdMean) * (Pstd[i] - PstdMean);
    }

    corr = fenzi / (sqrt(fenmu1) * sqrt(fenmu2));

    return corr;
}


int Sboxout(int num, int Nkey, int i)
{
    // S盒输入的num为6bit，对其做位操作
    int bits[6];
    getPerBit(num, bits);

    int HW = 0;
    int P = 0;
    
    /*
        // 取出最高位和最低位相加得纵坐标（C语言中0b开头标识二进制数）

        int h = num & (0b00100000);
        // 这里右移四位而不是五位是正确的，因为拼凑结果有权值
        h = h >> 4;

        int l = num & (0b00000001);

        // 取出num中间四位相加为横坐标
        int m1 = num & (0b00000010);
        m1 = m1 >> 1;

        int m2 = num & (0b00000100);
        m2 = m2 >> 1;

        int m3 = num & (0b00001000);
        m3 = m3 >> 1;

        int m4 = num & (0b00010000);
        m4 = m4 >> 1;

        int y = h + l;
        int x = m1 + m2 + m3 + m4;
    */

     // 拼凑出横纵坐标，做S盒查表替换
    int y = bits[0] + bits[5] * 2;
    int x = bits[1] * 1 + bits[2] * 2 + bits[3] * 4 + bits[4] * 8;

    // S盒输出
    int Snum = S_Box1[y][x];
    Sout[Nkey][i] = Snum;

    // 计算S盒输出密文的汉明重量
    HWw[Nkey][i] = HWFun(Snum);

    // 明文转为二进制
    std::bitset<sizeof(int) * 8> binaryNumber(mingwen[i]);
    std::string binaryString = binaryNumber.to_string().substr(sizeof(int) * 8 - 6);

    // 模拟测量到的功耗
    Ptest[Nkey][i] = HWw[Nkey][i] * 5;
    P = HWw[Nkey][i] * 5;

    cout << i << "\t" << mingwen[i] << "\t" << binaryString << "\t\t" << Snum << "\t" << HWw[Nkey][i] << "\t" << P << endl;

    // 当测试到正确密钥时，将功耗和汉明重量向量赋值给正确的数组
    if (Nkey == KEY) {
        Pstd[i] = P;
        HWstd[i] = HWw[Nkey][i];
    }

    return Snum;
}


void Dpa() {
    //以2为界
    int Larr[20];
    int Harr[20];
    int Lc = 0;
    int Hc = 0;

    for (int i = 0; i < 20; i++) {
        if (HWstd[i] < 2) {
            Larr[Lc] = i;
            Lc++;
        }
        else {
            Harr[Hc] = i;
            Hc++;
        }
    }

    for (int i = 0; i < 64; i++) {
        int Htemp = 0;
        int Ltemp = 0;

        //HW小于2,对应累加
        for (int j = 0; j < Lc; j++) {

            Ltemp += Ptest[i][Larr[j]];
        }
        //HW大于2，对应累加
        for (int j = 0; j < Hc; j++) {

            Htemp += Ptest[i][Harr[j]];
        }


        DpaP[i][0] = Ltemp * 1.0 / Lc;
        DpaP[i][1] = Htemp * 1.0 / Hc;
        Deta[i] = abs(DpaP[i][0] - DpaP[i][1]);


    }

    cout << "\n=========================================DPA攻击=========================================\n差分值：Deta0-63:\n";
    for (int i = 0; i < 64; i++) {
        cout << setw(10) << Deta[i] << " ";
        if ((i + 1) % 6 == 0) {
            cout << "\n";
        }
    }

    double temp = 0.0;
    for (int i = 0; i < 64; i++) {
        if (Deta[i] > temp) {
            DpaMax = i;
            temp = Deta[i];
        }
    }

    cout << "\n最大差分值：Deta" << DpaMax << " = " << Deta[DpaMax];

    cout << "\n正确密钥：";
    int bit[6];
    getPerBit(Max, bit);

    // 打印出正确的密钥
    for (int j = 5; j >= 0; j--) {
        cout << bit[j];
    }


}



int main()
{
    // 正确密钥为43,0b标志二进制数据
    int Key = 0;
    cout << "*********************************测量阶段*********************************\n";
    cout << "序号\t明文\t二进制明文\tSout\tHW\tP\n";

    // 猜测密钥，6bit为0 ~ 63
    for (int Nkey = 0; Nkey < 64; Nkey++) {
        cout << "密钥为" << Nkey << "时测试数据：\n";
        for (int i = 0; i < 20; i++) {
            // 明文与密钥异或得到S盒输入
            Sboxout(mingwen[i] ^ Key, Nkey, i);
        }
        Key++;
    }

    /*
     cout << "HW:key0-19\n";
    for (int i = 0; i < 20; i++) {

        for (int j = 0; j < 20; j++) {
            cout << HWw[i][j] << "   ";
        }
        cout << "\n";
    }
    cout << "HW:key20-39\n";
    for (int i = 0; i < 20; i++) {
        for (int j = 20; j < 40; j++) {
            cout << HWw[j][i] << "   ";
        }
        cout << "\n";
    }
    cout << "HW:key40-63\n";
    for (int i = 0; i < 20; i++) {
        for (int j = 40; j < 60; j++) {
            cout << HWw[j][i] << "   ";
        }
        cout << "\n";
    }
    cout << "HW:key60-63\n";
    for (int i = 0; i < 20; i++) {
        for (int j = 60; j < 64; j++) {
            cout << HWw[j][i] << "   ";
        }
        cout << "\n";
    }
    */
    cout << "*********************************攻击阶段*********************************\n";
    cout << "=========================================CPA攻击=========================================\n";
    cout << "测试密钥的相关系数：0-63：\n";

    double temp = 0.0;
    for (int i = 0; i < 64; i++) {
        // 计算测试数组汉明重量的相关系数
        Rl[i] = abs(Corr(10, Pstd, HWw[i]));
        
        // 找出最大的值
        if (Rl[i] > temp) {
            Max = i;
            temp = Rl[i];
        }
        
        cout << setw(10) << Rl[i] << "  ";
        
        if ((i + 1) % 6 == 0)
            cout << "\n";
    }
    
    cout << "\n最大相关：R" << Max << " = " << Rl[Max];
    cout << "\n正确密钥：";
    
    int bit[6];
    getPerBit(Max, bit);

    // 打印出正确的密钥
    for (int j = 5; j >= 0; j--) {
        cout << bit[j];
    }

    Dpa();

    return 0;
}

