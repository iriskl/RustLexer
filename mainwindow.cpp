#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置窗口标题
    setWindowTitle("Rust单词拼装分类器");
    // 设置窗口大小
    resize(1000, 700);
    
    // 设置UI布局
    setupUI();
    // 设置菜单栏
    setupMenus();
    
    // 设置状态栏
    statusLabel = new QLabel("就绪");
    statusBar()->addWidget(statusLabel);
}

MainWindow::~MainWindow()
{
    // 无需手动删除UI组件，它们会随着父窗口一起销毁
}

void MainWindow::setupUI()
{
    // 创建主部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建分割器，允许调整代码编辑器和结果显示区域的大小
    QSplitter *splitter = new QSplitter(Qt::Vertical);
    
    // 创建代码编辑器
    codeEditor = new QPlainTextEdit(this);
    codeEditor->setFont(QFont("Consolas", 11));
    codeEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    codeEditor->setTabStopDistance(40);
    
    // 创建结果显示区域
    resultDisplay = new QTextEdit(this);
    resultDisplay->setReadOnly(true);
    resultDisplay->setFont(QFont("Consolas", 11));
    
    // 添加组件到分割器
    splitter->addWidget(codeEditor);
    splitter->addWidget(resultDisplay);
    
    // 设置分割器的大小比例
    splitter->setStretchFactor(0, 4); // 代码编辑区占40%
    splitter->setStretchFactor(1, 6); // 结果显示区占60%
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(splitter);
    
    // 添加分析按钮
    QPushButton *analyzeButton = new QPushButton("分析代码", this);
    connect(analyzeButton, &QPushButton::clicked, this, &MainWindow::analyzeCode);
    
    // 将按钮添加到布局底部
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(analyzeButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
}

void MainWindow::setupMenus()
{
    // 创建文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    
    // 添加打开文件动作
    openAction = fileMenu->addAction("打开(&O)");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    
    fileMenu->addSeparator();
    
    // 添加退出动作
    QAction *exitAction = fileMenu->addAction("退出(&Q)");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // 创建分析菜单
    QMenu *analyzeMenu = menuBar()->addMenu("分析(&A)");
    
    // 添加分析代码动作
    analyzeAction = analyzeMenu->addAction("分析代码(&A)");
    analyzeAction->setShortcut(Qt::Key_F5);
    connect(analyzeAction, &QAction::triggered, this, &MainWindow::analyzeCode);
}

void MainWindow::openFile()
{
    // 打开文件对话框
    QString filePath = QFileDialog::getOpenFileName(this, 
                                                   "打开Rust源文件", 
                                                   "", 
                                                   "Rust源文件 (*.rs);;所有文件 (*.*)");
    
    if (filePath.isEmpty())
        return;
    
    // 打开文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件：" + filePath);
        return;
    }
    
    // 读取文件内容到代码编辑器
    QTextStream in(&file);
    // 删除 in.setCodec("UTF-8"); 这一行
    // Qt 6已经默认使用UTF-8编码
    codeEditor->setPlainText(in.readAll());
    file.close();
    
    // 保存当前文件路径
    currentFilePath = filePath;
    statusLabel->setText("已加载文件：" + QFileInfo(filePath).fileName());
}

void MainWindow::analyzeCode()
{
    if (codeEditor->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "警告", "请先输入或打开Rust代码");
        return;
    }
    
    // 获取代码文本
    QString code = codeEditor->toPlainText();
    
    // 创建词法分析器并分析代码
    RustLexer lexer(code.toStdString());
    std::vector<Token> tokens = lexer.tokenize();
    
    // 显示分析结果
    displayTokens(tokens);
    
    statusLabel->setText("分析完成，共识别 " + QString::number(tokens.size()) + " 个单词");
}

void MainWindow::displayTokens(const std::vector<Token>& tokens)
{
    resultDisplay->clear();
    
    if (tokens.empty()) {
        resultDisplay->setPlainText("未找到任何单词");
        return;
    }
    
    // 更改整体背景颜色
    resultDisplay->setStyleSheet("background-color: #f8f8f8; color: #000000;");
    
    QString result;
    QStringList lines = codeEditor->toPlainText().split("\n");
    
    // 按行组织tokens
    std::map<int, std::vector<Token>> tokensByLine;
    for (const Token& token : tokens) {
        tokensByLine[token.line].push_back(token);
    }
    
    // 遍历每一行
    for (const auto& [lineNum, lineTokens] : tokensByLine) {
        // 确保行号有效
        if (lineNum <= lines.size() && lineNum > 0) {
            // 输出源代码行
            QString sourceLine = lines[lineNum - 1];
            result += QString("<div style='background-color:#e8e8e8; padding: 5px; margin: 8px 0; border-left: 3px solid #4080c0; font-family: Consolas;'>") 
                   + sourceLine.toHtmlEscaped() + QString("</div>\n");
            
            // 输出该行的所有token
            for (const Token& token : lineTokens) {
                QString color;
                QString typeString;
                
                // 根据token类型设置颜色和类型文本
                switch (token.type) {
                    case TokenType::KEYWORD:      
                        typeString = "关键字"; 
                        color = "#0000CC"; 
                        break;
                    case TokenType::IDENTIFIER:   
                        typeString = "标识符"; 
                        color = "#006600"; 
                        break;
                    case TokenType::INTEGER_LITERAL: 
                        typeString = "字面量（整数）"; 
                        color = "#990099"; 
                        break;
                    case TokenType::FLOAT_LITERAL:   
                        typeString = "字面量（浮点数）"; 
                        color = "#990099"; 
                        break;
                    case TokenType::STRING_LITERAL:  
                        typeString = "字符串字面量"; 
                        color = "#CC0000"; 
                        break;
                    case TokenType::CHAR_LITERAL:    
                        typeString = "字符字面量"; 
                        color = "#CC0000"; 
                        break;
                    case TokenType::OPERATOR:     
                        typeString = "操作符"; 
                        color = "#000088"; 
                        break;
                    case TokenType::DELIMITER:    
                        typeString = "分隔符"; 
                        color = "#444444"; 
                        break;
                    case TokenType::COMMENT:      
                        typeString = "注释"; 
                        color = "#886600"; 
                        break;
                    case TokenType::MACRO_CALL:   
                        typeString = "宏调用名"; 
                        color = "#884400"; 
                        break;
                    default:                      
                        typeString = "未知类型"; 
                        color = "#000000"; 
                        break;
                }
                
                // 输出token及其类型
                result += QString("<div style='margin-left: 20px; color:") 
                       + color + QString(";'>") 
                       + QString::fromStdString(token.lexeme).toHtmlEscaped() 
                       + QString(": ") + typeString + QString("</div>\n");
            }
            
            // 添加行间距
            result += QString("<br>\n");
        }
    }
    
    resultDisplay->setHtml(result);
}
