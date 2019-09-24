# eparser. The expression parser.

Small expression parser.

## a calculator example

see tests/calculator.cpp

### Available operators

```
+, -, *, /, %, =, :=
```

#### = and :=

The operators bind values to names. Operator `=` creates a constant while `:=` makes a variable that is evaluated any time it's in the expression.

```
Enter: x = 0
    (x = 0) = 0
Enter: y = 0
    (y = 0) = 0
Enter: z:= x*x + 2*x*y + y
    (z := (((x * x) + ((2 * x) * y)) + y)) = 0
Enter: x = 45
    (x = 45) = 45
Enter: z
    z = 2025
Enter: y = 10
    (y = 10) = 10
Enter: z
    z = 2935
```

Here `x` and `y` are constans and `z` is a variable.

```
Enter: 2 + 2 * 2
        (2 + (2 * 2)) = 6
Enter: (2 + 2) * 2
        ((2 + 2) * 2) = 8
```
