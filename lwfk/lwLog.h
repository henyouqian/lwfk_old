#ifndef __LW_LOG_H__
#define	__LW_LOG_H__

#ifdef NO_LWLOG
#   define lwinfo(_e) ((void)0)
#   define lwerror(_e) ((void)0)
#else
#   define FLE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#   define lwinfo(_e) std::cout<<"Info: " << FLE << " - " << __LINE__ << " - " << __FUNCTION__ << ":\n" << _e << "\n";
#   define lwerror(_e) std::cout<<"Error: " << FLE << " - " << __LINE__ << " - " <<  __FUNCTION__ << ":\n" << _e << "\n";
#endif

#endif //__LW_LOG_H__