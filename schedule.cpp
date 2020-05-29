#include "schedule.h"
#include "ui_schedule.h"

using namespace std;

typedef vector<vector<int>> Dna;
typedef vector<pair<Dna, int>> Population;

Dna solve(vector<int>, int, vector<int>, int, vector<int>, int, vector<int>, int, int, double, double);

Schedule::Schedule(QWidget *parent,
                   std::vector<int> idTask,
                   QVector<QString> namesTask,
                   std::vector<int> lenth,
                   std::vector<int> resources,
                   std::vector<int> efficiency,
                   std::vector<int> peroid,
                   std::vector<int> idTsod,
                   QVector<QString> nameTsod,
                   QSqlDatabase *db) :
    QMainWindow(parent),
    ui(new Ui::schedule)
{
    ui->setupUi(this);


    qDebug() << "idtask " << idTask.size();
    qDebug() << "nametask " << namesTask.size();
    qDebug() << "len " << lenth.size();
    qDebug() << "res " << resources.size();
    qDebug() << "eff " << efficiency.size();
    qDebug() << "per " << peroid.size();
    qDebug() << "isTsod " << idTsod.size();
    qDebug() << "nameTsod " << nameTsod.size();

    idTaskLocal = idTask;
    namesTaskLocal = namesTask;
    lenthLocal = lenth;
    resourcesLocal = resources;
    efficiencyLocal = efficiency;
    peroidLocal = peroid;
    idTsodLocal = idTsod;
    nameTsodLocal = nameTsod;


    query = new QSqlQuery(*db);

    taskSchedule = new QTableView;
    taskSchedule->setParent(this);
    taskSchedule->setGeometry(20,40,this->size().width()-30,this->size().height()-120);
    taskSchedule->hide();

    query->prepare("select task, tsod, date_, latitude, longitude from schedule_");
    query->exec();
    QSqlQueryModel *model= new QSqlQueryModel;
    model->setQuery(*query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Работа"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Назавние ТСОД"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Дата"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Широта"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Долгота"));
    taskSchedule->setModel(model);
    //historyTable->setColumnHidden(0,true);
    taskSchedule->setColumnWidth(0,430);
    taskSchedule->setColumnWidth(1,150);
    taskSchedule->setColumnWidth(2,150);
    taskSchedule->setColumnWidth(3,150);
    taskSchedule->setColumnWidth(4,150);
    //taskSchedule->verticalHeader()->setVisible(false);
    taskSchedule->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    taskSchedule->show();

}

Schedule::~Schedule()
{
    delete ui;

}

void Schedule::on_pushButton_clicked()
{
    this->close();
}

void Schedule::on_pushButton_2_clicked()
{
    QString days = QInputDialog::getText(this, "Парамеир", "Колличество дней");
    QString res = QInputDialog::getText(this, "Парамеир", "Ограничение средств");
    insertData(days.toInt(), res.toInt());
}



void Schedule::insertData(int days, int res)
{
    query->exec("TRUNCATE TABLE schedule_");
    std::vector<std::vector<int>> tmp = solve(lenthLocal, 8, resourcesLocal, res, efficiencyLocal, 1000000, peroidLocal, days, idTaskLocal.size(), 0.5, 0.5);
    tmp[0][0] = 1;
    int k = 0;
    for (int i = 0; i < tmp.size(); ++i) {
        for (int j = 0; j < tmp[i].size(); ++j) {
            if (tmp[i][j]) {

                query->prepare("select latitude, longitude from tsod_coordinates where tsod_id = :id");
                query->bindValue(":id", idTsodLocal[j]);
                query->exec();
                query->next();
                double lat = query->value(0).toDouble();
                double log = query->value(1).toDouble();
                qDebug() << query->value(0).toDouble() << ' ' << query->value(1).toDouble();
                qDebug() <<"id " << idTsodLocal[j];


                query->prepare("insert into schedule_(task, tsod, date_, latitude, longitude) value(:taskName, :tsodName, CURDATE() + INTERVAL :days DAY, :lat, :log)");
                query->bindValue(":taskName", namesTaskLocal[j]);
                query->bindValue(":tsodName", nameTsodLocal[j] + ' ' + QString::number(idTsodLocal[j]));
                query->bindValue(":days", i + 1);
                query->bindValue(":lat", lat);
                query->bindValue(":log", log);
                query->exec();

                ui->statusbar->showMessage("Поколение" + QString::number(k));
                k++;
            }
        }
    }



    query->prepare("select task, tsod, date_, latitude, longitude from schedule_");
    query->exec();
    QSqlQueryModel *model= new QSqlQueryModel;
    model->setQuery(*query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Работа"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Назавние ТСОД"));
    //model->setHeaderData(2, Qt::Horizontal, QObject::tr("Тип ТСОД"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Широта"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Долгота"));
    taskSchedule->setModel(model);
    //historyTable->setColumnHidden(0,true);
    taskSchedule->setColumnWidth(0,430);
    taskSchedule->setColumnWidth(1,150);
    taskSchedule->setColumnWidth(2,150);
    taskSchedule->setColumnWidth(3,150);
    taskSchedule->setColumnWidth(4,150);
    //taskSchedule->verticalHeader()->setVisible(false);
    taskSchedule->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    taskSchedule->show();

}



struct sort_pred {
    bool operator()(const std::pair<Dna, int> &left, const std::pair<Dna, int> &right) {
        return left.second < right.second;
    }
};

struct sort_des {
    bool operator()(const std::pair<Dna, int> &left, const std::pair<Dna, int> &right) {
        return left.second > right.second;
    }
};

Dna propotionalSampling(Population population) {
    double sum = 0;
    for (int i = 0; i < population.size(); ++i) {
        sum += population[i].second;
    }

    vector<double> tmp;
    for (int i = 0; i < population.size(); ++i) {
        tmp.push_back(population[i].second / sum);
    }

    sort(population.begin(), population.end(), sort_pred());
    sort(tmp.begin(), tmp.end());

    vector<double> csum;
    csum.resize(population.size());
    csum[0] = tmp[0];
    for (int i = 1; i < csum.size(); ++i) {
        csum[i] = csum[i - 1] + tmp[i];
    }

    double r = (double)rand() / RAND_MAX;

    Dna result;

    for (int i = 0; i < csum.size(); ++i) {
        if (r <= csum[i]) {
            result = population[i].first;
            break;
        }
    }
    return result;
}


Dna backwardPropotionalSampling(Population population) {
    double sum = 0;
    double max = 0;
    for (int i = 0; i < population.size(); ++i) {
        if (population[i].second > max) {
            max = population[i].second;
        }
    }

    for (int i = 0; i < population.size(); ++i) {
        sum += max - population[i].second;
    }

    vector<double> tmp;
    for (int i = 0; i < population.size(); ++i) {
        tmp.push_back((max - population[i].second) / sum);
    }

    sort(population.begin(), population.end(), sort_des());
    sort(tmp.begin(), tmp.end());

    vector<double> csum;
    csum.resize(population.size());
    csum[0] = tmp[0];
    for (int i = 1; i < csum.size(); ++i) {
        csum[i] = csum[i - 1] + tmp[i];
    }

    double r = (double)rand() / RAND_MAX;
    //cout << r << endl;
    Dna result;

    for (int i = 0; i < csum.size(); ++i) {
        if (r <= csum[i]) {
            result = population[i].first;
            break;
        }
    }
    if (!result.size()) {
        return population[0].first;
    }
    return result;
}

Dna randomSampling(Population population) {
    int index = rand() % population.size();
    return population[index].first;
}

//фитнес функция
int fitness(vector<int> efficiency, Dna dna) {
    int res = 0;
    for (int i = 0; i < dna.size(); ++i) {
        for (int j = 0; j < dna[i].size(); ++j) {
            res += efficiency[j] * dna[i][j];
        }
    }
    return res;
}

//затрачиваемые ресурсы
int resources(vector<int> resources, Dna dna) {
    int res = 0;
    for (int i = 0; i < dna.size(); ++i) {
        for (int j = 0; j < dna[i].size(); ++j) {
            res += resources[j] * dna[i][j];
        }
    }
    return res;
}

//максимально загруженный день
int maxLoadDay(vector<int> load, Dna dna) {
    int result = 0;
    for (int i = 0; i < dna.size(); ++i) {
        int tmp = 0;
        for (int j = 0; j < dna[i].size(); ++j) {
            tmp += load[j] * dna[i][j];
        }
        if (tmp > result) {
            result = tmp;
        }
    }
    return result;
}

int timeIterval(vector<int> dayInterval, Dna dna) {
    if (!dna.size()) {
        return 0;
    }
    int works = dna[0].size();
    int days = dna.size();
    for (int i = 0; i < works; ++i) {
        int tmp = 0;
        for (int j = 0; j < days; ++j) {
            if (!dna[j][i]) {
                tmp++;
            }
            else {
                if (tmp < dayInterval[i]) {
                    return 0;
                }
                tmp = 0;
            }
        }
    }
    return 1;
}

// создание случайной днк
Dna initDna(int days, int works) {
    Dna dna(days, vector<int>(works));
    for (int i = 0; i < dna.size(); ++i) {
        for (int j = 0; j < dna[i].size(); ++j) {
            dna[i][j] = (rand() % 100) < 1;
        }
    }
    return dna;
}

// создание изначальной популяции
Population initPopulation(int populationSize, int days, int works) {
    Population population;
    population.resize(populationSize);

    for (int i = 0; i < population.size(); ++i) {
        population[i] = make_pair(initDna(days, works), 0);
    }
    return population;
}

// скрешивание
Dna crosingover(Dna dna1, Dna dna2) {
    Dna newDna = dna1;
    int parts = 3;
    int currentPart = 1;

    int index = 0;
    for (; currentPart < parts;) {

        for (; index < newDna.size()*currentPart/ 3; ++index) {
            newDna[index] = dna1[index];
        }
        currentPart++;

        for (; index < newDna.size()*currentPart / 3; ++index) {
            newDna[index] = dna2[index];
        }
        currentPart++;
    }

    return newDna;
}

//мутации
Dna mutation(Dna dna) {
    for (int i = 0; i < dna.size(); ++i) {
        for (int j = 0; j < dna[i].size(); ++j) {
            if ((rand() % 100) < 40) {
                dna[i][j] = (dna[i][j] == 0);
            }
        }
    }
    return dna;
}

Dna decriseResources(Dna dna) {
    for (int i = 0; i < dna.size(); ++i) {
        for (int j = 0; j < dna[i].size(); ++j) {
            if (dna[i][j]) {
                if ((rand() % 100) < 20) {
                    dna[i][j] = 0;
                }
            }
        }
    }
    return dna;
}

Dna addOnes(Dna dna) {
    for (int i = 0; i < dna.size(); ++i) {
        for (int j = 0; j < dna[i].size(); ++j) {
            if (!dna[i][j]) {
                if ((rand() % 100) < 2) {
                    dna[i][j] = 1;
                }
            }
        }
    }
    return dna;
}

Dna mixUpDays(Dna dna) {
    int days = dna.size();
    int countOfMixUpDays = days * 0.3;
    for (int i = 0; i < countOfMixUpDays; ++i) {
        iter_swap(dna.begin() + (rand() % days), dna.begin() + (rand() % days));
    }
    return dna;
}

Population addRondom(Population population, int populationSize, int days, int works, vector<int> eff) {
    while (population.size() < populationSize) {
        Dna tmp = initDna(days, works);
        population.push_back(make_pair(tmp, fitness(eff,tmp)));
    }
    return population;
}

Population selection(Population population, double proportionOfSelection, vector<int> eff) {
    int newDnaCount = population.size() * proportionOfSelection; //Количество новых особей

    Population newPopulation = population;

    for (int i = 0; i < newDnaCount; ++i) {
        Dna tmp = crosingover(propotionalSampling(population), propotionalSampling(population));
        newPopulation.push_back(make_pair(tmp, fitness(eff, tmp)));
    }
    return newPopulation;
}

Population generationMutation(Population population,double proportionOfMutation, vector<int> eff) {
    int mutatedDnaCount = population.size() * proportionOfMutation;

    Population newPopulation = population;

    for (int i = 0; i < mutatedDnaCount; ++i) {
        Dna tmp = mutation(randomSampling(population));
        newPopulation.push_back(make_pair(tmp, fitness(eff, tmp)));
    }
    return newPopulation;
}

Population mutateOverRes(Population population, vector<int> res, vector<int> eff, int resTarget) {
    for (int i = 0; i < population.size(); ++i) {
        if (resources(res, population[i].first) > resTarget) {
            population[i].first = decriseResources(population[i].first);
            population[i].second = fitness(eff, population[i].first);
        }
    }
    return population;
}

Population mutateUnderRes(Population population, vector<int> res, vector<int> eff, int resTarget) {
    for (int i = 0; i < population.size(); ++i) {
        if (resources(res, population[i].first) < resTarget) {
            population[i].first = addOnes(population[i].first);
            population[i].second = fitness(eff, population[i].first);
        }
    }
    return population;
}

Population mutateMixDays(Population population, vector<int> dayInterval) {
    for (int i = 0; i < population.size(); ++i) {
        if (!timeIterval(dayInterval, population[i].first)) {
            population[i].first = mixUpDays(population[i].first);
        }
    }
    return population;
}

Population removeExtra(Population population, int populationSize, vector<int> eff) {
    while (population.size() > populationSize) {
        Dna tmp = backwardPropotionalSampling(population);
        if (tmp.size() != 0) {
            population.erase(find(population.begin(), population.end(), make_pair(tmp, fitness(eff, tmp))));
        }
    }
    return population;
}

Population removeUsuitable(Population population, vector<int> res, int resTarget, vector<int> load, int loadTarget, vector<int> dayInterval) {
    for (int i = 0; i < population.size();) {
        if (resources(res, population[i].first) > resTarget) {
            population.erase(population.begin() + i);
        }/*
        else if (maxLoadDay(load, population[i].first) > loadTarget)
        {
            population.erase(population.begin() + i);
        }
        else if (!timeIterval(dayInterval, population[i].first)) {
            population.erase(population.begin() + i);
        }*/
        else {
            ++i;
        }
    }
    return population;
}

Dna findMostFited(Population population) {
    int max = 0;
    int index = 0;
    for (int i = 0; i < population.size(); ++i) {
        if (population[i].second > max) {
            max = population[i].second;
            index = i;
        }
    }
    if (index) {
        return population[index].first;
    }
    else {
        return Dna();
    }
}


int howManyOverFited(Population population, vector<int> res, int targetRes) {
    int count = 0;
    for (int i = 0; i < population.size(); ++i) {
        if (resources(res, population[i].first) > targetRes) {
            ++count;
        }
    }
    return count;
}

Dna Schedule::solve(vector<int> load, int loadTarget, vector<int> resources, int resourcesTarget, vector<int> efficiency, int targetEfficiency, vector<int> dayInterval,
    int days, int works, double proportionOfSelection, double proportionOfMutation) {
    srand(time(0));
    int populationSize = 100; // популяция

    // Оценка популяции
    Population population = initPopulation(100, days, works);
    for (int i = 0; i < population.size(); ++i) {
        population[i].second = fitness(efficiency, population[i].first);
    }

    Dna mostFited = findMostFited(population);
    Population mostFitedV;
    //mostFitedV.push_back(make_pair(mostFited, fitness(efficiency, mostFited)));

    int maxGeneration = 10;
    int currentGeneraion = 0;
    while (currentGeneraion < maxGeneration) {

        if (population.size() > populationSize) {
            population = removeExtra(population, populationSize, efficiency);
        }
        else if (population.size() < populationSize) {
            population = addRondom(population, populationSize, days, works, efficiency);
        }
        population = selection(population, proportionOfSelection, efficiency);

        population = generationMutation(population, proportionOfMutation, efficiency);


        //population = mutateUnderRes(population, resources, efficiency, resourcesTarget);

        //population = mutateOverRes(population, resources, efficiency, resourcesTarget);

        //population = mutateMixDays(population, dayInterval);


        population = removeUsuitable(population, resources, resourcesTarget, load, loadTarget, dayInterval);
        qDebug() << "Generaton: " << currentGeneraion << " Efficiency: " << fitness(efficiency, mostFited) << " Size: " << population.size() << endl;
        ui->statusbar->showMessage("Поколение" + QString::number(currentGeneraion));

        mostFited = findMostFited(population);
        mostFitedV.push_back(make_pair(mostFited, fitness(efficiency, mostFited)));
        if (fitness(efficiency, mostFited) > targetEfficiency) {
            return mostFited;
        }
        currentGeneraion++;
    }

    int max = 0;
    int index = 0;
    for (int i = 0; i < mostFitedV.size(); ++i) {
        if (mostFitedV[i].second > max) {
            max = mostFitedV[i].second;
            index = i;
        }
    }
    return mostFitedV[index].first;
}
