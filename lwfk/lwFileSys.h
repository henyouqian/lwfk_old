#ifndef __LW_FILE_SYS_H__
#define __LW_FILE_SYS_H__

#include <string>

namespace lw
{


class _f
{
public:
    _f(const char* filename);
    operator const char*();
    operator bool();
    
private:
    std::string _path;
};

class _fdoc{
public:
    _fdoc(const char* filename);
    operator const char*();
    
private:
    std::string _path;
};

	
} //namespace lw


#endif //__LW_FILE_SYS_H__