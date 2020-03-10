#ifndef SSPLAYER_H
#define SSPLAYER_H

#include <QMainWindow>
#include <QShowEvent>
#include <QDebug>
#include <QPalette>
#include <QLabel>
#include <QResizeEvent>
#include "ssvideoframe.h"
#include "ssmenu.h"
namespace Ui {
class SSPlayer;
}

class SSPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit SSPlayer(QWidget *parent = 0);
    ~SSPlayer();

    void onClickButton(enumMenuCmd menucmd, bool bChecked);
    void onSliderValue(enumMenuCmd menucmd, int value);
signals:

protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent* event);
    void initUI();

private slots:

public slots:


private:
    Ui::SSPlayer *ui;
    SSVideoFrame *mVideoA = nullptr;
    SSVideoFrame *mVideoB = nullptr;
//    VideoLabel *mVideoC = nullptr;
    SSMenu       *mMenu = nullptr;

};

#endif // SSPLAYER_H
