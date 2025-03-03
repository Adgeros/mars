// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/**
 * created on : 2012-07-19
 * author : yanguoyue
 */

#include <unistd.h>
#ifndef USE_CPP_CALLBACK

#include <android/log.h>
#include <jni.h>

#include <map>
#include <string>

#include "mars/baseevent/active_logic.h"
#include "mars/baseevent/baseevent.h"
#include "mars/baseevent/baseprjevent.h"
#include "mars/boot/context.h"
#include "mars/comm/bootrun.h"
#include "mars/comm/jni/util/comm_function.h"
#include "mars/comm/jni/util/scope_jenv.h"
#include "mars/comm/jni/util/scoped_jstring.h"
#include "mars/comm/jni/util/var_cache.h"
#include "mars/comm/singleton.h"
#include "mars/comm/strutil.h"
#include "mars/comm/xlogger/xlogger.h"
#include "mars/stn/config.h"
#include "mars/stn/proto/stnproto_logic.h"
#include "mars/stn/src/net_core.h"
#include "mars/stn/src/net_source.h"
#include "mars/stn/src/signalling_keeper.h"
#include "mars/stn/src/smart_heartbeat.h"
#include "mars/stn/stn_logic.h"
#include "mars/stn/stn_manager.h"

using namespace mars::stn;

DEFINE_FIND_CLASS(KNetJava2C, "com/tencent/mars/stn/StnLogic")

extern "C" {

/*
 * Class:     com_tencent_mars_stn_StnLogic
 * Method:    getLoadLibraries
 * Signature: ()jobject
 */
JNIEXPORT jobject JNICALL Java_com_tencent_mars_stn_StnLogic_getLoadLibraries(JNIEnv* _env, jclass clz) {
    return mars::baseevent::getLoadLibraries(_env);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    reset
 * Signature: ()V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_reset, KNetJava2C, "reset", "()V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_reset(JNIEnv* _env, jclass clz) {
    Reset();
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    resetAndInitEncoderVersion
 * Signature: (I)V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_resetAndInitEncoderVersion, KNetJava2C, "resetAndInitEncoderVersion", "(ILjava/lang/String;)V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_resetAndInitEncoderVersion(JNIEnv* _env,
                                                                                     jclass clz,
                                                                                     jint _packer_encoder_version,
                                                                                     jstring _packer_encoder_name) {
    std::string packer_encoder_name = (NULL == _packer_encoder_name ? "" : ScopedJstring(_env, _packer_encoder_name).GetChar());
    ResetAndInitEncoderVersion(_packer_encoder_version, packer_encoder_name);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    setLonglinkSvrAddr
 * Signature: (Ljava/lang/String;[ILjava/lang/String;)V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_setLonglinkSvrAddr,
                          KNetJava2C,
                          "setLonglinkSvrAddr",
                          "(Ljava/lang/String;[ILjava/lang/String;)V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_setLonglinkSvrAddr(JNIEnv* _env,
                                                                             jclass,
                                                                             jstring _host,
                                                                             jintArray _ports,
                                                                             jstring _debug_ip) {
    std::string host = (NULL == _host ? "" : ScopedJstring(_env, _host).GetChar());
    std::string debug_ip = (NULL == _debug_ip ? "" : ScopedJstring(_env, _debug_ip).GetChar());
    std::vector<uint16_t> ports;

    if (NULL != _ports && _env->GetArrayLength(_ports) > 0) {
        int ports_len = _env->GetArrayLength(_ports);
        jint* p_port = _env->GetIntArrayElements(_ports, NULL);

        for (int i = 0; i < ports_len; ++i) {
            ports.push_back((uint16_t)(p_port[i]));
        }
        _env->ReleaseIntArrayElements(_ports, p_port, 0);
    }

    SetLonglinkSvrAddr(host, ports, debug_ip);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    setShortlinkSvrAddr
 * Signature: (ILjava/lang/String;)V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_setShortlinkSvrAddr, KNetJava2C, "setShortlinkSvrAddr", "(ILjava/lang/String;)V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_setShortlinkSvrAddr(JNIEnv* _env,
                                                                              jclass,
                                                                              jint _port,
                                                                              jstring _debug_ip) {
    std::string debug_ip = (NULL == _debug_ip ? "" : ScopedJstring(_env, _debug_ip).GetChar());
    SetShortlinkSvrAddr(_port, debug_ip);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    setDebugIP
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_setDebugIP, KNetJava2C, "setDebugIP", "(Ljava/lang/String;Ljava/lang/String;)V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_setDebugIP(JNIEnv* _env,
                                                                     jclass,
                                                                     jstring _host,
                                                                     jstring _debug_ip) {
    std::string host = (NULL == _host ? "" : ScopedJstring(_env, _host).GetChar());
    std::string debug_ip = (NULL == _debug_ip ? "" : ScopedJstring(_env, _debug_ip).GetChar());
    SetDebugIP(host, debug_ip);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    setBackupIPs
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_setBackupIPs, KNetJava2C, "setBackupIPs", "(Ljava/lang/String;[Ljava/lang/String;)V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_setBackupIPs(JNIEnv* _env,
                                                                       jclass,
                                                                       jstring _host,
                                                                       jobjectArray _objarray) {
    std::string host = (NULL == _host ? "" : ScopedJstring(_env, _host).GetChar());
    std::vector<std::string> backupip_list;

    jsize size = _env->GetArrayLength(_objarray);
    for (int i = 0; i < size; i++) {
        jstring ip = (jstring)_env->GetObjectArrayElement(_objarray, i);
        if (ip != NULL) {
            backupip_list.push_back(ScopedJstring(_env, ip).GetChar());
        }
        _env->DeleteLocalRef(ip);
    }

    SetBackupIPs(host, backupip_list);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    startTask
 * Signature: (Lcom/tencent/mars/stn/StnLogic$Task;)V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_startTask, KNetJava2C, "startTask", "(Lcom/tencent/mars/stn/StnLogic$Task;)V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_startTask(JNIEnv* _env, jclass, jobject _task) {
    xverbose_function();

    // get the field value of the netcmd
    jint taskid = JNU_GetField(_env, _task, "taskID", "I").i;
    jint cmdid = JNU_GetField(_env, _task, "cmdID", "I").i;
    jint channel_select = JNU_GetField(_env, _task, "channelSelect", "I").i;
    jobject hostlist = JNU_GetField(_env, _task, "shortLinkHostList", "Ljava/util/ArrayList;").l;
    jstring cgi = (jstring)JNU_GetField(_env, _task, "cgi", "Ljava/lang/String;").l;

    jboolean send_only = JNU_GetField(_env, _task, "sendOnly", "Z").z;
    jboolean need_authed = JNU_GetField(_env, _task, "needAuthed", "Z").z;
    jboolean limit_flow = JNU_GetField(_env, _task, "limitFlow", "Z").z;
    jboolean limit_frequency = JNU_GetField(_env, _task, "limitFrequency", "Z").z;

    jint channel_strategy = JNU_GetField(_env, _task, "channelStrategy", "I").i;
    jboolean network_status_sensitive = JNU_GetField(_env, _task, "networkStatusSensitive", "Z").z;
    jint priority = JNU_GetField(_env, _task, "priority", "I").i;

    jint retrycount = JNU_GetField(_env, _task, "retryCount", "I").i;
    jint server_process_cost = JNU_GetField(_env, _task, "serverProcessCost", "I").i;
    jint total_timetout = JNU_GetField(_env, _task, "totalTimeout", "I").i;
    jstring report_arg = (jstring)JNU_GetField(_env, _task, "reportArg", "Ljava/lang/String;").l;

    jboolean long_polling = JNU_GetField(_env, _task, "longPolling", "Z").z;
    jint long_polling_timeout = JNU_GetField(_env, _task, "longPollingTimeout", "I").i;

    jobject oHeaders = JNU_GetField(_env, _task, "headers", "Ljava/util/Map;").l;
    std::map<std::string, std::string> headers = JNU_JObject2Map(_env, oHeaders);
    jint client_sequence_id = JNU_GetField(_env, _task, "clientSequenceId", "I").i;

    jboolean needRealtimeNetInfo = JNU_GetField(_env, _task, "needRealtimeNetInfo", "Z").z;

    jobject extraInfo = JNU_GetField(_env, _task, "extraInfo", "Ljava/util/Map;").l;
    std::map<std::string, std::string> extra_info = JNU_JObject2Map(_env, extraInfo);

    // init struct Task
    struct Task task(taskid);
    task.cmdid = cmdid;
    task.channel_select = channel_select;

    task.send_only = send_only;
    task.need_authed = need_authed;
    task.limit_flow = limit_flow;
    task.limit_frequency = limit_frequency;

    task.channel_strategy = channel_strategy;
    task.network_status_sensitive = network_status_sensitive;
    task.priority = priority;

    task.retry_count = retrycount;
    task.server_process_cost = server_process_cost;
    task.total_timeout = total_timetout;
    task.headers = headers;

    task.long_polling = long_polling;
    task.long_polling_timeout = long_polling_timeout;

    if (NULL != report_arg) {
        task.report_arg = ScopedJstring(_env, report_arg).GetChar();
    }

    if (NULL != hostlist) {
        jclass cls_arraylist = _env->GetObjectClass(hostlist);
        // method in class ArrayList
        jmethodID arraylist_get = _env->GetMethodID(cls_arraylist, "get", "(I)Ljava/lang/Object;");
        jmethodID arraylist_size = _env->GetMethodID(cls_arraylist, "size", "()I");
        jint len = _env->CallIntMethod(hostlist, arraylist_size);
        for (int i = 0; i < len; i++) {
            jstring host = (jstring)_env->CallObjectMethod(hostlist, arraylist_get, i);
            if (NULL != host) {
                task.shortlink_host_list.push_back(ScopedJstring(_env, host).GetChar());
                _env->DeleteLocalRef(host);
            }
        }
        _env->DeleteLocalRef(hostlist);
    }

    if (NULL != cgi) {
        task.cgi = ScopedJstring(_env, cgi).GetChar();
        _env->DeleteLocalRef(cgi);
    }

    jobject _user_context = JNU_GetField(_env, _task, "userContext", "Ljava/lang/Object;").l;
    if (NULL != _user_context) {
        task.user_context = _env->NewGlobalRef(_user_context);
        _env->DeleteLocalRef(_user_context);
    }
    task.client_sequence_id = client_sequence_id;
    task.need_realtime_netinfo = (needRealtimeNetInfo == JNI_TRUE);
    task.extra_info = extra_info;

    StartTask(task);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    stopTask
 * Signature: (I)V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_stopTask, KNetJava2C, "stopTask", "(I)V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_stopTask(JNIEnv* _env, jclass, jint _taskid) {
    xverbose_function();

    StopTask(_taskid);
}

DEFINE_FIND_STATIC_METHOD(KJava2C_hasTask, KNetJava2C, "hasTask", "(I)Z")
JNIEXPORT jboolean JNICALL Java_com_tencent_mars_stn_StnLogic_hasTask(JNIEnv* _env, jclass, jint _taskid) {
    xverbose_function();

    return HasTask(_taskid);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    redoTask
 * Signature: ()V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_redoTask, KNetJava2C, "redoTask", "()V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_redoTask(JNIEnv* _env, jclass) {
    xverbose_function();

    RedoTasks();
}

JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_touchTasks(JNIEnv* _env, jclass) {
    TouchTasks();
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    clearTask
 * Signature: ()V
 */
DEFINE_FIND_STATIC_METHOD(KJava2C_clearTask, KNetJava2C, "clearTask", "()V")
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_clearTask(JNIEnv* _env, jclass) {
    xverbose_function();

    ClearTasks();
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    makesureLongLinkConnected
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_makesureLongLinkConnected(JNIEnv* _env, jclass) {
    xverbose_function();

    MakesureLonglinkConnected();
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    setSignallingStrategy
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_setSignallingStrategy(JNIEnv* _env,
                                                                                jclass,
                                                                                jlong _period,
                                                                                jlong _keep_time) {
    xverbose_function();

    SetSignallingStrategy((long)_period, (long)_keep_time);
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    keepSignalling
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_keepSignalling(JNIEnv* _env, jclass) {
    xverbose_function();

    KeepSignalling();
}

/*
 * Class:     com_tencent_mars_stn_StnLogic_Java2C
 * Method:    stopSignalling
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_stopSignalling(JNIEnv* _env, jclass) {
    xverbose_function();

    StopSignalling();
}

JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_setClientVersion(JNIEnv* _env, jclass, jint _client_version) {
    mars::stn::SetClientVersion(_client_version);
}

JNIEXPORT jint JNICALL Java_com_tencent_mars_stn_StnLogic_genTaskID(JNIEnv* _env, jclass) {
    return (jint)mars::stn::GenTaskID();
}

JNIEXPORT void JNICALL Java_com_tencent_mars_stn_StnLogic_trigNooping(JNIEnv* _env, jclass) {
    SmartHeartbeat::SetHeartBeat(0);
    mars::stn::DefaultLongLink()->TrigNoop();
}

JNIEXPORT jint JNICALL Java_com_tencent_mars_stn_StnLogic_genSequenceId(JNIEnv* _env, jclass) {
    return (jint)mars::stn::GenSequenceId();
}
}

void ExportSTN() {
}

#endif
