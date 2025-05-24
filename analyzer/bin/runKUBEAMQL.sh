#!/bin/bash

PER_NUM=$1
PER_DIR=`printf "per%04d\n" "${PER_NUM}"`

DATE=`pwd | awk -F "/" '{ print $NF}'`
DATASET="${KUBEAM_DATA_DIR}/${DATE}/${PER_DIR}"

echo "Target Dataset: ${DATASET}" 
echo "Check the existence..."

if [ -e ${DATASET} ]; then
    echo "Dataset exist! "
else
    echo "No Dataset exists. Aborting..."
    exit
fi

if [ -e ${PER_DIR} ]; then
    echo "${PER_DIR} already exists."
else
    echo "Create ${PER_DIR} diretory..."
    mkdir ${PER_DIR}
fi

# Check config JSON file
\ls ${DATASET}/*.json > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: no config JSON files exist in dataset directory. Aborting..."
    exit
else
    ln -s ${DATASET}/*.json ./${PER_DIR}/
    echo "Symbolic link for config JSON file created."    
fi

# Create input filelist
\ls ${DATASET}/*.dat > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: no dat files exist in dataset directory. Aborting..."
    exit
else
    \ls ${DATASET}/*.dat > ./${PER_DIR}/inputFileList.txt
    echo "Input filelist created in ${PER_DIR}"
fi

# Symbolic link ShUtil for the analysis
if [ ! -d "ShUtil" ]; then
    echo "copy ShUtil from analysis package"
    ln -s ${ADALM_ANA_DIR}/root_macro/ShUtil ./
    ln -s ${ADALM_ANA_DIR}/root_macro/rootlogon.C ./
fi

INPUT_FILE_LIST="./${PER_DIR}/inputFileList.txt"
OUTPUT_DIR="./${PER_DIR}"

COMMAND=${ADALM_ANA_DIR}/root_macro/DrawWaveformMulti.C
root -l -q -b "${COMMAND}(\"${INPUT_FILE_LIST}\",\"${OUTPUT_DIR}\")"
