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

<func> ::= "func" <identifier> "(" [ <param_list> ] ")" <block>

<param_list> ::= <identifier> { "," <identifier> }

<decl> ::= "var" <identifier> [ "=" <expression> ]

<expr> ::= <expression>   // alias for clarity

<expression> ::= <term> { ("+" | "-" | "==" | "!=" | "<" | ">" | "<=" | ">=") <term> }

<term> ::= <factor> { ("*" | "/") <factor> }

<factor> ::= <identifier>
           | <number>
           | "(" <expression> ")"

<identifier> ::= /* identifier syntax, e.g. letter (letter|digit)* */

<number> ::= /* number literal */