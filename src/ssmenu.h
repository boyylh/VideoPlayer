#ifndef SSMENU_H
#define SSMENU_H

#include <QDialog>
#include <QKeyEvent>
namespace Ui {
class SSMenu;
}

enum enumMenuCmd
{
    MC_Open = 1,
    MC_Close,
    MC_Play,
    MC_Pause,
    MC_Stop,
    MC_Model,
    MC_Fullscreen,
    MC_SelectChannel,
    MC_edit,
    MC_Volume,
    MC_progress,
    MC_VolumeValue

};

class SSPlayer;
class SSMenu : public QDialog
{
    Q_OBJECT

public:
    explicit SSMenu(QWidget *parent = 0);
    ~SSMenu();
    void keyPressEvent(QKeyEvent *event);
private slots:
    void slotSliderMoved(int value);
    void slotBtnClick(bool isChecked);
private:
    Ui::SSMenu *ui;
};

#endif // SSMENU_H
