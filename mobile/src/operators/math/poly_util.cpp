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

#ifdef MULTICLASSNMS_OP

#include "operators/math/poly_util.h"

namespace paddle_mobile {
namespace operators {
namespace math {

template <class T>
void Array2PointVec(const T* box, const size_t box_size,
                    std::vector<Point_<T>>* vec) {
  size_t pts_num = box_size / 2;
  vec->resize(pts_num);
  for (size_t i = 0; i < pts_num; i++) {
    vec->at(i).x = box[2 * i];
    vec->at(i).y = box[2 * i + 1];
  }
}

template <class T>
void Array2Poly(const T* box, const size_t box_size, gpc::gpc_polygon* poly) {
  size_t pts_num = box_size / 2;
  poly->num_contours = 1;
  poly->hole = reinterpret_cast<int*>(malloc(sizeof(int)));
  poly->hole[0] = 0;
  poly->contour = (gpc::gpc_vertex_list*)malloc(sizeof(gpc::gpc_vertex_list));
  poly->contour->num_vertices = pts_num;
  poly->contour->vertex =
      (gpc::gpc_vertex*)malloc(sizeof(gpc::gpc_vertex) * pts_num);
  for (size_t i = 0; i < pts_num; ++i) {
    poly->contour->vertex[i].x = box[2 * i];
    poly->contour->vertex[i].y = box[2 * i + 1];
  }
}

template void Array2Poly(const float* box, const size_t box_size,
                         gpc::gpc_polygon* poly);

template <class T>
void Poly2PointVec(const gpc::gpc_vertex_list& contour,
                   std::vector<Point_<T>>* vec) {
  int pts_num = contour.num_vertices;
  vec->resize(pts_num);
  for (size_t i = 0; i < pts_num; i++) {
    vec->at(i).x = contour.vertex[i].x;
    vec->at(i).y = contour.vertex[i].y;
  }
}

template <class T>
T GetContourArea(const std::vector<Point_<T>>& vec) {
  int pts_num = vec.size();
  if (pts_num < 3) return T(0.);
  T area = T(0.);
  for (size_t i = 0; i < pts_num; ++i) {
    area += vec[i].x * vec[(i + 1) % pts_num].y -
            vec[i].y * vec[(i + 1) % pts_num].x;
  }
  return fabs(area / 2.0);
}

template <class T>
T PolyArea(const T* box, const size_t box_size, const bool normalized) {
  // If coordinate values are is invalid
  // if area size <= 0,  return 0.
  std::vector<Point_<T>> vec;
  Array2PointVec<T>(box, box_size, &vec);
  return GetContourArea<T>(vec);
}

template float PolyArea(const float* box, const size_t box_size,
                        const bool normalized);

template <class T>
T PolyOverlapArea(const T* box1, const T* box2, const size_t box_size,
                  const bool normalized) {
  gpc::gpc_polygon poly1;
  gpc::gpc_polygon poly2;
  Array2Poly<T>(box1, box_size, &poly1);
  Array2Poly<T>(box2, box_size, &poly2);
  gpc::gpc_polygon respoly;
  gpc::gpc_op op = gpc::GPC_INT;
  gpc::gpc_polygon_clip(op, &poly2, &poly1, &respoly);

  T inter_area = T(0.);
  int contour_num = respoly.num_contours;
  for (int i = 0; i < contour_num; ++i) {
    std::vector<Point_<T>> resvec;
    Poly2PointVec<T>(respoly.contour[i], &resvec);
    inter_area += GetContourArea<T>(resvec);
  }

  gpc::gpc_free_polygon(&poly1);
  gpc::gpc_free_polygon(&poly2);
  gpc::gpc_free_polygon(&respoly);
  return inter_area;
}

template float PolyOverlapArea(const float* box1, const float* box2,
                               const size_t box_size, const bool normalized);

}  // namespace math
}  // namespace operators
}  // namespace paddle_mobile

#endif
