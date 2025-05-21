DIR=`pwd`
echo "adalm daq package: ${DIR} is set"
export ADALM_DAQ_DIR=${DIR}
export PATH=${ADALM_DAQ_DIR}/bin:${PATH}
