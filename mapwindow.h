#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QVector>
#include <QPushButton>
#include <QLineEdit>
#include <QSignalMapper>
#include <ClickableLabel.h>
#include <QSqlQuery>
#include <QTableView>
#include <QSqlQueryModel>
#include <QComboBox>
#include <QSqlTableModel>
#include <QTimeEdit>
#include <QDateEdit>
#include <schedule.h>
#include <vector>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

namespace Ui {
class MapWindow;
}

class MapWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapWindow(QWidget *parent = nullptr, QSqlDatabase *db = nullptr);
    ~MapWindow();

    //virtual void paintEvent(QPaintEvent *event);

private slots:

    void addPoint();
    void addTsod();
    void cencelAdd();
    void showHistory();
    void showSchedule();
    void showHistoryTsod();
    void closeInfMenu();
    void showMainMenu();

    void addTsodM();

    void comboTypesel(int t);

    void showAddTask();
    void addTaskRecord();
    void addTaskField();
    void closeAddTask();

    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void clickOnLabel(int t);

private:
    Ui::MapWindow *ui;


    void paint();
    void mousePressEvent(QMouseEvent *e);
    void createActions();
    void createMenus();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void resizeEvent(QResizeEvent* event);

    QImage image;
    QLabel *imageLabel;
    QVector<ClickableLabel*> displayElements;
    QVector<std::pair<double,double>> displayElementsSize;
    QVector<ClickableLabel*> displayTmpElements;
    QScrollArea *scrollArea;
    double scaleFactor;

    QAction *addPointAct;
    QAction *addTsodAct;
    QAction *cencelAddAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QPixmap mapPix;

    int x;
    int y;
    int x1;
    int y1;
    int x_p=-1;
    int y_p=-1;
    bool mFirstClick;
    bool mPaintFlag;
    int resImg = 0;
    QVector<int> type_id_list;
    int id_clicked;
    int yTask;

    int menuFlag = 0;
    int pointsCount = 0;
    int addCondition = 0;
    QVector<std::pair<int,int>> points;

    QSignalMapper mapper;

    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;

    //add tsod
    QLabel *addTsodTitle;
    QLabel *addTsodType;
    QLabel *addTsodName;
    QLabel *addTsodVisibility;
    QLabel *addTsodCondition;
    QLabel *addTsodAttribute;

    QComboBox *addTsodTypeComboBox;
    QComboBox *addTsodNameComboBox;
    QComboBox *addTsodConditionsComboBox;

    QLineEdit *addTsodVisibilityLineEdit;
    QLineEdit *addTsodAttributeLineEdit;

    QPushButton *addTsodButton;
    QPushButton *cancelTsodButton;

    //tsod info
    QLabel *baseLabelTsodView = nullptr;
    QLabel *viewTitleTosdLable;
    QLabel *viewTsodTypeLable;
    QLabel *viewTsodNameLable;
    QLabel *viewTsodVisibilityLable;
    QLabel *viewTsodConditionLable;
    QLabel *viewTsodAttributeLable;

    QLabel *viewTsodTypeInfLable;
    QLabel *viewTsodNameInfLable;
    QLabel *viewTsodVisibilityInfLable;
    QLabel *viewTsodConditionInfLable;
    QLabel *viewTsodAttributeInfLable;

    QLineEdit *editTsodVisibilty;
    QComboBox *editTsodCondition;
    QLineEdit *editTsodAttribute;

    QPushButton *cancelViewTods;
    QPushButton *editTsod;
    QPushButton *dellTsod;
    QPushButton *viewHistoryTsod;
    QPushButton *addHistoryTsod;

    //add record to history

    QScrollArea *addToHistoryScrol;
    QLabel *historyLabel;
    QLabel *addToHistoryTitleLabel;
    QLabel *timeLabel;
    QLabel *dateLabel;

    QTimeEdit *timeEdit;
    QDateEdit *dateEdit;

    QVector<QLabel*> selectTypeTaskLabel;
    QVector<QLabel*> selectTaskLabel;

    QVector<QComboBox*> selectTypeTaskCombo;
    QVector<QComboBox*> selectTaskCombo;

    QPushButton *addNewTask;

    QPushButton *saveRecord;
    QPushButton *canceladdTask;

    QTableView *historyTable;

    Schedule *schedule;

    //QSqlQuery *query;



    QSqlQuery *query;

    QSqlDatabase dataBase;

    std::vector<int> idTask;
    QVector<QString> namesTask;
    std::vector<int> lenth;
    std::vector<int> resources;
    std::vector<int> efficiency;
    std::vector<int> peroid;
    std::vector<int> idTsod;
    QVector<QString> nameTsod;

};

#endif // MAPWINDOW_H
