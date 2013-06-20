#include "lwPrefix.h"
#include "lwCamera.h"
#include "lwApp.h"


namespace lw
{


    Camera::Camera()
    {
        perspective(PVRT_PIf/3.f, 3.0f/2.0f, 1, 1000);
        PVRTVECTOR3 eye = {10.f, 10.f, 10.f};
        PVRTVECTOR3 at = {0.f, 0.f, 0.f};
        PVRTVECTOR3 up = {0.f, 1.f, 0.f};
        lookat(eye, at, up);
    }

    void Camera::getView(PVRTMATRIX& mat)
    {
        mat = _viewMatrix;
    }

    void Camera::getProj(PVRTMATRIX& mat)
    {
        mat = _projMatrix;
    }

    void Camera::getViewProj(PVRTMATRIX& mat)
    {
    //    PVRTMatrixMultiplyF(mat, _projMatrix, _viewMatrix);
        PVRTMatrixMultiplyF(mat, _viewMatrix, _projMatrix);
    }

    void Camera::lookat(
        const PVRTVECTOR3& eye,
        const PVRTVECTOR3& at,
        const PVRTVECTOR3& up)
    {
        PVRTMatrixLookAtRH(_viewMatrix, eye, at, up);
    }

    void Camera::perspective(
        float fovy, 
        float aspect, 
        float znear,
        float zfar)
    {
        PVRTMatrixPerspectiveFovRH(_projMatrix, fovy, aspect, znear, zfar, false);
    }
        
    void _ortho(PVRTMATRIX &m, float left, float right, float bottom, float top, float nearp, float farp)
    {
        PVRTMatrixIdentity(m);
        
        m[0][0] = 2 / (right - left);
        m[1][1] = 2 / (top - bottom);
        m[2][2] = -2 / (farp - nearp);
        m[3][0] = -(right + left) / (right - left);
        m[3][1] = -(top + bottom) / (top - bottom);
        m[3][2] = -(farp + nearp) / (farp - nearp);
    }

    void Camera::ortho(float w, float h, float zn, float zf)
    {
        _ortho(_projMatrix, 0, w, -h, 0, zn, zf);
        //PVRTMatrixOrthoRH is not correct
        //PVRTMatrixOrthoRH(_projMatrix, w, h, zn, zf, false);
    }

} //namespace lw