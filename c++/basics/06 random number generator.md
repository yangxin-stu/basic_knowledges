## goals
➤ The concepts of random number engines and engine adapters
➤ How to generate random numbers
➤ How to change the distribution of random numbers

### definitions
1. A `random number engine` is responsible for generating the actual random numbers and storing the state for generating subsequent random numbers. 
2. The `distribution` determines the range of the generated random numbers and how they are mathematically distributed within that range.
3. A `random number engine adapter` modifies the results of a random number engine you associate it with.
4. Software-based random number generators can never generate truly random numbers. They are therefore called pseudorandom number generators (PRNGs) because they rely on mathematical formulas to give the impression of randomness

### C-style random numbers
1. `srand(), rand()`

```cpp
srand(static_cast<unsigned int>(time(nullptr)));
cout << rand() << endl;
int getRandom(int min, int max)
{
    return static_cast<int>(rand() % (max + 1UL - min)) + min;
}
```

### random number engines
1. engines: `random_device` (hardware engine), `linear_congruential_engine`, `mersenne_twister_engine`, `subtract_with_carry_engine`
2. `random_device.entropy()` returns a non-zero number if it can be used.
3. `Linear congruential engine`: Requires a minimal amount of memory to store its state. The state is a single integer containing the last generated random number or the initial seed if no random number has been generated yet. The period of this engine depends on an algorithmic parameter and can be up to 2^64 but is usually less. For this reason, the linear congruential engine should not be used when you need a high-quality random number sequence.
4. `Mersenne twister`: Of the three pseudorandom number engines, this one generates the highest quality of random numbers. The period of a Mersenne twister is a so-called Mersenne prime, which is a prime number one less than a power of two. This period is much bigger than the period of a linear congruential engine. The memory required to store the state of a Mersenne twister also depends on its parameters but is much larger than the single integer state of the linear congruential engine. For example, the predefined Mersenne twister mt19937 has a period of 2^19937−1, while the state contains 625 integers or 2.5 kilobytes. It is also one of the fastest engines.
5. `Subtract with carry engine`: Requires a state of around 100 bytes; however, the quality of the generated random numbers is less than that of the numbers generated by the Mersenne twister, and it is also slower than the Mersenne twister.

```cpp
random_device rnd;
cout << "Entropy: " << rnd.entropy() << endl;
cout << "Min value: " << rnd.min()
<< ", Max value: " << rnd.max() << endl;
cout << "Random number: " << rnd() << endl;


using mt19937 = mersenne_twister_engine<uint_fast32_t, 32, 624, 397, 31,
0x9908b0df, 11, 0xffffffff, 7, 0x9d2c5680, 15, 0xefc60000, 18, 1812433253>;
```

### random number engine adapters
1. The `discard_block_engine` adapter generates random numbers by discarding some of the values generated by its base engine. It requires three parameters: the engine to adapt, the block size `p`, and the used block size `r`. The base engine is used to generate `p` random numbers. The adapter then discards `p-r` of those numbers and returns the remaining r numbers.
2. The `independent_bits_engine` adapter generates random numbers with a given number of bits `w` by combining several random numbers generated by the base engine.
3. The `shuffle_order_engine` adapter generates the same random numbers that are generated by the base engine but delivers them in a different order. The template parameter k is the size of an internal table used by the adapter. A random number is randomly selected from this table upon request, and then replaced with a new random number generated by the base engine.

```cpp
template <typename Engine, size_t p, size_t r>
class discard_block_engine {}

template <typename Engine, size_t w, typename UintT> 
class independent_bits_engine {}

template<class Engine, size_t k>
class shuffle_order_engine {...}

```

### predifined engines and engine adapters
1. `minstd_rand0`
2. `minstd_rand`
3. `mt19937`
4. `mt19937_64`
5. `ralux24_base`
6. `ranlux48_base`
7. `ranlux24`
8. `ranlux488`
9. `kunth_b`
10. `default_random_engine`

### generating random numbers
1. Before you can generate any random number, you first need to create an `instance` of an `engine`. If you use a software-based engine, you also need to define a `distribution`. 


```cpp
random_device seeder;
const auto seed{seeder.entropy() ? seeder() : time(nullptr)};
// engine
mt19937 engine {static_cast<mt19937::result_type>(seed)}; // 构造函数
// distribution
uniform_int_distribution<int> distribution {1, 99};
// generate
cout << distribution(engine) << endl;

auto generator {std::bind(distribution, engine)};
vector<int> vec(10);
generate(vec.begin(), vec.end(), generator);

void fillVector(vector<int> & vec, const auto& gen) {
    generate(begin(vec), end(vec), gen);
}

```

### random number distributions
1. `uniform_int_distribution(IntType a = 0, IntType b = numeric_limits<IntType>::max())`;
2. `uniform_real_distribution(RealType a = 0.0, RealType b = 1.0);`
3. `bernoulli_distribution(double p = 0.5);`
4. `binomial_distribution(IntType t = 1, double p = 0.5);`
5. `geometric_distribution(double p = 0.5);`
6. `negative_binomial_distribution(IntType k = 1, double p = 0.5)`
7. `poisson_distribution(double mean = 1.0);`
8. `exponential_distribution(RealType lambda = 1.0);`
9. `gamma_distribution(RealType alpha = 1.0, RealType beta = 1.0);`
10. `weibull_distribution(RealType a = 1.0, RealType b = 1.0);`
11. `extreme_value_distribution(RealType a = 0.0, RealType b = 1.0);`
12. `cauchy_distribution(RealType a = 0.0, RealType b = 1.0);`
13. `chi_squared_distribution(RealType n = 1);`
14. `normal_distribution(RealType mean = 0.0, RealType stddev = 1.0);`
15. and so on ...