import json

ignore_codepoints = {}

for i in range(2 ** 16):
    ignore_codepoints[i] = True

    # lowercase Latin
    if 65+26 <= i <= 65+2*26:
        ignore_codepoints[i] = False
    
    # Khmer
    if 0x1780 <= i <= 0x17FF:
        ignore_codepoints[i] = False

with open('ignore_codepoints.json', 'w') as f:
    json.dump(ignore_codepoints, f, indent=2)
