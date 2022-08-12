#!/bin/bash 

executable="qfem"
build_dir="build-qfem"
app_dir="$(dirname $PWD)/SimCenterBackendApplications/"

build_all() {
  # Install nheri-sincenter python repositories
  yes | python3 -m pip install nheri-simcenter
}

build_macOS() {
  # Script to run SimCenter unit tests
  # Adapted from script by Stevan Gavrilovic

  BASEDIR=$(dirname "$0")

  cd $BASEDIR

  echo "Script file is in directory " $PWD

  mkdir -p "./$build_dir/" && cd "./$build_dir"

  # Run qmake for Tests
  qmake "../quoFEM-Test.pri"

  status=$?
  if [[ $status != 0 ]]
  then
      echo "RunTests: qmake failed";
      exit $status;
  fi

  # make
  make -j8
  status="$?";
  if [[ $status != 0 ]]
  then
      echo "RunTests: make failed";
      exit $status;
  fi

  # # Copy over the applications dir
  # cd ..
  # cp -Rf "../SimCenterBackendApplications/applications" "$build_dir/"

  # status=$?;
  # if [[ $status != 0 ]]
  # then
  #     echo "Error copying backend applications";
  #     exit $status;
  # fi

  # Download dakota and opensees, extract them, and install them to the build/applications folder
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
    build_macOS
    build_all
    ;;
  *)
esac

# Run the test app
for example in Examples/*000[12]/; do
  echo "Running example $example"
  ./$build_dir/$executable --config appDir="$app_dir" "$example/src/input.json"
done

status=$?
if [[ $status != 0 ]]
then
    echo "RunTests: unit tests failed";
    exit $status;
fi

echo "All unit tests passed!"

