/* ************************************************************************
 * Copyright (c) 2021-2022 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#include "roclapack_geblttrf_npvt.hpp"

template <typename T, typename U>
rocblas_status rocsolver_geblttrf_npvt_batched_impl(rocblas_handle handle,
                                                    const rocblas_int nb,
                                                    const rocblas_int nblocks,
                                                    U A,
                                                    const rocblas_int lda,
                                                    U B,
                                                    const rocblas_int ldb,
                                                    U C,
                                                    const rocblas_int ldc,
                                                    rocblas_int* info,
                                                    const rocblas_int batch_count)
{
    ROCSOLVER_ENTER_TOP("geblttrf_npvt_batched", "--nb", nb, "--nblocks", nblocks, "--lda", lda,
                        "--ldb", ldb, "--ldc", ldc, "--batch_count", batch_count);

    if(!handle)
        return rocblas_status_invalid_handle;

    // argument checking
    rocblas_status st = rocsolver_geblttrf_npvt_argCheck(handle, nb, nblocks, lda, ldb, ldc, A, B,
                                                         C, info, batch_count);
    if(st != rocblas_status_continue)
        return st;

    // working with unshifted arrays
    rocblas_int shiftA = 0;
    rocblas_int shiftB = 0;
    rocblas_int shiftC = 0;

    // normal execution
    rocblas_stride strideA = 0;
    rocblas_stride strideB = 0;
    rocblas_stride strideC = 0;

    // memory workspace sizes:
    // requirements for calling GETRF/GETRS
    bool optim_mem;
    size_t size_scalars, size_work1, size_work2, size_work3, size_work4, size_pivotval,
        size_pivotidx, size_iipiv, size_iinfo1;
    // size for temporary info values
    size_t size_iinfo2;

    rocsolver_geblttrf_npvt_getMemorySize<true, false, T>(
        nb, nblocks, batch_count, &size_scalars, &size_work1, &size_work2, &size_work3, &size_work4,
        &size_pivotval, &size_pivotidx, &size_iipiv, &size_iinfo1, &size_iinfo2, &optim_mem);

    if(rocblas_is_device_memory_size_query(handle))
        return rocblas_set_optimal_device_memory_size(
            handle, size_scalars, size_work1, size_work2, size_work3, size_work4, size_pivotval,
            size_pivotidx, size_iipiv, size_iinfo1, size_iinfo2);

    // memory workspace allocation
    void *scalars, *work1, *work2, *work3, *work4, *pivotval, *pivotidx, *iipiv, *iinfo1, *iinfo2;
    rocblas_device_malloc mem(handle, size_scalars, size_work1, size_work2, size_work3, size_work4,
                              size_pivotval, size_pivotidx, size_iipiv, size_iinfo1, size_iinfo2);

    if(!mem)
        return rocblas_status_memory_error;

    scalars = mem[0];
    work1 = mem[1];
    work2 = mem[2];
    work3 = mem[3];
    work4 = mem[4];
    pivotval = mem[5];
    pivotidx = mem[6];
    iipiv = mem[7];
    iinfo1 = mem[8];
    iinfo2 = mem[9];
    if(size_scalars > 0)
        init_scalars(handle, (T*)scalars);

    // Execution
    return rocsolver_geblttrf_npvt_template<true, false, T>(
        handle, nb, nblocks, A, shiftA, lda, strideA, B, shiftB, ldb, strideB, C, shiftC, ldc,
        strideC, info, batch_count, (T*)scalars, work1, work2, work3, work4, (T*)pivotval,
        (rocblas_int*)pivotidx, (rocblas_int*)iipiv, (rocblas_int*)iinfo1, (rocblas_int*)iinfo2,
        optim_mem);
}

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */

extern "C" {

rocblas_status rocsolver_sgeblttrf_npvt_batched(rocblas_handle handle,
                                                const rocblas_int nb,
                                                const rocblas_int nblocks,
                                                float* const A[],
                                                const rocblas_int lda,
                                                float* const B[],
                                                const rocblas_int ldb,
                                                float* const C[],
                                                const rocblas_int ldc,
                                                rocblas_int* info,
                                                const rocblas_int batch_count)
{
    return rocsolver_geblttrf_npvt_batched_impl<float>(handle, nb, nblocks, A, lda, B, ldb, C, ldc,
                                                       info, batch_count);
}

rocblas_status rocsolver_dgeblttrf_npvt_batched(rocblas_handle handle,
                                                const rocblas_int nb,
                                                const rocblas_int nblocks,
                                                double* const A[],
                                                const rocblas_int lda,
                                                double* const B[],
                                                const rocblas_int ldb,
                                                double* const C[],
                                                const rocblas_int ldc,
                                                rocblas_int* info,
                                                const rocblas_int batch_count)
{
    return rocsolver_geblttrf_npvt_batched_impl<double>(handle, nb, nblocks, A, lda, B, ldb, C, ldc,
                                                        info, batch_count);
}

rocblas_status rocsolver_cgeblttrf_npvt_batched(rocblas_handle handle,
                                                const rocblas_int nb,
                                                const rocblas_int nblocks,
                                                rocblas_float_complex* const A[],
                                                const rocblas_int lda,
                                                rocblas_float_complex* const B[],
                                                const rocblas_int ldb,
                                                rocblas_float_complex* const C[],
                                                const rocblas_int ldc,
                                                rocblas_int* info,
                                                const rocblas_int batch_count)
{
    return rocsolver_geblttrf_npvt_batched_impl<rocblas_float_complex>(
        handle, nb, nblocks, A, lda, B, ldb, C, ldc, info, batch_count);
}

rocblas_status rocsolver_zgeblttrf_npvt_batched(rocblas_handle handle,
                                                const rocblas_int nb,
                                                const rocblas_int nblocks,
                                                rocblas_double_complex* const A[],
                                                const rocblas_int lda,
                                                rocblas_double_complex* const B[],
                                                const rocblas_int ldb,
                                                rocblas_double_complex* const C[],
                                                const rocblas_int ldc,
                                                rocblas_int* info,
                                                const rocblas_int batch_count)
{
    return rocsolver_geblttrf_npvt_batched_impl<rocblas_double_complex>(
        handle, nb, nblocks, A, lda, B, ldb, C, ldc, info, batch_count);
}

} // extern C
