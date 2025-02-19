/*!
 *  Copyright (c) 2018 by Contributors
 * \file c_api_common.h
 * \brief DGL C API common util functions
 */
#ifndef DGL_C_API_COMMON_H_
#define DGL_C_API_COMMON_H_

#include <dgl/runtime/ndarray.h>
#include <dgl/runtime/packed_func.h>
#include <dgl/runtime/registry.h>
#include <algorithm>
#include <vector>

namespace {
/*! \brief Check whether two device contexts are the same.*/
inline bool operator == (const DLContext& ctx1, const DLContext& ctx2) {
  return ctx1.device_type == ctx2.device_type && ctx1.device_id == ctx2.device_id;
}

/*! \brief Output the string representation of device context.*/
inline std::ostream& operator << (std::ostream& os, const DLContext& ctx) {
  return os << "" << ctx.device_type << ":" << ctx.device_id << "";
}
}  // namespace

namespace dgl {

// Graph handler type
typedef void* GraphHandle;

// Communicator handler type
typedef void* CommunicatorHandle;

/*! \brief Enum type for bool value with unknown */
enum BoolFlag {
  kBoolUnknown = -1,
  kBoolFalse = 0,
  kBoolTrue = 1
};

/*!
 * \brief Convert a vector of NDArray to PackedFunc.
 */
dgl::runtime::PackedFunc ConvertNDArrayVectorToPackedFunc(
    const std::vector<dgl::runtime::NDArray>& vec);

/*!\brief Return whether the array is a valid 1D int array*/
inline bool IsValidIdArray(const dgl::runtime::NDArray& arr) {
  return arr->ctx.device_type == kDLCPU && arr->ndim == 1
    && arr->dtype.code == kDLInt && arr->dtype.bits == 64;
}

/*!
 * \brief Copy a vector to an int64_t NDArray.
 *
 * The element type of the vector must be convertible to int64_t.
 */
template<typename DType>
dgl::runtime::NDArray CopyVectorToNDArray(
    const std::vector<DType>& vec) {
  using dgl::runtime::NDArray;
  const int64_t len = vec.size();
  NDArray a = NDArray::Empty({len}, DLDataType{kDLInt, 64, 1}, DLContext{kDLCPU, 0});
  std::copy(vec.begin(), vec.end(), static_cast<int64_t*>(a->data));
  return a;
}

/* A structure used to return a vector of void* pointers. */
struct CAPIVectorWrapper {
  // The pointer vector.
  std::vector<void*> pointers;
};

/*!
 * \brief A helper function used to return vector of pointers from C to frontend.
 *
 * Note that the function will move the given vector memory into the returned
 * wrapper object.
 * 
 * \param vec The given pointer vectors.
 * \return A wrapper object containing the given data.
 */
template<typename PType>
CAPIVectorWrapper* WrapVectorReturn(std::vector<PType*> vec) {
  CAPIVectorWrapper* wrapper = new CAPIVectorWrapper;
  wrapper->pointers.reserve(vec.size());
  wrapper->pointers.insert(wrapper->pointers.end(), vec.begin(), vec.end());
  return wrapper;
}

}  // namespace dgl

#endif  // DGL_C_API_COMMON_H_
