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
Enter: a = 0
    (a = 0) = 0
Enter: b = 0
    (b = 0) = 0
Enter: c := a + b
    (c := (a + b)) = 0
Enter: a = 2
    (a = 2) = 2
Enter: b = 2
    (b = 2) = 2
Enter: c
    c = 4
Enter: b = 10
    (b = 10) = 10
Enter: c
    c = 12
```

Here `a` and `b` are constans and `c` is a variable.

```
Enter: 2 + 2 * 2
        (2 + (2 * 2)) = 6
Enter: (2 + 2) * 2
        ((2 + 2) * 2) = 8
```

## a plot example
Same as the calculator, but you enter an expresstion with 'X' and the example draws all 'Y' values as a line.
![plot](https://github.com/newenclave/eparser/blob/master/tests/plot.png)
