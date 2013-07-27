#include "lwPrefix.h"
#include "lwTouch.h"
#include "lwTask.h"
#include "lwLog.h"

namespace lw{
    
    void Touch::set(float xx, float yy, float px, float py) {
        x = xx; y = yy; prevX = px; prevY = py;
    }
    
    namespace {
        std::map<int, Touch> _touchMap;
    }

	void touchBegan(int key, float x, float y) {
        std::map<int, Touch>::iterator it;
        Touch touch;
        touch.set(x, y, -1.f, -1.f);

        it = _touchMap.find(key);
        if (it == _touchMap.end()) {
            it = _touchMap.insert(std::make_pair(key, touch)).first;
            Task::touchBegan(it->second);
        }
    }
    
    void touchMoved(int key, float x, float y, float prevX, float prevY) {
        std::map<int, Touch>::iterator it = _touchMap.find(key);
        if (it != _touchMap.end()) {
            float prevX = it->second.x;
            float prevY = it->second.y;
            it->second.set(x, y, prevX, prevY);
            Task::touchMoved(it->second);
        }
    }
    
    void touchEnded(int key, float x, float y, float prevX, float prevY) {
        std::map<int, Touch>::iterator it = _touchMap.find(key);
        if (it != _touchMap.end()) {
            Task::touchEnded(it->second);
            _touchMap.erase(it);
        }
    }
    
    void touchCanceled(int key, float x, float y, float prevX, float prevY) {
        std::map<int, Touch>::iterator it = _touchMap.find(key);
        if (it != _touchMap.end()) {
            Task::touchCanceled(it->second);
        }
        _touchMap.erase(it);
    }

    Touch* findTouch(int key) {
        std::map<int, Touch>::iterator it = _touchMap.find(key);
        if (it == _touchMap.end()) {
            return 0;
        } else {
            return &(it->second);
        }
    }
    
    std::map<int, Touch>& getTouchMap() {
        return _touchMap;
    }

} //namespace lw