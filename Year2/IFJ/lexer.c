/**
 * @file lexer.c
 * @authors Peter Rucek, Marek Micek, Rebeka Cernianska, Matej Jurik
 * @date 15 Oct 2020
 * @brief Lexer implementation
 */

#include "lexer.h"

#define IS_ESCCHAR(c) \
    ((c) == 'n' || (c) == '"' || (c) == 't' || (c) == '\\' || (c) == 'x')

const char *keywords[] = {"int", "string", "float64", "if", "else",
"for", "func", "package", "return"};

dynamic_string buffer;

/**
 * @brief Determine whether the passed string is a keyword
 * @param str String to check against keyword list
 * @return Keyword enumerated ID if str is a keyword,
 *         K_ERROR otherwise
 */
Keyword get_keywordID(char *str)
{
    int size = sizeof(keywords) / sizeof(keywords[0]);
    for(int i = 0; i < size; i++)
    {
        if(strcmp(str,keywords[i]) == 0)
            return i;
    }
    return K_ERROR;
}



/**
 * This function implements a Finite State Machine crafted to tokenize
 * IFJ20 language's specific code structures
 *
 * @brief Parse input stream from source file into exactly one token
 * @param f File opened for reading (.go source code)
 * @return Parsed token
 */
Token get_next_token(FILE *f)
{
    static int line = 1; //< counter of current line

    assert(f != NULL);
    Token t;
    State state = S_START;

    while(true)
    {
        int c = fgetc(f);
        switch (state)
        {
            case S_START:
                // Arithmetic operators & comments
                if (c == '+')
                {
                    t.type = ADD;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == '-')
                {
                    t.type = SUB;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == '*')
                {
                    t.type = MUL;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == '/')
                {
                    state = S_COM_OR_DIV;
                }

                else if (c == ',')
                {
                    t.type = COMMA;
                    t.data.s = NULL;
                    return t;
                }

                else if (c == '(')
                {
                    t.type = PARENTHESIS_LEFT;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == ')')
                {
                    t.type = PARENTHESIS_RIGHT;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == '{')
                {
                    t.type = BRACKET_LEFT;
                    t.data.s = NULL;
                    return t;
                }
                else if (c == '}')
                {
                    t.type = BRACKET_RIGHT;
                    t.data.s = NULL;
                    return t;
                }

                // Logical operators
                else if (c == '<')
                {
                    state = S_LT;
                }
                else if (c == '>')
                {
                    state = S_GT;
                }
                else if (c == '!')
                {
                    state = S_NE;
                }
                else if (c == '=')
                {
                    state = S_ASGN_OR_EQ;
                }
                else if (c == ':')
                {
                    state = S_DEF;
                }

                // Identifiers, keywords
                else if (isalpha(c) || c == '_')
                {
                    dynstr_init(&buffer);
                    add_char(&buffer, c);
                    state = S_ID_OR_KEY;
                }

                // Numbers
                else if (isdigit(c))
                {
                    dynstr_init(&buffer);
                    add_char(&buffer, c);
                    state = S_NUM;
                }

                // Strings
                else if (c == '"')
                {
                    dynstr_init(&buffer);
                    add_char(&buffer, c);
                    state = S_STRING;
                }

                // Semicolon
                else if (c == ';')
                {
                    t.type = SEMICLN;
                    t.data.s = NULL;
                    return t;
                }

                // End of Line
                else if (c == '\n')
                {
                    line++;
                    t.type = EOL;
                    t.data.s = NULL;
                    return t;
                }

                // Whitespace
                else if (isspace(c))
                {
                    state = S_START;
                }

                // End of File
                else if (c == EOF)
                {
                    t.type = EoF;
                    t.data.s = NULL;
                    return t;
                }

                // Any other character
                else
                {
                    state = S_ERROR;
                }
                break;

            // Two-part logical operators
            case S_LT:
                if (c == '=')
                {
                    t.type = LE;
                    t.data.s = NULL;
                    return t;
                }
                else
                {
                    ungetc(c, f);
                    t.type = LT;
                    t.data.s = NULL;
                    return t;
                }
                break;

            case S_GT:
                if (c == '=')
                {
                    t.type = GE;
                    t.data.s = NULL;
                    return t;
                }
                else
                {
                    ungetc(c, f);
                    t.type = GT;
                    t.data.s = NULL;
                    return t;
                }
                break;

            case S_NE:
                if (c == '=')
                {
                    t.type = NE;
                    t.data.s = NULL;
                    return t;
                }
                else
                {
                    state = S_ERROR;
                }
                break;

            case S_ASGN_OR_EQ:
                if (c == '=')
                {
                    t.type = EQ;
                    t.data.s = NULL;
                    return t;
                }
                // Assignment
                else
                {
                    ungetc(c, f);
                    t.type = VAR_ASSIGN;
                    t.data.s = NULL;
                    return t;
                }
                break;

            // Variable definition
            case S_DEF:
                if (c == '=')
                {
                    t.type = DEF_OF_VAR;
                    t.data.s = NULL;

                    return t;
                }
                else
                {
                    state = S_ERROR;
                }
                break;

            // Keywords or identifiers
            case S_ID_OR_KEY:
                if (isalnum(c) || c == '_')
                {
                    add_char(&buffer, c);
                }
                else
                {
                    ungetc(c, f);
                    Keyword kw = get_keywordID(buffer.buff);
                    if (kw == K_ERROR)
                    {
                        // Identifier
                        t.type = ID;
                        t.data.s = malloc(sizeof(char) * (strlen(buffer.buff) + 1));
                        if (t.data.s == NULL)
                        {
                            intern_error();
                        }
                        copy_token_string(t.data.s, &buffer);
                    }
                    else
                    {
                        // Keyword
                        t.type = KEYWORD;
                        t.data.k = kw;
                    }

                    dynstr_free(&buffer);
                    return t;
                }
                break;

            // INTs, FLOATs and FLOAT Exponentials
            case S_NUM:
                if ((isdigit(c)) && buffer.buff[0] == '0') //< useless zero at the beginning is forbidden
                {
                    state = S_ERROR;
                }
                else if (isdigit(c))
                {
                    add_char(&buffer, c);
                }
                else if (c == '.')
                {
                    add_char(&buffer, c);
                    state = S_DOUBLE;
                }
                else if (c == 'e' || c == 'E')
                {
                    add_char(&buffer, c);
                    state = S_EXPO_1;
                }
                else
                {
                    state = S_INT;
                    ungetc(c, f);
                }
                break;

            case S_INT:
                ungetc(c, f);
                t.type = INT;
                t.data.i = atoi(buffer.buff);
                dynstr_free(&buffer);
                return t;
                break;

            case S_DOUBLE:
                if (c == '.') //< Multiple decimal points detection (allow only one)
                {
                    state = S_ERROR;
                }
                else if (isdigit(c))
                {
                    add_char(&buffer, c);
                }
                else if (c == 'e' || c == 'E')
                {
                    add_char(&buffer, c);
                    state = S_EXPO_1;
                }
                else
                {
                    ungetc(c, f);
                    if (buffer.buff[buffer.len-1] == '.') //if there is a decimal point, but no decimal part of number
                    {
                        state = S_ERROR;
                    }
                    else
                    {
                        t.type = FLOAT64;
                        t.data.d = atof(buffer.buff);
                        dynstr_free(&buffer);
                        return t;
                    }

                }
                break;

            // Exponential classification - first phase
            case S_EXPO_1:
                // Expect either sign (pos, neg) or exponential number
                if (isdigit(c))
                {
                    add_char(&buffer, c);
                    state = S_EXPO_3;
                }
                else if (c == '+' || c == '-')
                {
                    add_char(&buffer, c);
                    state = S_EXPO_2;
                }
                else
                {
                    state = S_ERROR;
                }
                break;

            // Exponential classification - second phase
            case S_EXPO_2:
                // Sign was specified - accept exponential numbers only
                if (isdigit(c))
                {
                    add_char(&buffer, c);
                    state = S_EXPO_3;
                }
                else
                {
                    state = S_ERROR;
                }
                break;

            // Exponential classification - last phase
            case S_EXPO_3:
                // Accept more numbers if sign was not specified
                // or accept exponential as FLOAT64
                if (isdigit(c))
                {
                    add_char(&buffer, c);
                }
                else if ((isalpha(c)) || c == '.' || c == '_' || c == ':')
                {
                    state = S_ERROR;
                }
                else
                {
                    ungetc(c, f);
                    t.type = FLOAT64;
                    t.data.d = atof(buffer.buff);
                    dynstr_free(&buffer);
                    return t;
                }
                break;

            // Strings
            case S_STRING:
                if (isprint(c))
                {
                    add_char(&buffer, c);

                    if (c != '"') //< Add string contents
                    {
                        if (c == '\\')
                        {
                            state = S_ESC;
                        }
                    }
                    else //< Enclosed string - return str token
                    {
                        t.type = STRING;
                        t.data.s = malloc(sizeof(char) * (strlen(buffer.buff) + 1));
                        if (t.data.s == NULL)
                        {
                            intern_error();
                        }
                        copy_token_string(t.data.s, &buffer);
                        dynstr_free(&buffer);
                        return t;
                    }
                }
                // Non printable character cannot be added to str literal
                else
                {
                    state = S_ERROR;
                }
                break;

            // String's valid escape characters
            case S_ESC:
                if (IS_ESCCHAR(c))
                {
                    add_char(&buffer, c);
                    if (c == 'x') // Handle hexadecimal num escape
                    {
                        state = S_ESCHEX_1;
                    }
                    else
                    {
                        state = S_STRING;
                    }
                }
                else
                {
                    state = S_ERROR;
                }
                break;

            // String's escaped hex number - hex 1 of 2
            case S_ESCHEX_1:
                if (isxdigit(c))
                {
                    add_char(&buffer, c);
                    state = S_ESCHEX_2;
                }
                else
                {
                    state = S_ERROR;
                }
                break;

            // String's escaped hex number - hex 2 of 2
            case S_ESCHEX_2:
                if (isxdigit(c))
                {
                    add_char(&buffer, c);
                    state = S_STRING;
                }
                else
                {
                    state = S_ERROR;
                }
                break;

            // Distinguish div operator from comments
            case S_COM_OR_DIV:
                if (c == '/')
                {
                    state = S_L_COMMENT;
                }
                else if (c == '*')
                {
                    state = S_B_COMMENT;
                }
                else //< Division operator
                {
                    ungetc(c,f);
                    t.type = DIV;
                    t.data.s = NULL;
                    return t;
                }
                break;

            case S_L_COMMENT:
                if (c == '\n' || c == EOF)
                {
                    ungetc(c,f);
                    state = S_START;
                }
                else
                {
                    state = S_L_COMMENT;
                }
                break;

            case S_B_COMMENT:
                if (c == '*')
                {
                    state = S_B_COMMENT_END;
                }
                else if (c == EOF)
                {
                    state = S_ERROR;
                }
                else
                {
                    state = S_B_COMMENT;
                }
                break;

            case S_B_COMMENT_END:
                if (c == '/')
                {
                    state = S_START;
                }
                else if (c == EOF)
                {
                    state = S_ERROR;
                }
                else
                {
                    state = S_B_COMMENT;
                }
                break;

            // Unwanted lexeme
            case S_ERROR:
                t.type = ERROR; //< (null)
                t.data.s = NULL;
                return t;
                break;

            default:
                break;
        }
    }
}