# <h1><center> Cloud </center></h1>

This is a toy pogramming language.

### Example
`helloworld`
```python
declare putchar(c: int) -> int

def main() -> int:
    putchar("H")
    putchar("e")
    putchar("l")
    putchar("l")
    putchar("o")
    putchar(",")
    putchar(" ")
    putchar("W")
    putchar("o")
    putchar("r")
    putchar("l")
    putchar("d")
    putchar("!")
    putchar("\n")
    return '\0'

```

### Develop
`linux` & `windows`
#### You have to install make and LLVM

```console
git clone https://github.com/littlewhitecloud/cloud
mkdir obj
make -j2
./cloud example/helloworld.cd -v
```

### Goals
- As simple as Python
- As fast as C++
- Loose grammar checking
- More warnings instead of errors


#### Thanks to @Akuli, the "jou" programming language really inspired me a lot.