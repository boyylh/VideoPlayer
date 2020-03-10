#ifndef IAUDIOPLAYER_H
#define IAUDIOPLAYER_H

class IAudioPlayer
{
public:
    static IAudioPlayer *Get() ;//单例模式
    virtual bool Start() = 0;//启动
    virtual void Play(bool isplay)=0;//暂停
    virtual bool Write(const char *data,int datasize) = 0;//将音频写入
    virtual void Stop() = 0;//停止
    virtual int GetFree() =0;//获取剩余空间
    virtual ~IAudioPlayer(){}

    int sampleRate = 48000;//样本率
    int sampleSize = 16;//样本大小
    int channel = 2;///通道数

protected:
    IAudioPlayer(){}
};
#endif // IAUDIOPLAYER_H
