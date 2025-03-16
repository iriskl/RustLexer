#ifndef RUSTLEXER_H
#define RUSTLEXER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

// 单词类型枚举
enum class TokenType {
    KEYWORD,         // 关键字
    IDENTIFIER,      // 标识符
    INTEGER_LITERAL, // 整数字面量
    FLOAT_LITERAL,   // 浮点数字面量
    STRING_LITERAL,  // 字符串字面量
    CHAR_LITERAL,    // 字符字面量
    OPERATOR,        // 运算符
    DELIMITER,       // 分隔符
    COMMENT,         // 注释
    MACRO_CALL,      // 宏调用
    UNKNOWN          // 未知类型
};

// 单词结构
struct Token {
    std::string lexeme;  // 词素本身
    TokenType type;      // 词素类型
    int line;            // 行号
    int column;          // 列号
};

class RustLexer {
public:
    RustLexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t position;
    int line;
    int column;
    
    // Rust关键词集合
    static const std::unordered_set<std::string> KEYWORDS;
    
    // 运算符映射
    static const std::unordered_map<std::string, TokenType> OPERATORS;
    static const std::unordered_map<char, TokenType> DELIMITERS;
    
    // 辅助方法
    char peek(int offset = 0) const;
    char advance();
    bool isAtEnd() const;
    bool match(char expected);
    
    // 单词识别方法
    Token scanToken();
    Token identifier();
    Token number();
    Token string();
    Token character();
    Token comment();
    Token operatorOrDelimiter();
    
    // 判断字符类型
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isHexDigit(char c) const;
    bool isOctalDigit(char c) const;
    bool isBinaryDigit(char c) const;
};

#endif // RUSTLEXER_H