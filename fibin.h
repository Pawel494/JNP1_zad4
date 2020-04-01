//
// Created by Marcin Abramowicz and Paweł Majewski
//

#ifndef JNPI_FIBIN_FIBIN_H
#define JNPI_FIBIN_FIBIN_H

#include <utility>
#include <cassert>
#include <iostream>

struct True {
};

struct False {
};


//Var
constexpr unsigned int Var(const char *input) {
  unsigned int count = 0;
  while (input[count] != '\0')
    count++;
  const unsigned int len = count;
  assert(len >= 1 && len <= 6);

  unsigned int base = 36;
  unsigned int x = 0;
  bool isalnum = true;
  for (std::size_t i = 0; i < len; i++) {
    if (!((input[i] >= 48 && input[i] <= 57) ||
      (input[i] >= 65 && input[i] <= 90)
        || (input[i] >= 97 && input[i] <= 122)))
      isalnum = false;
  }
  assert(isalnum);

  unsigned int base2 = 1;
  for (std::size_t i = 0; input[i] != '\0'; i++) {
    int integer = 0;

    if (input[i] >= 48 && input[i] <= 57) //[0-9]
      integer = input[i] - 47;
    if (input[i] >= 65 && input[i] <= 90) //[A-Z]
      integer = input[i] - 54;
    if (input[i] >= 97 && input[i] <= 122) //[a-z]
      integer = input[i] - 86;

    x += base2 * integer;
    base2 *= base;
  }

  const unsigned int result = x;
  return result;
}


template<typename T>
struct Lit {
};

template<int N>
struct Fib {
};

template<typename Expr>
struct Inc1 {
};

template<typename Expr>
struct Inc10 {
};

template<typename Left, typename Right>
struct Eq {
};

template<typename Expr, typename... RestExpr>
struct Sum {
};

template<unsigned int Expr>
struct Ref {
};

template<unsigned int varHash, typename Value, typename Expression>
struct Let {
};

template<typename Condition, typename Then, typename Else>
struct If {
};

template<unsigned int varHash, typename Body>
struct Lambda {
};

template<typename Fun, typename Param>
struct Invoke {
};

template<typename ValueType, typename Enable = void>
class Fibin;

template<typename ValueType>
class Fibin<ValueType, typename std::enable_if_t<!std::is_integral_v<ValueType>>> {
public:

  template<typename Expr>
  static constexpr void eval() {
    std::cout << "Fibin doesn't support: " << typeid(ValueType).name() << "\n";
  }

private:
};


template<typename ValueType>
class Fibin<ValueType, typename std::enable_if_t<std::is_integral_v<ValueType>>> {

public:

  template<typename Expr>
  static constexpr ValueType eval() {
    typedef typename _Eval<Expr>::result evalLit;
    return _Eval<evalLit>::result::val;
  }

private:

  // environment
  // ---------------------------------------------------------------------------

  // unsigned int wrapper
  // ---------------------------------------------------------------------------
  template<unsigned int N>
  struct _UInt {
    static const unsigned int result = N;
  };

  template<class Left, class Righ>
  struct _EqUInt {
    static const bool result = false;
  };

  template<class Left>
  struct _EqUInt<Left, Left> {
    static const bool result = true;
  };

  template<ValueType N, ValueType M>
  struct _safeAdd {
    static const ValueType result = static_cast<ValueType>(N + M);
  };

  // env element
  // ---------------------------------------------------------------------------
  template<typename UInt, typename Value>
  struct _EnvElem {
    typedef Value result;
  };

  template<typename Left, typename Right>
  struct _EqEnvElem {
  };

  template<typename ULong1, typename ULong2, typename Value2>
  struct _EqEnvElem<ULong1, _EnvElem<ULong2, Value2>> {
    static const bool result = _EqUInt<ULong1, ULong2>::result;
  };


  // env
  // ---------------------------------------------------------------------------
  struct _EmptyEnv {
    typedef _EmptyEnv Head;
    typedef _EmptyEnv Tail;
  };

  template<class H, class T=_EmptyEnv>
  struct _Env {
    typedef H Head;
    typedef T Tail;
  };

  template<bool Cond, class _EnvElemCand, class _EnvElem, class _Env>
  struct _IsFoundFirstOrNext {
  };

  // ---------------------------------------------------------------------------
  template<typename Elm, typename Env=_EmptyEnv>
  struct _AddToEnv {
    typedef _Env<Elm, Env> result;
  };

  template<class Uint, class _Env>
  struct _FindFirstEnv {
    typedef typename _Env::Head Head;
    typedef typename _Env::Tail Tail;
    static const bool found = _EqEnvElem<Uint, Head>::result;

    typedef typename _IsFoundFirstOrNext<found, Head, Uint, Tail>::result result;
  };

  // ---------------------------------------------------------------------------
  template<class _EnvElemCand, class _EnvElem, class _Env>
  struct _IsFoundFirstOrNext<true, _EnvElemCand, _EnvElem, _Env> {
    typedef typename _EnvElemCand::result result;
  };

  template<class _EnvElemCand, class _EnvElem, class _Env>
  struct _IsFoundFirstOrNext<false, _EnvElemCand, _EnvElem, _Env> {
    typedef typename _FindFirstEnv<_EnvElem, _Env>::result result;
  };

  // facade
  // ---------------------------------------------------------------------------
  template<typename Expr, class Env = _EmptyEnv>
  struct _Eval {
  };

  // Fib<N> eval
  // ---------------------------------------------------------------------------

  template<ValueType N>
  struct _EvalFib {
    static const ValueType fibN1 = _EvalFib<N - 1>::result;
    static const ValueType fibN2 = _EvalFib<N - 2>::result;

    static const ValueType result = _safeAdd<fibN1, fibN2>::result;
  };

  template<>
  struct _EvalFib<1> {
    static const ValueType result = 1;
  };

  template<>
  struct _EvalFib<0> {
    static const ValueType result = 0;
  };

  template<ValueType N>
  struct _FibWrapper {
    static const ValueType val = N;
  };

  // FibWrapper eval
  // ---------------------------------------------------------------------------
  template<ValueType N, typename Env>
  struct _Eval<Lit<_FibWrapper<N>>, Env> {
    typedef _FibWrapper<N> result;
  };

  // Lit<Fit<N>> eval
  // ---------------------------------------------------------------------------
  template<ValueType N, typename Env>
  struct _Eval<Lit<Fib<N>>, Env> {
    typedef Lit<_FibWrapper<_EvalFib<N>::result>> result;
  };


  // Lit<True/False> eval
  // ---------------------------------------------------------------------------
  template<typename Env>
  struct _Eval<Lit<True>, Env> {
    typedef Lit<True> result;
  };

  template<typename Env>
  struct _Eval<Lit<False>, Env> {
    typedef Lit<False> result;
  };

  // Sum<...> eval
  // ---------------------------------------------------------------------------
  template<typename Expr, typename... RestExpr, typename Env>
  struct _Eval<Sum<Expr, RestExpr...>, Env> {
    typedef typename _Eval<Expr, Env>::result expr1Lit;
    static const ValueType expr1Val = _Eval<expr1Lit, Env>::result::val;

    typedef typename _Eval<Sum<RestExpr...>, Env>::result exprLit;
    static const ValueType exprVal = _Eval<exprLit, Env>::result::val;

    static const ValueType sum = _safeAdd<expr1Val, exprVal>::result;

    typedef Lit<_FibWrapper<sum>> result;
  };

  template<typename Expr1, typename Expr2, typename Env>
  struct _Eval<Sum<Expr1, Expr2>, Env> {
    typedef typename _Eval<Expr1, Env>::result expr1Lit;
    static const ValueType expr1Val = _Eval<expr1Lit, Env>::result::val;

    typedef typename _Eval<Expr2, Env>::result expr2Lit;
    static const ValueType expr2Val = _Eval<expr2Lit, Env>::result::val;

    static const ValueType sum = _safeAdd<expr1Val, expr2Val>::result;

    typedef Lit<_FibWrapper<sum>> result;
  };

  // Inc1<> eval
  // ---------------------------------------------------------------------------
  template<typename Expr, typename Env>
  struct _Eval<Inc1<Expr>, Env> {
    typedef typename _Eval<Sum<Lit<Fib<1>>, Expr>, Env>::result result;
  };

  // Inc10<> eval
  // ---------------------------------------------------------------------------
  template<typename Expr, typename Env>
  struct _Eval<Inc10<Expr>, Env> {
    typedef typename _Eval<Sum<Lit<Fib<10>>, Expr>, Env>::result result;
  };

  // If<Cond, Then, Else> eval
  // ---------------------------------------------------------------------------
  template<typename Condition, typename Then, typename Else, typename Env>
  struct _Eval<If<Condition, Then, Else>, Env> {
    typedef typename _Eval<Condition, Env>::result conditionVal;

    typedef typename _Eval<If<conditionVal, Then, Else>, Env>::result result;
  };

  template<typename Then, typename Else, typename Env>
  struct _Eval<If<Lit<True>, Then, Else>, Env> {
    typedef typename _Eval<Then, Env>::result result;
  };

  template<typename Then, typename Else, typename Env>
  struct _Eval<If<Lit<False>, Then, Else>, Env> {
    typedef typename _Eval<Else, Env>::result result;
  };

  // Eq<Left, Right> eval
  // ---------------------------------------------------------------------------

  template<bool Cond>
  struct _EvalEq {
  };

  template<>
  struct _EvalEq<true> {
    typedef Lit<True> result;
  };

  template<>
  struct _EvalEq<false> {
    typedef Lit<False> result;
  };


  template<typename Left, typename Right, typename Env>
  struct _Eval<Eq<Left, Right>, Env> {
    typedef typename _Eval<Left, Env>::result leftLit;
    static const ValueType leftVal = _Eval<leftLit, Env>::result::val;

    typedef typename _Eval<Right, Env>::result rightLit;
    static const ValueType rightVal = _Eval<rightLit, Env>::result::val;

    typedef typename _EvalEq<leftVal == rightVal>::result result;
  };

  // Ref<hash> eval
  // ---------------------------------------------------------------------------
  template<unsigned int hash, typename Env>
  struct _Eval<Ref<hash>, Env> {
    typedef typename _FindFirstEnv<_UInt<hash>, Env>::result result;
  };

  // Let<hash, val, exp> eval
  // ---------------------------------------------------------------------------
  template<unsigned int varHash, typename Value, typename Expression, typename Env>
  struct _Eval<Let<varHash, Value, Expression>, Env> {
    typedef typename _Eval<Value, Env>::result evalVal;
    typedef _EnvElem<_UInt<varHash>, evalVal> Elem;
    typedef typename _AddToEnv<Elem, Env>::result updatedEnv;

    typedef typename _Eval<Expression, updatedEnv>::result result;
  };

  // Lambda & invoke utility
  // ---------------------------------------------------------------------------

  template<typename Lambda, typename Env>
  struct _LambdaWIthEnv {
  };

  template<typename LamdaWithEnv, typename Arg>
  struct _Apply {
  };

  template<typename Lambda, typename Env>
  struct _Call {
  };

  template<unsigned int varHash, typename Body, typename Env>
  struct _Call<Lambda<varHash, Body>, Env> {
    typedef typename _Eval<Body, Env>::result result;
  };

  template<unsigned int varHash, typename Body, typename Env, typename Arg>
  struct _Apply<_LambdaWIthEnv<Lambda<varHash, Body>, Env>, Arg> {
    typedef _EnvElem<_UInt<varHash>, Arg> Elem;
    typedef typename _AddToEnv<Elem, Env>::result updatedEnv;

    typedef typename _Call<Lambda<varHash, Body>, updatedEnv>::result result;
  };

  // Lambda<hash, Body> eval
  // ---------------------------------------------------------------------------
  template<unsigned int varHash, typename Body, typename Env>
  struct _Eval<Lambda<varHash, Body>, Env> {
    typedef _LambdaWIthEnv<Lambda<varHash, Body>, Env> result;
  };

  // Invoke<Fun, Param> eval
  // ---------------------------------------------------------------------------
  template<typename Fun, typename Param, typename Env>
  struct _Eval<Invoke<Fun, Param>, Env> {
    typedef typename _Eval<Fun, Env>::result evalFunWithEnv;
    typedef typename _Eval<Param, Env>::result evalParam;

    typedef typename _Apply<evalFunWithEnv, evalParam>::result result;
  };

};


#endif //JNPI_FIBIN_FIBIN_H
