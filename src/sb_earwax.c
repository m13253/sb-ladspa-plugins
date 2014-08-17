#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ladspa.h"

#define BINAURAL_DISTANCE 0.18
#define SOUND_SPEED       340
#define conv_earwax(Instance) ((earwax_t *) Instance)

#define PORT_DISTANCE 0
#define PORT_ANGLE    1
#define PORT_DRYWET   2
#define PORT_INL      3
#define PORT_INR      4
#define PORT_OUTL     5
#define PORT_OUTR     6
#define BUFFER_L      0
#define BUFFER_R      1

typedef struct {
    LADSPA_Data *port[7];
    LADSPA_Data *buffer[2];
    unsigned long sample_rate;
    unsigned long delay;
    double near;
    double far;
    double drygain;
    double wetgain;
    int activated;
} earwax_t;

void activate(LADSPA_Handle Instance);

LADSPA_Handle instantiate(const LADSPA_Descriptor *Descriptor, unsigned long SampleRate) {
    earwax_t *Instance = malloc(sizeof (earwax_t));
    if(Instance) {
        Instance->sample_rate = SampleRate;
        Instance->activated = 0;
        Instance->buffer[BUFFER_L] = NULL;
        Instance->buffer[BUFFER_R] = NULL;
    }
    return Instance;
}

void connect_port(LADSPA_Handle Instance, unsigned long Port, LADSPA_Data *DataLocation) {
    if(Port < 7)
        conv_earwax(Instance)->port[Port] = DataLocation;
    if(conv_earwax(Instance)->activated)
        activate(Instance);
}

void activate(LADSPA_Handle Instance) {
    double distance = *conv_earwax(Instance)->port[PORT_DISTANCE];
    double angle = *conv_earwax(Instance)->port[PORT_ANGLE]*(M_PI/180);
    conv_earwax(Instance)->far = hypot(distance*sin(angle)+BINAURAL_DISTANCE, distance*cos(angle));
    if(distance <= conv_earwax(Instance)->far)
        conv_earwax(Instance)->near = distance;
    else { /* swap ports */
        LADSPA_Data *tmpport = conv_earwax(Instance)->port[PORT_INL];
        conv_earwax(Instance)->port[PORT_INL] = conv_earwax(Instance)->port[PORT_INR];
        conv_earwax(Instance)->port[PORT_INR] = tmpport;
        conv_earwax(Instance)->near = conv_earwax(Instance)->far;
        conv_earwax(Instance)->far = distance;
    }
    conv_earwax(Instance)->delay = round((conv_earwax(Instance)->far-conv_earwax(Instance)->near)*conv_earwax(Instance)->sample_rate/SOUND_SPEED);
    if(conv_earwax(Instance)->far != 0.0) {
        double gain = *conv_earwax(Instance)->port[PORT_DRYWET]*conv_earwax(Instance)->near/conv_earwax(Instance)->far;
        conv_earwax(Instance)->drygain = 1/(gain+1);
        conv_earwax(Instance)->wetgain = 1-conv_earwax(Instance)->drygain;
    } else { /* should not happen */
        conv_earwax(Instance)->drygain = 0;
        conv_earwax(Instance)->wetgain = 1;
    }
    if(conv_earwax(Instance)->buffer[BUFFER_L])
        free(conv_earwax(Instance)->buffer[BUFFER_L]);
    conv_earwax(Instance)->buffer[BUFFER_L] = calloc(conv_earwax(Instance)->delay, sizeof (LADSPA_Data));
    if(conv_earwax(Instance)->buffer[BUFFER_R])
        free(conv_earwax(Instance)->buffer[BUFFER_R]);
    conv_earwax(Instance)->buffer[BUFFER_R] = calloc(conv_earwax(Instance)->delay, sizeof (LADSPA_Data));
    conv_earwax(Instance)->activated = 1;
}

void run(LADSPA_Handle Instance, unsigned long SampleCount) {
    long i;
    for(i = SampleCount; i >= conv_earwax(Instance)->delay; i--) {
        conv_earwax(Instance)->port[PORT_OUTL][i] = conv_earwax(Instance)->port[PORT_INL][i]*conv_earwax(Instance)->drygain+conv_earwax(Instance)->port[PORT_INR][i-conv_earwax(Instance)->delay]*conv_earwax(Instance)->wetgain;
        conv_earwax(Instance)->port[PORT_OUTR][i] = conv_earwax(Instance)->port[PORT_INR][i]*conv_earwax(Instance)->drygain+conv_earwax(Instance)->port[PORT_INL][i-conv_earwax(Instance)->delay]*conv_earwax(Instance)->wetgain;
    }
    for(; i >= 0; i--) {
        conv_earwax(Instance)->port[PORT_OUTL][i] = conv_earwax(Instance)->port[PORT_INL][i]*conv_earwax(Instance)->drygain+conv_earwax(Instance)->buffer[BUFFER_R][i]*conv_earwax(Instance)->wetgain;
        conv_earwax(Instance)->port[PORT_OUTR][i] = conv_earwax(Instance)->port[PORT_INR][i]*conv_earwax(Instance)->drygain+conv_earwax(Instance)->buffer[BUFFER_L][i]*conv_earwax(Instance)->wetgain;
    }
    if(SampleCount >= conv_earwax(Instance)->delay) {
        memcpy(conv_earwax(Instance)->buffer[BUFFER_L], conv_earwax(Instance)->port[PORT_INL]+(SampleCount-conv_earwax(Instance)->delay), conv_earwax(Instance)->delay*sizeof (LADSPA_Data));
        memcpy(conv_earwax(Instance)->buffer[BUFFER_R], conv_earwax(Instance)->port[PORT_INL]+(SampleCount-conv_earwax(Instance)->delay), conv_earwax(Instance)->delay*sizeof (LADSPA_Data));
    } else {
        memmove(conv_earwax(Instance)->buffer[BUFFER_L], conv_earwax(Instance)->buffer[BUFFER_L]+SampleCount, (conv_earwax(Instance)->delay-SampleCount)*sizeof (LADSPA_Data));
        memmove(conv_earwax(Instance)->buffer[BUFFER_R], conv_earwax(Instance)->buffer[BUFFER_R]+SampleCount, (conv_earwax(Instance)->delay-SampleCount)*sizeof (LADSPA_Data));
        memcpy(conv_earwax(Instance)->buffer[BUFFER_L]+(conv_earwax(Instance)->delay-SampleCount), conv_earwax(Instance)->port[PORT_INL], SampleCount*sizeof (LADSPA_Data));
        memcpy(conv_earwax(Instance)->buffer[BUFFER_R]+(conv_earwax(Instance)->delay-SampleCount), conv_earwax(Instance)->port[PORT_INR], SampleCount*sizeof (LADSPA_Data));
    }
}

void deactivate(LADSPA_Handle Instance) {
    conv_earwax(Instance)->activated = 0;
    if(conv_earwax(Instance)->buffer[BUFFER_L]) {
        free(conv_earwax(Instance)->buffer[BUFFER_L]);
        conv_earwax(Instance)->buffer[BUFFER_L] = NULL;
    }
    if(conv_earwax(Instance)->buffer[BUFFER_R]) {
        free(conv_earwax(Instance)->buffer[BUFFER_R]);
        conv_earwax(Instance)->buffer[BUFFER_R] = NULL;
    }
}

void cleanup(LADSPA_Handle Instance) {
    if(conv_earwax(Instance)->activated)
        deactivate(Instance);
    free(Instance);
}

LADSPA_PortDescriptor desc_port_descriptor[] = {
    LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
    LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
    LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
    LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
    LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
    LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
    LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO
};
const char *desc_port_names[] = {
    "Loudspeaker distance in meters", "Loudspeaker angle (0 is straightforward)", "Wet/Dry",
    "Input (Left)", "Input (Right)", "Output (Left)", "Output (Right)"
};
const LADSPA_PortRangeHint desc_port_range_hints[] = {
    {.HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_DEFAULT_LOW, .LowerBound = 0, .UpperBound = 20},
    {.HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_HIGH, .LowerBound = -90, .UpperBound = 90},
    {.HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MAXIMUM, .LowerBound = 0, .UpperBound = 1},
    {.HintDescriptor = 0},
    {.HintDescriptor = 0},
    {.HintDescriptor = 0},
    {.HintDescriptor = 0}
};
const LADSPA_Descriptor descriptor = {
    .UniqueID = 132532,
    .Label = "sb_earwax",
    .Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE,
    .Name = "SB Earwax",
    .Maker = "StarBrilliant",
    .Copyright = "LGPL 3",
    .PortCount = 7,
    .PortDescriptors = desc_port_descriptor,
    .PortNames = desc_port_names,
    .PortRangeHints = desc_port_range_hints,
    .instantiate = instantiate,
    .connect_port = connect_port,
    .activate = activate,
    .run = run,
    .run_adding = NULL,
    .set_run_adding_gain = NULL,
    .deactivate = deactivate,
    .cleanup = cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long Index) {
    return Index == 0 ? &descriptor : NULL;
}
