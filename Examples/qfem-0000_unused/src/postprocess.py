from pathlib import Path


def read_data_from_file(file_path: Path, results: list = []):
    # Open the file
    with open(file_path, "r") as file:
        # Read the first two lines
        for line in file.readlines()[:2]:
            # Split the line into values and convert them to floats
            values = line.strip().split()
            results.extend([float(value) for value in values])
    return results


if __name__ == "__main__":
    # List of file paths to read data from
    file_path_list = [
        Path("AIM.json0.max_abs_acceleration.response.11.out"),
        Path("AIM.json0.max_rel_disp.response.11.out"),
    ]
    results = []
    for file_path in file_path_list:
        # Read data from each file and append it to the results list
        results = read_data_from_file(file_path, results=results)

    # Write the results to a file
    with open("results.out", "w") as file:
        file.write(" ".join([str(result) for result in results]))
