#ifndef __LW_STAGE_H__
#define __LW_STAGE_H__

namespace lw
{

class Stage
{
public:
    Stage();
    virtual ~Stage();
    
    void active();
    
private:
    virtual void vActive(){};
    virtual void vDisactive(){};
    virtual void vUpdate(){};
    virtual void vDraw(){};
    
//don't use below
public:
    static void create();
    static void destroy();
    static void update();
    static void draw();
};
    
} //namespace lw


#endif //__LW_STAGE_H__