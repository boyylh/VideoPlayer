/**
 * 叶海辉
 * QQ群121376426
 * http://blog.yundiantech.com/
 */

#include "ssvideoframe.h"
#include "ui_ssvideoframe.h"

#include <QPainter>
#include <QPaintEvent>
#include <QFileDialog>
#include <QDebug>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QMessageBox>
#include <QProcess>
#include <QCoreApplication>
#include <windows.h>
#include <QJsonDocument>
#include <QJsonObject>
#include "AppConfig.h"
#include "Base/FunctionTransfer.h"

Q_DECLARE_METATYPE(VideoPlayerState)

SSVideoFrame::SSVideoFrame(QWidget *parent) :
    DragAbleWidget(parent),
    ui(new Ui::SSVideoFrame)
{
    ui->setupUi(this->getContainWidget());

    FunctionTransfer::init(QThread::currentThreadId());

    ///初始化播放器
    VideoPlayer::initPlayer();

    setWindowFlags (Qt::CustomizeWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);//|Qt::WindowStaysOnTopHint);  //使窗口的标题栏隐藏
    //    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowOpacity(0.7);

    //因为VideoPlayer::PlayerState是自定义的类型 要跨线程传递需要先注册一下
    qRegisterMetaType<VideoPlayerState>();

    connect(ui->pushButton_open, &QPushButton::clicked, this, &SSVideoFrame::slotBtnClick);
    connect(ui->toolButton_open, &QPushButton::clicked, this, &SSVideoFrame::slotBtnClick);
    connect(ui->pushButton_play, &QPushButton::clicked, this, &SSVideoFrame::slotBtnClick);
    connect(ui->pushButton_pause,&QPushButton::clicked, this, &SSVideoFrame::slotBtnClick);
    connect(ui->pushButton_stop, &QPushButton::clicked, this, &SSVideoFrame::slotBtnClick);
    connect(ui->pushButton_volume, &QPushButton::clicked, this, &SSVideoFrame::slotBtnClick);

    connect(ui->horizontalSlider, SIGNAL(sig_valueChanged(int)), this, SLOT(slotSliderMoved(int)));
    connect(ui->horizontalSlider_volume, SIGNAL(valueChanged(int)), this, SLOT(slotSliderMoved(int)));

    ui->page_video->setMouseTracking(true);
    ui->page_video->installEventFilter(this);
    ui->widget_container->installEventFilter(this);

    mPlayer = new VideoPlayer();
    mPlayer->setVideoPlayerCallBack(this);

    mTimer = new QTimer; //定时器-获取当前视频时间
    connect(mTimer, &QTimer::timeout, this, &SSVideoFrame::slotTimerTimeOut);
    mTimer->setInterval(500);

    mTimer_CheckControlWidget = new QTimer; //用于控制控制界面的出现和隐藏
    connect(mTimer_CheckControlWidget, &QTimer::timeout, this, &SSVideoFrame::slotTimerTimeOut);
    mTimer_CheckControlWidget->setInterval(1500);

    mAnimation_ControlWidget  = new QPropertyAnimation(ui->widget_controller, "geometry");

    ui->stackedWidget->setCurrentWidget(ui->page_open);
    ui->pushButton_pause->hide();

    //resize(1024,768);
    //    setTitle(QStringLiteral("音视频回放软件-V%1").arg(AppConfig::VERSION_NAME));

    mVolume = mPlayer->getVolume();

}

SSVideoFrame::~SSVideoFrame()
{
    AppConfig::saveConfigInfoToFile();
    AppConfig::removeDirectory(AppConfig::AppDataPath_Tmp);

    delete ui;
}

void SSVideoFrame::setVideoId(enumVideoId videoId)
{
    switch ((enumVideoId)videoId) {
    case enumVideoId::VId_A:
        mTittleId = "A";
        break;
    case enumVideoId::VId_B:
        mTittleId = "B";
        break;
    default:
        mTittleId = "";
        break;
    }
    setTitle(mTittleId);

    qDebug()<< ui->toolButton_open->styleSheet();
}

void SSVideoFrame::setVideoFile(const QString &sFileName)
{
    mProcess  = new QProcess;
    QString svideoPath = sFileName;
    QString sFFprobePath = AppConfig::gExePath+("/ffprobe.exe");
    //    QString sOutjason = AppConfig::gExePath+ QString("/AA.txt");

    //QDir::setCurrent(QString("%1").arg(appPath));
    QFileInfo fi(sFFprobePath);
    if(!fi.exists())
    {
        qDebug()<<"不存在路径"<<sFFprobePath;
        return;
    }
    //    qDebug()<<sFFprobePath;
    //    qDebug()<<sOutjason;
    QStringList command ;// = sFFprobePath+ QString(" -v quiet -print_format json -show_format %1 >>%2").arg(svideoPath).arg(sOutjason);
    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(sltOnReadOutput()));
    connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFinished(int, QProcess::ExitStatus)));
    command<<"-v"<<"quiet"<<"-print_format"<<"json"<<"-show_format"<<svideoPath/*<<">>"<<sOutjason*/;

    mProcess->setProcessChannelMode(QProcess::MergedChannels);
    mProcess->start(QString("\"%1\"").arg(sFFprobePath),command);
    mProcess->waitForFinished();


    mPlayer->stop(true); //如果在播放则先停止
    mPlayer->loadVideoFile(sFileName);

    AppConfig::gVideoFilePath = sFileName;
    AppConfig::saveConfigInfoToFile();
    setTitle(mTittleId+"-"+AppConfig::gVideoFilePath);
}

void SSVideoFrame::synPlay()
{
    if(!IsOpenFile()) return;
    if(mPlayer->isPaused())
    {
        mPlayer->play();
    }else
    {
        mPlayer->startPlay("");
    }
}

void SSVideoFrame::synPause()
{
    mPlayer->pause();
}

void SSVideoFrame::synSeek(int v)
{
    ui->horizontalSlider->setValue(v);
    mPlayer->seek((qint64)v * 1000000);
}

void SSVideoFrame::synSetVolume(int v)
{
    ui->horizontalSlider_volume->setValue(v);
    mPlayer->setVolume(v / 100.0);
    ui->label_volume->setText(QString("%1").arg(v));
}

bool SSVideoFrame::IsOpenFile()
{
    return mPlayer->IsOpenFile();
}

QDateTime SSVideoFrame::getCreationTime()
{
    return mCreationTime;
}

void SSVideoFrame::showOutControlWidget()
{

    mAnimation_ControlWidget->setDuration(800);

    int w = ui->widget_controller->width();
    int h = ui->widget_controller->height();
    int x = 0;
    int y = ui->widget_container->height() - ui->widget_controller->height();

    if (ui->widget_controller->isHidden())
    {
        ui->widget_controller->show();
        mAnimation_ControlWidget->setStartValue(ui->widget_controller->geometry());
    }
    else
    {
        mAnimation_ControlWidget->setStartValue(ui->widget_controller->geometry());
    }

    //    mAnimation_ControlWidget->setKeyValueAt(0, QRect(0, 0, 00, 00));
    //    mAnimation_ControlWidget->setKeyValueAt(0.4, QRect(20, 250, 20, 30));
    //    mAnimation_ControlWidget->setKeyValueAt(0.8, QRect(100, 250, 20, 30));
    //    mAnimation_ControlWidget->setKeyValueAt(1, QRect(250, 250, 100, 30));
    mAnimation_ControlWidget->setEndValue(QRect(x, y, w, h));
    mAnimation_ControlWidget->setEasingCurve(QEasingCurve::Linear); //设置动画效果

    mAnimation_ControlWidget->start();

}

void SSVideoFrame::hideControlWidget()
{
    mAnimation_ControlWidget->setTargetObject(ui->widget_controller);

    mAnimation_ControlWidget->setDuration(300);

    int w = ui->widget_controller->width();
    int h = ui->widget_controller->height();
    int x = 0;
    int y = ui->widget_container->height() + h;

    mAnimation_ControlWidget->setStartValue(ui->widget_controller->geometry());
    mAnimation_ControlWidget->setEndValue(QRect(x, y, w, h));
    mAnimation_ControlWidget->setEasingCurve(QEasingCurve::Linear); //设置动画效果

    mAnimation_ControlWidget->start();
}


void SSVideoFrame::slotSliderMoved(int value)
{
    if (QObject::sender() == ui->horizontalSlider)
    {
        mPlayer->seek((qint64)value * 1000000);
    }
    else if (QObject::sender() == ui->horizontalSlider_volume)
    {
        mPlayer->setVolume(value / 100.0);
        ui->label_volume->setText(QString("%1").arg(value));
    }
}

void SSVideoFrame::slotTimerTimeOut()
{
    if (QObject::sender() == mTimer)
    {
        qint64 Sec = mPlayer->getCurrentTime();

        ui->horizontalSlider->setValue(Sec);

        //    QString hStr = QString("00%1").arg(Sec/3600);
        QString mStr = QString("00%1").arg(Sec/60);
        QString sStr = QString("00%1").arg(Sec%60);

        QString str = QString("%1:%2").arg(mStr.right(2)).arg(sStr.right(2));
        ui->label_currenttime->setText(str);
    }
    else if (QObject::sender() == mTimer_CheckControlWidget)
    {
        mTimer_CheckControlWidget->stop();
        //hideControlWidget();
    }
}

void SSVideoFrame::slotBtnClick(bool isChecked)
{
    if (QObject::sender() == ui->pushButton_play)
    {
        mPlayer->startPlay("");
    }
    else if (QObject::sender() == ui->pushButton_pause)
    {
        mPlayer->pause();
    }
    else if (QObject::sender() == ui->pushButton_stop)
    {
        mPlayer->stop(true);
        setTitle(mTittleId);
    }
    else if (QObject::sender() == ui->pushButton_open || QObject::sender() == ui->toolButton_open)
    {
        QString s = QFileDialog::getOpenFileName(
                    this, QStringLiteral("选择要播放的文件"),
                    AppConfig::gVideoFilePath,//初始目录
                    QStringLiteral("视频文件 (*.flv *.rmvb *.avi *.MP4 *.mkv);;")
                    +QStringLiteral("音频文件 (*.mp3 *.wma *.wav);;")
                    +QStringLiteral("所有文件 (*.*)"));
        if (!s.isEmpty())
        {
            //            s.replace("/","\\");

            mPlayer->stop(true); //如果在播放则先停止
            setVideoFile(s);
             //           mPlayer->startPlay(s.toStdString());
            //            mPlayer->getCurrentTime();

            //            AppConfig::gVideoFilePath = s;
            //            AppConfig::saveConfigInfoToFile();
            //            setTitle(mTittleId+"-"+AppConfig::gVideoFilePath);
        }
    }
    else if (QObject::sender() == ui->pushButton_volume)
    {
        qDebug()<<isChecked;

        bool isMute = isChecked;
        mPlayer->setMute(isMute);

        if (isMute)
        {
            mVolume = mPlayer->getVolume();

            ui->horizontalSlider_volume->setValue(0);
            ui->horizontalSlider_volume->setEnabled(false);
            ui->label_volume->setText(QString("%1").arg(0));
        }
        else
        {
            int volume = mVolume * 100.0;
            ui->horizontalSlider_volume->setValue(volume);
            ui->horizontalSlider_volume->setEnabled(true);
            ui->label_volume->setText(QString("%1").arg(volume));
        }

    }

}

void SSVideoFrame::sltOnReadOutput()
{
    QByteArray baJson =  mProcess->readAllStandardOutput();
   // qDebug()<<"output"<<baJson;
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(baJson,&jsonError);
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug()<<"json error";
        return;
    }else
    {
        qDebug()<<jsonError.errorString();
    }

    QJsonObject rootObj = document.object();
    QStringList keys = rootObj.keys();
    //qDebug()<<keys;

    if(rootObj.contains("format"))
    {
        QJsonObject ObjFormat = rootObj.value("format").toObject();
        if(ObjFormat.contains("duration"))
        {
            qDebug() << "duration is:" << ObjFormat.value("duration").toVariant().toDouble();
        }
        if(ObjFormat.contains("tags"))
        {
            QJsonObject subObj = ObjFormat.value("tags").toObject();
            QString creationtime = subObj.value("creation_time").toString();
            QDateTime utcTime = QDateTime::fromString(creationtime,Qt::ISODate);
            mCreationTime = utcTime.toLocalTime();
            qDebug()<<utcTime.toLocalTime().toString("yyyy-MM-dd HH:mm:ss");
        }
    }

}

void SSVideoFrame::onFinished(int e, QProcess::ExitStatus)
{
    qDebug()<<e<<mProcess->errorString();


    "ffprobe -v quiet -print_format json -show_format -show_streams G:\AAAA.mp4 >>G:\media_json.txt";


}

///打开文件失败
void SSVideoFrame::onOpenVideoFileFailed(const int &code)
{
    FunctionTransfer::runInMainThread([=]()
    {
        QMessageBox::critical(NULL, "tips", QString("open file failed %1").arg(code));
    });
}

///打开SDL失败的时候回调此函数
void SSVideoFrame::onOpenSdlFailed(const int &code)
{
    FunctionTransfer::runInMainThread([=]()
    {
        QMessageBox::critical(NULL, "tips", QString("open Sdl failed %1").arg(code));
    });
}

///获取到视频时长的时候调用此函数
void SSVideoFrame::onTotalTimeChanged(const int64_t &uSec)
{
    FunctionTransfer::runInMainThread([=]()
    {
        qint64 Sec = uSec/1000000;

        ui->horizontalSlider->setRange(0,Sec);

        //    QString hStr = QString("00%1").arg(Sec/3600);
        QString mStr = QString("00%1").arg(Sec/60);
        QString sStr = QString("00%1").arg(Sec%60);

        QString str = QString("%1:%2").arg(mStr.right(2)).arg(sStr.right(2));
        ui->label_totaltime->setText(str);
    });
}

///播放器状态改变的时候回调此函数
void SSVideoFrame::onPlayerStateChanged(const VideoPlayerState &state, const bool &hasVideo, const bool &hasAudio)
{
    FunctionTransfer::runInMainThread([=]()
    {
        if (state == VideoPlayer_Stop)
        {
            ui->stackedWidget->setCurrentWidget(ui->page_open);

            ui->pushButton_pause->hide();
            ui->widget_videoPlayer->clear();

            ui->horizontalSlider->setValue(0);
            ui->label_currenttime->setText("00:00");
            ui->label_totaltime->setText("00:00");

            mTimer->stop();

        }
        else if (state == VideoPlayer_Playing)
        {
            if (hasVideo)
            {
                ui->stackedWidget->setCurrentWidget(ui->page_video);
            }
            else
            {
                ui->stackedWidget->setCurrentWidget(ui->page_audio);
            }

            ui->pushButton_play->hide();
            ui->pushButton_pause->show();

            mTimer->start();
        }
        else if (state == VideoPlayer_Pause)
        {
            ui->pushButton_pause->hide();
            ui->pushButton_play->show();
        }
    });
}

///显示视频数据，此函数不宜做耗时操作，否则会影响播放的流畅性。
void SSVideoFrame::onDisplayVideo(std::shared_ptr<VideoFrame> videoFrame)
{
    ui->widget_videoPlayer->inputOneFrame(videoFrame);
}

//图片显示部件时间过滤器处理
bool SSVideoFrame::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->widget_container)
    {
        if(event->type() == QEvent::Resize)
        {
            ///停止动画，防止此时刚好开始动画，导致位置出错
            mAnimation_ControlWidget->stop();

            QResizeEvent * e = (QResizeEvent*)event;
            int w = e->size().width();
            int h = e->size().height();
            ui->stackedWidget->move(0, 0);
            ui->stackedWidget->resize(w, h);

            int x = 0;
            int y = h - ui->widget_controller->height();
            ui->widget_controller->move(x, y);
            ui->widget_controller->resize(w, ui->widget_controller->height());

        }
    }
    else if(target == ui->page_video)
    {
        if(event->type() == QEvent::MouseMove)
        {
            if (!mTimer_CheckControlWidget->isActive())
            {
                showOutControlWidget();
            }

            mTimer_CheckControlWidget->stop();
            mTimer_CheckControlWidget->start();
        }
        else if(event->type() == QEvent::Enter)
        {
            ui->widget_controller->show();
        }
        else if(event->type() == QEvent::Leave)
        {
            mTimer_CheckControlWidget->stop();
            mTimer_CheckControlWidget->start();
        }
    }

    //其它部件产生的事件则交给基类处理
    return DragAbleWidget::eventFilter(target, event);
}
