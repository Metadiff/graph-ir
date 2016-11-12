//
// Created by alex on 29/09/16.
//

#ifndef METADIFF_GRAPH_IR_ENUMS_H
#define METADIFF_GRAPH_IR_ENUMS_H
namespace md{
    namespace gir{
        /** Fundamental types of possible variables in arrays */
        enum FundamentalType {
            /** A boolean variable */
            BOOLEAN = 0,
            /** An unsigned integer variable */
            UNSIGNED_INT = 1,
            /** A signed integer variable */
            SIGNED_INT = 2,
            /** A float variable */
            FLOAT = 3,
            /** A complex variable */
            COMPLEX = 4
        };

        /** Possible precision of variables */
        enum Precision {
            /** 8 bit precision */
                    p8 = 0,
            /** 16 bit precision */
                    p16 = 1,
            /** 16 bit precision */
                    p32 = 2,
            /** 16 bit precision */
                    p64 = 3
        };

        /** A data type is a combination of a fundamental type and precision */
        class DataType{
        public:
            FundamentalType type;
            Precision precision;

            DataType(): type(FLOAT), precision(p32) {};

            DataType(FundamentalType const type, Precision const precision):
                    type(type), precision(precision) {
                if(type == BOOLEAN and precision != p8){
                    throw -1;
                }
            };

            DataType(DataType const & data_type):
                    type(data_type.type), precision(data_type.precision) {};
        };

        inline bool operator==(DataType const & data_type1, DataType const & data_type2){
            return data_type1.type == data_type2.type and data_type1.precision == data_type2.precision;
        }

        inline bool operator!=(DataType const & data_type1, DataType const & data_type2){
            return data_type1.type != data_type2.type or data_type1.precision != data_type2.precision;
        }

        // Define common alias for types
        const DataType b8 (BOOLEAN, p8);
        const DataType u8 (UNSIGNED_INT, p8);
        const DataType u16 (UNSIGNED_INT, p16);
        const DataType u32 (UNSIGNED_INT, p32);
        const DataType u64 (UNSIGNED_INT, p64);
        const DataType i8 (SIGNED_INT, p8);
        const DataType i16 (SIGNED_INT, p16);
        const DataType i32 (SIGNED_INT, p32);
        const DataType i64 (SIGNED_INT, p64);
        const DataType f8 (FLOAT, p8);
        const DataType f16 (FLOAT, p16);
        const DataType f32 (FLOAT, p32);
        const DataType f64 (FLOAT, p64);
        const DataType c8 (COMPLEX, p8);
        const DataType c16 (COMPLEX, p16);
        const DataType c32 (COMPLEX, p32);
        const DataType c64 (COMPLEX, p64);

//        /** Data type of the storage */
//        enum DataType{
//            /** 8 bit boolean */
//                    b8 = 0,
//            /** 8 bit unsigned integer */
//                    u8 = 1,
//            /** 16 bit unsigned integer */
//                    u16 = 2,
//            /** 32 bit unsigned integer */
//                    u32 = 3,
//            /** 64 bit unsigned integer */
//                    u64 = 4,
//            /** 8 bit signed integer */
//                    i8 = 5,
//            /** 16 bit signed integer */
//                    i16 = 6,
//            /** 32 bit signed integer */
//                    i32 = 7,
//            /** 64 bit signed integer */
//                    i64 = 8,
//            /** 8 bit floating point */
//                    f8 = 9,
//            /** 16 bit floating point */
//                    f16 = 10,
//            /** 32 bit floating point */
//                    f32 = 11,
//            /** 64 bit floating point */
//                    f64 = 12
//        };

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

//        /** The default promotion table for data types */
//        const DataType default_promotion_table[13][13] = {
//                {b8,u8,u16,u32,u64,i8,i16,i32,i64,f8,f16,f32,f64},
//                {u8,u8,u16,u32,u64,i8,i16,i32,i64,f8,f16,f32,f64},
//                {u16,u16,u16,u32,u64,i16,i16,i32,i64,f16,f16,f32,f64},
//                {u32,u32,u32,u32,u64,i32,i32,i32,i64,f32,f32,f32,f64},
//                {u64,u64,u64,u64,u64,i64,i64,i64,i64,f64,f64,f64,f64},
//                {i8,i8,i16,i32,i64,i8,i16,i32,i64,f8,f16,f32,f64},
//                {i16,i16,i16,i32,i64,i16,i16,i32,i64,f16,f16,f32,f64},
//                {i32,i32,i32,i32,i64,i32,i32,i32,i64,f32,f32,f32,f64},
//                {i64,i64,i64,i64,i64,i64,i64,i64,i64,f64,f64,f64,f64},
//                {f8,f8,f16,f32,f64,f8,f16,f32,f64,f8,f16,f32,f64},
//                {f16,f16,f16,f32,f64,f16,f16,f32,f64,f16,f16,f32,f64},
//                {f32,f32,f32,f32,f64,f32,f32,f32,f64,f32,f32,f32,f64},
//                {f64,f64,f64,f64,f64,f64,f64,f64,f64,f64,f64,f64,f64}
//        };
    }
}
#endif //METADIFF_GRAPH_IR_ENUMS_H
