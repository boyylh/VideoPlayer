#include "ssplayer.h"
#include "ui_ssplayer.h"


SSPlayer *globalSSPlayer;
SSPlayer::SSPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SSPlayer)
{
    ui->setupUi(this);
    globalSSPlayer = this;

    resize(1280,768);
    setWindowTitle(QStringLiteral("某音视频回放软件"));
}

SSPlayer::~SSPlayer()
{
    delete ui;
}

void SSPlayer::onClickButton(enumMenuCmd menucmd, bool bChecked)
{

    switch (menucmd) {
    case enumMenuCmd::MC_Open:

        break;
    case enumMenuCmd::MC_Play:
    {
        if(mVideoA->getCreationTime()!= mVideoB->getCreationTime())
        {
            qDebug()<<"创建时间不一致";
            //return;
        }
        mVideoA->synPlay();
        mVideoB->synPlay();
    }
        break;
    case enumMenuCmd::MC_Pause:
        mVideoA->synPause();
        mVideoB->synPause();
        break;
    case enumMenuCmd::MC_Close:

        break;
    case enumMenuCmd::MC_Fullscreen:

        break;
    case enumMenuCmd::MC_Model:

        break;
    case enumMenuCmd::MC_edit:

        break;
    case enumMenuCmd::MC_SelectChannel:

        break;
    case enumMenuCmd::MC_Volume:

        break;
    default:
        break;
    }
}

void SSPlayer::onSliderValue(enumMenuCmd menucmd, int value)
{
    switch (menucmd) {
    case enumMenuCmd::MC_progress:
        mVideoA->synSeek(value);
        mVideoB->synSeek(value);
        break;
    case enumMenuCmd::MC_VolumeValue:
        mVideoA->synSetVolume(value);
        mVideoB->synSetVolume(value);

        break;

    default:
        break;
    }
}


void SSPlayer::showEvent(QShowEvent *event)
{
    initUI();
    //initMenu();


    //    if(mVideoA)
    //    {
    //        connect(this,SIGNAL(signal_timerplay(int)),mVideoA, SLOT(slot_timerPlay(int)));
    //    }
    //    if(mVideoB)
    //    {
    //        connect(this,SIGNAL(signal_timerplay(int)),mVideoB, SLOT(slot_timerPlay(int)));
    //    }
    //    if(mVideoC)
    //    {
    //        connect(this,SIGNAL(signal_timerplay(int)),mVideoC, SLOT(slot_timerPlay(int)));
    //    }
}

void SSPlayer::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        qApp->quit();
    }
    if( event->modifiers() & Qt::AltModifier )
    {
        if(event->key() == Qt::Key_1)
        {
            //mVideoA->Video_Openfile("G://第七届成武老乡会开场视频——毕文倩个人作品.mp4");
            //mVideoA->Video_Openfile("C:/Users/User/Downloads/171124_B1_HD_001.mp4");
            //mVideoB->Video_Openfile("C:/Users/User/Downloads/171124_B1_HD_001.mp4");
            //mVideoC->Video_Openfile("C:/Users/User/Downloads/171124_B1_HD_001.mp4");
            //initVideo();
            qDebug()<<"sss";
        }
        if(event->key() == Qt::Key_2)
        {

            //mVideoB->Video_Startplay();
            //mVideoC->Video_Startplay();
        }
        if(event->key() == Qt::Key_3)
        {

        }

    }
}

void SSPlayer::initUI()
{

    //QGridLayout *gridLayout = new QGridLayout();

    ui->gridLayout->setRowStretch(0,100);
    ui->gridLayout->setRowStretch(1,100);
    ui->gridLayout->setRowStretch(2,1);

    ui->gridLayout->setColumnStretch(0,3);
    ui->gridLayout->setColumnStretch(1,2);

    mVideoA = new SSVideoFrame();
    mVideoA->setVideoId(SSVideoFrame::enumVideoId::VId_A);
    ui->gridLayout->addWidget(mVideoA, 0, 0, 1, 1);


    mVideoB = new SSVideoFrame();
    mVideoB->setVideoId(SSVideoFrame::enumVideoId::VId_B);
    ui->gridLayout->addWidget(mVideoB, 1, 0, 1, 1);

    //    mVideoC = new VideoLabel(enumVideoWidget::VW_C,this);
    //    ui->gridLayout->addWidget(mVideoC, 0, 1, 1, 1);

    mMenu = new SSMenu;
    ui->gridLayout->addWidget(mMenu,2,0,2,2);
    //connect();


}





