////////////////////////////////////////////////////////////////////////////////
//
// UnpackageUnorderedParameters.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description:
//
////////////////////////////////////////////////////////////////////////////////
#ifndef UNPACKAGE_UNORDERED_PARAMETERS_H_
#define UNPACKAGE_UNORDERED_PARAMETERS_H_
#include "ByteBuffer.h"
#include "UnpackageParameters.h"

#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) a ## b


/**
* Count the number of arguments. Up to 60 arguments.
*/
#define VA_NUM_PARAMS(...) VA_NUM_PARAMS_IMPL(__VA_ARGS__,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)
#define VA_NUM_PARAMS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,N,...) N

#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,NAME,...) NAME

#define FEP_1(WHAT, X) WHAT(X)
#define FEP_2(WHAT, X,...) WHAT(X)FEP_1(WHAT, __VA_ARGS__)
#define FEP_3(WHAT, X,...) WHAT(X)FEP_2(WHAT, __VA_ARGS__)
#define FEP_4(WHAT, X,...) WHAT(X)FEP_3(WHAT, __VA_ARGS__)
#define FEP_5(WHAT, X,...) WHAT(X)FEP_4(WHAT, __VA_ARGS__)
#define FEP_6(WHAT, X,...) WHAT(X)FEP_5(WHAT, __VA_ARGS__)
#define FEP_7(WHAT, X,...) WHAT(X)FEP_6(WHAT, __VA_ARGS__)
#define FEP_8(WHAT, X,...) WHAT(X)FEP_7(WHAT, __VA_ARGS__)
#define FEP_9(WHAT, X,...) WHAT(X)FEP_8(WHAT, __VA_ARGS__)
#define FEP_10(WHAT, X,...) WHAT(X)FEP_9(WHAT, __VA_ARGS__)
#define FEP_11(WHAT, X,...) WHAT(X)FEP_10(WHAT, __VA_ARGS__)
#define FEP_12(WHAT, X,...) WHAT(X)FEP_11(WHAT, __VA_ARGS__)
#define FEP_13(WHAT, X,...) WHAT(X)FEP_12(WHAT, __VA_ARGS__)
#define FEP_14(WHAT, X,...) WHAT(X)FEP_13(WHAT, __VA_ARGS__)
#define FEP_15(WHAT, X,...) WHAT(X)FEP_14(WHAT, __VA_ARGS__)
#define FEP_16(WHAT, X,...) WHAT(X)FEP_15(WHAT, __VA_ARGS__)
#define FEP_17(WHAT, X,...) WHAT(X)FEP_16(WHAT, __VA_ARGS__)
#define FEP_18(WHAT, X,...) WHAT(X)FEP_17(WHAT, __VA_ARGS__)
#define FEP_19(WHAT, X,...) WHAT(X)FEP_18(WHAT, __VA_ARGS__)
#define FEP_20(WHAT, X,...) WHAT(X)FEP_19(WHAT, __VA_ARGS__)
#define FEP_21(WHAT, X,...) WHAT(X)FEP_20(WHAT, __VA_ARGS__)
#define FEP_22(WHAT, X,...) WHAT(X)FEP_21(WHAT, __VA_ARGS__)
#define FEP_23(WHAT, X,...) WHAT(X)FEP_22(WHAT, __VA_ARGS__)
#define FEP_24(WHAT, X,...) WHAT(X)FEP_23(WHAT, __VA_ARGS__)
#define FEP_25(WHAT, X,...) WHAT(X)FEP_24(WHAT, __VA_ARGS__)
#define FEP_26(WHAT, X,...) WHAT(X)FEP_25(WHAT, __VA_ARGS__)
#define FEP_27(WHAT, X,...) WHAT(X)FEP_26(WHAT, __VA_ARGS__)
#define FEP_28(WHAT, X,...) WHAT(X)FEP_27(WHAT, __VA_ARGS__)
#define FEP_29(WHAT, X,...) WHAT(X)FEP_28(WHAT, __VA_ARGS__)
#define FEP_30(WHAT, X,...) WHAT(X)FEP_29(WHAT, __VA_ARGS__)
#define FEP_31(WHAT, X,...) WHAT(X)FEP_30(WHAT, __VA_ARGS__)
#define FEP_32(WHAT, X,...) WHAT(X)FEP_31(WHAT, __VA_ARGS__)
#define FEP_33(WHAT, X,...) WHAT(X)FEP_32(WHAT, __VA_ARGS__)
#define FEP_34(WHAT, X,...) WHAT(X)FEP_33(WHAT, __VA_ARGS__)
#define FEP_35(WHAT, X,...) WHAT(X)FEP_34(WHAT, __VA_ARGS__)
#define FEP_36(WHAT, X,...) WHAT(X)FEP_35(WHAT, __VA_ARGS__)
#define FEP_37(WHAT, X,...) WHAT(X)FEP_36(WHAT, __VA_ARGS__)
#define FEP_38(WHAT, X,...) WHAT(X)FEP_37(WHAT, __VA_ARGS__)
#define FEP_39(WHAT, X,...) WHAT(X)FEP_38(WHAT, __VA_ARGS__)
#define FEP_40(WHAT, X,...) WHAT(X)FEP_39(WHAT, __VA_ARGS__)
#define FEP_41(WHAT, X,...) WHAT(X)FEP_40(WHAT, __VA_ARGS__)
#define FEP_42(WHAT, X,...) WHAT(X)FEP_41(WHAT, __VA_ARGS__)
#define FEP_43(WHAT, X,...) WHAT(X)FEP_42(WHAT, __VA_ARGS__)
#define FEP_44(WHAT, X,...) WHAT(X)FEP_43(WHAT, __VA_ARGS__)
#define FEP_45(WHAT, X,...) WHAT(X)FEP_44(WHAT, __VA_ARGS__)
#define FEP_46(WHAT, X,...) WHAT(X)FEP_45(WHAT, __VA_ARGS__)
#define FEP_47(WHAT, X,...) WHAT(X)FEP_46(WHAT, __VA_ARGS__)
#define FEP_48(WHAT, X,...) WHAT(X)FEP_47(WHAT, __VA_ARGS__)
#define FEP_49(WHAT, X,...) WHAT(X)FEP_48(WHAT, __VA_ARGS__)
#define FEP_50(WHAT, X,...) WHAT(X)FEP_49(WHAT, __VA_ARGS__)
#define FEP_51(WHAT, X,...) WHAT(X)FEP_50(WHAT, __VA_ARGS__)
#define FEP_52(WHAT, X,...) WHAT(X)FEP_51(WHAT, __VA_ARGS__)
#define FEP_53(WHAT, X,...) WHAT(X)FEP_52(WHAT, __VA_ARGS__)
#define FEP_54(WHAT, X,...) WHAT(X)FEP_53(WHAT, __VA_ARGS__)
#define FEP_55(WHAT, X,...) WHAT(X)FEP_54(WHAT, __VA_ARGS__)
#define FEP_56(WHAT, X,...) WHAT(X)FEP_55(WHAT, __VA_ARGS__)
#define FEP_57(WHAT, X,...) WHAT(X)FEP_56(WHAT, __VA_ARGS__)
#define FEP_58(WHAT, X,...) WHAT(X)FEP_57(WHAT, __VA_ARGS__)
#define FEP_59(WHAT, X,...) WHAT(X)FEP_58(WHAT, __VA_ARGS__)
#define FEP_60(WHAT, X,...) WHAT(X)FEP_59(WHAT, __VA_ARGS__)

#define FOR_EACH(action,...) GET_MACRO(__VA_ARGS__,FEP_60, FEP_59, FEP_58, FEP_57, FEP_56, FEP_55, FEP_54, FEP_53, FEP_52, FEP_51, FEP_50, FEP_49, FEP_48, FEP_47, FEP_46, FEP_45, FEP_44, FEP_43, FEP_42, FEP_41, FEP_40, FEP_39, FEP_38, FEP_37, FEP_36, FEP_35, FEP_34, FEP_33, FEP_32, FEP_31, FEP_30, FEP_29, FEP_28, FEP_27, FEP_26, FEP_25, FEP_24, FEP_23, FEP_22, FEP_21, FEP_20, FEP_19, FEP_18, FEP_17, FEP_16, FEP_15, FEP_14, FEP_13, FEP_12, FEP_11, FEP_10, FEP_9, FEP_8, FEP_7, FEP_6, FEP_5, FEP_4, FEP_3, FEP_2, FEP_1)(action,__VA_ARGS__)

//2 param
#define FEP2_2(WHAT, X, Y) WHAT(X,Y)
#define FEP2_4(WHAT, X, Y,...) WHAT(X,Y)FEP2_2(WHAT, __VA_ARGS__)
#define FEP2_6(WHAT, X, Y,...) WHAT(X,Y)FEP2_4(WHAT, __VA_ARGS__)
#define FEP2_8(WHAT, X, Y,...) WHAT(X,Y)FEP2_6(WHAT, __VA_ARGS__)
#define FEP2_10(WHAT, X, Y,...) WHAT(X,Y)FEP2_8(WHAT, __VA_ARGS__)
#define FEP2_12(WHAT, X, Y,...) WHAT(X,Y)FEP2_10(WHAT, __VA_ARGS__)
#define FEP2_14(WHAT, X, Y,...) WHAT(X,Y)FEP2_12(WHAT, __VA_ARGS__)
#define FEP2_16(WHAT, X, Y,...) WHAT(X,Y)FEP2_14(WHAT, __VA_ARGS__)
#define FEP2_18(WHAT, X, Y,...) WHAT(X,Y)FEP2_16(WHAT, __VA_ARGS__)
#define FEP2_20(WHAT, X, Y,...) WHAT(X,Y)FEP2_18(WHAT, __VA_ARGS__)
#define FEP2_22(WHAT, X, Y,...) WHAT(X,Y)FEP2_20(WHAT, __VA_ARGS__)
#define FEP2_24(WHAT, X, Y,...) WHAT(X,Y)FEP2_22(WHAT, __VA_ARGS__)
#define FEP2_26(WHAT, X, Y,...) WHAT(X,Y)FEP2_24(WHAT, __VA_ARGS__)
#define FEP2_28(WHAT, X, Y,...) WHAT(X,Y)FEP2_26(WHAT, __VA_ARGS__)
#define FEP2_30(WHAT, X, Y,...) WHAT(X,Y)FEP2_28(WHAT, __VA_ARGS__)
#define FEP2_32(WHAT, X, Y,...) WHAT(X,Y)FEP2_30(WHAT, __VA_ARGS__)
#define FEP2_34(WHAT, X, Y,...) WHAT(X,Y)FEP2_32(WHAT, __VA_ARGS__)
#define FEP2_36(WHAT, X, Y,...) WHAT(X,Y)FEP2_34(WHAT, __VA_ARGS__)
#define FEP2_38(WHAT, X, Y,...) WHAT(X,Y)FEP2_36(WHAT, __VA_ARGS__)
#define FEP2_40(WHAT, X, Y,...) WHAT(X,Y)FEP2_38(WHAT, __VA_ARGS__)
#define FEP2_42(WHAT, X, Y,...) WHAT(X,Y)FEP2_40(WHAT, __VA_ARGS__)
#define FEP2_44(WHAT, X, Y,...) WHAT(X,Y)FEP2_42(WHAT, __VA_ARGS__)
#define FEP2_46(WHAT, X, Y,...) WHAT(X,Y)FEP2_44(WHAT, __VA_ARGS__)
#define FEP2_48(WHAT, X, Y,...) WHAT(X,Y)FEP2_46(WHAT, __VA_ARGS__)
#define FEP2_50(WHAT, X, Y,...) WHAT(X,Y)FEP2_48(WHAT, __VA_ARGS__)
#define FEP2_52(WHAT, X, Y,...) WHAT(X,Y)FEP2_50(WHAT, __VA_ARGS__)
#define FEP2_54(WHAT, X, Y,...) WHAT(X,Y)FEP2_52(WHAT, __VA_ARGS__)
#define FEP2_56(WHAT, X, Y,...) WHAT(X,Y)FEP2_54(WHAT, __VA_ARGS__)
#define FEP2_58(WHAT, X, Y,...) WHAT(X,Y)FEP2_56(WHAT, __VA_ARGS__)
#define FEP2_60(WHAT, X, Y,...) WHAT(X,Y)FEP2_58(WHAT, __VA_ARGS__)
#define FOR_EACH_2(action,...) GET_MACRO(__VA_ARGS__,FEP2_60,FEP2_60,FEP2_58,FEP2_58,FEP2_56,FEP2_56,FEP2_54,FEP2_54,FEP2_52,FEP2_52,FEP2_50,FEP2_50,FEP2_48,FEP2_48,FEP2_46,FEP2_46,FEP2_44,FEP2_44,FEP2_42,FEP2_42,FEP2_40,FEP2_40,FEP2_38,FEP2_38,FEP2_36,FEP2_36,FEP2_34,FEP2_34,FEP2_32,FEP2_32,FEP2_30,FEP2_30,FEP2_28,FEP2_28,FEP2_26,FEP2_26,FEP2_24,FEP2_24,FEP2_22,FEP2_22,FEP2_20,FEP2_20,FEP2_18,FEP2_18,FEP2_16,FEP2_16,FEP2_14,FEP2_14,FEP2_12,FEP2_12,FEP2_10,FEP2_10,FEP2_8,FEP2_8,FEP2_6,FEP2_6,FEP2_4,FEP2_4,FEP2_2,FEP2_2)(action,__VA_ARGS__)

#define FEPN_1(WHAT, X) WHAT##1(X)
#define FEPN_2(WHAT,X,...) WHAT##2(X)FEPN_1(WHAT, __VA_ARGS__)
#define FEPN_3(WHAT,X,...) WHAT##3(X)FEPN_2(WHAT, __VA_ARGS__)
#define FEPN_4(WHAT,X,...) WHAT##4(X)FEPN_3(WHAT, __VA_ARGS__)
#define FEPN_5(WHAT,X,...) WHAT##5(X)FEPN_4(WHAT, __VA_ARGS__)
#define FEPN_6(WHAT,X,...) WHAT##6(X)FEPN_5(WHAT, __VA_ARGS__)
#define FEPN_7(WHAT,X,...) WHAT##7(X)FEPN_6(WHAT, __VA_ARGS__)
#define FEPN_8(WHAT,X,...) WHAT##8(X)FEPN_7(WHAT, __VA_ARGS__)
#define FEPN_9(WHAT,X,...) WHAT##9(X)FEPN_8(WHAT, __VA_ARGS__)
#define FEPN_10(WHAT,X,...) WHAT##10(X)FEPN_9(WHAT, __VA_ARGS__)
#define FEPN_11(WHAT,X,...) WHAT##11(X)FEPN_10(WHAT, __VA_ARGS__)
#define FEPN_12(WHAT,X,...) WHAT##12(X)FEPN_11(WHAT, __VA_ARGS__)
#define FEPN_13(WHAT,X,...) WHAT##13(X)FEPN_12(WHAT, __VA_ARGS__)
#define FEPN_14(WHAT,X,...) WHAT##14(X)FEPN_13(WHAT, __VA_ARGS__)
#define FEPN_15(WHAT,X,...) WHAT##15(X)FEPN_14(WHAT, __VA_ARGS__)
#define FEPN_16(WHAT,X,...) WHAT##16(X)FEPN_15(WHAT, __VA_ARGS__)
#define FEPN_17(WHAT,X,...) WHAT##17(X)FEPN_16(WHAT, __VA_ARGS__)
#define FEPN_18(WHAT,X,...) WHAT##18(X)FEPN_17(WHAT, __VA_ARGS__)
#define FEPN_19(WHAT,X,...) WHAT##19(X)FEPN_18(WHAT, __VA_ARGS__)
#define FEPN_20(WHAT,X,...) WHAT##20(X)FEPN_19(WHAT, __VA_ARGS__)
#define FEPN_21(WHAT,X,...) WHAT##21(X)FEPN_20(WHAT, __VA_ARGS__)
#define FEPN_22(WHAT,X,...) WHAT##22(X)FEPN_21(WHAT, __VA_ARGS__)
#define FEPN_23(WHAT,X,...) WHAT##23(X)FEPN_22(WHAT, __VA_ARGS__)
#define FEPN_24(WHAT,X,...) WHAT##24(X)FEPN_23(WHAT, __VA_ARGS__)
#define FEPN_25(WHAT,X,...) WHAT##25(X)FEPN_24(WHAT, __VA_ARGS__)
#define FEPN_26(WHAT,X,...) WHAT##26(X)FEPN_25(WHAT, __VA_ARGS__)
#define FEPN_27(WHAT,X,...) WHAT##27(X)FEPN_26(WHAT, __VA_ARGS__)
#define FEPN_28(WHAT,X,...) WHAT##28(X)FEPN_27(WHAT, __VA_ARGS__)
#define FEPN_29(WHAT,X,...) WHAT##29(X)FEPN_28(WHAT, __VA_ARGS__)
#define FEPN_30(WHAT,X,...) WHAT##30(X)FEPN_29(WHAT, __VA_ARGS__)
#define FEPN_31(WHAT,X,...) WHAT##31(X)FEPN_30(WHAT, __VA_ARGS__)
#define FEPN_32(WHAT,X,...) WHAT##32(X)FEPN_31(WHAT, __VA_ARGS__)
#define FEPN_33(WHAT,X,...) WHAT##33(X)FEPN_32(WHAT, __VA_ARGS__)
#define FEPN_34(WHAT,X,...) WHAT##34(X)FEPN_33(WHAT, __VA_ARGS__)
#define FEPN_35(WHAT,X,...) WHAT##35(X)FEPN_34(WHAT, __VA_ARGS__)
#define FEPN_36(WHAT,X,...) WHAT##36(X)FEPN_35(WHAT, __VA_ARGS__)
#define FEPN_37(WHAT,X,...) WHAT##37(X)FEPN_36(WHAT, __VA_ARGS__)
#define FEPN_38(WHAT,X,...) WHAT##38(X)FEPN_37(WHAT, __VA_ARGS__)
#define FEPN_39(WHAT,X,...) WHAT##39(X)FEPN_38(WHAT, __VA_ARGS__)
#define FEPN_40(WHAT,X,...) WHAT##40(X)FEPN_39(WHAT, __VA_ARGS__)
#define FEPN_41(WHAT,X,...) WHAT##41(X)FEPN_40(WHAT, __VA_ARGS__)
#define FEPN_42(WHAT,X,...) WHAT##42(X)FEPN_41(WHAT, __VA_ARGS__)
#define FEPN_43(WHAT,X,...) WHAT##43(X)FEPN_42(WHAT, __VA_ARGS__)
#define FEPN_44(WHAT,X,...) WHAT##44(X)FEPN_43(WHAT, __VA_ARGS__)
#define FEPN_45(WHAT,X,...) WHAT##45(X)FEPN_44(WHAT, __VA_ARGS__)
#define FEPN_46(WHAT,X,...) WHAT##46(X)FEPN_45(WHAT, __VA_ARGS__)
#define FEPN_47(WHAT,X,...) WHAT##47(X)FEPN_46(WHAT, __VA_ARGS__)
#define FEPN_48(WHAT,X,...) WHAT##48(X)FEPN_47(WHAT, __VA_ARGS__)
#define FEPN_49(WHAT,X,...) WHAT##49(X)FEPN_48(WHAT, __VA_ARGS__)
#define FEPN_50(WHAT,X,...) WHAT##50(X)FEPN_49(WHAT, __VA_ARGS__)
#define FEPN_51(WHAT,X,...) WHAT##51(X)FEPN_50(WHAT, __VA_ARGS__)
#define FEPN_52(WHAT,X,...) WHAT##52(X)FEPN_51(WHAT, __VA_ARGS__)
#define FEPN_53(WHAT,X,...) WHAT##53(X)FEPN_52(WHAT, __VA_ARGS__)
#define FEPN_54(WHAT,X,...) WHAT##54(X)FEPN_53(WHAT, __VA_ARGS__)
#define FEPN_55(WHAT,X,...) WHAT##55(X)FEPN_54(WHAT, __VA_ARGS__)
#define FEPN_56(WHAT,X,...) WHAT##56(X)FEPN_55(WHAT, __VA_ARGS__)
#define FEPN_57(WHAT,X,...) WHAT##57(X)FEPN_56(WHAT, __VA_ARGS__)
#define FEPN_58(WHAT,X,...) WHAT##58(X)FEPN_57(WHAT, __VA_ARGS__)
#define FEPN_59(WHAT,X,...) WHAT##59(X)FEPN_58(WHAT, __VA_ARGS__)
#define FEPN_60(WHAT,X,...) WHAT##60(X)FEPN_59(WHAT, __VA_ARGS__)



#define FOR_EACH_N(action,...) GET_MACRO(__VA_ARGS__,FEPN_60, FEPN_59, FEPN_58, FEPN_57, FEPN_56, FEPN_55, FEPN_54, FEPN_53, FEPN_52, FEPN_51, FEPN_50, FEPN_49, FEPN_48, FEPN_47, FEPN_46, FEPN_45, FEPN_44, FEPN_43, FEPN_42, FEPN_41, FEPN_40, FEPN_39, FEPN_38, FEPN_37, FEPN_36, FEPN_35, FEPN_34, FEPN_33, FEPN_32, FEPN_31, FEPN_30, FEPN_29, FEPN_28, FEPN_27, FEPN_26, FEPN_25, FEPN_24, FEPN_23, FEPN_22, FEPN_21, FEPN_20, FEPN_19, FEPN_18, FEPN_17, FEPN_16, FEPN_15, FEPN_14, FEPN_13, FEPN_12, FEPN_11, FEPN_10, FEPN_9, FEPN_8, FEPN_7, FEPN_6, FEPN_5, FEPN_4, FEPN_3, FEPN_2, FEPN_1)(action,__VA_ARGS__)


#define PARAM_DEFINE(var) , decltype(var)& var
#define PARAM_DEFINE_WITH_ID(paramId, var) , paramId, decltype(var)& var
//#define PARAM_INPLACE(var) , var
#define UNPACKAGE_FUNC_START(suffix, method) struct PP_CAT(PP_CAT(suffix,_), method) { static void PP_CAT(PP_CAT(PP_CAT(UnpackageUnorderedParameters_,suffix), _),method)(dep::ByteBuffer& byteBuffer


#define CASE_FIELD_NUMBER(fn, var) case (params - fn + 1): \
    inputStreamHandler.readUnordered(tag, var);  \
    break;

#define CASE_FIELD_NUMBER_WITH_ID(fn, var) case fn: \
    inputStreamHandler.readUnordered(tag, var);  \
    break;

#define FE_CASE_FIELD_NUMBER1(var) CASE_FIELD_NUMBER(1, var)
#define FE_CASE_FIELD_NUMBER2(var) CASE_FIELD_NUMBER(2, var)
#define FE_CASE_FIELD_NUMBER3(var) CASE_FIELD_NUMBER(3, var)
#define FE_CASE_FIELD_NUMBER4(var) CASE_FIELD_NUMBER(4, var)
#define FE_CASE_FIELD_NUMBER5(var) CASE_FIELD_NUMBER(5, var)
#define FE_CASE_FIELD_NUMBER6(var) CASE_FIELD_NUMBER(6, var)
#define FE_CASE_FIELD_NUMBER7(var) CASE_FIELD_NUMBER(7, var)
#define FE_CASE_FIELD_NUMBER8(var) CASE_FIELD_NUMBER(8, var)
#define FE_CASE_FIELD_NUMBER9(var) CASE_FIELD_NUMBER(9, var)
#define FE_CASE_FIELD_NUMBER10(var) CASE_FIELD_NUMBER(10, var)
#define FE_CASE_FIELD_NUMBER11(var) CASE_FIELD_NUMBER(11, var)
#define FE_CASE_FIELD_NUMBER12(var) CASE_FIELD_NUMBER(12, var)
#define FE_CASE_FIELD_NUMBER13(var) CASE_FIELD_NUMBER(13, var)
#define FE_CASE_FIELD_NUMBER14(var) CASE_FIELD_NUMBER(14, var)
#define FE_CASE_FIELD_NUMBER15(var) CASE_FIELD_NUMBER(15, var)
#define FE_CASE_FIELD_NUMBER16(var) CASE_FIELD_NUMBER(16, var)
#define FE_CASE_FIELD_NUMBER17(var) CASE_FIELD_NUMBER(17, var)
#define FE_CASE_FIELD_NUMBER18(var) CASE_FIELD_NUMBER(18, var)
#define FE_CASE_FIELD_NUMBER19(var) CASE_FIELD_NUMBER(19, var)
#define FE_CASE_FIELD_NUMBER20(var) CASE_FIELD_NUMBER(20, var)
#define FE_CASE_FIELD_NUMBER21(var) CASE_FIELD_NUMBER(21, var)
#define FE_CASE_FIELD_NUMBER22(var) CASE_FIELD_NUMBER(22, var)
#define FE_CASE_FIELD_NUMBER23(var) CASE_FIELD_NUMBER(23, var)
#define FE_CASE_FIELD_NUMBER24(var) CASE_FIELD_NUMBER(24, var)
#define FE_CASE_FIELD_NUMBER25(var) CASE_FIELD_NUMBER(25, var)
#define FE_CASE_FIELD_NUMBER26(var) CASE_FIELD_NUMBER(26, var)
#define FE_CASE_FIELD_NUMBER27(var) CASE_FIELD_NUMBER(27, var)
#define FE_CASE_FIELD_NUMBER28(var) CASE_FIELD_NUMBER(28, var)
#define FE_CASE_FIELD_NUMBER29(var) CASE_FIELD_NUMBER(29, var)
#define FE_CASE_FIELD_NUMBER30(var) CASE_FIELD_NUMBER(30, var)
#define FE_CASE_FIELD_NUMBER31(var) CASE_FIELD_NUMBER(31, var)
#define FE_CASE_FIELD_NUMBER32(var) CASE_FIELD_NUMBER(32, var)
#define FE_CASE_FIELD_NUMBER33(var) CASE_FIELD_NUMBER(33, var)
#define FE_CASE_FIELD_NUMBER34(var) CASE_FIELD_NUMBER(34, var)
#define FE_CASE_FIELD_NUMBER35(var) CASE_FIELD_NUMBER(35, var)
#define FE_CASE_FIELD_NUMBER36(var) CASE_FIELD_NUMBER(36, var)
#define FE_CASE_FIELD_NUMBER37(var) CASE_FIELD_NUMBER(37, var)
#define FE_CASE_FIELD_NUMBER38(var) CASE_FIELD_NUMBER(38, var)
#define FE_CASE_FIELD_NUMBER39(var) CASE_FIELD_NUMBER(39, var)
#define FE_CASE_FIELD_NUMBER40(var) CASE_FIELD_NUMBER(40, var)
#define FE_CASE_FIELD_NUMBER41(var) CASE_FIELD_NUMBER(41, var)
#define FE_CASE_FIELD_NUMBER42(var) CASE_FIELD_NUMBER(42, var)
#define FE_CASE_FIELD_NUMBER43(var) CASE_FIELD_NUMBER(43, var)
#define FE_CASE_FIELD_NUMBER44(var) CASE_FIELD_NUMBER(44, var)
#define FE_CASE_FIELD_NUMBER45(var) CASE_FIELD_NUMBER(45, var)
#define FE_CASE_FIELD_NUMBER46(var) CASE_FIELD_NUMBER(46, var)
#define FE_CASE_FIELD_NUMBER47(var) CASE_FIELD_NUMBER(47, var)
#define FE_CASE_FIELD_NUMBER48(var) CASE_FIELD_NUMBER(48, var)
#define FE_CASE_FIELD_NUMBER49(var) CASE_FIELD_NUMBER(49, var)
#define FE_CASE_FIELD_NUMBER50(var) CASE_FIELD_NUMBER(50, var)
#define FE_CASE_FIELD_NUMBER51(var) CASE_FIELD_NUMBER(51, var)
#define FE_CASE_FIELD_NUMBER52(var) CASE_FIELD_NUMBER(52, var)
#define FE_CASE_FIELD_NUMBER53(var) CASE_FIELD_NUMBER(53, var)
#define FE_CASE_FIELD_NUMBER54(var) CASE_FIELD_NUMBER(54, var)
#define FE_CASE_FIELD_NUMBER55(var) CASE_FIELD_NUMBER(55, var)
#define FE_CASE_FIELD_NUMBER56(var) CASE_FIELD_NUMBER(56, var)
#define FE_CASE_FIELD_NUMBER57(var) CASE_FIELD_NUMBER(57, var)
#define FE_CASE_FIELD_NUMBER58(var) CASE_FIELD_NUMBER(58, var)
#define FE_CASE_FIELD_NUMBER59(var) CASE_FIELD_NUMBER(59, var)
#define FE_CASE_FIELD_NUMBER60(var) CASE_FIELD_NUMBER(60, var)

#define GET_FIRST(X, Y) ,X

#define GET_MAX(X, Y) getmax(X,
#define RIGHT_OPERAND(X, Y) )

constexpr int getmax(int x, int y) {
	return x > y ? x : y;
}


#define UNPACKAGE_PARAMETERS(bytebuffer, ...) {    \
    using namespace google::protobuf::internal; \
    using namespace google::protobuf::io; \
    ArrayInputStream arrayInputStream(byteBuffer.getArray() + byteBuffer.getPosition(), byteBuffer.remaining());\
    CodedInputStream codeInputStream(&arrayInputStream);\
    lightmq::util::CodeInputStreamHandler inputStreamHandler(codeInputStream); \
    uint32_t tag;   \
    constexpr int params = VA_NUM_PARAMS(__VA_ARGS__);    \
    while ((tag = codeInputStream.ReadTag()) != 0) {   \
        int fieldNumer = WireFormatLite::GetTagFieldNumber(tag);    \
        switch (fieldNumer) {   \
            FOR_EACH_N(FE_CASE_FIELD_NUMBER, __VA_ARGS__)   \
            default: throw lightmq::util::ParameterUnpackagingException("can not found fieldNumber [" + std::to_string(fieldNumer) + "] in parameters list with UNPACKAGE_PARAMETERS function."); \
        }   \
    }   \
}


//int lastParamNumber = (0 FOR_EACH_2(GET_FIRST, __VA_ARGS__)); 
#define UNPACKAGE_PARAMETERS_WITHID(byteBuffer, ...) {    \
    using namespace google::protobuf::internal; \
    using namespace google::protobuf::io; \
    ArrayInputStream arrayInputStream(byteBuffer.getArray() + byteBuffer.getPosition(), byteBuffer.remaining());\
    CodedInputStream codeInputStream(&arrayInputStream);\
    lightmq::util::CodeInputStreamHandler inputStreamHandler(codeInputStream); \
    uint32_t tag;   \
    while ((tag = codeInputStream.ReadTag()) != 0) {   \
        int fieldNumer = WireFormatLite::GetTagFieldNumber(tag);    \
        switch (fieldNumer) {   \
            FOR_EACH_2(CASE_FIELD_NUMBER_WITH_ID, __VA_ARGS__)   \
            default: if(!WireFormatLite::SkipField(&codeInputStream, tag)) {throw lightmq::util::ParameterUnpackagingException("Failed to skip fieldNumber [" + std::to_string(fieldNumer) + "] in parameters list with UNPACKAGE_PARAMETERS function.");} \
        }   \
    }   \
}
//PP_CAT(PP_CAT(PP_CAT(service,_), method)::UnpackageUnorderedParameters_ , PP_CAT(PP_CAT(service,_), method)) (bytebuffer, __VA_ARGS__)


 #endif


