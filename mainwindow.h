#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <vector>
#include "./Student.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onAddStudent();        // 录入学生
    void onDeleteStudent();     // 删除学生
    void onSearchStudent();     // 查找学生
    void onShowAll();           // 显示全部学生
    void onSaveToFile();        // 保存到文件
    void onLoadFromFile();      // 从文件读取
    void onStatistic();         // 统计（平均分/最高分/不及格）
    void onSortByMath();        // 按数学成绩排序

private:
    void refreshTable();        // 刷新表格显示
    void displayStudents(vector<Student>& students); // 显示指定学生列表

    vector<Student> students;
    QTableWidget* table;
    QLineEdit* searchEdit;
};

#endif
