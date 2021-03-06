/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once
#include <cstdint>

namespace paddle_mobile {
namespace framework {

typedef uint16_t half_t;

half_t Float2Half(float f);

float Half2Float(half_t h);

void FloatArray2HalfArray(float *f_array, half_t *h_array, int count);

void HalfArray2FloatArray(half_t *h_array, float *f_array, int count);

}  // namespace framework
}  // namespace paddle_mobile
