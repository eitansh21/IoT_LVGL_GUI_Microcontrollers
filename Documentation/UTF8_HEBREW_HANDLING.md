# UTF-8 and Hebrew Text Handling

## The Problem

Hebrew characters are 2-3 bytes each in UTF-8. Standard C string functions count bytes, not characters, leading to incorrect truncation and length calculations.

```cpp
char* hebrew = "שלום";        // 4 characters, 8 bytes
int wrong = strlen(hebrew);   // Returns 8 (bytes), not 4 (characters)
```

## UTF-8 Aware Functions from This Project

### Character Counting
```cpp
static int utf8_char_count(const char* text) {
    if (!text) return 0;

    int char_count = 0;
    const unsigned char* p = (const unsigned char*)text;

    while (*p) {
        // Skip UTF-8 continuation bytes (10xxxxxx)
        if ((*p & 0xC0) != 0x80) {
            char_count++;
        }
        p++;
    }
    return char_count;
}
```

### Finding Byte Position of Nth Character
```cpp
static int utf8_char_to_byte_pos(const char* text, int char_pos) {
    if (!text || char_pos <= 0) return 0;

    int char_count = 0;
    int byte_pos = 0;
    const unsigned char* p = (const unsigned char*)text;

    while (*p && char_count < char_pos) {
        if ((*p & 0xC0) != 0x80) {
            char_count++;
        }
        if (char_count < char_pos) {
            byte_pos++;
        }
        p++;
    }
    return byte_pos;
}
```

### Safe Text Truncation
```cpp
static char* create_truncated_text(const char* full_text, int max_chars, const char* suffix) {
    if (!full_text) return NULL;

    int full_char_count = utf8_char_count(full_text);
    int suffix_char_count = suffix ? utf8_char_count(suffix) : 0;

    // Check if truncation needed
    if (full_char_count <= max_chars) {
        return strdup(full_text);
    }

    // Calculate how many characters to keep
    int truncate_char_count = max_chars - suffix_char_count;
    if (truncate_char_count <= 0) {
        return suffix ? strdup(suffix) : strdup("");
    }

    // Find correct byte position for truncation
    int truncate_byte_pos = utf8_char_to_byte_pos(full_text, truncate_char_count);

    // Create result
    size_t result_size = truncate_byte_pos + (suffix ? strlen(suffix) : 0) + 1;
    char* result = malloc(result_size);

    if (result) {
        memcpy(result, full_text, truncate_byte_pos);
        result[truncate_byte_pos] = '\0';

        if (suffix) {
            strcat(result, suffix);
        }
    }

    return result;
}
```

## Usage Example

```cpp
// From expandable card widget
const char* long_hebrew = "זהו טקסט ארוך בעברית שצריך להיות מקוצר כראוי";

// ❌ Wrong - may cut in middle of character
char bad[20];
strncpy(bad, long_hebrew, 19);

// ✅ Correct - UTF-8 aware truncation
char* good = create_truncated_text(long_hebrew, 15, "...");
lv_label_set_text(label, good);
free(good);
```

## How UTF-8 Works

UTF-8 uses different byte patterns:
- **ASCII (1 byte):** `0xxxxxxx`
- **2-byte char:** `110xxxxx 10xxxxxx`
- **3-byte char:** `1110xxxx 10xxxxxx 10xxxxxx`

**Continuation bytes** always start with `10xxxxxx` - that's how we skip them when counting.

## When to Use These Functions

**Use UTF-8 aware functions for:**
- Text truncation in widgets
- Character counting for UI limits
- Substring operations with Hebrew

**Regular functions are fine for:**
- File paths and URLs
- ASCII-only strings
- Binary data