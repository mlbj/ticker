#include "scpi/types.h"
#include "scpi/ieee488.h"

scpi_result_t scpi_pulse_frequency(scpi_t * context);
scpi_result_t scpi_pulse_width(scpi_t * context);
scpi_result_t scpi_pulse_delay(scpi_t * context);
scpi_result_t scpi_pulse_count(scpi_t * context);
scpi_result_t scpi_pulse_mode(scpi_t * context);
scpi_result_t scpi_output(scpi_t * context);
scpi_result_t scpi_trigger_source(scpi_t * context);
scpi_result_t scpi_trigger(scpi_t * context);

static const scpi_command_t scpi_commands[] = {
    {
        .pattern = "*IDN?",
        .callback = SCPI_CoreIdnQ,
    },

    {
        .pattern = "*RST",
        .callback = SCPI_CoreRst,
    },

    {
        .pattern = "PULSe:FREQuency",
        .callback = scpi_pulse_frequency,
    },

    {
        .pattern = "PULSe:WIDth",
        .callback = scpi_pulse_width,
    },

    {
        .pattern = "PULSe:DELay",
        .callback = scpi_pulse_delay,
    },

    {
        .pattern = "PULSe:COUNt",
        .callback = scpi_pulse_count,
    },

    {
        .pattern = "PULSe:MODe",
        .callback = scpi_pulse_mode,
    },

    {
        .pattern = "OUTPut",
        .callback = scpi_output,
    },

    {
        .pattern = "TRIGger:SOURce",
        .callback = scpi_trigger_source,
    },

    {
        .pattern = "TRIGger",
        .callback = scpi_trigger,
    },

    SCPI_CMD_LIST_END
};

