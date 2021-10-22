#!/bin/bash

dia_path="$(dirname $0)"
resource_path="$dia_path/../resources"
image_names=(
    "libvdma5050"
    "libvdma5050_linking"
    "libvdma5050_process_sequence"
)

[ -z $DIA] && DIA=dia

which $DIA || (echo "Dia is missing..." ; exit 1) || exit 1

mkdir -p "$resource_path"

for name in ${image_names[@]}; do
    $DIA --export="$resource_path/$name.png" "$dia_path/$name.dia"
done
