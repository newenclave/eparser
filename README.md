# eparser. The expression parser.

Small expression parser.

## a calculator example 

see tests/calculator.cpp

```
Enter: 2 + 2 * 2
        (2 + (2 * 2)) = 6
Enter: (2 + 2) * 2
        ((2 + 2) * 2) = 8
Enter: 11.11 / 45.9 * 5467 + 321 - 1024 / 234 * 43
        ((((11.11 / 45.9) * 5467) + 321) - ((1024 / 234) * 43)) = 1456.11
Enter: x = 10
        (x = 10) = 10
Enter: y = 20
        (y = 20) = 20
Enter: z = 30
        (z = 30) = 30
Enter: x + y * z
        (x + (y * z)) = 610
Enter: -x
        (- x) = -10
Enter: -(-x)
        (- (- x)) = 10
Enter: x - y
        (x - y) = -10
Enter: none = 0
        (none = 0) = 0
Enter: x / none
        (x / none) = inf
Enter: none / 0
        (none / 0) = inf
Enter: (k=11.11) + 22.22
        ((k = 11.11) + 22.22 ) = 33.33
Enter: k
        k = 11.11
Enter: q

```