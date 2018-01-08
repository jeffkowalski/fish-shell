/*
                 __________
    _____   __ __\______   \_____  _______  ______  ____ _______
   /     \ |  |  \|     ___/\__  \ \_  __ \/  ___/_/ __ \\_  __ \
  |  Y Y  \|  |  /|    |     / __ \_|  | \/\___ \ \  ___/ |  | \/
  |__|_|  /|____/ |____|    (____  /|__|  /____  > \___  >|__|
        \/                       \/            \/      \/

  Copyright (C) 2013 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify,
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "muParser.h"

//--- Standard includes ------------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

/** \brief Pi (what else?). */
#define PARSER_CONST_PI 3.141592653589793238462643

/** \brief The Eulerian number. */
#define PARSER_CONST_E 2.718281828459045235360287

using namespace std;

/** \file
    \brief Implementation of the standard floating point parser.
*/

/** \brief Namespace for mathematical applications. */
namespace mu {

//---------------------------------------------------------------------------
// Trigonometric function
ValueOrError Parser::Sin(value_type v) { return std::sin(v); }
ValueOrError Parser::Cos(value_type v) { return std::cos(v); }
ValueOrError Parser::Tan(value_type v) { return std::tan(v); }
ValueOrError Parser::ASin(value_type v) { return std::asin(v); }
ValueOrError Parser::ACos(value_type v) { return std::acos(v); }
ValueOrError Parser::ATan(value_type v) { return std::atan(v); }
ValueOrError Parser::ATan2(value_type v1, value_type v2) { return std::atan2(v1, v2); }
ValueOrError Parser::Sinh(value_type v) { return std::sinh(v); }
ValueOrError Parser::Cosh(value_type v) { return std::cosh(v); }
ValueOrError Parser::Tanh(value_type v) { return std::tanh(v); }
ValueOrError Parser::ASinh(value_type v) { return std::asinh(v); }
ValueOrError Parser::ACosh(value_type v) { return std::acosh(v); }
ValueOrError Parser::ATanh(value_type v) { return std::atanh(v); }

//---------------------------------------------------------------------------
// Logarithm functions

// Logarithm base 2
ValueOrError Parser::Log2(value_type v) {
#ifdef MUP_MATH_EXCEPTIONS
    if (v <= 0) return ParserError(ecDOMAIN_ERROR, _T("Log2"));
#endif

    return std::log2(v);
}

// Logarithm base 10
ValueOrError Parser::Log10(value_type v) {
#ifdef MUP_MATH_EXCEPTIONS
    if (v <= 0) return ParserError(ecDOMAIN_ERROR, _T("Log10"));
#endif

    return std::log10(v);
}

// Logarithm base e (natural logarithm)
ValueOrError Parser::Ln(value_type v) {
#ifdef MUP_MATH_EXCEPTIONS
    if (v <= 0) return ParserError(ecDOMAIN_ERROR, _T("Ln"));
#endif

    return std::log(v);
}

//---------------------------------------------------------------------------
//  misc
ValueOrError Parser::Exp(value_type v) { return std::exp(v); }
ValueOrError Parser::Abs(value_type v) { return std::abs(v); }
ValueOrError Parser::Sqrt(value_type v) {
#ifdef MUP_MATH_EXCEPTIONS
    if (v < 0) return ParserError(ecDOMAIN_ERROR, _T("sqrt"));
#endif

    return std::sqrt(v);
}
ValueOrError Parser::Rint(value_type v) { return std::floor(v + 0.5); }
ValueOrError Parser::Sign(value_type v) {
    // return 1, 0, -1 according to whether v is positive, zero, negative.
    return (v > 0) - (v < 0);
}

//---------------------------------------------------------------------------
/** \brief Callback for the unary minus operator.
    \param v The value to negate
    \return -v
*/
ValueOrError Parser::UnaryMinus(value_type v) { return -v; }

//---------------------------------------------------------------------------
/** \brief Callback for the unary minus operator.
    \param v The value to negate
    \return -v
*/
ValueOrError Parser::UnaryPlus(value_type v) { return v; }

//---------------------------------------------------------------------------
/** \brief Callback for adding multiple values.
    \param [in] a_afArg Vector with the function arguments
    \param [in] a_iArgc The size of a_afArg
*/
ValueOrError Parser::Sum(const value_type *a_afArg, int a_iArgc) {
    if (!a_iArgc) return ParserError(_T("too few arguments for function sum."));

    value_type fRes = 0;
    for (int i = 0; i < a_iArgc; ++i) fRes += a_afArg[i];
    return fRes;
}

//---------------------------------------------------------------------------
/** \brief Callback for averaging multiple values.
    \param [in] a_afArg Vector with the function arguments
    \param [in] a_iArgc The size of a_afArg
*/
ValueOrError Parser::Avg(const value_type *a_afArg, int a_iArgc) {
    if (!a_iArgc) return ParserError(_T("too few arguments for function sum."));

    value_type fRes = 0;
    for (int i = 0; i < a_iArgc; ++i) fRes += a_afArg[i];
    return fRes / (value_type)a_iArgc;
}

//---------------------------------------------------------------------------
/** \brief Callback for determining the minimum value out of a vector.
    \param [in] a_afArg Vector with the function arguments
    \param [in] a_iArgc The size of a_afArg
*/
ValueOrError Parser::Min(const value_type *a_afArg, int a_iArgc) {
    if (!a_iArgc) return ParserError(_T("too few arguments for function min."));

    value_type fRes = a_afArg[0];
    for (int i = 0; i < a_iArgc; ++i) fRes = std::min(fRes, a_afArg[i]);

    return fRes;
}

//---------------------------------------------------------------------------
/** \brief Callback for determining the maximum value out of a vector.
    \param [in] a_afArg Vector with the function arguments
    \param [in] a_iArgc The size of a_afArg
*/
ValueOrError Parser::Max(const value_type *a_afArg, int a_iArgc) {
    if (!a_iArgc) return ParserError(_T("too few arguments for function max."));

    value_type fRes = a_afArg[0];
    for (int i = 0; i < a_iArgc; ++i) fRes = std::max(fRes, a_afArg[i]);

    return fRes;
}

//---------------------------------------------------------------------------
/** \brief Default value recognition callback.
    \param [in] a_szExpr Pointer to the expression
    \param [in, out] a_iPos Pointer to an index storing the current position within the expression
    \param [out] a_fVal Pointer where the value should be stored in case one is found.
    \return 1 if a value was found 0 otherwise.
*/
int Parser::IsVal(const char_type *a_szExpr, int *a_iPos, value_type *a_fVal) {
    value_type fVal(0);

    stringstream_type stream(a_szExpr);
    stream.seekg(0);  // todo:  check if this really is necessary
    stream.imbue(Parser::s_locale);
    stream >> fVal;
    stringstream_type::pos_type iEnd = stream.tellg();  // Position after reading

    if (iEnd == (stringstream_type::pos_type)-1) return 0;

    *a_iPos += (int)iEnd;
    *a_fVal = fVal;
    return 1;
}

//---------------------------------------------------------------------------
/** \brief Constructor.

  Call ParserBase class constructor and trigger Function, Operator and Constant initialization.
*/
Parser::Parser() : ParserBase() {
    AddValIdent(IsVal);

    InitCharSets();
    InitFun();
    InitConst();
    InitOprt();
}

//---------------------------------------------------------------------------
/** \brief Define the character sets.
    \sa DefineNameChars, DefineOprtChars, DefineInfixOprtChars

  This function is used for initializing the default character sets that define
  the characters to be useable in function and variable names and operators.
*/
void Parser::InitCharSets() {
    DefineNameChars(_T("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    DefineOprtChars(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-*^/?<>=#!$%&|~'_{}"));
    DefineInfixOprtChars(_T("/+-*^?<>=#!$%&|~'_"));
}

/// assert that the given optional error \p oerr is not an error.
/// This is used only during initialization, when it ought to be impossible
/// to generate an error.
static void assertNoError(OptionalError oerr) {
    assert(!oerr.has_error() && "Unexpected error during initialization");
    (void)oerr;
}

//---------------------------------------------------------------------------
/** \brief Initialize the default functions. */
void Parser::InitFun() {
    if (std::numeric_limits<mu::value_type>::is_integer) {
        // When setting MUP_BASETYPE to an integer type
        // Place functions for dealing with integer values here
        // ...
        // ...
        // ...
    } else {
        // trigonometric functions
        assertNoError(DefineFun(_T("sin"), Sin));
        assertNoError(DefineFun(_T("cos"), Cos));
        assertNoError(DefineFun(_T("tan"), Tan));
        // arcus functions
        assertNoError(DefineFun(_T("asin"), ASin));
        assertNoError(DefineFun(_T("acos"), ACos));
        assertNoError(DefineFun(_T("atan"), ATan));
        assertNoError(DefineFun(_T("atan2"), ATan2));
        // hyperbolic functions
        assertNoError(DefineFun(_T("sinh"), Sinh));
        assertNoError(DefineFun(_T("cosh"), Cosh));
        assertNoError(DefineFun(_T("tanh"), Tanh));
        // arcus hyperbolic functions
        assertNoError(DefineFun(_T("asinh"), ASinh));
        assertNoError(DefineFun(_T("acosh"), ACosh));
        assertNoError(DefineFun(_T("atanh"), ATanh));
        // Logarithm functions
        assertNoError(DefineFun(_T("log2"), Log2));
        assertNoError(DefineFun(_T("log10"), Log10));
        assertNoError(DefineFun(_T("log"), Ln));
        assertNoError(DefineFun(_T("ln"), Ln));
        // misc
        assertNoError(DefineFun(_T("exp"), Exp));
        assertNoError(DefineFun(_T("sqrt"), Sqrt));
        assertNoError(DefineFun(_T("sign"), Sign));
        assertNoError(DefineFun(_T("rint"), Rint));
        assertNoError(DefineFun(_T("abs"), Abs));
        // Functions with variable number of arguments
        assertNoError(DefineFun(_T("sum"), Sum));
        assertNoError(DefineFun(_T("avg"), Avg));
        assertNoError(DefineFun(_T("min"), Min));
        assertNoError(DefineFun(_T("max"), Max));
    }
}

//---------------------------------------------------------------------------
/** \brief Initialize constants.

  By default the parser recognizes two constants. Pi ("pi") and the Eulerian
  number ("_e").
*/
void Parser::InitConst() {
    assertNoError(DefineConst(_T("_pi"), (value_type)PARSER_CONST_PI));
    assertNoError(DefineConst(_T("_e"), (value_type)PARSER_CONST_E));
}

//---------------------------------------------------------------------------
/** \brief Initialize operators.

  By default only the unary minus operator is added.
*/
void Parser::InitOprt() {
    assertNoError(DefineInfixOprt(_T("-"), UnaryMinus));
    assertNoError(DefineInfixOprt(_T("+"), UnaryPlus));
}

}  // namespace mu
