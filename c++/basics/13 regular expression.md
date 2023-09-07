## basic knowledge
1. `Pattern`: The actual regular expression is a pattern represented by a string.
2. `Match`: This determines whether there is a match between a given regular expression and all of the characters in a given sequence `[first, last)`.
3. `Search`: This determines whether there is some substring within a given sequence `[first, last)` that matches a given regular expression.
4. `Replace`: This identifies substrings in a given sequence and replaces them with a corresponding new substring computed from another pattern, called a substitution pattern

## ECMAScript Syntax
1. A regular expression pattern is a sequence of characters representing what you want to match. `^ $ \ . * + ? ( ) [ ] { } |` should use in the form of `\^` if you want to match them
2. `anchors`: `^` and `$`, match the start and end of a string. `^test$` matches `test`
3. `Wildcards`: `.` matches everything except new line `\n`
4. `alternation`:  `|`, or relationship. `a|b` matches `a` or `b`
5. `grouping`, `()`, parentheses, are used to mark `subexpressions`, also called `capture groups`
   1. `Capture groups` can be used to identify individual subsequences of the original string; each marked subexpression (capture group) is returned in the result.
   2. `Capture groups` can be used during matching for a purpose called `back references`
   3. Capture groups can be used to identify components during `replace operations`
6. `Repetition`: `*` matches preceding part 0+; `+` 1+; `?` 0 or 1; `{...}` means a `bounded repeat`. `a{n}` repeat a `n` times. `a{n, m}`: `n~m`, `a{n,}`: `n+`. greedy, find the longest match. non-greedy: `*?`, `+?`, `??`, `{...}?`
7. `precedence`: `elements`, `quantifiers`, `concatenation`, `alternation`
8. `character of set matches`: `[\[\]\^\$]`, `[a-zA-Z\-]`, `[[:digit:]]`, `[[:alnum:]]`, `[[:lower:]5-7]`, `\d`==`[[:digit:]]`
9. `back reference`: `\n` refers to the `n-th` captured group with `n>0`. `(\d+)-.*-\1`: 123-abc-123, 1234-a-1234,
10. `lookahead`: `positive lookahead` and `negative lookahead`, respectively `?=` and `?!` . `a(?!b)` (字符`a` 后面不能是`b`), `a(?=b)` (字符`a` 后面只能是`b`，但`b`不参与匹配的过程，需要其他模式匹配), `(?=.*[[:lower:]])(?=.*[[:upper:]])(?=.*[[:punct:]]).{8,}` uesd in search.
11. `?:`: 用于`()`中，表示改组的结果不会被保存

## regex library
1. `basic_regex`: An object representing a specific regular expression.
2. `match_results`: A substring that matched a regular expression, including all the captured groups. It is a collection of `sub_matches`.
3. `sub_match`: An object containing `a pair of iterators` into the input sequence. These iterators represent the matched `capture group`. The pair is an iterator pointing to the first character of a matched capture group and an iterator pointing to one-past-the-last character of the matched capture group. It has an `str()` method that returns the matched capture group as a string.
4. `regex_match()`, `regex_search()`, `regex_replace()`
5. Never use `regex_search()` in a loop to find all occurrences of a pattern in a source string. Instead, use a `regex_iterator` or `regex_token_iterator`

```cpp
// six version of regex_match
// template<...>
// bool regex_match(InputSequence[, MatchResults], RegEx[, Flags]);
// input: iterator pair, std::string, c-style string
// match_result::empty() or size() if returned false

regex r {"\\d{4}/(?:0?[1-9]|1[0-2])/(?:0?[1-9]|[1-2][0-9]|3[0-1])"};
while (true) {
   cout << "Enter a date (year/month/day) (q=quit): ";
   string str;
   if (!getline(cin, str) || str == "q") { break; }
   if (regex_match(str, r)) { cout << " Valid date." << endl; }
   else { cout << " Invalid date!" << endl; }
}

// match_result[0]: entire pattern; [1]: first group, [2] second group and so on...
while (true) {
   cout << "Enter a date (year/month/day) (q=quit): ";
   string str;
   if (!getline(cin, str) || str == "q") { break; }
   if (smatch m; regex_match(str, m, r)) {
      int year { stoi(m[1]) };
      int month { stoi(m[2]) };
      int day { stoi(m[3]) };
      cout << format(" Valid date: Year={}, month={}, day={}",year, month, day) << endl;
   } else {
      cout << " Invalid date!" << endl;
   }
}

// regex_search: search substring
regex r { "//\\s*(.+)$" };
while (true) {
   cout << "Enter a string with optional code comments (q=quit): ";
   string str;
   if (!getline(cin, str) || str == "q") { break; }
   if (smatch m; regex_search(str, m, r)) {
      cout << format(" Found comment '{}'", m[1].str()) << endl;
   } else {
      cout << " No comment found!" << endl;
   }
}

// regex_iterator

```