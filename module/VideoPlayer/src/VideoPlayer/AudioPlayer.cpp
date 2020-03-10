#include "AudioPlayer.h"

AudioPlayer::AudioPlayer()
{

}

AudioPlayer::~AudioPlayer()
{
    mIsStart = false;
}

bool AudioPlayer::IsStartAudio()
{
    return mIsStart;
}

void AudioPlayer::Stop()
{
    mutex.lock();
    if (output)//为打开AudioOutput
    {
        output->stop();
        delete output;
        output = NULL;
        io = NULL;
    }
    mutex.unlock();
}

bool AudioPlayer::Start()
{
    Stop();
    mutex.lock();
    QAudioFormat fmt;//设置音频输出格式
    fmt.setSampleRate(48000);//1秒的音频采样率
    fmt.setSampleSize(16);//声音样本的大小
    fmt.setChannelCount(2);//声道
    fmt.setCodec("audio/pcm");//解码格式
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);//设置音频类型
    output = new QAudioOutput(fmt);
    io = output->start();//播放开始
    mutex.unlock();
    mIsStart = true;
    return true;
}

void AudioPlayer::Play(bool isplay)
{
    mutex.lock();
    if (!output)
    {
        mutex.unlock();
        return;
    }

    if (isplay)
    {
        output->resume();//恢复播放
    }
    else
    {
        output->suspend();//暂停播放
    }
    mutex.unlock();

}



int AudioPlayer::GetFree()
{
    mutex.lock();
    if (!output)
    {
        mutex.unlock();
        return 0;
    }
    int free = output->bytesFree();//剩余的空间

    mutex.unlock();

    return free;
}

bool AudioPlayer::Write(const char *data, int datasize)
{
    mutex.lock();
    if (io)
        io->write(data, datasize);//将获取的音频写入到缓冲区中
    mutex.unlock();
    return true;

}

IAudioPlayer *AudioPlayer::Get()
{
    static AudioPlayer ap;
    return &ap;
}
