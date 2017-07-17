#include <iostream>
#include "json/json.h"
using namespace std;


void CreateResponse(bool value, string reason)
{
    Json::Value root;
    Json::Value arrayObj;
    Json::Value item;

    if(value)
    {
        item["video_port"] = 1234;
        arrayObj.append(item);
        item.clear();
        item["audio_port"] = 4567;
        arrayObj.append(item);
        root["message_type"] = "create_ok";
        root["ports"] = arrayObj;
    }
    else
    {
        root["message_type"] = "create_fail";
        root["reason"] = reason;
    }

    root["conference_id"] = 1111;


    cout <<root.toStyledString()<<endl;
}

void StopResponse(bool value, string reason)
{
    Json::Value root;
    root["conference_id"] = 111;
    root["message_type"] = value ? "stop_ok" : "stop_fail";
    root["requestor"] = "admin";
    if(!value)
        root["reason"] = reason;
    cout <<root.toStyledString()<<endl;
}


void JoinResponse(bool value, string reason)
{
    Json::Value root;
    root["conference_id"] = 111;
    root["message_type"] = value ? "join_ok" : "join_fail";
    root["device_type"] = "minicc_2";
    root["device_sn"] = "ASDFGASDGLASJ2R24";
    if(!value)
        root["reason"] = reason;

    cout <<root.toStyledString()<<endl;
}


void LeaveResponse(bool value, string reason)
{
    Json::Value root;
    root["conference_id"] = 111;
    root["message_type"] = value ? "leave_ok" : "leave_fail";
    root["device_type"] = "minicc_2";
    root["device_sn"] = "ASDFGASDGLASJ2R24";
    if(!value)
        root["reason"] = reason;

    cout <<root.toStyledString()<<endl;
}

void RecordResponse(bool value, string reason)
{
    Json::Value root;
    root["conference_id"] = 111;
    root["message_type"] = value ? "start_record_ok" : "start_record_fail";
    if(!value)
        root["reason"] = reason;
    cout <<root.toStyledString()<<endl;
}


void VideoMixResponse(bool value, string reason)
{
    Json::Value root;
    root["conference_id"] = 111;
    root["message_type"] = value ? "video_mix_ok" : "video_mix_fail";
    if(!value)
        root["reason"] = reason;
    cout <<root.toStyledString()<<endl;
}

void AudioMixResponse(bool value, string reason)
{
    Json::Value root;
    root["conference_id"] = 111;
    root["message_type"] = value ? "audio_mix_ok" : "audio_mix_fail";
    if(!value)
        root["reason"] = reason;
    cout <<root.toStyledString()<<endl;
}

void Update()
{
    Json::Value root;
    Json::Value arrayObj;
    Json::Value memberObj;
    Json::Value item;

    item["device_sn"] = "ABCDEF";
    item["bandwidth"] = "12312414";
    memberObj.append(item);
    item.clear();
    item["device_sn"] = "abcdef";
    item["bandwidth"] = "233333";
    memberObj.append(item);
    item.clear();

    item["room_id"] = 123;
    item["total"] = 8888;
    item["start_time"] = 11111;
    item["end_time"] = 22222;
    item["members"] = memberObj;
    arrayObj.append(item);

    root["name"] = "media_server_123";
    root["message_type"] = "counter_report";
    root["device_type"] = "media_server";
    root["counter_type"] = "bandwidth";
    root["counters"] = arrayObj;
    string msg = root.toStyledString();
    cout << msg << endl;
}

void Alarm()
{
    Json::Value root;
    Json::Value data;
    Json::Value item;
    item["device"] = "ABCDEFGGG";
    data["data"] = item;
    data["cause"] = "connection lost";
    root["alarm"] = data;
    root["name"] = "media_server_123";
    root["message_type"] = "alarm";
    root["time"] = 1234;


    string msg = root.toStyledString();
    cout << msg << endl;
}
int main(int argc, char **argv)
{
    
    //create_response(true, "test");
    //create_response(false, "fuck failed");
    StopResponse(true, "sttttt");
    StopResponse(false, "sttttt");
    JoinResponse(true, "asgdhfdh");
    JoinResponse(false, "asgdhfdh");
    LeaveResponse(true, "1324ertyu");
    LeaveResponse(false, "1324ertyu");
    RecordResponse(true,"sdfhew3");
    RecordResponse(false,"sdfhew3");
    VideoMixResponse(true,"sagdad");
    VideoMixResponse(false,"sagdad");
    AudioMixResponse(true,"sagdad");
    AudioMixResponse(false,"sagdad");
    Update();
    Alarm();
    return 0;
}
