#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include "memallocator.h"
#include "generators.h"


typedef enum {
    NONE, // not recognized token
    BOS, // begin of sentence
    EOS, // end of sentence
    WORD, 
    PUNCT, // punctuation
    SEP // separator
} TokenType;


// Dynamic String

DYNARR_GEN(char, DString, '\0', TokenType tokenType);
DYNARR_FREE_FUNC_GEN(DString, );

void extendDString(DString* dstr, char* str) {
    while (*str) {
        appendDString(dstr, *(str++));
    }
}

void initDString(DString* dstr, char* str) {
    initEmptyDString(dstr);
    extendDString(dstr, str);
}

//


// Dynamic (dynamic) String array

static DString termDString = {NULL, 0, 0};

DYNARR_GEN(DString, DStringArray, termDString, );
DYNARR_FREE_FUNC_GEN(DStringArray, freeDString);

void extractSubArrayDStringArray(DStringArray* dynarr, DString* pointerStart, DString* pointerEnd) {

    DString* arr = dynarr->array;
    size_t len = dynarr->len;

    assert(pointerStart <= pointerEnd);
    assert(pointerEnd < (arr + len));

    for (DString* p = pointerStart; p <= pointerEnd; p++) {
        freeDString(p);
    }

    size_t shift = pointerEnd - pointerStart + 1;

    for (DString* p = pointerEnd + 1; p < (arr + len); p++) {
        *(p - shift) = *(p);
    }

    dynarr->len = len - shift;

}

void changeValueDStringArray(DStringArray* dynarr, size_t ind, DString newValue) {
    assert(ind < (dynarr->len));

    DString* arr = (dynarr->array);
    freeDString(arr + ind);
    arr[ind] = newValue;
}

//


typedef struct _IndLinkedListNode {
    struct _IndLinkedListNode* parrent;
    size_t value;
} IndLinkedListNode;

IndLinkedListNode* newIndLinkedListNode(IndLinkedListNode* pointerToParrent, size_t value) {
    IndLinkedListNode* newNode = (IndLinkedListNode*)memalloc(sizeof(IndLinkedListNode));

    newNode->parrent = pointerToParrent;
    newNode->value = value;

    return newNode;
}

IndLinkedListNode* extractIndLinkedList(IndLinkedListNode* head) {
    if (head == NULL) {
        return NULL;
    }

    IndLinkedListNode* newHead = head->parrent;

    memfree(head);

    return newHead;
}

void freeIndLinkedList(IndLinkedListNode* headNode) {
    while (headNode->parrent != NULL) {
        IndLinkedListNode* prevNode = headNode;
        headNode = prevNode->parrent;
        memfree(prevNode);
    }
}


#define IS_NUM(c) (((c) >= '0') && ((c) <= '9'))

#define IS_ALPHA(c) (                                               \
    (((c) >= 'a') && ((c) <= 'z')) ||                               \
    (((c) >= 'A') && ((c) <= 'Z'))                                  \
)

#define IS_ALPHA_NUM(c) (                                           \
    (((c) >= 'a') && ((c) <= 'z')) ||                               \
    (((c) >= 'A') && ((c) <= 'Z')) ||                               \
    (((c) >= '0') && ((c) <= '9'))                                  \
)

#define IS_SEP(c) (                                                 \
    (((c) == ' ') || ((c) == '\n'))                                 \
)

#define IS_OP(c) (                                                  \
    (((c) == '+') || ((c) == '-') || ((c) == '*') || ((c) == '/'))  \
)

#define MATCH(stream, buff, type, pred, dstr) ({                    \
    dstr.tokenType = type;                                          \
    DString* pDstr = &dstr;                                         \
    while( ((buff = fgetc(stream)) != EOF) && (pred(buff)) ) {      \
        appendDString(pDstr, buff);                                 \
    }                                                               \
})

void parseFile(FILE* stream, DStringArray* dynarrDest) {
    initEmptyDStringArray(dynarrDest);

    DString currentToken;
    initEmptyDString(&currentToken);

     

    currentToken.tokenType = BOS;
    appendDStringArray(dynarrDest, currentToken);

    

    char buff = fgetc(stream);

    while (IS_SEP(buff)) {
        buff = fgetc(stream);  
    }

    while(buff != EOF) {
        //printDebugAlloc();
        
        initEmptyDString(&currentToken);
        appendDString(&currentToken, buff);

        switch(buff) 
        {
            case ' ':
            case '\n':
                MATCH(stream, buff, SEP, IS_SEP, currentToken);
                break;

            case '+':
            case '-':
                buff = fgetc(stream);
                if (!(IS_ALPHA_NUM(buff))) {
                    currentToken.tokenType = PUNCT;
                    break;
                } // fallthrough
                appendDString(&currentToken, buff);

            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
                MATCH(stream, buff, WORD, IS_ALPHA_NUM, currentToken);
                break;
            
            default:
                currentToken.tokenType = PUNCT;
                buff = fgetc(stream);
                break;

        }

        appendDStringArray(dynarrDest, currentToken);
    }

    
    initEmptyDString(&currentToken);
    currentToken.tokenType = EOS;
    appendDStringArray(dynarrDest, currentToken);
}


int isFarenhDString(DString dstr) {
    if (dstr.len < 4) {
        return 0;
    }

    if ( ((dstr.array[0] == '+') || (dstr.array[0] == '-')) &&
    ( ((dstr.array[dstr.len - 2] == 't') && (dstr.array[dstr.len - 1] == 'F')) ) ) {
        for (size_t i = 1; i < dstr.len - 2; i++) {
            if ( !(IS_NUM(dstr.array[i])) ) {
                return 0;
            }
        }

        return 1;
    }

    return 0;
}

void convertFarenhDStringToCelsius(DString dstrFarenh, DString* dstrOut) {
    char valueBuff[dstrFarenh.len - 1];

    for (size_t i = 0; i < dstrFarenh.len - 2; i++) {
        valueBuff[i] = dstrFarenh.array[i];
    }
    valueBuff[dstrFarenh.len - 2] = '\0'; 

    int farenh = atoi(valueBuff);

    float cels = ((float)(farenh - 32)) * (5.0f / 9.0f);
    DString convStr;
    initEmptyDString(&convStr);

    if (farenh >= 32) {
        appendDString(&convStr, '+');
    }

    // 9999999999 tF == 5555555537,2 tC
    // -9999999999 tF == -5555555572,8 tC
    //                   [13 characters + 1 + \0 ]

    char celsBuff[15];
    sprintf(celsBuff, "%.2f", cels);
    extendDString(&convStr, celsBuff); // apparently accuracy is bad
    popDString(&convStr, convStr.len - 1);

    convStr.array[convStr.len - 2] = ',';
    extendDString(&convStr, "tC");

    *dstrOut = convStr;
}

int isPolindromDString(DString dstr) {
    if (dstr.len == 1) {
        return IS_ALPHA(dstr.array[0]);
    }

    for (size_t i = 0; i < (dstr.len / 2); i++) {
        char c = dstr.array[i];
        if ( (c != dstr.array[dstr.len - i - 1]) || !( IS_ALPHA(c) ) ) {
            return 0;
        }
    }
    return 1;
}

DString* matchNextToken(DStringArray text, DString* startPointer, TokenType type) {
    assert(startPointer >= text.array);
    assert(startPointer <= text.array + (text.len - 1));

    for (DString* p = startPointer + 1; p < text.array + text.len; p++) {
        TokenType currentType = p->tokenType;

        if (currentType == SEP)
            continue;

        if (currentType == type) 
            return p;

        break;
    }

    return NULL;
}

DString* matchClosedBracket(DStringArray text, DString* startPointer) {
    DString* nextPunctPointer = matchNextToken(text, startPointer, PUNCT);

    if (nextPunctPointer == NULL) 
        return NULL;

    DString token = *nextPunctPointer;

    return (token.len == 1 && token.array[0] == ')') ? nextPunctPointer : NULL;
}

// Convert DString value to string and store in dest variable, returns true if convrsion is possible, false otherwise
_Bool convDStringToInt(DString dstrSource, int* dest) {
    if (dstrSource.len == 1 && dstrSource.array[0] == '0') {
        *dest = 0;
        return true;
    }

    int x = atoi(dstrSource.array);
    *dest = x;

    return x;
}

void evalExprToDString(int x, int y, char op, DString* dstrOut) {
    switch (op)
    {
        case '+':
            x += y;
            break;
        case '-':
            x -= y;
            break;
        case '*':
            x *= y;
            break;
        case '/':
            if (y == 0) {
                initDString(dstrOut, "ERROR");
                return;
            }

            x /= y;
            break;
    }
    char valueBuff[12];

    sprintf(valueBuff, "%d", x);
    initDString(dstrOut, valueBuff);
    dstrOut->tokenType = WORD;
}

void modifyText(DStringArray* textSource) {


    DStringArray text = *textSource;

    IndLinkedListNode* lastNotSepTokenNode = newIndLinkedListNode(NULL, 0);
    DString currentToken;


    for (size_t currentTokenInd = 1; currentTokenInd < text.len - 1; ) {
        currentToken = text.array[currentTokenInd];
        
        if (currentToken.tokenType == PUNCT) {
            if (currentToken.len == 1) {
                if (currentToken.array[0] == '(') {

                    DString* nextWordPointer = matchNextToken(text, text.array + currentTokenInd, WORD);
                    DString* closedBracketPointer = nextWordPointer == NULL ? NULL : matchClosedBracket(text, nextWordPointer); 
                    
                    if (nextWordPointer != NULL && closedBracketPointer != NULL) {

                        extractSubArrayDStringArray(&text, nextWordPointer + 1, closedBracketPointer);
                        extractSubArrayDStringArray(&text, text.array + currentTokenInd, nextWordPointer - 1);

                        currentTokenInd = lastNotSepTokenNode->value;

                        if (lastNotSepTokenNode->parrent == NULL) {
                            currentTokenInd++;
                        } else {
                            IndLinkedListNode* prevNotSepTokenNode = extractIndLinkedList(lastNotSepTokenNode);
                            lastNotSepTokenNode = prevNotSepTokenNode;
                        }

                        continue;
                    }
                } else if (IS_OP(currentToken.array[0])) {
                    int x, y;

                    DString* nextWordPointer = matchNextToken(text, text.array + currentTokenInd, WORD); 

                    if (nextWordPointer != NULL) {
                        if (( text.array[lastNotSepTokenNode->value].tokenType == WORD && convDStringToInt(text.array[lastNotSepTokenNode->value], &x) ) && \
                            convDStringToInt(*nextWordPointer, &y)) {

                                DString evalRes;
                                evalExprToDString(x, y, currentToken.array[0], &evalRes);
                                extractSubArrayDStringArray(&text, text.array + (lastNotSepTokenNode->value + 1), nextWordPointer);
                                
                                changeValueDStringArray(&text, lastNotSepTokenNode->value, evalRes);
                                
                                lastNotSepTokenNode = extractIndLinkedList(lastNotSepTokenNode);
                                currentTokenInd = lastNotSepTokenNode->value;

                                if (lastNotSepTokenNode->parrent == NULL) {
                                    currentTokenInd++;
                                } else {
                                    lastNotSepTokenNode = extractIndLinkedList(lastNotSepTokenNode);
                                }
                                
                                continue;
                        }
                    }
                }
            }
            lastNotSepTokenNode = newIndLinkedListNode(lastNotSepTokenNode, currentTokenInd);

        } else if (currentToken.tokenType == WORD) {
            if (isFarenhDString(currentToken)) {
                initEmptyDString(text.array + currentTokenInd);
                convertFarenhDStringToCelsius(currentToken, text.array + currentTokenInd);
                (text.array + currentTokenInd)->tokenType = WORD;
                freeDString(&currentToken);
            } else if (isPolindromDString(currentToken)) {
                initDString(text.array + currentTokenInd, "PALINDROM");
                (text.array + currentTokenInd)->tokenType = WORD;
                freeDString(&currentToken);
            }

            lastNotSepTokenNode = newIndLinkedListNode(lastNotSepTokenNode, currentTokenInd);
        } else if (currentToken.tokenType == SEP) {
            while (text.array[currentTokenInd + 1].tokenType == SEP) {
                DString nextSep = popDStringArray(&text, currentTokenInd + 1);

                extendDString(&currentToken, nextSep.array);
                freeDString(&nextSep);
            }

            _Bool isSpaceCurr;
            _Bool isSpaceLast = currentToken.array[0] == ' ';
            _Bool isSpacePrev = true;

            for (size_t i = 1; i < currentToken.len; ) {
                isSpaceCurr = currentToken.array[i] == ' '; 
                if (isSpaceCurr || !(isSpaceLast || isSpacePrev)) {
                    popDString(&currentToken, i);
                } else {
                    isSpacePrev = isSpaceLast;
                    isSpaceLast = isSpaceCurr;
                    i++;   
                }
            }

            text.array[currentTokenInd] = currentToken;
        }

        currentTokenInd++;
    }

    freeIndLinkedList(lastNotSepTokenNode);

    textSource->len = text.len;

}

void writeFile(FILE* stream, DStringArray text) {
    for (size_t i = 1; i < text.len - 1; i++) {
        for (size_t j = 0; j < text.array[i].len; j++) {
            fputc(text.array[i].array[j], stream);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    resetAllocator();

    FILE* inputFile = fopen(argv[1], "r");
    DStringArray text;

    initEmptyDStringArray(&text);
    parseFile(inputFile, &text);
    fclose(inputFile);

    modifyText(&text);

    FILE* outputFile = fopen(argv[2], "w");

    writeFile(outputFile, text);

    fclose(outputFile);

    freeDStringArray(&text);
    
    freeHeap();
    return 0;
}
