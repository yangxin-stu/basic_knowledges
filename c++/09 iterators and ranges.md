### goals
Details on iterators
How to use stream iterators
What iterator adapters are, and how to use the standard iterator adapters
The power of the ranges library consisting of ranges, range-based algorithms, projections, views, and factories

### iterators
1. The Standard Library uses the `iterator` pattern to provide a generic abstraction for accessing the elements of a container
2. you can usually use `operator*` and `operator->` on the iterator to access the actual element or field of the element
3. All iterators must be `copy constructible`, `copy assignable`, and `destructible`. 
4. `mutable iterator`: requires `output iterator`

#### different iterator types
1. input(read): Provides `read-only` access, `forward` only. Iterators can be assigned, copied, and compared for equality.
2. output(write): Provides write-only access, forward only. Iterators can be assigned, but cannot be compared for equality.
3. forward: `input` + `default constructor`
4. bidirectional: `forward` + `operator--`
5. random access: `bidirectional` + `comparison`, integer algorighm for `plus` and `subtract`, `subscription`
6. contiguous: `random` + `adjacent memory`

#### getting iterators
1. `iterator` or `const_iterator` or `reverse_iterator` or ...
2. methods or functions: `begin(), end(), cbegin(), cend(), rbegin(), rend()` in `<iterator>`
3. It’s recommended to use these `nonmember` functions instead of the member versions
4. `ADL`: argument-dependent lookup. use `begin(params)` rather than `std::begin(params)`
5. `std::distance()`: get the distance between two iterators

#### iterator traits
1. why need? know the type of elems to create temporary value or get the type of iterator 
2. `value_type`, `difference_type`, `iterator_catogary`, `pointer`, `reference` 
3. class template `iterator_traits`

```cpp
template <typename IteratorType> 
void iteratorTypeTest(IteratorType it) {
    // value_type 是 类型
    typename iterator_traits<IteratorType>::value_type temp;
    temp = *it;
    cout << temp << endl;
}

template <typename Iter>
auto myFind(Iter begin, Iter end, const typename iterator_traits<Iter>::value_type& value)
{
    for (auto iter { begin }; iter != end; ++iter) {
        if (*iter == value) { return iter; }
    }
    return end;
}
```

#### stream iterators
1. These are `iterator-like class templates` that allow you to treat input and output streams as input and output iterators.
2. `ostream_iterator`, `iostream_iterator`

```cpp
template <typename Input, typename Output>
void myCopy(Input beg, Input en, Output target) {
    for (auto st{beg}; st!=en; ++st, ++target) {
        *target = *st;
    } 
}

myCopy(cbegin(myVector), cend(myVector), ostream_iterator<int> {std::cout, " "});


template <typename InputIter>
auto sum(InputIter begin, InputIter end)
{
    auto sum { *begin };
    for (auto iter { ++begin }; iter != end; ++iter) { sum += *iter; }
    return sum;
}
cout << "Enter numbers separated by whitespace." << endl;
cout << "Press Ctrl+Z followed by Enter to stop." << endl;
istream_iterator<int> numbersIter { cin };
istream_iterator<int> endIter;
int result { sum(numbersIter, endIter) };
cout << "Sum: " << result << endl;
```

#### iterator adapters
1. `back_insert_iterator`: push_back
2. `front_insert_iterator`: push_front
3. `insert_iterator`: insert
4. `reverse_iterator`:
5. `move_iterator`: The dereferencing operator of a `move_iterator` automatically converts the value to an `rvalue reference`
6. obtain the underlying `iterator` from a `reverse_iterator` by calling its `base()` method. the `iterator` returned from `base()` always refers to one element past the element referred to by the `reverse_iterator` on which it’s called. To get to the same element, you must subtract one.

```cpp
vector<int> vector1 {1,2,3,4,5};
vector<int> vector2;

back_insert_iterator<vector<int>> inserter {vector2}; 
// vector2.resize(vector1.size()), vector2.begin() ...
myCopy(cbegin(vector1), cend(vector1), inserter); // or std::back_inserter(vector2)
myCopy(cbegin(vector2), cend(vector2), ostream_iterator<int> {cout, " "});


vector myVector { 11, 22, 33, 22, 11 };
auto it1 { myFind(begin(myVector), end(myVector), 22) };
auto it2 { myFind(rbegin(myVector), rend(myVector), 22) };
if (it1 != end(myVector) && it2 != rend(myVector)) {
    cout << format("Found at position {} going forward.", distance(begin(myVector), it1)) << endl;
    cout << format("Found at position {} going backward.", distance(begin(myVector), --it2.base())) << endl;
} else {
    cout << "Failed to find." << endl;
}

class MoveableClass
{
public:
    MoveableClass() {
        cout << "Default constructor" << endl;
    }
    MoveableClass(const MoveableClass& src) {
        cout << "Copy constructor" << endl;
    }
    MoveableClass(MoveableClass&& src) noexcept {
        cout << "Move constructor" << endl;
    }
    MoveableClass& operator=(const MoveableClass& rhs) {
        cout << "Copy assignment operator" << endl;
        return *this;
    }
    MoveableClass& operator=(MoveableClass&& rhs) noexcept {
        cout << "Move assignment operator" << endl;
        return *this;
    }
};
vector<MoveableClass> vecSource;
MoveableClass mc;
vecSource.push_back(mc);
vecSource.push_back(mc);

vector<MoveableClass> vecTwo { make_move_iterator(begin(vecSource)),
make_move_iterator(end(vecSource)) };

vector<MoveableClass> vecTwo { move_iterator { begin(vecSource) },
move_iterator { end(vecSource) } };
```

### ranges
1. an abstraction layer on top of iterators, eliminating mismatching iterator errors, and adding extra functionality such as allowing range adapters to lazily filter and transform underlying sequences of elements.
2. A `range` is a `concept` defining the `requirements` for a type that allows iteration over its elements. ex: data structures support `begin()` and `end()`
3. `Range-based algorithms`: accepting iterator pairs to perform their work.
4. `Projection`: callback function; This `callback` is called for each element in the range and can `transform` an element to some other value `before` it is passed to the algorithm.
5. `Views`: view can be used to `transform` or `filter` the elements of an underlying range. Views can be composed together to form so-called pipelines of operations to be applied to a range.
6. `Factories`: construct a view that produces values on demand.
7. retrieve elems in ranges: `ranges::begin()`, `ranges::end()`, `ranges::data()`, `ranges::size()` ...
8. concepts: `ranges::random_access_range` and so on...
9. A view itself is also a range, but not every range is a view. A container is a range but not a view, as it owns its elements.

#### range-based algorithms
```cpp
using namespace std;
vector<int> data {11, 22, 33};

std::ranges::sort(data);
// not  std::sort(begin(data), end(data));


// projections
class Person
{
public:
    Person(string first, string last) : m_firstName { move(first) }, m_lastName { move(last) } { }
    const string& getFirstName() const { return m_firstName; }
    const string& getLastName() const { return m_lastName; }
private:
    string m_firstName;
    string m_lastName;
};
vector persons { Person {"John", "White"}, Person {"Chris", "Blue"} };

// projection is the third arguments
ranges::sort(persons, {},[](const Person& person) { return person.getFirstName(); });
// or 
ranges::sort(persons, {}, &Person.getFirstName);

// the good properties of views
// 1. lazily evaluated
// 2. nonowning
// 3. nonmutating: view cannot change the origin container or range.

// create a view: range adapters (accept a range, other params(optional))
using namespace std::ranges::views; // std::views == std::ranges::views;
// class
std::ranges::filter_view;
// functions
views::all;
views::filter(ranges, predicate);
views::transform;
views::take; // first n elems
views::take_while ;
views::drop v6;
views::drop_while ;
views::reverse ;
views::elements ;
views::keys ; // pair-like range
views::values;
views::common; // begin() and end() might return different types

void printRange(string_view message, auto& range) {
    cout << message << endl;
    for (const auto& value: range) {cout << value << " ";}
    cout << endl;
}

vector values { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
printRange("Original sequence: ", values);

// filter
auto result1 {values 
    | views::filter ([](const auto& value) {return value%2==0;})
};
printRange("Only even values: ", result1);

// transform
// Transform all values to their double value.
auto result2 { result1
    | views::transform([](const auto& value) { return value * 2.0; }) };
printRange("Values doubled: ", result2);


// Drop the first 2 elements.
auto result3 { result2 | views::drop(2) };
printRange("First two dropped: ", result3);

// Reverse the view.
auto result4 { result3 | views::reverse };
printRange("Sequence reversed: ", result4);


vector values { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
printRange("Original sequence: ", values);
auto result { values
| views::filter([](const auto& value) { return value % 2 == 0; })
| views::take(3)
| views::transform([](const auto& v) { return format(R"("{}")", v); }) };
printRange("Result: ", result);

// range factory
// empty_view
// single_view: single element
// iota_view
// istream_view

// start with 10
std::ranges::iota_view values{10};

for (auto value : ranges::istream_view<int>(cin)
| views::take_while([](const auto& v) { return v < 5; })
| views::transform([](const auto& v) { return v * 2; })) {
    cout << format("> {}\n", value);
}
cout << "Terminating..." << endl;
```



