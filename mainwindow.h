#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "json.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readJSON();

    void readJSONToDefinationMask(Json::Value *rp);
    void readJSONToDefination(Json::Value *rp);
    void addKeysMember(Json::Value *rp, bool opt);
    void writeToFile(Json::Value *rp);
    void updateTemplate(QString *qfopath);
    void blockUpdate(bool isChecked);

    void readJSONRootNamesMask(Json::Value *rp);
    void readJSONRootNames(Json::Value *rp, int layer, QString preLayer);

    void on_actionOpenOutputFile_triggered();

    void on_actionOpenInputFile_triggered();

private:
    Ui::MainWindow *ui;

    //std::vector<QString> namelist;
    std::vector<std::vector<QString>> namelistlayer;

    struct Defination{
//        QString name;
        unsigned int vendorProductId;
        float width;
        float height;
//        unsigned int vendorProductId;
//        QString vendorId;
//        QString productId;
    }keyboardDefintion;

    struct KeyDefintion{
        unsigned int row;
        unsigned int col;
        QString optrow;
        QString optcol;
        float x;
        float y;
        unsigned int r;
        float rx;
        float ry;
        float h;
        float w;
        QString color;
        bool d;//开启或关闭
    } keyDefintion;
    bool iskeyWReDefintion = false;
    bool iskeyHReDefintion = false;
    float addXNext;
};
#endif // MAINWINDOW_H
