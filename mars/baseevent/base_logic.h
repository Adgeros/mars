// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/*
 * baseprj.h
 *
 *  Created on: 2014-7-7
 *      Author: yerungui
 */

#ifndef MARS_BASELOGIC_H_
#define MARS_BASELOGIC_H_

#include <stdint.h>

#include <string>

namespace mars {
namespace baseevent {
void OnCreate();
void OnInitBeforeOnCreate(int _encoder_status);
void OnInitBeforeOnCreateV2(int _encoder_status, std::string _encoder_name);
void OnDestroy();
void OnSingalCrash(int _sig);
void OnExceptionCrash();
void OnForeground(bool _isforeground);
void OnNetworkChange();
void OnNetworkDataChange(const char* _tag, int32_t _send, int32_t _recv);
void OnAlarm(int64_t _id);
}  // namespace baseevent
}  // namespace mars

#endif /* MARS_BASELOGIC_H_ */
