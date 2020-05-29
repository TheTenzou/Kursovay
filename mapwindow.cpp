#include "mapwindow.h"
#include "ui_mapwindow.h"
#include <QPixmap>
#include <QtWidgets>


MapWindow::MapWindow(QWidget *parent, QSqlDatabase *db) :
    QMainWindow(parent),
    imageLabel(new QLabel),
    scrollArea(new QScrollArea),
    ui(new Ui::MapWindow),
    scaleFactor(1)
{
    ui->setupUi(this);
    dataBase = *db;
    query = new QSqlQuery(dataBase);

    //лейбел для изображения
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    //скролы для лейбла
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    //setCentralWidget(scrollArea);
    scrollArea->setParent(this);
    scrollArea->move(0,20);
    scrollArea->show();

    //создаем действия
    createActions();

    //размер окна
    resize(QGuiApplication::primaryScreen()->availableSize()* 3 / 5);

    //загрузка изображения
    mapPix.load(":/Img/Map.jpg");
    imageLabel->setPixmap(mapPix);
    imageLabel->adjustSize();
    scaleFactor = 1.0;

    paint();

    scrollArea->setVisible(true);
    //fitToWindowAct->setEnabled(true);

    //обновляем действия
    updateActions();
    //scaleImage(0.5);

    //связь мапера с функцией
    connect(&mapper, SIGNAL(mapped(int)), this, SLOT(clickOnLabel(int)));


    //кнорка увеличения
    zoomInButton = new QPushButton();
    zoomInButton->setText("+");
    zoomInButton->setParent(scrollArea);
    zoomInButton->setGeometry(scrollArea->size().width()-30,20,30,30);
    connect(zoomInButton, SIGNAL (clicked()), this, SLOT(zoomIn()));
    zoomInButton->show();

    //кнопка уменьшения
    zoomOutButton = new QPushButton();
    zoomOutButton->setText("-");
    zoomOutButton->setParent(scrollArea);
    zoomOutButton->setGeometry(scrollArea->size().width()-30,60,30,30);
    connect(zoomOutButton, SIGNAL (clicked()), this, SLOT(zoomOut()));
    zoomOutButton->show();


    addTsodTitle = new QLabel;
    QFont font( "Arial", 12);
    addTsodTitle->setFont(font);
    addTsodTitle->setText("Ввод данных ТСОД");
    addTsodTitle->setParent(this);
    addTsodTitle->setGeometry(60,40,150,40);
    addTsodTitle->setGeometry(this->size().width()-240,40,150,40);
    addTsodTitle->hide();

    addTsodType = new QLabel;
    QFont font1( "Arial", 10);
    addTsodType->setFont(font1);
    addTsodType->setText("Тип");
    addTsodType->setParent(this);
    addTsodType->setGeometry(this->size().width()-270,100,150,30);
    addTsodType->hide();

    addTsodName = new QLabel;
    addTsodName->setFont(font1);
    addTsodName->setText("Название");
    addTsodName->setParent(this);
    addTsodName->setGeometry(this->size().width()-270,130,150,30);
    addTsodName->hide();

    addTsodVisibility = new QLabel;
    addTsodVisibility->setFont(font1);
    addTsodVisibility->setText("Видимость");
    addTsodVisibility->setParent(this);
    addTsodVisibility->setGeometry(this->size().width()-270,160,150,30);
    addTsodVisibility->hide();

    addTsodCondition = new QLabel;
    addTsodCondition->setFont(font1);
    addTsodCondition->setText("Состояние");
    addTsodCondition->setParent(this);
    addTsodCondition->setGeometry(this->size().width()-270,190,150,30);
    addTsodCondition->hide();

    addTsodAttribute = new QLabel;
    addTsodAttribute->setFont(font1);
    addTsodAttribute->setText("Атрибут");
    addTsodAttribute->setParent(this);
    addTsodAttribute->setGeometry(this->size().width()-270,220,150,30);
    addTsodAttribute->hide();

    query->exec("select null union select type_name from tsod_type");
    QSqlQueryModel *model =new QSqlQueryModel();
    model->setQuery(*query);

    addTsodTypeComboBox = new QComboBox;
    addTsodTypeComboBox->setFont(font1);
    //addTsodNameComboBox->setText("Атрибут");
    addTsodTypeComboBox->setParent(this);
    addTsodTypeComboBox->setGeometry(this->size().width()-170,105,150,20);
    addTsodTypeComboBox->setModel(model);
    connect(addTsodTypeComboBox, SIGNAL(currentIndexChanged(int)),this, SLOT(comboTypesel(int)));
    addTsodTypeComboBox->hide();

    addTsodNameComboBox = new QComboBox;
    addTsodNameComboBox->setFont(font1);
    addTsodNameComboBox->setParent(this);
    addTsodNameComboBox->setGeometry(this->size().width()-170,135,150,20);
    query->prepare("select null,-1 union select tsod_name, id from tsod_name");
    query->exec();
    addTsodNameComboBox->clear();
    type_id_list.clear();
    while(query->next())
    {
        type_id_list.push_back(query->value(1).toInt());
        addTsodNameComboBox->addItem(query->value(0).toString());
    }
    //addTsodNameComboBox->setModel(model);
    addTsodNameComboBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    addTsodNameComboBox->setEditable(true);
    addTsodNameComboBox->hide();

    query->exec("select null union select condition_name from conditions");
    model =new QSqlQueryModel();
    model->setQuery(*query);

    addTsodConditionsComboBox = new QComboBox;
    addTsodConditionsComboBox->setFont(font1);
    addTsodConditionsComboBox->setParent(this);
    addTsodConditionsComboBox->setGeometry(this->size().width()-170,195,150,20);
    addTsodConditionsComboBox->setModel(model);
    addTsodConditionsComboBox->hide();

    addTsodVisibilityLineEdit = new QLineEdit;
    addTsodVisibilityLineEdit->setFont(font1);
    addTsodVisibilityLineEdit->setParent(this);
    addTsodVisibilityLineEdit->setGeometry(this->size().width()-170,165,150,20);
    addTsodVisibilityLineEdit->hide();

    addTsodAttributeLineEdit = new QLineEdit;
    addTsodAttributeLineEdit->setFont(font1);
    addTsodAttributeLineEdit->setParent(this);
    addTsodAttributeLineEdit->setGeometry(this->size().width()-170,225,150,20);
    addTsodAttributeLineEdit->hide();

    addTsodButton = new QPushButton;
    addTsodButton->setParent(this);
    addTsodButton->setText("Добавить");
    addTsodButton->setGeometry(this->size().width()-220,260,70,30);
    connect(addTsodButton, SIGNAL (clicked()), this, SLOT(addTsodM()));
    addTsodButton->hide();

    cancelTsodButton = new QPushButton;
    cancelTsodButton->setParent(this);
    cancelTsodButton->setText("Отмена");
    cancelTsodButton->setGeometry(this->size().width()-80,260,60,30);
    connect(cancelTsodButton, SIGNAL (clicked()), this, SLOT(cencelAdd()));
    cancelTsodButton->hide();

    //view tsod
    baseLabelTsodView = new QLabel;
    baseLabelTsodView->setParent(this);
    baseLabelTsodView->setGeometry(10,30,260,240);
    mapPix.load(":/Img/baseTsodInf.png");
    baseLabelTsodView->setPixmap(mapPix);
    //baseLabelTsodView->
    baseLabelTsodView->hide();

    viewTitleTosdLable = new QLabel;
    viewTitleTosdLable->setParent(baseLabelTsodView);
    viewTitleTosdLable->setFont(font);
    viewTitleTosdLable->setGeometry(80,10,90,20);
    viewTitleTosdLable->setText("Иформация");
    viewTitleTosdLable->show();

    viewTsodTypeLable = new QLabel;
    viewTsodTypeLable->setParent(baseLabelTsodView);
    viewTsodTypeLable->setFont(font1);
    viewTsodTypeLable->setGeometry(10,30,80,20);
    viewTsodTypeLable->setText("Тип");
    viewTsodTypeLable->show();

    viewTsodNameLable = new QLabel;
    viewTsodNameLable->setParent(baseLabelTsodView);
    viewTsodNameLable->setFont(font1);
    viewTsodNameLable->setGeometry(10,50,80,20);
    viewTsodNameLable->setText("Название");
    viewTsodNameLable->show();

    viewTsodVisibilityLable = new QLabel;
    viewTsodVisibilityLable->setParent(baseLabelTsodView);
    viewTsodVisibilityLable->setFont(font1);
    viewTsodVisibilityLable->setGeometry(10,70,80,20);
    viewTsodVisibilityLable->setText("Видимость");
    viewTsodVisibilityLable->show();

    viewTsodConditionLable = new QLabel;
    viewTsodConditionLable->setParent(baseLabelTsodView);
    viewTsodConditionLable->setFont(font1);
    viewTsodConditionLable->setGeometry(10,90,80,20);
    viewTsodConditionLable->setText("Состояние");
    viewTsodConditionLable->show();

    viewTsodAttributeLable = new QLabel;
    viewTsodAttributeLable->setParent(baseLabelTsodView);
    viewTsodAttributeLable->setFont(font1);
    viewTsodAttributeLable->setGeometry(10,110,80,20);
    viewTsodAttributeLable->setText("Атрибут");
    viewTsodAttributeLable->show();


    viewTsodTypeInfLable = new QLabel;
    viewTsodTypeInfLable->setParent(baseLabelTsodView);
    viewTsodTypeInfLable->setFont(font1);
    viewTsodTypeInfLable->setGeometry(100,30,150,20);
    viewTsodTypeInfLable->setText("tmp");
    viewTsodTypeInfLable->show();

    viewTsodNameInfLable = new QLabel;
    viewTsodNameInfLable->setParent(baseLabelTsodView);
    viewTsodNameInfLable->setFont(font1);
    viewTsodNameInfLable->setGeometry(100,50,150,20);
    viewTsodNameInfLable->setText("tmp");
    viewTsodNameInfLable->show();

    viewTsodVisibilityInfLable = new QLabel;
    viewTsodVisibilityInfLable->setParent(baseLabelTsodView);
    viewTsodVisibilityInfLable->setFont(font1);
    viewTsodVisibilityInfLable->setGeometry(100,70,150,20);
    viewTsodVisibilityInfLable->setText("tmp");
    viewTsodVisibilityInfLable->show();

    viewTsodConditionInfLable = new QLabel;
    viewTsodConditionInfLable->setParent(baseLabelTsodView);
    viewTsodConditionInfLable->setFont(font1);
    viewTsodConditionInfLable->setGeometry(100,90,150,20);
    viewTsodConditionInfLable->setText("tmp");
    viewTsodConditionInfLable->show();

    viewTsodAttributeInfLable = new QLabel;
    viewTsodAttributeInfLable->setParent(baseLabelTsodView);
    viewTsodAttributeInfLable->setFont(font1);
    viewTsodAttributeInfLable->setGeometry(100,110,150,20);
    viewTsodAttributeInfLable->setText("tmp");
    viewTsodAttributeInfLable->show();


    editTsodVisibilty = new QLineEdit;
    editTsodVisibilty->setParent(baseLabelTsodView);
    //editTsodVisibilty->setFrameStyle(1);
    editTsodVisibilty->setFont(font1);
    editTsodVisibilty->setGeometry(100,70,150,20);
    //editTsodVisibilty->setText("tmp");
    editTsodVisibilty->hide();


    query->exec("select null union select condition_name from conditions");
    model =new QSqlQueryModel();
    model->setQuery(*query);

    editTsodCondition = new QComboBox;
    editTsodCondition->setFont(font1);
    editTsodCondition->setParent(baseLabelTsodView);
    editTsodCondition->setGeometry(100,90,150,20);
    editTsodCondition->setModel(model);
    editTsodCondition->hide();

    editTsodAttribute = new QLineEdit;
    editTsodAttribute->setParent(baseLabelTsodView);
    //editTsodAttribute->setFrameStyle(1);
    editTsodAttribute->setFont(font1);
    editTsodAttribute->setGeometry(100,110,150,20);
    //editTsodAttribute->setText("tmp");
    editTsodAttribute->hide();

    cancelViewTods = new QPushButton;
    cancelViewTods->setParent(baseLabelTsodView);
    cancelViewTods->setText("Отмена");
    cancelViewTods->setGeometry(190,200,60,30);
    connect(cancelViewTods, SIGNAL (clicked()), this, SLOT(closeInfMenu()));
    cancelViewTods->show();

    //*editTsod;
    editTsod = new QPushButton;
    editTsod->setParent(baseLabelTsodView);
    editTsod->setText("Редактировать");
    editTsod->setGeometry(10,130,100,25);
    connect(editTsod, SIGNAL (clicked()), this, SLOT(showHistoryTsod()));
    editTsod->show();
    //*dellTsod;
    dellTsod = new QPushButton;
    dellTsod->setParent(baseLabelTsodView);
    dellTsod->setText("Удалить");
    dellTsod->setGeometry(10,160,100,25);
    connect(dellTsod, SIGNAL (clicked()), this, SLOT(showHistoryTsod()));
    dellTsod->show();


    viewHistoryTsod = new QPushButton;
    viewHistoryTsod->setParent(baseLabelTsodView);
    viewHistoryTsod->setText("История работ");
    viewHistoryTsod->setGeometry(115,130,135,25);
    connect(viewHistoryTsod, SIGNAL (clicked()), this, SLOT(showHistoryTsod()));
    viewHistoryTsod->show();

    addHistoryTsod = new QPushButton;
    addHistoryTsod->setParent(baseLabelTsodView);
    addHistoryTsod->setText("Добавть запись");
    addHistoryTsod->setGeometry(115,160,135,25);
    connect(addHistoryTsod, SIGNAL (clicked()), this, SLOT(showAddTask()));
    addHistoryTsod->show();


    //add record to history
    historyLabel = new QLabel;
    //historyLabel->setParent(addToHistoryScrol);
    historyLabel->setGeometry(0,0,340,300);
    historyLabel->show();

    addToHistoryScrol = new QScrollArea;
    addToHistoryScrol->setParent(this);
    addToHistoryScrol->setGeometry(20,30,340,500);
    addToHistoryScrol->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    addToHistoryScrol->setWidget(historyLabel);
    addToHistoryScrol->hide();

    addToHistoryTitleLabel = new QLabel;
    addToHistoryTitleLabel->setParent(historyLabel);
    addToHistoryTitleLabel->setText("Добавление записи");
    addToHistoryTitleLabel->setFont(font);
    addToHistoryTitleLabel->setGeometry(60,20,250,20);
    addToHistoryTitleLabel->show();

    timeLabel = new QLabel;
    timeLabel->setParent(historyLabel);
    timeLabel->setText("Время");
    timeLabel->setFont(font1);
    timeLabel->setGeometry(10,50,120,20);
    //timeLabel->setFrameStyle(1);
    timeLabel->show();

    dateLabel = new QLabel;
    dateLabel->setParent(historyLabel);
    dateLabel->setText("Дата");
    dateLabel->setFont(font1);
    dateLabel->setGeometry(10,80,120,20);
   // dateLabel->setFrameStyle(1);
    dateLabel->show();

    timeEdit = new QTimeEdit;
    timeEdit->setParent(historyLabel);
    //timeEdit->setText("date");
    timeEdit->setFont(font1);
    timeEdit->setGeometry(130,50,190,20);
    //timeEdit->setFrameStyle(1);
    timeEdit->setDisplayFormat("hh:mm");
    timeEdit->show();

    dateEdit = new QDateEdit;
    dateEdit->setParent(historyLabel);
    //timeEdit->setText("date");
    dateEdit->setFont(font1);
    dateEdit->setGeometry(130,80,190,20);
    //timeEdit->setFrameStyle(1);
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->show();

    yTask = 110;
    QLabel *tmpL = new QLabel;
    selectTypeTaskLabel.append(tmpL);
    selectTypeTaskLabel.last()->setParent(historyLabel);
    selectTypeTaskLabel.last()->setText("Тип работы");
    selectTypeTaskLabel.last()->setFont(font1);
    selectTypeTaskLabel.last()->setGeometry(10,110,120,20);
    //selectTypeTaskLabel.last()->setFrameStyle(1);
    selectTypeTaskLabel.last()->show();

    tmpL = new QLabel;
    selectTaskLabel.append(tmpL);
    selectTaskLabel.last()->setParent(historyLabel);
    selectTaskLabel.last()->setText("Название работы");
    selectTaskLabel.last()->setFont(font1);
    selectTaskLabel.last()->setGeometry(10,140,120,20);
    //selectTaskLabel.last()->setFrameStyle(1);
    selectTaskLabel.last()->show();

    //QVector<QComboBox> *selectTypeTaskCombo;
    QComboBox *tmpC = new QComboBox;
    selectTypeTaskCombo.append(tmpC);
    query->exec("select null union select task_group_name from group_of_tasks");
    model =new QSqlQueryModel();
    model->setQuery(*query);

    selectTypeTaskCombo.last()->setFont(font1);
    selectTypeTaskCombo.last()->setParent(historyLabel);
    selectTypeTaskCombo.last()->setGeometry(130,110,190,20);
    selectTypeTaskCombo.last()->setModel(model);
    selectTypeTaskCombo.last()->show();

    //QVector<QComboBox> *selectTaskCombo;
    tmpC = new QComboBox;
    selectTaskCombo.append(tmpC);
    query->exec("select null union select task_name from tasks");
    model =new QSqlQueryModel();
    model->setQuery(*query);

    selectTaskCombo.last()->setFont(font1);
    selectTaskCombo.last()->setParent(historyLabel);
    selectTaskCombo.last()->setGeometry(130,140,190,20);
    selectTaskCombo.last()->setModel(model);
    selectTaskCombo.last()->show();

    addNewTask = new QPushButton;
    addNewTask->setParent(historyLabel);
    addNewTask->setText("Добавить");
    addNewTask->setFont(font1);
    addNewTask->setGeometry(130,170,190,20);
    connect(addNewTask, SIGNAL (clicked()), this, SLOT(addTaskField()));
    //addNewTask->setFrameStyle(1);
    addNewTask->show();

    saveRecord = new QPushButton;
    saveRecord->setParent(historyLabel);
    saveRecord->setText("Сохранить");
    saveRecord->setFont(font1);
    saveRecord->setGeometry(130,220,80,30);
    connect(saveRecord, SIGNAL (clicked()), this, SLOT(addTaskRecord()));
    //addNewTask->setFrameStyle(1);
    saveRecord->show();

    canceladdTask = new QPushButton;
    canceladdTask->setParent(historyLabel);
    canceladdTask->setText("Отмена");
    canceladdTask->setFont(font1);
    canceladdTask->setGeometry(240,220,80,30);
    connect(canceladdTask, SIGNAL (clicked()), this, SLOT(closeAddTask()));
    //addNewTask->setFrameStyle(1);
    canceladdTask->show();

     historyTable = new QTableView;
     historyTable->setParent(this);
     historyTable->setGeometry(20,40,this->size().width()-40,this->size().height()-80);
     historyTable->hide();
}

void MapWindow::addPoint()
{
    addCondition = 1;
    updateActions();
}

void MapWindow::addTsod()
{
    addCondition = 0;
    menuFlag = 1;
    updateActions();
    points.clear();
    scrollArea->resize(this->size().width()-300, this->size().height()-20);
    addTsodTitle->show();

    addTsodType->show();

    addTsodName->show();

    addTsodVisibility->show();

    addTsodCondition->show();

    addTsodAttribute->show();

    addTsodTypeComboBox->show();

    addTsodNameComboBox->show();

    addTsodConditionsComboBox->show();

    addTsodVisibilityLineEdit->show();

    addTsodAttributeLineEdit->show();

    addTsodButton->show();

    cancelTsodButton->show();
}

void MapWindow::comboTypesel(int t)
{
    query->prepare("select null,-1 union select tsod_name, id from tsod_name where type_id = :id");
    query->bindValue(":id", t);
    query->exec();
    addTsodNameComboBox->clear();
    type_id_list.clear();
    while(query->next())
    {
        type_id_list.push_back(query->value(1).toInt());
        addTsodNameComboBox->addItem(query->value(0).toString());
    }
}

void MapWindow::addTsodM()
{
    if (addTsodVisibilityLineEdit->text().toDouble() > 1 || addTsodVisibilityLineEdit->text().toDouble() < 0)
    {
        QMessageBox::warning(this, tr(""),tr("Значение видимость должно быть меньше единицы и больше нуля"));
    }
    else
    {
        qDebug() << addTsodNameComboBox->currentIndex();
        qDebug() << type_id_list[addTsodNameComboBox->currentIndex()];

        query->prepare("insert into tsod(name_id, visibility, condition_id, attribute) values (:id, :vis, :con, :att)");
        query->bindValue(":id", type_id_list[addTsodNameComboBox->currentIndex()]);
        query->bindValue(":vis", addTsodVisibilityLineEdit->text().toDouble());
        query->bindValue(":con", addTsodConditionsComboBox->currentIndex());
        query->bindValue(":att", addTsodAttributeLineEdit->text().toDouble());
        query->exec();


        int id = query->lastInsertId().toInt();

        for(int i = 0; i < points.size(); ++i)
        {
            query->prepare("insert into tsod_coordinates(tsod_id, latitude, longitude) values (:id, :la, :lo)");
            query->bindValue(":id", id);
            double x = 47.00694444 + points[i].first * (49.04500000 - 47.00694444) / 8209.0;
            double y = 60.34805556 + points[i].second * (59.09361111 - 60.34805556) / 6377.0;
            query->bindValue(":la", y);
            query->bindValue(":lo", x);
            query->exec();
        }

        cencelAdd();
    }
}

void MapWindow::cencelAdd()
{
    addTsodTitle->hide();
    addTsodType->hide();
    addTsodName->hide();
    addTsodVisibility->hide();
    addTsodCondition->hide();
    addTsodAttribute->hide();

    addTsodTypeComboBox->setCurrentIndex(0);
    addTsodTypeComboBox->hide();
    addTsodNameComboBox->setCurrentIndex(0);
    addTsodNameComboBox->hide();
    addTsodConditionsComboBox->setCurrentIndex(0);
    addTsodConditionsComboBox->hide();

    addTsodVisibilityLineEdit->clear();
    addTsodVisibilityLineEdit->hide();
    addTsodAttributeLineEdit->clear();
    addTsodAttributeLineEdit->hide();

    addTsodButton->hide();
    cancelTsodButton->hide();

    scrollArea->show();
    addCondition = 0;


    points.clear();

    for(int i = 0; i < displayTmpElements.size(); ++i)
    {
        delete displayTmpElements[i];
        displayTmpElements[i] = nullptr;
    }
    displayTmpElements.clear();
    menuFlag = 0;
    updateActions();
    scrollArea->resize(this->size().width(),this->height()-20);
    points.clear();
    updateActions();
    resImg=1;
    menuFlag = 0;
    paint();
    x_p=-1;
    y_p=-1;
    x1=-1;
    y1=-1;
    scrollArea->resize(this->size().width(),this->height()-20);
}

void MapWindow::closeInfMenu()
{
    baseLabelTsodView->hide();
}

void MapWindow::addTaskField()
{
    yTask +=60;
    QFont font1( "Arial", 10);
    historyLabel->resize(historyLabel->size().width(), historyLabel->size().height()+60);

    QLabel *tmpL = new QLabel;
    selectTypeTaskLabel.append(tmpL);
    selectTypeTaskLabel.last()->setParent(historyLabel);
    selectTypeTaskLabel.last()->setText("Тип работы");
    selectTypeTaskLabel.last()->setFont(font1);
    selectTypeTaskLabel.last()->setGeometry(10,yTask,120,20);
    selectTypeTaskLabel.last()->show();

    tmpL = new QLabel;
    selectTaskLabel.append(tmpL);
    selectTaskLabel.last()->setParent(historyLabel);
    selectTaskLabel.last()->setText("Название работы");
    selectTaskLabel.last()->setFont(font1);
    selectTaskLabel.last()->setGeometry(10,yTask+30,120,20);
    selectTaskLabel.last()->show();

    QComboBox *tmpC = new QComboBox;
    selectTypeTaskCombo.append(tmpC);
    query->exec("select null union select task_group_name from group_of_tasks");
    QSqlQueryModel *model =new QSqlQueryModel();
    model->setQuery(*query);

    selectTypeTaskCombo.last()->setFont(font1);
    selectTypeTaskCombo.last()->setParent(historyLabel);
    selectTypeTaskCombo.last()->setGeometry(130,yTask,190,20);
    selectTypeTaskCombo.last()->setModel(model);
    selectTypeTaskCombo.last()->show();

    //QVector<QComboBox> *selectTaskCombo;
    tmpC = new QComboBox;
    selectTaskCombo.append(tmpC);
    query->exec("select null union select task_name from tasks");
    model =new QSqlQueryModel();
    model->setQuery(*query);

    selectTaskCombo.last()->setFont(font1);
    selectTaskCombo.last()->setParent(historyLabel);
    selectTaskCombo.last()->setGeometry(130,yTask+30,190,20);
    selectTaskCombo.last()->setModel(model);
    selectTaskCombo.last()->show();

    addNewTask->move(130,yTask+60);

    saveRecord->move(130,yTask+110);

    canceladdTask->move(240, yTask+110);
}

void MapWindow::showAddTask()
{
    addToHistoryScrol->setGeometry(baseLabelTsodView->pos().x(),30,340,500);
    addToHistoryScrol->show();
}

void MapWindow::closeAddTask()
{
    addToHistoryScrol->hide();

    selectTypeTaskLabel.erase(selectTypeTaskLabel.begin()+1,selectTypeTaskLabel.end());
    selectTaskLabel.erase(selectTaskLabel.begin()+1,selectTaskLabel.end());
    selectTypeTaskCombo.erase(selectTypeTaskCombo.begin()+1,selectTypeTaskCombo.end());
    selectTaskCombo.erase(selectTaskCombo.begin()+1,selectTaskCombo.end());
    yTask = 110;
    selectTypeTaskCombo.front()->setCurrentIndex(0);
    selectTaskCombo.front()->setCurrentIndex(0);
}

void MapWindow::addTaskRecord()
{

    query->prepare("insert into task_history(tsod_id, date_, time_) values (:id,STR_TO_DATE(':date','%d/%m/%Y'),':time')");
    query->bindValue(":id",id_clicked);
    query->bindValue(":date", dateEdit->text());
    query->bindValue(":time", timeEdit->text());
    query->exec();
    qDebug() <<query->lastQuery();
    int last_id = query->lastInsertId().toInt();

    //qDebug() << "======";
    for(int i=0; i < selectTaskCombo.size(); ++i)
    {
        qDebug() <<selectTaskCombo[i]->currentIndex();
        query->prepare("insert into completed_task(record_id,task_id) values (:id_rec,:id_task);");
        query->bindValue(":id_rec",last_id);
        query->bindValue(":id_task",selectTaskCombo[i]->currentIndex());
        query->exec();
    }
    closeAddTask();
}

void MapWindow::showHistory()
{

    cencelAdd();
    scrollArea->hide();

    query->exec("select h.tsod_id, tn.tsod_name, tt.type_name, GROUP_CONCAT(t.task_name), date_, time_ from task_history h, tsod ts, tsod_name tn, tsod_type tt, completed_task c, tasks t where h.id = c.record_id and t.id = c.task_id  and ts.id = h.tsod_id and ts.name_id = tn.id and tn.type_id = tt.id group by h.id");
    QSqlQueryModel *model= new QSqlQueryModel;
    model->setQuery(*query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Назавние ТСОД"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Тип ТСОД"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Произведеные работы"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Дата"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Время"));
    historyTable->setModel(model);

    historyTable->setColumnWidth(1,170);
    historyTable->setColumnWidth(2,170);
    historyTable->setColumnWidth(3,340);
    historyTable->setColumnWidth(4,170);
    historyTable->setColumnWidth(5,170);
    historyTable->verticalHeader()->setVisible(false);
    historyTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    historyTable->show();


}

void MapWindow::showSchedule() {
    query->exec("select tasks.id, tasks.task_name, tasks.lenth, tasks.resources, tasks.efficiency, tasks.period, tsod.id,  tsod_name.tsod_name from tsod, tsod_name, tasks where tsod_name.type_id = tasks.task_group_id;");
    while(query->next())
    {

        idTask.push_back(query->value(0).toInt());
        namesTask.push_back(query->value(1).toString());
        lenth.push_back(query->value(2).toInt());
        resources.push_back(query->value(3).toInt());
        efficiency.push_back(query->value(4).toInt());
        peroid.push_back(query->value(5).toInt());
        idTsod.push_back(query->value(6).toInt());
        nameTsod.push_back(query->value(7).toString());
    }

    schedule = new Schedule(this, idTask, namesTask, lenth, resources, efficiency, peroid, idTsod, nameTsod, &dataBase);

    schedule->resize(this->width(),this->height());
    schedule->show();
}

void MapWindow::showHistoryTsod()
{

    cencelAdd();
    scrollArea->hide();

    query->prepare("select h.tsod_id, tn.tsod_name, tt.type_name, GROUP_CONCAT(t.task_name), date_, time_ from task_history h, tsod ts, tsod_name tn, tsod_type tt, completed_task c, tasks t where h.id = c.record_id and t.id = c.task_id  and ts.id = h.tsod_id and ts.name_id = tn.id and tn.type_id = tt.id and ts.id = :id group by h.id");
    query->bindValue(":id",id_clicked);
    query->exec();
    QSqlQueryModel *model= new QSqlQueryModel;
    model->setQuery(*query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Назавние ТСОД"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Тип ТСОД"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Произведеные работы"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Дата"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Время"));
    historyTable->setModel(model);

    historyTable->setColumnWidth(1,170);
    historyTable->setColumnWidth(2,170);
    historyTable->setColumnWidth(3,340);
    historyTable->setColumnWidth(4,170);
    historyTable->setColumnWidth(5,170);
    historyTable->verticalHeader()->setVisible(false);
    historyTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    historyTable->show();
}

void MapWindow::showMainMenu()
{
    historyTable->hide();
    scrollArea->show();
}

//рисуем
void MapWindow::paint()
{
    QImage tmp(imageLabel->pixmap()->toImage());
    QPainter painter(&tmp);
    QPen paintpen;


    displayElements.clear();
    displayElementsSize.clear();

    if(resImg)
    {
        mapPix.load(":/Img/Map.jpg");
        imageLabel->setPixmap(mapPix);
        resImg = 0;
    }

    if(menuFlag)
    {

    if (x_p>0)
    {
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawLine(x_p, y_p, x1, y1);
    }
    painter.setPen(QPen(Qt::gray));
    painter.setBrush(QBrush(Qt::gray));
    painter.drawEllipse(QRect(x1-5,y1-5,10,10));
    imageLabel->setPixmap(QPixmap::fromImage(tmp));
    }
    else
    {
        int prev_id = 0;
        int x_p1 = -1;
        int y_p1 = -1;
        query->exec("select t.id, tt.id, longitude, latitude from tsod t, tsod_coordinates tc, tsod_type tt, tsod_name tn where t.id = tc.tsod_id and t.name_id = tn.id and tn.type_id = tt.id");
        while(query->next())
        {
            int x2 = int(8209 * (query->value(2).toDouble() - 47.00694444)/(49.04500000 - 47.00694444));
            int y2 = int(6377 * (query->value(3).toDouble() - 60.34805556)/(59.09361111 - 60.34805556));
            if(prev_id == query->value(0).toInt())
            {
                painter.setPen(QPen(Qt::gray, 1));
                painter.drawLine(x_p1, y_p1, x2, y2);
            }
            if(query->value(1).toInt() == 1)
            {
                painter.setPen(QPen(Qt::gray));
                painter.setBrush(QBrush(Qt::gray));
            }
            else if(query->value(1).toInt() == 2)
            {
                painter.setPen(QPen(Qt::red));
                painter.setBrush(QBrush(Qt::red));
                qDebug() << 2;
            }
            else if(query->value(1).toInt() == 3)
            {
                painter.setPen(QPen(Qt::green));
                painter.setBrush(QBrush(Qt::green));
                qDebug() << 3;
            }
            painter.drawEllipse(QRect(x2-3,y2-3,6,6));
            imageLabel->setPixmap(QPixmap::fromImage(tmp));

            ClickableLabel *tmp = new ClickableLabel();
            displayElements.append(tmp);
            displayElements.last()->setParent(imageLabel);
            displayElements.last()->setGeometry(QRect(x2-5,y2-5,int(6 * scaleFactor),int(6 * scaleFactor)));
            displayElementsSize.push_back(std::make_pair(6 * scaleFactor,6 * scaleFactor));
            displayElements.last()->setText("");
            //displayElements.last()->setFlat(true);
            //displayElements.last()->setFrameStyle(1);

            connect(displayElements.last(),SIGNAL (clicked()), &mapper, SLOT (map()));
            mapper.setMapping(displayElements.last(), query->value(0).toInt());
            displayElements.last()->show();

            prev_id = query->value(0).toInt();
            x_p1 = x2;
            y_p1 = y2;
        }
    }
}


void MapWindow::clickOnLabel(int t)
{

    id_clicked = t;
    if(menuFlag)
        return;
    int x = this->mapFromGlobal(QCursor::pos()).x() + 10;
    int y = this->mapFromGlobal(QCursor::pos()).y() - 10;
    if (x<0)
    {
        x=0;
    }
    else if (x > this->size().width() - 300)
    {
        x = this->size().width() - 300;
    }

    if (y<0)
    {
        y=20;
    }
    else if (y > this->size().height() - 240)
    {
        y = this->size().height() - 240;
    }

    query->prepare("select tt.type_name, tn.tsod_name, t.visibility, c.condition_name, t.attribute from tsod t, tsod_name tn, tsod_type tt, conditions c where c.id = t.condition_id and t.name_id = tn.id and tn.type_id = tt.id and t.id = :id;");
    query->bindValue(":id", t);
    query->exec();
    query->next();

    viewTsodTypeInfLable->setText(query->value(0).toString());
    viewTsodNameInfLable->setText(query->value(1).toString());
    viewTsodVisibilityInfLable->setText(query->value(2).toString());
    viewTsodConditionInfLable->setText(query->value(3).toString());
    viewTsodAttributeInfLable->setText(query->value(4).toString());


    baseLabelTsodView->move(x,y);
    baseLabelTsodView->show();

}

void MapWindow::mousePressEvent(QMouseEvent *e)
{
    x=0;
    y=0;
    mFirstClick=true;
    mPaintFlag=false;

    if(menuFlag)
    {
    if(e->button() == Qt::LeftButton)
    {


        x = scrollArea->horizontalScrollBar()->value() + e->x();
        y = scrollArea->verticalScrollBar()->value() + e->y()-21;

        x_p = x1;
        y_p = y1;
        x1 = scrollArea->horizontalScrollBar()->value() + int((e->x())/scaleFactor);
        y1 = scrollArea->verticalScrollBar()->value() + int((e->y()-20)/scaleFactor);


        points.push_back(std::make_pair(x,y));

        mFirstClick = false;
        mPaintFlag = true;



        paint();

    }
    }
}

void MapWindow::zoomIn()
{
    scaleImage(1.25);
    for(int i = 0; i < displayElements.size(); ++i)
    {
        QPoint position = displayElements[i]->pos();

        position.setX(int(position.x() * 1.25));
        position.setY(int(position.y() * 1.25));

        displayElements[i]->move(position.x(),position.y());

        QSize size = displayElements[i]->size();

        size.setWidth(int(size.width()*1.25));
        size.setHeight(int(size.height()*1.25));

        displayElementsSize[i].first *=1.27;
        displayElementsSize[i].second *=1.27;

        displayElements[i]->resize(int(displayElementsSize[i].first),int(displayElementsSize[i].second));
    }
}

void MapWindow::zoomOut()
{
    scaleImage(0.8);
    for(int i = 0; i < displayElements.size(); ++i)
    {
        QPoint position = displayElements[i]->pos();

        position.setX(int(position.x() * 0.8));
        position.setY(int(position.y() * 0.8));

        displayElements[i]->move(position.x(),position.y());


        QSize size = displayElements[i]->size();

        size.setWidth(int(size.width()*0.8));
        size.setHeight(int(size.height()*0.8));

        displayElementsSize[i].first *=0.9;
        displayElementsSize[i].second *=0.9;

        displayElements[i]->resize(int(displayElementsSize[i].first),int(displayElementsSize[i].second));
    }
}

void MapWindow::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void MapWindow::fitToWindow()
{
    bool fitToWindow = false; //fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
    {
        normalSize();
    }
    updateActions();
}

void MapWindow::createActions()
{


    QMenu *viewMenu= menuBar()->addMenu(tr("Данные"));
    QAction *mainMenu = viewMenu->addAction(tr("Главное меню"), this, &MapWindow::showMainMenu);
    QAction *history = viewMenu->addAction(tr("История работ"), this, &MapWindow::showHistory);
    QAction *schedule = viewMenu->addAction(tr("Регламент обслуживания ТСОД"), this, &MapWindow::showSchedule);


    QMenu *insertMenu = menuBar()->addMenu(tr("Добавить"));

    addTsodAct = insertMenu->addAction(tr("Добавить ТСОД"), this, &MapWindow::addTsod);
    cencelAddAct = insertMenu->addAction(tr("Отмена"), this, &MapWindow::cencelAdd);

    cencelAddAct->setEnabled(false);



}

void MapWindow::updateActions()
{

    if(menuFlag)
    {
        addTsodAct->setEnabled(false);
        cencelAddAct->setEnabled(true);
    }
    else
    {
        addTsodAct->setEnabled(true);
        cencelAddAct->setEnabled(false);
    }
}

void MapWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
}

void MapWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar-> value() + (factor-1)* scrollBar->pageStep()/2));
}

void MapWindow::resizeEvent(QResizeEvent* event)
{
        QMainWindow::resizeEvent(event);

        QSize sc = this->size();
        if(!menuFlag)
        {
            sc.setHeight(sc.height()-20);
            sc.setWidth(sc.width());
            scrollArea->resize(sc);
        }
        else if (menuFlag == 1)
        {
            sc.setHeight(sc.height()-20);
            sc.setWidth(sc.width()-300);
            scrollArea->resize(sc);
        }
        zoomInButton->move(scrollArea->size().width() - 60, 20);
        zoomOutButton->move(scrollArea->size().width() - 60, 60);

        addTsodTitle->move(this->size().width()-240,40);

        addTsodType->move(this->size().width()-240,100);

        addTsodName->move(this->size().width()-270,130);

        addTsodVisibility->move(this->size().width()-270,160);

        addTsodCondition->move(this->size().width()-270,190);

        addTsodAttribute->move(this->size().width()-270,220);

        addTsodTypeComboBox->move(this->size().width()-170,105);

        addTsodNameComboBox->move(this->size().width()-170,135);

        addTsodConditionsComboBox->move(this->size().width()-170,195);

        addTsodVisibilityLineEdit->move(this->size().width()-170,165);

        addTsodAttributeLineEdit->move(this->size().width()-170,225);

        addTsodButton->move(this->size().width()-170,260);

        cancelTsodButton->move(this->size().width()-80,260);
}

MapWindow::~MapWindow()
{
    delete imageLabel;
    delete scrollArea;
}

