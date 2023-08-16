## goals
➤ How to work with `compile-time rational numbers`
➤ How to work with `time`
➤ How to work with `dates` and `calendars`
➤ How to convert time points between different time zones

### chrono library
1. durations
2. clocks
3. time points
4. dates
5. time zones

### compile-time rational numbers
1. `<ratio>`, numerator, denominator `std::intmax_t`
2. compute their `gcd`. `num=sign(n)*sign(d)*abs(n)/gcd`, `den=abs(d)/gcd`
3. library supports: adding, subtracting, multiplying, deviding by suing specific templates
4. `std::bool_constant` -> `std::integral_constant` -> `type + compile-time value`


```cpp
using r1 = ratio<1, 60>;
using r2 = ratio<1, 30>;

using result = ratio_add<r1, r2>::type;
// other comparison: ratio_equal, ratio_not_equal<r1 ,r2>

using result2 = ratio_less<r1, r2>;

intmax_t num{r1::num}, den{r1::den}; // 分子， 分母

```

### duration
1. A duration is an `interval` between two points in time. It is represented by the `duration` class template, which stores a number of `ticks` and a `tick period`. The `tick period` is the time in seconds between two ticks and is represented as a `compile-time ratio constant`
2. `template <class Rep, class Period = ratio<1>> class duration {...}` . `Rep`: `long`, `double` ...
3. Durations support `arithmetic operations` such as +, -, *, /, %, ++, --, +=, -=, *=, /=, and %=, and they support the `comparison operators` == and <=>. method: `min, max, count, zero`

```cpp
duration<long> d1{3}; // 3 seconds;
duration<long, ratio<1>> d2{123}; // 123 seconds
duration<double, ratio<1, 60>> d3{60}; // 60 * (1/60) seconds 
duration<double, std::milli> d4;
duration<long, ratio<60>> d5 {10}; // 10 mins

std::cout << d1.count() << endl; 
std::cout << d2.max() << endl; 
if (d2 > d3) {
    std::cout << "d2 > d3" << std::endl;
}

// arithmatic operations
++ d5;
d5 *=2;

// duration_cast
duration<long> d6{30}; // 30 seconds
auto d7{duration_cast<duration<long, ratio<60>>>(d6)}; // 0 minutes!

// X is compiler-dependent
using nanoseconds = duration<X 64 bits, nano>;
using minutes = duration<long, ratio<60>>;
using hours = duration<X 23 bits, ratio<3600>>;
using days = duration<X 25 bits, ratio_multiply<ratio<24>, hours::period>>;

seconds s{60};
minutes m{s}; // compile error! seconds -> minutes could loss presicion
```

### clock
1. A `clock` is a class consisting of a `time_point` and a `duration`.
2. `system_clock` `steady_clock` `high_resolution_clock`. c++20: `utc_clock`, `tai_clock` , `gps_clock`, `file_clock`
3. `system_clock`: the wall clock time from the system-wide real-time clock
4. `steady_clock`: guarantees its `time_point` will never decrease
5. `high_resolution_clock`: has the shortest possible tick period.
6. Every clock has a static `now()` method to get the current time as a time_point. 
7.  converting `time_points` to and from the `time_t` C-style:  `to_time_t()`, ` from_time_t()`
8.  The `localtime()` function converts a `time_t` to a local time represented by `tm` and is defined in the `<ctime>` header file.


```cpp
using namespace std::chrono;

system_clock::time_point tpoint{std::system_clock::now()};
time_t tt{system_clock::to_time_t(tpoint)};
tm *t {localtime(&tt)};
cout << put_time(t, "%H:%M:%S") << std::endl;


stringstream ss;
ss << put_time(t, "%H:%M:%S");
string stringTime {ss.str()};


auto start {high_resolution_clock::now()};
double d {0};
for (int i=0;i<1'000'000; ++i) {
    d += square(sin(i)*cos(i));
}
auto end {high_resolution_clock::now()};
auto diff {end - start};
cout << duration<double, milli> diff.count() << "ms" << endl;
```

### time_point
1. A `point` in time is represented by the `time_point` class and stored as a `duration` relative to an `epoch`, representing the beginning of `time`. A `time_point` is always associated with a certain `clock`, and the `epoch` is the origin of this associated clock.
2. constructor: `time_point()`; `time_point(const duratioin& d)`; `template <class Duration2> time_point(const time_point<clock, Duration2>& t`
3. `time_point.time_since_epoch()`

```cpp
using namespace std::chrono;
time_point<steady_clock> tp1;
// or 
steady_clock::time_point tp2; // from the beginning of `time`
tp1 += 10min; // using namespace std::chrono::literals; or ...
auto d1 {tp1.time_since_epoch()}; // get the duration
duration<double, ratio<1, 60>> d2{d1};
cout << d2.count() << std::endl;

time_point<steady_clock, seconds> tpSeconds{42s};

time_point<steady_clock, milliseconds> tpMilliseconds { 42'424ms };
// Convert milliseconds to seconds explicitly.
time_point<steady_clock, seconds> tpSeconds {
time_point_cast<seconds>(tpMilliseconds) };
```

### date
1. `year`: [-32767, 32767], `is_leap`, `min`, `max`
2. `month`: [1, 12] `std::chrono::January`
3. `day`: [1, 31]
4. `weekday`: [0, 6]
5. `weekday_indexed`: [1,2,3,4,5] weekday of a month
6. `weekday_last`
7. `month_day`
8. `sys_time` is a new type alias for a `time_point` of a system_clock with a certain duration.

```cpp
// ok() to check a given object is in a valid range
year y1 {2020};
year y2 {2020y};
month m1 {6};
year_month_day fuudate1 {2020y, June, 22d};
auto fulldate2 { 2020y / June / 22d };
auto fulldate3 { 22d / June / 2020y };

auto lastMondayOfJune2020 { 2020y / June / Monday[last] };

template <typename Duration>
using sys_time = std::chrono::time_point<sys_clock, Duration>;
using sys_seconds = sys_time<std::chrono::seconds>;

auto today {floor<days>(system_clock::now())};
```

### time zone