#! /bin/bash

main() {
  echo

  for file in ./*; do
    echo $file
  done

  exit 0
}

main "$@"
