---
tags:
  - 读书笔记
---

# 📖 C++ Primer

!!! abstract

    这篇笔记是《C++ Primer》英文版的书摘。中文版我已经读过两遍，但因长时间没有写 C++ 程序而淡忘。我阅读英文版作为复习，同时把一些自己还不太熟的点摘下来。

    这篇笔记不适合初学者。

!!! note "进度表"

    Current On: Chapter 12.

    - [x] Part I: The Basics
    - [ ] Part II: The C++ Library
        - [x] Ch 8: The IO Library
        - [x] Ch 9: Sequential Containers
        - [x] Ch 10: Generic Algorithms
        - [x] Ch 11: Associative Containers
        - [x] Ch 12: Dynamic Memory
    - [ ] Part III: Tools for Class Authors
        - [ ] Ch 13: Copy Control
        - [ ] Ch 14: Overloaded Operations and Conversions
        - [ ] Ch 15: Object-Oriented Programming
        - [ ] Ch 16: Templates and Generic Programming
    - [ ] Part IV: Advanced Topics
        - [ ] Chapter 17: Specialized Library Facilities
        - [ ] Chapter 18: Tools for Large Programs
        - [ ] Chapter 19: Specialized Tools and Techniques

## Chapter 1 Getting Started

Somethings different from C:

- `iostream` library:
    - `istream`, `ostream`
    - `cin`, `cout`, `cerr` (stderr, not buffered), `clog` (stderr, buffered)
    - built-in (primitive) type, class type
    - `::` scope operator

!!! note "some words"

    - curly brace, open curly, close curly
    - parentheses
    - underscore
    - directive

## Part I: The Basics

### Chapter 2 Variables and Basic Types

!!! abstract "Key"

    - `const` Qualifier
    - reference

#### Type Conversions

- If we assign an out-of-range value to an object of **unsigned** type, the result is the remainder of the value modulo the number of values the target type can hold (**wrap around**).
- If we assign an out-of-range value to an object of **signed** type, the result is **undefined**.

!!! example "unsigned type"

    Assigning –1 to an 8-bit unsigned char gives that object the value 255.

!!! danger "Don't mix signed and unsigned types"

!!! note "Specifying the Type of a Literal"

    Character and character string literals **prefix**:

    - `u`: `char16_t` (Unicode 16)
    - `U`: `char32_t` (Unicode 32)
    - `L`: `wchar_t`
    - `u8`: `char` (utf-8, string literal only)

    Integer literals **suffixes**:

    - `u`: `unsigned`
    - `l`: `long`
    - `ll`: `long long`

    Floating-point literals **suffix**:

    - `f`: `float`
    - `l`: `long double`

#### List Initialization (C++ 11)

Braced lists of initializers can now be used whenever we initialize an object and in some cases when we assign a new value to an object.

```cpp
int units_sold = 0;
int units_sold = {0};
int units_sold{0};
int units_sold(0);
```

List initialization prevents narrowing conversions. In C++, a **narrowing conversion** is a potentially unsafe numeric conversion where the destination type may not be able to hold all the values of the source type.

!!! note "Variables must be defined exactly once but can be declared many times."

#### Compound Types

More generally, a declaration is a base type followed by a list of **declarators**. Each declarator names a variable and gives the variable a type that is related to the **base type**.

!!! tip "A reference or pointer is part of a particular declarator and not part of the base type for the declaration."

- A reference defines an alternative name (alias) for an object.

    ```cpp
    int &refVal = ival; // refVal refers to (is another name for) ival
    ```

    - There is **no way to rebind** a reference to refer to a different object. References must be initialized.
    - The type of a reference and the object to which the reference refers must match exactly.
    - A reference may be bound only to an object, **not to a literal or to the result of a more general expression**.

        ```c title="reference to pointer"
        int *p;
        int *&r = p;
        ```

        !!! tip "read from right to left"

    - A reference is not an object. Hence, we may **not have a pointer to a reference**.

`nullptr` (C++ 11) is a literal that has a special type that can be converted to any other pointer type. Use it to initialize pointers. `NULL` is now defined in `<cstdlib>`.

We cannot use a `void*` to operate on the object it addresses—we don’t know that object’s type, and the **type determines what operations we can perform** on the object. Use a `void*` pointer to deal with memory as memory, rather than using the pointer to access the object stored in that memory.

#### `const` Qualifier

!!! note "To share a `const` object among multiple files, you must define the variable as extern."

??? note "By Default, `const` Objects Are Local to a File"

    When a const object is initialized from a **compile-time** constant, the compiler will usually replace uses of the variable with its corresponding value during compilation.

    To substitute the value for the variable, the compiler has to see the variable’s initializer. To support this usage, yet avoid multiple definitions of the same variable, `const` variables are defined as local to the file.

    Sometimes we have a `const` variable that we want to share across multiple files but whose initializer is not a constant expression. We use the keyword extern on both its definition and declaration(s).

!!! note "Initialize a **reference to `const`** from any expression that can be **converted** to the type of the reference."

    We can bind a reference to const to a nonconst object, a literal, or a more general expression:

    ```cpp
    const int &r2 = 42; // ok: r1 is a reference to const
    const int &r3 = r1 * 2; // ok: r3 is a reference to const
    ```

!!! tip "A Reference to `const` May Refer to an Object That Is Not `const`."

- Top-level `const`: an object itself is a `const`.
- Low-level `const`: the pointer points to a `const` object. appears in the base type of compound types such as pointers or references.

When we copy an object, top-level `const`s are ignored. Low-level `const` is never ignored.

```cpp
int *const p1 = &i; // we can’t change the value of p1; const is top-level
const int ci = 42; // we cannot change ci; const is top-level
const int *p2 = &ci; // we can change p2; const is low-level
```

!!! note "`constexpr` Variables"

    There are several contexts in the language that require **constant expressions**.

    We can ask the compiler to verify that a variable is a constant expression by declaring the variable in a constexpr declaration. Variables declared as constexpr are implicitly const and must be initialized by constant expressions.

    ```cpp
    constexpr int sq(int x) { return x * x; }
    constexpr int mf = sq(20); // calculated at compile time
    ```

    !!! note "The address of an object defined outside of any function is a constant expression (stored in fixed address), and so may be used to initialize a constexpr pointer."

    `constexpr` imposes a **top-level** const on the objects it defines. A constexpr pointer may point to a const or a nonconst type.

#### Dealing with Types

- new way to define a type alias: `using SI = Sales_item`. (C++ 11)

!!! danger "Declarations that use type aliases that represent compound types and const can yield surprising results."

- `auto` (C++ 11): the type of the variable that we are defining will be deduced from the initializer we provide.
    - `auto` ordinarily ignores **top-level** consts.
    - If you want so, use `const auto`.
- `decltype` (C++ 11): analyzes the expression to determine its type but **does not evaluate** the expression.

    ```cpp
    decltype(f()) sum = x; // sum has whatever type f returns
    ```

!!! danger "`decltype` will include top-level `const`s and references in its deduced type."

    `decltype` is the **only** context in which a variable defined as a reference is not treated as a synonym for the object to which it refers.

!!! danger "`decltype` returns a reference type for expressions that yield objects that can stand on the left-hand side of the assignment"

    If `p` is a pointer to `int`, then `decltype(*p)` is **`int&`**, not `int`.

!!! danger "enclosing the name of a variable in parentheses affects the type returned by `decltype`"

    - `decltype((variable))` (note, double parentheses) is always a reference type
    - `decltype(variable)` is a reference type only if variable is a reference.

??? note "随手记点单词"

    - variable-langth
    - undefined behavior, implementation-defined behavior

### Chapter 3 Strings, Vectors, and Arrays

!!! abstract "Key"

    - `using`
    - `string`
    - `vector`
    - Iterators

#### `using` Declaration

- A Separate `using` Declaration Is Required for Each Name

    ```cpp
    using std::cout; using std::endl;
    ```

- Headers Should Not Contain `using` Declarations

#### String

Ways to Initialize a `string`:

```cpp
string s1; // default initialization; s1 is the empty string
string s2(s1); // s2 is a copy of s1
string s3("value"); // s3 is a copy of the string literal
string s3 = "value"; // equivalent to s3("value")
...
```

Some operation:

```cpp
is >> s; // Reads whitespace-separated string into s. Returns is.
getline(is, s); // Reads a line of input from is into s. Return is.
s.empty(); // Returns true if s is empty; otherwise returns false.
...
```

!!! tip "Stream is valid until it has hit end-of-file or an error."

    Used as condition.

!!! tip "`getline()` don't save the newline character."
!!! warning "`string::size_type` Type"

    It is an unsigned type.

    For example, if `n` is an int that holds a negative value, then `s.size() < n` will almost surely evaluate as true. It yields true because the negative value in n will convert to a large unsigned value.

`string` library lets us convert both **character literals and character string literals** to `strings`.

#### Range-Based `for`

```cpp
for (declaration : expression)
    statement
```

- `expression` is an object of a type that represents a sequence
- `declaration` defines the variable that we’ll use to access the underlying elements in the sequence.

On each iteration, the variable in declaration is initialized from the value of the next element in expression.

```cpp
decltype(s.size()) punct_cnt = 0;
// count the number of punctuation characters in s
for (auto c : s) // for every char in s
    if (ispunct(c)) // if the character is punctuation
        ++punct_cnt; // increment the punctuation counter
cout << punct_cnt
     << " punctuation characters in " << s << endl;
```

!!! tip "If we want to change the value of the characters in a string, wemust define the loop variable as a reference type"

#### `vector`-container and class template

!!! tip "`vector` is a template, not a type. Types generated from vector must include the element type."

Ways to Initialize a `vector`:

```cpp
vector<T> v1; // v1 is empty
vector<T> v2(v1); // v2 is a copy of v1
vector<T> v2 = v1; // equivalent to v2(v1)
vector<T> v3(n, val); // v3 has n elements with value val
vector<T> v4(n); // v4 has n copies of the value-initialized value of T
vector<T> v5{a, b, c...}; // v5 has as many elements as there are initializers
vector<T> v5 = {a, b, c...}; // equivalent to v5{a, b, c...}
```

!!! tip "Parentheses construct the object, curly braces **list initialize** the object."

Some operation:

```cpp
v.push_back(t); // Adds a copy of t to the end of v. Returns void.
```

!!! danger "The body of a range `for` must not change the size of the sequence over which it is iterating."
!!! tip "`vector<int>::size_type` instead of `vector::size_type`"

#### Iterators

All of the **library containers** have iterators, but only a few of them support the subscript operator.

A **valid** iterator either denotes an element or denotes a position **one past the last element** in a container.

Obtain an iterator:

```cpp
auto b = v.begin(); // denotes the first element
auto b = v.cbegin(); // const iterator
auto c = cv.begin(); // also const iterator (vector is const)
auto e = v.end(); // denotes one past the last element
```

!!! tip "**of-the-end** iterator"

    If container is empty, begin and end iterator are equal.

Dereference an iterator to obtain the element denoted
by an iterator.

```cpp
*iter;
iter->mem;
++iter;
--iter;
iter1 == iter2;
```

!!! tip "C++ prefers `!=` to `>` and `<`. Use it."
!!! info "Iterator Types: `vector<int>::iterator`, `vector<int>::const_iterator`"
!!! danger "Changes the size of a `vector` potentially invalidates all iterators into that `vector`."

**Some** iterator support all relational operators (such as `vector` and `string`):

```cpp
+ - += -= > >= < <=
```

!!! note "Subtraction of two iterators result in `difference_type`, which is **signed**."

!!! example "iterator arithmetic"

    ```cpp
    // text must be sorted
    // beg and end will denote the range we’re searching
    auto beg = text.begin(), end = text.end();
    auto mid = text.begin() + (end - beg)/2; // original midpoint
    // while there are still elements to look at and we haven’t yet found sought
    while (mid != end && *mid != sought) {
        if (sought < *mid) // is the element we want in the first half?
            end = mid; // if so, adjust the range to ignore the second half
        else // the element we want is in the second half
            beg = mid + 1; // start looking with the element just after mid
        mid = beg + (end - beg)/2; // new midpoint
    }
    ```

!!! tip "Complicated Array Declarations"

    No array of references. No vector of references.

    Reference is not a type.

    ```cpp
    int *ptrs[10]; // ptrs is an array of ten pointers to int
    int &refs[10] = /* ? */; // error: no arrays of references
    int (*Parray)[10] = &arr; // Parray points to an array of ten ints
    int (&arrRef)[10] = arr; // arrRef refers to an array of ten ints
    int *(&arry)[10] = ptrs; // arry is a reference to an array of ten pointers
    ```

!!! note "随手记点单词"

    - instantiation

### Chapter 4 Expressions

Skipped.

#### Named Casts

A named cast has the following form:

```cpp
cast-name<type>(expression);
```

- `static_cast`:
    - Any well-defined type conversion, **other than those involving low-level const**, can be requested using a `static_cast`.
    - useful to perform a conversion that the compiler will not generate automatically.
- `dynamic_cast` → see dynamic memory
- `const_cast`:

    ```cpp
    const char *pc;
    char *p = const_cast<char*>(pc); // ok: but writing through p is undefined
    ```

    - A `const_cast` changes only a low-level const in its operand.
    - the compiler will no longer prevent us from writing to that object.

- `reinterpret_cast`:

    ```cpp
    int *ip;
    char *pc = reinterpret_cast<char*>(ip);
    ```

    - performs a low-level reinterpretation of the bit pattern of its operands.

!!! danger "Avoid casts, especially `reinterpret_cast` and old-style casts `(type) expr` and `type (expr)`."

### Chapter 5 Statements

Skipped.

### Chapter 6 Functions

!!! abstract "Key"

    - Overloaded Functions
    - Reference Parameters
    - `const` Parameters and Arguments
    - Return Types

- `void f1();` **implicit** `void` parameter list.

!!! warning "In C, `void no_args()` declares a function that takes an **unspecified (but not variable) number of parameters**."

#### Arguments

- Pass by reference:

```cpp
void reset(int &i) {
    i = 0;
}
```

Reference parameters that are not changed inside a function should be references to const.

!!! tip "Reference parameters let us **effectively** return multiple results. "

- `const` arguments:

Just as in any other **initialization**, when we copy an argument to initialize a parameter, **top-level** `consts` are ignored. The following two declarations are equivalent:

```cpp
void fcn(const int i) { /* fcn can read but not write to i */}
void fcn(int i) { /* .. . */} // error: redefines fcn(int)
```

!!! tip "The above rule is evident because arguments is passed by value except reference arguments which is **low-level** `const`."

- Array arguments:

Cannot copy an array. Arrays is (usually) converted to pointer.

```cpp
// despite appearances, these three declarations ofprintare equivalent
// each function has a single parameter of type constint*
void print(const int*);
void print(const int[]); // shows the intent that the function takes an array
void print(const int[10]); // dimension for documentation purposes (at best)
```

!!! note "three common techniques used to manage pointer parameters"

    - Using a Marker to Specify the Extent of an Array
    - Using the Standard Library Conventions: pass `begin` and `end` pointers.
    - Explicitly Passing a Size Parameter

!!! danger "Array Reference Parameters"

    ```cpp
    f(int (&arr)[10]);
    ```

    - The parentheses around `&arr` are necessary.
    - Because **the size of an array is part of its type**, it is safe to rely on the dimension in the body of the function.

!!! tip "Passing a Multidimensional Array"

    - The **size of the second (and any subsequent) dimension is part of the element type** and must be specified.

#### `initializer_list` Parameters (C++ 11)

an unknown number of arguments of a **single
type**.

`initializer_list` header.

```cpp
void error_msg(initializer_list<string> il) {
    for (auto beg = il.begin(); beg != il.end(); ++beg)
        cout << *beg << " ";
    cout << endl;
}
```

- the elements in an `initializer_list` are always `const` values.
- When we pass a sequence of values to an initializer_list parameter, we must enclose the sequence in **curly brace**.

#### Ellipsis Parameters

A C library facility named `varargs`.

- **Only** for types that are common to both C and C++. Objects of most class types are not copied properly when passed to an ellipsis parameter.
- Appear only as the last element in a parameter list.
- No type checking is done for the arguments that correspond to the ellipsis parameter.

#### Return Value

Values are returned in exactly the same way as variables and parameters are initialized: The return value is used to initialize a temporary at the call site, and that temporary is the result of the function call.

!!! danger "Never Return a Reference or Pointer to a Local Object"

    Here is a local temporary string.

    ```cpp
    return "Empty"; // WRONG: "Empty"is a local temporary string
    ```

!!! note "Calls to functions that return references are lvalues; other return types yield rvalues."

    ```cpp
    char &get_val(string &str, string::size_type ix) {
        return str[ix]; // get_val assumes the given index is valid
    }
    int main() {
        string s("a value");
        cout << s << endl; // prints a value
        get_val(s, 0) = ’A’; // changes s[0] to A
        cout << s << endl; // prints A value return 0;
    }
    ```

- List Initializing the Return Value
- The main function may not call itself.

The form of a function that returns a pointer to an array is:

```cpp
Type (*function(parameter_list))[dimension]
```

!!! example "Think about follows"

    ```cpp
    func(int);
    (*func(int));
    (*func(int))[10];
    int (*func(int))[10];
    ```

- Trailing Return Type (C++ 11)

Useful for functions with complicated return types

A trailing return type follows the parameter list and is preceded by `->`.

```cpp
auto func(int i) -> int(*)[10];
```

#### Overloaded Functions

Functions that have the same name but different parameter lists and that appear in the **same scope** are overloaded.

- The main function may not be overloaded.
- A type alias does not create a new type. So not overloadable.
- **top-level** `const` is ignored. Can overload based on whether the parameter is a **reference (or pointer) to** the `const` or non`const` version of a given type.

`const_cast` is usually useful in overloaded functions:

```cpp
string &shorterString(string &s1, string &s2)
{
    auto &r = shorterString(const_cast<const string&>(s1),
                            const_cast<const string&>(s2));
    return const_cast<string&>(r);
}
```

Inner scope name hides uses of that name declared in an outer scope:

```cpp
string read();
void print(const string &);
void print(double); // overloads the printfunction v
void fooBar(int ival)
{
    bool read = false; // new scope: hides the outer declaration of read
    string s = read(); // error: readis a boolvariable, not a function
    // bad practice: usually it’s a bad idea to declare functions at local scope
    void print(int); // new scope: hides previous instances of print
    print("Value: "); // error: print(const string &)is hidden
    print(ival); // ok: print(int)is visible
    print(3.14);
}
```

!!! note "In C++, name lookup happens **before** type checking."
!!! danger "Notice above **all** instances in outer scope is hidden."

#### Specialized Uses

- Default Arguments:
    - The default arguments are used for the trailing (right-most) arguments of a call.

    !!! danger "can omit only trailing arguments"

- Default Argument Declarations

    ```cpp
    string screen(sz, sz, char = ’*’);
    string screen(sz = 24, sz = 80, char); // ok: adds default arguments
    ```

    - it is legal to redeclare a function multiple times.
    - each parameter can have its default specified only once in a given scope.
    - any subsequent declaration can add a default only for a parameter that has **not previously** had a default specified.
    - defaults can be specified only if all parameters to the right already have defaults

- Default Argument Initializers
    - a default argument can be any **expression** that has a type that is convertible to the type of the parameter
- `inline` Functions Avoid Function Call Overhead
    - optimize small, straight-line functions that are called frequently
    - putting the keyword `inline` before the function’s **return type**
    - The compiler may choose to ignore this request.
- `constexpr` function is a function that can be used in a constant expression

    ```cpp
    // scale(arg)is a constant expression if arg is a constant expression
    constexpr size_t scale(size_t cnt) { return new_sz() * cnt; }
    ```

    - The return type and the type of each parameter in a must be a literal type, and the function body must contain exactly one return statement.
    - A constexpr function is not required to return a constant expression.
    - A constexpr function is permitted to return a value that is not a constant.

!!! note "Put `inline` and `constexpr` Functions in Header Files"

    All of the definitions of a given `inline` or `constexpr` must match exactly. As a result, `inline` and `constexpr` functions normally are defined in headers.

#### `assert` and `NDEBUG`

- `__func__`: compiler defines, the name of the function in which the call appears.
- `__FILE__`: preprocessor defines, string literal containing the name of the file
- `__LINE__`: preprocessor defines, integer literal containing the line number of the file
- `__TIME__`: preprocessor defines, string literal containing the time of translation
- `__DATE__`: preprocessor defines, string literal containing the date of translation

#### Function Matching

- candidate functions: same name, can be seen.
- viable functions: same number of parameters, each argument can be converted to the type of its corresponding parameter.

!!! tip "When a function has default arguments, a call may appear to have fewer arguments than it actually does."

Select the best match:

- The match for each argument is no worse than the match required by any other viable function.
- There is at least one argument for which the match is better than the match provided by any other viable function.

If after looking at each argument there is no single function that is preferable, then the call is in error. The compiler will complain that the call is **ambiguous**.

Casts should not be needed to call an overloaded function. The need for a cast suggests that the parameter sets are designed poorly.

Conversion ranks:

- exact match:
    - identical type
    - array to pointer
    - top-level const
- const conversion
- promotion: integral promotion, floating-point promotion
- arithmetic or pointer conversion
- class-type conversion

#### Pointers to Functions

Pointers to Overloaded Functions must match one of the overloaded functions **exactly**.

Returning a Pointer to Function better use type alias. Try to read this:

```cpp
int (*f1(int))(int*, int);
auto f1(int) -> int (*)(int*, int);
```

Using auto or decltype for Function Pointer Types:

```cpp
string::size_type sumLength(const string&, const string&);
string::size_type largerLength(const string&, const string&); // depending on the value of its string parameter,
// getFcn returns a pointer to sumLengthor to largerLength
decltype(sumLength) *getFcn(const string &);
```

!!! danger "The only tricky part in declaring `getFcn` is to remember that when we apply `decltype` to a function, it returns a function type, not a pointer to function type. We must add a `*` to indicate that we are **returning a pointer, not a function**."

### Chapter 7 Classes

!!! abstract "Key"

    - `this`
    - Constructors
    - Static Class Members

#### `this`

Member functions access the object on which they were called through an extra, implicit parameter named `this`. When we call a member function, this is **initialized with the address of the object** on which the function was invoked.

!!! warning "`this` is top-level `const` by default."

By default, the type of `this` is a `const` pointer to the non`const` version of the class type. Thus we cannot bind this to a `const` object. A `const` **following the parameter list** indicates that `this` is a pointer to `const`. Member functions that use `const` in this way are `const` member functions.

You can think of :

```cpp
std::string isbn() const { return this->bookNo; }
```

as:

```cpp
std::string Sales_data::isbn(const Sales_data, *const this)
{ return this->isbn; }
```

But the latter is not legal because we may not explicitly define the `this` pointer ourselves.

Return `this` object: use reference to `this`.

```cpp
Sales_data& Sales_data::combine(const Sales_data &rhs) // return reference
{ return *this; } // return the object on which the function was called
```

!!! warning "Return reference!"

#### Members

Class is itself a scope. The definitions of the member functions of a class are nested inside the scope of the class itself.

When we define a member function outside the class body, the member’s definition must match its declaration. The name of a member defined outside the class must include the name of the class of which it is a member.

!!! tip "Use the scope operator `::`."

    ```cpp
    double Sales_data::avg_price() const {
        if (units_sold)
            return revenue/units_sold;
        else
            return 0;
    }
    ```

#### Constructors

- Constructors have the same name as the class.
- Constructors have no return type.
- A class can have multiple constructors.
- Constructors may not be declared as `const`.

When we create a `const` object of a class type, the object does not assume its "`const`ness" until after the constructor completes the object’s initialization. Thus, constructors **can write to `const` objects during their construction**.

Defining outside the class body:

```cpp
Sales_data::Sales_data(std::istream &is) {
    read(is, *this); // read will read a transaction from is into this object
}
```

##### Synthesized Default Constructor

The default constructor is one that takes **no arguments**. The **compiler-generated** constructor is known as the synthesized default constructor.

- If there is an in-class initializer , use it to initialize the member.
- Otherwise, default-initialize the member.

The compiler generates a default constructor automatically **only if a class declares no constructors**.

!!! danger "For some classes, the synthesized default constructor does the wrong thing."

1. Classes that have members of **built-in or compound type** should ordinarily either initialize those members inside the class or define their own version of the default constructor. Otherwise, users could create objects with members that have **undefined value**.
2. A class has a member that has a class type, and that class doesn’t have a default constructor, then the compiler can’t initialize that member.

!!! tip "Often we are defining default constructor only because we want to provide other constructors as well as the default constructor"

```cpp
Sales_data() = default; // we already provide initializers for the data members with built-in type.
```

A constructor that supplies **default arguments** for all its parameters also defines the default constructor.

##### Constructor Initializer List

It is usually best for a constructor to use an **in-class initializer** if one exists and gives the member the correct value.

If your compiler **does not support in-class initializers**, your default constructor should use the constructor initializer list (described immediately following) to initialize every member of the class.

```cpp
Sales_data(const std::string &s, unsigned n, double p): bookNo(s), units_sold(n), revenue(p*n) { }
```

When a member is **omitted from the constructor initializer list**, it is implicitly initialized using the same process as is used by the **synthesized default constructor**.

By the time the body of the constructor begins executing, initialization is complete. Our **only chance to initialize `const` or reference datamembers** is in the constructor initializer. A class type member that does not have a default constructor also must be initialized.

!!! warning "Members are initialized in the order in which they appear in the class definition."

    It is a good idea to write constructor initializers in the same order as the members are declared. Moreover, when possible, avoid using members to initialize other members.

##### Delegating Constructors

In a delegating constructor, the member initializer list has a **single** entry that is the name of the class itself.

```cpp
// remaining constructors all delegate to another constructor
Sales_data(): Sales_data("", 0, 0) {}
Sales_data(std::string s): Sales_data(s, 0,0) {}
Sales_data(std::istream &is): Sales_data()
    { read(is, *this); }
```

When a constructor delegates to another constructor, the constructor initializer list and function body of the delegated-to constructor are both executed.

##### The Role of the Default Constructor

Default initialization happens:

- nonstatic variables or Arrays at block scope without initializers.
- a class that itself has members of class type uses the synthesized default constructor
- members of class type are not explicitly initialized in a constructor initializer list

Value initialization happens:

- array initialization when we provide fewer initializers than the size of the array
- local static object without an initializer
- request value initialization by writing an expressions of the form `T()` where `T` is the name of a type

!!! warning "It is a common mistake among programmers new to C++ to try to declare an object initialized with the default constructor as follows:"

    ```cpp
    Sales_data obj(); // oops! declares a function, not an object
    Sales_data obj2; // ok: obj2is an object, not a function
    ```

##### Implicit Class-Type Conversions

Every constructor that can be called with a **single** argument defines an implicit conversion to a class type.

!!! warning "Only One Class-Type Conversion Is Allowed"

    ```cpp
    // error: requires two user-defined conversions:
    // (1) convert "9-999-99999-9"to string
    // (2) convert that (temporary) stringto Sales_data
    item.combine("9-999-99999-9");
    ```

We can **prevent the use of a constructor in a context that requires an implicit conversion** by declaring the constructor as `explicit`. The `explicit` keyword is meaningful **only** on constructors that can be called with a **single** argument. The `explicit` keyword is used **only** on the constructor declaration inside the class.

!!! warning "Implicit conversions happens when we use the **copy form of initialization** (with an `=`)."

`explicit` Constructors Can Be Used **Only** for Direct Initialization.

- `string` constructor that takes a single parameter of type `const char*` **is not** explicit.
- `vector` constructor that takes a size **is** explicit.

#### Copy, Assignment, and Destruction

If we do not define these operations, the compiler will synthesize them for us.
Ordinarily, the versions that the compiler generates for us execute by copying, assigning, or destroying **each member of the object**.

In particular, the synthesized versions are unlikely to work correctly for classes that **allocate resources that reside outside the class objects themselves**.

#### Access Control and Encapsulation

access specifiers to enforce encapsulation:

- `public`: members are accessible to all parts of the program.
- `private`: members are accessible to the member functions of the class.

The only difference between `struct` and `class` is the default access level. If we use the `struct` keyword, the members defined before the first access specifier are `public`; if we use `class`, then the members are `private`.

##### Friends

A class can allow another class or function to access its nonpublic members by making that class or function a friend.

Friend declarations may appear anywhere inside a class definition:

```cpp
class Sales_data { // friend declarations for nonmember Sales_dataoperations added
    friend Sales_data add(const Sales_data&, const Sales_data&);
    friend std::istream &read(std::istream&, Sales_data&);
    friend std::ostream &print(std::ostream&, const Sales_data&);
}
```

!!! tip "A friend declaration only specifies access. It is **not a general declaration** of the function."

#### Additional Class Features

- Defining a Type Member
- member functions defined inside the class are automatically inline
- `mutable` Data Members is never `const`, a `const` member function may change a `mutable` member.
- Initializers for Data Members of Class Type (C++ 11). When we provide an in-class initializer, we must do so following **an `=` sign or inside braces**.

!!! tip "Overloading Based on `const`"

    ```cpp
    Screen &display(std::ostream &os)
        { do_display(os); return *this; }
    const Screen &display(std::ostream &os) const
        { do_display(os); return *this; }
    void do_display(std::ostream &os) const {os << contents;}
    ```

!!! tip "USE PRIVATE UTILITY FUNCTIONS FOR COMMON CODE"

!!! warning "Forward Declaration"

    After a declaration and **before a definition is seen, the type `Screen` is an incomplete type**—it’s known that `Screen` is a class type but not known what members that type contains.

    We can use an incomplete type in only **limited ways**: We can define pointers or references to such types, and we can declare (but not define) functions that use an incomplete type as a parameter or return type.

- Friendship between Classes
- Making A Member Function a Friend
    - First, define the `Window_mgr` class, which declares, but cannot define, `clear`. `Screen` must be declared before `clear` can use the members of `Screen`.
    - Next, define `class Screen`, including a friend declaration for `clear`.
    - Finally, define `clear`, which can now refer to the members in `Screen`.

!!! tip "Good practice: don’t use a member name for a parameter or other local variable"

??? info "Aggregate Classes and Literal Classes"

    An aggregate class gives users direct access to its members and has **special initialization syntax**. A class is an aggregate if:

    -   All of its data members are `public`
    -   It does not define any constructors
    -   It has no in-class initializers
    -   It has no base classes or `virtual` functions

    We can initialize the data members of an aggregate class by providing a braced list of member initializers.

    An aggregate Class whose data members are all of literal type is a **literal class**. A nonaggregate class, that meets the following restrictions, is also a literal class.

    Although constructors can’t be `const`, constructors in a literal class can be `constexpr` functions. Indeed, a literal class must provide at least one `constexpr` constructor.

#### `static` Class Members

Members that are **associated with the class**, rather than with individual objects of the class type.

The `static` members of a class exist **outside any object**. Objects do not contain data associated with `static` data members.

```cpp
r = Account::rate(); // access a staticmember using the scope operator
r = ac1.rate(); // through an Account object or reference
r = ac2->rate(); // through a pointer to an Account object
```

They are not initialized by the class’ constructors. We may not initialize a `static` member inside the class. We must define and initialize each `static` data member **outside the class body**.

```cpp
double Account::interestRate = initRate();
```

!!! info "Even if a `const static` data member is initialized in the class body, that member ordinarily should be defined outside the class definition with no initializer."

`static` Members Can Be Used in Ways Ordinary Members Can’t. A `static` data member can have incomplete type. We can use a `static` member as a default argument.

Similarly, `static` member **functions** are not bound to any object; they do not have a `this` pointer. As a result, static member functions may not be declared as `const`, and we maynot refer to `this` in the body of a static member.

!!! note "随手记点单词"

    - encapsulation

## Part II: The C++ Library

!!! abstract

    Central to the library are a number of container classes and a family of generic algorithms that let us write programs that are succinct and efficient.

### Chapter 8 The IO Library

Headers:

- `iostream`: `istream, wistream, ostream, wostream, iostream, wiostream`
- `fstream`: `ifstream, wifstream, ofstream, wofstream, fstream, wfstream`
- `sstream`: `istringstream, wistringstream, ostringstream, wostringstream, stringstream, wstringstream`

The names of the **widecharacter** versions begin with a `w`.

The library lets us **ignore the differences among these different kinds of streams** by using **inheritance**.

!!! example

    The types `ifstream` and `istringstream` inherit from `istream`.Thus, we can use objects of type `ifstream` or `istringstream` as if they were `istream` objects.

!!! warning "No Copy or Assign for IO Objects"
!!! tip "We can read from or write to a stream only when it is in a non-error state."
!!! info "Interrogating the State of a Stream"

    - `iostate` Type
    - Functions: `.eof()`, `.fail()`, `.bad()`, `.good()`.
    - Recover: `.clear()`, `.clear(flags)`, `.setstate(flags)`, `.rdstate()`.

    ```cpp
    // turns off failbit and badbit but all other bits unchanged
    cin.clear(cin.rdstate() & ~cin.failbit & ~cin.badbit);
    ```

Each output stream manages a buffer. Several conditions that cause the buffer to be flushed:

- The program completes normally.
- the buffer can become full.
- using a manipulator such as `endl`.
- use the unitbuf manipulator to set the stream’s internal state to empty the buffer after each output operation.
- An output stream might be tied to another stream. In this case, the output stream is flushed whenever the stream to which it is tied is read or written. **By default, `cin` and `cerr` are both tied to `cout`. Hence, reading `cin` or writing to `cerr` flushes the buffer in `cout`.**

!!! tip "When an input stream is tied to an output stream, any attempt to read the input stream will first flush the buffer associated with the output stream."

    Interactive systems usually should tie their input stream to their output stream. Doing so means that all output, which might include **prompts to the user, will be written before attempting to read the input.**

    - `tie()`: return the ostream to which this istream is tied.
    - `tie(&ostream)`: take a pointer to `ostream` and tie to this ostream, return the old tie.

    Each stream can be tied to at most one stream at a time. However, multiple streams can tie themselves to the same ostream.

!!! info "manipulators"

    - `endl`: newline, flush
    - `flush`: flush
    - `ends`: null, flush
    - `unitbuf`, `nounitbuF`: flush or not.

#### File Stream

```cpp
fstream fstrm(s);
fstream fstrm(s, mode);
fstrm.open(s);
fstrm.open(s, mode);
fstrm.close();
fstrm.is_open();
```

Calling open on a file stream that is already open will fail and set `failbit`. **Subsequent attempts** to use that file stream will fail.

When an `fstream` object is destroyed, `close` is called automatically.

File modes:

- `in`: read
- `out`: write
- `app`: append
- `ate`: seek to the end of the stream immediately after opening the file
- `trunc`: truncate the file, only when `out` is set
- `binary`: binary mode

#### String Stream

```cpp
sstream strm(s);
strm.str(); // return a copy of the string
strm.str(s); // copy s into strm
```

An `istringstream` is often used when we have some work to do on an entire line.

```cpp
string line, word; // will hold a line and word from input, respectively
vector<PersonInfo> people; // will hold all the records from the input
// read the input a line at a time until cinhits end-of-file (or another error)
while (getline(cin, line)) {
    PersonInfo info; // create an object to hold this record’s data
    istringstream record(line); // bind recordto the linewe just read
    record >> info.name; // read the name
    while (record >> word) // read the phone numbers
        info.phones.push_back(word); // and store them
    people.push_back(info); // append this record to people
}
```

An `ostringstream` is useful when we need to build up our output a little at a time but do not want to print the output until later.

```cpp
for(const auto &entry : people) { // for each entry in people
    ostringstream formatted, badNums; // objects created on each loop
    for (const auto &nums : entry.phones) { // for each number
        if (!valid(nums)) {
            badNums << " " << nums; // string in badNums
        } else
            // “writes” to formatted’s string
            formatted << " " << format(nums);
    }
    if (badNums.str().empty()) // there were no bad numbers
        os << entry.name << " " // print the name
            << formatted.str() << endl; // and reformatted numbers
    else // otherwise, print the name and bad numbers
        cerr << "input error: " << entry.name
            << " invalid number(s) " << badNums.str() << endl;
}
```

### Chapter 9 Sequential Containers

!!! abstract

    - `vector`
        - random access
        - fast insert/delete at the end
    - `deque`
        - random access
        - fast insert/delete at front or back
    - `list`: doubly linked list
        - fast insert and delete at any point in the sequence
        - bidirectional sequential access
        - substantial memory overhead
    - `forward_list`: singly linked list
        - fast insert and delete at any point in the sequence
        - sequential access in one direction
        - **no `size` operation**
        - substantial memory overhead
    - `array`: fixed-size
        - cannot remove elements
    - `string`: similar to `vector`

    Each container is defined in a header file with the same name as the type.

#### Container Operations

!!! note "Contatiner Operations"

    -   Type Aliases: useful in generic programming
        -   `iterator`
        -   `const_iterator`
        -   `size_type`
        -   `difference_type`
        -   `value_type`
        -   `reference`
        -   `const_reference`
    -   construction
        -   `C c;`
        -   `C c1(c2);`
        -   `C c(b, e);`
        -   `C c{a, b, c};`
    -   assignment and `swap`
        -   `c1 = c2`
        -   `a.swap(b)`
        -   `swap(a, b)`
    -   Add/Remove Elements
        -   `c.insert(args)`
        -   `c.emplace(inits)`
        -   `c.erase(args)`
        -   `c.clear()`
    -   Equality and Relational Operators
        -   `==`, `!=`, `<`, `<=`, `>`, `>=`
    -   Obtain Iterators
        -   `c.begin()`, `c.end()`
        -   `c.cbegin()`, `c.cend()`
    -   Additional Members of Reversible Containers
        -   `reverse_iterator`
        -   `const_reverse_iterator`
        -   `c.rbegin(), c.rend()`
        -   `c.crbegin(), c.crend()`

!!! warning "Iterator Ranges"

    An iterator range is denoted by a pair of iterators each of which refers to **an element, or to one past the last element, in the same container**.

    This element range is called a **left-inclusive** interval. The standard mathematical notation for such a range is

    $$
    [begin, end)
    $$

    Three convient properties:

    -   If `begin` equals `end`, the range is empty
    -   If `begin` is not equal to `end`, there is at least one element in the range, and `begin` refers to the first element in that range
    -   We can increment `begin` some number of times until `begin == end`

    ```cpp
    while (begin != end) {
        *begin = val; // ok: range isn’t empty so begindenotes an element
        ++begin; // advance the iterator to get the next element
    }
    ```

Initializing a Container as a Copy of Another Container:

- To create a container as a copy of another container, the **container and element types must match**.
- When we pass iterators, there is no requirement that the container types be identical. as long as it is **possible to convert**.
    - As usual, the iterators mark the first and **one past the last element to be copied**

!!! warning "the size of a library `array` is part of its type."

Library `array` is:

- Default initialized
- If there are fewer initializers than the size of the array, any remaining elements are value initialized.

Although we cannot **copy or assign objects** of built-in array types, there is no such restriction on `array`.

##### Assign

!!! abstract

    ```cpp
    sqe.assign(b, e);
    sqe.assign(il);
    seq.assign(n, t);
    ```

!!! warning "Assignment related operations **invalidate iterators, references, and pointers** into the left-hand container. Aside from `string` they remain **valid after a `swap`**,and (excepting arrays) the containers to which they refer are swapped."

- **assignment operator** requires that the left-hand and right-hand operands have the same type. It copies all the elements from the right-hand operand into the lefthand operand.
- `assign` lets us assign from a different but compatible type, or assign from a subsequence of a container. replaces all the elements in the left-hand container with (copies of) the elements specified by its arguments.

```cpp
list<string> names;
vector<const char*> oldstyle;
names = oldstyle; // error: container types don’t match
// ok: can convert from const char* to string
names.assign(oldstyle.cbegin(), oldstyle.cend());
```

!!! warning "Because the existing elements are replaced, the iterators passed to assign must not refer to the container on which assign is called."

##### Swap

The `swap` operation exchanges the contents of two containers of the same type. After the call to `swap`, the elements in the two containers are interchanged.

Excepting array, swapping two containers is guaranteed to be fast— the elements themselves are not swapped; internal data structures are swapped. guaranteed to run in constant time.

with the exception of `string`, iterators, references, and pointers into the containers are not invalidated. They refer to **the same elements as they did before the swap**. However, after the swap, those elements are in a different container.

##### Relational Operators

Comparing two containers performs a **pairwise comparison of the elements**. These operators work similarly to the string relationals

Relational Operators Use **Their Element’s Relational Operator**. We can use a relational operator to compare two containers only if the appropriate comparison operator is defined for the element type.

#### Sequential Container Operations

!!! danger "Container Operations May Invalidate Iterators"

    Refer to book.

##### Add

- `push_back`, `emplace_back`
    - aside from `array` and `forward_list`
- `push_front`, `emplace_front`
    - not valid for `vector`, `array` or `string`

Specified Point:

```cpp
c.insert(p, t);
c.emplace(p, args);
c.insert(p, n, t);
c.insert(p, b, e);
c.insert(p, il);
// all insert before p
// return iterator to first element inserted
```

Note it returns an iterator to the first element inserted. Try to understand this:

```cpp
list<string> lst;
auto iter = lst.begin();
while (cin >> word)
    iter = lst.insert(iter, word); // same as calling push_front
```

!!! warning "Adding elements to a `vector`, `string`, or `deque` potentially invalidates all existing iterators, references, and pointers into the container."

##### Access

- `front`, `back` return reference to elements
- `c[n]`, `c.at(n)` return reference to elements
    - `at` **throws `out_of_range` exception**
    - `c[n]` is undefined if `n >= c.size()`

!!! warning "Don't call `front` or `back` on an empty container."

##### Erase

- `pop_back()`
    - not valid for `forward_list`
- `pop_front()`
    - not valid for `vector`, `array` or `string`

```cpp
c.erase(p);
c.earse(b, e);
c.clear();
// returns an iterator to the element after the one deleted or the off-the-end iterator if p denotes the last element.
```

Removing elements **anywhere but the beginning or end of** a `deque` invalidates all iterators, references, and pointers. Iterators, references, and pointers to elements **after the erasure point** in a `vector` or `string` are invalidated.

##### Resize

```cpp
c.resize(n);
c.resize(n, t);
```

If resize shrinks the container, then iterators, references, and pointers to the deleted elements are invalidated.

#### `forward_list`

```cpp
lst.before_begin();
lst.cbefore_begin();
lst.insert_after(p, t);
lst.insert_after(p, n, t);
lst.insert_after(p, b, e);
lst.insert_after(p, il);
emplace_after(p, args);
lst.erase_after(p); // Returns an iterator to the element after the one deleted
lst.erase_after(b, e); // from the one after the iterator b up to but not including the one denoted by e
```

When we add or remove elements in a forward_list, wehaveto keep track of **two iterators**—one to the element we’re **checking** and one to that element’s **predecessor**.

#### `vector` and `string`

part of the implementation leaks into its interface:

- Container Size Management
    - `c.shrink_to_fit()`, no guarantee to reduce capacity
    - `c.capacity()`
    - `c.reserve(n)`, will never shrink

!!! note "Understand the difference between `size` and `capacity`."
!!! note "Also notice that `resize()` member do not change the capacity of the container, just change number of elements."

String supports addtional operations:

- Construct:

    ```cpp
    string s(cp, n);
    string s(s2, pos2);
    string s(s2, pos2, len2);
    ```

- Substr:
    `cpp
string s2 = s.substr(pos, len);
string s3 = s.substr(pos);
`
    In addition to the versions of `insert` and `erase` that take iterators, `string` provides versions that take an **index**. The string library also provides versions of insert and assign that take **Cstyle character arrays**.

The string class defines two additional members, `append` and `replace`,that can change the contents of a string.

- Search:

    ```cpp
    s.find(args);
    s.rfind(args);
    s.find_first_of(args);
    s.find_last_not_of(args);
    ...
    ```

`args` can be:

- `cp, pos, n`: Look for the first n characters in the array pointed to by the pointer cp. Start looking at position pos in s. No default for pos or n.
- `c, pos`: Look for the character c starting at position pos in s. pos defaults to 0.

If there is **no match**, the function returns a staticmember named `string::npos`.

- `compare` function also has many overloaded versions.
- Numeric Conversions

    ```cpp
    to_string(val);
    stoi(s, p, b);
    stol(s, p, b);
    stoul(s, p, b);
    stof(s, p);
    stod(s, p);
    ...
    ```

#### Container Adaptors

Three sequential container adaptors: `stack`, `queue`,and `priority_queue`. A container adaptor takes an existing container type and makes it act like a different type.

```cpp
stack<int> stk(deq); // copies elements from deq into stk
```

By default:

- both `stack` and `queue` are implemented in terms of `deque`
- a `priority_queue` is implemented on a `vector`.

```cpp
// empty stackimplemented on top ofvector
stack<string, vector<string>> str_stk;
```

!!! note "`stack`"

    ```cpp
    s.pop();
    s.push(item);
    s.emplace(args);
    s.top();
    ```

!!! note "`queue` and `priority_queue`"

    ```cpp
    q.pop();
    q.push(item);
    q.emplace(args);
    q.front();
    q.back(); // only for queue
    q.top(); // only for priority_queue
    ```

### Chapter 10 Generic Algorithms

!!! abstract

    - Header Files: `<algorithm> <numeric><functional><iterator>`
    - Algorithms:

        ```text
        find, count, accumulate, equal
        fill, fill_n, copy, replace, replace_copy
        back_inserter(container)
        sort, unique
        stable_sort, partition
        find_if
        for_each
        transform
        count_if
        unique_copy
        replace, replace_if, replace_copy, replace_copy_if
        ```

    - Container-Specifc Algorithm as members: `list` and `forward_list`

        ```text
        sort, merge, remove, reverse, unique
        splice
        ```

    - Lambda expression and `Bind()` function:

        ```cpp
        [capture list](parameter list) -> return type {function body};
        []{return 42;};
        bind(callable, arg_list);
        ref(); cref();
        vector<int> vec(in_iter, eof);
        ```

    - Iterator:
        - adaptor: insert, stream, reverse, move
        - back_inserter, front_inserter, inserter
        - (template) istream_iterator, ostream_iterator, default initialize: end iterator value
        - rbegin, rend, crbegin, crend, `.base()` **adjacent positions**

            !!! warning "Understand left-inclusive range and asymmetric"

        - categorize by operations required by algorithms: input, output, forward, bidirectional, random-access
            - single pass: may invalidate all other iterators into the stream
            - random-access: subscript `iter[n]` synonym for `*(iter + n)`
            - some auxiliary function: `distance()`
    - Misc:
        - C-style string equal
        - container capacity, reserve, resize operation
        - unique overwrites but not reorder

### Chapter 11 Associative Containers

Two primary types:

- `map` key-value pairs (array)
- `set` keys

Eight associative containers:

- `map` or `set`
- unique or `multi`
- ordered or `unordered` (often Hash)

Common feature:

- bidirectional iterators
- initialize as copy of another container (`vector` for example)

Key constraints:

- ordered: must define a way to compare elements, **strict weak ordering** `<`

    ```cpp title="Example of comparison function"
    multiset<Sales_data, decltype(compareIsbn) *> bookstore(compareIsbn);
    ```

    Function pointer type. Supply the function to the constructor.

Aliases:

```cpp
key_type
mapped_type
value_type // pair for map, key is const
```

Iterator:

- `key`s are `const`
    - For example, both `iterator` and `const_iterator` for `set` are read-only.

    ```cpp
    // notice the function const arguments
    multiset<Sales_data, bool (*)(const Sales_data &, const Sales_data &>::iterator it = bookstore.begin();
    ```

- ascending order

!!! note "In general, we do not use the generic algorithms with the associative
containers"

Insert:

- `insert` returns a `pair` of an iterator and a `bool` indicating whether the insertion took place.

    ```cpp
    auto ret = word_count.insert({word, 1});
    if (!ret.second)
        ++ret.first->second;
    ```

- For `multi` containers, don't return `bool` because insert always success.

Erase：

- `key_type` argument version return the number of elements removed

!!! warning "must not be equal to the `end` iterator"

Find:

- `find`, `count`
- `lower_bound`, `upper_bound`, `equal_range`

#### Unordered Containers

- use `==` operator, hash function
- map hash to a bucket
- object `hash<key_type>` to generate hash value
    - built-in types and `string`, smart pointer has hash function

!!! info "customized hash function see Chap.16"

#### Pair

```cpp
{v1, v2}; // easiest after c++11
make_pair(v1, v2);
p1 < p2; // compare first, then second
p1 == p2; // respectively equal
return pair<string, int>(v1, v2);
```

#### Map

- `m[k]` returns value
    - if `key` is not present, new element will be inserted (**value initialized**)
    - type returned differ from dereferencing iterator
- `m.at(k)` will throw `out_of_range` exception

!!! warning "Can't be used on `const` map"

!!! warning "Only `map` and `unordered_map` has `[]` operator and `at` function"

- range `for` fetch template type `pair`, which has `first` and `second` members

### Chapter 12 Dynamic Memory

Object types:

- automatic (saved in **stack** memory)
- `static` (saved in **static** memory)
- Dynamically allocated objects (stored in **heap** memory)

Two smart pointer types that manage dynamic objects. A smart pointer acts like a regular pointer with the important exception that it **automatically deletes** the object to which it points.

Common operations:

```cpp
p.get(); // return a pointer
swap(p, q);
p.swap(q);
shared_ptr<T> p(q, d);
```

#### `shared_ptr`

```cpp
shared_ptr<string> p;
auto p = make_shared<vector<string>>();
auto q(p);
p.unique();
p.use_count();
```

Allows multiple pointers to refer to the same object. Think of a `shared_ptr` as if it has an associated counter.

- count is incremented when
    - **copied**
    - used as **right-hand** operand of **assignment**
    - pass to or
    - return from a function by **value**.
- decremented when
    - assign new value to
    - destroyed.

Once a `shared_ptr`’s counter goes to zero, the `shared_ptr` automatically frees the object that it manages.

Cannot convert ordinary Pointers to `shared_ptr`s implicitly. Use initializer.

!!! warning "a smart pointer must point to dynamic memory because `delete` is used to free the associated object"

Use `make_shared` function to allocate and use dynamic memory.

- It returens a `shared_ptr`.
- It uses its arguments to construct an object of the given type.
    - Value initialization if no arguments.
- Use `auto` to avoid type name.

!!! note "Value Initialization"

    Value Initialization is similar to default initialization. In the case of **built-in types** the difference is significant; a value-initialized object of built-in type has a well-defined value but a default-initialized object does not.

    We can usually omit the value and supply only a size. In this case the library creates a **value-initialized element initializer for us**. This library-generated value is used to initialize each element in the container. The value of the element initializer depends on the type of the elements stored in the vector.

    If the `vector` holds elements of a built-in type, such as int, then the element initializer has a value of 0. If the elements are of a class type, such as string, then the element initializer is itself default initialized.

!!! danger "Memory Leak: `shared_ptr`s in a container"

    If you put shared_ptrs in a container, and you subsequently need to use some, but not all, of the elements, remember to erase the elements you no longer need.

`shared_ptr` is usually used to allow multiple objects to **share the same state** (refer to same object when copied).

!!! example "`StrBlob`"

    ```cpp
    class StrBlob {
    public:
        typedef std::vector<std::string>::size_type size_type;
        StrBlob();
        StrBlob(std::initializer_list<std::string> il);
        size_type size() const { return data->size(); }
        bool empty() const { return data->empty(); }
        void push_back(const std::string &t) {data->push_back(t);}
        void pop_back();
        std::string& front();
        std::string& back();
    private:
        std::shared_ptr<std::vector<std::string>> data;
        void check(size_type i, const std::string &msg) const;
    };
    ```

    Uses the **default versions** of the operations that copy, assign, and destroy objects of its type.

#### Managing Memory Directly

Two **operators**: `new`, `delete`.

```cpp
int *p1 = new int; // ifallocation fails, new throws std::bad_alloc
int *p2 = new (nothrow) int; // if allocation fails, new returns a null pointer
int *pi = new int(1024);
auto p1 = new auto(obj);
vector<int> *pv = new vector<int>{0,1,2,3,4,5,6,7,8,9};
string *ps = new string(); // value initialized
string *ps1 = new string; // default initialized
const string *pcs = new const string;
```

Can use direct initialization, traditional construction (parentheses) or list initialization.

!!! tip "Caller is responsible for deleting the memory."
!!! tip "assign `nullptr` to the pointer after we use `delete`"

## Part III: Tools for Class Authors

### Chapter 13 Copy Control

Five special member functions: copy constructor, copy-assignment operator, move constructor, move-assignment operator, and destructor.

- copy and move constructor: initialize new object from another object of the same type
- copy and move assignment: assign an object of a class type to another object of that same class type

#### Copy Constructor

- First parameter is a **reference** to the class type and any additional parameters have default values.
- Always a reference to `const`.
- Usually used **implicitly**.

```cpp
Foo(const Foo&);
```

- Synthesized copy constructor: memberwise copy.
-

#### Copy initialization

```cpp
string dots(10, ’.’); // direct initialization
string s(dots); // direct initialization
string s2 = dots; // copy initialization
string null_book = "9-999-99999-9"; // copy initialization
string nines = string(100, ’9’); // copy initialization
```

When copy initialization happens and that copy initialization requires either the copy constructor **or the move constructor**.

- Pass an object as an argument
- Return
- Brace initialize
- library containers allocate objects

### Chapter 14 Overloaded Operations and Conversions

### Chapter 15 Object-Oriented Programming

### Chapter 16 Templates and Generic Programming

## Part IV: Advanced Topics

### Chapter 17 Specialized Library Facilities

### Chapter 18 Tools for Large Programs

#### Exception Handling

- `throw` expression
- `try` block
- `catch` clause
- `exception` class
    - `what` member function: return C-style string

!!! note "Functions Are Exited during the Search for a Handler"

    If no appropriate catch is found, execution is transferred to a library function named `terminate`. The behavior of that function is system dependent but is guaranteed to stop further execution of the program.

!!! note "Standard Exception Classes"

    Default initialization:

    - `exception`
    - `bad_alloc`
    - `bad_cast`
    - `bad_typeid`
    - `bad_exception`

    Cannot be default initialized:

    - `runtime_error`
        - `range_error`
        - `overflow_error`
        - `underflow_error`
    - `logic_error`
        - `domain_error`
        - `invalid_argument`
        - `length_error`
        - `out_of_range`

!!! note "Stack Unwind"

    Because destructors may be invoked during stack unwinding, they should **never throw exceptions that the destructor itself does not handle**.

    All of the standard library types guarantee that their destructors will not raise an exception.

!!! note "Function `try` Blocks and Constructors"

    An exception might occur while processing a constructor initializer.

    ```cpp
    template <typename T>
    Blob<T>::Blob(std::initializer_list<T> il) try :
    data(std::make_shared<std::vector<T>>(il)) {
    /*empty body*/
    } catch(const std::bad_alloc &e) { handle_out_of_memory(e); }
    ```

### Chapter 19 Specialized Tools and Techniques

### 零碎技巧

- 被换行符困扰？使用`getline`函数，或者在`>>`后面加上`ws`。
    - 注意 `getline` 不会跳过前导空白字符。

```cpp
while (map_file >> key && getline(map_file, value))
```
