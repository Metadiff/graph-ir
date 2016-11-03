//
// Created by alex on 29/09/16.
//

#ifndef METADIFF_GRAPH_IR_ENUMS_H
#define METADIFF_GRAPH_IR_ENUMS_H
namespace md{
    namespace gir{
        /** Data type of the storage */
        enum DataType{
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

        /** Type of the device for execution */
        enum DeviceType {
            /** Represents a host with one or more CPUs */
                    CPU = 0,
            /** Represents a single GPU */
                    GPU = 1
        };

        /** A policy defines beahviour for certain errors */
        enum Policy {
            /** Does nothing */
                    QUIET = 0,
            /** Prints a warning */
                    WARN = 1,
            /** Throws an error */
                    RAISE = 2
        };

        /** Positivity contraints */
        enum Positivity {
            /** Elements are not constrained */
                    NON_CONSTRAINED = 0,
            /** Strictly > 0 */
                    POSITIVE = 1,
            /** Strictly >= 0 */
                    NON_NEGATIVE = 2,
            /** Strictly < 0 */
                    NEGATIVE = 3,
            /** Strictly <= 0 */
                    NON_POSITIVE = 4
        };

        /** Postivity constraints for matrices */
        enum MatrixPositivity {
            /** Indefinite */
                    INDEFINITE = 0,
            /** Positive definite */
                    POSITIVE_DEFINITE = 1,
            /** Positive semi-definite */
                    POSITIVE_SEMI_DEFINITE = 2,
            /** Negative definite */
                    NEGATIVE_DEFINITE = 3,
            /** Negative semi-definite */
                    NEGATIVE_SEMI_DEFINITE = 4
        };

        /** Symmetry constraints for matrices */
        enum MatrixSymmetry {
            /** Neither symmetric or skew-symmetric */
                    NON_SYMMETRIC = 0,
            /** Symmetric */
                    SYMMETRIC = 1,
            /** Skew-symmetric */
                    SKEW_SYMMETRIC = 2
        };

        /** Partial fill for matrices */
        enum MatrixFill {
            /** None of below options */
                    NON_STRUCTURED_FILL = 0,
            /** Diagonal matrix */
                    DIAGONAL = 1,
            /** Tri-diagonal matrix */
                    TRI_DIAGONAL = 2,
            /** Strictly lower triangular */
                    STRICTLY_LOWER_TRIANGULAR = 3,
            /** Strictly upper triangualr */
                    STRICTLY_UPPER_TRIANGULAR = 4,
            /** Lower triangular */
                    LOWER_TRIANGULAR = 5,
            /** Upper triangualr */
                    UPPER_TRIANGULAR = 6
        };

        /** The default promotion table for data types */
        const DataType default_promotion_table[13][13] = {
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
    }
}
#endif //METADIFF_GRAPH_IR_ENUMS_H
