### generic programming
goal: reuse code

tool: template  --> parameterize types and values

object: data and behaviors

### class template
1. definition: the types of some of the variables, return types of methods, and/or parameters to the methods are specified as template parameters.
2. used in: containers, data structures
3. without it: 
   1. polymorphism: many subclass derived from a same parent class. virutal function. pointers(can have `nullptr` compared with values)
   2. shortage: example, GamePieces --> ChessPieces, unique_ptr<GamePieces>, cannot make full use of the functionality of ChessPieces. cannot store primitive types such as int, double(they don't derive from GamePieces).
advantages: type safety; 


#### an example of class template interface
recall: once you have a user-declared destructor, it’s deprecated for the compiler to implicitly generate a copy constructor or copy assignment operator,  move constructor or move assignment operator.
```cpp
export template <typename T>
class Grid {
    public:
        explicit Grid(size_t width = DefaultWidth, size_t height = DefaultHeight);
        virtual ~Grid() = default; // user-declared destructor
        // Explicitly default a copy constructor and assignment operator.
        Grid(const Grid& src) = default;
        Grid& operator=(const Grid& rhs) = default;
        // Explicitly default a move constructor and assignment operator.
        Grid(Grid&& src) = default;
        Grid& operator=(Grid&& rhs) = default;
        std::optional<T>& at(size_t x, size_t y);
        const std::optional<T>& at(size_t x, size_t y) const;
        size_t getHeight() const { return m_height; }
        size_t getWidth() const { return m_width; }
        static const size_t DefaultWidth { 10 };
        static const size_t DefaultHeight { 10 };
    private:
        void verifyCoordinate(size_t x, size_t y) const;
        std::vector<std::vector<std::optional<T>>> m_cells;
        size_t m_width { 0 }, m_height { 0 };
};


// the implementation is in the same file as declaration
template <typename T>
void Grid<T>::verifyCoordinate(size_t x, size_t y) const
{
    if (x >= m_width) {
        throw std::out_of_range {
            std::format("{} must be less than {}.", x, m_width) };
    }
    if (y >= m_height) {
        throw std::out_of_range {
            std::format("{} must be less than {}.", y, m_height) };
    }
}
template <typename T>
const std::optional<T>& Grid<T>::at(size_t x, size_t y) const
{
    verifyCoordinate(x, y);
    return m_cells[x][y];
}
template <typename T>
std::optional<T>& Grid<T>::at(size_t x, size_t y)
{
    return const_cast<std::optional<T>&>(std::as_const(*this).at(x, y));
}

```

#### using class template
```cpp
using IntGrid = Grid<int>;
void processIntGrid(IntGrid&) {}
```

#### how the complier processes templates
1. encounter definitions: syntax checking; not compile templates(don't the types of templates);
2. encounter instantiation: replace T with type such as `int`. different types generate different codes
3. selective instantiation: generates code for `virtual functions`; but generates code only for those `non-virtual methods` that you actually call.
4. force the compiler to generate code for all method: `template class Grid<int>`. error checking.  explicit template instantiations
5. Template Requirements on Types: the type `T` is: destructible, copy/move constructible, and copy/move assignable. `concepts`

#### Distributing Template Code Between Files
```cpp
export module gird;

export import :definition;
export import :implemetation;

// =============
export module grid:definition;
import <vector>;
import <optional>;
export template <typename T> class Grid { ... };

// =============
export module grid:implementation;
import :definition;
import <vector>;
...
export template <typename T>
Grid<T>::Grid(size_t width, size_t height) : m_width { width }, m_height { height }
{ ... }

```

#### template parameters
1. write a class with as many template parameters as you want
2. don't have to be types; can have default values
3. need not specify the default values such as `T` in the template specification for the method definitions after you provide default value for `T` in the beginning
4. unlike normal function parameters
```cpp
template <typename RetType = long long, typename T1, typename T2>
RetType add(const T1& t1, const T2& t2) { return t1 + t2; }

auto result { add(1, 2) };
```

#### Non-type Template Parameters
1. integral types(int, char, long ...), enumeration types, pointers, references, nullptr_t, auto, auto&, auto*
2. c++20: floating-point, class types(limitation)
3. Non-type template parameters become part of the type specification of instantiated objects
```cpp
export template <typename T = int, size_t WIDTH = 10, size_t HEIGHT = 10>
class Grid
{
    public:
        Grid() = default;
        virtual ~Grid() = default;
        // Explicitly default a copy constructor and assignment operator.
        Grid(const Grid& src) = default;
        Grid& operator=(const Grid& rhs) = default;
        std::optional<T>& at(size_t x, size_t y);
        const std::optional<T>& at(size_t x, size_t y) const;
        size_t getHeight() const { return HEIGHT; }
        size_t getWidth() const { return WIDTH; }
    private:
        void verifyCoordinate(size_t x, size_t y) const;
        std::optional<T> m_cells[WIDTH][HEIGHT]; // C-styles arrays don't support move semantics
};

template <typename T, size_t WIDTH, size_t HEIGHT>
const std::optional<T>& Grid<T, WIDTH, HEIGHT>::at(size_t x, size_t y) const
{
    verifyCoordinate(x, y);
    return m_cells[x][y];
}
Grid<> myIntGrid;
Grid<int> myGrid;
Grid<int, 5> anotherGrid;
Grid<int, 5, 5> aFourthGrid;

// wrong 
Grid myIntGrid; // not use template
```

#### class template argument deduction
1. function: the compiler can automatically deduce the template parameters from the arguments passed to a class template constructor. ex: `std::pair p{1, 2.0}`; 
2. This type deduction is disabled for `std::unique_ptr` and `shared_ptr`.You pass a `T*` to their constructors, which means that the compiler would have to choose between deducing `<T>` or `<T[]>`, a dangerous choice to get wrong. So, just remember that for `unique_ptr` and `shared_ptr`, you need to keep using `make_unique()` and `make_shared()`.
3. User-Defined Deduction Guides
```cpp
// defined outside the definition but inside the same namespace as the SpreadSheetCell class.
// constructor  --> decuction
// const char*  --> std::string
SpreadSheetCell(const char*) -> SpreadSheetCell<std::string>;
// explicit  behaves the same as constructors
explicit TemplateName(Parameters) -> DeduceTemplate;

namespace std{
   explicit vector(std::initilizer_list<const char*) -> vector<std::string>;
}

std::vector vec {"fasgaf"}; // type == vector<string>
```

### method templates
1. C++ allows you to templatize individual methods of a class
2. can be inside in a normal class or a class template
3. useful for assignment operators and copy constructors in class templates.
4. Virtual methods and destructors cannot be method templates.
5. why we need it? cannot pass `Grid<int>` to construct or assign to `Grid<double>`
```cpp
export template <typename T>
class Grid
{
    public:
        template <typename E>
        Grid(const Grid<E>& src);
        template <typename E>
        Grid& operator=(const Grid<E>& rhs);
        void swap(Grid& other) noexcept;
    // Omitted for brevity
};

template <typename T>
template <typename E>
Grid<T>::Grid(const Grid<E>& src)
: Grid { src.getWidth(), src.getHeight() }
{
    // The ctor-initializer of this constructor delegates first to the
    // non-copy constructor to allocate the proper amount of memory.
    // The next step is to copy the data.
    for (size_t i { 0 }; i < m_width; i++) {
        for (size_t j { 0 }; j < m_height; j++) {
            m_cells[i][j] = src.at(i, j);
        }
    }
}

// ============
template <typename T, size_t WIDTH, size_t HEIGHT>
template <typename E, size_t WIDTH2, size_t HEIGHT2>
Grid<T, WIDTH, HEIGHT>::Grid(const Grid<E, WIDTH2, HEIGHT2>& src)
{
    for (size_t i { 0 }; i < WIDTH; i++) {
        for (size_t j { 0 }; j < HEIGHT; j++) {
            if (i < WIDTH2 && j < HEIGHT2) {
                m_cells[i][j] = src.at(i, j);
            } else {
                m_cells[i][j].reset();
            }
        }       
    }
}
```

### class template specialization
1. `class Grid {};` is illegal. there is a class template named Grid
2. When you specialize a template, you don’t “inherit” any code; specializations are not like derivations.


```cpp
export module grid:string;
import :main; // origin definition in this module partition file

export template<>
class Grid<const char*>
{
    public:
        explicit Grid(size_t width = DefaultWidth, size_t height = DefaultHeight);
        virtual ~Grid() = default;
        // Explicitly default a copy constructor and assignment operator.
        Grid(const Grid& src) = default;
        Grid& operator=(const Grid& rhs) = default;
        // Explicitly default a move constructor and assignment operator.
        Grid(Grid&& src) = default;
        Grid& operator=(Grid&& rhs) = default;
        std::optional<std::string>& at(size_t x, size_t y);
        // the template argument is const char*
        const std::optional<std::string>& at(size_t x, size_t y) const;
        size_t getHeight() const { return m_height; }
        size_t getWidth() const { return m_width; }
        static const size_t DefaultWidth { 10 };
        static const size_t DefaultHeight { 10 };
    private:
        void verifyCoordinate(size_t x, size_t y) const;
        std::vector<std::vector<std::optional<std::string>>> m_cells;
        size_t m_width { 0 }, m_height { 0 };
};

```

### deriving from class template
1. If the derived class inherits from the template itself, it must be a template as well. 
2. you can derive from a specific instantiation of the class template, in which case your derived class does not need to be a template
3. derived class templates don't really derive from generic template, but from the instantiation for the same type
4. Use inheritance for extending implementations and for polymorphism. Use specialization for customizing implementations for particular types.
![Alt text](image.png)

### function template
1. override
2. friend function
```cpp
// Forward declare Grid template.
export template <typename T> class Grid;
// Prototype for templatized operator+.
export template <typename T>
Grid<T> operator+(const Grid<T>& lhs, const Grid<T>& rhs);
export template <typename T>
class Grid
{
    public:
        friend Grid operator+<T>(const Grid& lhs, const Grid& rhs);
        // Omitted for brevity
};
```

#### return type of function templates
1. compiler deduces the return type of a function by using `auto`. but strips `const` and `reference` qualifier. (`decltype` does not)
2.  C++ supports `auto return type deduction` and `decltype(auto)` 

```cpp
template <typename T1, typename T2>
auto add(const T1& t1, const T2& t2) {
    return t1 + t2;
}

const std::string message {"message"};
const std::string& getString() {return message;}

auto m1 {getString()}; // type(m1) == std::string
const auto& m2 {getString()}; // type(m2) == const std::string&
decltype(getString()) m3 {getString()}; // type(m3) == const std::string&
decltype(auto) m3 {getString()}; // the same as m3

// new version of add  c++14
template <typename T1, typename T2>
decltype(auto) add(const T1& t1, const T2& t2) {
    return t1 + t2;
}

// before c++14
template <typename T1, typename T2>
auto add(const T1& t1, const T2& t2) -> decltype(t1+t2) {
    return t1 + t2;
}
```

#### Abbreviated Function Template Syntax
1. introduced in C++20
2. each parameter specified as `auto` becomes a different template type parameter. 
3.  cannot use the deduced types explicitly in the implementation of the function template, as these automatically deduced types have no name. use `decltype(parameter name)` such as `decltype(t1)`
```cpp
template <typename T1, typename T2>
decltype(auto) add(const T1& t1, const T2& t2) {}

// c++20
decltype(auto) add(const auto& t1, const auto& t2) {return t1+t2;}
```

### variable templates
1. 变量模板
```cpp
template <typename T>
constexpr T pi {T{3.141592653589793238462643383279502884}};

float piFloat {pi<float>};
auto piDouble {pi<double>};
// std::numbers::pi;
```


### concepts
1. definition: named requirements used to constrain template type and non-type parameters of class and function templates.
2. grammer: written as predicates that are evaluated at compile time to verify the template arguments passed to a template.
3. goal: make template-related compiler errors more human readable
4. When writing concepts, make sure they model semantics and not just syntax. `int` and `string` support `operator+`, they have same syntax but not senamtics. sortable and swappable are good concepts

#### syntax
1. definition generic syntax: `template <parameter-list> concept concept-name = constraits-expression;`
2. concept expression: `concept-name<argument-list>` -> `true` or `false`

#### constraits expression
1. evaliate to a Boolean without any type conversions
2. requires expressions. syntax: `requires (parameter-list) {requirements;}`
3. Existing concept expressions can be combined using conjunctions (&&) and disjunctions (||)


```cpp
template <typename T>
concept C = sizeof(T)==4;

// simple requirements   an arbitrary expression statement
template <typename T>
concept Incrementable = requires(T x) {x++; ++x;};


// type requirements verify a certain type is valid; verify that a certain template can be instantiated with a given type
template <typename T>
concept C = requires {typename T::value_type;
                    typename SomeTemplate<T>;};


//compound requirements verify that something does not throw any exceptions and/or to verify that a certain method returns a certain type
// check method noexcept and return type
template <typename T>
concept C = requires(T x, T y) {
    {x.swap(y)} noexcept;
};

// std::convertible_to<From, To>: another concepts
// the return type of x.size() is passed to From (the first parameter of convertible_to)
template <typename T>
concept C = requires (const T x) {
    { x.size() } -> convertible_to<size_t>; 
};

template <typename T>
concept Comparable = requires(const T a, const T b) {
    { a == b } -> convertible_to<bool>;
    { a < b } -> convertible_to<bool>;
    // ... similar for the other comparison operators ...
};

// nested requirements
template <typename T>
concept C = requires (T t) {
    requires sizeof(t) == 4;
    ++t; --t; t++; t--;
};

template <typename T>
concept IncrementableAndDecrementable = Incrementable<T> && Decrementable<T>;
```

#### predifined standard concepts
1. core language concepts: `same_as`, `derived_from`, `convertible_to`, `integral`, `floating_point`...
2. comparison concepts: `equality_comparable`, `totally_ordered`...
3. object concepts: `movable`, `copyable`
4. callable concepts: `invocable`, `predicate`
5. <concepts>, <iterator>, <ranges>

```cpp
template <typename T>
concept IsDerivedFromFoo = derived_from<T, Foo>;

template <typename T>
concept DefaultAndCopyConstructible =
default_initializable<T> && copy_constructible<T>;
```

#### type-constrained auto
```cpp
Incrementable auto value1 { 1 }; // ok int++, ++int √
Incrementable auto value { "abc"s }; // wrong  string++, ++string x
```

#### type constraints
```cpp
// function
template <convertible_to<bool> T>
void handle(const T& t);

template <Incrementable T>
void process(const T& t);

template <typename T> requires Incrementable<T>
void process(const T& t);

template <typename T> requires convertible_to<T, bool>
void process(const T& t);

template <typename T> requires requires(T x) { x++; ++x; }
void process(const T& t);

template <typename T> requires (sizeof(T) == 4)
void process(const T& t);

template <typename T> requires Incrementable<T> && Decrementable<T>
void process(const T& t);

template <typename T> requires is_arithmetic_v<T>
void process(const T& t);

// class template
template <std::derived_from<GamePiece> T>
void GameBoard<T>::move(size_t xSrc, size_t ySrc, size_t xDest, size_t yDest) { ... }

// class method
template <std::derived_from<GamePiece> T>
void GameBoard<T>::move(size_t xSrc, size_t ySrc, size_t xDest, size_t yDest)
requires std::movable<T>
{ ... }

// class specification
template <std::floating_point T>
bool AreEqual(T x, T y, int precision = 2)
{
// Scale the machine epsilon to the magnitude of the given values and multiply
// by the required precision.
return fabs(x - y) <= numeric_limits<T>::epsilon() * fabs(x + y) * precision
|| fabs(x - y) < numeric_limits<T>::min(); // The result is subnormal.
}


template <std::floating_point T>
size_t Find(const T& value, const T* arr, size_t size)
{
    for (size_t i { 0 }; i < size; i++) {
        if (AreEqual(arr[i], value)) {
        return i; // Found it; return the index.
        }
    }
    return NOT_FOUND; // Failed to find it; return NOT_FOUND.
}
```

## advanced templates
1. The different kinds of template parameters
2. How to use partial specialization
3. How to write recursive templates
4. What variadic templates are
5. How to write type-safe variable argument functions using variadic templates
6. What `constexpr if` statements are
7. How to use fold expressions
8. What metaprogramming is and how to use it
9. What type traits can be used for


### More About Template Type Parameters
1. `type`, `non-type`, `template template`
2. `template <..., template <TemplateTypeParams> class ParameterName, ...>`


```cpp
// define a concepts
template <typename Container>
concept ContainerType = requires(Container c)
{
    c.resize(1);   // c is a object
    typename Container::value_type; // Container is a type
};

export template <typename T, typename C=std::vector<std::optional<T>>> requires ContanerType(C)
class Grid {
typename Container::value_type& at(size_t x, size_t y);
const typename Container::value_type& at(size_t x, size_t y) const;
}; // definition is omitted.

// why we need template template parameters?
Grid<int, vector<optional<int>>> myIntGrid; // good
Grid<int, vector<optional<SpreadsheetCell>>> myIntGrid; // bad

// we want to write: 
Grid<int, vector> myIntGrid; // means the right behaviors. but vector is a template not a type ...

// method: copy and paste the declaration of class template to replace the Container.
export template <typename T,
template <typename E, typename Allocator = std::allocator<E>> class Container = std::vector>
class Grid {
    // ...
    std::vector<Container<std::optional<T>>> m_cells;
};

// DEFAULT is a non-type template parameter.
export template <typename T, const T DEFAULT = T{}>
class Grid {};

```

### CLASS TEMPLATE PARTIAL SPECIALIZATION
1. specialize some template parameters but not others
2. Note that wherever you refer to the full class name, you must use `Grid<const char*, WIDTH, HEIGHT>`.


```cpp
export template <size_t WIDTH, size_t HEIGHT>
class Grid<const char*, WIDTH, HEIGHT> // 没有默认参数就得补全模板参数
{
public:
    Grid() = default;
    virtual ~Grid() = default;
    // Explicitly default a copy constructor and assignment operator.
    Grid(const Grid& src) = default;
    Grid& operator=(const Grid& rhs) = default;
    std::optional<std::string>& at(size_t x, size_t y);
    const std::optional<std::string>& at(size_t x, size_t y) const;
    size_t getHeight() const { return HEIGHT; }
    size_t getWidth() const { return WIDTH; }
private:
    void verifyCoordinate(size_t x, size_t y) const;
    std::optional<std::string> m_cells[WIDTH][HEIGHT];
};
```

### emulating partial specialization with overload
1. The C++ standard does not permit partial template specialization of function templates.Instead, you can overload the function template with another function template. 


### template recursion
1. Template recursion is similar to function recursion, which means that a function is defined in terms of calling itself with a slightly easier version of the problem.

```cpp
export template <typename T, size_t N>
class NDGrid
{
public:
    explicit NDGrid(size_t size = DefaultSize) { resize(size); }
    virtual ~NDGrid() = default;
    NDGrid<T, N-1>& operator[](size_t x) { return m_elements[x]; }
    const NDGrid<T, N-1>& operator[](size_t x) const { return m_elements[x]; }
    void resize(size_t newSize)
    {
        m_elements.resize(newSize);
        // Resizing the vector calls the 0-argument constructor for
        // the NDGrid<T, N-1> elements, which constructs
        // it with the default size. Thus, we must explicitly call
        // resize() on each of the elements to recursively resize all
        // nested Grid elements.
        for (auto& element : m_elements) {
            element.resize(newSize);
        }
    }
    size_t getSize() const { return m_elements.size(); }
    static const size_t DefaultSize { 10 };
private:
    std::vector<NDGrid<T, N-1>> m_elements;
};

export template <typename T>
class NDGrid<T, 1>
{
public:
    explicit NDGrid(size_t size = DefaultSize) { resize(size); }
    virtual ~NDGrid() = default;
    T& operator[](size_t x) { return m_elements[x]; }
    const T& operator[](size_t x) const { return m_elements[x]; }
    void resize(size_t newSize) { m_elements.resize(newSize); }
    size_t getSize() const { return m_elements.size(); }
    static const size_t DefaultSize { 10 };
private:
    std::vector<T> m_elements;
}
```


### variadic templates
1. `Variadic templates` can take a variable number of template parameters.
2. Variadic templates allow you to create `type-safe variable-length argument lists`.
3. `Perfect forwarding` means that if an rvalue is passed to processValues(), it is forwarded as an rvalue reference. If an lvalue or lvalue reference is passed, it is forwarded as an lvalue reference.
4. `T&&` is only a `forwarding reference` when it is used as a parameter for a `function- or method template` with T as one of its template parameters. If a class method has a T&& parameter, but with T a template parameter of the class and not of the method itself, then that T&& is not a forwarding reference, but just an rvalue reference. That’s because at the time the compiler starts processing that method with a T&& parameter, the class template parameter T has already been resolved to a concrete type, for example int, and at that time, that method parameter has already been replaced with int&&.



```cpp
// can put space before and after `...`
template <typename ... types> 
class MyVariadicTemplate {};

MyVariadicTemplate<int> instance1;
MyVariadicTemplate<string, double, vector<int>> instance2;
MyVariadicTemplate<> instance3; // ok!

// diallow zero template arguments
template <typename T1, typename ... Types>
class MyClass {};

void handleValue(int value) { cout << "Integer: " << value << endl; }
void handleValue(double value) { cout << "Double: " << value << endl; }
void handleValue(string_view value) { cout << "String: " << value << endl; }
void processValues() // Base case to stop recursion
{ /* Nothing to do in this base case. */ }

// parameter pack
template <typename T1, typename ... Types>
// parameter pack expansion: unpacks/expands the parameter pack into arguments
void processValues(T1 arg1, Types ... args) {
    handleValue(arg1);
    processValues(args...);
    // get the number of arguments in the pack
    int number {sizeof ...(args)};
}

template <typename T1, typename ... Tn>
void processValues(T1 && arg1, Tn ... args) {
    handleValue(std::forward<T1>(arg1));
    processValues(std::forward<Tn>(args...));
    // processValues(forward<A1>(a1), forward<A2>(a2), forward<A3>(a3))
}

```


### variable number of Mixin classes
```cpp
class Mixin1
{
public:
    Mixin1(int i) : m_value { i } {}
    virtual void mixin1Func() { cout << "Mixin1: " << m_value << endl; }
private:
    int m_value;
};
class Mixin2
{
public:
    Mixin2(int i) : m_value { i } {}
    virtual void mixin2Func() { cout << "Mixin2: " << m_value << endl; }
private:
    int m_value;
};
template <typename... Mixins>
class MyClass : public Mixins...
{
public:
    MyClass(const Mixins&... mixins) : Mixins { mixins }... {}
    virtual ~MyClass() = default;
};
MyClass<Mixin1, Mixin2> a { Mixin1 { 11 }, Mixin2 { 22 } };
a.mixin1Func();
a.mixin2Func();
MyClass<Mixin1> b { Mixin1 { 33 } };
b.mixin1Func();
//b.mixin2Func(); // Error: does not compile.
MyClass<> c;
//c.mixin1Func(); // Error: does not compile.
//c.mixin2Func(); // Error: does not compile.
```

### fold expressions
1. The following table lists the four types of folds that are supported. In this table, `Ѳ` can be any of the following operators: `+ - * / % ^ & | << >> += -= *= /= %= ^= &= |= <<= >>= = == != < > <= >= && || , .* ->*`.
![Alt text](image-5.png)

```cpp
void processValues() { /* Nothing to do in this base case.*/ }
template <typename T1, typename... Tn>
void processValues(T1 arg1, Tn... args)
{
    handleValue(arg1);
    processValues(args...);
}

template <typename ... Tn>
void processValues(Tn ... args) {
    (cout << args << endl; ...);
    // ((cout << v1 << endl), ((cout << v2 << endl), (cout << v3 << endl)));
}

template <typename T, typename ... Tn>
double sum(const T& init, const Tn... args) {
    return (init + ... + args);
}

template <typename... Values>
double sumValues(const Values&... values) { 
    return (... + values); 
}

template <typename... Values>
double allTrue(const Values&... values) { return (... && values); }
template <typename... Values>
double anyTrue(const Values&... values) { return (... || values); }

{
cout << allTrue(1, 1, 0) << allTrue(1, 1) << allTrue() << endl; // 011
cout << anyTrue(1, 1, 0) << anyTrue(0, 0) << anyTrue() << endl; // 100
}
```

### metaprogramming
1. goals: perform some computation at `compile time` instead of at run time. It is basically a programming language on top of another programming language.
2. functions: `compile time computation`; `loop unrolling` or `loop unwinding`; `printing tuples`
3. `constexpr if` since `c++17`
4. using a Compile-Time Integer Sequence with Folding: `std::integer_sequence`, `std::index_sequence`, `std::index_sequence_for`


```cpp
template <size_t N>
class Factorial {
public:
    static const unsigned long long value {f*factorial<N-1>::value};
};

template <>
class Factorial<0>
{
public:
    static const unsigned long long value {1};
};

// or a better way
consteval unsigned long long factorial (unsigned char f) {
    if (f == 0) {return 1};
    return {f*factorial(f-1);}
}

// loop unwinding
template <int i> 
class Loop {
public:
    template <typename Func>
    static inline void run(Func func) {
        Loop<i-1>::run(func);
        func(i);
    }
};
template <>
class Loop<0>
{
public:
    template <typename FuncType>
    static inline void run(FuncType /* func */) { }
};

void doWork(int i) { cout << "doWork(" << i << ")" << endl; }
Loop<3>::run(doWork);

// print tuple
template <typename TupleType, int n>
class TuplePrint
{
public:
TuplePrint(const TupleType& t) {
    TuplePrint<TupleType, n - 1> tp { t };
    cout << get<n - 1>(t) << endl;
    }
};
template <typename TupleType>
class TuplePrint<TupleType, 0>
{
public:
    TuplePrint(const TupleType&) { }
};


template <typename TupleType, int n = tuple_size<TupleType>::value>
void tuplePrint(const TupleType& t) {
    if constexpr (n > 1) {
        tuplePrint<TupleType, n - 1>(t);
    }
    cout << get<n - 1>(t) << endl;
}

template <typename Tuple, size_t ... Indices>
void tuplePrintHelper(const Tuple& t, index_sequence<Indices...>) {
    ((cout << get<Indices>(t) << endl;), ...);
}

template <typename ... Args>
void tuplePrint(const tuple<Args...>& t) {
    tuplePrintHelper(t, index_sequence_for<Args...>());
}
```

### type traits
1. `Type traits` allow you to make decisions based on types at compile time. 
2. `some_trait<T>::value == some_trait_v<T>`
3.  if you just want compilation to fail when you use a template with the wrong types, use `concepts`, or use `static_assert()`, instead of `SFINAE`.
4.  Relying on SFINAE is tricky and complicated. If your use of SFINAE and `enable_if` selectively disables the wrong overloads in your overload set, you will get cryptic compiler errors, which will be hard to track down.
5. `static_assert()` allows certain conditions to be asserted at compile time. If an assertion is false, the compiler issues an error. accepts two parameters: an `expression` to evaluate at compile time and a `string`(optional since c++17)


![Alt text](image-6.png)

```cpp
template <class T, T v>
struct integral_constant {
    static constexpr T value { v };
    using value_type = T;
    using type = integral_constant<T, v>;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

template <bool B>
using bool_constant = integral_constant<bool, B>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <>
struct is_integral<bool>: public true_type {}; // bool 类型是 integral


template <typename T>
void processHelper(const T& t, true_type)
{
    cout << t << " is an integral type." << endl;
}
template <typename T>
void processHelper(const T& t, false_type)
{
    cout << t << " is a non-integral type." << endl;
}
template <typename T>
void process(const T& t)
{
    processHelper(t, typename is_integral<T>::type{});
}

// type relationship is_same, is_base_of, is_convertible
template <typename T1, typename T2>
void same(const T1& t1, const T2& t2)
{
    bool areTypesTheSame { is_same_v<T1, T2> };
    cout << format("'{}' and '{}' are {} types.", t1, t2,
    (areTypesTheSame ? "the same" : "different")) << endl;
}

// 实现 move_assign_if_noexcept()
// convert a given reference to an rvalue reference if the move assignment operator is marked noexcept, and to a reference-to-const otherwise.

// std::conditional<type, if_true, if_false>  is_nothrow_move_assignable
// constexpr conditional_t<is_nothrow_move_assignable_v<T>, T&&, const T&>
template <typename T>
constexpr std::conditional<is_nothrow_move_assignable_v<T>, T&&, const T&>::type
move_assign_if_noexcept(T& t) noexcept {
    return move(t);
}

// enable_if: based on substitution failure is not an error (SFINAE)
//  It stipulates that a failure to specialize a function template for a given set of template parameters should not be seen as a compilation error.

template <typename T1, typename T2>
enable_if_t<is_same_v<T1, T2>, bool> // is_same_v<T1, T2> -> true, return type is bool, other wise there is no type
checkType(const T1& t1, const T2& t2)
{
    cout << format("'{}' and '{}' are the same types.", t1, t2) << endl;
    return true;
}
template <typename T1, typename T2>
enable_if_t<!is_same_v<T1, T2>, bool>
checkType(const T1& t1, const T2& t2)
{
    cout << format("'{}' and '{}' are different types.", t1, t2) << endl;
    return false;
}

class IsDoable {
public:
    virtual void doit() const {};
};

class Derived : public IsDoable {};
template <typename T>
void callDoit(const T& [[maybe_unused]] t)
{
    if constexpr (is_base_of_v<IsDoable, T>) {
        t.doit();
    } else {
        cout << "Cannot call doit()!" << endl;
    }
}

// is_invocable determines whether a given function can be called with a given set of arguments
template <typename T>
void callDoit(const T& [[maybe_unused]] t)
{
    if constexpr (is_invocable_v<decltype(&IsDoable::doit), T>) {
        t.doit();
    } else {
        cout << "Cannot call doit()!" << endl;
    }
}

// logical ops: &&, ||  -
cout << conjunction_v<is_integral<int>, is_integral<short>> << " ";
cout << conjunction_v<is_integral<int>, is_integral<double>> << " ";
cout << disjunction_v<is_integral<int>, is_integral<double>, is_integral<short>> << " ";
cout << negation_v<is_integral<int>> << " ";
// 1 0 1 0


// static_assert test
static_assert(sizeof(void*)==8, "required 64-bit compiler!");

// template, requires t is an integral.
void foo(const integral auto& t) {}
```



















