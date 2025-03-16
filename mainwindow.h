#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabWidget>
#include <QSplitter>
#include <QScrollBar>
#include <QStatusBar>
#include "rustlexer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void analyzeCode();

private:
    QPlainTextEdit *codeEditor;
    QTextEdit *resultDisplay;
    QString currentFilePath;
    QLabel *statusLabel;
    QAction *openAction;
    QAction *analyzeAction;
    
    void setupUI();
    void setupMenus();
    void displayTokens(const std::vector<Token>& tokens);
};

#endif // MAINWINDOW_H
