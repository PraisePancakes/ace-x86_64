<program> ::= { <statement> }
<statement> ::= <block>
              | <if>
              | <while>
              | <for>
              | <decl> ";"
              | <expr> ";"
              | <func>
              | <class>

<block> ::= "{" { <statement> } "}"
<if> ::= "if" <expression> <statement> |  "if" <expression> <statement>  "else" <statement> 

<while> ::= "while" <expression> <statement>

<for> ::= "for" "(" [ <expr> ] ";" [ <expression> ] ";" [ <expr> ] ")" <statement>

<func> ::= <type> <access_specifier> <identifier> "(" [ <param_list> ] ")"  ( <block> | "=>" "return" <expr> )

<class> ::= "type" <identifier> "{" { <class_member> } "}"

<class_member> ::= <decl> ";"
                 | <func>
                 | <constructor>

<constructor> ::= <identifier> "(" [ <param_list> ] ")" <block>

<param_list> ::= <identifier> { "," <identifier> }

<decl> ::= <type> <access_specifier> <identifier> [":" <qual> ] [ "=" <expression> ]
<type> ::= "int" | "float" | "double" | "long" | "long long" | "unsigned" | "char" | "bool"
<qual> ::= "mut" | "volatile" | "mut" "volatile" | "volatile" "mut"

// expression statement
<expr> ::= <expression>   

//handled by pratt parser and prec climbing
<expression> ::= <assignment>
<assignment> ::= <identifier> "=" <assignment> | <equality_expr>
<equality_expr> ::= <term> { ("==" | "!=" | "<" | ">"| "<=" | ">=") <term> }
<term> { ("+" | "-") <term>  } 
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= <primary>
<primary> ::= <identifier> | <number> | <string> | <grouping>
<identifier> ::= letters 
<string> ::= "\"" letters "\""
<grouping> ::= "(" <expression> ")"
<number> ::= numbers