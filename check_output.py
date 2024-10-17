import os
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


def setup_logging(log_file="check_output_log.txt"):
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


def check_output_files(cache_dir):
    """Check for output files in the cache directory and report their status."""
    logging.info("Checking output files...")
    all_passed = True
    for dirpath in os.listdir(cache_dir):
        dir_full_path = os.path.join(cache_dir, dirpath)
        if any(
            os.path.exists(os.path.join(dir_full_path, f))
            for f in [
                "dakotaTab.out",
                "posterior_samples_table.out",
                "dakota_mcmc_tabular.dat",
            ]
        ):
            logging.info(f"{dir_full_path}: PASS")
        else:
            logging.warning(f"{dir_full_path}: FAIL")
            all_passed = False
    return all_passed


def main():
    setup_logging()

    # Constants
    pwd = os.getcwd()
    cache_dir = os.path.join(pwd, "cache")

    # Check for output files
    all_passed = check_output_files(cache_dir)

    if all_passed:
        logging.info("All output files are present.")
        exit(0)
    else:
        logging.error("Some output files are missing.")
        exit(1)


if __name__ == "__main__":
    main()
