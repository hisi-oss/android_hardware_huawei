#define LOG_TAG "libimonitor"

#define cJSON_False (1 << 0)
#define cJSON_True (1 << 1)
#define cJSON_NULL (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw (1 << 7)

#define cJSON_IsReference (1 << 8)
#define cJSON_StringIsConst (1 << 9)

#include <log/log.h>

#include <locale.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* ep;

struct cJSON {
    struct cJSON* next;
    struct cJSON* prev;
    struct cJSON* child;
    int type;
    char* valuestring;
    int valueint;
    double valuedouble;
    char* string;
};

static const unsigned char firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

/* The compiler is not smart enough to pick these automatically */
static const char* parse_value(struct cJSON* item, const char* value);
static char* print_value(struct cJSON* item, int depth, int fmt);
static const char* parse_array(struct cJSON* item, const char* value);
static char* print_array(struct cJSON* item, int depth, int fmt);
static const char* parse_object(struct cJSON* item, const char* value);
static char* print_object(struct cJSON* item, int depth, int fmt);

static void suffix_object(struct cJSON* prev, struct cJSON* item) {
    prev->next = item;
    item->prev = prev;
}

static const char* skip(const char* in) {
    while (in && *in && (unsigned char)*in <= 32) in++;
    return in;
}

static struct cJSON* cJSON_New_Item(void) {
    struct cJSON* node = (struct cJSON*)malloc(sizeof(struct cJSON));
    if (node) memset(node, '\0', sizeof(struct cJSON));
    return node;
}

struct cJSON* cJSON_CreateObject(void) {
    struct cJSON* item = cJSON_New_Item();
    if (item) item->type = cJSON_Object;
    return item;
}

void cJSON_Delete(struct cJSON* c) {
    struct cJSON* next;
    while (c) {
        next = c->next;
        if (!(c->type & cJSON_IsReference) && c->child) {
            cJSON_Delete(c->child);
        }
        if (!(c->type & cJSON_IsReference) && c->valuestring) {
            free(c->valuestring);
        }
        if (!(c->type & cJSON_StringIsConst) && c->string) {
            free(c->string);
        }
        free(c);
        c = next;
    }
}

struct cJSON* cJSON_CreateNumber(double num) {
    struct cJSON* item = cJSON_New_Item();
    if (item) {
        item->type = cJSON_Number;
        item->valuedouble = num;

        if (num >= INT_MAX) {
            item->valueint = INT_MAX;
        } else if (num <= INT_MIN) {
            item->valueint = INT_MIN;
        } else {
            item->valueint = (int)num;
        }
    }

    return item;
}

struct cJSON* cJSON_CreateString(const char* string) {
    struct cJSON* item = cJSON_New_Item();
    if (item) {
        item->type = cJSON_String;
        item->valuestring = strdup(string);
        if (!item->valuestring) {
            cJSON_Delete(item);
            return NULL;
        }
    }

    return item;
}

void cJSON_AddItemToArray(struct cJSON* array, struct cJSON* item) {
    struct cJSON* child = NULL;

    if ((item == NULL) || (array == NULL)) return;

    child = array->child;

    if (child == NULL) {
        array->child = item;
    } else {
        while (child->next) {
            child = child->next;
        }
        suffix_object(child, item);
    }
}

void cJSON_AddItemToObject(struct cJSON* object, const char* string, struct cJSON* item) {
    if (!item) {
        return;
    }

    if (item->string) {
        free(item->string);
    }

    item->string = strdup(string);

    cJSON_AddItemToArray(object, item);
}

int cJSON_GetArraySize(const struct cJSON* array) {
    struct cJSON* c = array->child;
    size_t i = 0;
    while (c) {
        i++;
        c = c->next;
    }

    return (int)i;
}

struct cJSON* cJSON_GetArrayItem(const struct cJSON* array, int item) {
    struct cJSON* c = array ? array->child : NULL;
    while (c && item > 0) {
        item--;
        c = c->next;
    }

    return c;
}

struct cJSON* cJSON_GetObjectItem(const struct cJSON* object, const char* string) {
    struct cJSON* c = object ? object->child : NULL;
    while (c && strcasecmp(c->string, string)) {
        c = c->next;
    }
    return c;
}

struct cJSON* cJSON_ParseWithOpts(const char* value, const char** return_parse_end,
                                  int require_null_terminated) {
    const char* end = 0;
    struct cJSON* c = cJSON_New_Item();
    if (!c) return 0;

    end = parse_value(c, skip(value));
    if (!end) {
        cJSON_Delete(c);
        return 0;
    }

    if (require_null_terminated) {
        end = skip(end);
        if (*end) {
            cJSON_Delete(c);
            return 0;
        }
    }
    if (return_parse_end) *return_parse_end = end;
    return c;
}

struct cJSON* cJSON_Parse(const char* value) {
    return cJSON_ParseWithOpts(value, 0, 0);
}

static unsigned parse_hex4(const char* str) {
    unsigned h = 0;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    return h;
}

static const char* parse_string(struct cJSON* item, const char* str) {
    const char* ptr = str + 1;
    char* ptr2;
    char* out;
    int len = 0;
    unsigned uc, uc2;
    if (*str != '\"') {
        ep = str;
        return 0;
    }

    while (*ptr != '\"' && *ptr && ++len)
        if (*ptr++ == '\\') ptr++;

    out = (char*)malloc(len + 1);
    if (!out) return 0;

    ptr = str + 1;
    ptr2 = out;
    while (*ptr != '\"' && *ptr) {
        if (*ptr != '\\')
            *ptr2++ = *ptr++;
        else {
            ptr++;
            switch (*ptr) {
                case 'b':
                    *ptr2++ = '\b';
                    break;
                case 'f':
                    *ptr2++ = '\f';
                    break;
                case 'n':
                    *ptr2++ = '\n';
                    break;
                case 'r':
                    *ptr2++ = '\r';
                    break;
                case 't':
                    *ptr2++ = '\t';
                    break;
                case 'u':
                    uc = parse_hex4(ptr + 1);
                    ptr += 4;

                    if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0) break;

                    if (uc >= 0xD800 && uc <= 0xDBFF) {
                        if (ptr[1] != '\\' || ptr[2] != 'u') break;
                        uc2 = parse_hex4(ptr + 3);
                        ptr += 6;
                        if (uc2 < 0xDC00 || uc2 > 0xDFFF) break;
                        uc = 0x10000 + (((uc & 0x3FF) << 10) | (uc2 & 0x3FF));
                    }

                    len = 4;
                    if (uc < 0x80)
                        len = 1;
                    else if (uc < 0x800)
                        len = 2;
                    else if (uc < 0x10000)
                        len = 3;
                    ptr2 += len;

                    switch (len) {
                        case 4:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;
                        case 3:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;
                        case 2:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;
                        case 1:
                            *--ptr2 = (uc | firstByteMark[len]);
                    }
                    ptr2 += len;
                    break;
                default:
                    *ptr2++ = *ptr;
                    break;
            }
            ptr++;
        }
    }
    *ptr2 = 0;
    if (*ptr == '\"') ptr++;
    item->valuestring = out;
    item->type = cJSON_String;
    return ptr;
}

static const char* parse_number(struct cJSON* item, const char* num) {
    double n = 0, sign = 1, scale = 0;
    int subscale = 0, signsubscale = 1;

    if (*num == '-') sign = -1, num++;
    if (*num == '0') num++;
    if (*num >= '1' && *num <= '9') do
            n = (n * 10.0) + (*num++ - '0');
        while (*num >= '0' && *num <= '9');
    if (*num == '.' || (*num == ',' && num[1] >= '0' && num[1] <= '9')) {
        num++;
        do n = (n * 10.0) + (*num++ - '0'), scale--;
        while (*num >= '0' && *num <= '9');
    }
    if (*num == 'e' || *num == 'E') {
        num++;
        if (*num == '+')
            num++;
        else if (*num == '-')
            signsubscale = -1, num++;
        while (*num >= '0' && *num <= '9') subscale = (subscale * 10) + (*num++ - '0');
    }

    n = sign * n * pow(10.0, (scale + subscale * signsubscale));

    item->valuedouble = n;
    item->valueint = (int)n;
    item->type = cJSON_Number;
    return num;
}

static const char* parse_array(struct cJSON* item, const char* value) {
    struct cJSON* child;
    if (*value != '[') {
        ep = value;
        return 0;
    }

    item->type = cJSON_Array;
    value = skip(value + 1);
    if (*value == ']') return value + 1;

    item->child = child = cJSON_New_Item();
    if (!item->child) return 0;
    value = skip(parse_value(child, skip(value)));
    if (!value) return 0;

    while (*value == ',') {
        struct cJSON* new_item;
        if (!(new_item = cJSON_New_Item())) return 0;
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip(parse_value(child, skip(value + 1)));
        if (!value) return 0;
    }

    if (*value == ']') return value + 1;
    ep = value;
    return 0;
}

static const char* parse_object(struct cJSON* item, const char* value) {
    struct cJSON* child;
    if (*value != '{') {
        ep = value;
        return 0;
    }

    item->type = cJSON_Object;
    value = skip(value + 1);
    if (*value == '}') return value + 1;

    item->child = child = cJSON_New_Item();
    if (!item->child) return 0;
    value = skip(parse_string(child, skip(value)));
    if (!value) return 0;
    child->string = child->valuestring;
    child->valuestring = 0;
    if (*value != ':') {
        ep = value;
        return 0;
    }
    value = skip(parse_value(child, skip(value + 1)));
    if (!value) return 0;

    while (*value == ',') {
        struct cJSON* new_item;
        if (!(new_item = cJSON_New_Item())) return 0;
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip(parse_string(child, skip(value + 1)));
        if (!value) return 0;
        child->string = child->valuestring;
        child->valuestring = 0;
        if (*value != ':') {
            ep = value;
            return 0;
        }
        value = skip(parse_value(child, skip(value + 1)));
        if (!value) return 0;
    }

    if (*value == '}') return value + 1;
    ep = value;
    return 0;
}

static const char* parse_value(struct cJSON* item, const char* value) {
    if (!value) return 0;

    if (!strncmp(value, "null", 4)) {
        item->type = cJSON_NULL;
        return value + 4;
    }
    if (!strncmp(value, "false", 5)) {
        item->type = cJSON_False;
        return value + 5;
    }
    if (!strncmp(value, "true", 4)) {
        item->type = cJSON_True;
        item->valueint = 1;
        return value + 4;
    }
    if (*value == '\"') {
        return parse_string(item, value);
    }
    if (*value == '-' || (*value >= '0' && *value <= '9')) {
        return parse_number(item, value);
    }
    if (*value == '[') {
        return parse_array(item, value);
    }
    if (*value == '{') {
        return parse_object(item, value);
    }

    ep = value;
    return 0;
}

static char* print_number(struct cJSON* item) {
    char* locale = setlocale(LC_ALL, NULL);
    setlocale(LC_NUMERIC, "C");

    char* str;
    double d = item->valuedouble;
    if (fabs(((double)item->valueint) - d) <= DBL_EPSILON && d <= INT_MAX && d >= INT_MIN) {
        str = (char*)malloc(21);
        if (str) sprintf(str, "%d", item->valueint);
    } else {
        str = (char*)malloc(64);
        if (str) {
            if (fabs(floor(d) - d) <= DBL_EPSILON && fabs(d) < 1.0e60)
                sprintf(str, "%.0f", d);
            else if (fabs(d) < 1.0e-6 || fabs(d) > 1.0e9)
                sprintf(str, "%e", d);
            else
                sprintf(str, "%f", d);
        }
    }

    setlocale(LC_NUMERIC, locale);
    return str;
}

static char* print_string_ptr(const char* str) {
    const char* ptr;
    char *ptr2, *out;
    int len = 0;
    unsigned char token;

    if (!str) return strdup("");
    ptr = str;
    while ((token = *ptr) && ++len) {
        if (strchr("\"\\\b\f\n\r\t", token))
            len++;
        else if (token < 32)
            len += 5;
        ptr++;
    }

    out = (char*)malloc(len + 3);
    if (!out) return 0;

    ptr2 = out;
    ptr = str;
    *ptr2++ = '\"';
    while (*ptr) {
        if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\')
            *ptr2++ = *ptr++;
        else {
            *ptr2++ = '\\';
            switch (token = *ptr++) {
                case '\\':
                    *ptr2++ = '\\';
                    break;
                case '\"':
                    *ptr2++ = '\"';
                    break;
                case '\b':
                    *ptr2++ = 'b';
                    break;
                case '\f':
                    *ptr2++ = 'f';
                    break;
                case '\n':
                    *ptr2++ = 'n';
                    break;
                case '\r':
                    *ptr2++ = 'r';
                    break;
                case '\t':
                    *ptr2++ = 't';
                    break;
                default:
                    sprintf(ptr2, "u%04x", token);
                    ptr2 += 5;
                    break;
            }
        }
    }
    *ptr2++ = '\"';
    *ptr2++ = 0;
    return out;
}

static char* print_string(struct cJSON* item) {
    return print_string_ptr(item->valuestring);
}

static char* print_array(struct cJSON* item, int depth, int fmt) {
    char** entries;
    char *out = 0, *ptr, *ret;
    int len = 5;
    struct cJSON* child = item->child;
    int numentries = 0, i = 0, fail = 0;

    while (child) numentries++, child = child->next;
    if (!numentries) {
        out = (char*)malloc(3);
        if (out) strcpy(out, "[]");
        return out;
    }
    entries = (char**)malloc(numentries * sizeof(char*));
    if (!entries) return 0;
    memset(entries, 0, numentries * sizeof(char*));
    child = item->child;
    while (child && !fail) {
        ret = print_value(child, depth + 1, fmt);
        entries[i++] = ret;
        if (ret)
            len += strlen(ret) + 2 + (fmt ? 1 : 0);
        else
            fail = 1;
        child = child->next;
    }

    if (!fail) out = (char*)malloc(len);
    if (!out) fail = 1;

    if (fail) {
        for (i = 0; i < numentries; i++)
            if (entries[i]) free(entries[i]);
        free(entries);
        return 0;
    }

    *out = '[';
    ptr = out + 1;
    *ptr = 0;
    for (i = 0; i < numentries; i++) {
        strcpy(ptr, entries[i]);
        ptr += strlen(entries[i]);
        if (i != numentries - 1) {
            *ptr++ = ',';
            if (fmt) *ptr++ = ' ';
            *ptr = 0;
        }
        free(entries[i]);
    }
    free(entries);
    *ptr++ = ']';
    *ptr++ = 0;
    return out;
}

static char* print_object(struct cJSON* item, int depth, int fmt) {
    char **entries = 0, **names = 0;
    char *out = 0, *ptr, *ret, *str;
    int len = 7, i = 0, j;
    struct cJSON* child = item->child;
    int numentries = 0, fail = 0;
    while (child) numentries++, child = child->next;
    if (!numentries) {
        out = (char*)malloc(fmt ? depth + 4 : 3);
        if (!out) return 0;
        ptr = out;
        *ptr++ = '{';
        if (fmt) {
            *ptr++ = '\n';
            for (i = 0; i < depth - 1; i++) *ptr++ = '\t';
        }
        *ptr++ = '}';
        *ptr++ = 0;
        return out;
    }
    entries = (char**)malloc(numentries * sizeof(char*));
    if (!entries) return 0;
    names = (char**)malloc(numentries * sizeof(char*));
    if (!names) {
        free(entries);
        return 0;
    }
    memset(entries, 0, sizeof(char*) * numentries);
    memset(names, 0, sizeof(char*) * numentries);

    child = item->child;
    depth++;
    if (fmt) len += depth;
    while (child) {
        names[i] = str = print_string_ptr(child->string);
        entries[i++] = ret = print_value(child, depth, fmt);
        if (str && ret)
            len += strlen(ret) + strlen(str) + 2 + (fmt ? 2 + depth : 0);
        else
            fail = 1;
        child = child->next;
    }

    if (!fail) out = (char*)malloc(len);
    if (!out) fail = 1;

    if (fail) {
        for (i = 0; i < numentries; i++) {
            if (names[i]) free(names[i]);
            if (entries[i]) free(entries[i]);
        }
        free(names);
        free(entries);
        return 0;
    }

    *out = '{';
    ptr = out + 1;
    if (fmt) *ptr++ = '\n';
    *ptr = 0;
    for (i = 0; i < numentries; i++) {
        if (fmt)
            for (j = 0; j < depth; j++) *ptr++ = '\t';
        strcpy(ptr, names[i]);
        ptr += strlen(names[i]);
        *ptr++ = ':';
        if (fmt) *ptr++ = '\t';
        strcpy(ptr, entries[i]);
        ptr += strlen(entries[i]);
        if (i != numentries - 1) *ptr++ = ',';
        if (fmt) *ptr++ = '\n';
        *ptr = 0;
        free(names[i]);
        free(entries[i]);
    }

    free(names);
    free(entries);
    if (fmt)
        for (i = 0; i < depth - 1; i++) *ptr++ = '\t';
    *ptr++ = '}';
    *ptr++ = 0;
    return out;
}

static char* print_value(struct cJSON* item, int depth, int fmt) {
    char* out = 0;
    if (!item) return 0;

    switch ((item->type) & 255) {
        case cJSON_NULL:
            out = strdup("null");
            break;
        case cJSON_False:
            out = strdup("false");
            break;
        case cJSON_True:
            out = strdup("true");
            break;
        case cJSON_Number:
            out = print_number(item);
            break;
        case cJSON_String:
            out = print_string(item);
            break;
        case cJSON_Array:
            out = print_array(item, depth, fmt);
            break;
        case cJSON_Object:
            out = print_object(item, depth, fmt);
            break;
    }
    return out;
}

char* cJSON_Print(struct cJSON* item) {
    return print_value(item, 0, 1);
}
