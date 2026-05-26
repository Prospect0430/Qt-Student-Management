#include "mainwindow.h"
#include <QHeaderView>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QTextStream>
#include <QFile>
#include <QStringConverter>
#include <QDebug>
#include <algorithm>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("学生信息管理系统");
    resize(900, 600);

    // 中央控件
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // 搜索栏
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel("学号/姓名:"));
    searchEdit = new QLineEdit();
    searchLayout->addWidget(searchEdit);
    QPushButton* searchBtn = new QPushButton("搜索");
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchStudent);
    searchLayout->addWidget(searchBtn);
    mainLayout->addLayout(searchLayout);

    // 表格
    table = new QTableWidget();
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({ "姓名", "学号", "性别", "专业", "地址", "出生日期", "成绩" });
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(table);

    // 按钮栏
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton("录入学生");
    QPushButton* deleteBtn = new QPushButton("删除学生");
    QPushButton* showAllBtn = new QPushButton("显示全部");
    QPushButton* saveBtn = new QPushButton("保存到文件");
    QPushButton* loadBtn = new QPushButton("读取文件");
    QPushButton* statBtn = new QPushButton("统计信息");
    QPushButton* sortBtn = new QPushButton("按数学排序");

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddStudent);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteStudent);
    connect(showAllBtn, &QPushButton::clicked, this, &MainWindow::onShowAll);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveToFile);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadFromFile);
    connect(statBtn, &QPushButton::clicked, this, &MainWindow::onStatistic);
    connect(sortBtn, &QPushButton::clicked, this, &MainWindow::onSortByMath);

    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(showAllBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(loadBtn);
    btnLayout->addWidget(statBtn);
    btnLayout->addWidget(sortBtn);
    mainLayout->addLayout(btnLayout);

  
    QFile autoFile("students.txt");
    if (autoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&autoFile);
        in.setEncoding(QStringConverter::Utf8);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.isEmpty()) continue;
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() < 6) break;
            Student s;
            s.setName(parts[0].toStdString());
            s.setId(parts[1].toStdString());
            s.setGender(parts[2].toStdString());
            s.setaddress(parts[3].toStdString());
            s.setMajor(parts[4].toStdString());
            s.setBorndate(parts[5].toStdString());
            while (!in.atEnd()) {
                QString courseLine = in.readLine();
                if (courseLine.trimmed() == "0") break;
                QStringList courseParts = courseLine.split(' ', Qt::SkipEmptyParts);
                if (courseParts.size() >= 2) {
                    bool ok;
                    float score = courseParts[1].toFloat(&ok);
                    if (ok) {
                        s.addScore(courseParts[0].toStdString(), score);
                    }
                }
            }
            students.push_back(s);
        }
        autoFile.close();
    }

    refreshTable();
}

MainWindow::~MainWindow() {}

void MainWindow::refreshTable()
{
    displayStudents(students);
}

void MainWindow::displayStudents(vector<Student>& stuList)
{
    table->setRowCount(stuList.size());
    for (size_t i = 0; i < stuList.size(); ++i) {
        Student& s = stuList[i];
        QString scoresStr;
        for (auto& p : s.getAllScores()) {
            scoresStr += QString("%1:%2 ").arg(QString::fromStdString(p.first)).arg(p.second);
        }
        table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(s.getName())));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s.getId())));
        table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(s.getGender())));
        table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(s.getMajor())));
        table->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(s.getaddress())));
        table->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(s.getBorndate())));
        table->setItem(i, 6, new QTableWidgetItem(scoresStr));
    }
}

void MainWindow::onAddStudent()
{
    QDialog dialog(this);
    dialog.setWindowTitle("录入学生");
    QFormLayout form(&dialog);
    QLineEdit nameEdit, idEdit, genderEdit, majorEdit, addrEdit, birthEdit;
    form.addRow("姓名:", &nameEdit);
    form.addRow("学号:", &idEdit);
    form.addRow("性别:", &genderEdit);
    form.addRow("专业:", &majorEdit);
    form.addRow("地址:", &addrEdit);
    form.addRow("出生日期:", &birthEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        Student s;
        s.setName(nameEdit.text().toStdString());
        s.setId(idEdit.text().toStdString());
        s.setGender(genderEdit.text().toStdString());
        s.setMajor(majorEdit.text().toStdString());
        s.setaddress(addrEdit.text().toStdString());
        s.setBorndate(birthEdit.text().toStdString());

        // 录入课程和成绩（循环输入，可无限添加）
        bool ok;
        QString course = QInputDialog::getText(&dialog, "课程", "课程名(输入0结束):", QLineEdit::Normal, "", &ok);
        while (ok && !course.isEmpty() && course != "0") {
            double score = QInputDialog::getDouble(&dialog, "成绩", "成绩:", 0, 0, 100, 1, &ok);
            if (ok) {
                s.addScore(course.toStdString(), score);
            }
            course = QInputDialog::getText(&dialog, "课程", "课程名(输入0结束):", QLineEdit::Normal, "", &ok);
        }
        students.push_back(s);
        refreshTable();
        QMessageBox::information(this, "成功", "学生录入成功！");
    }
}

void MainWindow::onDeleteStudent()
{
    int row = table->currentRow();
    if (row < 0 || row >= (int)students.size()) {
        QMessageBox::warning(this, "错误", "请选择要删除的行");
        return;
    }
    students.erase(students.begin() + row);
    refreshTable();
    QMessageBox::information(this, "成功", "删除成功！");
}

void MainWindow::onSearchStudent()
{
    QString keyword = searchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        onShowAll();
        return;
    }
    vector<Student> result;
    for (auto& s : students) {
        if (QString::fromStdString(s.getId()).contains(keyword) ||
            QString::fromStdString(s.getName()).contains(keyword)) {
            result.push_back(s);
        }
    }
    displayStudents(result);
    if (result.empty()) {
        QMessageBox::information(this, "提示", "未找到匹配学生");
    }
}

void MainWindow::onShowAll()
{
    displayStudents(students);
}

void MainWindow::onSaveToFile()
{
    // 保存到 students.txt（和源程序一致）
    QFile file("students.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法向文件写入");
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    for (auto& s : students) {
        out << QString::fromStdString(s.getName()) << " "
            << QString::fromStdString(s.getId()) << " "
            << QString::fromStdString(s.getGender()) << " "
            << QString::fromStdString(s.getaddress()) << " "
            << QString::fromStdString(s.getMajor()) << " "
            << QString::fromStdString(s.getBorndate()) << "\n";
        for (const auto& p : s.getAllScores()) {
            out << QString::fromStdString(p.first) << " " << p.second << "\n";
        }
        out << "0\n";
    }
    file.close();
    QMessageBox::information(this, "成功", "保存成功！");
}

void MainWindow::onLoadFromFile()
{
    // 从 students.txt 重新加载
    QFile file("students.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开 students.txt");
        return;
    }
    students.clear();
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 6) break;
        Student s;
        s.setName(parts[0].toStdString());
        s.setId(parts[1].toStdString());
        s.setGender(parts[2].toStdString());
        s.setaddress(parts[3].toStdString());
        s.setMajor(parts[4].toStdString());
        s.setBorndate(parts[5].toStdString());
        while (!in.atEnd()) {
            QString courseLine = in.readLine();
            if (courseLine.trimmed() == "0") break;
            QStringList courseParts = courseLine.split(' ', Qt::SkipEmptyParts);
            if (courseParts.size() >= 2) {
                bool ok;
                float score = courseParts[1].toFloat(&ok);
                if (ok) {
                    s.addScore(courseParts[0].toStdString(), score);
                }
            }
        }
        students.push_back(s);
    }
    file.close();
    refreshTable();
    QMessageBox::information(this, "成功", "读取完成！");
}

void MainWindow::onStatistic()
{
    if (students.empty()) {
        QMessageBox::information(this, "统计", "暂无学生数据");
        return;
    }
    double totalScore = 0;
    double maxScore = -1;
    int failCount = 0;
    int excellentCount = 0;
    int courseCount = 0;
    for (auto& s : students) {
        for (auto& p : s.getAllScores()) {
            double sc = p.second;
            totalScore += sc;
            courseCount++;
            if (sc > maxScore) maxScore = sc;
            if (sc < 60) failCount++;
            if (sc >= 90) excellentCount++;
        }
    }
    double avg = courseCount ? totalScore / courseCount : 0;
    QMessageBox::information(this, "统计信息",
        QString("学生总数: %1\n课程总门次: %2\n平均分: %3\n最高分: %4\n不及格门次: %5\n优秀门次(≥90): %6")
        .arg(students.size())
        .arg(courseCount)
        .arg(avg, 0, 'f', 2)
        .arg(maxScore, 0, 'f', 1)
        .arg(failCount)
        .arg(excellentCount));
}

void MainWindow::onSortByMath()
{
    if (students.empty()) {
        QMessageBox::information(this, "排序", "暂无学生数据");
        return;
    }
    std::sort(students.begin(), students.end(), [](Student& a, Student& b) {
        float sa = a.getScore("数学");
        float sb = b.getScore("数学");
        // 没有数学成绩的排到最后
        if (sa < 0 && sb < 0) return false;
        if (sa < 0) return false;
        if (sb < 0) return true;
        return sa > sb;
        });
    refreshTable();
    QMessageBox::information(this, "排序", "已按数学成绩从高到低排序");
}
