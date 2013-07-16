#ifndef __LW_CAMERA_H__
#define __LW_CAMERA_H__


namespace lw
{

class Camera
{
public:
    Camera();
    
    void lookat(const PVRTVECTOR3& eye,
                const PVRTVECTOR3& at,
                const PVRTVECTOR3& up);
    void perspective(float fovy, float aspect, float znear, float zfar);
    void ortho(float w, float h, float zn, float zf);
    
    void getView(PVRTMATRIX& mat) const;
    void getProj(PVRTMATRIX& mat)  const;
    void getViewProj(PVRTMATRIX& mat)  const;
    
    static Camera* getCurrent();
    static void setCurrent(Camera* pCamera);
    
private:
    PVRTMATRIX _viewMatrix;
    PVRTMATRIX _projMatrix;
};

} //namespace lw



#endif //__LW_CAMERA_H__