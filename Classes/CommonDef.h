#ifndef _COMMON_DEF_H
#define _COMMON_DEF_H

#define CHECK_NULL(_x_) if (!_x_){goto EXIT;}

#ifndef SAFE_DELETE 
#define SAFE_DELETE(p) { if(p) { delete (p); (p) = NULL; } } 
#endif 
#ifndef SAFE_ARRAY_DELETE 
#define SAFE_ARRAY_DELETE(p) { if(p) { delete [] (p); (p) = NULL; } } 
#endif 

#endif /*_COMMON_DEF_H*/
