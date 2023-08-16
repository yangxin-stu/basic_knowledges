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