#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <QTextStream>

#include <iostream>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButton_3, SIGNAL(released()), this, SLOT(readJSON()));
    QString filePath = QDir::homePath() + "/AppData/Roaming/VIA/config.json";
    QFile file(filePath);
    if(file.exists())
    {
        ui->comboBox_2->addItem(filePath);
    }
    else
    {
        qDebug() << "默认config.json不存在,请手动选择";
        ui->textBrowser->append("默认config.json不存在,请手动选择");
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::readJSON()
{
    QString qfipath = ui->comboBox->currentText();
    Json::Value root;
    Json::Reader reader;
    std::ifstream in(qfipath.toLocal8Bit(), std::ios::binary);
    if (!in.is_open())
    {
        qDebug() << "输入文件打开错误";
        ui->textBrowser->append("输入文件打开错误");
        return;
    }
    if(reader.parse(in, root))
    {
        qDebug() << "Reading File " << qfipath;
        ui->textBrowser->append("Reading File " + qfipath);
        ui->textBrowser->append("");

        readJSONToDefinationMask(&root);
        //readJSONRootNamesMask(&root);

        blockUpdate(ui->checkBox->isChecked());

        return;
    }
    else
    {
        qDebug() << "json解析失败";
        ui->textBrowser->append("json解析失败");
        return;
    }
}

void MainWindow::blockUpdate(bool isChecked)
{
    QString aFileName = "C:/Windows/System32/drivers/etc/hosts";
    QString bFileName = QDir::currentPath() + "/hosts.bak";
    QFile aFile(aFileName);
    QFile bFile(bFileName);
    if (!aFile.exists()) //文件不存在
    {
        qDebug() << "hosts不存在";
        ui->textBrowser->append("hosts不存在");
        return;
    }
    if (!aFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "hosts不可读";
        ui->textBrowser->append("hosts不可读");
        return;
    }
    QTextStream aStream(&aFile); //用文本流读取文件
    aStream.setAutoDetectUnicode(true); //自动检测Unicode,才能正常显示文档内的汉字

    if(isChecked)
    {
        while (!aStream.atEnd())
        {
            QString qstr=aStream.readLine();//读取文件的一行
            if(qstr == "127.0.0.1 www.caniusevia.com")
            {
                return;
            }
            qDebug() << qstr;
        }
        aStream << "127.0.0.1 www.caniusevia.com";
        aFile.close();//关闭文件
    }
    else
    {
        if(bFile.exists())
        {
            QFile::remove(bFileName);
        }
        QFile::copy(aFileName, bFileName);
        aFile.close();//关闭文件
        if (!aFile.open(QIODevice::Truncate | QIODevice::ReadWrite| QIODevice::Text))
        {
            qDebug() << "hosts不可读";
            ui->textBrowser->append("hosts不可读");
            return;
        }
        if (!bFile.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            qDebug() << "hosts不可读";
            ui->textBrowser->append("hosts.bak不可读");
            return;
        }
        QTextStream aStream(&aFile);
        QTextStream wStream(&bFile);
        while (!wStream.atEnd())
        {
            QString qstr=wStream.readLine();//读取文件的一行
            if(qstr == "127.0.0.1 www.caniusevia.com")
            {
                //return;
            }
            else
            {
                aStream << qstr;
                aStream << Qt::endl;
                qDebug() << qstr;
            }
        }
        aFile.close();//关闭文件
    }

    QProcess process;
    QString cmd = "ipconfig";
    QStringList arguments;
    arguments <<  "/flushdns";
    process.start(cmd,arguments);
    process.waitForStarted();
    process.waitForFinished();
//    ui->textBrowser->append(QString::fromLocal8Bit(process.readAllStandardOutput()));
}

void MainWindow::readJSONToDefinationMask(Json::Value *rp)
{
    keyDefintion.row = 0;
    keyDefintion.col = 0;
    keyDefintion.optrow = 0;
    keyDefintion.optcol = 0;
    keyDefintion.x = 0;
    keyDefintion.y = 0;
    keyDefintion.r = 0;
    keyDefintion.rx = 0;
    keyDefintion.ry = 0;
    keyDefintion.h = 1;
    keyDefintion.w = 1;
    keyDefintion.color = "alpha";
    keyDefintion.d = false;

    this->readJSONToDefination(rp);
    writeToFile(rp);
}
void MainWindow::readJSONToDefination(Json::Value *rp)
{
    keyboardDefintion.width = 0;
    keyboardDefintion.height = 0;

    std::string pid_str = (QString::fromStdString(rp->operator[]("vendorId").asString()).mid(2,5) + QString::fromStdString(rp->operator[]("productId").asString()).mid(2,5)).toStdString();
    std::stringstream stream;
    stream << std::hex << pid_str;
    stream >> keyboardDefintion.vendorProductId;
    qDebug() << keyboardDefintion.vendorProductId;

    for (int i = 0; i < rp->operator[]("layouts")["keymap"].size(); i++)
    {
        for (int j = 0; j < rp->operator[]("layouts")["keymap"][i].size(); j++)
        {
            switch (rp->operator[]("layouts")["keymap"][i][j].type())
            {
            case Json::objectValue:
                for (std::string elem : rp->operator[]("layouts")["keymap"][i][j].getMemberNames())
                {
                    QString nowValue = QString::fromStdString(rp->operator[]("layouts")["keymap"][i][j][elem].asString());
                    if(elem == "c")
                    {
                        if(nowValue == "#cccccc")
                        {
                            keyDefintion.color = "alpha";
                        }
                        else if(nowValue == "#aaaaaa")
                        {
                            keyDefintion.color = "mod";
                        }
                        else if(nowValue == "#777777")
                        {
                            keyDefintion.color = "accent";
                        }
                        else
                        {
                            keyDefintion.color = "alpha";
                        }
                        qDebug() << "设置颜色color:" << keyDefintion.color;
                    }
                    else if(elem == "w")
                    {
                        keyDefintion.w = nowValue.toFloat();
                        addXNext = keyDefintion.w - 1;
                        iskeyWReDefintion = true;
                        qDebug() << "设置宽度w:" << keyDefintion.w;
                    }
                    else if(elem == "h")
                    {
                        keyDefintion.h = nowValue.toFloat();
                        //keyDefintion.y += keyDefintion.h;
                        iskeyHReDefintion = true;
                        qDebug() << "设置高度h:" << keyDefintion.h;
                    }
                    else if(elem == "x")
                    {
                        keyDefintion.x += nowValue.toFloat();
                        qDebug() << "设置坐标x:" << keyDefintion.x;
                    }
                    else if (elem == "y")
                    {
                        keyDefintion.y += nowValue.toFloat();
                        qDebug() << "设置坐标y:" << keyDefintion.y;
                    }
                    else if(elem == "r")
                    {
                        keyDefintion.r = nowValue.toUInt();
                        qDebug() << "设置旋转角度r:" << keyDefintion.r;
                    }
                    else if(elem == "rx")
                    {
                        keyDefintion.rx = nowValue.toFloat();
                        qDebug() << "设置旋转中心rx:" << keyDefintion.rx;
                    }
                    else if(elem == "ry")
                    {
                        keyDefintion.ry = nowValue.toFloat();
                        qDebug() << "设置旋转中心ry:" << keyDefintion.ry;
                    }
                    else if(elem == "d")
                    {
                        if(nowValue.toStdString() == "true")
                        {
                            keyDefintion.d = true;
                        }
                        else
                        {
                            keyDefintion.d = false;
                        }
                        qDebug() << "是否关闭该按键" << keyDefintion.d;
                    }
                    else
                    {
                        qDebug() << "忽略的参数：" << nowValue << "@" << i << " " << j << " "<< QString::fromStdString(elem);
                    }
                    rp->operator[]("layouts")["keymap"][i][j][elem].asString();
                }
                break;
            case Json::stringValue:
                QString nowValue = QString::fromStdString(rp->operator[]("layouts")["keymap"][i][j].asString());

                if(iskeyWReDefintion)
                {
                    iskeyWReDefintion = false;
                }
                else
                {
                    keyDefintion.w = 1;
                }
                if(iskeyHReDefintion)
                {
                    iskeyHReDefintion = false;
                }
                else
                {
                    keyDefintion.h = 1;
                }

                int isoptionKeys = nowValue.indexOf("\n\n\n");
                int index = nowValue.indexOf(",");
                if(isoptionKeys == -1)
                {
                    keyDefintion.row = nowValue.mid(0,index).toUInt();
                    keyDefintion.col = nowValue.mid(index + 1,nowValue.size()).toUInt();

                    if(!keyDefintion.d)
                        addKeysMember(rp, false);
                    else
                        keyDefintion.d = false;
                }
                else
                {
                    keyDefintion.row = nowValue.mid(0,index).toUInt();
                    keyDefintion.col = nowValue.mid(index + 1,isoptionKeys).toUInt();
                    keyDefintion.optrow = nowValue.mid(isoptionKeys + 3,nowValue.size());   //可能是因为转义字符，有问题
                    index = keyDefintion.optrow.indexOf(",");
                    keyDefintion.optcol = keyDefintion.optrow.mid(index + 1,nowValue.size());
                    keyDefintion.optrow = keyDefintion.optrow.mid(0,index);
                    qDebug() << keyDefintion.optrow;

                    if(!keyDefintion.d)
                        addKeysMember(rp, true);
                    else
                        keyDefintion.d = false;
                }

                keyDefintion.x += 1;
                if(addXNext)
                {
                    keyDefintion.x += addXNext;
                    addXNext = 0;
                }
                break;
            }
        }

        keyboardDefintion.width = keyDefintion.x + keyDefintion.w - 1 > keyboardDefintion.width ? keyDefintion.x + keyDefintion.w - 1: keyboardDefintion.width;
        keyboardDefintion.height =keyDefintion.y + keyDefintion.h > keyboardDefintion.height ? keyDefintion.y + keyDefintion.h : keyboardDefintion.height;
        keyDefintion.x = 0;
        keyDefintion.y += 1;
    }
}

void MainWindow::addKeysMember(Json::Value *rp, bool opt)
{
    Json::Value root;

    root["row"] = keyDefintion.row;
    root["col"] = keyDefintion.col;
    root["x"] = keyDefintion.x;
    root["y"] = keyDefintion.y;
    root["r"] = keyDefintion.r;
    root["rx"] = keyDefintion.rx;
    root["ry"] = keyDefintion.ry;
    root["w"] = keyDefintion.w;
    root["h"] = keyDefintion.h;
    root["color"] = keyDefintion.color.toStdString();

    if(opt)
    {
        rp->operator[]("layouts")["optionKeys"][keyDefintion.optrow.toStdString()][keyDefintion.optcol.toStdString()].append(root);
    }
    else
    {
        rp->operator[]("layouts")["keys"].append(root);
    }

}

void MainWindow::writeToFile(Json::Value *rp)
{
    Json::StyledWriter style_writer;
    Json::Reader reader;
    Json::Value root;
    QString qfopath = ui->comboBox_2->currentText();
    QString tfpath;

    updateTemplate(&qfopath);

    rp->operator[]("vendorProductId") = keyboardDefintion.vendorProductId;

    rp->operator[]("layouts").removeMember("keymap");
    rp->removeMember("vendorId");
    rp->removeMember("productId");

    rp->operator[]("layouts")["width"] = keyboardDefintion.width;
    rp->operator[]("layouts")["height"] = keyboardDefintion.height;

    if(ui->radioButton->isChecked())
    {
        tfpath = QDir::currentPath() + "/template.json";
    }
    else if(ui->radioButton_2->isChecked())
    {
        tfpath = qfopath;
    }

    std::ifstream tin(tfpath.toLocal8Bit(), std::ios::binary);
    if (!tin.is_open())
    {
        qDebug() << "模板文件打开错误";
        ui->textBrowser->append("模板文件打开错误");
        return;
    }
    if(reader.parse(tin, root))
    {
        qDebug() << "Reading Template File " << tfpath;
    }
    if(!root["remoteData"]["definitions"][std::to_string(keyboardDefintion.vendorProductId)].isNull())
        root["remoteData"]["definitions"].removeMember(std::to_string(keyboardDefintion.vendorProductId));
    root["remoteData"]["definitions"][std::to_string(keyboardDefintion.vendorProductId)] = *rp;

    std::ofstream out(qfopath.toLocal8Bit(), std::ios::binary);
    if (!out.is_open())
    {
        qDebug() << "输出文件打开错误";
        ui->textBrowser->append("输出文件打开错误");
        return;
    }
    std::string str_out = style_writer.write(root);
    out << str_out;
    out.close();
    qDebug() << "输出完成";
    ui->textBrowser->append("输出完成");

}

void MainWindow::updateTemplate(QString *qfopath)
{
    QString tfpath = QDir::currentPath() + "/template.json";
    Json::Value troot;
    Json::Reader reader;
    std::ifstream tin(tfpath.toLocal8Bit(), std::ios::binary);
    if (!tin.is_open())
    {
        qDebug() << "模板文件打开错误";
        ui->textBrowser->append("模板文件打开错误");
        return;
    }
    if(reader.parse(tin, troot))
    {
        qDebug() << "Reading Template File " << tfpath;
    }

    Json::Value iroot;
    std::ifstream in(qfopath->toLocal8Bit(), std::ios::binary);
    if (!in.is_open())
    {
//        qDebug() << "输出文件打开错误";
//        ui->textBrowser->append("输出文件打开错误");
        return;
    }
    if(reader.parse(in, iroot))
    {
        qDebug() << "Reading File " << *qfopath;
    }
    if(iroot["remoteData"]["generatedAt"].isNull())
    {
        return;
    }
    else
    {
        if(iroot["remoteData"]["generatedAt"].asString() == "-1")
        {
            qDebug() << "generatedAt is -1";
            return;
        }
        if(iroot["remoteData"]["generatedAt"].asUInt64() != troot["remoteData"]["generatedAt"].asUInt64())
        {
            troot["remoteData"].removeMember("generatedAt");
            troot["remoteData"]["generatedAt"] = iroot["remoteData"]["generatedAt"].asUInt64();
            if(!iroot["remoteData"]["version"].isNull())
                troot["remoteData"]["version"] = iroot["remoteData"]["version"].asString();
            std::ofstream out(tfpath.toLocal8Bit(), std::ios::binary);
            if (!out.is_open())
            {
                qDebug() << "模板输出文件打开错误";
                ui->textBrowser->append("模板输出文件打开错误");
                return;
            }
            Json::StyledWriter style_writer;

            std::string str_out = style_writer.write(troot);
            out << str_out;
            out.close();
            qDebug() << "模板更新完成";
            ui->textBrowser->append("模板更新完成");
        }
    }
}


void MainWindow::readJSONRootNamesMask(Json::Value *rp)
{
    this->readJSONRootNames(rp, 0, "");
    for(std::vector<QString> v : namelistlayer)
    {
        for(QString qs : v)
        {
            qDebug() << qs;
            ui->textBrowser->append(qs);
        }
    }
    namelistlayer.clear();
}
void MainWindow::readJSONRootNames(Json::Value *rp, int layer, QString preLayer)
{
    std::vector<QString> namelist;
    int now_layer = layer;
    if(namelistlayer.size() < layer + 1)
    {
        namelistlayer.push_back(namelist);
    }
    layer++;

    switch(rp->type())
    {
    case 7:
        for (std::string elem : rp->getMemberNames())
        {
            QString qelem = QString::fromStdString(elem);
            //qDebug() << qelem << " ";
            Json::Value *rp1 = &(rp->operator[](elem));
            int found = 0;
            for(QString find : namelistlayer.at(now_layer))
            {
                if(find.isEmpty())
                    break;
                if(find == qelem)
                {
                    found = 1;
                    break;
                }
            }
            if(!found)
            {
                if (preLayer == "definitions")
                {

                }
                else
                    namelistlayer.at(now_layer).push_back(qelem);
            }
            this->readJSONRootNames(rp1, layer, qelem);
        }
        break;
    default:

        break;
    }
}

void MainWindow::on_actionOpenOutputFile_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this,QStringLiteral("选择输出json"),"F:",QStringLiteral("json文件(*json)"));
    if(!file_name.isEmpty())
        ui->comboBox_2->addItem(file_name);
}


void MainWindow::on_actionOpenInputFile_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this,QStringLiteral("选择输入json"),"F:",QStringLiteral("json文件(*json)"));
    if(!file_name.isEmpty())
        ui->comboBox->addItem(file_name);
}

