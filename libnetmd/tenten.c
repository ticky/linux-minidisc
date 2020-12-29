#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static const uint8_t FULL_HEAD = 0xe3;

struct tail {
    uint8_t a;
    uint8_t b;
};

struct kana {
    uint8_t a;
    uint8_t dakuten_b;
    uint8_t handakuten_b;
};

static const struct tail TENTEN_DECOMPOSED_TAIL[] = {
    { 0x82, 0xa6 }, // "ウ"
    { 0x82, 0xab }, // "カ"
    { 0x82, 0xad }, // "キ"
    { 0x82, 0xaf }, // "ク"
    { 0x82, 0xb1 }, // "ケ"
    { 0x82, 0xb3 }, // "コ"
    { 0x82, 0xb5 }, // "サ"
    { 0x82, 0xb7 }, // "シ"
    { 0x82, 0xb9 }, // "ス"
    { 0x82, 0xbb }, // "セ"
    { 0x82, 0xbd }, // "ソ"
    { 0x82, 0xbf }, // "タ"
    { 0x83, 0x81 }, // "チ"
    { 0x83, 0x84 }, // "ツ"
    { 0x83, 0x86 }, // "テ"
    { 0x83, 0x88 }, // "ト"
    { 0x83, 0x8f }, // "ハ"
    { 0x83, 0x92 }, // "ヒ"
    { 0x83, 0x95 }, // "フ"
    { 0x83, 0x98 }, // "ヘ"
    { 0x83, 0x9b }, // "ホ"
    { 0x83, 0xaf }, // "ワ"
    { 0x83, 0xb0 }, // "ヰ"
    { 0x83, 0xb1 }, // "ヱ"
    { 0x83, 0xb2 }, // "ヲ"
    { 0, 0 }, // end
};

// NOTE: "カ゚", "キ゚", "ク゚", "ケ゚", "コ゚", "ツ゚", "セ゚" and "ト゚"
// are not included in the following list as they are all
// comprised of a katakana character plus the combining
// handakuten diacritic, which is handled separately

static const struct kana TENTEN_COMPOSED_KANA[] = {
    { 0x83, 0xb4, 0 }, // "ヴ" only
    { 0x82, 0xac, 0 }, // "ガ" only
    { 0x82, 0xae, 0 }, // "ギ" only
    { 0x82, 0xb0, 0 }, // "グ" only
    { 0x82, 0xb2, 0 }, // "ゲ" only
    { 0x82, 0xb4, 0 }, // "ゴ" only
    { 0x82, 0xb6, 0 }, // "ザ" only
    { 0x82, 0xb8, 0 }, // "ジ" only
    { 0x82, 0xba, 0 }, // "ズ" only
    { 0x82, 0xbc, 0 }, // "ゼ" only
    { 0x82, 0xbe, 0 }, // "ゾ" only
    { 0x83, 0x80, 0 }, // "ダ" only
    { 0x83, 0x82, 0 }, // "ヂ" only
    { 0x83, 0x85, 0 }, // "ヅ" only
    { 0x83, 0x87, 0 }, // "デ" only
    { 0x83, 0x89, 0 }, // "ド" only
    { 0x83, 0x90, 0x91 }, // "バ" and "パ"
    { 0x83, 0x93, 0x94 }, // "ビ" and "ピ"
    { 0x83, 0x96, 0x97 }, // "ブ" and "プ"
    { 0x83, 0x99, 0x9a }, // "ベ" and "ペ"
    { 0x83, 0x9c, 0x9d }, // "ボ" and "ポ"
    { 0x83, 0xb7, 0 }, // "ヷ" only
    { 0x83, 0xb8, 0 }, // "ヸ" only
    { 0x83, 0xb9, 0 }, // "ヹ" only
    { 0x83, 0xba, 0 }, // "ヺ" only
    { 0, 0, 0 }, // end
};

// rewrites dakuten (゛) and handakuten (゜) to separate characters
uint8_t*
kata_tenten_decompose(uint8_t* input)
{
    uint8_t length = strlen((char*)input);
    uint8_t* decomposed_input = malloc(length * 2);
    uint8_t* output = decomposed_input;

    for (; *input; input++, output++) {
        if (*input == FULL_HEAD) {
            // check for early string termination
            if (input[1] == 0 || input[2] == 0) {
                return NULL;
            }

            // check for combining tenten characters, and convert to non-combining
            if (input[1] == (uint8_t)0x82 && (input[2] == (uint8_t)0x99 || input[2] == (uint8_t)0x9a)) {
                input[2] += 2;
                input += 2;
                output += 2;
            }

            const struct kana* from_i = TENTEN_COMPOSED_KANA;
            const struct tail* to_i = TENTEN_DECOMPOSED_TAIL;

            for (; from_i->a && (from_i->dakuten_b || from_i->handakuten_b); from_i++, to_i++) {
                if (from_i->a == input[1] && from_i->dakuten_b == input[2]) {
                    output[0] = FULL_HEAD;
                    output[1] = to_i->a;
                    output[2] = to_i->b;
                    output[3] = FULL_HEAD;
                    output[4] = 0x82;
                    output[5] = 0x9b;
                    input += 5;
                    output += 5;
                    break;
                }

                if (from_i->a == input[1] && from_i->handakuten_b == input[2]) {
                    output[0] = FULL_HEAD;
                    output[1] = to_i->a;
                    output[2] = to_i->b;
                    output[3] = FULL_HEAD;
                    output[4] = 0x82;
                    output[5] = 0x9c;
                    input += 5;
                    output += 5;
                    break;
                }
            }
        } else {
            output = input;
        }
    }

    return decomposed_input;
}

// rewrites dakuten (゛) and handakuten (゜) to composed characters
uint8_t*
kata_tenten_compose(uint8_t* input)
{
    uint8_t* search = input;
    uint8_t length = strlen((char*)search);

    // see if we have the start of something that looks like a character in there
    uint8_t* needle = memchr(search, FULL_HEAD, length);

    // if there aren't any characters we're interested in, bail early
    if (needle == NULL) {
        printf("needle is null");
        return NULL;
    }

    uint8_t* decomposed_input = malloc(length * 2);
}

int main(void) {
    uint8_t str[] = "`ヴ` <=> `ウ゛`\n";
    printf("%s\n", str);

    uint8_t* decomposed_str = kata_tenten_decompose(str);
    printf("%s\n", (char*)decomposed_str);

    uint8_t* composed_str = kata_tenten_compose(str);
    printf("%s\n", (char*)composed_str);
}