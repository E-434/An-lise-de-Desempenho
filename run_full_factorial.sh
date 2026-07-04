#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

GERADOR_EXE="$SCRIPT_DIR/gerador"
SORTING_EXE="$SCRIPT_DIR/sorting"
DATASETS_DIR="$SCRIPT_DIR/datasets"
RESULTS_DIR="$SCRIPT_DIR/results"

mkdir -p "$DATASETS_DIR"
mkdir -p "$RESULTS_DIR"

echo "Compilando gerador..."
g++ gerador.cpp -std=c++17 -O2 -o "$GERADOR_EXE"

echo "Compilando sorting..."
g++ sorting.cpp -std=c++17 -O2 -pthread -o "$SORTING_EXE"

ALGORITHMS=(heap merge quick)
STRESSES=(none cpu ram both)
SIZES=(8)
REPS=(1 2 3 4 5)

for rep in "${REPS[@]}"; do
  echo "=== Repetição $rep / ${#REPS[@]} ==="

  for size in "${SIZES[@]}"; do
    echo "Gerando datasets para tamanho 10^$size..."
    "$GERADOR_EXE" "$size" 10

    mapfile -t dataset_files < <(find "$DATASETS_DIR" -maxdepth 1 -type f -name '*.bin' | sort)

    for dataset in "${dataset_files[@]}"; do
      dataset_name="$(basename "$dataset")"
      for algorithm in "${ALGORITHMS[@]}"; do
        for stress in "${STRESSES[@]}"; do
          echo "Rep $rep | Size 10^$size | Dataset $dataset_name | Alg $algorithm | Stress $stress"
          "$SORTING_EXE" "$algorithm" "$dataset" "$stress"
        done
      done
    done
  done

done

echo "Full factorial concluído. Resultados em: $RESULTS_DIR"
