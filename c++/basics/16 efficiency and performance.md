### definition
1. efficiency: running without wasted effort
2. performance: speed, disk access, memory usage, network usage


### two approaches to efficiency
1. `Language-level efficiency`: pass reference instead of value. (good coding style...)
2. `design-level efficiency`: algorithm, data structures, ...
3. separate the performance capabilities of the language itself from the effectiveness of its compilers at optimizing it
4. if you don’t use a feature, you don’t need to pay for it.

### improve language-level efficiency
1. Prefer pass-by-value parameters for parameters that a function inherently would copy, but only if the parameter is of a type that supports move semantics. Otherwise, use reference-to-const parameters.
2. When you pass an object of a derived class by value as an argument for a function parameter that has one of the base classes as its type, then the derived object is sliced to fit into the base class type.
3. If a function must modify an object, pass the object by reference-to-nonconst. If the function should not modify the object, pass it by value or by reference-to-const.
4. Avoid using pass-by-pointer, which is a relatively obsolete method for pass-by-reference. It is a throwback to the C language and thus rarely suitable in C++ (unless passing nullptr has meaning in your design).
5. You could return objects by reference from functions to avoid copying the objects unnecessarily. Unfortunately, it is sometimes impossible to return objects by reference, such as when you write overloaded operator+ and other similar operators. And, you should never return a reference or a pointer to a local object that will be destroyed when the function exits
6. returning objects by value is usually fine. This is due to (named) return value optimization and move semantics
7. you should catch exceptions by reference to avoid slicing and unnecessary copying. Throwing exceptions is heavy in terms of performance
8. You should make sure your classes support move semantics.According to the rule of zero, you should try to design your classes such that the compiler-generated copy and move constructors and copy and move assignment operators are sufficient.
9. The compiler creates temporary, unnamed objects in several circumstances. avoiding it.
10. Return-Value Optimization: in most cases, the compiler optimizes away the temporary variable to avoid all copying and moving.Both NRVO and RVO are forms of copy elision, allowing compilers to avoid any copying and moving of objects that are returned from a function. This results in zero-copy pass-by-value semantics. but `return` statement must be a single local variable
11. pre-allocate memory: `std::vector`, elements contiguously in memory. if growing size ... may allocate new memory ...
12. use `inline method and function`. but throws away a fundamental design principle about implementation and interface. In Visual C++, this feature (without `inline` but still `inline`) is called `link-time code generation` (LTCG) and supports cross-module inlining. The GCC compiler calls it `link time optimization` (LTO).


```cpp
class B {
public:
	B(double d) : dd{ d } {}
private:
	double dd;
};

class A {
public:
	A(B d) : b{ d } {}
private:
	B b;
};
void process(const A& a) {}


// int to double is OK! number convertion.
process(4); // error! int->double->B->A cannot from double to A 两步转换！ 
process(4.0); // error!
process(B(4)); // OK!   只支持一步转换！


class Person;
Person processPerson() {
    // ...
    if (someCondition()) {
        return person1;
    } else {
        return person2;
    }
}

```

### improve design-level efficiency

#### caching where neccessary
1. Caching means storing items for future use to avoid retrieving or recalculating them.
2. slow ops: disk access; network communication; computations; object allocation; create thread
3. One common problem with caching is that the data you store often comprises only copies of the underlying information. The original data might change during the lifetime of the cache. So  you need a mechanism for `cache invalidation`: when the underlying data changes, you must either stop using your cached information or repopulate your cache.
4. request that the entity managing the underlying data notifies your program of the data change. `callback`; your program could poll for certain events that would trigger it to repopulate the cache automatically


#### use object pools
1. many kinds of it. one kind of object pool where it allocates a large chunk of memory at once, in which the pool creates smaller objects in-place. 

##### example
1.  keeps a `vector` of chunks of objects of type `T`
2.  keeps track of free objects in a vector that contains pointers to all free objects. 

```cpp
// Provides an object pool that can be used with any class that provides a default constructor.

// acquireObject() returns an object from the list of free objects. If there are no more free objects, acquireObject() creates a new chunk  of objects.

// The pool only grows: objects are never removed from the pool, until the pool is destroyed.

// acquireObject() returns an `std::shared_ptr` with a custom deleter that automatically puts the object back into the object pool when the `shared_ptr` is destroyed and its reference count reaches 0.

export
template <typename T, typename Allocator = std::allocator<T>>
class ObjectPool
{
public:
    ObjectPool() = default;
    explicit ObjectPool(const Allocator& allocator);
    virtual ~ObjectPool();
    // Allow move construction and move assignment.
    ObjectPool(ObjectPool&& src) noexcept = default;
    ObjectPool& operator=(ObjectPool&& rhs) noexcept = default;
    // Prevent copy construction and copy assignment.
    ObjectPool(const ObjectPool& src) = delete;
    ObjectPool& operator=(const ObjectPool& rhs) = delete;
    // Reserves and returns an object from the pool. Arguments can be provided which are perfectly forwarded to a constructor of T.
    template<typename... Args>
    std::shared_ptr<T> acquireObject(Args... args);
private:
    // Contains chunks of memory in which instances of T will be created. For each chunk, the pointer to its first object is stored.
    std::vector<T*> m_pool;
    // Contains pointers to all free instances of T that are available in the pool.
    std::vector<T*> m_freeObjects;
    // The number of T instances that should fit in the first allocated chunk.
    static const size_t ms_initialChunkSize { 5 };
    // The number of T instances that should fit in a newly allocated chunk.
    // This value is doubled after each newly created chunk.
    size_t m_newChunkSize { ms_initialChunkSize };
    // Creates a new block of uninitialized memory, big enough to hold m_newChunkSize instances of T.
    void addChunk();
    // The allocator to use for allocating and deallocating chunks.
    Allocator m_allocator;
};

template <typename T, typename Allocator>
ObjectPool<T, Allocator>::ObjectPool(const Allocator& allocator)
: m_allocator { allocator }
{
}

template <typename T, typename Allocator>
void ObjectPool<T, Allocator>::addChunk()
{
    std::cout << "Allocating new chunk..." << std::endl;
    // Allocate a new chunk of uninitialized memory big enough to hold m_newChunkSize instances of T, and add the chunk to the pool.
    auto* firstNewObject { m_allocator.allocate(m_newChunkSize) };
    m_pool.push_back(firstNewObject);
    // Create pointers to each individual object in the new chunk
    // and store them in the list of free objects.
    auto oldFreeObjectsSize { m_freeObjects.size() };
    m_freeObjects.resize(oldFreeObjectsSize + m_newChunkSize);
    std::iota(begin(m_freeObjects) + oldFreeObjectsSize, end(m_freeObjects), firstNewObject);
    // Double the chunk size for next time.
    m_newChunkSize *= 2;
}

template <typename T, typename Allocator>
template <typename... Args>
std::shared_ptr<T> ObjectPool<T, Allocator>::acquireObject(Args... args)
{
    // If there are no free objects, allocate a new chunk.
    if (m_freeObjects.empty()) { addChunk(); }
    // Get a free object.
    T* object { m_freeObjects.back() };
    // Initialize, i.e. construct, an instance of T in an
    // uninitialized block of memory using placement new, and
    // perfectly forward any provided arguments to the constructor.
    new(object) T { std::forward<Args>(args)... };
    // Remove the object from the list of free objects.
    m_freeObjects.pop_back();
    // Wrap the initialized object and return it.
    return std::shared_ptr<T> { object, [this](T* object) {
        // Destroy object.
        std::destroy_at(object);
        // Put the object back in the list of free objects.
        m_freeObjects.push_back(object);
    } };
}

template <typename T, typename Allocator>
ObjectPool<T, Allocator>::~ObjectPool()
{
    // Note: this implementation assumes that all objects handed out by this pool have been returned to the pool before the pool is destroyed.
    // The following statement asserts if that is not the case.
    assert(m_freeObjects.size() == 
    ms_initialChunkSize * (std::pow(2, m_pool.size()) - 1));
    // Deallocate all allocated memory.
    size_t chunkSize { ms_initialChunkSize };
    for (auto* chunk : m_pool) {
    m_allocator.deallocate(chunk, chunkSize);
    chunkSize *= 2;
    }
    m_pool.clear();
}


// the usage of such object pools
class ExpensiveObject
{
public:
    ExpensiveObject() { /* ... */ }
    virtual ~ExpensiveObject() = default;
    // Methods to populate the object with specific information.
    // Methods to retrieve the object data.

private:
    // An expensive data member.
    array<double, 4 * 1024 * 1024> m_data;
    // Other data members 
};

using MyPool = ObjectPool<ExpensiveObject>;
shared_ptr<ExpensiveObject> getExpensiveObject(MyPool& pool)
{
    // Obtain an ExpensiveObject object from the pool.
    auto object { pool.acquireObject() };
    // Populate the object. (not shown)
    return object;
}
void processExpensiveObject(ExpensiveObject* object) { /* ... */ }
int main()
{
const size_t NumberOfIterations { 500'000 };
cout << "Starting loop using pool..." << endl;
MyPool requestPool;
auto start1 { chrono::steady_clock::now() };
for (size_t i { 0 }; i < NumberOfIterations; ++i) {
    auto object { getExpensiveObject(requestPool) };
    processExpensiveObject(object.get());
}
auto end1 { chrono::steady_clock::now() };
auto diff1 { end1 - start1 };
cout << format("{}ms\n", chrono::duration<double, milli>(diff1).count());
cout << "Starting loop using new/delete..." << endl;
auto start2 { chrono::steady_clock::now() };
for (size_t i { 0 }; i < NumberOfIterations; ++i) {
    auto object { new ExpensiveObject{} };
    processExpensiveObject(object);
    delete object; object = nullptr;
}
auto end2 { chrono::steady_clock::now() };
auto diff2 { end2 - start2 };
cout << format("{}ms\n", chrono::duration<double, milli>(diff2).count());
}
```

#### profiling
1. bottleneck


### testing
1. What software quality control is and how to track bugs
2. What unit testing means
3. Unit testing in practice using the Visual C++ Testing Framework
4. What fuzz testing or fuzzing means
5. What integration, system, and regression testing means


#### unit testing
1. Unit tests are pieces of code that exercise specific functionality of a class or subsystem. These are the finest-grained tests that you could possibly write. 
2. divide your unit tests into logical groups of tests, called test classes.
3. In real-world scenarios, you usually divide the testing code and the code you want to test into separate projects. 
4. `Fuzz testing`, also known as `fuzzing`, involves a fuzzer that automatically generates random input data for a program or component to try to find unhandled edge cases. 
5. `Higher-level tests` focus on how pieces of the product work together, as opposed to the relatively narrow focus of unit tests.
6. An integration test covers areas where components meet. Unlike a unit test, which generally acts on the level of a single class, an integration test usually involves two or more classes. Integration tests excel at testing interactions between two components, often written by two different programmers.
7. System tests operate at an even higher level than integration tests. These tests examine the program as a whole. 












