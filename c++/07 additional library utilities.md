## goals
1. How to use the variant and any data types
2. What tuples are and how to use them

### optional
```cpp
std::optional<int> op_i;
op_i = 4;
cout<< op_i.value_or(44) << endl;

cout << std::boolalpha << op_i.has_value() << endl;
```

### variant
1. `std::variant`, defined in `<variant>`, can hold a single value of one of a given set of types. When you define a variant, you have to specify the types it can potentially contain.
2. cannot store `array`, `reference`. but can store `reference_wrapper<T>`

```cpp
// like union.
using namespace std;
variant<int, string, float> v;

class Foo {
    Foo() = delete;
    Foo(int) {}
};
class Bar {
    Bar() = delete;
    Bar(int) {}
};

// error
variant<Foo, Bar> v1; // can not default construct

// ok
variant<std::monostate, Foo, Bar> v2;

variant<int, double, string> v3;
v3 = 0;
v3 = 111.0;
v3 = "hello world!"s;

// get current stored type index
cout << v3.index() << endl;

// judge whether a type is current type
cout << std::holds_alternative<int>(v) << endl;  // false. cause `string`

try {
    cout << std::get<0>(v3) << endl;
} catch (const std::bad_variant_access& ex) {
    cout << ex.what(); 
}

int* theInt {std::get_if<0>(&v)}; // nullptr or anything else
```


### any
1. `.type().name()`
2. `has_value()`

```cpp
using namespace std;
any empty;
any aInt {3};
any aString {"string"s};
cout << "empty.has_value = " << empty.has_value() << endl;  // 0
cout << "anInt.has_value = " << aInt.has_value() << endl << endl; // 1
cout << "anInt wrapped type = " << aInt.type().name() << endl;
cout << "aString wrapped type = " << aString.type().name() << endl << endl;

try{
    int test {std::any_cast<int>(aString)};
} catch(const std::bad_any_cast& ex) {
    cout << ex.what() << endl;
}

std::vector<any> va;
va.push(1), va.push("shasfd"s);
string s {std::any_cast<string>(va[1])};

```

### tuples
```cpp
using myTuple = tuple<int, string, bool>;
myTuple t1{11, "shasd"s, true};

// get the value 
cout << format("{}, {}, {}", get<0>(t1), get<1>(t1), get<2>(t1)) << endl;

// get the type
cout << format("type of t1<1> is: {}", typeid(get<1>(t1)).name()) << endl;

// get tuple size or decltype(t1)
cout << std::tuple_size<myTuple>::value << endl;

double d1=11.0, d2=1123.0;
string te{"fsad"s};
tuple<int, double&, const double&, string&> t2{11, ref(d1), cref(d2), ref(te)}; // <functional>

auto t2 { make_tuple(16, ref(d), cref(d), ref(str1)) };
```

### decompose tuples
```cpp
// 1. structured binging  can not ignore any elems
tuple t1 {11, "hello"s, true}; 
auto [i, str, b] {t1};
// or 
auto& [i2,str2, b2] {t1};

// 2. using std::tie function and special value std::ignore
int i3{0};
bool b3{false};
std::tie(i3, std::ignore, b3) = t1;
```

### concat and comparision
```cpp
tuple t1 { 16, "Test"s, true };
tuple t2 { 3.14, "string 2"s };
auto t3 { tuple_cat(t1, t2) };
// comparision
tuple t1 { 123, "def"s };
tuple t2 { 123, "abc"s };
if (t1 < t2) {
    cout << "t1 < t2" << endl;
} else {
    cout << "t1 >= t2" << endl;
}
```

### std::apply
```cpp
tuple t1 {1, 2};

int add(int a, int b) {
    return a+b;
}
int res {std::apply(add, t1)};
```