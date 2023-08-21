## goals
Containers overview: `requirements` on elements and general `error handling`
Sequential containers: `vector`, `deque`, `list`, `forward_list`, and `array`
How to use the `span` class
Container adapters: `queue`, `priority_queue`, and `stack`
Associative containers: the `pair` utility, `map`, `multimap`, `set`, and `multiset`
Unordered associative containers or hash tables: `unordered_map`, `unordered_multimap`, `unordered_set`, and `unordered_multiset`
Other containers: standard C-style arrays, strings, streams, and bitset


### requirements and error checking
1. value semantics. store a copy of elements, assign to elements with the assignment operator, and destroy elements with the destructor. elements: `copyable`
2. `reference semantics`: store pointers to elements (`std::unique_ptr` or `std::shared_ptr`); `std::reference_wrapper` make references copyable
3. `copy`, `move`, `destructor`, `default constructor`, `compare`
4. `out-of-bounds`

```cpp
// overview
template <class T, class Allocator = std::allocator<T>> 
class vector;

template <class Key, class T, class Compare = std::less<Key>,
class Allocator = std::allocator<std::pair<const Key, T>>> 
class map;


```
### vector
1. C++20, like `std::string`, `std::vector` is `constexpr`, can be used to perform operations at compile time and that it can be used in the implementation of `constexpr` functions and classes. 
2. Iterators allow you to insert and delete elements and sequences of elements at any point in the container compared with indexing
3. Iterators allow you to use the Standard Library algorithms
4. Iterators are more efficient than indexing
5. If you don’t control the reallocations explicitly, you should assume that all insertions cause a reallocation and thus invalidate all iterators.

```cpp
using namespace std;
vector<int> v{1,2,3,4,5};
// constructor
vector<int> v1; // zero elems
vector<int> v2(10, 100); // 10 ints with value 100
vector<int> v3 {10, 100}; // two elems: 10 and 100
auto v4 {make_unique<vector<int>>({10, 100})};  // v4->push_back(1000);.


v.at(0); // roughly equal to v[0]
v.front();
v.back();
v.size();
v.push_back(6);
// 5 ints with value 100 or use initializer_list {100, 100, 100, 100, 100}
v2.assign(5, 100); // remove all the current elems and adds any number of new elements.
v2.swap(v3);
// coparision. requires: elements are comparable
v2 < v3;

// vector iterator
for (auto iter {cbegin(v2)}; iter != cend(v2); ++iter) {
    // do something like: *iter *= 2;
}

vector<int> intVector(10);
auto it {intVector.begin()};
*it;
it++;
it+=5;

// storing references
string str1 { "Hello" };
string str2 { "World" };
vector<reference_wrapper<string>> vec{ref (str1)};
vec.push_back(ref(2));
vec[1].get() += "!"; // get(): get access to the object wrapped by a reference_wrapper

// adding/removing elems
v2.clear(); // remove all elements
v2.assign(5, 1);
vector<int> vectorOne {1,2,3,5};
vectorOne.insert(cbegin(vectorOne)+3, 4); // pos, val
// .insert(pos, [left, right))
// .insert(pos, n, val)  the number of val is n

// non-member function
vector vals{1,2,3,2,1,2,4,5};
std::erase(vals, 2); // erase elem == 2
std::erase_if(vals, [](int i){return i==2;});

// move semantics
vector<string> v5 {"string"};
v5.push_back(string(5, 'a'));

// emplace operations  since C++ 17, return reference to inserted element
v5.emplace_back(5, 'a'); // construct a new element and insert it into container

// size, capacity
v.size();
v.empty();
// or
std::size(v);
std::empty(v);
v.resize(v_temp.size());

```

### deque
1. Elements are not stored contiguously in memory.
2. A deque supports true constant-time insertion and removal of elements at both the front and the back (a vector supports amortized constant time at just the back)
3. A deque does not invalidate iterators when inserting elements at the front or at the back
4. A deque does not expose its memory management scheme via reserve() or capacity().

### list
1. standard doubly linked list.
2. Most of the list operations are identical to those of vector, including the constructors, destructor, copying operations, assignment operations, and comparison operations.
3. Lists do not provide random access to elements. (`operator[]`)
4. `size()`, `empty()`, `resize()`, not `reserve()`, `capacity()`
5. `splicing`: insert an entire `list` into another `list`. Splicing is destructive to the list passed as an argument: it removes the spliced elements from one list to insert them into the other.
6. `list` specific method: `remove`, `remove_if`, `unique`, `merge`, `sort`, `reverse`

```cpp
// Store the a words in the main dictionary.
list<string> dictionary { "aardvark", "ambulance" };
// Store the b words.
list<string> bWords { "bathos", "balderdash" };
// Add the c words to the main dictionary.
dictionary.push_back("canticle");
dictionary.push_back("consumerism");
// Splice the b words into the main dictionary.
if (!bWords.empty()) {
    // Get an iterator to the last b word.
    auto iterLastB { --(cend(bWords)) };
    // Iterate up to the spot where we want to insert b words.
    auto it { cbegin(dictionary) };
    for (; it != cend(dictionary); ++it) {
        if (*it > *iterLastB)
            break;
    }
    // Add in the b words. This action removes the elements from bWords.
    dictionary.splice(it, bWords);
}
// Print out the dictionary.
for (const auto& word : dictionary) {
    cout << word << endl;
}
```

### forward_list and array
```cpp
// Create 3 forward lists using an initializer_list
// to initialize their elements (uniform initialization).
forward_list<int> list1 { 5, 6 };
forward_list list2 { 1, 2, 3, 4 }; // CTAD is supported.
forward_list list3 { 7, 8, 9 };
// Insert list2 at the front of list1 using splice.
list1.splice_after(list1.before_begin(), list2);
// Add number 0 at the beginning of the list1.
list1.push_front(0);
// Insert list3 at the end of list1.
// For this, we first need an iterator to the last element.
auto iter { list1.before_begin() };
auto iterTemp { iter };
while (++iterTemp != end(list1)) { ++iter; }
list1.insert_after(iter, cbegin(list3), cend(list3));
// Output the contents of list1.
for (auto& i : list1) { cout << i << ' '; }
// 0 1 2 3 4 5 6 7 8 9

// fixed-size array
// Create an array of 3 integers and initialize them
// with the given initializer_list using uniform initialization.
array<int, 3> arr { 9, 8, 7 };
// Output the size of the array.
cout << "Array size = " << arr.size() << endl; // or std::size(arr);
// Output the contents using a range-based for loop.
for (const auto& i : arr) {
    cout << i << endl;
}
cout << "Performing arr.fill(3)..." << endl;
// Use the fill method to change the contents of the array.
arr.fill(3);
// Output the contents of the array using iterators.
for (auto iter { cbegin(arr) }; iter != cend(arr); ++iter) {
    cout << *iter << endl;
}
array<int, 3> myArray{ 11, 22, 33 };
cout << std::get<1>(myArray) << endl;
```

### span
1. `<span>`: it allows you to write a single function that works with vectors, C-style arrays, and std::arrays of any size.
2. like `string_view`, a `span` is cheap to copy; unlike `string_view`, `span` can support read/write access to the underlying elements
3. A subview can be created from an existing `span` using the `subspan()` method. Its first argument is the `offset` into the span, and the second argument is the `number` of elements to include in the subview.
4. When writing a function accepting a `const vector<T>&`, consider accepting a `span<const T>` instead

```cpp
void print(span<int> values) {
    for (const auto& val: values) {cout << val << endl;}
}


vector v { 11, 22, 33, 44, 55, 66 };
// Pass the whole vector, implicitly converted to a span.
print(v);
// Pass an explicitly created span.
span mySpan { v };
print(mySpan);
// Create a subview and pass that.
span subspan { mySpan.subspan(2, 3) };
print(subspan);
// Pass a subview created in-line.  .data() return a pointer to the first element
print({ v.data() + 2, 3 }); 
// Pass an std::array.
array<int, 5> arr { 5, 4, 3, 2, 1 };
print(arr);
print({ arr.data() + 2, 3 });
// Pass a C-style array.
int carr[] { 9, 8, 7, 6, 5 };
print(carr); // The entire C-style array.
print({ carr + 2, 3 }); // A subview of the C-style array.
```

### queue adapters
1. `<queue>`: standard first-in, first-out semantics.
2. `<priority_queue>`
3. `<stack>`
4. `top()`, `pop()`

### pair
```cpp
// Two-argument constructor and default constructor
pair<string, int> myPair { "hello", 5 };
pair<string, int> myOtherPair;
// Can assign directly to first and second
myOtherPair.first = "hello";
myOtherPair.second = 6;
// Copy constructor
pair<string, int> myThirdPair { myOtherPair };
// operator<
if (myPair < myOtherPair) {
    cout << "myPair is less than myOtherPair" << endl;
} else {
    cout << "myPair is greater than or equal to myOtherPair" << endl;
}
// operator==
if (myOtherPair == myThirdPair) {
    cout << "myOtherPair is equal to myThirdPair" << endl;
} else {
    cout << "myOtherPair is not equal to myThirdPair" << endl;
}

pair<int, double> pair1 { make_pair(5, 10.10) };
auto pair2 { make_pair(5, 10.10) };
pair pair3 { 5, 10.10 }; // CTAD
```

### map
1. stores key/value pairs instead of just single values
2. insertion, lookup, deletion are all based on `keys`
3. A `map` keeps elements in `sorted` order, based on the `keys`, so that insertion, deletion, and lookup all take logarithmic time. 
4. map stores `pair<key, value>` as elements.
5. A `node handle` can only be moved and is the owner of the element stored in a node. It provides read/write access to both the key and the value.

```cpp
map<string, int> m {
    {"string"s, 1},
    {"world!"s, 2},
    {"lili"s, 3}
};

// insert new pairs or initializer_list. do not overwrite old value
auto ret {m.insert({"string1", 4})}; // ret is a pair (<map<string, int>::iterator, bool>)
if (ret.second) {
    printf("inserted successfully");
} else {
    puts("failure");
}

// insert_or_assign()  do overwrite old value
ret = m.insert_or_assign(make_pair("sss", 4));

// operator[] always success. create a new element
ret["sss"] = 4;

// emplace(), emplace_hint(), try_emplace()

// iterator
for (auto iter{m.cbegin()}; iter != m.cend(); ++iter) {}

// lookup elements
m["hello"] = 4;

auto it {m.find("world!")};
if (it != std::end(m)) {
    puts("found!");
}

auto isKeyInMap {m.contains("sss")}; // true

// removing elems
m.erase("sss");

// node and node handles
// extract
map<string, int> m2 {
    {"asdsf"s, 1},
    {"s"s, 2}
};

auto extractedNode {m2.extract("s")};
m2.insert(move(extractedNode));

map<int, int> src { {1, 11}, {2, 22} };
map<int, int> dst { {2, 22}, {3, 33}, {4, 44}, {5, 55} };
dst.merge(src);
``` 

### unordered associative containers
```cpp
class IntWrapper
{
public:
    IntWrapper(int i): m_wrappedInt{i} {}
    int getValue() const {return m_wrappedInt;}
    bool operator==(const IntWrapper& rhs) const = default; // element-wise, sinec++20
private:
    int m_wrappedInt;
};

namespace std {
    template <>
    struct hash<IntWrapper> 
    {
        size_t operator()(const IntWrapper& x) const {
            return std::hash<int>{}(x.getValue()); // construct a hash object(callable)
        }
    };
}
unordered_map<int, string> m {
    {1, "Item 1"},
    {2, "Item 2"},
    {3, "Item 3"},
    {4, "Item 4"}
};
// Using C++17 structured bindings.
for (const auto& [key, value] : m) {
    cout << format("{} = {}", key, value) << endl;
}
// Without structured bindings.
for (const auto& p : m) {
    cout << format("{} = {}", p.first, p.second) << endl;
}
```