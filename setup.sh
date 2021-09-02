echo "Setup path to ADALM2000 tools"
echo ""

export ADALM=`pwd -P`
export ADALM_DAQ="${ADALM}/adalm_daq/bin"
export ADALM_ROOT_MACRO="${ADALM}/analyzer/root_macro"

PATH=${PATH}:${ADALM_DAQ}
PATH=${PATH}:${ADALM_ROOT_MACRO}

echo "Add Path for ADALM DAQ tools: ${ADALM_DAQ}"
echo "Add Path for ADALM ROOT macros: ${ADALM_ROOT_MACRO}"

