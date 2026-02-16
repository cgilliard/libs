#ifndef _LIMITS_H
#define _LIMITS_H

#ifndef U8_MIN
#define U8_MIN ((u8)0x0)
#endif

#ifndef U16_MIN
#define U16_MIN ((u16)0x0)
#endif

#ifndef U32_MIN
#define U32_MIN ((u32)0x0)
#endif

#ifndef U64_MIN
#define U64_MIN ((u64)0x0)
#endif

#ifndef U8_MAX
#define U8_MAX ((u8)0xFF)
#endif

#ifndef U16_MAX
#define U16_MAX ((u16)0xFFFF)
#endif

#ifndef U32_MAX
#define U32_MAX ((u32)0xFFFFFFFF)
#endif

#ifndef U64_MAX
#define U64_MAX ((u64)0xFFFFFFFFFFFFFFFF)
#endif

#ifndef I8_MIN
#define I8_MIN ((i8)(-0x7F - 1))
#endif

#ifndef I16_MIN
#define I16_MIN ((i16)(-0x7FFF - 1))
#endif

#ifndef I32_MIN
#define I32_MIN ((i32)(-0x7FFFFFFF - 1))
#endif

#ifndef I64_MIN
#define I64_MIN ((i64)(-0x7FFFFFFFFFFFFFFF - 1))
#endif

#ifndef I8_MAX
#define I8_MAX ((i8)0x7F)
#endif

#ifndef I16_MAX
#define I16_MAX ((i16)0x7FFF)
#endif

#ifndef I32_MAX
#define I32_MAX ((i32)0x7FFFFFFF)
#endif

#ifndef I64_MAX
#define I64_MAX ((i64)0x7FFFFFFFFFFFFFFF)
#endif

#endif /* _LIMITS_H */

