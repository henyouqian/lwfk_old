#ifndef __LW_TOUCH_H__
#define __LW_TOUCH_H__

namespace lw{
	
	struct Touch{
		float x, y, prevX, prevY;
        void set(float x, float y, float prevX, float prevY);
	};
    
    void touchBegan(int key, float x, float y);
    void touchMoved(int key, float x, float y, float prevX, float prevY);
    void touchEnded(int key, float x, float y, float prevX, float prevY);
    void touchCanceled(int key, float x, float y, float prevX, float prevY);

    lw::Touch* findTouch(int key);
    std::map<int, Touch>& getTouchMap();

} //namespace lw

#endif //__LW_EVENT_H__