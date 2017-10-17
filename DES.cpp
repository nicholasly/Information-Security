#include <iostream>
#include <bitset>  
#include <string>  
using namespace std;

// 初始置换IP
int IP[] = {58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7};

// 结尾置换IP-1 
int IP_1[] = {40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41,  9, 49, 17, 57, 25}; 

// 用bitset存储密钥及子密钥
bitset<64> key;
bitset<48> subkey[16];

// PC-1置换表
int PC_1[] = {57, 49, 41, 33, 25, 17, 9,  
        1, 58, 50, 42, 34, 26, 18,  
        10,  2, 59, 51, 43, 35, 27,  
        19, 11,  3, 60, 52, 44, 36,  
        63, 55, 47, 39, 31, 23, 15,  
        7, 62, 54, 46, 38, 30, 22,  
        14,  6, 61, 53, 45, 37, 29,  
        21, 13,  5, 28, 20, 12, 4};
        

// PC-2置换表
int PC_2[] = {14, 17, 11, 24,  1,  5,  
        3, 28, 15,  6, 21, 10,  
        23, 19, 12,  4, 26,  8,  
        16,  7, 27, 20, 13,  2,  
        41, 52, 31, 37, 47, 55,  
        30, 40, 51, 45, 33, 48,  
        44, 49, 39, 56, 34, 53,  
        46, 42, 50, 36, 29, 32};  

// E扩展置换表
int E[] = {32,  1,  2,  3,  4,  5,  
        4,  5,  6,  7,  8,  9,  
        8,  9, 10, 11, 12, 13,  
        12, 13, 14, 15, 16, 17,  
        16, 17, 18, 19, 20, 21,  
        20, 21, 22, 23, 24, 25,  
        24, 25, 26, 27, 28, 29,  
        28, 29, 30, 31, 32, 1};

// 8个S盒 
int S_BOX[8][4][16] = {  
        {    
            {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},    
            {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},    
            {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},   
            {15, 12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}   
        },  
        {    
            {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},    
            {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},   
            {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},    
            {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}    
        },   
        {    
            {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},    
            {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},    
            {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},    
            {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}    
        },   
        {    
            {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},    
            {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},    
            {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},    
            {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}    
        },  
        {    
            {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},    
            {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},    
            {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},    
            {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}    
        },  
        {    
            {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},    
            {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},    
            {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},    
            {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}    
        },   
        {    
            {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},    
            {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},    
            {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},    
            {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}    
        },   
        {    
            {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},    
            {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},    
            {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},    
            {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}    
        }   
};  

// P置换表
int P[] = {16,  7, 20, 21,  
        29, 12, 28, 17,  
        1, 15, 23, 26,  
        5, 18, 31, 10,  
        2, 8, 24, 14,  
        32, 27, 3, 9,  
        19, 13, 30, 6,  
        22, 11, 4, 25}; 

/** 
 *  字符串转二进制数 
 */
bitset<64> string_to_bitset(string s) {
    bitset<64> binary_code;
    // 默认字符串长度为8，将字符串转换成64位二进制数
    for (unsigned i = 0; i < s.size(); ++i) {
        for (int j = 0; j < 8; ++j) {
            binary_code[i * 8 + j] = (s[i] >> j) & 1;
        }
    }
    return binary_code;
}


/** 
 *  输入明文转换 
 */
bitset<64> input_clear_text(string string_clear_text) {
    // 规定明文有8个字符
    // 将明文转换成64位二进制数
    return string_to_bitset(string_clear_text);
}

/** 
 *  输入密钥转换 
 */
bitset<64> input_key(string string_key) {
    // 规定密钥有8个字符
    // 将密钥转换成64位二进制数
    return string_to_bitset(string_key);
}

/** 
 *  IP置换
 */
bitset<64> IP_permutation(bitset<64> clear_text) {
    bitset<64> permutated_text;
    // 对二进制明文进行IP置换
    for (int i = 0; i < 64; ++i) {
        permutated_text[63 - i] = clear_text[64 - IP[i]];
    }
    return permutated_text;
}

/** 
 *  循环左移 
 */
bitset<28> LS(bitset<28> A, int i) {
    bitset<28> new_set;
    // 将二进制串A循环左移
    int shift_bits = 2;
    if (i == 1 || i == 2 || i == 9 || i == 16) {
        shift_bits = 1;
    } 
    for (int j = 27; j >= 0; --j) {
        if (j - shift_bits < 0) {
            new_set[j] = A[j - shift_bits + 28];
        }
        else {
            new_set[j] = A[j - shift_bits];
        }
    }
    return new_set;
}

/** 
 *  生成16个48位的子密钥 
 */
void subkey_generation(bitset<64> key) {
    bitset<56> reduced_key;
    bitset<28> C, D;
    bitset<48> compressed_key; 
    // 对密钥K的56个非校验位实行置换PC_1
    for (int i = 0; i < 56; ++i) {
        reduced_key[55 - i] = key[64 - PC_1[i]];
    }
    // 进行16轮迭代
    for (int i = 0; i < 16; ++i) {
        // 得到Ci和Di
        for (int j = 0; j < 28; ++j) {
            C[j] = reduced_key[j];
        }
        for (int j = 28; j < 56; ++j) {
            D[j - 28] = reduced_key[j];
        }
        // 对Ci和Di进行循环左移
        C = LS(C, i);
        D = LS(D, i);
        for (int j = 28; j < 56; ++j) {
            reduced_key[j] = C[j - 28];
        }
        for (int j = 0; j < 28; ++j) {
            reduced_key[j] = D[j];
        }
        // 对CiDi实行PC-2压缩置换
        for (int j = 0; j < 48; ++j) {
            compressed_key[47 - j] = reduced_key[56 - PC_2[j]];
        }
        subkey[i] = compressed_key;
    }
}

/** 
 *  Feistel轮函数 
 */
bitset<32> f(bitset<32> R, bitset<48> K) {
    // 将32位的串R进行E扩展成为48位的串E(R)
    bitset<48> E_expand;
    for (int i = 0; i < 48; ++i) {
        E_expand[47 - i] = R[32 - E[i]];
    }
    // 将E(R)和长度为48位的子密钥K进行48位二进制异或运算
    E_expand ^= K;
    // 将结果分成8个分组
    bitset<6> group[8];
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 6; ++j) {
            group[7 - i][5 - j] = E_expand[47 - 6 * i - j];
        }
    }
    // 经过S盒作6-4转换且合并结果
    bitset<32> output;
    for (int i = 7; i >= 0; --i) {
        int n = group[i][5] * 2 + group[i][0];
        int m = group[i][4] * 8 + group[i][3] * 4 + group[i][2] * 2 + group[i][1];
        int num = S_BOX[7 - i][n][m];
        bitset<4> binary(num);
        for (int j = 0; j < 4; ++j) {
            output[31 - i * 4 - j] = binary[j];
        }
    }
    // 对输出结果进行P置换
    bitset<32> temp = output;
    for (int i = 0; i < 32; ++i) {
        output[31 - i] = temp[32 - P[i]];
    }  
    return output;
}

/** 
 *  16轮迭代T(加密时用) 
*/
bitset<64> T(bitset<64> permutated_text) {
    bitset<32> L, R;
    bitset<64> iterated_text;
    // 获取Li和Ri  
    for (int i = 32; i < 64; ++i) {
        L[i - 32] = permutated_text[i];
    }
    for (int i = 0; i < 32; ++i) {
        R[i] = permutated_text[i];
    }
    // 进行16次T迭代
    for (int i = 0; i < 16; ++i) {  
        bitset<32> temp = R;  
        R = L ^ f(R, subkey[i]);  
        L = temp;  
    }  
    //合并L和R为RL  
    for (int i = 0; i < 32; ++i) {
        iterated_text[i] = L[i];
    }
    for (int i = 32; i < 64; ++i) {
        iterated_text[i] = R[i - 32];
    }
    return iterated_text;
}

/** 
 *  16轮迭代T(解密时用) 
*/
bitset<64> inverse_T(bitset<64> permutated_text) {
    bitset<32> L, R;
    bitset<64> iterated_text;
    // 获取Li和Ri  
    for (int i = 32; i < 64; ++i) {
        L[i - 32] = permutated_text[i];
    }
    for (int i = 0; i < 32; ++i) {
        R[i] = permutated_text[i];
    }
    // 进行16次T迭代
    for (int i = 0; i < 16; ++i) {  
        bitset<32> temp = R;  
        R = L ^ f(R, subkey[15 - i]);  
        L = temp;  
    }  
    //合并L和R为RL  
    for (int i = 0; i < 32; ++i) {
        iterated_text[i] = L[i];
    }
    for (int i = 32; i < 64; ++i) {
        iterated_text[i] = R[i - 32];
    }
    return iterated_text;
}

/** 
 *  逆置换IP-1 
*/
bitset<64> IP_1_permutation(bitset<64> iterated_text) {
    bitset<64> cipher_text;
    // 进行逆IP置换
    for (int i = 0; i < 64; ++i) {
        cipher_text[63 - i] = iterated_text[64 - IP_1[i]];
    }
    return cipher_text;
}

/** 
 *  加密函数 
*/
bitset<64> encrypt(bitset<64>& clear_text) {
    // 初始置换IP
    bitset<64> permutated_text = IP_permutation(clear_text);
    // 16轮迭代T
    bitset<64> iterated_text = T(permutated_text);
    // 结尾置换IP-1
    bitset<64> cipher_text = IP_1_permutation(iterated_text);
    return cipher_text;
}

/** 
 *  解密函数 
*/
bitset<64> decrypt(bitset<64>& cipher_text) {  
    // 初始置换IP
    bitset<64> permutated_text = IP_permutation(cipher_text);
    // 16轮迭代T
    bitset<64> iterated_text = inverse_T(permutated_text);
    // 结尾置换IP-1
    bitset<64> clear_text = IP_1_permutation(iterated_text);
    return clear_text;  
}

/** 
 *  二进制数转字符串 
*/
string binary_to_string(bitset<64> binary) {
    string text;
    for (int i = 0; i < 64; i += 8) {
        bitset<8> temp;
        for (int j = 0; j < 8; ++j) {
            temp[j] = binary[i + j];
        }
        text += char(temp.to_ulong());
    }
    return text;
}

/** 
 *  测试程序 
*/
int main() {
    // 输入明文和密钥
    string string_clear_text;
    string string_key;
    cout << "Please input the text you want to encrypt :" << endl;
    cin >> string_clear_text;
    cout << "Please input the key you want to use :" << endl;
    cin >> string_key;
    // 生成子密钥
    key = input_key(string_key);
    subkey_generation(key);
    // 明文转换成二进制数形式
    bitset<64> binary_clear_text = input_clear_text(string_clear_text);
    cout << "The binary form of the encrypted text is :" << endl;
    cout << binary_clear_text << endl;
    // 对明文进行加密
    bitset<64> binary_cipher_text = encrypt(binary_clear_text);
    cout << "The binary form of the cipher text is :" << endl;
    cout << binary_cipher_text << endl;
    // 密文转换成字符串
    cout << "The string form of the cipher text is :" << endl;
    cout <<  binary_to_string(binary_cipher_text) << endl;
    // 对密文进行解密
    bitset<64> decrypted_cipher_text = decrypt(binary_cipher_text);
    cout << "The binary form of the decrypted text is :" << endl;
    cout << decrypted_cipher_text << endl;
    // 解密后的明文转换成字符串
    cout << "The string form of the decrypted text is :" << endl;
    cout <<  binary_to_string(decrypted_cipher_text) << endl;
    return 0;
}
