#include "rustlexer.h"
#include <cctype>
#include <algorithm>
#include <cstdlib>

// 初始化Rust关键词集合
const std::unordered_set<std::string> RustLexer::KEYWORDS = {
    "as", "break", "const", "continue", "crate", "else", "enum", "extern",
    "false", "fn", "for", "if", "impl", "in", "let", "loop", "match", "mod",
    "move", "mut", "pub", "ref", "return", "self", "Self", "static", "struct",
    "super", "trait", "true", "type", "unsafe", "use", "where", "while", "async",
    "await", "dyn", "abstract", "become", "box", "do", "final", "macro", "override",
    "priv", "typeof", "unsized", "virtual", "yield", "try"
};

// 初始化运算符映射
const std::unordered_map<std::string, TokenType> RustLexer::OPERATORS = {
    {"+", TokenType::OPERATOR}, {"-", TokenType::OPERATOR}, {"*", TokenType::OPERATOR},
    {"/", TokenType::OPERATOR}, {"%", TokenType::OPERATOR}, {"=", TokenType::OPERATOR},
    {"==", TokenType::OPERATOR}, {"!=", TokenType::OPERATOR}, {">", TokenType::OPERATOR},
    {"<", TokenType::OPERATOR}, {">=", TokenType::OPERATOR}, {"<=", TokenType::OPERATOR},
    {"&", TokenType::OPERATOR}, {"|", TokenType::OPERATOR}, {"^", TokenType::OPERATOR},
    {"!", TokenType::OPERATOR}, {"~", TokenType::OPERATOR}, {"&&", TokenType::OPERATOR},
    {"||", TokenType::OPERATOR}, {"+=", TokenType::OPERATOR}, {"-=", TokenType::OPERATOR},
    {"*=", TokenType::OPERATOR}, {"/=", TokenType::OPERATOR}, {"%=", TokenType::OPERATOR},
    {"&=", TokenType::OPERATOR}, {"|=", TokenType::OPERATOR}, {"^=", TokenType::OPERATOR},
    {"<<", TokenType::OPERATOR}, {">>", TokenType::OPERATOR}, {"<<=", TokenType::OPERATOR},
    {">>=", TokenType::OPERATOR}, {"..", TokenType::OPERATOR}, {"..=", TokenType::OPERATOR},
    {"->", TokenType::OPERATOR}, {"=>", TokenType::OPERATOR}
};

// 初始化分隔符映射
const std::unordered_map<char, TokenType> RustLexer::DELIMITERS = {
    {'(', TokenType::DELIMITER}, {')', TokenType::DELIMITER}, {'{', TokenType::DELIMITER},
    {'}', TokenType::DELIMITER}, {'[', TokenType::DELIMITER}, {']', TokenType::DELIMITER},
    {';', TokenType::DELIMITER}, {':', TokenType::DELIMITER}, {',', TokenType::DELIMITER},
    {'.', TokenType::DELIMITER}
};

RustLexer::RustLexer(const std::string& source)
    : source(source), position(0), line(1), column(0)
{
}

std::vector<Token> RustLexer::tokenize()
{
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        // 记录开始位置
        int startColumn = column;
        
        // 获取下一个单词
        Token token = scanToken();
        
        // 更新列位置
        token.column = startColumn;
        token.line = line;
        
        // 添加到结果中
        tokens.push_back(token);
    }
    
    return tokens;
}

char RustLexer::peek(int offset) const
{
    if (position + offset >= source.length()) {
        return '\0';
    }
    return source[position + offset];
}

char RustLexer::advance()
{
    char current = peek();
    position++;
    column++;
    
    if (current == '\n') {
        line++;
        column = 0;
    }
    
    return current;
}

bool RustLexer::isAtEnd() const
{
    return position >= source.length();
}

bool RustLexer::match(char expected)
{
    if (isAtEnd() || peek() != expected) {
        return false;
    }
    
    advance();
    return true;
}

Token RustLexer::scanToken()
{
    // 跳过空白字符
    while (peek() != '\0' && isascii(peek()) && std::isspace(peek())) {
        advance();
    }
    
    if (isAtEnd()) {
        Token token = {"", TokenType::UNKNOWN, line, column};
        return token;
    }
    
    char c = peek();
    
    // 标识符或关键字
    if (isAlpha(c) || c == '_') {
        return identifier();
    }
    
    // 数字
    if (isDigit(c)) {
        return number();
    }
    
    // 字符串字面量
    if (c == '"') {
        return string();
    }
    
    // 字符字面量
    if (c == '\'') {
        return character();
    }
    
    // 注释
    if (c == '/' && (peek(1) == '/' || peek(1) == '*')) {
        return comment();
    }
    
    // 运算符或分隔符
    return operatorOrDelimiter();
}

Token RustLexer::identifier()
{
    size_t start = position;
    
    // 第一个字符可以是字母或下划线
    advance();
    
    // 后续字符可以是字母、数字或下划线
    while (isAlphaNumeric(peek()) || peek() == '_') {
        advance();
    }
    
    // 提取标识符文本
    std::string text = source.substr(start, position - start);
    
    // 检查是否是关键字
    TokenType type = TokenType::IDENTIFIER;
    if (KEYWORDS.find(text) != KEYWORDS.end()) {
        type = TokenType::KEYWORD;
    }
    // 检查是否是宏调用
    else if (peek() == '!' && text != "r" && text != "b") {
        advance(); // 消费 '!'
        type = TokenType::MACRO_CALL;
    }
    
    return {text, type, line, column};
}

Token RustLexer::number()
{
    size_t start = position;
    TokenType type = TokenType::INTEGER_LITERAL;
    bool isFloat = false;
    
    // 检查是否是十六进制、八进制或二进制字面量
    if (peek() == '0') {
        advance();
        
        if (match('x') || match('X')) {
            // 十六进制
            while (isHexDigit(peek()) || peek() == '_') {
                advance();
            }
        }
        else if (match('o') || match('O')) {
            // 八进制
            while (isOctalDigit(peek()) || peek() == '_') {
                advance();
            }
        }
        else if (match('b') || match('B')) {
            // 二进制
            while (isBinaryDigit(peek()) || peek() == '_') {
                advance();
            }
        }
        else {
            // 可能是小数点开头的浮点数或普通的0
            // 继续处理
            if (peek() == '.') {
                isFloat = true;
                advance();
                while (isDigit(peek()) || peek() == '_') {
                    advance();
                }
            }
        }
    }
    else {
        // 处理普通十进制数
        while (isDigit(peek()) || peek() == '_') {
            advance();
        }
        
        // 检查是否是浮点数
        if (peek() == '.' && isDigit(peek(1))) {
            isFloat = true;
            advance(); // 消费 '.'
            
            // 小数部分
            while (isDigit(peek()) || peek() == '_') {
                advance();
            }
        }
    }
    
    // 处理科学计数法表示
    if ((peek() == 'e' || peek() == 'E') && (isDigit(peek(1)) || 
        ((peek(1) == '+' || peek(1) == '-') && isDigit(peek(2))))) {
        
        isFloat = true;
        advance(); // 消费 'e' 或 'E'
        
        if (match('+') || match('-')) {
            // 消费 '+' 或 '-'
        }
        
        while (isDigit(peek()) || peek() == '_') {
            advance();
        }
    }
    
    // 处理类型后缀，比如 u8, i32, f32 等
    if (isAlpha(peek())) {
        size_t typeStart = position;
        while (isAlphaNumeric(peek())) {
            advance();
        }
        std::string typeSuffix = source.substr(typeStart, position - typeStart);
        
        // 如果类型后缀以'f'开头，将类型设为浮点数
        if (!typeSuffix.empty() && (typeSuffix[0] == 'f')) {
            isFloat = true;
        }
    }
    
    // 根据是否有小数点或科学计数法决定类型
    if (isFloat) {
        type = TokenType::FLOAT_LITERAL;
    }
    
    return {source.substr(start, position - start), type, line, column};
}

Token RustLexer::string()
{
    size_t start = position;
    
    advance(); // 消费开头的引号
    
    // 处理字符串内容直到找到结束引号
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\' && peek(1) == '"') {
            advance(); // 消费转义字符 '\'
        }
        advance();
    }
    
    if (isAtEnd()) {
        // 字符串未闭合
        return {source.substr(start, position - start), TokenType::UNKNOWN, line, column};
    }
    
    advance(); // 消费结尾的引号
    
    return {source.substr(start, position - start), TokenType::STRING_LITERAL, line, column};
}

Token RustLexer::character()
{
    size_t start = position;
    
    advance(); // 消费开头的单引号
    
    // 处理字符内容
    if (peek() == '\\') {
        advance(); // 消费转义字符 '\'
        advance(); // 消费转义后的字符
    } else {
        // 处理普通字符，包括可能的Unicode字符
        // Unicode字符可能占据多个字节，但我们只需要前进一次
        // 让字符串提取来处理具体的编码
        advance();
        
        // 处理UTF-8编码的Unicode字符（可能的额外字节）
        // 在Rust中，字符是Unicode码点，可能需要多个字节
        // 通常情况下，我们不需要手动处理这些额外字节，因为字符串提取会包含它们
    }
    
    if (peek() != '\'') {
        // 字符未闭合或格式不正确
        while (!isAtEnd() && peek() != '\'') {
            advance();
        }
    }
    
    if (!isAtEnd()) {
        advance(); // 消费结尾的单引号
    }
    
    return {source.substr(start, position - start), TokenType::CHAR_LITERAL, line, column};
}

Token RustLexer::comment()
{
    size_t start = position;
    
    advance(); // 消费第一个'/'
    
    if (match('/')) {
        // 行注释
        while (!isAtEnd() && peek() != '\n') {
            advance();
        }
    } else if (match('*')) {
        // 块注释
        int nesting = 1;
        while (!isAtEnd() && nesting > 0) {
            if (peek() == '/' && peek(1) == '*') {
                advance(); advance();
                nesting++;
            } else if (peek() == '*' && peek(1) == '/') {
                advance(); advance();
                nesting--;
            } else {
                advance();
            }
        }
    }
    
    return {source.substr(start, position - start), TokenType::COMMENT, line, column};
}

Token RustLexer::operatorOrDelimiter()
{
    // 先检查是否是分隔符
    char c = peek();
    if (DELIMITERS.find(c) != DELIMITERS.end()) {
        advance();
        return {std::string(1, c), TokenType::DELIMITER, line, column};
    }
    
    // 尝试匹配最长的运算符
    size_t start = position;
    std::string op;
    
    // 最多匹配3个字符的运算符
    for (int i = 0; i < 3 && !isAtEnd(); i++) {
        op.push_back(advance());
        
        // 如果下一个字符不可能是运算符的一部分，就退出循环
        // 添加isascii检查，并使用我们自定义的检查函数
        if (isAtEnd() || (isascii(peek()) && std::isspace(static_cast<unsigned char>(peek()))) 
            || isAlphaNumeric(peek())) {
            break;
        }
    }
    
    // 从最长的可能运算符开始检查
    while (!op.empty()) {
        if (OPERATORS.find(op) != OPERATORS.end()) {
            return {op, TokenType::OPERATOR, line, column};
        }
        
        // 缩短运算符并回退扫描位置
        position--;
        column--;
        op.pop_back();
    }
    
    // 如果未识别出任何运算符，则消费一个字符并返回未知类型
    position = start + 1;
    column = column - op.length() + 1;
    return {std::string(1, source[start]), TokenType::UNKNOWN, line, column};
}

bool RustLexer::isDigit(char c) const
{
    return isascii(c) && std::isdigit(static_cast<unsigned char>(c));
}

bool RustLexer::isAlpha(char c) const
{
    return isascii(c) && std::isalpha(static_cast<unsigned char>(c));
}

bool RustLexer::isAlphaNumeric(char c) const
{
    return isascii(c) && (std::isalnum(static_cast<unsigned char>(c)));
}

bool RustLexer::isHexDigit(char c) const
{
    return isascii(c) && std::isxdigit(static_cast<unsigned char>(c));
}

bool RustLexer::isOctalDigit(char c) const
{
    return c >= '0' && c <= '7';
}

bool RustLexer::isBinaryDigit(char c) const
{
    return c == '0' || c == '1';
}