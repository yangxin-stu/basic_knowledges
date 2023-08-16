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
