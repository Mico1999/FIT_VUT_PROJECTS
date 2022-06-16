/**
 * @file test_lexer.c
 * @authors Matej Jurik
 * @date 29 Oct 2020
 * @brief Lexer test suite
 */

#include "test_lexer.h"

// Standard macros & globals
#define setError() if(fail!=true) fail=true

#define testerror(error) perror("**TESTERROR " __FILE__ "** "error); exit(errno)
#define testfail(fmt, ...) \
    setError(); \
    fprintf(stderr, "**TESTFAIL " __FILE__"> %s** "fmt"\n", __func__, ##__VA_ARGS__)

bool fail = false;

// Customized failure messages
#define TESTFAIL_TYPE "Token no.%d's TYPE does not match the expected type (src[%d:%d])"
#define TESTFAIL_DATA "Token no.%d's DATA does not match the expected data (src[%d:%d])"

FILE* goSource1;
FILE* goSource2;
FILE* goSource3;


void setUp()
{
    goSource1 = fopen("source1.go", "r");
    goSource2 = fopen("source3.go", "r");
    goSource3 = fopen("source2.go", "r");

    if (!goSource1)
    {
        testerror("test_source1.go cannot be opened");
    }
    if (!goSource2)
    {
        testerror("test_source2.go cannot be opened");
    }
    if (!goSource3)
    {
        testerror("test_source3.go cannot be opened");
    }
}

void tearDown()
{
    if (fclose(goSource1) == EOF)
    {
        testerror("test_source1.go cannot be closed");
    }
    if (fclose(goSource2) == EOF)
    {
        testerror("test_source2.go cannot be closed");
    }
    if (fclose(goSource3) == EOF)
    {
        testerror("test_source3.go cannot be closed");
    }
}

static void lexAgainst(Token expected[])
{
    int tokIdx = 0;
    int tokCnt = 1;
    int curLine = 1;
    int tokenOffset = 0;

    Token curTok;
    while ( (curTok = get_next_token(goSource1)).type != EoF)
    {   
        tokenOffset++;

        // Check if token type was parsed as expected
        if (curTok.type == expected[tokIdx].type)
        {
            // Check if token type was parsed as expected
            if (expected[tokIdx].type == INT  || expected[tokIdx].type == ERROR)
            {
                if (curTok.data.i != expected[tokIdx].data.i)
                {
                    testfail(TESTFAIL_DATA, tokCnt, curLine, tokenOffset);
                }
            }
            else if (expected[tokIdx].type == FLOAT64)
            {
                if (curTok.data.d != expected[tokIdx].data.d)
                {
                    testfail(TESTFAIL_DATA, tokCnt, curLine, tokenOffset);
                }
            }
            else if (expected[tokIdx].type == KEYWORD)
            {
                if (curTok.data.k != expected[tokIdx].data.k)
                {
                    testfail(TESTFAIL_DATA, tokCnt, curLine, tokenOffset);
                }                
            }
            else if (expected[tokIdx].type == STRING || expected[tokIdx].type == ID)
            {
                if (strcmp(curTok.data.s, expected[tokIdx].data.s) != 0)
                {
                    testfail(TESTFAIL_DATA, tokCnt, curLine, tokenOffset);
                }
            }
            else
            {
                if (curTok.data.s != NULL) //< All other token types hold NULL as their data
                {
                    testfail(TESTFAIL_DATA, tokCnt, curLine, tokenOffset);
                }

                if (curTok.type == EOL)
                {
                    curLine++;
                    tokenOffset = 0;
                }
            } //< Dataparse end

            tokIdx++;
            tokCnt++;
        }
        else {
            testfail(TESTFAIL_TYPE, tokCnt, curLine, tokenOffset);
        } //< Typeparse end
    }
}

/**
 * Tokenize the contents of example program no 1
 * Pass when Llexer output matches expected output, else throw error
 * 
 * @todo Make this function call testfail() to achieve more precise failure message
 */
void test_goSource1()
{
    Token expected[] = {
        // Row 1 <1> (Comment)
        {.type = EOL, .data.s = NULL},
        // Row 2 <2, 4>
        {.type = KEYWORD, .data.k = K_PACKAGE},
        {.type = ID, .data.s = "main"},
        {.type = EOL, .data.s = NULL},
        // Row 3 <5, 10>
        {.type = KEYWORD, .data.k = K_FUNC},
        {.type = ID, .data.s = "main"},
        {.type = PARENTHESIS_LEFT, .data.s = NULL},
        {.type = PARENTHESIS_RIGHT, .data.s = NULL},
        {.type = BRACKET_LEFT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 4 <11, 15>
        {.type = ID, .data.s = "print"},
        {.type = PARENTHESIS_LEFT, .data.s = NULL},
        {.type = STRING, .data.s = "\"Zadejte cislo pro vypocet faktorialu: \""},
        {.type = PARENTHESIS_RIGHT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 5 <16, 19>
        {.type = ID, .data.s = "a"},
        {.type = DEF_OF_VAR, .data.s = NULL},
        {.type = INT, .data.i = 0},
        {.type = EOL, .data.s = NULL},
        // Row 6 <20, 27>
        {.type = ID, .data.s = "a"},
        {.type = COMMA, .data.s = NULL},
        {.type = ID, .data.s = "_"},
        {.type = VAR_ASSIGN, .data.s = NULL},
        {.type = ID, .data.s = "inputi"},
        {.type = PARENTHESIS_LEFT, .data.s = NULL},
        {.type = PARENTHESIS_RIGHT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 7 <28, 33>
        {.type = KEYWORD, .data.k = K_IF},
        {.type = ID, .data.s = "a"},
        {.type = LT, .data.s = NULL},
        {.type = INT, .data.i = 0},
        {.type = BRACKET_LEFT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 8 <34, 38>
        {.type = ID, .data.s = "print"},
        {.type = PARENTHESIS_LEFT, .data.s = NULL},
        {.type = STRING, .data.s = "\"Faktorial nejde spocitat!\\n\""},
        {.type = PARENTHESIS_RIGHT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 9 <39, 42>
        {.type = BRACKET_RIGHT, .data.s = NULL},
        {.type = KEYWORD, .data.k = K_ELSE},
        {.type = BRACKET_LEFT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 10 <43, 46>
        {.type = ID, .data.s = "vysl"},
        {.type = DEF_OF_VAR, .data.s = NULL},
        {.type = INT, .data.i = 1},
        {.type = EOL, .data.s = NULL},
        // Row 11 <47, 59>
        {.type = KEYWORD, .data.k = K_FOR},
        {.type = SEMICLN, .data.s = NULL},
        {.type = ID, .data.s = "a"},
        {.type = GT, .data.s = NULL},
        {.type = INT, .data.i = 0},
        {.type = SEMICLN, .data.s = NULL},
        {.type = ID, .data.s = "a"},
        {.type = VAR_ASSIGN, .data.s = NULL},
        {.type = ID, .data.s = "a"},
        {.type = SUB, .data.s = NULL},
        {.type = INT, .data.i = 1},
        {.type = BRACKET_LEFT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 12 <60, 65>
        {.type = ID, .data.s = "vysl"},
        {.type = VAR_ASSIGN, .data.s = NULL},
        {.type = ID, .data.s = "vysl"},
        {.type = MUL, .data.s = NULL},
        {.type = ID, .data.s = "a"},
        {.type = EOL, .data.s = NULL},
        // Row 13 <66, 67>
        {.type = BRACKET_RIGHT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 14 <68, 76>
        {.type = ID, .data.s = "print"},
        {.type = PARENTHESIS_LEFT, .data.s = NULL},
        {.type = STRING, .data.s = "\"Vysledek je \""},
        {.type = COMMA, .data.s = NULL},
        {.type = ID, .data.s = "vysl"},
        {.type = COMMA, .data.s = NULL},
        {.type = STRING, .data.s = "\"\\n\""},
        {.type = PARENTHESIS_RIGHT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 15 <77, 78>
        {.type = BRACKET_RIGHT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
        // Row 16 <79, 80>
        {.type = BRACKET_RIGHT, .data.s = NULL},
        {.type = EOL, .data.s = NULL},
    };
    lexAgainst(expected);
}
void test_goSource2()
{
    return;
}
void test_gosource3()
{
    return;
}
void test_goErrors()
{
    return;
}

int main(void)
{
    setUp();

    test_goSource1();
    test_goSource2();
    test_gosource3();
    test_goErrors();

    tearDown();

    if (fail) return TEST_FAIL;
    return TEST_SUCCESS;
}