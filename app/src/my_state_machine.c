/**
 * @file my_state_machine.c
 * @brief Simple LED toggle state machine using Zephyr SMF
 */

#include "my_state_machine.h"
#include "LED.h"
#include <zephyr/smf.h>

// --- Typedefs and Enums ---

enum led_state_machine_states {
    LED_ON_STATE,
    LED_OFF_STATE
};

typedef struct {
    struct smf_ctx ctx; // Must be first
    uint16_t count;
} led_state_object_t;

// --- Forward Declarations of State Functions ---

static void led_on_state_entry(void* o);
static enum smf_state_result led_on_state_run(void* o);
static void led_off_state_entry(void* o);
static enum smf_state_result led_off_state_run(void* o);

// --- Local Variables ---

static led_state_object_t led_state_object;

static const struct smf_state led_states[] = {
    [LED_ON_STATE]  = SMF_CREATE_STATE(led_on_state_entry, led_on_state_run, NULL, NULL, NULL),
    [LED_OFF_STATE] = SMF_CREATE_STATE(led_off_state_entry, led_off_state_run, NULL, NULL, NULL)
};

// --- State Function Definitions ---

static void led_on_state_entry(void* o) {
    LED_set(LED0, LED_ON);
}

static enum smf_state_result led_on_state_run(void* o) {
    if (led_state_object.count > 500) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[LED_OFF_STATE]);
    } else {
        led_state_object.count++;
    }
    return SMF_EVENT_HANDLED;
}

static void led_off_state_entry(void* o) {
    LED_set(LED0, LED_OFF);
}

static enum smf_state_result led_off_state_run(void* o) {
    if (led_state_object.count > 500) {
        led_state_object.count = 0;
        smf_set_state(SMF_CTX(&led_state_object), &led_states[LED_ON_STATE]);
    } else {
        led_state_object.count++;
    }
    return SMF_EVENT_HANDLED;
}

// --- Public Functions ---

void state_machine_init(void) {
    led_state_object.count = 0;
    smf_set_initial(SMF_CTX(&led_state_object), &led_states[LED_ON_STATE]);
}

int state_machine_run(void) {
    return smf_run_state(SMF_CTX(&led_state_object));
    k_msleep(1);
}
