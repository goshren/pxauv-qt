#include "Tool.h"

Tool::Tool()
{

}

//去除字符串开头的空格
QString Tool::trimLeftSpace(const QString &str)
{
    int start = 0;
    while (start < str.size() && str[start].isSpace()) {
        ++start;
    }
    return str.mid(start);
}

//将无符号32位数据转换成浮点型数据并且返回
float Tool::parseIEEE754( uint32_t data)
{
    float result = 0.0;

    uint8_t part1 = data >> 31;						//符号部分
    int16_t part2 = ((data >> 23) & 0xFF) - 127;	//指数部分  0-255间的数据
    uint32_t part4 = (data & 0x7FFFFF) | 0x800000;	//尾数部分 - 组合 1.尾数部分

    for (int i = 0; i < 24; i++)
    {
        int p1 = (part4 >> (23 - i)) & 0x1;
        if (p1 == 0)									//因数为0，乘积为0，不参与本次计算
            continue;

        int power = part2 - i;						//计算幂指数

        double p2 = 0.0;
        if (power >= 0 && power < 25)					//查正值表
            p2 = PowerPositiveValue[power];
        else if (power <= 0 && power > -26)			//查负值表
            p2 = PowerNegativeValue[-power];
        else										//表外数据，计算,不影响结果的，可以直接取零
            p2 = 0;//pow(2,part2 - i);

        result += p1 * p2;
    }
    return part1 == 0 ? result : -result;
}
