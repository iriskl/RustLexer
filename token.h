#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// Rust单词类型枚举
enum TokenType {
    KEYWORD,            // 关键字
    IDENTIFIER,         // 标识符
    INTEGER_LITERAL,    // 整数字面量
    FLOAT_LITERAL,      // 浮点数字面量
    STRING_LITERAL,     // 字符串字面量
    CHAR_LITERAL,       // 字符字面量
    OPERATOR,           // 运算符
    DELIMITER,          // 分隔符
    COMMENT,            // 注释
    MACRO_CALL,         // 宏调用
    UNKNOWN             // 未知类型
};

// Token结构定义
struct Token {
    std::string lexeme;  // 词素本身
    TokenType type;      // 词素类型
    int line;            // 行号
    int column;          // 列号
    std::string subType; // 子类型信息，如具体是哪种运算符或分隔符

    // 构造函数
    Token(const std::string& lex = "", TokenType t = UNKNOWN, int l = 0, int c = 0, const std::string& sub = "")
        : lexeme(lex), type(t), line(l), column(c), subType(sub) {}
};

// 辅助函数：将TokenType转换为字符串
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
    case KEYWORD: return "关键字";
    case IDENTIFIER: return "标识符";
    case INTEGER_LITERAL: return "字面量（整数）";
    case FLOAT_LITERAL: return "字面量（浮点数）";
    case STRING_LITERAL: return "字符串字面量";
    case CHAR_LITERAL: return "字符字面量";
    case OPERATOR: return "操作符";
    case DELIMITER: return "分隔符";
    case COMMENT: return "注释";
    case MACRO_CALL: return "宏调用名";
    case UNKNOWN: return "未知类型";
    default: return "未定义类型";
    }
}

#endif // TOKEN_H