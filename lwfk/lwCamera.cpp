#include "lwPrefix.h"
#include "lwCamera.h"
#include "lwApp.h"


namespace lw
{

    Camera::Camera()
    {
        perspective(PVRT_PIf/3.f, 3.0f/2.0f, 1, 1000);
        PVRTVec3 eye(10.f, 10.f, 10.f);
        PVRTVec3 at(0.f, 0.f, 0.f);
        PVRTVec3 up(0.f, 1.f, 0.f);
        lookat(eye, at, up);
    }

    void Camera::getView(PVRTMat4& mat) const
    {
        mat = _viewMatrix;
    }

    void Camera::getProj(PVRTMat4& mat) const
    {
        mat = _projMatrix;
    }

    void Camera::getViewProj(PVRTMat4& mat) const
    {
        mat = _projMatrix * _viewMatrix;
    }

    void Camera::lookat(
        const PVRTVec3& eye,
        const PVRTVec3& at,
        const PVRTVec3& up)
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
        
    void _ortho(PVRTMat4 &m, float left, float right, float bottom, float top, float nearp, float farp)
    {
        m = PVRTMat4::Identity();
        
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
    
    static Camera* g_current_camera = NULL;
    
    Camera* Camera::getCurrent() {
        return g_current_camera;
    }
    
    void Camera::setCurrent(Camera* pCamera) {
        g_current_camera = pCamera;
    }

} //namespace lw