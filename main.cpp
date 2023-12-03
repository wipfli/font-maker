#define BOOST_NO_CXX98_FUNCTION_BASE
// workaround unary_function in boost::geometry
#include <cstdint>
#include "mapbox/glyph_foundry.hpp"
#include "mapbox/glyph_foundry_impl.hpp"
#include <protozero/pbf_writer.hpp>

#ifndef EMSCRIPTEN
#include "ghc/filesystem.hpp"
#include "cxxopts.hpp"
#endif
#include <iostream>

using namespace std;

// // ភ្នំពេញ
// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 70;
// //                                      65  66   67   68   69  70
// const std::vector<int> mapping_index = {51, 185, 113, 107, 50, 34};
// //                        height        14  5   6
// // const std::vector<int> y_offset    = {0, -26, -29, 0, 0, 0};
//    const std::vector<int> mapping_top = {0, -15, 7, 0, 0, 0};
// //                        width          13   11   5
// // const std::vector<int> x_offset     = {0,   2, -23, 0, 0, 0};
//    const std::vector<int> mapping_left = {0, -12, -8, 0, 0, 0};
// const std::vector<int> mapping_advance = {13,  0,   0, 6, 13, 20};


// //នុ
// // char code 65
// // index 45, width 11
// // index 45, height 14
// // char code 66
// // index 91, width 3
// // index 91, height 5

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {45, 91};
// const std::vector<int> mapping_top = {0, -26};
// // harfbuzz
// //const std::vector<int> mapping_left = {0, -21};
// const std::vector<int> mapping_left = {0, -22};
// const std::vector<int> mapping_advance = {26, 0};


// // សុ
// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {59, 91};
// const std::vector<int> mapping_top = {14, -26};
// const std::vector<int> mapping_left = {0, -6};
// const std::vector<int> mapping_advance = {19, 0};

// harfbuzz x_advance = 928, glyph width = 19, ratio 928 / 19 = 48.84
// conclusion: probably the conversion from harfbuzz advance to maplibre advance is a factor of 1/48

// [
//   {
//     "word": "\u179f\u17bb",
//     "glyphs": [
//       {
//         "index": 59,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 928,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 91,
//         "x_offset": -6,
//         "y_offset": -26,
//         "x_advance": 0,
//         "y_advance": 0,
//         "cluster": 0
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSansKhmer-Regular.ttf"
//   }
// ]

// char code 65
// index 59, width 19
// index 59, height 14
// char code 66
// index 91, width 3
// index 91, height 5

// // កx

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 65;
// const std::vector<int> mapping_index = {25};
// const std::vector<int> mapping_top = {14};
// const std::vector<int> mapping_left = {0};
// const std::vector<int> mapping_advance = {11};

// harfbuzz x_advance is 636, glyph width is 11, ratio 636 / 11 = 57.81
// on the other hand 636 / 48 = 13.25. With an advance of 13 it looks OK
// [
//   {
//     "word": "\u1780",
//     "glyphs": [
//       {
//         "index": 25,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 636,
//         "y_advance": 0,
//         "cluster": 0
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSansKhmer-Regular.ttf"
//   }
// ]


// char code 65
// index 25, width 11
// index 25, height 14

// Te

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {55, 72};
// const std::vector<int> mapping_top = {17, 13};
// const std::vector<int> mapping_left = {0, 0};
// const std::vector<int> mapping_advance = {13, 11};

// harfbuzz x_advance on the T is 486, glyph width is 13, ratio is 486 / 13 = 37.38
// on the other hand, assume the correct scaling is 48, then the advance should
// be 486 / 48 = 10.12, i.e., 10

// [
//   {
//     "word": "Te",
//     "glyphs": [
//       {
//         "index": 55,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 486,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 72,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 564,
//         "y_advance": 0,
//         "cluster": 1
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"
//   }
// ]

// char code 65
// index 55, width 13
// index 55, height 17
// char code 66
// index 72, width 11
// index 72, height 13

// សិx


// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {59, 81};
// const std::vector<int> mapping_top = {14, 20};
// const std::vector<int> mapping_left = {0, -11};
// const std::vector<int> mapping_advance = {19, 0};

// harfbuzz x_advance 928 / 48 = 19.33

// harfbuzz x_offset = -32
// empirical left = -32 / 48 - 10 = -10.66 = -11 (10 is the width of the second glyph)

// harfbuzz y_offset = -29
// empirical y_offset = -29 / 48 * (-1) + 14 + 5 = 19.6 = 20 (14 is the height of the first glyph, 5 is the height of the second glyph)

// [
//   {
//     "word": "\u179f\u17b7",
//     "glyphs": [
//       {
//         "index": 59,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 928,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 81,
//         "x_offset": -32,
//         "y_offset": -29,
//         "x_advance": 0,
//         "y_advance": 0,
//         "cluster": 0
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSansKhmer-Regular.ttf"
//   }
// ]

// char code 65
// index 59, width 19
// index 59, height 14
// char code 66
// index 81, width 10
// index 81, height 5


// ខំ

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {26, 113};
// const std::vector<int> mapping_top = {14, 22};
// const std::vector<int> mapping_left = {0, -10};
// const std::vector<int> mapping_advance = {13, 0};

// harfbuzz x_advance is 635, 635 / 48 = 13.22 = 13

// second glyph top is 22
// harfbuzz y_offset is -29, -29 / 48 = -1

// second glyph left is -10

// [
//   {
//     "word": "\u1781\u17c6",
//     "glyphs": [
//       {
//         "index": 26,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 635,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 113,
//         "x_offset": -20,
//         "y_offset": -29,
//         "x_advance": 0,
//         "y_advance": 0,
//         "cluster": 0
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSansKhmer-Regular.ttf"
//   }
// ]

// Adding khmer.ttf
// Adding roboto.ttf
// char code 65
// index 26, width 11
// index 26, height 14
// index 26, top 14
// index 26, left 2
// index 26, advance 0
// char code 66
// index 113, width 6
// index 113, height 6
// index 113, top 22
// index 113, left -10
// index 113, advance 0


// // ag

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {68, 74};
// const std::vector<int> mapping_top = {13, 13};
// const std::vector<int> mapping_left = {1, 1};
// const std::vector<int> mapping_advance = {12, 13};

// harfbuzz x_advance 561 / 48 = 11.68 = 12
// harfbuzz x_advance 615 / 48 = 12.81 = 13

// [
//   {
//     "word": "ag",
//     "glyphs": [
//       {
//         "index": 68,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 561,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 74,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 615,
//         "y_advance": 0,
//         "cluster": 1
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"
//   }
// ]


// char code 65
// index 68, width 11
// index 68, height 13
// index 68, top 13
// index 68, left 1
// char code 66
// index 74, width 12
// index 74, height 19
// index 74, top 13
// index 74, left 1


// ថ្នូ

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 67;
// const std::vector<int> mapping_index = {42, 185, 93};
// const std::vector<int> mapping_top = {16, -2, -7};
// const std::vector<int> mapping_left = {1, -13 + 3, -11 + 3};
// const std::vector<int> mapping_advance = {13, 0, 0};

// 635 / 48 = 13.22 = 13

// first
// index 42, top 16
// index 42, left 1

// second
// index 185, top -1
// index 185, left -13
// harfbuzz
//         "y_offset": -26,
//         "x_offset": 5,
// effective
// top -1 + -26/48 = -2
// left -13 + 5/48 = -13

// third
// index 93, top -1
// index 93, left -11
// harfbuzz
//         "y_offset": -288,
//         "x_offset": 5,
// effective
// top -1 + -288/48 = -7
// left -11 + 5 /48 = -11


// [
//   {
//     "word": "\u1790\u17d2\u1793\u17bc",
//     "glyphs": [
//       {
//         "index": 42,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 635,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 185,
//         "x_offset": 5,
//         "y_offset": -26,
//         "x_advance": 0,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 93,
//         "x_offset": 5,
//         "y_offset": -288,
//         "x_advance": 0,
//         "y_advance": 0,
//         "cluster": 0
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSansKhmer-Regular.ttf"
//   }
// ]

// Adding khmer.ttf
// Adding roboto.ttf
// char code 65
// index 42, width 12
// index 42, height 16
// index 42, top 16
// index 42, left 1
// index 42, advance 0
// char code 66
// index 185, width 11
// index 185, height 5
// index 185, top -1
// index 185, left -13
// index 185, advance 0
// char code 67
// index 93, width 7
// index 93, height 5
// index 93, top -1
// index 93, left -11
// index 93, advance 0

// ប់


// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {46, 122};
// const std::vector<int> mapping_top = {14, 21 - 1};
// const std::vector<int> mapping_left = {1, -8 - (-1)};
// const std::vector<int> mapping_advance = {13, 0};

// 635 / 48  = 13.2 = 13
// [
//   {
//     "word": "\u1794\u17cb",
//     "glyphs": [
//       {
//         "index": 46,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 635,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 122,
//         "x_offset": -46,
//         "y_offset": -29,
//         "x_advance": 0,
//         "y_advance": 0,
//         "cluster": 0
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSansKhmer-Regular.ttf"
//   }

// char code 65
// index 46, width 12
// index 46, height 14
// index 46, top 14
// index 46, left 1
// index 46, advance 0
// char code 66
// index 122, width 2
// index 122, height 5
// index 122, top 21
// index 122, left -8
// index 122, advance 0

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {46, 122};
// const std::vector<int> mapping_x_offset = {0, -46};
// const std::vector<int> mapping_y_offset = {0, -29};
// const std::vector<int> mapping_x_advance = {635, 0};

// ញ្ចូ

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 67;
// const std::vector<int> mapping_index = {35, 165, 93};
// const std::vector<int> mapping_x_offset = {0, 20, -14};
// const std::vector<int> mapping_y_offset = {0, -26, -319};
// const std::vector<int> mapping_x_advance = {952, 0, 0};

// [
//   {
//     "word": "\u1789\u17d2\u1785\u17bc",
//     "glyphs": [
//       {
//         "index": 35,
//         "x_offset": 0,
//         "y_offset": 0,
//         "x_advance": 952,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 165,
//         "x_offset": 20,
//         "y_offset": -26,
//         "x_advance": 0,
//         "y_advance": 0,
//         "cluster": 0
//       },
//       {
//         "index": 93,
//         "x_offset": -14,
//         "y_offset": -319,
//         "x_advance": 0,
//         "y_advance": 0,
//         "cluster": 0
//       }
//     ],
//     "direction": "ltr",
//     "font_path": "/usr/share/fonts/truetype/noto/NotoSansKhmer-Regular.ttf"
//   }
// ]

// ក្នុងឋានៈ


// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 71;
// const std::vector<int> mapping_index = {25, 185, 91, 29, 236, 45, 116};
// const std::vector<int> mapping_x_offset = {0, 4, 4, 0, 0, 0, 0};
// const std::vector<int> mapping_y_offset = {0, -26, -288, 0, 0, 0, 0};
// const std::vector<int> mapping_x_advance = {636, 0, 0, 635, 923, 635, 283};

// សិទ្ធិទទួលបានសន្ដិសុខសង្គម

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 86;
// const std::vector<int> mapping_index = {59, 81, 43, 184, 81, 43, 43, 95, 55, 254, 45, 59, 45, 172, 81, 59, 91, 26, 59, 29, 161, 52};
// const std::vector<int> mapping_x_offset = {0, -32, 0, 17, -10, 0, 0, -3, 0, 0, 0, 0, 0, -1, -36, 0, -6, 0, 0, 0, 0, 0};
// const std::vector<int> mapping_y_offset = {0, -29, 0, -26, -29, 0, 0, -26, 0, 0, 0, 0, 0, -26, -29, 0, -26, 0, 0, 0, -26, 0};
// const std::vector<int> mapping_x_advance = {928, 0, 599, 0, 0, 599, 599, 0, 928, 923, 635, 928, 635, 0, 0, 928, 0, 635, 928, 635, 0, 635};

// ក្ដីថ្លៃថ្នូរ

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 74;
// const std::vector<int> mapping_index = {25, 172, 85, 109, 42, 198, 42, 185, 93, 54};
// const std::vector<int> mapping_x_offset = {0, -2, -23, 0, 0, -1, 0, 5, 5, 0};
// const std::vector<int> mapping_y_offset = {0, -26, -29, 0, 0, -26, 0, -26, -288, 0};
// const std::vector<int> mapping_x_advance = {636, 0, 0, 288, 635, 0, 635, 0, 0, 288};

// ట్టి



// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 66;
// const std::vector<int> mapping_index = {184, 480};
// const std::vector<int> mapping_x_offset = {0, -100};
// const std::vector<int> mapping_y_offset = {0, 0};
// const std::vector<int> mapping_x_advance = {772, 0};


// టుం

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 67;
// const std::vector<int> mapping_index = {33, 63, 6};
// const std::vector<int> mapping_x_offset = {0, 0, 0};
// const std::vector<int> mapping_y_offset = {0, 0, 0};
// const std::vector<int> mapping_x_advance = {772, 346, 507};


// గౌరవప్రతిష్థలను

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 75;
// const std::vector<int> mapping_index = {436, 49, 54, 43, 569, 189, 56, 486, 51, 42, 63};
// const std::vector<int> mapping_x_offset = {0, 0, 0, 0, 0, 0, 0, -108, 0, 0, 0};
// const std::vector<int> mapping_y_offset = {0, 0, 0, 0, 0, 0, 0, -30, 0, 0, 0};
// const std::vector<int> mapping_x_advance = {767, 593, 711, 711, 0, 782, 713, 0, 709, 702, 346};

// ဖမ်းဆီးခြင်းကို

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 79;
// const std::vector<int> mapping_index = {26, 29, 381, 379, 10, 370, 379, 47, 5, 8, 381, 379, 4, 369, 209};
// const std::vector<int> mapping_x_offset = {0, 0, -35, 0, 0, -35, 0, 0, 0, 0, -7, 0, 0, -27, -5};
// const std::vector<int> mapping_y_offset = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// const std::vector<int> mapping_x_advance = {675, 676, 0, 346, 1127, 0, 346, 229, 676, 650, 0, 346, 1124, 0, 0};

// ၍ည်ကြာခြ

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 72;
// const std::vector<int> mapping_index = {63, 14, 381, 198, 4, 368, 47, 5};
// const std::vector<int> mapping_x_offset = {0, 0, -7, 0, 0, 0, 0, 0};
// const std::vector<int> mapping_y_offset = {0, 0, 0, 0, 0, 0, 0, 0};
// const std::vector<int> mapping_x_advance = {892, 1109, 0, 229, 1124, 455, 229, 676};

// काठमाण्डौँ

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 72;
// const std::vector<int> mapping_index = {25, 66, 36, 50, 66, 197, 37, 743};
// const std::vector<int> mapping_x_offset = {0, 0, 0, 0, 0, 0, 0, 0};
// const std::vector<int> mapping_y_offset = {0, 0, 0, 0, 0, 0, 0, 0};
// const std::vector<int> mapping_x_advance = {762, 259, 585, 598, 259, 430, 577, 259};

// धुलिखेल

// const int first_indexed_codepoint = 65;
// const int last_indexed_codepoint = 71;
// const std::vector<int> mapping_index = {43, 69, 609, 54, 26, 75, 54};
// const std::vector<int> mapping_x_offset = {0, 0, 0, 0, 0, 0, 0};
// const std::vector<int> mapping_y_offset = {0, 0, 0, 0, 0, 0, 0};
// const std::vector<int> mapping_x_advance = {615, 0, 259, 678, 818, 0, 678};

// nepali cities

const int first_indexed_codepoint = 58000;
const int last_indexed_codepoint = 58109;
const std::vector<int> mapping_index = {36, 269, 60, 542, 511, 50, 6, 258, 181, 75, 43, 210, 69, 183, 202, 39, 49, 612, 75, 183, 32, 207, 652, 256, 219, 202, 203, 271, 274, 202, 183, 207, 23, 48, 653, 51, 614, 618, 497, 608, 606, 9, 201, 41, 13, 27, 29, 197, 80, 203, 12, 202, 25, 79, 215, 494, 417, 26, 464, 11, 57, 70, 559, 5, 54, 30, 47, 10, 40, 453, 69, 81, 210, 76, 531, 610, 58, 38, 635, 609, 197, 655, 450, 611, 28, 557, 605, 607, 46, 68, 42, 44, 179, 181, 14, 75, 419, 69, 52, 5, 69, 61, 33, 521, 207, 31, 66, 37, 35, 75};
const std::vector<int> mapping_x_offset = {0, 0, 0, 0, 0, 0, 0, 0, 0, -6, 0, 0, -221, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -35, 0, 0, 0, 0, 0, 0, -222, -147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -221, 0, -76, 0, -88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -221};
const std::vector<int> mapping_y_offset = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const std::vector<int> mapping_x_advance = {585, 552, 676, 903, 1014, 598, 0, 647, 0, 0, 615, 451, 0, 533, 286, 723, 703, 259, 0, 539, 742, 350, 0, 563, 440, 309, 301, 520, 568, 298, 529, 325, 1023, 571, 259, 580, 259, 259, 647, 259, 259, 764, 362, 642, 548, 563, 647, 456, 259, 308, 491, 318, 762, 259, 389, 859, 563, 818, 259, 491, 556, 0, 696, 0, 678, 634, 771, 1023, 570, 0, 0, 0, 457, 0, 791, 259, 680, 563, 259, 259, 430, 259, 0, 259, 591, 922, 259, 259, 568, 259, 520, 555, 712, 0, 765, 0, 520, 0, 409, 0, 0, 531, 756, 681, 337, 701, 259, 577, 504, 0};

void do_codepoint(protozero::pbf_writer &parent, std::vector<FT_Face> &faces, FT_ULong char_code) {
    for (auto const &face : faces) {
        FT_UInt char_index = 0;
        int mapping_id = char_code - first_indexed_codepoint;
        if (first_indexed_codepoint <= char_code && char_code <= last_indexed_codepoint) {
            char_index = mapping_index[mapping_id];
        }
        else {
            char_index = FT_Get_Char_Index(face, char_code);
        }
        if (char_index > 0) {
            sdf_glyph_foundry::glyph_info glyph;
            glyph.glyph_index = char_index;
            sdf_glyph_foundry::RenderSDF(glyph, 24, 3, 0.25, face);

            string glyph_data;
            protozero::pbf_writer glyph_message{glyph_data};

            if (first_indexed_codepoint <= char_code && char_code <= last_indexed_codepoint) {
                printf("before changes\n");
                printf("char code %d\n", char_code);
                printf("index %d, width %d\n", char_index, glyph.width);
                printf("index %d, height %d\n", char_index, glyph.height);
                printf("index %d, top %d\n", char_index, glyph.top);
                printf("index %d, left %d\n", char_index, glyph.left);
                printf("index %d, advance %d\n", char_index, glyph.advance);

                glyph.top += mapping_y_offset[mapping_id] / 48; // TODO use Round instead
                // if (mapping_x_offset[mapping_id] != 0) {
                //     glyph.left += int(round(float(mapping_x_offset[mapping_id]) / 48.0));
                //     glyph.left += 2;
                // }
                
                // glyph.advance = int(round(float(mapping_x_advance[mapping_id]) / 40.0));
            }

            // direct type conversions, no need for checking or casting
            glyph_message.add_uint32(3,glyph.width);
            glyph_message.add_uint32(4,glyph.height);
            glyph_message.add_sint32(5,glyph.left);

            // conversions requiring checks, for safety and correctness

            // shortening conversion
            if (char_code > numeric_limits<uint32_t>::max()) {
                throw runtime_error("Invalid value for char_code: too large");
            } else {
                glyph_message.add_uint32(1,static_cast<uint32_t>(char_code));
            }

            // node-fontnik uses glyph.top - glyph.ascender, assuming that the baseline
            // will be based on the ascender. However, Mapbox/MapLibre shaping assumes
            // a baseline calibrated on DIN Pro w/ ascender of ~25 at 24pt
            int32_t top = glyph.top - 25;
            if (top < numeric_limits<int32_t>::min() || top > numeric_limits<int32_t>::max()) {
                throw runtime_error("Invalid value for glyph.top-25");
            } else {
                glyph_message.add_sint32(6,top);
            }

            // double to uint
            if (glyph.advance < numeric_limits<uint32_t>::min() || glyph.advance > numeric_limits<uint32_t>::max()) {
                throw runtime_error("Invalid value for glyph.top-glyph.ascender");
            } else {
                glyph_message.add_uint32(7,static_cast<uint32_t>(glyph.advance));
            }

            if (glyph.width > 0) {
                glyph_message.add_bytes(2,glyph.bitmap);
            }
            parent.add_message(3,glyph_data);
            return;
        }
    }
}

string do_range(std::vector<FT_Face> &faces, std::string name, unsigned start, unsigned end) {
    string fontstack_data;
    {
        protozero::pbf_writer fontstack{fontstack_data};

        fontstack.add_string(1,name);
        fontstack.add_string(2,to_string(start) + "-" + to_string(end)); 

        for (unsigned x = start; x <= end; x++) {
            FT_ULong char_code = x;
            do_codepoint(fontstack,faces, x);
        }
    }

    string glyphs_data;
    {
        protozero::pbf_writer glyphs{glyphs_data};
        glyphs.add_message(1,fontstack_data);
    }
    return glyphs_data;
}

struct fontstack {
    FT_Library library;
    std::vector<FT_Face> *faces;
    std::vector<char *> *data;
    std::set<std::string> *seen_face_names;
    std::string *name;
    bool auto_name;
};

struct glyph_buffer {
    char *data;
    uint32_t size;
};

extern "C" {
    fontstack *create_fontstack(const char *name) {
        fontstack *f = (fontstack *)malloc(sizeof(fontstack));
        f->faces = new std::vector<FT_Face>;
        f->data = new std::vector<char *>;
        f->seen_face_names = new std::set<std::string>;

        if (name != nullptr) {
            f->name = new std::string(name);
            f->auto_name = false;
        } else {
            f->name = new std::string;
            f->auto_name = true;
        }

        FT_Library library = nullptr;
        FT_Error error = FT_Init_FreeType(&library);

        f->library = library;
        return f;
    }

    void fontstack_add_face(fontstack *f, FT_Byte *base, FT_Long data_size) {
        FT_Face face = 0;
        FT_Error face_error = FT_New_Memory_Face(f->library, base, data_size, 0, &face);
        if (face_error) {
            throw runtime_error("Could not open font face");
        }
        if (face->num_faces > 1) {
            throw runtime_error("file has multiple faces; cowardly exiting");
        }
        if (!face->family_name) {
            throw runtime_error("face does not have family name");
        }
        const double scale_factor = 1.0;
        double size = 24 * scale_factor;
        FT_Set_Char_Size(face, 0, static_cast<FT_F26Dot6>(size * (1 << 6)), 0, 0);
        f->faces->push_back(face);

        if (f->auto_name) {
            std::string combined_name = std::string(face->family_name);
            if (face->style_name != NULL) {
                combined_name += " " + std::string(face->style_name);
            }

            if (f->seen_face_names->count(combined_name) == 0) {
                if (f->seen_face_names->size() > 0) {
                  *f->name += ",";
                }
                *f->name += combined_name;
                f->seen_face_names->insert(combined_name);
            }
        }
    }

    void free_fontstack(fontstack *f) {
        for (auto fc : *f->faces) {
            FT_Done_Face(fc);
        }
        for (auto d : *f->data) {
            free(d);
        }
        FT_Done_FreeType(f->library);
        delete f->faces;
        delete f->name;
        delete f->seen_face_names;
        free(f);
    }

    char *fontstack_name(fontstack *f) {
        char *fname = (char *)malloc((f->name->size()+1) * sizeof(char));
        strcpy(fname,f->name->c_str());
        return fname;
    }

    glyph_buffer *generate_glyph_buffer(fontstack *f, uint32_t start_codepoint) {
        string result = do_range(*f->faces,*f->name,start_codepoint,start_codepoint+255);

        glyph_buffer *g = (glyph_buffer *)malloc(sizeof(glyph_buffer));
        char *result_ptr = (char *)malloc(result.size());
        result.copy(result_ptr,result.size());
        g->data = result_ptr;
        g->size = result.size();
        return g;
    }

    char *glyph_buffer_data(glyph_buffer *g) {
        return g->data;
    }

    uint32_t glyph_buffer_size(glyph_buffer *g) {
        return g->size;
    }

    void free_glyph_buffer(glyph_buffer *g) {
        free(g->data);
        free(g);
    }
}

#ifndef EMSCRIPTEN
int main(int argc, char *argv[])
{
    printf("hello\n");
    cxxopts::Options cmd_options("font-maker", "Create font PBFs.");
    cmd_options.add_options()
        ("output", "Output directory", cxxopts::value<string>())
        ("fonts", "Input fonts TTF or OTF", cxxopts::value<vector<string>>())
        ("name", "Override output fontstack name", cxxopts::value<string>())
    ;
    cmd_options.parse_positional({"output","fonts"});
    auto result = cmd_options.parse(argc, argv);
    if (result.count("output") == 0 || result.count("fonts") == 0) {
        cout << "usage: font-maker OUTPUT_DIR INPUT_FONT [INPUT_FONT2 ...]" << endl;
        exit(1);
    }
    printf("parsed\n");
    auto output_dir = result["output"].as<string>();
    auto fonts = result["fonts"].as<vector<string>>();

    if (ghc::filesystem::exists(output_dir)) {
        cout << "ERROR: output directory " << output_dir << " exists." << endl;
        exit(1);
    }
    if (ghc::filesystem::exists(output_dir)) ghc::filesystem::remove_all(output_dir);
    ghc::filesystem::create_directory(output_dir);
    printf("directory created\n");
    printf("name OK\n");

    fontstack *f = create_fontstack("the-name");
    printf("good\n");
    for (auto const &font : fonts) {
        std::ifstream file(font, std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        char * buffer = (char *)malloc(size);
        f->data->push_back(buffer);
        file.read(buffer, size);
        std::cout << "Adding " << font << std::endl;
        fontstack_add_face(f,(FT_Byte *)buffer,size);
    }

    std::string fname{fontstack_name(f)};

    ghc::filesystem::create_directory(output_dir + "/" + fname);

    for (int i = 0; i < 65536; i += 256) {
        glyph_buffer *g = generate_glyph_buffer(f,i);
        char *data = glyph_buffer_data(g);
        uint32_t buffer_size = glyph_buffer_size(g);

        ofstream output;
        std::string outname = output_dir + "/" + fname + "/" + to_string(i) + "-" + to_string(i+255) + ".pbf";
        output.open(outname);
        output.write(data,buffer_size);
        output.close();

        // std::cout << "Wrote " << outname << std::endl;

        free_glyph_buffer(g);
    }

    free_fontstack(f);

    return 0;
}
#endif
