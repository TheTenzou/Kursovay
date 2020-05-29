#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QMainWindow>
#include <QInputDialog>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <vector>
#include <QVector>

#include <mapwindow.h>

namespace Ui {
class schedule;
}

using namespace std;
typedef vector<vector<int>> Dna;

class Schedule : public QMainWindow
{
    Q_OBJECT

public:
    explicit Schedule(QWidget *parent = nullptr,
            std::vector<int> idTask = std::vector<int>(),
            QVector<QString> namesTask = QVector<QString>(),
            std::vector<int> lenth = std::vector<int>(),
            std::vector<int> resources = std::vector<int>(),
            std::vector<int> efficiency = std::vector<int>(),
            std::vector<int> peroid = std::vector<int>(),
            std::vector<int> idTsod = std::vector<int>(),
            QVector<QString> nameTsod = QVector<QString>(),
            QSqlDatabase *db = nullptr);
    ~Schedule();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    void insertData(int days, int res);
    Dna Schedule::solve(vector<int> load, int loadTarget, vector<int> resources,
        int resourcesTarget, vector<int> efficiency, int targetEfficiency, vector<int> dayInterval,
        int days, int works, double proportionOfSelection, double proportionOfMutation);

private:
    Ui::schedule *ui;

    //QSqlDatabase *dataBase;
    QSqlQuery *query;

    std::vector<int> idTaskLocal;
    QVector<QString> namesTaskLocal;
    std::vector<int> lenthLocal;
    std::vector<int> resourcesLocal;
    std::vector<int> efficiencyLocal;
    std::vector<int> peroidLocal;
    std::vector<int> idTsodLocal;
    QVector<QString> nameTsodLocal;

    QTableView *taskSchedule;

};

#endif // SCHEDULE_H
