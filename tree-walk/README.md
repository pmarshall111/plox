# Tree Walk Lox Implementation

## High Level overview

Code from the user goes through the following steps:

1. It goes through a `scanner`, which splits the code into tokens. The scanner allows later code to ignore how long each token within the code is. I.e. `var abcdefg = "hello" + " " + "world"` is 8 tokens: [`var`, `abcdefg`, `=`, `hello`, `+`, ` `, `+`, `world`]. It can also highlight errors if there's an incomplete token. I.e. `"world` would be an unterminated string.
1. Once scanned into tokens, those tokens are parsed by the `parser`. This parser structures the tokens into the order they should be evaluated in. I.e. a multiply should be evaluated before a plus, a parenthesis before a subtract. It can also highlight errors if the tokens do not match the grammar of the language. I.e. `2 ** 3` is not a supported operation in lox and therefore not valid syntax.
1. After parsing, the code is interpreted by the `interpreter`. This component evaluates expressions created by the parser. I.e. `1+2` is finally evaluated to be `3`. It can also highlight errors that are not picked up by the parser. I.e. `-"hello"` is a valid unary from the parser's pov, but is not a valid expression to be interpreted.