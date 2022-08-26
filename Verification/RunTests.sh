#!/bin/bash 

# usage: ./RunTests.sh [OS]
# Options
#  OS        optional; if provided, either 'macos' or 'linux'

# set -e

executable="quoFEM"
build_dir="build-cli"
app_dir="$(dirname $PWD)/SimCenterBackendApplications/"

run_build() {
  # Install nheri-sincenter python repositories
  yes | python3 -m pip install nheri-simcenter

  cd $(dirname "$0")

  echo "Script file is in directory " $PWD

  mkdir -p "./$build_dir/" && cd "./$build_dir"

  # Run qmake for Tests
  qmake "../quoFEM-Test.pro"

  status=$?
  if [[ $status != 0 ]]
  then
      echo "RunTests: qmake failed";
      exit $status;
  fi

  make -j8
  status="$?";
  if [[ $status != 0 ]]
  then
      echo "RunTests: make failed";
      exit $status;
  fi

  cd -
}

install_macOS() {
  # Script to install SimCenter unit tests on MacOS
  # Adapted from script by Stevan Gavrilovic
  cd $(dirname "$0")

  # Download Dakota and OpenSees, extract them, and install them to the build/applications folder
  mkdir -p dakota && cd dakota
  curl -O  https://dakota.sandia.gov/sites/default/files/distributions/public/dakota-6.15.0-public-darwin.Darwin.x86_64-cli.tar.gz
  tar -xf *.tar.gz
  cd ..

  mkdir  "./$build_dir/applications/dakota"

  status=$?;
  if [[ $status != 0 ]]
  then
      echo "Error making the dakota dir";
      exit $status;
  fi

  cp -rf "./dakota/dakota-*/*" "./$build_dir/applications/dakota"

  status=$?;
  if [[ $status != 0 ]]
  then
      echo "Error copying dakota to $build_dir/applications/dakota dir";
      exit $status;
  fi

  mkdir opensees
  cd opensees
  curl -O  https://opensees.berkeley.edu/OpenSees/code/OpenSees3.3.0Mac.tar.gz
  tar -xf *.tar.gz

  cd ..

  mkdir  "./$build_dir/applications/opensees"
  status=$?;
  if [[ $status != 0 ]]
  then
      echo "Error making the $build_dir/applications/opensees dir";
      exit $status;
  fi

  cp -rf "./opensees/OpenSees*/*" "./$build_dir/applications/opensees"
  status=$?;
  if [[ $status != 0 ]]
  then
      echo "Error copying opensees to applications dir";
      exit $status;
  fi

  # Disable gatekeeper because dakota is unsigned
  sudo spctl --master-disable
}

case $1 in
  macos)
    run_build
    install_macOS
    ;;
  linux)
    run_build
    ;;
  *)
esac

outDir="$(pwd)/_verification/"
archName="$(uname -s | awk '{print tolower($0)}')-$(uname -m)"

# Run the test app
for example in Examples/*-001[0-9]*; do

  # Run examples that we have exected outputs for.
  if [ -d "$example/verification" ] && [ "$(find "$example/verification/" -name *$archName* | wc -l)" -gt 0 ]
  then
    echo ">>> Running example '$(basename $example)'"

    # Name and create a local working directory.
    localWorkDir="$outDir/$(basename $example)/"
    mkdir -p $localWorkDir


    # Execute the application with all output streams silenced.
    ./$build_dir/$executable \
        --config "localWorkDir=$localWorkDir" \
        --config appDir="$app_dir" \
        "$example/src/input.json" > /dev/null 2>&1


    # Loop over expected output files and print the number of lines that differ.
    {
      for expectation in $example/verification/*$archName*; do
        output="$localWorkDir/tmp.SimCenter/$(basename ${expectation/-$archName/})"
        differing_lines="$(sdiff -B -b -s "$expectation" "$output" | wc -l)"
        printf "$(basename $output) \t $differing_lines\n"
      done

      # pipe into `column` to format table, and finally `sed` to shift output.
    } | column -t | sed 's/.*/\t&/'


    # NOTE: Currently nothing is cleaned up. This is for debugging purposes.
    # rm -rf "$localWorkDir/tmp.SimCenter/*"

  else
    echo ">>> No validation files for '$(basename $example)' on '$archName'; skipping."
  fi

  printf "\n"
done

status=$?
if [[ $status != 0 ]]
then
    echo "RunTests: unit tests failed.";
    exit $status;
fi

echo "All unit tests passed."

