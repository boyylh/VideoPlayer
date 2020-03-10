#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H
#include "iaudioplayer.h"
#include <QAudioOutput>
#include <QMutex>
class AudioPlayer : public IAudioPlayer
{
public:
    AudioPlayer();
    virtual ~AudioPlayer();

    QAudioOutput *output = NULL;
    QIODevice *io = NULL;
    QMutex mutex;

    bool IsStartAudio();
    void Stop();

    //首先设置播放的格式以及参数
    bool Start();

    void Play(bool isplay);


    int GetFree();

    bool Write(const char *data, int datasize);

    IAudioPlayer* Get();
private:
    bool mIsStart =false;
};

#endif // AUDIOPLAYER_H
