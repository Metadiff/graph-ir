//
// Created by alex on 29/09/16.
//

#ifndef METADIFF_CORE_ENUMS_H
#define METADIFF_CORE_ENUMS_H
namespace md{
    namespace core{
        /** Data type of the storage */
        enum dataType{
            /** 8 bit boolean */
                    b8 = 0,
            /** 8 bit unsigned integer */
                    u8 = 1,
            /** 16 bit unsigned integer */
                    u16 = 2,
            /** 32 bit unsigned integer */
                    u32 = 3,
            /** 64 bit unsigned integer */
                    u64 = 4,
            /** 8 bit signed integer */
                    i8 = 5,
            /** 16 bit signed integer */
                    i16 = 6,
            /** 32 bit signed integer */
                    i32 = 7,
            /** 64 bit signed integer */
                    i64 = 8,
            /** 8 bit floating point */
                    f8 = 9,
            /** 16 bit floating point */
                    f16 = 10,
            /** 32 bit floating point */
                    f32 = 11,
            /** 64 bit floating point */
                    f64 = 12
        };

        /** This will be used for multy-device support */
        enum deviceType {
            /** Represents a host with one or more CPUs */
                    CPU = 0,
            /** Represents a single GPU */
                    GPU = 1
        };

//        /** Each compute Node is one of these types */
//        enum nodeType {
//            /** The node represents a constant */
//                    CONSTANT = 0,
//            /** The node is derived from a constant, trough operators, but have no dependencies on any inputs */
//                    CONSTANT_DERIVED = 1,
//            /** The node is derived from an input, trough one operators, but in a non-differentiable manner */
//                    INPUT_DERIVED_NON_DIFF = 2,
//            /** The node represents an input */
//                    INPUT = 3,
//            /** The node is derived from an input, trough one operators */
//                    INPUT_DERIVED = 4,
//
//        };

        /** An error policy defines how should we behave when an error occurs */
        enum errorPolicy {
            /** Does nothing */
                    QUIET = 0,
            /** Prints a warning */
                    WARN = 1,
            /** Throws an error */
                    RAISE = 2
        };

        /** The default promotion table for data types */
        const dataType default_promotion_table[13][13] = {
                {b8,u8,u16,u32,u64,i8,i16,i32,i64,f8,f16,f32,f64},
                {u8,u8,u16,u32,u64,i8,i16,i32,i64,f8,f16,f32,f64},
                {u16,u16,u16,u32,u64,i16,i16,i32,i64,f16,f16,f32,f64},
                {u32,u32,u32,u32,u64,i32,i32,i32,i64,f32,f32,f32,f64},
                {u64,u64,u64,u64,u64,i64,i64,i64,i64,f64,f64,f64,f64},
                {i8,i8,i16,i32,i64,i8,i16,i32,i64,f8,f16,f32,f64},
                {i16,i16,i16,i32,i64,i16,i16,i32,i64,f16,f16,f32,f64},
                {i32,i32,i32,i32,i64,i32,i32,i32,i64,f32,f32,f32,f64},
                {i64,i64,i64,i64,i64,i64,i64,i64,i64,f64,f64,f64,f64},
                {f8,f8,f16,f32,f64,f8,f16,f32,f64,f8,f16,f32,f64},
                {f16,f16,f16,f32,f64,f16,f16,f32,f64,f16,f16,f32,f64},
                {f32,f32,f32,f32,f64,f32,f32,f32,f64,f32,f32,f32,f64},
                {f64,f64,f64,f64,f64,f64,f64,f64,f64,f64,f64,f64,f64}
        };

        /** For Operators wich can auto infer arguments */
        const short auto_infer = 100;

        /** The three monitor options */
        enum monitorRole {
            /** Do nothing */
            SILENT = 0,
            /** Print the statement */
            PRINT = 1,
            /** Add to returned monitors */
            RETURN = 2,
            /** Log to the logging directory */
            LOG = 3
        };
    }
}
#endif //METADIFF_CORE_ENUMS_H
