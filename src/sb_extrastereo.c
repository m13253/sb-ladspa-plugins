#include <stdlib.h>
#include "ladspa.h"

#define PORT_FACTOR 0
#define PORT_INL    1
#define PORT_INR    2
#define PORT_OUTL   3
#define PORT_OUTR   4

typedef struct {
    LADSPA_Data *port[5];
} extrastereo_t;

LADSPA_Handle instantiate(const LADSPA_Descriptor *Descriptor, unsigned long SampleRate) {
    return malloc(sizeof (extrastereo_t));
}

void connect_port(LADSPA_Handle Instance, unsigned long Port, LADSPA_Data *DataLocation) {
    if(Port < 5)
        ((extrastereo_t *) Instance)->port[Port] = DataLocation;
}

void run(LADSPA_Handle Instance, unsigned long SampleCount) {
    LADSPA_Data factor = *((extrastereo_t *) Instance)->port[PORT_FACTOR];
    LADSPA_Data *inl = ((extrastereo_t *) Instance)->port[PORT_INL];
    LADSPA_Data *inr = ((extrastereo_t *) Instance)->port[PORT_INR];
    LADSPA_Data *outl = ((extrastereo_t *) Instance)->port[PORT_OUTL];
    LADSPA_Data *outr = ((extrastereo_t *) Instance)->port[PORT_OUTR];
    unsigned long i;
    for(i = 0; i < SampleCount; i++, inl++, inr++, outl++, outr++) {
        LADSPA_Data avg = (*inl+*inr)/2;
        *outl = avg+(*inl-avg)*factor;
        *outr = avg+(*inr-avg)*factor;
    }
}

void cleanup(LADSPA_Handle Instance) {
    if(Instance)
        free(Instance);
}

LADSPA_PortDescriptor desc_port_descriptor[] = {
    LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
    LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
    LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
    LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
    LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO
};
const char *desc_port_names[] = {
    "Factor", "Input (Left)", "Input (Right)", "Output (Left)", "Output (Right)"
};
const LADSPA_PortRangeHint desc_port_range_hints[] = {
    {.HintDescriptor = LADSPA_HINT_DEFAULT_MIDDLE, .LowerBound = 0, .UpperBound = 5},
    {.HintDescriptor = 0},
    {.HintDescriptor = 0},
    {.HintDescriptor = 0},
    {.HintDescriptor = 0}
};
const LADSPA_Descriptor descriptor = {
    .UniqueID = 132531,
    .Label = "sb_extrastereo",
    .Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE,
    .Name = "SB Extrastereo",
    .Maker = "StarBrilliant",
    .Copyright = "LGPL 3",
    .PortCount = 5,
    .PortDescriptors = desc_port_descriptor,
    .PortNames = desc_port_names,
    .PortRangeHints = desc_port_range_hints,
    .instantiate = instantiate,
    .connect_port = connect_port,
    .activate = NULL,
    .run = run,
    .run_adding = NULL,
    .set_run_adding_gain = NULL,
    .deactivate = NULL,
    .cleanup = cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long Index) {
    return Index == 0 ? &descriptor : NULL;
}
