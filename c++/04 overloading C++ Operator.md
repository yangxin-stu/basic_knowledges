## limitations to Operator Overloading
1. You cannot add new operator symbols.
2. cannot overload, such as . and .* (member access in an object), :: (scope resolution operator), and ?: (the conditional operator)
3. The `arity` describes the number of arguments, or operands, associated with the operator. You can only change the arity for the `function call`, `new`, and `delete` operators.
4. You cannot change the `precedence` nor the `associativity` of an operator.
5. You cannot `redefine` operators for `built-in types`.


## choises in operator overloading
1. Operators don’t make sense outside of a class. `operator=`
2. allow the left-hand side of the operator to be a variable of a different type than your class. `operator+`, `operator<<`
3. 不应该重载`operator&` 取地址  `operator,`, `operator&&`, `operator||`
4. Do not use `std::rel_ops`! Instead, just explicitly default or implement `operator<=>` for your classes.

### unary minus and unary plus
返回的是value 而非 ref
```cpp
SpreadSheet SpreadSheet::operator-() const {
    // 原对象不变
    return SpreadSheet {-getValue()};
}
SpreadSheet c1 {4}, c2{-c1};
```

### increment and decrement
```cpp
// 1. ++i --i
SpreadSheet& SpreadSheet::operator++() {
    this->set(getValue() +1);
    return *this;
}

// 2. i++, i--
SpreadSheet SpreadSheet::operator++(int) {
    auto temp {*this};
    ++(*this);
    return temp;
}

```

### insertion and extraction
```cpp
ostream& operator<<(ostream& os, const SpreadSheetCell& cell) {
    os << cell.getValue(); // do not << endl;
    return os;
}

istream& operator>>(istream& is, SpreadSheetCell& cell) {
    double value;
    istr >> value;
    cell.set(value);
    return istr;
}

```

### subscripting
```cpp
template <typename T> 
T& array<T>::operator[](std::size_t idx) {
    if (idx >= m_size) {
        throw std::out_of_range("oh, gosh!");
    }
    return m_elements[m_size];
}

array<int> arr{1,3,4};
// ex
for (size_t i{0}; i<m_size; ++ i) {
    cout << arr[i] << endl;
}

// const version
template <typename T>
const T& Array<T>::operator[](std::size_t idx) const {
    if (idx >= m_size) {
        static T nullValue {T{}};
        return nullValue;
    }
    return m_elements[m_size];
}

// other version: map, ...
template <typename T1, typename T2>
const T& Map<T1, T2>::operator[](const T1& idx) {
    // your code  dependent on your data structure
    if (notFound(idx)) { 
        static T2 nullValue {T2{}};
        return nullValue;
    }
    // else ...
}

```

### function, functor
1. only non-static functions
2. Objects can retain information in their data members between repeated calls to their function call operators.
3. You can customize the behavior of a function object by setting data members.


```cpp
class A {
public:
    A() {}
    ~A() {}
    // function call operator
    int operator() (int a) {
        return a;
    }
};
```

### dereferencing operators
1. overload `*` make objects behave like pointers. it requires that your class has a pointer member.


```cpp
export template <typename T> class Pointer
{
public:
    Pointer(T* ptr) : m_ptr { ptr } {}
    virtual ~Pointer()
    {
        delete m_ptr;
        m_ptr = nullptr;
    }
    // Prevent assignment and pass by value.
    Pointer(const Pointer& src) = delete;
    Pointer& operator=(const Pointer& rhs) = delete;
    // Dereferencing operators will go here.
    T& operator*() { return *m_ptr; }
    const T& operator*() const { return *m_ptr; }
    // operator->
    // cell->setValue()  == (cell.operator())->setValue();
    // it means that -> should return a pointer
    T* operator->() {return m_ptr;}
    const T* operator->() const {return m_ptr;}

    // convert to a ptr
    operator void*() const {
        return m_ptr;
    }
private:
    T* m_ptr { nullptr };
};
```


### conversion operators
```cpp
class A {
public:
    A() {}
    ~A(){}
    // return double 
    operator double() const {
        return getValue();
    }
    // avert implicit A -> string
    explicit operator const auto&() const{
        return std::string("hello world!");
    }
};

```

### user-defined literal operators
1. In `raw mode`, your literal operator receives a sequence of characters
2. In `cooked mode` your literal operator receives a specific interpreted type
3. 123 -> `'1', '2', '3'` for raw mode; 123 -> `123` for cooked mode


```cpp
// cooked mode
// 1. numeric: one param: ull, ll, ld, char, wc, char8_t, ...
// 2. string: two params: first: array; second: length

complex<long double> operator""_i(long double d) {
    return complex<long double> {0, d};
}

complex<long double> c1 {1.2355i}; // image
auto c2 {1.23i};

string operator"" _s(const char* str, size_t len) {
    return string(str, len);
}
string str1 {"hello world!"s};



```