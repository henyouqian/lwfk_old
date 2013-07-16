#ifndef __LW_MESH_H__
#define __LW_MESH_H__


namespace lw {
    
    struct VertexInfo {
        VertexInfo():stride(0), ptr(NULL) {}
        void set(GLsizei _stride, GLvoid* _ptr) {
            stride = _stride;
            ptr = _ptr;
        }
        
        GLsizei stride;
        const GLvoid* ptr;  //0 if vbo
    };
    
    enum {
        UV_SIZE = 4,
    };
    
    struct Mesh {
        Mesh():faceCount(0) {}
        VertexInfo position;
        VertexInfo normal;
        VertexInfo uv[UV_SIZE];
        GLsizei faceCount;
    };
    
    
}//namespace lw

#endif //__LW_MESH_H__