#!/bin/bash
PATH=$PATH:/home/ckemere/eagle-9.2.2/

for i in "$@"; do
  prefix=$(echo $i | sed -e "s/\(.*\).brd/\1/");
  student=$(echo $i | sed -e "s/\.\/\(.*\).brd/\1/");

  echo $student;

  eagle -X -dCAMJOB -j/mnt/d/Development/Design/ELEC327/PCBs/AdvancedCircuitsBarebones.cam -o. "$prefix.brd"
  # eagle -X -d GERBER_RS274X -o "$prefix.GTL" "$prefix.brd" Top Pads Vias Dimension > /dev/null
  # eagle -X -d GERBER_RS274X -o "$prefix.GBL" "$prefix.brd" Bottom Pads Vias > /dev/null
  # eagle -X -d GERBER_RS274X -o "$prefix.GML" "$prefix.brd" Dimension Milling > /dev/null
  # eagle -X -d EXCELLON_24 -o "$prefix.TXT" "$prefix.brd" Drills Holes > /dev/null
done
