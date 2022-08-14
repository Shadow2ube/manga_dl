//
// Created by christian on 8/8/22.
//

#ifndef MANGA_DL_SRC_MDLX_MDLX_TYPES_H_
#define MANGA_DL_SRC_MDLX_MDLX_TYPES_H_



enum chain_type {
  ct_PASS,
  ct_FOR_EACH,
  ct_ORDERED_FOR_EACH,
  ct_MERGE,
  ct_INVALID,
};

enum command {
  c_DOWNLOAD,
  c_PASS,
  c_FIND_ALL,
  c_CLICK,
  c_VEC,
  c_MERGE,
  c_ATTRIBUTE,
  c_CONTAINS,
  c_INVALID,
};

enum command_op {
  co_REGEX,
  co_XPATH,
  co_ORDERED,
  co_INVALID,
  co_BEFORE,
  co_AFTER,
};

enum modifier {
  m_PASS,
  m_ARG_ALL,
  m_ARG_0,
  m_ARG_1,
  m_ARG_2,
  m_INVALID,
};

#endif //MANGA_DL_SRC_MDLX_MDLX_TYPES_H_
