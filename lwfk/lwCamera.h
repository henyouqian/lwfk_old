#ifndef __LW_CAMERA_H__
#define __LW_CAMERA_H__


namespace lw
{

class Camera
{
public:
    Camera();
    
    void lookat(const PVRTVec3& eye,
                const PVRTVec3& at,
                const PVRTVec3& up);
    void perspective(float fovy, float aspect, float znear, float zfar);
    void ortho(float w, float h, float zn, float zf);
    
    void getView(PVRTMat4& mat) const;
    void getProj(PVRTMat4& mat)  const;
    void getViewProj(PVRTMat4& mat)  const;
    
    static Camera* getCurrent();
    static void setCurrent(Camera* pCamera);
    
private:
    PVRTMat4 _viewMatrix;
    PVRTMat4 _projMatrix;
};

} //namespace lw



#endif //__LW_CAMERA_H__