<program> ::= { <statement> }
<statement> ::= <block>
              | <if>
              | <while>
              | <for>
              | <decl> ";"
              | <expr> ";"
              | <func>
<block> ::= "{" { <statement> } "}"
<if> ::= "if" <expression> <statement> |  "if" <expression> <statement>  "else" <statement> 

<while> ::= "while" <expression> <statement>

<for> ::= "for" "(" [ <expr> ] ";" [ <expression> ] ";" [ <expr> ] ")" <statement>

<func> ::= "func" <identifier> "(" [ <param_list> ] ")" ( <block> | <arrow> "return" <expr> )

<param_list> ::= <identifier> { "," <identifier> }

<decl> ::= <type> <identifier> [":" <qual> ] [ "=" <expression> ]
<type> ::= "int" | "float" | "double" | "long" | "long long" | "unsigned" | "char" | "bool"
<qual> ::= "mut" | "volatile" | "mut" "volatile" | "volatile" "mut"

<expr> ::= <expression>   // alias for clarity

<expression> ::= <term> { ("+" | "-" | "==" | "!=" | "<" | ">" | "<=" | ">=") <term> }

<term> ::= <factor> { ("*" | "/") <factor> }

<factor> ::= <identifier>
           | <number>
           | "(" <expression> ")"

<identifier> ::= /* identifier syntax, e.g. letter (letter|digit)* */

<number> ::= /* number literal */