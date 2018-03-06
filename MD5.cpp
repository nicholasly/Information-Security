#include <iostream>
#include <vector>
#include <string>
using namespace std;

// 分割之后得到的512位的分组M
vector<vector<unsigned int> > M;    

// MD缓冲区中4个32位寄存器(a0, b0, c0, d0)作为初始向量IV
unsigned int a0 = 0x67452301;
unsigned int b0 = 0xefcdab89;
unsigned int c0 = 0x98badcfe;
unsigned int d0 = 0x10325476;

// 循环左移s表
int s[64] = { 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22, 
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21};

// T表
int T[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

/** 
 *  将输入的字符串转化成unsigned char类型的vector
*/
vector<unsigned char> string_to_uchar(string s) {
    int padding_info_length, padding_zero_length, message_length;
    message_length = s.size();
    // 先填充原始消息数据
    vector<unsigned char> message;
    for (int i = 0; i < message_length; ++i) {
        message.push_back(s[i]);
    }
    padding_zero_length = message.size();
    // 在原始消息数据尾部填充标志1000...0
    message.push_back(static_cast<unsigned char>(0x80));
    ++padding_zero_length;
    for ( ; (padding_zero_length << 3) % 512 != 448; ++padding_zero_length) {
        message.push_back(static_cast<unsigned char>(0));
    }
    padding_info_length = padding_zero_length;
    // 考虑到一般数据长度不会超过int的表示范围，故用小端规则只填充数据长度的低32位，其余均填0
    for (int i = 0; i < 4; ++padding_info_length, ++i) {
        message.push_back(static_cast<unsigned char>((((message_length << 3) >> (i << 3)) & 0xff)));
    }
    for (int i = 0; i < 4; ++padding_info_length, ++i) {
        message.push_back(static_cast<unsigned char>(0));
    }
    return message;
}

/** 
 *  将unsigned char类型的vector转化成unsigned int类型的vector
*/
vector<unsigned int> uchar_to_uint(vector<unsigned char> message1) {
    vector<unsigned int> message2;
    unsigned int message1_length = message1.size();
    unsigned int temp;
    // 由于unsigned char占一个字节，unsigned int占四个字节，转成unsigned int需要根据一定的比重进行
    for (int i = 0; i < message1_length / 4; ++i) {
        temp = 0;
        for (int j = 0; j < 4; ++j) {
            temp += static_cast<unsigned int>(message1[4 * i + j] << (j << 3));
        }
        message2.push_back(temp);
    }
    return message2;
}

/** 
 *  将unsigned int类型的vector分割为512位的分组M
*/
void divide_message(vector<unsigned int> message) {
    vector<unsigned int> temp;
    // 分割后的unsigned int类型的vector存入相应的分组M之中，为了方便，M设为全局变量
    // M中的一个分组包含16个unsigned int类型的vector，因为unsigned int类型数组占32位，16 * 32 = 512 
    for (int i = 0; i < message.size(); ++i) {
        temp.push_back(message[i]);
        if (i % 16 == 15) {
            M.push_back(temp);
            temp.clear();
        }
    }
}

/** 
 *  哈希函数
*/
void hash(vector<unsigned int> M) {
    // 以512位分组M为单位，每一分组经过4个循环的压缩算法
    // CV0 = IV, 迭代在MD缓冲区进行
    // 哈希函数从CV输入128位，从消息分组输入512位，完成4轮循环后，输出128位，用于下一轮输入的CV值。
    unsigned int A = a0;
    unsigned int B = b0;
    unsigned int C = c0;
    unsigned int D = d0;
    // 每轮循环分别固定不同的生成函数F，结合指定的T表元素T[]和消息分组的不同部分M[]做16次运算
    for (int i = 0; i < 64; ++i) {
        unsigned int F, g;
        if (i >= 0 && i <= 15) {
            F = (B & C) | ((~B) & D);
            g = i;
        }
        else if (i >= 16 && i <= 31) {
            F = (D & B) | ((~D) & C);
            g = (5 * i + 1) % 16;
        }
        else if (i >= 32 && i <= 47) {
            F = B ^ C ^ D;
            g = (3 * i + 5) % 16;
        }
        else if (i >= 48 && i <= 63) {
            F = C ^ (B | (~D));
            g = (7 * i) % 16;
        }
        F = F + A + T[i] + M[g];
        A = D;
        D = C;
        C = B;
        B = B + (F << s[i] | (F >> (32 - s[i])));
    }
    a0 += A;
    b0 += B;
    c0 += C;
    d0 += D;
}

/** 
 *  MD5整体函数，返回一个装有加密字符串的unsigned char类型的vector
*/
vector<unsigned char> MD5(vector<unsigned int> padded_message) {
    // 分组
    divide_message(padded_message);
    // 对每一个分组M进行加密
    for (int i = 0; i < M.size(); ++i) {
        hash(M[i]);
    }
    // MD缓冲区中4个32位寄存器(a0, b0, c0, d0)中的加密结果存入unsigned char类型的vector
    vector<unsigned char> cipher(16, 0);
    for (int i = 0; i < 16; ++i) {
        if (i < 4) {
            cipher[i] = a0 >> 8 * (i % 4);
        } else if (i < 8) {
            cipher[i] = b0 >> 8 * (i % 4);
        } else if (i < 12) {
            cipher[i] = c0 >> 8 * (i % 4);
        } else if (i < 16) {
            cipher[i] = d0 >> 8 * (i % 4);
        }
    }
    return cipher;
}

int main() {
    // 输入要加密的字符串
    string s;
    cout << "Please enter the string you want to encrypt : " << endl;
    cin >> s;
    // 字符串存入unsigned char类型的vector
    vector<unsigned char> message1 = string_to_uchar(s);
    // unsigned char类型的vector转成unsigned int类型的vector
    vector<unsigned int> message2 = uchar_to_uint(message1);
    // MD5加密
    vector<unsigned char> cipher = MD5(message2);
    // 输出加密结果
    cout << "Here is the string encrypted by MD5 : " << endl;
    for (int i = 0; i < cipher.size(); ++i) {
        cout.fill('0');
        cout.width(2);
        cout << std::hex << (unsigned int)cipher[i];
    }
    cout << endl;
    return 0;
}