// sal_compat.h - SAL注解兼容性头文件
// 解决DirectX 9 SDK与新Windows SDK的SAL注解冲突

#ifndef SAL_COMPAT_H
#define SAL_COMPAT_H

// 禁用SAL注解
#ifndef _USE_DECLSPECS_FOR_SAL
#define _USE_DECLSPECS_FOR_SAL 0
#endif

#ifndef _USE_ATTRIBUTES_FOR_SAL
#define _USE_ATTRIBUTES_FOR_SAL 0
#endif

// 定义所有缺失的SAL宏为空
#ifndef __RPC__out_xcount_part
#define __RPC__out_xcount_part(x,y)
#endif

#ifndef __RPC__in_xcount_full
#define __RPC__in_xcount_full(x)
#endif

#ifndef __RPC__inout_xcount
#define __RPC__inout_xcount(x)
#endif

#ifndef __RPC__in_xcount
#define __RPC__in_xcount(x)
#endif

#ifndef __RPC__deref_out_xcount_part
#define __RPC__deref_out_xcount_part(x,y)
#endif

#ifndef __RPC__deref_out_xcount_full
#define __RPC__deref_out_xcount_full(x)
#endif

#ifndef __RPC__in_range
#define __RPC__in_range(x,y)
#endif

#ifndef __RPC__out_xcount_full
#define __RPC__out_xcount_full(x)
#endif

#ifndef __RPC__deref_out_xcount
#define __RPC__deref_out_xcount(x)
#endif

#ifndef __RPC__out_xcount
#define __RPC__out_xcount(x)
#endif

#ifndef __RPC__inout_xcount_full
#define __RPC__inout_xcount_full(x)
#endif

#ifndef __RPC__inout_xcount_part
#define __RPC__inout_xcount_part(x,y)
#endif

#ifndef __RPC__deref_inout_xcount
#define __RPC__deref_inout_xcount(x)
#endif

#ifndef __RPC__deref_inout_xcount_full
#define __RPC__deref_inout_xcount_full(x)
#endif

#ifndef __RPC__deref_inout_xcount_part
#define __RPC__deref_inout_xcount_part(x,y)
#endif

#endif // SAL_COMPAT_H
