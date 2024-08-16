#define LOG_TAG "libimonitor"

#define MAX_PATH_NUMBER (10)
#define MAX_PATH_LEN (256)
#define MAX_STR_LEN (10 * 1024)

#include <log/log.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct imonitor_param {
    int key;
    char* value;
    struct imonitor_param* next;
};

struct imonitor_eventobj {
    unsigned int eventid;
    const unsigned short* desc;
    unsigned int params_count;
    struct imonitor_param* head;
    long long time;
    char* dynamic_path[MAX_PATH_NUMBER];
    char* dynamic_path_delete[MAX_PATH_NUMBER];
};

struct imonitor_eventobj* imonitor_create_eventobj(unsigned int eventId) {
    struct imonitor_eventobj* event =
            (struct imonitor_eventobj*)malloc(sizeof(struct imonitor_eventobj));
    if (!event) return NULL;

    event->eventid = eventId;
    event->desc = NULL;
    event->params_count = 0;
    event->head = NULL;
    event->time = 0;

    for (int i = 0; i < MAX_PATH_NUMBER; i++) {
        event->dynamic_path[i] = NULL;
        event->dynamic_path_delete[i] = NULL;
    }

    ALOGV("%s: eventId: %d", __func__, eventId);

    return event;
}

void imonitor_destroy_eventobj(struct imonitor_eventobj* eventobj) {
    ALOGV("%s: eventobj: %p", __func__, eventobj);

    if (eventobj) {
        struct imonitor_param* current_param = eventobj->head;
        struct imonitor_param* temp_param;
        while (current_param != NULL) {
            temp_param = current_param;
            current_param = current_param->next;
            free(temp_param->value);
            free(temp_param);
        }
        for (int i = 0; i < MAX_PATH_NUMBER; i++) {
            if (eventobj->dynamic_path[i] != NULL) {
                free(eventobj->dynamic_path[i]);
            }
            if (eventobj->dynamic_path_delete[i] != NULL) {
                free(eventobj->dynamic_path_delete[i]);
            }
        }
        free(eventobj);
    }
}

int imonitor_set_param(struct imonitor_eventobj* eventobj, unsigned short paramId, long value) {
    ALOGV("%s: eventobj: %p, paramId: %d, value: %ld", __func__, eventobj, paramId, value);
    return 0;
}

int imonitor_set_param_class(struct imonitor_eventobj* eventobj, const char* paramId, int* value) {
    ALOGV("%s: eventobj: %p, paramId: %s, value: %p", __func__, eventobj, paramId, value);
    return 0;
}

int imonitor_set_param_class_v2(struct imonitor_eventobj* eventobj, const char* param, int* value) {
    ALOGV("%s: eventobj: %p, param: %s, value: %p", __func__, eventobj, param, value);
    return 0;
}

int imonitor_set_param_string(struct imonitor_eventobj* eventobj, unsigned short paramId,
                              const char* value) {
    ALOGV("%s: eventobj: %p, paramId: %d, value: %s", __func__, eventobj, paramId, value);
    return 0;
}

int imonitor_set_param_string_v2(struct imonitor_eventobj* eventobj, const char* param,
                                 const char* value) {
    ALOGV("%s: eventobj: %p, param: %s, value: %s", __func__, eventobj, param, value);
    return 0;
}

int imonitor_set_param_integer(struct imonitor_eventobj* eventobj, unsigned short paramId,
                               long value) {
    ALOGV("%s: eventobj: %p, paramId: %d, value: %ld", __func__, eventobj, paramId, value);
    return 0;
}

int imonitor_set_param_integer_v2(struct imonitor_eventobj* eventobj, const char* param,
                                  long value) {
    ALOGV("%s: eventobj: %p, param: %s, value: %ld", __func__, eventobj, param, value);
    return 0;
}

int imonitor_unset_param(struct imonitor_eventobj* eventobj, unsigned short paramId) {
    ALOGV("%s: eventobj: %p, paramId: %d", __func__, eventobj, paramId);
    return 0;
}

int imonitor_unset_param_v2(struct imonitor_eventobj* eventobj, const char* param) {
    ALOGV("%s: eventobj: %p, param: %s", __func__, eventobj, param);
    return 0;
}

int imonitor_add_dynamic_path(struct imonitor_eventobj* eventobj, const char* path) {
    ALOGV("%s: eventobj: %p, path: %s", __func__, eventobj, path);
    return 0;
}

int imonitor_add_and_del_dynamic_path(struct imonitor_eventobj* eventobj, const char* path) {
    ALOGV("%s: eventobj: %p, path: %s", __func__, eventobj, path);
    return 0;
}

int imonitor_set_time(struct imonitor_eventobj* eventobj, long long seconds) {
    ALOGV("%s: eventobj: %p, seconds: %lld", __func__, eventobj, seconds);
    return 0;
}

int imonitor_send_event(struct imonitor_eventobj* eventobj) {
    ALOGV("%s: eventobj: %p", __func__, eventobj);
    return 0;
}
