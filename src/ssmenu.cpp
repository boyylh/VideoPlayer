#include "ssmenu.h"
#include "ui_ssmenu.h"
#include "ssplayer.h"
extern SSPlayer *globalSSPlayer;
SSMenu::SSMenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SSMenu)
{
    ui->setupUi(this);

    connect(ui->pushButton_open, &QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_close, &QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_play, &QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_pause,&QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_stop, &QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_volume, &QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_fullscreen, &QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_model, &QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_selectchannel, &QPushButton::clicked, this, &SSMenu::slotBtnClick);
    connect(ui->pushButton_edit, &QPushButton::clicked, this, &SSMenu::slotBtnClick);

    connect(ui->horizontalSlider_2, SIGNAL(sig_valueChanged(int)), this, SLOT(slotSliderMoved(int)));
    connect(ui->horizontalSlider_volume, SIGNAL(valueChanged(int)), this, SLOT(slotSliderMoved(int)));
}

SSMenu::~SSMenu()
{
    delete ui;
}

void SSMenu::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        qApp->quit();
    }
}

void SSMenu::slotSliderMoved(int value)
{
    if (QObject::sender() == ui->horizontalSlider_2)
    {
        globalSSPlayer->onSliderValue(enumMenuCmd::MC_progress, value);
    }
    else if (QObject::sender() == ui->horizontalSlider_volume)
    {
        globalSSPlayer->onSliderValue(enumMenuCmd::MC_VolumeValue, value);
    }
}

void SSMenu::slotBtnClick(bool isChecked)
{
    if (QObject::sender() == ui->pushButton_play)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_Play, isChecked);
    }
    else if (QObject::sender() == ui->pushButton_pause)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_Pause, isChecked);
    }
    else if (QObject::sender() == ui->pushButton_open)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_Open, isChecked);
    }
    else if (QObject::sender() == ui->pushButton_close)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_Close, isChecked);
    }
    else if (QObject::sender() == ui->pushButton_fullscreen)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_Fullscreen, isChecked);
    }
    else if (QObject::sender() == ui->pushButton_model)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_Model, isChecked);
    }
    else if (QObject::sender() == ui->pushButton_selectchannel)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_SelectChannel, isChecked);
    }
    else if (QObject::sender() == ui->pushButton_edit)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_edit, isChecked);
    }
    else if (QObject::sender() == ui->pushButton_volume)
    {
        globalSSPlayer->onClickButton(enumMenuCmd::MC_Volume, isChecked);
    }


}
