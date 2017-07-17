#include <iostream>
#include <fstream>
#include "json/json.h"
using namespace std;


//parse create.json
int ReadJsonFromFile(const char *filename)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream is;
    is.open(filename, std::ios::binary);
    if(reader.parse(is, root))
    {
        std::string code;
        int id;
        //id= root["conference_id"].asInt();
        //code = root["conference_id"].asInt();
        //id= root["video"].size();
        Json::Value video = root["video"];
        string vcodec = video["video_codec"].asString();
        int width = video["width"].asInt();
        int height = video["height"].asInt();
        int vbitrate = video["bitrate"].asInt();
        int framerate = video["framerate"].asInt();
        int kint = video["kint"].asInt();
        int bframe = video["bframe"].asInt();
        cout << vcodec << endl;
        cout << width << endl;
        cout << height << endl;
        cout << vbitrate << endl;
        cout << framerate << endl;
        cout << kint << endl;
        cout << bframe << endl;
        cout << "========================" << endl;

        Json::Value audio = root["audio"];
        string acodec = audio["audio_codec"].asString();
        int samplerate = audio["samplerate"].asInt();
        int channel = audio["channel"].asInt();
        int abitrate = audio["bitrate"].asInt();

        cout << acodec << endl;
        cout << samplerate << endl;
        cout << channel << endl;
        cout << abitrate << endl;
    }
    is.close();

    return 0;
}

int ParseVideoMixFile(const char *filename)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream is;
    is.open(filename, std::ios::binary);
    if(reader.parse(is, root))
    {
        string deviceSN[16];
        int roomID = root["room_id"].asInt();
        string messageType = root["message_type"].asString();
        string to = root["to"].asString();
        string requestor = root["requestor"].asString();
        int sourceNumber = root["source_number"].asInt();
        int id= root["video_mixer"].size();
        if(sourceNumber != id )
        {
            cout << "json file error" << endl;
        }
        for(int i = 0; i < id; i++)
        {
            deviceSN[i] = root["video_mixer"][i].asString();
        }
        int width = root["width"].asInt();
        int height = root["height"].asInt();
        string layout = root["layout"].asString();
        int bitrate = root["bitrate"].asInt();
        int framerate = root["framerate"].asInt();
        
        cout << "roomID:" << roomID<<endl;
        cout << "messageType:" << messageType <<endl;
        cout << "to:" << to<<endl;
        cout << "requestor:" << requestor<<endl;
        cout << "sourceNumber:" <<sourceNumber <<endl;
        cout << "id:" << id <<endl;
        for(int i = 0 ; i <id ; i++)
        {
            cout << "deviceSN:" << deviceSN[i] <<endl;
        }
        cout << "width:" << width <<endl;
        cout << "height:" << height<<endl;
        cout << "layout:" << layout<<endl;
        cout << "bitrate" << bitrate<<endl;
        cout << "framerate" << framerate<<endl;
    }
    is.close();

    return 0;
}

struct AudioMixJsonParam{
    unsigned int roomID;
    string messageType;
    string to;
    string requestor;
    unsigned int sourceNumber;
    string deviceSN[16];
};
void ShowAudioMixParam(AudioMixJsonParam amjp)
{
    cout << "roomID:" << amjp.roomID << endl;
    cout << "messageType:" << amjp.messageType<< endl;
    cout << "to:" << amjp.to<< endl;
    cout << "requestor:" << amjp.requestor << endl;
    cout << "sourceNumber:" << amjp.sourceNumber << endl;
    for(int i = 0; i < amjp.sourceNumber; i++)
    {
        cout << "deviceSN:" << amjp.deviceSN[i] << endl;
    }
}

int ParseAudioMixFile(const char *filename)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream is;
    is.open(filename, std::ios::binary);
    AudioMixJsonParam amjp;
    if(reader.parse(is, root))
    {
        amjp.roomID = root["room_id"].asInt();
        amjp.messageType = root["message_type"].asString();
        amjp.to = root["to"].asString();
        amjp.requestor = root["requestor"].asString();
        amjp.sourceNumber = root["source_number"].asInt();
        int id = root["audio_mixer"].size();
        for(int i = 0; i < id; i++)
        {
            amjp.deviceSN[i] = root["audio_mixer"][i].asString();
        }
        ShowAudioMixParam(amjp);
    }
    is.close();

    return 0;
}

struct RecordJsonParam{
    unsigned roomID;
    string messageType;
    string to;
    string requestor;
    string fileName;
    string notes;
    string fileType;
    unsigned duration;
    unsigned maxSize;
    //video
    string video_codec;
    unsigned width;
    unsigned height;
    unsigned video_bitrate;
    unsigned framerate;
    unsigned kint;
    unsigned bframe;
    //audio
    string audio_codec;
    unsigned samplerate;
    unsigned channel;
    unsigned audio_bitrate;
};

void ShowRecordParam(RecordJsonParam rjp)
{
    cout << "roomID:" << rjp.roomID << endl;
    cout << "messageType:" << rjp.messageType << endl;
    cout << "to:" << rjp.to << endl;
    cout << "requestor:" << rjp.requestor << endl;
    cout << "fileName:" << rjp.fileName << endl;
    cout << "notes:" << rjp.notes << endl;
    cout << "fileType:" << rjp.fileType << endl;
    cout << "duration:" << rjp.duration << endl;
    cout << "maxSize:" << rjp.maxSize << endl;
    //video
    cout << "video_codec:" << rjp.video_codec << endl;
    cout << "width:" << rjp.width << endl;
    cout << "height:" << rjp.height << endl;
    cout << "video_bitrate:" << rjp.video_bitrate << endl;
    cout << "framerate:" << rjp.framerate << endl;
    cout << "kint:" << rjp.kint << endl;
    cout << "bframe:" << rjp.bframe << endl;
    //audio
    cout << "audio_codec:" << rjp.audio_codec << endl;
    cout << "samplerate:" << rjp.samplerate << endl;
    cout << "channel:" << rjp.channel << endl;
    cout << "audio_bitrate:" << rjp.audio_bitrate << endl;
}

int ParseRecordFile(const char *filename)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream is;
    is.open(filename, std::ios::binary);
    RecordJsonParam rjp;
    if(reader.parse(is, root))
    {
        rjp.roomID = root["room_id"].asInt();
        rjp.messageType = root["message_type"].asString();
        rjp.to = root["to"].asString();
        rjp.requestor = root["requestor"].asString();
        rjp.fileName= root["file_name"].asString();
        rjp.notes = root["notes"].asString();
        rjp.fileType= root["file_type"].asString();
        rjp.duration= root["duration"].asInt()*60;//转成s
        rjp.maxSize= root["max_size"].asInt()*1024*1024;//转成bit

        Json::Value video = root["video"];
        rjp.video_codec = video["video_codec"].asString();
        rjp.width = video["width"].asInt();
        rjp.height = video["height"].asInt();
        rjp.video_bitrate = video["bitrate"].asInt();
        rjp.framerate = video["framerate"].asInt();
        rjp.kint = video["kint"].asInt();
        rjp.bframe = video["bframe"].asInt();

        Json::Value audio = root["audio"];
        rjp.audio_codec = audio["audio_codec"].asString();
        rjp.samplerate = audio["samplerate"].asInt();
        rjp.channel = audio["channel"].asInt();
        rjp.audio_bitrate = audio["bitrate"].asInt();
        ShowRecordParam(rjp);
    }
    is.close();

    return 0;
}

int main(int argc, char **argv)
{
    //ReadJsonFromFile("../conf/create.json");
    //ParseVideoMixFile("../conf/mix.json");
    ParseAudioMixFile("../conf/mix_audio.json");
    ParseRecordFile("../conf/record.json");
    return 0;
}
