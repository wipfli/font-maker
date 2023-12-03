from glob import glob

from itertools import chain

from fontTools.ttLib import TTFont
from fontTools.unicode import Unicode

import os
import json
import unicodedata
import re
import ctypes

import fribidi

lib = ctypes.CDLL('./run_raqm.so')

lib.runRaqm.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_size_t]
lib.runRaqm.restype = None

def is_rtl_language(text):
    for char in text:
        if unicodedata.bidirectional(char) in ('R', 'AL'):
            return True
    return False

def get_glyphs(font_path, text):
    fontfile = font_path.encode('utf-8')
    text = text.encode('utf-8')
    direction = b"ltr"
    language = b"en"

    buffer_size = 2 ** 12
    buffer = ctypes.create_string_buffer(buffer_size)

    lib.runRaqm(fontfile, text, direction, language, buffer, buffer_size)
    output = buffer.value.decode('utf-8')

    if output == '\n':
        return None
    glyphs = [line_to_glyph(line) for line in output.splitlines()]
    if 0 in [glyph["index"] for glyph in glyphs]:
        return None
    return glyphs


def line_to_glyph(line):
    index, x_offset, y_offset, x_advance, y_advance, cluster = [int(num) for num in line.split()]
    return {
        "index": index,
        "x_offset": x_offset,
        "y_offset": y_offset,
        "x_advance": x_advance,
        "y_advance": y_advance,
        "cluster": cluster,
    }

def build_unicode_to_font_path():
    result = {
        # unicode codepoint decimal number: list of font path strings
    }

    font_paths = []
    # font_paths.extend(glob("/usr/share/fonts/truetype/**/*.ttf", recursive=True))
    font_paths.extend(glob("/usr/share/fonts/truetype/noto/*", recursive=True))
    # font_paths.extend(glob("/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"))

    for font_path in font_paths:
        print('reading', font_path)
        with TTFont(
            font_path, 0, allowVID=0, ignoreDecompileErrors=True, fontNumber=-1
        ) as ttf:
            chars = chain.from_iterable(
                [y + (Unicode[y[0]],) for y in x.cmap.items()] for x in ttf["cmap"].tables
            )
            for c in chars:
                if c[0] in result:
                    if font_path not in result[c[0]]:
                        result[c[0]].append(font_path)
                else:
                    result[c[0]] = [font_path]

    return result

def find_font_with_raqm(folder, text):
    for font_path in glob(folder + "/**/*", recursive=True):
        glyphs = get_glyphs(font_path, text)
        if glyphs is not None:
            return font_path
    return None

unicode_to_font_path = {}

filename = 'unicode_to_font_path.json'
if os.path.exists(filename):
    with open(filename) as f:
        data = json.load(f)
        for key, value in data.items():
            unicode_to_font_path[int(key)] = value
else:
    unicode_to_font_path = build_unicode_to_font_path()
    with open(filename, 'w') as f:
        json.dump(unicode_to_font_path, f)

def find_fonts(text):
    font_to_character_count = {}
    for letter in text:
        if ord(letter) in unicode_to_font_path:
            for font_path in unicode_to_font_path[ord(letter)]:
                if font_path in font_to_character_count:
                    font_to_character_count[font_path] += 1
                else:
                    font_to_character_count[font_path] = 1
    
    complete_fonts = []
    for font_path in font_to_character_count:
        if font_to_character_count[font_path] == len(text):
            complete_fonts.append(font_path)

    filtered_fonts = []
    for font_path in complete_fonts:
        if 'UI' in font_path:
            continue
        if 'Mono' in font_path:
            continue
        if 'Italic' in font_path:
            continue
        if 'Display' in font_path:
            continue
        if 'Bold' in font_path:
            continue
        if 'Serif' in font_path:
            continue
        filtered_fonts.append(font_path)
    
    if len(filtered_fonts) > 0:
        return filtered_fonts
    
    if len(complete_fonts) > 0:
        return complete_fonts
    
    # maybe it is a CJK string

    cjk_font_path = '/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc'
    if get_glyphs(cjk_font_path, text) is not None:
        return [cjk_font_path]
    
    return []

def find_font(text):
    fonts = find_fonts(text)

    if len(fonts) == 0:
        return None
    
    return fonts[0]

def can_break_after(font_path, text, break_after):

    glyphs = get_glyphs(font_path, text)
    # pprint(glyphs)

    text_before = text[0:(break_after + 1)]
    text_after = text[(break_after + 1):]
        
    # print(text_before, text_after)

    glyphs_before = get_glyphs(font_path, text_before)
    if glyphs_before is None:
        return False
    
    glyphs_after = get_glyphs(font_path, text_after)
    if glyphs_after is None:
        return False

    max_cluster_before = max([glyph['cluster'] for glyph in glyphs_before])
    
    for glyph in glyphs_after:
        glyph['cluster'] += max_cluster_before + 1

    if is_rtl_language(text):
        glyphs_broken = glyphs_after + glyphs_before
    else:
        glyphs_broken = glyphs_before + glyphs_after

    # pprint(glyphs_broken)

    if len(glyphs) != len(glyphs_broken):
        # print('not same length')
        return False
    
    properties = ['index', 'x_advance', 'x_offset', 'y_advance', 'y_offset']
    last_cluster = -1
    last_cluster_broken = -1
    for i in range(len(glyphs)):
        for property in properties:
            if glyphs[i][property] != glyphs_broken[i][property]:
                # print('not same property', i, property, glyphs[i][property], glyphs_broken[i][property])
                return False
        if glyphs[i]['cluster'] != last_cluster or glyphs_broken[i]['cluster'] != last_cluster_broken:
            if glyphs[i]['cluster'] == last_cluster or glyphs_broken[i]['cluster'] == last_cluster_broken:
                # print('cannot break i', i)
                return False
            last_cluster = glyphs[i]['cluster']
            last_cluster_broken = glyphs_broken[i]['cluster']
    
    return True

    
# print(font_path)

# print('can break after', break_after, can_break_after(font_path, text, break_after))

def segment_word(font_path, text, direction):
    cursor = 0
    parts = []
    for break_after in range(len(text)):
        if can_break_after(font_path, text, break_after):
            if direction == 'rtl':
                parts.insert(0, text[cursor:(break_after + 1)])
            else:
                parts.append(text[cursor:(break_after + 1)])
            
            cursor = break_after + 1
    return parts

def split_words(string):
    # Define the pattern to match special characters (+ or -)
    pattern = r'([\?\.\,\+\-\*/\^=<>!&\|\(\)\[\]\{\}\'\s])'

    # Use the re.split() function to split the string based on the pattern
    split_strings = re.split(pattern, string)

    # Return the split strings
    return split_strings


def split_embedding_levels(embedding_levels):
    if len(embedding_levels) == 0:
        return []
    
    result = [] # [[0, 1], [1, 3], ..] with [from (including), to (excluding)]
    sublist = [0, 0] 
    
    for i in range(1, len(embedding_levels)):
        if embedding_levels[i - 1] != embedding_levels[i]:
            sublist[1] = i
            result.append(list(sublist))
            sublist = [i, i]
    
    sublist[1] = len(embedding_levels)
    result.append(list(sublist))
    return result

def get_bidis(text):
    bidi_types = fribidi.get_bidi_types(text)

    _, embedding_levels = fribidi.get_par_embedding_levels(bidi_types, fribidi.FRIBIDI.PAR_LTR)[1:]
        
    sections = split_embedding_levels(embedding_levels)

    bidis = [] # [{'text': 'oliver', 'direction': 'ltr'}, ...]
    
    for section in sections:
        bidis.append({
            'text': text[section[0]:section[1]],
            'direction': 'ltr' if embedding_levels[section[0]] % 2 == 0 else 'rtl'
        })

    return bidis

def segment_label(text):

    bidis = get_bidis(text)

    parts = []
    for bidi in bidis:

        words = split_words(bidi['text'])

        if len(words) == 0:
            continue

        subparts = []

        for word in words:
            font_path = find_font(word)
            word_parts = []
            if font_path is None:
                word_parts = [word]
            else:
                word_parts = segment_word(font_path, word, bidi['direction'])
            
            if bidi['direction'] == 'ltr':
                subparts = subparts + word_parts
            else:
                subparts = word_parts + subparts

        parts += subparts

    return parts

def shape_label(text):

    bidis = get_bidis(text)

    parts = []
    for bidi in bidis:

        words = split_words(bidi['text'])

        if len(words) == 0:
            continue

        for word in words:
            font_path = find_font(word)
            glyphs = []
            if font_path is not None:
                glyphs = get_glyphs(font_path, word)
            parts.append({
                'word': word,
                'glyphs': glyphs,
                'direction': bidi['direction'],
                'font_path': font_path,
            })
            
    return parts


#############################################

# app = FastAPI()

# cache_filename = 'cache.csv'

# cache = {}

# if os.path.exists(cache_filename):
#     print('reading cache')
#     with open(cache_filename) as f:
#         for line in f.readlines():
#             line = line.strip()
#             encoded_request, encoded_result = line.split(',')
#             cache[encoded_request] = encoded_result

# def add_to_cache(encoded_request, encoded_result):
#     if encoded_request not in cache:
#         cache[encoded_request] = encoded_result
#         with open(cache_filename, 'a') as f:
#             f.write(f'{encoded_request},{encoded_result}\n')

# @app.get("/")
# async def root():
#     return {"message": "Hello World"}

# @app.get("/segment", response_class=HTMLResponse)
# async def segment(text: str):

#     encoded_request = urllib.parse.quote(text)
#     print('requested', text, encoded_request)

#     if encoded_request in cache:
#         print('cache hit')
#     else:
#         print('cache miss')

#     tic = time.time()
    
#     encoded_result = ''

#     if encoded_request in cache:
#         encoded_result = cache[encoded_request]
#     else:
#         parts = segment_label(text)
#         if parts:
#             encoded_result = urllib.parse.quote(''.join(["@".join(part) for part in parts]))
#             add_to_cache(encoded_request, encoded_result)

#     print('duration', time.time() - tic)
#     return encoded_result

# @app.get("/shape", response_class=HTMLResponse)
# async def shape(text: str):
#     encoded_request = urllib.parse.quote(text)
#     print('requested', text, encoded_request)
#     parts = shape_label(text)
#     return json.dumps(parts)

# if __name__ == "__main__":
#     uvicorn.run(app, host="0.0.0.0", port=3000)


# font_path = '/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf'
# text = 'モナコ'

# print(find_font(text))

# tic = time.time()
# for _ in range(1000):
#     segment_word(font_path, text)
# print(time.time() - tic)

# text = 'મુંબઈ मुंबई دبي-بي Oliver'  
# text = 'ᄀᄀᄀ각ᆨᆨ' # 'دبي' # 'મુંબઈ' # 'मुंबई' #'oliver' # 'モナコ'
# text = '456 תל־אבי  213ב-יפ Wip'
# text = 'جمهو What is going on here? رية مصر العربية'
# # text = 'oliver'
# parts = segment_label(text)

# if parts is not None:
#     for part in parts:
#         print(part)

# const int first_indexed_codepoint = 65;
# const int last_indexed_codepoint = 66;
# const std::vector<int> mapping_index = {107, 103};
# const std::vector<int> mapping_top = {-10, 14};
# const std::vector<int> mapping_left = {4, -254};
# const std::vector<int> mapping_advance = {0, 0};

# မည်သူမျှ ဥပဒေအရ မဟုတ်သော ဖမ်းဆီးခြင်းကို ဖြစ်စေ၊ ချုပ်နှောင်ခြင်းကို ဖြစ်စေ၊ ပြည်နှင်ခြင်းကို ဖြစ်စေ မခံစေရ။ အခွင့်အရေးများနှင့် တာဝန် ဝတ္တရားများကို အဆုံးအဖြတ်ခံရာတွင် လည်းကောင်း၊ ပြစ်မှုကြောင့် တရားစွဲဆို စီရင်ဆုံးဖြတ်ခံရာတွင် လည်းကောင်း၊ လူတိုင်းသည် လွတ်လပ်၍ ဘက်မလိုက်သော တရားရုံးတော်၏ လူအများ ရှေ့မှောက်တွင် မျှတစွာ ကြားနာစစ်ဆေးခြင်းကို တူညီစွာ ခံစား
# ఆంతరంగిక, కుటుంబ, గృహ, లేఖావ్యవహారములలో, విధి విరుద్ధమయిన జోక్యమునకుగాని, గౌరవప్రతిష్థలను భంగపరచు ప్రచారములకుగాని యెవరిని గురిచేయరాదు. అట్టి జోక్యము నుండియు, ఆ ప్రచారముల నుండియు విధి ద్వారా పరిరక్షింపబడుటకు ప్రతి యొక్కరికిని హక్కు గలదు. ప్రతి వ్యక్తికిని భావస్వాతంత్ర్య, అంతఃకరణస్వాతంత్ర్య, మతస్వాతంత్ర్యములకు హక్కు గలదు. తన మతమును ప్రత్యయమును మార్చుకొనుటయును, ఒంటరిగ గాని, సాంఘికముగ గాని, బహిరంగముగను, 
text = 'धुलिखेल' # 'काठमाण्डौँ' # 'ក្នុងឋានៈជាសមាជិកនៃសង្គម មនុស្សគ្រប់រូប មានសិទ្ធិទទួលបានសន្ដិសុខសង្គម និងមានបុព្វ សិទ្ធិសម្រេចបានសិទ្ធិខាងសេដ្ឋកិច្ច សង្គមកិច្ច និងវប្បធម៌ ដែលចាំបាច់សម្រាប់សេចក្ដីថ្លៃថ្នូរ និងការ រីកចំរើនដោយសេរីនៃបុគ្គលិកលក្ខណៈរបស់ខ្លួន តាមរយៈការខិតខំរបស់ជាតិ និងសហប្រតិបត្ដិការអន្ដរ ជាតិ និងដោយយោងទៅតាមការរៀបចំ និងធនធានរបស់ប្រទេសនីមួយៗ។ មនុស្សគ្រប់រូប មានសិទ្ធិឈប់សម្រាក និងលំហែកំសាន្ដ រួមបញ្ចូលទាំងកម្រិតម៉ោងការងារ សមហេតុផល និងការឈប់សម្រាក ដោយបានប្រាក់បៀវត្សតាមពេលកំណត់ទៀងទាត់។ មនុស្សគ្រប់រូប មានសិទ្ធិទទួលបាននូវសណ្ដាប់ធ្នាប់សង្គមនិងអន្ដរជាតិ ដែលធ្វើឱ្យសិទ្ធិ និង សេរីភាព ចែងក្នុងសេចក្ដីប្រកាសនេះ អាចសម្រេចបានដោយពេញលេញ។ គ្មានបទបញ្ញត្ដិណាមួយនៃសេចក្ដីប្រកាសនេះ អាចត្រូវបានបកស្រាយ តម្រូវថា រដ្ឋណាមួយ ក្រុមណាមួយ ឬបុគ្គលណាម្នាក់ មានសិទ្ធិបែបណាមួយ ក្នុងការធ្វើសកម្មភាព ឬការប្រព្រឹត្ដអំពើអ្វ្វីមួយ ដែលសំដៅទៅបំផ្លិចបំផ្លាញនូវសិទ្ធិ និងសេរីភាពទាំងឡាយ ដែលមានចែងនៅក្នុងសេចក្ដីប្រកាសនេះ ឡើយ។'
parts = shape_label(text)

glyphs = parts[0]['glyphs']

mapping_index = [glyph['index'] for glyph in glyphs]
mapping_x_offset = [int(glyph['x_offset'] / 1) for glyph in glyphs]
mapping_y_offset = [int(glyph['y_offset'] / 1) for glyph in glyphs]
mapping_x_advance = [int(glyph['x_advance'] / 1) for glyph in glyphs]

first_indexed_codepoint = 65
last_indexed_codepoint = first_indexed_codepoint + len(mapping_index) - 1

mapping = f'''
const int first_indexed_codepoint = {first_indexed_codepoint};
const int last_indexed_codepoint = {last_indexed_codepoint};
const std::vector<int> mapping_index = {{{", ".join([str(x) for x in mapping_index])}}};
const std::vector<int> mapping_x_offset = {{{", ".join([str(x) for x in mapping_x_offset])}}};
const std::vector<int> mapping_y_offset = {{{", ".join([str(x) for x in mapping_y_offset])}}};
const std::vector<int> mapping_x_advance = {{{", ".join([str(x) for x in mapping_x_advance])}}};
'''

print(mapping)

print(json.dumps(parts, indent=2))
# print(mapping_advance)

