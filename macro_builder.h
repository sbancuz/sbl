//
// Source: https://www.scs.stanford.edu/~dm/blog/va-opt.html
//

#ifndef SBL_MACRO_BUILDER_H
#define SBL_MACRO_BUILDER_H
#define PARENS ()

#define EXPAND(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__
#define LAST_EXPAND(tok, ...)  __VA_OPT__(tok)

#define FOR_EACH(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, ...)                         \
  macro(a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define FOR_EACH2(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER2(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER2(macro, a1, a2, ...)                         \
  macro(a1, a2)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN2 PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN2() FOR_EACH_HELPER2

#define FOR_EACH3(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER3(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER3(macro, a1, a2, a3, ...)                         \
  macro(a1, a2, a3)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN3 PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAI3N() FOR_EACH_HELPER3

#define FOR_EACH_ARGS(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER_ARGS(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER_ARGS(macro, a1, ...)                         \
  macro(a1) __VA_OPT__(,)                                                    \
  __VA_OPT__(FOR_EACH_AGAIN_ARGS PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN_ARGS() FOR_EACH_HELPER_ARGS

#define FOR_EACH2_ARGS(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER2_ARGS(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER2_ARGS(macro, a1, a2, ...)                         \
  macro(a1, a2)  __VA_OPT__(,)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN2_ARGS PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN2_ARGS() FOR_EACH_HELPER2_ARGS

#define FOR_EACH3_ARGS(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER3_ARGS(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER3_ARGS(macro, a1, a2, a3, ...)                         \
  macro(a1, a2, a3)  __VA_OPT__(,)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN3_ARGS PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAI3N_ARGS() FOR_EACH_HELPER3_ARGS




#endif//SBL_MACRO_BUILDER_H
