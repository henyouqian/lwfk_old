#include "lwPrefix.h"
#include "lwTask.h"
#include "lwLog.h"

namespace lw {
    
    struct TaskMgr
    {
        std::list<Task*> running;
        std::list<Task*> adding;
        std::list<Task*> removing;
    };

    static TaskMgr *_pTaskMgr = NULL;

    void Task::init()
    {
        assert(!_pTaskMgr);
        _pTaskMgr = new TaskMgr();
    }

    void Task::quit()
    {
        assert(_pTaskMgr);
        delete _pTaskMgr;
        _pTaskMgr = NULL;
    }
    
    bool task_cmp (Task *first, Task *second)
    {
        return first->getPriority() < second->getPriority();
    }

    void Task::update()
    {
        if (!_pTaskMgr->removing.empty()) {
            std::list<Task*>::iterator it = _pTaskMgr->removing.begin();
            std::list<Task*>::iterator itend = _pTaskMgr->removing.end();
            for (; it != itend; ++it) {
                _pTaskMgr->running.remove(*it);
                (*it)->vStop();
                (*it)->_state = IDLE;
            }
            _pTaskMgr->removing.clear();
        }
        if (!_pTaskMgr->adding.empty()) {
            std::list<Task*>::iterator it = _pTaskMgr->adding.begin();
            std::list<Task*>::iterator itend = _pTaskMgr->adding.end();
            for (; it != itend; ++it) {
                (*it)->vStart();
                _pTaskMgr->running.push_back(*it);
                (*it)->_state = RUNNING;
            }
            _pTaskMgr->running.unique();
            _pTaskMgr->running.sort(task_cmp);
            _pTaskMgr->adding.clear();
        }
        
        std::list<Task*>::iterator it = _pTaskMgr->running.begin();
        std::list<Task*>::iterator itend = _pTaskMgr->running.end();
        for (; it != itend; ++it) {
            (*it)->vUpdate();
        }
    }

    void Task::draw()
    {
        std::list<Task*>::iterator it = _pTaskMgr->running.begin();
        std::list<Task*>::iterator itend = _pTaskMgr->running.end();
        for (; it != itend; ++it) {
            (*it)->vDraw();
        }
    }
    
    void Task::touchBegan(const Touch &touch) {
        std::list<Task*>::iterator it = _pTaskMgr->running.begin();
        std::list<Task*>::iterator itend = _pTaskMgr->running.end();
        for (; it != itend; ++it) {
            (*it)->vTouchBegan(touch);
        }
    }
    
    void Task::touchMoved(const Touch &touch) {
        std::list<Task*>::iterator it = _pTaskMgr->running.begin();
        std::list<Task*>::iterator itend = _pTaskMgr->running.end();
        for (; it != itend; ++it) {
            (*it)->vTouchMoved(touch);
        }
    }
    
    void Task::touchEnded(const Touch &touch) {
        std::list<Task*>::iterator it = _pTaskMgr->running.begin();
        std::list<Task*>::iterator itend = _pTaskMgr->running.end();
        for (; it != itend; ++it) {
            (*it)->vTouchEnded(touch);
        }
    }
    
    void Task::touchCanceled(const Touch &touch) {
        std::list<Task*>::iterator it = _pTaskMgr->running.begin();
        std::list<Task*>::iterator itend = _pTaskMgr->running.end();
        for (; it != itend; ++it) {
            (*it)->vTouchCanceled(touch);
        }
    }
    
    //======================================
    Task::Task()
    :_priority(0.f), _state(IDLE)
    {
        
    }

    Task::~Task()
    {
        
    }
    
    void Task::setPriority(float priority)
    {
        _priority = priority;
    }
    
    float Task::getPriority() {
        return _priority;
    }
        
    void Task::start()
    {
        if (_state == IDLE) {
            _pTaskMgr->adding.push_back(this);
            _state = PENDING;
        } else {
            lwerror("wrong state");
        }
    }

    void Task::stop()
    {
        if (_state == RUNNING) {
            _pTaskMgr->removing.push_back(this);
            _state = PENDING;
        } else {
            lwerror("wrong state");
    
        }
    }

} //namespace lw
