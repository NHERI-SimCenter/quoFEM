import os
import subprocess
import json
import shutil
import logging
import sys


class StreamToLogger(object):
    """Class that redirects stream to a logger."""

    def __init__(self, logger, level=logging.INFO):
        self.logger = logger
        self.level = level

    def write(self, message):
        if message.strip():  # Only log non-empty messages
            self.logger.log(self.level, message)

    def flush(self):
        pass  # No operation needed for flush


def setup_logging(log_file="run_examples_output.txt"):
    """Sets up logging to both console and a log file."""
    
    # Clear any existing handlers
    for handler in logging.getLogger().handlers[:]:
        logging.getLogger().removeHandler(handler)
    
    # Set up the root logger with a default level and format
    logging.basicConfig(
        level=logging.DEBUG,  # Set the level to DEBUG to capture all messages
        format="%(asctime)s - %(levelname)s - %(message)s",
        datefmt="%H:%M:%S",
    )

    # Create a file handler for logging to a file
    file_handler = logging.FileHandler(log_file, mode="w")
    file_handler.setLevel(logging.DEBUG)  # Ensure file handler captures all messages
    file_handler.setFormatter(
        logging.Formatter(
            "%(asctime)s - %(levelname)s - %(message)s", datefmt="%H:%M:%S"
        )
    )

    # Create a console handler for logging to stdout
    console_handler = logging.StreamHandler()
    console_handler.setLevel(logging.DEBUG)  # Set console handler to DEBUG as well
    console_handler.setFormatter(
        logging.Formatter(
            "%(asctime)s - %(levelname)s - %(message)s", datefmt="%H:%M:%S"
        )
    )

    # Add the handlers to the root logger
    logging.getLogger().addHandler(file_handler)
    logging.getLogger().addHandler(console_handler)

    # Redirect stdout and stderr to the logger
    sys.stdout = StreamToLogger(logging.getLogger(), level=logging.INFO)
    sys.stderr = StreamToLogger(logging.getLogger(), level=logging.ERROR)


def copy_files(src_dir, dst_dir):
    """Copy all files and subdirectories from src_dir to dst_dir."""
    if os.path.exists(src_dir) and os.path.isdir(src_dir):
        for filename in os.listdir(src_dir):
            src_file = os.path.join(src_dir, filename)
            dst_file = os.path.join(dst_dir, filename)
            try:
                if os.path.isfile(src_file):
                    shutil.copy2(src_file, dst_file)  # copy2 preserves metadata
                elif os.path.isdir(src_file):
                    shutil.copytree(src_file, dst_file, dirs_exist_ok=True)
            except Exception as e:
                logging.error(f"Error copying {src_file} to {dst_file}: {e}")
    else:
        logging.warning(
            f"Source directory {src_dir} does not exist or is not a directory."
        )


def delete_directory(directory):
    """Deletes the specified directory and all its contents."""
    if os.path.exists(directory) and os.path.isdir(directory):
        shutil.rmtree(directory)
        logging.info(f"Deleted directory: {directory}")
    else:
        logging.warning(
            f"Directory {directory} does not exist or is not a directory."
        )


def create_cache_directory(cache_dir):
    """Create the cache directory, removing it first if it exists."""
    try:
        if os.path.exists(cache_dir):
            shutil.rmtree(cache_dir)
        os.makedirs(cache_dir)
        logging.info(f"Created cache directory: {cache_dir}")
    except Exception as e:
        logging.error(f"Error creating cache directory {cache_dir}: {e}")


def process_example(example, base_dir, tmp_dir, cache_dir):
    """Process a single example from the JSON configuration."""
    name = example.get("name")
    description = example.get("description")
    input_file = os.path.join(base_dir, "quoFEM", "Examples", example.get("inputFile", ""))
    src_dir = os.path.dirname(input_file)
    example_number = os.path.basename(os.path.dirname(src_dir))

    logging.info("="*80)
    logging.info(f"Processing Example Name: {name}") 
    logging.info(f"Example Number: {example_number}")
    logging.info(f"Description: {description}")
    logging.info(f"Input File: {input_file}")
    logging.info(f"Source Directory: {src_dir}")

    if os.path.exists(tmp_dir):
        delete_directory(tmp_dir)
    os.makedirs(os.path.join(tmp_dir, "templatedir"))

    dst_dir = os.path.join(tmp_dir, "templatedir")
    copy_files(src_dir, dst_dir)

    # Special handling for specific examples
    if example_number in ["qfem-0027", "qfem-0028"]:
        handle_special_examples(dst_dir, example_number)

    input_file = os.path.join(dst_dir, os.path.basename(input_file))
    add_json_params(input_file, dst_dir, base_dir)

    # Run the backend application and capture output
    logging.info("Running Python application...")
    try:
        # Capture stdout and stderr in the log
        result = subprocess.run(
            [
                "python3",
                os.path.join(
                    base_dir,
                    "SimCenterBackendApplications/applications/Workflow/qWHALE.py",
                ),
                "runningLocal",
                input_file,
                os.path.join(
                    base_dir,
                    "SimCenterBackendApplications/applications/Workflow/WorkflowApplications.json",
                ),
            ],
            check=True,
            stdout=subprocess.PIPE,  # Capture standard output
            stderr=subprocess.PIPE,  # Capture standard error
        )

        # Log the captured standard output if it exists
        if result.stdout:
            logging.info("Subprocess Output:\n%s", result.stdout.decode())
        
        # Log the captured standard error if it exists
        if result.stderr:
            logging.warning("Subprocess Warnings/Info:\n%s", result.stderr.decode())

    except subprocess.CalledProcessError as e:
        logging.error(
            f"Error running application: {e}\nOutput: {e.output.decode()}\nError Output: {e.stderr.decode()}"
        )

    # Save output to cache
    cache_tmp = os.path.join(cache_dir, f"tmp.SimCenter.{example_number}")
    shutil.copytree(tmp_dir, cache_tmp, dirs_exist_ok=True)
    logging.info(f"Saved output to cache: {cache_tmp}")


def handle_special_examples(dst_dir, example_number):
    """Handle specific file operations for certain example numbers."""
    if example_number == "qfem-0027":
        copy_files(os.path.join(dst_dir, "model1"), dst_dir)
        copy_files(os.path.join(dst_dir, "model2"), dst_dir)
        delete_directory(os.path.join(dst_dir, "model1"))
        delete_directory(os.path.join(dst_dir, "model2"))
    elif example_number == "qfem-0028":
        copy_files(
            os.path.join(dst_dir, "CouponCyclicTestData_every_20th_point"), dst_dir
        )
        copy_files(os.path.join(dst_dir, "model"), dst_dir)
        delete_directory(
            os.path.join(dst_dir, "CouponCyclicTestData_every_20th_point")
        )
        delete_directory(os.path.join(dst_dir, "model"))


def add_json_params(input_file, dst_dir, base_dir):
    """Add necessary parameters to the input JSON file."""
    logging.info("Adding JSON parameters to input file...")
    try:
        with open(input_file, "r") as f:
            input_json = json.load(f)

        input_json["runDir"] = os.path.join(base_dir, "tmp.SimCenter")
        input_json["localAppDir"] = os.path.join(
            base_dir, "SimCenterBackendApplications"
        )
        input_json["remoteAppDir"] = os.path.join(
            base_dir, "SimCenterBackendApplications"
        )
        input_json["runType"] = "runningLocal"

        with open(input_file, "w") as f:
            json.dump(input_json, f, indent=2)

        # Replace "{Current_Dir}" with dst_dir in the input file content
        with open(input_file, "r") as f:
            filedata = f.read()

        filedata = filedata.replace("{Current_Dir}", dst_dir)

        with open(input_file, "w") as f:
            f.write(filedata)
    except Exception as e:
        logging.error(f"Error modifying JSON parameters in {input_file}: {e}")


def main():
    """Main entry point of the script."""
    setup_logging()

    # Constants
    pwd = os.getcwd()
    json_file = os.path.join(pwd, "quoFEM/Examples/Examples.json")
    cache_dir = os.path.join(pwd, "cache")
    tmp_dir = os.path.join(pwd, "tmp.SimCenter")

    logging.info(f"In folder {pwd}")

    # Create cache directory
    create_cache_directory(cache_dir)

    # Read the JSON file
    try:
        with open(json_file, "r") as f:
            examples = json.load(f).get("Examples", [])
    except Exception as e:
        logging.error(f"Error reading JSON file {json_file}: {e}")
        return

    # Process each example
    for example in examples:
        process_example(example, pwd, tmp_dir, cache_dir)


if __name__ == "__main__":
    main()
