#include "scpi.h"

scpi_result_t scpi_pulse_frequency(scpi_t * context) {
    double freq_hz;

    if (!SCPI_ParamDouble(context, &freq_hz, TRUE)) {
        return SCPI_RES_ERR;
    }

    if (!pulse_set_frequency(freq_hz)) {
        return SCPI_RES_ERR;
    }

    return SCPI_RES_OK;
}

scpi_result_t scpi_pulse_width(scpi_t * context) {
    double width_s;

    if (!SCPI_ParamDouble(context, &width_s, TRUE)) {
        return SCPI_RES_ERR;
    }

    if (!pulse_set_width(width_s)) {
        return SCPI_RES_ERR;
    }

    return SCPI_RES_OK;
}

scpi_result_t scpi_pulse_delay(scpi_t * context) {
    return SCPI_RES_OK;
}

scpi_result_t scpi_pulse_count(scpi_t * context) {
    return SCPI_RES_OK;
}

scpi_result_t scpi_pulse_mode(scpi_t * context) {
    return SCPI_RES_OK;
}

scpi_result_t scpi_output(scpi_t * context) {
    scpi_bool_t enable;

    if (!SCPI_ParamBool(context, &enable, TRUE)) {
        return SCPI_RES_ERR;
    }

    pulse_set_output(enable);

    return SCPI_RES_OK;
}

scpi_result_t scpi_trigger_source(scpi_t * context) {
    return SCPI_RES_OK;
}

scpi_result_t scpi_trigger(scpi_t * context) {
    return SCPI_RES_OK;
}
