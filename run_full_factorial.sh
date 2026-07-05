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
SIZES=(7)
REPS=(1 2 3 4 5)
CARGAS=(0 0.5 1.0)

for carga in "${CARGAS[@]}"; do
  echo "### Carga $carga ###"

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
            # Carga 0 -> só roda "none" (sem estresse nenhum).
            # Carga > 0 -> roda só cpu/ram/both, "none" já foi coberto na carga 0.
            if [[ "$carga" == "0" ]]; then
              if [[ "$stress" != "none" ]]; then
                continue
              fi
            else
              if [[ "$stress" == "none" ]]; then
                continue
              fi
            fi

            echo "Carga $carga | Rep $rep | Size 10^$size | Dataset $dataset_name | Alg $algorithm | Stress $stress"

            if [[ "$carga" == "0" ]]; then
              "$SORTING_EXE" "$algorithm" "$dataset" "$stress"
            else
              "$SORTING_EXE" "$algorithm" "$dataset" "$stress" "$carga"
            fi
          done
        done
      done
    done

  done

done

echo "Full factorial concluído. Resultados em: $RESULTS_DIR"