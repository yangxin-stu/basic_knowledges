## goals
1. How to use function pointers
2. How to use pointers to class methods
3. How to use polymorphic function wrappers
4. What function objects are and how to write your own
5. Details of lambda expressions

### basic concepts
1. Functions in C++ are so-called `first-class functions`, as functions can be used in the same way as normal variables, such as passing them as arguments to other functions, returning them from other functions, and assigning them to variables. 
2. A term that often comes up in this context is a `callback`, representing something that can be called. It can be a function pointer or something that behaves like a function pointer, such as an object with an overloaded operator(), or an inline lambda expression. 
3. A class that overloads operator() is called a function object, or functor for short.
4. Functions that accept other functions as parameters, or functions that return a function are called higher-order functions. 


### function pointers
```cpp
using Matcher = bool (*)(int, int) {return true;}
using MatcherHandle = void (*)(size_t, int , int) {
    cout << format("Match found at position {} ({}, {})", position, value1, value2) << endl;
}

void findMatches(span<const int> values1, span<const int> values2, Matcher matcher, MatchHandler handler)
{
    if (values1.size() != values2.size()) { return; } // Must be same size.
    for (size_t i { 0 }; i < values1.size(); ++i) {
        if (matcher(values1[i], values2[i])) {
            handler(i, values1[i], values2[i]);
        }
    }
}


```

#### a windows dll example
```cpp
HMODULE lib {::LoadLibrary("hardware.dll")};
// example function Connet in hardware.dll
using ConnectFunction = int(__stdcall *)(bool, int, const char*);
ConnectFunction connect { (ConnectFunction)::GetProcAddress(lib, "Connect") };
connect(true, 3, "Hello world");
```

#### pointers to methods and members
```cpp
int (Employee::*methodPtr)() const {&Employee::getSalary}; // variable name is methodPtr
Employee employee { "John", "Doe" };
cout << (employee.*methodPtr)() << endl;

int (Employee::*methodPtr) () const { &Employee::getSalary };
Employee* employee { new Employee { "John", "Doe" } };
cout << (employee->*methodPtr)() << endl;

using PtrToGet = int (Employee::*) () const;
PtrToGet methodPtr { &Employee::getSalary };
Employee employee { "John", "Doe" };
cout << (employee.*methodPtr)() << endl;

auto methodPtr { &Employee::getSalary };
Employee employee { "John", "Doe" };
cout << (employee.*methodPtr)() << endl;
```

#### std::function
1. The `std::function` function template, defined in `<functional>`, is a` polymorphic function wrapper`, which can be used to create a type that can point to anything that is callable such as a `function`, a `function object`, or a `lambda expression`; 
2. `std::function` behaves as function pointer

```cpp
// return type and parameters
template <typename R, typename ArgType ...>
std::function<R(ArgTypes...)>;

int f1(int num, string_view str) {return 0;}
std::function<int (int, string_view)> fptr{f1};
// or
std::function f2 {f1}; // CTAD

// using Matcher = std::function<bool(int, int)>;

// a better way
template<typename Matcher, typename MatchHandler>
void findMatches(span<const int> values1, span<const int> values2,
Matcher matcher, MatchHandler handler)
{ /* ... */ }

// or:
void findMatches(span<const int > v1, span<const int> v2, auto Matcher, auto MatcherHandle);
```

### function objects
1. standard algorithm: `find_if()`, `accumulate()` accepts callbacks.
2. class template: `plus`, `minus`, `multiplies`, `divides`, `modulus`, `negate`
3. It’s recommended to always use the transparent operator functors like `std::multiplies<> {}`. heterogeneous lookups.
4. comparision functors: `equal_to`, `not_equal_to`, `less`, `greater`, `less_equal`, `greater_equal`
5. logical functors: `logical_and`, `logical_or`, `logical_not`
6. bitwise functors: `bit_and`, `bit_or`, `bit_xor`, `bit_not`
7. Binders can be used to bind parameters of `callables` to certain values. For this you use `std::bind()`




```cpp
class IsLargerThan
{
public:
    explicit IsLargerThan(int value) : m_value { value } {}
    bool operator()(int value1, int value2) const {
        return value1 > m_value && value2 > m_value;
    }
private:
    int m_value;
};

plus<int> myPlus; // function object: accept two ints
int res {myPlus(4, 6)};

template <typename Iter, typename StartValue, typename Operation>
auto accumulateData(Iter begin, Iter end, StartValue startValue, Operation op)
{
    auto accumulated { startValue };
    for (Iter iter { begin }; iter != end; ++iter) {
        accumulated = op(accumulated, *iter);
    }
    return accumulated;
}


// transparent operator functors: omit template value argument
double geometricMeanTransparent(span<const int> values)
{
    // 3rd argument: 1 for int, 1.0 for double!
    auto mult { accumulateData(cbegin(values), cend(values), 1, multiplies<>{}) };
    return pow(mult, 1.0 / values.size());
}

priority_queue<int, vector<int>, greater<>> myQueue; // result: 1, 2, 3, 4   greater

// heterogenous lookups
set<string, less<>> mySet;
auto i1 { mySet.find("Key") }; // No string constructed, no memory allocated.
auto i2 { mySet.find("Key"sv) }; // No string constructed, no memory allocated.

// c++ 20  unordered container
class Hasher
{
public:
    using is_transparent = void;
    size_t operator()(string_view sv) const { return hash<string_view>{}(sv); }
};

unordered_set<string, Hasher, equal_to<>> mySet;
auto i1 { mySet.find("Key") }; // No string constructed, no memory allocated.
auto i2 { mySet.find("Key"sv) }; // No string constructed, no memory allocated.


// binders
void func(int num, string_view sv) {
    // do your job here.
}

string myString {"world!"};
// _1 means first parameter of func
auto f1 {std::bind(func, std::placeholders::_1, myString)};
f1(4); // call func(4, string("world!"));

// rearrage arguments
auto f2 {std::bind(func, std::placeholders::_2, std::placeholders::_1)};
f2("test", 4);


// deal with reference
void increment(int& val) {++val;}
int ind {0};
increment(ind);

// but for binding... it makes a copy
auto incr {std::bind(increment, ind)};
incr(); // not change ind
auto incr2 {std::bind(increment, ref(ind))};
incre2(); // change ind
 

// overload functions
void overloaded(int num) {}
void overloaded(float f) {}

auto f4 { bind((void(*)(float))overloaded, placeholders::_1) }; // OK


// class method
class Handler
{
public:
    void handleMatch(size_t position, int value1, int value2)
    {
        cout << format("Match found at position {} ({}, {})", position, value1, value2) << endl;
    }
};

// a object pointer (this) is the real first argument for a class method
Handler handler;
findMatches(values1, values2, intEqual, bind(&Handler::handleMatch, &handler,
placeholders::_1, placeholders::_2, placeholders::_3));

auto greaterEqualTo100 { bind(greater_equal<>{}, placeholders::_1, 100) };


template <typename Matcher>
void printMatchingStrings(const vector<string>& strings, Matcher matcher)
{
    for (const auto& string : strings) {
        if (matcher(string)) { cout << string << " "; }
    }
}

// not_fn() a function adapter. accept a function or method. negate the result of passed function. (bool value)
vector<string> values { "Hello", "", "", "World", "!" };
printMatchingStrings(values, not_fn(mem_fn(&string::empty)));

```

### lambda expression
1. anonymous (unnamed) functions represented by `lambda expressions`
2. The `lambda expressions` are called `stateless` if they don’t capture anything from the enclosing scope.
3. [=] capture by value; [&] capture by reference; [this]: capture current object. [*this]
4. Global variables are always captured by `reference`, even if asked to capture by `value`! capturing a global variable explicitly is not allowed  
5. Lambda capture expressions allow you to initialize capture variables with any kind of expression. It can be used to introduce variables in the lambda expression that are not captured from the enclosing scope.

```cpp
auto parametersLambda {
[](int value){ cout << "The value is " << value << endl; } };

class CompilerGeneratedName
{
public:
    auto operator()(int value) const {
        cout << "The value is " << value << endl; 
    }
};

class Person ; // definition is omitted
[] (const Person& person) -> decltype(auto) { return person.getName(); }

const double data { 1.23 };
auto capturingLambda { [data] (params) (mutable if const for double) { cout << "Data = " << data << endl; } }; 

class CompilerGeneratedName
{
public:
    CompilerGeneratedName(const double& d) : data { d } {}
    auto operator()() const { cout << "Data = " << data << endl; }
private:
    double data;
};

// lambda expression
[capture_block] <template_params> (parameters) mutable constexpr
noexcept_specifier attributes
-> return_type requires {body}

// 1. as parameters (callbacks)
findMatches(values1, values2,
[](int value1, int value2) { return value1 == value2; },
printMatch);

// 2. generic (template)
auto areEqual { [](const auto& value1, const auto& value2) {
return value1 == value2; } };
findMatches(values1, values2, areEqual, printMatch);

// 3. lambda capture expressions
double pi { 3.1415 };
auto myLambda { [myCapture = "Pi: ", pi]{ cout << myCapture << pi; } };

auto myPtr { make_unique<double>(3.1415) };
auto myLambda { [p = move(myPtr)] { cout << *p; } };


// 4. tempated lambda expressions
auto lambda { [](const auto& values) {
    using V = decay_t<decltype(values)>; // The real type of the vector. remove const and reference
    using T = typename V::value_type; // The type of the elements of the vector.
    T someValue { };
    T::some_static_function();
    } 
};

// other examples
[] <typename T> (const vector<T>& values) {
    T someValue { };
    T::some_static_function();
}

[] <typename T> (const T& value1, const T& value2) requires integral<T> {/* ... */}

// 5. as return type
function<int(void)> multiplyBy2Lambda(int x)
{
    return [x]{ return 2 * x; };
}

function<int(void)> fn {multiplyBy2Lambda(5)};
cout << fn() << endl; // 10

// 6. unevaluated contexts
using LambdaType = decltype([](int a, int b) { return a + b; });

// 7. std::invoke() used to call any callable object with a set of parameters.   thread??

void printMessage(string_view message) { cout << message << endl; }
int main()
{
    invoke(printMessage, "Hello invoke.");
    invoke([](const auto& msg) { cout << msg << endl; }, "Hello invoke.");
    string msg { "Hello invoke." };
    cout << invoke(&string::size, msg) << endl;
}
```